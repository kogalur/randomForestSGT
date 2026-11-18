use.tune.hcut <- function(f, hcut = NULL) {
  if (!inherits(f, "tune.hcut")) {
    stop("this function only works on 'tune.hcut' objects")
  }
  if (is.null(hcut)) {
    return(f)
  }
  hcutSeq <- attr(f, "hcutSeq")
  hcut <- max(min(hcut, max(hcutSeq)), min(hcutSeq))
  hcut.idx <- which(hcut == hcutSeq)
  fnew <- attr(f, "all")[[hcut.idx]]
  attributes(fnew) <- attributes(f)
  if (!is.null(attr(f, "formula.all"))) {
    attr(fnew, "formula") <- attr(f, "formula.all")[[hcut.idx]]
  }
  if (!is.null(attr(f, "term.map.all"))) {
    attr(fnew, "term.map") <- attr(f, "term.map.all")[[hcut.idx]]
  }
  attr(fnew, "hcut") <- hcutSeq[hcut.idx]
  fnew
}
tune.hcut <- function(f, data, hcut=3,
                      ntree=5,
                      treesize=10,
                      p.filter=40,
                      hcut.threshold=3,
                      nfolds=10,
                      pure.lasso.cv=TRUE,
                      bsf="inbag",
                      verbose=FALSE,
                      node.center=TRUE) {
  if (!is.logical(node.center) || length(node.center) != 1L || is.na(node.center)) {
    stop("node.center must be TRUE or FALSE")
  }
  if (!is.numeric(hcut) || length(hcut) != 1L || !is.finite(hcut) ||
      hcut < 0L || hcut != as.integer(hcut)) {
    stop("hcut must be one nonnegative integer")
  }
  ## normalize the BSF empirical risk strategy used during CV
  bsf <- get.bsf(bsf, warn = TRUE)
  ## hcut sequence to be evaluated
  hcut.max <- hcut
  hcutSeq <- 0:hcut.max
  ## stump tree for extraction
  o.stump <- get.stump(f, data)
  ## hot-encode the data
  x <- get.hotencode(o.stump$xvar)
  N <- nrow(x)
  if (!is.numeric(nfolds) || length(nfolds) != 1L || !is.finite(nfolds) ||
      nfolds < 2L || nfolds > N || nfolds != as.integer(nfolds)) {
    stop("nfolds must be an integer between 2 and the number of observations")
  }
  ## pull yvar and redefine the data using the hot-encoded X
  yvar.names <- o.stump$yvar.names
  f <- as.formula(paste(yvar.names, "~."))
  data <- data.frame(o.stump$yvar, x)
  colnames(data) <- c(yvar.names, colnames(x))
  ## fast filtering
  if (length(colnames(x)) > p.filter) {
    if (isTRUE(verbose)) {
      cat("preliminary dimension reduction used for large p\n")
    }
    filterS <- c(yvar.names, filter.rfsgt(f, data, hcut=1, treesize=treesize,
                                         node.center=node.center))
  }
  else {
    filterS <- c(yvar.names, colnames(x))
  }
  if (hcut.max >= hcut.threshold) {
    filterX <- unique(c(filterS,
      filter.rfsgt(f, data[, filterS, drop=FALSE], hcut=hcut.threshold,
                   treesize=treesize, original=TRUE,
                   node.center=node.center)))
  }
  ## dimension reduction for each hcut
  dots <- list()
  dots$ntree <- 1
  dots$tune <- FALSE
  dots$bootstrap <- "none"
  dots$node.center <- node.center
  xd.info <- lapply(hcutSeq, function(hcut) {
    if (isTRUE(verbose)) {
      cat("dimension reduction, hcut:", hcut, "\n")
    }
    o <- do.call("rfsgt", c(list(f,
      if (hcut < hcut.threshold) data[, filterS, drop=FALSE] else data[, filterX, drop=FALSE], hcut=hcut), dots))
    if (is.null(o$xvar.augment.names)) {
      xk <- o$xvar
    }
    else {
      xk <- data.frame(o$xvar, o$xvar.augment)
    }
    list(x = xk, term.map = o$term.map)
  })
  xd <- lapply(xd.info, function(z) {z$x})
  term.map.all <- lapply(xd.info, function(z) {z$term.map})
  if (verbose) {
    cat("filtered variables by hcut\n")
    xdn <- lapply(xd, function(x) {colnames(x)})
    names(xdn) <- hcutSeq
    print(xdn)
  }
  ## Draw folds after screening, retaining the existing tuning workflow.
  folds <- cv.folds(N, nfolds)
  dots <- list()
  dots$ntree <- ntree
  dots$treesize <- treesize
  dots$bsf <- bsf
  dots$node.center <- node.center
  dots$tune <- FALSE
  if (ntree == 1) {
    dots$bootstrap <- "none"
  }
  dots$pure.lasso <- pure.lasso.cv
  ## cv-loop
  cv <- data.frame(do.call(rbind, lapply(seq_along(hcutSeq), function(k) {
    if (verbose) cat("cv hcut:", hcutSeq[k], "\n")
    dots.k <- dots
    if (node.center) {
      ## Keep sources and term maps so products are formed within each node.
      ## The hcut=0 candidate retains the existing linear screening proxy.
      hcut.cv <- max(1L, hcutSeq[k])
      filter.k <- colnames(xd[[k]])
      attr(filter.k, "formula") <- filter.baselearner.workhorse(filter.k)
      attr(filter.k, "term.map") <- term.map.all[[k]]
      attr(filter.k, "hcut") <- hcut.cv
      attr(filter.k, "bsf") <- bsf
      attr(filter.k, "node.center") <- node.center
      class(filter.k) <- "tune.hcut"
      dots.k$hcut <- hcut.cv
      dots.k$filter <- filter.k
    }
    else {
      ## The uncentered procedure fits the prepared columns at hcut=1.
      dots.k$hcut <- 1
      dots.k$filter <- FALSE
    }
    cvk <- sapply(folds, function(tst) {
      trn <- setdiff(1:N, tst)
      if (node.center) {
        d.trn <- data[trn, , drop=FALSE]
        x.tst <- x[tst, , drop=FALSE]
      }
      else {
        d.trn <- data.frame(data[trn, yvar.names, drop=FALSE], xd[[k]][trn, , drop=FALSE])
        x.tst <- xd[[k]][tst, , drop=FALSE]
      }
      o <- do.call("rfsgt", c(list(f, d.trn), dots.k))
      if (node.center) {
        prediction <- as.numeric(predict.rfsgt(o, x.tst)$predicted)
        if (length(prediction) != length(tst) || any(!is.finite(prediction))) {
          stop("missing or nonfinite tuning-CV predictions for hcut=", hcutSeq[k])
        }
        mean((prediction - data[tst, yvar.names])^2)
      }
      else {
        mean((predict.rfsgt(o, x.tst)$predicted - data[tst, yvar.names])^2, na.rm=TRUE)
      }
    })
    c(hcutSeq[k], mean(cvk, na.rm=!node.center),
      sd(cvk, na.rm=!node.center) / sqrt(nfolds))
  })))
  colnames(cv) <- c("hcut", "err", "serr")
  if (verbose) print(cv)
  ## identify optimal hcut
  idx <- which.min(cv[, 2])[1]
  if (verbose) cat(paste0("optimal hcut="), hcutSeq[idx], "\n")
  ## return the goodies
  filter <- colnames(xd[[idx]])
  attr(filter, "all") <- lapply(xd, function(x) {colnames(x)})
  attr(filter, "formula") <- filter.baselearner.workhorse(filter)
  attr(filter, "formula.all") <- lapply(xd, function(x) {
    filter.baselearner.workhorse(colnames(x))})
  attr(filter, "term.map") <- term.map.all[[idx]]
  attr(filter, "term.map.all") <- term.map.all
  attr(filter, "cv") <- cv
  if (node.center) {
    attr(filter, "folds") <- folds
  }
  attr(filter, "bsf") <- bsf
  attr(filter, "node.center") <- node.center
  attr(filter, "hcut") <- hcutSeq[idx]
  attr(filter, "hcutSeq") <- hcutSeq
  class(filter) <- "tune.hcut"
  filter
}
##########################################################################################
##
## utilities
##
##########################################################################################
## cv-fold utility
cv.folds <- function (n, folds = 10) {
  split(sample(1:n), rep(1:folds, length = n))
}
## confirm this is a term.map object
is.term.map <- function(x) {
  is.data.frame(x) &&
    all(c("term", "order", "degree", "is.augment",
          "xvar.names", "xvar.power", "xvar.org.names") %in% names(x))
}
## exact original-variable names associated with hotencoded columns
get.xvar.org.names <- function(xvar.names, xmap = NULL) {
  if (is.null(xmap)) {
    return(unique(xvar.names))
  }
  vars <- unique(unlist(lapply(xvar.names, function(nn) {
    names(xmap)[sapply(xmap, function(xx) {nn %in% xx})]
  }), use.names = FALSE))
  if (length(vars) == 0) {
    unique(xvar.names)
  }
  else {
    vars
  }
}
## parse a baselearner name into exact components
get.term.info <- function(term, xmap = NULL,
                          sqchar = "_SQ2_", ichar = "_I_") {
  ## exact-match path: if the term already exists as a column in x, then it is
  ## an atomic feature for this design.  User-supplied columns such as
  ## x1_SQ2_ or x1_I_x2 are not decomposed based on their names alone.
  if (!is.null(xmap)) {
    xvar.org.names <- unique(names(xmap)[sapply(xmap, function(xx) {
      term %in% xx
    })])
    if (length(xvar.org.names) > 0) {
      return(list(term = term,
                  order = 1L,
                  degree = 1L,
                  is.augment = FALSE,
                  xvar.names = term,
                  xvar.power = 1L,
                  xvar.org.names = xvar.org.names))
    }
  }
  xparts <- unlist(strsplit(term, ichar, fixed = TRUE), use.names = FALSE)
  xparts <- xparts[nzchar(xparts)]
  if (length(xparts) == 0) {
    xparts <- term
  }
  xvar.power <- as.integer(sapply(xparts, function(xx) {
    if (grepl(sqchar, xx, fixed = TRUE)) {
      2
    }
    else {
      1
    }
  }))
  xvar.names <- gsub(sqchar, "", xparts, fixed = TRUE)
  xvar.org.names <- get.xvar.org.names(xvar.names, xmap)
  list(term = term,
       order = length(xvar.names),
       degree = sum(xvar.power),
       is.augment = (length(xvar.names) > 1) || any(xvar.power > 1),
       xvar.names = xvar.names,
       xvar.power = xvar.power,
       xvar.org.names = xvar.org.names)
}
## exact baselearner map used for filtered variables and prediction
get.term.map <- function(x, filter = NULL, hcut = 1,
                         sqchar = "_SQ2_", ichar = "_I_") {
  if (is.null(filter)) {
    filter <- colnames(make.baselearner(x, hcut = hcut))
  }
  filter <- as.character(filter)
  hotencode <- attr(x, "hotencode")
  if (!is.null(hotencode$map)) {
    xmap <- hotencode$map
  }
  else {
    xmap <- as.list(colnames(x))
    names(xmap) <- colnames(x)
  }
  if (length(filter) == 0) {
    term.map <- data.frame(term = character(0),
                           order = integer(0),
                           degree = integer(0),
                           is.augment = logical(0),
                           stringsAsFactors = FALSE)
    term.map$xvar.names <- I(list())
    term.map$xvar.power <- I(list())
    term.map$xvar.org.names <- I(list())
    return(term.map)
  }
  term.info <- lapply(filter, function(nn) {
    get.term.info(nn, xmap = xmap, sqchar = sqchar, ichar = ichar)
  })
  term.map <- data.frame(
    term = sapply(term.info, `[[`, "term"),
    order = as.integer(sapply(term.info, `[[`, "order")),
    degree = as.integer(sapply(term.info, `[[`, "degree")),
    is.augment = as.logical(sapply(term.info, `[[`, "is.augment")),
    stringsAsFactors = FALSE
  )
  term.map$xvar.names <- I(lapply(term.info, `[[`, "xvar.names"))
  term.map$xvar.power <- I(lapply(term.info, `[[`, "xvar.power"))
  term.map$xvar.org.names <- I(lapply(term.info, `[[`, "xvar.org.names"))
  ## term labels are not guaranteed to be unique in every downstream path,
  ## so we avoid embedding them as row names.
  rownames(term.map) <- NULL
  term.map
}
## match filter terms to a term.map preserving order and multiplicity
get.term.map.index <- function(term.map, filter = NULL) {
  if (is.null(term.map)) {
    return(integer(0))
  }
  if (is.null(filter)) {
    return(seq_len(nrow(term.map)))
  }
  filter <- as.character(filter)
  term <- as.character(term.map$term)
  used <- rep(FALSE, length(term))
  idx <- integer(0)
  for (nn in seq_along(filter)) {
    ii <- which(!used & term == filter[nn])
    if (length(ii) > 0) {
      idx <- c(idx, ii[1])
      used[ii[1]] <- TRUE
    }
  }
  idx
}
## subset a term.map preserving the order of the filter vector
get.term.map.subset <- function(term.map, filter = NULL) {
  if (is.null(term.map)) {
    return(NULL)
  }
  idx <- get.term.map.index(term.map, filter)
  term.map <- term.map[idx, , drop = FALSE]
  ## filter vectors can legitimately contain duplicates (for example when
  ## selections from multiple hcut values are stacked together).  Preserve
  ## order/multiplicity, but keep ordinary integer row names.
  rownames(term.map) <- NULL
  term.map
}
## pull exact parent variables from a term.map object
get.term.map.parents <- function(term.map,
                                 filter = NULL,
                                 type = c("xvar.names", "xvar.org.names")) {
  type <- match.arg(type)
  term.map <- get.term.map.subset(term.map, filter)
  if (is.null(term.map) || nrow(term.map) == 0) {
    return(character(0))
  }
  parents <- unlist(term.map[[type]], use.names = FALSE)
  parents <- parents[nzchar(parents)]
  unique(parents)
}
## fast construction of filtered baselearner variables using an exact map
make.baselearner.from.term.map <- function(x, term.map) {
  term.map <- get.term.map.subset(term.map)
  if (is.null(term.map) || nrow(term.map) == 0) {
    return(data.frame(matrix(nrow = nrow(x), ncol = 0)))
  }
  xvar.all <- unique(unlist(term.map$xvar.names, use.names = FALSE))
  if (!all(xvar.all %in% colnames(x))) {
    stop("filtered baselearner variables do not match x columns\n")
  }
  xmod <- data.frame(lapply(seq_len(nrow(term.map)), function(ii) {
    xn <- rep(1, nrow(x))
    for (jj in seq_along(term.map$xvar.names[[ii]])) {
      xn <- xn * (x[, term.map$xvar.names[[ii]][jj], drop = TRUE] ^
                    term.map$xvar.power[[ii]][jj])
    }
    xn
  }))
  colnames(xmod) <- term.map$term
  attr(xmod, "term.map") <- term.map
  xmod
}
## baselearner formula constructed from filtered base learner variables
filter.baselearner.workhorse <- function (filter, sqchar = "_SQ2_", ichar="_I_") {
  if (is.term.map(filter)) {
    filter <- filter$term
  }
  filter <- as.character(filter)
  ## convert encoded base-learner names into R formula for model.matrix
  nmX <- lapply(filter, function(ll) {
    s <- gsub(ichar, ":", ll)
    if (any(grep(sqchar, s))) {
      paste(lapply(strsplit(s, "\\:")[[1]], function(ss) {
        if(any(grepl(sqchar, ss))) {
          paste0("I(", gsub(sqchar, "^2)", ss))
        }
        else {
          ss
        }
      }), collapse=":")
    }
    else {
      s
    }
  })
  ## don't save as formula (embeds environment unecessarily)
  paste("~-1+", paste(nmX, collapse="+"))
}
## baselearner X: fast construction using only filtered base learner variables
filter.baselearner <- function (x, filter, sqchar = "_SQ2_", ichar="_I_") {
  ## remove coherence check ---> this is a useful function for other applications
  term.map <- NULL
  if (is.term.map(filter)) {
    term.map <- filter
    filter <- term.map$term
  }
  else {
    if (inherits(filter, "tune.hcut")) {
      term.map <- attr(filter, "term.map")
    }
    if (is.null(term.map)) {
      term.map <- attr(filter, "term.map")
    }
  }
  filter <- as.character(filter)
  if (length(filter) == 0) {
    return(list(xvar.names = character(0),
                xvar.augment.names = character(0),
                term.map = get.term.map.subset(term.map, filter),
                x = data.frame(matrix(nrow = nrow(x), ncol = 0))))
  }
  if (is.term.map(term.map)) {
    term.map <- get.term.map.subset(term.map, filter)
    xmod <- make.baselearner.from.term.map(x, term.map)
    xvar.names <- term.map$term[!term.map$is.augment]
    xvar.augment.names <- term.map$term[term.map$is.augment]
    return(list(xvar.names = xvar.names,
                xvar.augment.names = xvar.augment.names,
                term.map = term.map,
                x = xmod))
  }
  ## mod data set using model.matrix
  f <- filter.baselearner.workhorse(filter, sqchar=sqchar, ichar=ichar)
  xmod <- data.frame(model.matrix(as.formula(f), x))
  colnames(xmod) <- filter
  ## filtered effects
  xvar.names <- filter[filter %in% colnames(x)]
  xvar.augment.names <- filter[!(filter %in% xvar.names)]
  ## return the goodies
  list(xvar.names=xvar.names,
       xvar.augment.names=xvar.augment.names,
       term.map = NULL,
       x=xmod)
}
