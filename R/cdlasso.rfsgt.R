cdlasso.rfsgt <- function(formula,
                    data,
                    nfolds = 0,
                    weights = NULL,
                    nlambda = 100,
                    lambda.min.ratio = ifelse(n < n.xvar, 0.01, 1e-04),
                    lambda = NULL,
                    threshold = 1e-7,
                    eps = .0001,
                    maxit = 5000,
                    efficiency = ifelse(n.xvar < 500, "covariance", "naive"),
                    seed = NULL,
                    do.trace = FALSE)
{
  ## hard coded options
  m.target.idx <- 1
  ## initialize the seed
  seed <- get.seed(seed)
  ## formula, family, xvar.names, yvar.names
  formula.prelim <- parse.formula(formula, data, NULL)
  formula.detail <- finalize.formula(formula.prelim, data)
  family <- formula.detail$family
  xvar.org.names <- formula.detail$xvar.names
  yvar.names <- formula.detail$yvar.names
  ## coherence check
  if (!(family == "regr")) {
    stop("this function (currently) only works for regression")
  }
  if (length(xvar.org.names) == 0) {
    stop("something seems wrong: your formula did not define any x-variables")
  }
  if (length(yvar.names) == 0) {
    stop("something seems wrong: your formula did not define any y-variables")
  }
  ## hot-encode x + legacy factor processing
  xvar <- get.hotencode(data[, xvar.org.names, drop=FALSE])
  xvar <- data.matrix(xvar)
  xvar.names <- colnames(xvar)
  n.xvar <- length(xvar.names)
  n <- nrow(xvar)
  xvar.types <- rep("R", n.xvar)
  xvar.nlevels <- rep(0, n.xvar)
  xvar.numeric.levels <- NULL
  ## get y
  ## determine the immutable yvar factor map
  ## get y-outcome type and nlevels and append to y-immutable map (saved in forest later)
  yvar <- data[, yvar.names]
  yfactor <- extract.factor(data, yvar.names)
  yvar.types <- get.yvar.type(family, yfactor$generic.types, yvar.names)
  yvar.nlevels <- yfactor$nlevels
  yvar.numeric.levels <- get.numeric.levels(family, yfactor$nlevels, yvar)
  ## don't need the data anymore.
  remove(data)
  ## convert efficiency to C-side parameter: covariance (true), naive (false)
  efficiency.idx <- as.integer(ifelse(efficiency == "covariance", TRUE, FALSE))
  ## set the user defined trace
  do.trace <- get.trace(do.trace)
  ## Start the C external timer.
  ctime.external.start  <- proc.time()
  nativeOutput <- tryCatch({.Call("entryCDL",
                                  as.integer(do.trace),
                                  as.integer(seed),
                                  as.integer(0),        ## low option byte
                                  as.integer(0),        ## high option byte
                                  as.integer(0),        ## rfsgt (local) option byte
                                  as.integer(n),
                                  list(as.integer(length(yvar.types)),
                                       if (is.null(yvar.types)) NULL else as.character(yvar.types),
                                       if (is.null(yvar.types)) NULL else as.integer(yvar.nlevels),
                                       if (is.null(yvar.numeric.levels)) NULL else sapply(1:length(yvar.numeric.levels), function(nn) {as.integer(length(yvar.numeric.levels[[nn]]))})),
                                  if (is.null(yvar.numeric.levels)) {
                                      NULL
                                  }
                                  else {
                                      lapply(1:length(yvar.numeric.levels),
                                             function(nn) {as.integer(yvar.numeric.levels[[nn]])})
                                  },
                                  if (is.null(yvar.types)) NULL else as.double(as.vector(yvar)),
                                  list(as.integer(n.xvar),
                                       as.character(xvar.types),
                                       if (is.null(xvar.types)) NULL else as.integer(xvar.nlevels),
                                       if (is.null(xvar.numeric.levels)) NULL else sapply(1:length(xvar.numeric.levels), function(nn) {as.integer(length(xvar.numeric.levels[[nn]]))}),
                                       NULL,
                                       NULL),
                                  if (is.null(xvar.numeric.levels)) {
                                      NULL
                                  }
                                  else {
                                      lapply(1:length(xvar.numeric.levels),
                                             function(nn) {as.integer(xvar.numeric.levels[[nn]])})
                                  },
                                  as.double(as.vector(xvar)),
                                  list(if (is.null(m.target.idx)) as.integer(0) else as.integer(length(m.target.idx)),
                                       if (is.null(m.target.idx)) NULL else as.integer(m.target.idx)),
                                  list(as.integer(nlambda),
                                       if (is.null(lambda)) NULL else as.double(as.vector(lambda)),
                                       as.double(lambda.min.ratio),
                                       as.double(threshold),
                                       as.double(eps),
                                       as.integer(maxit),
                                       as.integer(efficiency.idx),
                                       if (is.null(weights)) NULL else as.double(as.vector(weights)),
                                       as.integer(nfolds)),
                                  as.integer(get.rf.cores()))}, error = function(e) {
                                    print(e)
                                    NULL})
  ## Stop the C external timer.
  ctime.external.stop <- proc.time()
  ## check for error return condition in the native code
  if (is.null(nativeOutput)) {
    stop("An error has occurred in prediction.  Please turn trace on for further analysis.")
  }
    convgCount  <- nativeOutput$convgCount
    lambdaCount <- nativeOutput$lambdaCount
    ## beta is virutalized as [lambdaCount] x [n.xvar + 1]
    if ((family == "regr") || (family == "regr+")) {
        beta  <- matrix(nativeOutput$beta, c(lambdaCount, n.xvar + 1), byrow=TRUE)
        colnames(beta)  <- c("Intercept", xvar.names)
        lambda  <- nativeOutput$lambda
        cdlassoOutput  <- list(convgCount=convgCount,
                               lambdaCount=lambdaCount,
                               lambda=lambda,
                               beta=beta,
                               xvar=xvar,
                               yvar=yvar)
        if (!is.null(nativeOutput$yHat)) {
            ## Note that we default to n as the case count. We do not
            ## yet support subsetting on the R-side.
            yHat  <- matrix(nativeOutput$yHat, c(lambdaCount, n), byrow=TRUE)
            lambda.min.indx  <- nativeOutput$lambdaMinIndx
            lambda.1se.indx  <- nativeOutput$lambda1SEIndx
            lambda.cvm  <- nativeOutput$lambdaMeanErr
            lambda.cvsd <- nativeOutput$lambdaCVSD
            cdlassoOutput  <- c(cdlassoOutput,
                                list(yHat = yHat,
                                     lambda.min.indx = lambda.min.indx,
                                     lambda.1se.indx = lambda.1se.indx,
                                     lambda.cvm      = lambda.cvm,
                                     lambda.cvsd     = lambda.cvsd))
        }
        cdlassoOutput  <- c(cdlassoOutput, list(ctime.internal = nativeOutput$cTimeInternal,
                                                ctime.external  = ctime.external.stop - ctime.external.start))
    } else if ((family == "class") || (family == "class+")) {
        beta  <- matrix(nativeOutput$beta, c(lambdaCount, n.xvar + 1), byrow=TRUE)
        colnames(beta)  <- c("Intercept", xvar.names)
        lambda  <- nativeOutput$lambda
        cdlassoOutput  <- list(convgCount=convgCount,
                               lambdaCount=lambdaCount,
                               lambda=lambda,
                               beta=beta,
                               xvar=xvar,
                               yvar=yvar)
        cdlassoOutput  <- c(cdlassoOutput, list(ctime.internal = nativeOutput$cTimeInternal,
                                                ctime.external  = ctime.external.stop - ctime.external.start))
    }
    return(cdlassoOutput)
}
cdlasso <- cdlasso.rfsgt
