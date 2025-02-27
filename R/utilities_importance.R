##-------------------------------------------------------------------
##
## filter variables - calls the main function "vimp.rfsgt"
##
##-------------------------------------------------------------------
## custom filtering used by workhorse
filter.custom.rfsgt <- function(f, dta, hcut=1, method="liberal", fast=TRUE, ...) {
  ## hidden options
  dots <- list(...)
  dots$membership <- FALSE
  if (fast) {
    dots$ntree <- 1
  }
  ## hcut=1 call
  keep <- do.call("filter.rfsgt", c(list(f, dta, hcut=min(1, hcut), method="liberal"), dots))
  ## higher order hcut values
  if (hcut >= 2) {    
    keepXtra <- unlist(lapply(2:hcut, function(j) {
      do.call("filter.rfsgt", c(list(f, dta, hcut=j, method=method), dots))
    }))
    xvar.names <- colnames(make.baselearner(get.hotencode(get.stump(f, dta)$xvar), hcut=1))
    keepMain <- xvar.names[sapply(xvar.names, function(xv) {any(grepl(xv, keepXtra))})]
    keep <- unique(c(keep, keepMain, keepXtra))
  }
  ## return the goodies
  keep
}
## filter variables using varpro+sgt
filter.rfsgt <- function(f, dta, ntree=50, hcut=1, treesize=3, nfolds=5,
                         method = c("min", "conserve", "liberal")[3],
                         fast = TRUE, original = FALSE, 
                         maxit=100, eps=.05, names.only=TRUE, ...) {
  ## hot-encode x and create the base learner x matrix
  stump <- get.stump(f, dta)
  dta <- data.frame(stump$yvar, make.baselearner(get.hotencode(stump$xvar), hcut=hcut))
  colnames(dta)[1] <- stump$yvar.names
  ##hidden options
  dots <- list(...)
  if (fast) {
    ntree <- 1
  }
  if (ntree == 1) {
    if (is.null(dots$imp)) {
      dots$omp <- "nfolds"
    }
    dots$bootstrap <- "none"
  }
  dots$membership <- FALSE
  ## hereafter we pretend hcut=1
  ## obtain split-weights for varpro
  swt <- do.call("vimp.rfsgt", c(list(f, dta, ntree=ntree, hcut=1, treesize=treesize,
               nfolds=nfolds, maxit=maxit, eps=eps), dots))
  ## call varpro using custom split-weights
  checkfail <- tryCatch({capture.output(vmp <- cv.varpro(f, dta,
          split.weight.custom=swt))}, error=function(ex){NULL})
  ## return vimp.rfsgt if varpro fails 
  if (is.null(checkfail)) {
    if (original) {
      attr(swt, "xvar.org.names") <- stump$xvar.names
      swt <- get.orgvimp.rfsgt(swt)
    }
    if (sum(swt>0, na.rm=TRUE) > 0) {
      return(names(swt[swt>0]))
    }
    else {
      return(names(swt[order(swt, decreasing = TRUE)])[1])
    }
  }
  ## extract cv-varpro vimp
  vmp <- get.vimp(vmp, pretty=FALSE)
  vmp.names <- rownames(vmp)
  colnames(vmp) <- c("min", "conserve", "liberal")
  vmp <- vmp[, match(method, c("min", "conserve", "liberal"))]
  names(vmp) <- vmp.names
  if (original) {
    attr(vmp, "xvar.org.names") <- stump$xvar.names
    attr(vmp, "xvar.names") <- vmp.names
    vmp <- get.orgvimp.rfsgt(vmp)
  }
  ## return selected variables
  if (sum(vmp>0, na.rm=TRUE) > 0) {
    keep <- vmp[vmp>0]
  }
  else {
    keep <- (vmp[order(vmp, decreasing=TRUE)])[1]
  }
  if (names.only) {
    names(keep)
  }
  else {
    keep
  }
}
##-------------------------------------------------------------------
##
## main function used for variable selection uses varpro + sgt
##
##-------------------------------------------------------------------
vimp.rfsgt <- function(f, dta, ntree=50, hcut=1, treesize=3,
                       nfolds=5, maxit=100, eps=.05, p=NULL, ...) {
  ##-------------------------------------------------------------------
  ##
  ## initialize
  ##
  ##-------------------------------------------------------------------
  ## stump tree used to probe and clean the data
  stump <- get.stump(f, dta)
  ## extract x and  hotencode it 
  x <- get.hotencode(stump$xvar)
  xvar.names <- colnames(make.baselearner(x, hcut=hcut))
  if (is.null(p)) {
    p <- ncol(x)
  }
  p <- min(p, length(xvar.names))
  ## rescale x,y and assemble the new data
  y <- stump$yvar
  ysd <- sd(y, na.rm = TRUE)
  y <- y / (1 * (ysd==0) + ysd)
  yvar.names <- stump$yvar.names
  dta <- data.frame(y, normalize(x))
  colnames(dta) <- c(yvar.names, colnames(x))
  ##-------------------------------------------------------------------
  ##
  ## hidden options
  ##
  ##-------------------------------------------------------------------
  dots <- list(...)
  dots$tune <- dots$filter <- FALSE
  dots$perf.type <- "none"
  dots$experimental.bits <- as.integer(0)
  ##-------------------------------------------------------------------
  ##
  ## super greedy forest call 
  ##
  ##-------------------------------------------------------------------
  nativeArray <- do.call("rfsgt.workhorse", c(list(
                                 formula=f,
                                 data=dta,
                                 ntree=ntree, 
                                 hcut=hcut,
                                 treesize=treesize,
                                 nfolds=nfolds,
                                 maxit=maxit,
                                 eps=eps), dots))$forest$nativeArray
  ##-------------------------------------------------------------------
  ##
  ## get beta
  ##
  ##-------------------------------------------------------------------
  ## yBar!= NA--> CART split
  ar <- nativeArray[is.na(nativeArray$yBar),, drop = FALSE]
  ## remove everything but beta
  if (nrow(ar) > 0) {
    beta <- ar[, !(colnames(ar) %in%
      c("treeID", "nodeID", "nodeSZ", "brnodeID","betaZ", "yStar", "yBar", "prnodeID")), drop = FALSE]
    beta <- beta[, 1:p, drop = FALSE]
    beta <- colMeans(abs(beta), na.rm = TRUE)
  }
  else {
    beta <- rep(NA, p)
  }
  names(beta) <- xvar.names[1:p]
  ##-------------------------------------------------------------------
  ##
  ## return beta with hidden attributes
  ##
  ##-------------------------------------------------------------------
  class(beta) <- "vimp.rfsgt"
  attr(beta, "xvar.names") <- xvar.names
  attr(beta, "xvar.org.names") <- stump$xvar.names
  beta  
}
##----------------------------------------------------------------------
##
## custom print object for vimp.rfsgt to make attributes invisible
##
##----------------------------------------------------------------------
print.vimp.rfsgt <- function(x, ...) {
  attr(x, "class") <- attr(x, "xvar.names") <-
    attr(x, "xvar.org.names") <- NULL
  print(x)
}
##----------------------------------------------------------------------
##
## custom function for extracting beta from vimp.rfsgt
##
##----------------------------------------------------------------------
get.orgvimp.rfsgt <- function(beta, pretty=FALSE) {
  ## pull xvar names (original and hot-encoded)
  xvar.org.names <- attr(beta, "xvar.org.names")
  xvar.names <- attr(beta, "xvar.names")
  ## match original variable names to varpro names which uses hot encode data
  vars <- xvar.org.names[which(unlist(lapply(xvar.org.names, function(nn) {
    if (any(grepl(nn, names(beta)))) {
      TRUE
    }
    else {
      FALSE
    }
  })))]
  ## obtain beta for mapped variables
  vars.beta <- lapply(xvar.org.names, function(nn) {
    if (any((pt <- grepl(nn, names(beta))))) {
      if (!all(is.na(beta[pt]))) {
        max(beta[pt], na.rm = TRUE)
      }
      else {
        0
      }
    }
    else {
      NULL
    }
  })
  ## remove NULL entries
  vars.beta <- unlist(vars.beta[!sapply(vars.beta, is.null)])
  ## make nice table for return
  if (pretty) {
    topvars <- data.frame(variable = vars, beta = vars.beta)
    topvars[order(topvars$beta, decreasing = TRUE),, drop = FALSE]
  }
  ## return named vector with beta values (0 if not selected)
  else {
    beta <- rep(0, length(xvar.org.names))
    names(beta) <- xvar.org.names
    beta[vars] <- vars.beta
    beta
  }
}
