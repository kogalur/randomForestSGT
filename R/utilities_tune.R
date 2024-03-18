tune.rfsgt <- function(f, data, hcut=3, ntree=1, hcut.threshold=3, nfolds=10,
                       verbose=TRUE, p.filter=40) {
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
    cat("dimension reduction, hcut:", hcut, "\n")
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
  if (verbose) {
    cat("filtered variables by hcut\n")
    xdn <- sapply(xd, function(x){colnames(x)})
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
    c(hcutSeq[k], mean(cvk, na.rm=TRUE), sd(cvk, na.rm=TRUE))
  })))
  ## process results
  cvm <- cv[, 2]
  cvs <- cv[, 3]
  cv <- cv[, -3]
  colnames(cv) <- c("hcut", "err")
  cv$serr <- cv$err / var(data[,yvar.names], na.rm=TRUE)
  rownames(cv) <- NULL
  ## optimized hcut
  hcut.min <- cv$hcut[which.min(cvm)[1]]
  hcut.1se <- cv$hcut[which(cvm <= (min(cvm, na.rm=TRUE) + cvs / sqrt(nfolds)))[1]]
  ## return the cv results
  list(cv=cv, hcut.min=hcut.min, hcut.1se=hcut.1se) 
}
## utilities
cv.folds <- function (n, folds = 10) {
  split(sample(1:n), rep(1:folds, length = n))
}
