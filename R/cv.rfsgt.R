##########################################################################################
##
## cv.rfsgt: M-fold cross-validation for rfsgt (regression)
##
## Each forest is grown on the training folds only.  This includes the variable filter
## and, when tune is requested, the tuning by tune.hcut.  The held-out fold is used only
## for prediction and its response values never enter the rfsgt or predict call.  The CV
## error is therefore honest, unlike the OOB error of rfsgt when a filter was fit on all
## cases.
##
## formula, data   as in rfsgt
## M               number of folds (ignored when folds is supplied)
## folds           optional folds: the value of make.folds, a vector of fold labels of
##                 length n, or a list of held-out case indices.  Use this to run
##                 different calls on identical folds (the folds used are returned)
## tune            TRUE or a list of tune.hcut options.  tune.hcut is then called on every
##                 training fold and its result is used as the rfsgt filter.  With hcut
##                 supplied in ..., that hcut's prefilter is used via use.tune.hcut;
##                 without hcut in ..., the tuner's selected hcut is used.
##                 tune = list(hcut = H) sets the maximum hcut evaluated by the tuner.
##                 node.center and bsf supplied in ... are also used for tuning unless
##                 they are set in tune.
## verbose         print the results for each fold?
## fold.seeds      optional positive integer seed per fold, used for the R and the native
##                 random number generators of the fold's fit and prediction
## ...             options passed to rfsgt
##
## When folds is not supplied, make.folds draws the folds before anything else, so
## set.seed() beforehand gives reproducible folds.
##
## An error in a fold is raised as a condition of class "cv.rfsgt.error" whose message
## identifies the fold and the stage (tune.hcut, use.tune.hcut, rfsgt, predict); the
## condition keeps the original error, the fold's rows, the filter and the argument
## lists of the failed call.
##
##########################################################################################
cv.rfsgt <- function(formula, data, M = 10, folds = NULL, tune = FALSE, verbose = FALSE,
                     fold.seeds = NULL, ...) {
  ## options passed to rfsgt
  dots <- list(...)
  ## each list entry must represent one named argument; in particular a tune.hcut
  ## result remains one filter entry with its attributes intact
  check.arguments <- function(x, label) {
    if (length(x) && (is.null(names(x)) || anyNA(names(x)) ||
                     any(!nzchar(names(x))) || anyDuplicated(names(x)))) {
      stop(label, " must contain uniquely named arguments.", call. = FALSE)
    }
  }
  check.arguments(dots, "Options in ...")
  ## coherence checks
  if (!inherits(formula, "formula")) {
    stop("'formula' is not a formula object.")
  }
  if (!is.data.frame(data)) {
    stop("'data' must be a data frame.")
  }
  n <- nrow(data)
  if (!is.null(dots[["samp"]])) {
    stop("samp is not allowed: the bootstrap is specific to each training fold")
  }
  ## missing and nonfinite values are not allowed
  vars <- all.vars(formula)
  vars <- if (any(vars == ".")) colnames(data) else intersect(vars, colnames(data))
  bad <- vapply(data[, vars, drop = FALSE], function(x) {
    if (is.numeric(x)) any(!is.finite(x)) else anyNA(x)
  }, logical(1))
  if (any(bad)) {
    stop("missing or nonfinite values are not allowed: ", paste(vars[bad], collapse = ", "))
  }
  ## folds: random partition of the data unless supplied by the user
  if (is.null(folds)) {
    folds <- make.folds(n, M)
  }
  else if (!is.list(folds)) {
    if (length(folds) != n) {
      stop("fold labels must have length equal to the number of observations")
    }
    folds <- split(1:n, folds)
  }
  folds <- lapply(folds, function(tst) {sort(as.integer(tst))})
  M <- length(folds)
  if (M < 2 || !identical(sort(unlist(folds, use.names = FALSE)), 1:n)) {
    stop("folds must partition the cases 1,...,n into two or more test sets")
  }
  ## optional seed per fold
  if (!is.null(fold.seeds)) {
    if (!is.numeric(fold.seeds) || length(fold.seeds) != M ||
        any(!is.finite(fold.seeds)) || any(fold.seeds < 1) ||
        any(fold.seeds != floor(fold.seeds)) ||
        any(fold.seeds >= .Machine$integer.max)) {
      stop("fold.seeds must contain one positive integer seed per fold.")
    }
    if (!is.null(dots[["seed"]])) {
      stop("Supply fold.seeds or seed, not both.")
    }
    fold.seeds <- as.integer(fold.seeds)
  }
  ## tune.hcut options: tuning is repeated on every training fold
  tune.args <- NULL
  if (is.list(tune)) {
    tune.args <- tune
  }
  else if (isTRUE(tune)) {
    tune.args <- list()
  }
  else if (!is.null(tune) && !isFALSE(tune)) {
    stop("tune must be TRUE, FALSE or a list of tune.hcut options")
  }
  ## a fixed hcut with a tuned prefilter differs from tuning hcut itself: the tuner's
  ## maximum must cover the requested hcut
  hcut.requested <- dots[["hcut"]]
  check.hcut <- function(h, label) {
    if (!is.numeric(h) || length(h) != 1L || !is.finite(h) ||
        h < 0 || h != floor(h)) {
      stop(label, " must be one nonnegative integer.", call. = FALSE)
    }
  }
  if (!is.null(tune.args)) {
    check.arguments(tune.args, "tune")
    if (any(c("f", "formula", "data") %in% names(tune.args))) {
      stop("Do not put formula or data in tune; they come from the training fold.",
           call. = FALSE)
    }
    if (!is.null(hcut.requested)) {
      check.hcut(hcut.requested, "hcut")
      if (is.null(tune.args[["hcut"]])) {
        default.max <- eval(formals(tune.hcut)[["hcut"]], envir = environment(tune.hcut))
        check.hcut(default.max, "The default tuning maximum")
        tune.args$hcut <- max(default.max, hcut.requested)
      }
      check.hcut(tune.args[["hcut"]], "tune$hcut")
      if (tune.args[["hcut"]] < hcut.requested) {
        stop("tune$hcut must be at least the requested final hcut (",
             hcut.requested, ").", call. = FALSE)
      }
    }
    ## node.center and bsf mean the same thing to tune.hcut and rfsgt: unless they are
    ## set in tune, the values supplied to rfsgt are also used for the tuning
    for (nn in c("node.center", "bsf")) {
      if (!(nn %in% names(tune.args)) && !is.null(dots[[nn]])) {
        tune.args[[nn]] <- dots[[nn]]
      }
    }
  }
  if (is.null(tune.args) && inherits(dots[["filter"]], "tune.hcut")) {
    warning("the tune.hcut filter is used as is in every fold: the CV error is optimistic ",
            "if it was tuned on these data (use the tune option to tune within folds)",
            call. = FALSE)
  }
  ## cv-loop
  cv <- lapply(1:M, function(k) {
    ## training data
    tst <- folds[[k]]
    trn <- setdiff(1:n, tst)
    d.trn <- data[trn,, drop = FALSE]
    ## case specific options are restricted to the training cases
    dots.k <- dots
    for (nn in c("case.wt", "weights")) {
      if (length(dots[[nn]]) == n) {
        dots.k[[nn]] <- dots[[nn]][trn]
      }
    }
    tuning.k <- filter.k <- NULL
    tune.call.args <- fit.call.args <- NULL
    stage <- "prepare fit"
    tryCatch({
      ## the tuner sees only the training rows
      if (!is.null(tune.args)) {
        stage <- "tune.hcut"
        tune.call.args <- c(list(formula, data = d.trn), tune.args)
        if (!is.null(fold.seeds)) set.seed(fold.seeds[k])
        tuning.k <- do.call(tune.hcut, tune.call.args)
        if (!inherits(tuning.k, "tune.hcut")) {
          stop("tune.hcut did not return a tune.hcut object.")
        }
        filter.k <- tuning.k
        ## a fixed hcut needs the term set for that hcut, not the term set of the
        ## tuner's chosen hcut
        if (!is.null(hcut.requested)) {
          stage <- "use.tune.hcut"
          if (!(hcut.requested %in% attr(tuning.k, "hcutSeq"))) {
            stop("The tuner did not evaluate requested hcut = ", hcut.requested,
                 "; increase tune$hcut.")
          }
          filter.k <- use.tune.hcut(tuning.k, hcut = hcut.requested)
          if (!inherits(filter.k, "tune.hcut") ||
              !identical(as.numeric(attr(filter.k, "hcut")),
                         as.numeric(hcut.requested))) {
            stop("use.tune.hcut did not return the requested hcut.")
          }
        }
        filter.hcut <- attr(filter.k, "hcut")
        check.hcut(filter.hcut, "The filter's hcut")
        ## the filter is one argument; hcut is synchronized before rfsgt evaluates
        ## its defaults
        dots.k$filter <- filter.k
        dots.k$hcut <- filter.hcut
        for (nn in c("node.center", "bsf")) {
          if (is.null(dots.k[[nn]]) && !is.null(attr(filter.k, nn))) {
            dots.k[[nn]] <- attr(filter.k, nn)
          }
        }
      }
      else if (inherits(dots.k[["filter"]], "tune.hcut")) {
        filter.k <- dots.k[["filter"]]
      }
      ## grow the forest: do.call preserves each list element and its attributes
      stage <- "rfsgt"
      if (!is.null(fold.seeds)) {
        set.seed(fold.seeds[k])
        dots.k$seed <- -fold.seeds[k]
      }
      fit.call.args <- c(list(formula = formula, data = d.trn), dots.k)
      o <- do.call(rfsgt, fit.call.args)
      if (o$family != "regr") {
        stop("cv.rfsgt (currently) only works for regression")
      }
      ## predict the held-out cases: the response is removed from the test data
      stage <- "predict"
      yvar.names <- o$yvar.names
      d.tst <- data[tst, colnames(data) != yvar.names, drop = FALSE]
      if (is.null(fold.seeds)) {
        yhat <- as.numeric(predict(o, newdata = d.tst)$predicted)
      }
      else {
        set.seed(fold.seeds[k])
        yhat <- as.numeric(predict(o, newdata = d.tst, seed = -fold.seeds[k])$predicted)
      }
      if (length(yhat) != length(tst) || any(!is.finite(yhat))) {
        stop("missing or nonfinite predictions for fold ", k)
      }
      err <- mean((as.numeric(data[[yvar.names]][tst]) - yhat)^2)
      ## verbose
      if (verbose) cat("fold:", k, " hcut:", o$hcut, " err:", err, "\n")
      list(predicted = yhat, err = err, hcut = o$hcut, yvar.names = yvar.names,
           tuning = tuning.k, filter = filter.k,
           hcut.selected = if (is.null(tuning.k)) NA_real_ else attr(tuning.k, "hcut"))
    }, error = function(e) {
      ## a failed fold is an error, not an NA: keep the evidence in the condition
      condition <- structure(list(
        message = paste0("cv.rfsgt: fold ", k, " [", stage, "]: ",
                         conditionMessage(e)),
        call = NULL, fold = k, stage = stage,
        original.error = e, original.call = conditionCall(e),
        train.rows = trn, test.rows = tst,
        tuning = tuning.k, filter = filter.k,
        tune.args = tune.call.args, fit.args = fit.call.args
      ), class = c("cv.rfsgt.error", "error", "condition"))
      stop(condition)
    })
  })
  names(cv) <- names(folds)
  ## assemble the cross-validated predicted values
  yvar.names <- cv[[1]]$yvar.names
  yvar <- as.numeric(data[[yvar.names]])
  predicted <- rep(NA_real_, n)
  for (k in 1:M) {
    predicted[folds[[k]]] <- cv[[k]]$predicted
  }
  err.fold <- sapply(cv, function(o) {o$err})
  ## return the goodies
  rO <- list(err = mean((yvar - predicted)^2),
             serr = sd(err.fold) / sqrt(M),
             err.fold = err.fold,
             hcut = sapply(cv, function(o) {o$hcut}),
             predicted = predicted,
             yvar = yvar,
             yvar.names = yvar.names,
             folds = folds,
             fold.seeds = fold.seeds,
             M = M,
             n = n,
             tuning = lapply(cv, function(z) z$tuning),
             filter = lapply(cv, function(z) z$filter),
             hcut.selected = sapply(cv, function(z) z$hcut.selected),
             hcut.requested = hcut.requested)
  class(rO) <- "cv.rfsgt"
  rO
}
##----------------------------------------------------------------------
##
## make.folds: random partition of the cases 1,...,n into M folds
##
## n               sample size (the data frame can be supplied in its place)
## M               number of folds
##
## Returns a list of length M with the held-out cases of each fold.  Make the
## folds up front and pass them to cv.rfsgt through the folds option in order
## to reuse identical folds over different calls.  Use set.seed() beforehand
## for reproducible folds.
##
##----------------------------------------------------------------------
make.folds <- function(n, M = 10) {
  ## the data frame can be supplied in place of the sample size
  if (is.data.frame(n)) {
    n <- nrow(n)
  }
  ## coherence checks
  if (!is.numeric(n) || length(n) != 1 || is.na(n) || n != round(n) || n < 2) {
    stop("n must be a sample size of at least 2 (or the data frame)")
  }
  if (!is.numeric(M) || length(M) != 1 || is.na(M) || M != round(M) || M < 2 || M > n) {
    stop("M must be an integer between 2 and the number of observations")
  }
  ## random partition: the held-out cases are sorted within each fold
  lapply(split(sample(1:n), rep(1:M, length = n)), sort)
}
##----------------------------------------------------------------------
##
## print method for cv.rfsgt: same layout as print.rfsgt
##
##----------------------------------------------------------------------
print.cv.rfsgt <- function(x, ...) {
  var.y <- var(x$yvar, na.rm = TRUE)
  cat("                         Sample size: ", x$n,                                        "\n", sep="")
  cat("                     Number of folds: ", x$M,                                        "\n", sep="")
  cat("                                hcut: ", paste(sort(unique(x$hcut)), collapse=", "), "\n", sep="")
  cat("                        CV R-squared: ", 1 - x$err / var.y,                          "\n", sep="")
  cat("          CV standardized error rate: ", x$err / var.y,                              "\n", sep="")
  cat("                       CV error rate: ", x$err,                                      "\n", sep="")
  cat("                   CV standard error: ", x$serr,                                     "\n", sep="")
}
