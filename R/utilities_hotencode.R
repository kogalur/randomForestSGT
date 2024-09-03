## get subset of hotencoded data frame
get.hotencode.subset <- function(x, filter = NULL) {
  xvar.names <- attr(x, "hotencode")$xvar.names
  xnms <- intersect(filter, xvar.names)
  if (length(xnms) == 0) {
    return(x)
  }
  ## obtain the subset data frame
  xn <- data.frame(lapply(filter, function(xv) {
    pt <- grepl(xv, colnames(x))
    if (sum(pt) > 0) {
      x[, pt, drop = FALSE]
    }
    else {
      NULL
    }
  }))
  ## last check to make sure xn is coherent
  xn <- xn[, intersect(colnames(xn), colnames(x)), drop = FALSE]
  ## embed hotencoding
  attr(xn, "hotencode") <- attr(x, "hotencode")
  xn
}
## hot-encoding
get.hotencode <- function(x, papply = mclapply) {
  anyF <- sapply(x, is.factor) | sapply(x, is.character)
  ## factors are present -> hotencode them
  if (sum(anyF) > 0) {
    x.f <- do.call(cbind, papply(names(anyF[anyF==TRUE]), function(nn) {
      ## pull the feature: force to factor
      xn <- factor(x[, nn])
      ## one-level factors are converted to zero
      if (length(levels(xn)) == 1) {
        xn <- data.frame(rep(0, length(xn)))
        colnames(xn) <- nn
        xn
      }
      ## two-level factors are converted to binary
      else if (length(levels(xn)) == 2) {
        #xn <- data.frame(as.numeric(factor(xn)) - 1)
        xn <- data.frame(as.numeric(xn) - 1)
        colnames(xn) <- nn
        xn
      }
      else {
        f <- as.formula(paste0("~ -1 +", nn))
        xn <- data.frame(xn)
        colnames(xn) <- nn
        model.matrix(f, xn)
      }
    }))
    ## store factor information from original data
    xvar.names <- colnames(x)
    xlevels <- lapply(x[, anyF, drop = FALSE], function(x){levels(as.factor(x))})
    ## package up as data frame, store useful attributes
    x <- data.frame(x[, !anyF, drop = FALSE], x.f)
    attr(x, "hotencode") <- list(hotencode = TRUE, xvar.names = xvar.names, levels = xlevels)
  }
  ## no hotencoding performed
  else {
    attr(x, "hotencode") <- list(hotencode = FALSE, xvar.names = colnames(x))
  }
  x
}
## hot-encoding for test data
get.hotencode.test <- function(x, xtest, papply = mclapply, return.all = TRUE) {
  ## pull the original variable names (which may not be the same as colnames of x)
  xvar.names <- attr(x, "hotencode")$xvar.names
  ## confirm test data coherence
  if (length(intersect(xvar.names, names(xtest))) != length(xvar.names)) {
    stop("variable names from test data do not match training data\n")
  }
  ## restrict columns of test data to training data
  xtest <- xtest[, intersect(xvar.names, names(xtest)), drop = FALSE]
  ## nothing to do if hotencoding was not used
  if (attr(x, "hotencode")$hotencode == FALSE) {
    attr(xtest, "hotencode") <- list(hotencode = FALSE, xvar.names = colnames(xtest))
    ## return all variables (as.is)
    if (return.all) {
      return(xtest)
    }
    ## removes unncessary extra columns
    else {
      return(xtest[, colnames(x), drop = FALSE])
    }
  }
  ## pull the training levels
  xlevels <- attr(x, "hotencode")$levels
  ## there are factors present in the test data: encode them
  x.f <- do.call(cbind, papply(names(xlevels), function(nn) {
    ## pull the test feature: convert to character 
    xn <- as.character(xtest[, nn])
    ## extract train/test set labels
    trn.labels <- xlevels[[nn]]
    tst.labels <- sort(unique(xn))
    ## convert the test feature to a factor
    ## - superimpose original levels first followed by levels in test data that differ
    ## - factors get coded as before -> with potentially additional columns
    ## - for binary factors -> makes a fake integer value > 1
    xn <- factor(xn, levels = c(trn.labels, setdiff(tst.labels, trn.labels)))
    ## one-level factors are converted to zero 
    if (length(trn.labels) == 1) {
      xn <- data.frame(rep(0, length(xn)))
      colnames(xn) <- nn
      xn
    }
    ## two-level factors are converted to binary
    else if (length(trn.labels) == 2) {
      xn <- data.frame(as.numeric(xn) - 1)
      colnames(xn) <- nn
      xn
    }
    else {
      f <- as.formula(paste0("~ -1 +", nn))
      xn <- data.frame(xn)
      colnames(xn) <- nn
      model.matrix(f, xn)
    }
  }))
  ## package up as data frame, store useful attributes
  xtest <- data.frame(xtest[, setdiff(xvar.names, names(xlevels)), drop = FALSE], x.f)
  attr(xtest, "hotencode") <- list(hotencode = TRUE, xvar.names = xvar.names, levels = xlevels)
  ## return all variables (as.is)
  if (return.all) {
    xtest
  }
  ## removes unncessary extra columns
  else {
    xtest[, colnames(x), drop = FALSE]
  }
}
