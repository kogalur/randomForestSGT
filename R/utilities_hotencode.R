## pull original x-variable names needed for a filtered set of baselearners
get.hotencode.filter <- function(x, filter = NULL,
                                 sqchar = "_SQ2_", ichar = "_I_") {
  info <- attr(x, "hotencode")
  if (is.null(info)) {
    stop("hotencode information is missing\n")
  }
  if (is.null(filter) || length(filter) == 0) {
    return(info$xvar.names)
  }
  filter <- unique(filter)
  ## exact matches must be preserved.  This is essential when x has already
  ## been converted into a filtered/prepared design, such as in tune.hcut CV.
  vars.exact <- info$xvar.names[info$xvar.names %in% filter]
  filter.extra <- setdiff(filter, vars.exact)
  ## backward compatibility for older objects that do not store a map
  if (is.null(info$map)) {
    vars.partial <- info$xvar.names[sapply(info$xvar.names, function(nn) {
      any(grepl(nn, filter.extra, fixed = TRUE))
    })]
    vars <- unique(c(vars.exact, vars.partial))
    if (length(vars) > 0) {
      return(info$xvar.names[info$xvar.names %in% vars])
    }
    return(info$xvar.names)
  }
  ## break unmatched baselearners into their hotencoded components
  if (length(filter.extra) > 0) {
    terms <- unique(unlist(lapply(filter.extra, function(nn) {
      nn <- unlist(strsplit(nn, ichar, fixed = TRUE))
      gsub(sqchar, "", nn, fixed = TRUE)
    }), use.names = FALSE))
    terms <- terms[nzchar(terms)]
  }
  else {
    terms <- character(0)
  }
  vars.map <- info$xvar.names[sapply(info$xvar.names, function(nn) {
    any(info$map[[nn]] %in% terms)
  })]
  vars <- unique(c(vars.exact, vars.map))
  if (length(vars) > 0) {
    return(info$xvar.names[info$xvar.names %in% vars])
  }
  info$xvar.names
}
## zero-row hotencode object used as prediction schema
get.hotencode.info <- function(x, filter = NULL) {
  info <- attr(x, "hotencode")
  if (is.null(info)) {
    stop("hotencode information is missing\n")
  }
  if (is.null(filter) || length(filter) == 0) {
    filter <- info$xvar.names
  }
  else {
    filter <- intersect(filter, info$xvar.names)
    if (length(filter) == 0) {
      filter <- info$xvar.names
    }
  }
  if (is.null(info$map)) {
    cols <- intersect(colnames(x), filter)
    map <- as.list(cols)
    names(map) <- cols
  }
  else {
    map <- info$map[filter]
    cols <- unique(unlist(map, use.names = FALSE))
  }
  xn <- as.data.frame(matrix(nrow = 0, ncol = length(cols)))
  if (length(cols) > 0) {
    colnames(xn) <- cols
  }
  info$xvar.names <- filter
  if (!is.null(info$levels)) {
    info$levels <- info$levels[intersect(names(info$levels), filter)]
  }
  if (is.null(info$levels) || length(info$levels) == 0) {
    info$levels <- NULL
    info$hotencode <- FALSE
  }
  else {
    info$hotencode <- TRUE
  }
  info$map <- map
  attr(xn, "hotencode") <- info
  xn
}
## get subset of hotencoded data frame
get.hotencode.subset <- function(x, filter = NULL) {
  xn.info <- get.hotencode.info(x, filter)
  xn <- x[, intersect(colnames(xn.info), colnames(x)), drop = FALSE]
  ## embed hotencoding
  attr(xn, "hotencode") <- attr(xn.info, "hotencode")
  xn
}
## hot-encoding
get.hotencode <- function(x, papply = lapply) {
  anyF <- sapply(x, is.factor) | sapply(x, is.character)
  xvar.names <- colnames(x)
  ## factors are present -> hotencode them
  if (sum(anyF) > 0) {
    x.f.names <- names(anyF[anyF == TRUE])
    x.f.list <- papply(x.f.names, function(nn) {
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
    })
    names(x.f.list) <- x.f.names
    x.f <- do.call(cbind, unname(x.f.list))
    ## store factor information from original data
    xlevels <- lapply(x[, anyF, drop = FALSE], function(x) {levels(as.factor(x))})
    xmap <- lapply(xvar.names, function(nn) {
      if (anyF[[nn]]) {
        colnames(x.f.list[[nn]])
      }
      else {
        nn
      }
    })
    names(xmap) <- xvar.names
    ## package up as data frame, store useful attributes
    x <- data.frame(x[, !anyF, drop = FALSE], x.f)
    attr(x, "hotencode") <- list(
      hotencode = TRUE,
      xvar.names = xvar.names,
      levels = xlevels,
      map = xmap
    )
  }
  ## no hotencoding performed
  else {
    xmap <- lapply(xvar.names, function(nn) {nn})
    names(xmap) <- xvar.names
    attr(x, "hotencode") <- list(
      hotencode = FALSE,
      xvar.names = xvar.names,
      levels = NULL,
      map = xmap
    )
  }
  x
}
## hot-encoding for test data
get.hotencode.test <- function(x, xtest, papply = lapply, return.all = TRUE) {
  hotencode <- attr(x, "hotencode")
  ## pull the original variable names (which may not be the same as colnames of x)
  xvar.names <- hotencode$xvar.names
  ## confirm test data coherence
  if (!all(xvar.names %in% names(xtest))) {
    stop("variable names from test data do not match training data\n")
  }
  ## restrict columns of test data to training data
  xtest <- xtest[, xvar.names, drop = FALSE]
  ## nothing to do if hotencoding was not used
  if (hotencode$hotencode == FALSE) {
    attr(xtest, "hotencode") <- list(
      hotencode = FALSE,
      xvar.names = colnames(xtest),
      levels = NULL,
      map = hotencode$map[colnames(xtest)]
    )
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
  xlevels <- hotencode$levels
  ## no factor variables remain in the prediction schema
  if (is.null(xlevels) || length(xlevels) == 0) {
    attr(xtest, "hotencode") <- list(
      hotencode = FALSE,
      xvar.names = xvar.names,
      levels = NULL,
      map = hotencode$map[xvar.names]
    )
    if (return.all) {
      return(xtest)
    }
    else {
      return(xtest[, colnames(x), drop = FALSE])
    }
  }
  ## there are factors present in the test data: encode them
  x.f.list <- papply(names(xlevels), function(nn) {
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
  })
  x.f <- do.call(cbind, unname(x.f.list))
  ## package up as data frame, store useful attributes
  xtest <- data.frame(xtest[, setdiff(xvar.names, names(xlevels)), drop = FALSE], x.f)
  ## preserve the training hotencode schema exactly.
  ## Under train/test Monte Carlo splits, it is possible for a training
  ## hotencoded factor column to be absent from the raw test-side rebuild
  ## (for example when a factor level does not occur in the current test set).
  ## Those columns should be present with all zero values.
  expected.cols <- colnames(x)
  missing.cols <- setdiff(expected.cols, colnames(xtest))
  if (length(missing.cols) > 0) {
    for (nn in missing.cols) {
      xtest[[nn]] <- 0
    }
  }
  if (length(expected.cols) > 0) {
    extra.cols <- setdiff(colnames(xtest), expected.cols)
    xtest <- xtest[, c(expected.cols, extra.cols), drop = FALSE]
  }
  attr(xtest, "hotencode") <- list(
    hotencode = TRUE,
    xvar.names = xvar.names,
    levels = xlevels,
    map = hotencode$map[xvar.names]
  )
  ## return all variables (as.is)
  if (return.all) {
    xtest
  }
  ## removes unncessary extra columns
  else {
    xtest[, colnames(x), drop = FALSE]
  }
}
