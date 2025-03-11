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
  attr(fnew, "hcut") <- hcutSeq[hcut.idx]
  if (!is.null(attr(f, "pure.lasso"))) {
    attr(fnew, "pure.lasso") <- attr(f, "pure.lasso.all")[hcut.idx]
  }
  fnew
}
tune.hcut <- function(f, data, hcut=3,
                      ntree=5, treesize=5,
                      hcut.threshold=3, nfolds=10,
                      pure.lasso.check=TRUE,
                      verbose=FALSE, p.filter=40) {
  ## hcut sequence to be evaluated
  hcut.max <- hcut
  hcutSeq <- 0:hcut.max
  ## stump tree for extraction
  o.stump <- get.stump(f, data)
  ## hot-encode the data
  x <- get.hotencode(o.stump$xvar)
  ## pull yvar and redefine the data using the hot-encoded X
  yvar.names <- o.stump$yvar.names
  f <- as.formula(paste(yvar.names, "~."))
  data <- data.frame(o.stump$yvar, x)
  colnames(data) <- c(yvar.names, colnames(x))
  ## fast filtering
  if (length(colnames(x)) > p.filter) {
    if (verbose) cat("preliminary dimension reduction used for large p\n")
    filterS <- c(yvar.names, filter.rfsgt(f, data, hcut=1, treesize=treesize))
  }
  else {
    filterS <- c(yvar.names, colnames(x))
  }
  if (hcut.max >= hcut.threshold) {
    filterX <- unique(c(filterS,
      filter.rfsgt(f, data[, filterS, drop=FALSE], hcut=hcut.threshold, treesize=treesize, original=TRUE)))
  }
  ## dimension reduction for each hcut
  dots <- list()
  dots$ntree <- 1
  dots$tune <- FALSE
  dots$bootstrap <- "none"
  xd <- lapply(hcutSeq, function(hcut) {
    if (verbose) cat("dimension reduction, hcut:", hcut, "\n")
    o <- do.call("rfsgt", c(list(f,
            if (hcut < hcut.threshold) data[, filterS, drop=FALSE] else data[, filterX, drop=FALSE],
            hcut=hcut), dots))
    if (is.null(o$xvar.augment.names)) {
      o$xvar
    }
    else {
      data.frame(o$xvar, o$xvar.augment)
    }
  })
  ## verbose
  if (verbose) {
    cat("filtered variables by hcut\n")
    xdn <- lapply(xd, function(x){colnames(x)})
    names(xdn) <- hcutSeq
    print(xdn)
  }
  ## prelimary calculations for hcut optimization
  N <- nrow(x)
  folds <- cv.folds(N, nfolds)
  dots <- list()
  dots$hcut <- 1
  dots$ntree <- ntree
  dots$treesize <- treesize
  dots$tune <- dots$filter <- FALSE
  if (ntree==1) {
    dots$bootstrap <- "none"
  }
  ## cv-loop
  dots$pure.lasso <- FALSE
  cv <- data.frame(do.call(rbind, lapply(1:length(hcutSeq), function(k) {
    if (verbose) cat("cv hcut:", hcutSeq[k], "\n")
    cvk <- sapply(folds, function(tst) {
      trn <- setdiff(1:N, tst)
      d.trn <- data.frame(data[trn,yvar.names,drop=FALSE],xd[[k]][trn,,drop=FALSE])
      x.tst <- xd[[k]][tst,,drop=FALSE]
      o <- do.call("rfsgt", c(list(f, d.trn), dots))
      mean((predict.rfsgt(o,x.tst)$predicted - data[tst,yvar.names])^2, na.rm=TRUE)
    })
    c(hcutSeq[k], mean(cvk, na.rm=TRUE), sd(cvk, na.rm=TRUE) / sqrt(nfolds))
  })))
  colnames(cv) <- c("hcut", "err", "serr")
  ## verbose
  if (verbose) print(cv)
  ## cv-loop - pure.lasso
  if (pure.lasso.check) {
    dots$pure.lasso <- TRUE
    cv.pure <- data.frame(do.call(rbind, lapply(1:length(hcutSeq), function(k) {
      if (verbose) cat("cv hcut (pure lasso):", hcutSeq[k], "\n")
      cvk <- sapply(folds, function(tst) {
        trn <- setdiff(1:N, tst)
        d.trn <- data.frame(data[trn,yvar.names,drop=FALSE],xd[[k]][trn,,drop=FALSE])
        x.tst <- xd[[k]][tst,,drop=FALSE]
        o <- do.call("rfsgt", c(list(f, d.trn), dots))
        mean((predict.rfsgt(o,x.tst)$predicted - data[tst,yvar.names])^2, na.rm=TRUE)
      })
      c(hcutSeq[k], mean(cvk, na.rm=TRUE), sd(cvk, na.rm=TRUE) / sqrt(nfolds))
    })))
    colnames(cv.pure) <- c("hcut", "err", "serr")    
  }
  ## verbose: print pure lasso results (if requested)
  if (verbose && pure.lasso.check) print(cv.pure)
  ## determine the optimal hcut (use non-pure lasso which is more robust)
  idx <- which.min(cv[, 2])[1]
  ## set the pure lasso option
  ## pure lasso cv has to be convincing:  plus one maximal standard error
  if (pure.lasso.check) {
    pure.lasso.all <- cv[, 2] > (cv.pure[, 2] + max(cv.pure[, 3], na.rm=TRUE))
    pure.lasso.all[1] <- FALSE #hcut=0
    if (all(pure.lasso.all[-1])) {#if pure lasso dominates, then choose hcut based on pure cv
      cv.pure[1, 2] <- max(cv[1, 2], cv.pure[1, 2], na.rm=TRUE) #hcut=0
      idx <- which.min(cv.pure[, 2])[1]
    }
    pure.lasso <- pure.lasso.all[idx]
  }
  else {
    pure.lasso <- pure.lasso.all <- NULL
  }
  ## verbose
  if (verbose) cat(paste0("optimal hcut="), hcutSeq[idx], "\n")
  ## return the goodies
  filter <- colnames(xd[[idx]])
  attr(filter, "all") <- lapply(xd, function(x) {colnames(x)})
  attr(filter, "formula") <- filter.baselearner.workhorse(filter)
  attr(filter, "formula.all") <- lapply(xd, function(x) {
    filter.baselearner.workhorse(colnames(x))})
  attr(filter, "cv") <- cv
  attr(filter, "cv.pure") <- cv.pure
  attr(filter, "hcut") <- hcutSeq[idx]
  attr(filter, "hcutSeq") <- hcutSeq
  attr(filter, "pure.lasso") <- pure.lasso
  attr(filter, "pure.lasso.all") <- pure.lasso.all
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
## baselearner formula constructed from filtered base learner variables
filter.baselearner.workhorse <- function (filter, sqchar = "_SQ2_", ichar="_I_") {
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
  ## confirm this is a tune.hcut object
  #if (!inherits(filter, "tune.hcut")) {
  #  stop("this function only works for objects of class 'tune.hcut'")
  #}
  ## mod data set using model.matrix
  f <- filter.baselearner.workhorse(filter, sqchar=sqchar, ichar=ichar)
  xmod <- data.frame(model.matrix(as.formula(f), x))
  colnames(xmod) <- filter
  ## filtered effects
  xvar.names <- intersect(filter, colnames(x))
  xvar.augment.names <- setdiff(filter, xvar.names)
  ## return the goodies
  list(xvar.names=xvar.names,
       xvar.augment.names=xvar.augment.names,
       x=xmod)
}
