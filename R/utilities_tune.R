tune.rfsgt <- function(f, data, hcut=3,
                       ntree=1, hcut.threshold=3, nfolds=10,
                       method=c("min", "conserve")[2],
                       verbose=FALSE, p.filter=40) {
  ## hcut sequence to be evaluated
  max.cut <- hcut
  hcutSeq <- 0:max.cut
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
    filter1 <- c(yvar.names, filter.rfsgt(f, data, hcut=1))
  }
  else {
    filter1 <- c(yvar.names, colnames(x))
  }
  if (max.cut >= hcut.threshold) {
    filterX <- unique(c(filter1,
      filter.rfsgt(f, data[, filter1, drop=FALSE], hcut=hcut.threshold, original=TRUE)))
  }
  ## dimension reduction for each hcut
  dots <- list()
  dots$cart.lasso <- dots$tune <- FALSE
  dots$ntree <- 1
  dots$bootstrap <- "none"
  xd <- lapply(hcutSeq, function(hcut) {
    if (verbose) cat("dimension reduction, hcut:", hcut, "\n")
    o <- do.call("rfsgt", c(list(f,
            if (hcut < hcut.threshold) data[, filter1, drop=FALSE] else data[, filterX, drop=FALSE],
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
  ## prelimary for hcut optimization
  N <- nrow(x)
  folds <- cv.folds(N, nfolds)
  dots <- list()
  dots$hcut <- 1
  dots$cart.lasso <- dots$tune <- dots$filter <- FALSE
  if (ntree==1) {
    dots$bootstrap <- "none"
  }
  ## cv-loop
  cv <- data.frame(do.call(rbind, lapply(1:length(hcutSeq), function(k) {
    if (verbose) cat("cv hcut:", hcutSeq[k], "\n")
    cvk <- sapply(folds, function(tst) {
      trn <- setdiff(1:N, tst)
      d.trn <- data.frame(data[trn,yvar.names,drop=FALSE],xd[[k]][trn,,drop=FALSE])
      x.tst <- xd[[k]][tst,,drop=FALSE]
      o <- do.call("rfsgt", c(list(f, d.trn, ntree=ntree), dots))
      mean((predict.rfsgt(o,x.tst)$predicted - data[tst,yvar.names])^2, na.rm=TRUE)
    })
    c(hcutSeq[k], mean(cvk, na.rm=TRUE), sd(cvk, na.rm=TRUE) / sqrt(nfolds))
  })))
  ## process results
  cvm <- cv[, 2]
  cvs <- cv[, 3]
  colnames(cv) <- c("hcut", "err", "serr")
  ## verbose
  if (verbose) print(cv)
  ## optimized hcut
  idx.min <- which.min(cvm)[1]
  idx.conserve <- which(cvm <= (min(cvm, na.rm=TRUE) + cvs))[1]
  idx <- if (method == "min") idx.min else idx.conserve
  ## verbose
  if (verbose) cat(paste0("optimal hcut using method=", method, ":"), cv$hcut[idx], "\n")
  ## return the goodies
  filter <- colnames(xd[[idx]])
  attr(filter, "all") <- lapply(xd, function(x) {colnames(x)})
  attr(filter, "formula") <- make.filter.baselearner.workhorse(filter)
  attr(filter, "formula.all") <- lapply(xd, function(x) {
    make.filter.baselearner.workhorse(colnames(x))})
  attr(filter, "cv") <- cv
  attr(filter, "hcut") <- cv$hcut[idx]
  attr(filter, "hcut.min") <- cv$hcut[idx.min]
  attr(filter, "hcut.1se") <- cv$hcut[idx.conserve]
  class(filter) <- "tune.rfsgt"
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
make.filter.baselearner.workhorse <- function (filter, sqchar = "_SQ2_", ichar="_I_") {
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
make.filter.baselearner <- function (x, filter, sqchar = "_SQ2_", ichar="_I_") {
  ## confirm this is a tune.rfsgt object
  if (!inherits(filter, "tune.rfsgt")) {
    stop("this function only works for objects of class 'tune.rfsgt'")
  }
  ## mod data set using model.matrix
  f <- make.filter.baselearner.workhorse(filter, sqchar=sqchar, ichar=ichar)
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
