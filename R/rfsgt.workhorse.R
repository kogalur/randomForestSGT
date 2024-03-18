rfsgt.workhorse <- function(formula,
                            data,
                            ntree = 100,
                            hcut = 1,
                            treesize = NULL,
                            tune = FALSE,
                            nodesize = NULL,
                            filter = TRUE,
                            fast = TRUE,
                            cart.lasso = FALSE,
                            eps = .005,
                            maxit = 500,
                            nfolds = 10,
                            block.size = 10,
                            bootstrap = c("by.root", "none", "by.user"),
                            samptype = c("swor", "swr"),
                            samp = NULL,
                            membership = FALSE,
                            sampsize = if (samptype == "swor") function(x){x * .632} else function(x){x},
                            seed = NULL,
                            do.trace = FALSE,
                            ...
                            )
{
  ## hidden options (used later)
  dots <- list(...)
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
  ## get y + immutable yvar factor map + y-outcome type and nlevels
  yvar <- data[, yvar.names]
  yfactor <- extract.factor(data, yvar.names)
  yvar.types <- get.yvar.type(family, yfactor$generic.types, yvar.names)
  yvar.nlevels <- yfactor$nlevels
  yvar.numeric.levels <- get.numeric.levels(family, yfactor$nlevels, yvar)
  ## grow options depending on splitrule and hcut
  splitinfo <- get.splitinfo(formula.detail, dots$splitrule, hcut, dots$nsplit, nfolds)
  hcut <- splitinfo$hcut
  nsplit <- splitinfo$nsplit
  nfolds <- splitinfo$nfolds
  ## user specified filter variable list?
  keep.filter <- xvar.org.names
  if (is.character(filter)) {
    keep.filter <- xvar.org.names[sapply(xvar.org.names, function(xv) {any(grepl(xv, filter))})]    
    if (length(keep.filter) == 0) {
      stop("user specified filering variables do not match original xvariable names\n")
    }
  }
  ## hot-encode x 
  xvar <- get.hotencode(data[, keep.filter, drop = FALSE])
  ## don't need the data anymore
  remove(data)
  ## dimension reduction using custom filtering via vimp.rfsgt
  filter.flag <- FALSE
  #if ((is.logical(filter) && filter == TRUE) || is.character(filter)) {
  if ((is.logical(filter) && filter == TRUE)) {
    filter.flag <- TRUE
    df <- data.frame(yvar, xvar)
    colnames(df) <- c(yvar.names, colnames(xvar))
    keep.custom.filter <- filter.custom.rfsgt(formula, df, hcut=hcut, method="liberal", fast=fast)
    ## keep the filter variables that the user supplied - does not make sense to remove them
    #if (is.character(filter)) {
    #  keep.custom.filter <- unique(c(colnames(xvar), keep.custom.filter))
    #}
    if (sum(colnames(xvar) %in% keep.custom.filter) > 0) {
      xvar <- xvar[, colnames(xvar) %in% keep.custom.filter, drop = FALSE]
    }
    rm(df)
  }
  ## final processing of original x
  xvar.names <- colnames(xvar)
  n.xvar <- length(xvar.names)
  n <- nrow(xvar)
  xvar.types <- rep("R", n.xvar)
  xvar.nlevels <- rep(0, n.xvar)
  xvar.numeric.levels <- NULL
  ## augmented x: calls baselearner and removes original variables
  ## filtering is then applied if user has requested it
  augmentX <- augmentXlist <-NULL
  hcutCnt <- n.xvar
  if (hcut > 1) {
    augmentX <- make.baselearner(xvar, hcut)[, -(1:n.xvar), drop = FALSE]
    if (filter.flag) {
      if (sum(colnames(augmentX) %in% keep.custom.filter) > 0) {
        augmentX <- augmentX[, colnames(augmentX) %in% keep.custom.filter, drop = FALSE]
        hcutCnt <- n.xvar + ncol(augmentX)
        augmentXlist <- list(ncol(augmentX), as.double(as.vector(data.matrix(augmentX))))
      }
      else {
        ## no augmented variables were selected: need to re-initalize hcut
        hcut <- 1
        augmentX <- augmentXlist <- NULL
      }
    }
    else {
      hcutCnt <- n.xvar + ncol(augmentX)
      augmentXlist <- list(ncol(augmentX), as.double(as.vector(data.matrix(augmentX))))
    }
  }
  ## cart.lasso: experimental bits, tune
  if (cart.lasso && hcut > 1) {
    tune <- FALSE
  }
  if (is.null(dots$experimental.bits) && !cart.lasso) {
    dots$experimental.bits <- as.integer(0)
  }
  experimental.bits <- as.integer(get.experimental.bits(dots$experimental.bits, hcut))
  ## tree grow options
  mtry <- get.mtry(family, n.xvar, dots$mtry)
  nodesize <- get.nodesize(family, nodesize)    
  lot <- get.lot(hcut=hcut,
                 treesize=treesize,
                 tune=tune,
                 lag=dots$lag,
                 strikeout=dots$strikeout,
                 max.two=dots$max.two,
                 max.three=dots$max.three,
                 max.four=dots$max.four)
  ## convert treesize to integer
  if (is.function(lot$treesize)) {
    lot$treesize <- as.integer(lot$treesize(n, hcut))
  }
  ## openMP vector defaults to ntree: valid hidden values are c("ntree", "nfolds")
  ## over-ride the default if ntree=1
  if (ntree == 1 && is.null(dots$omp)) {
    dots$omp <- "nfolds"
  }
  omp.bits  <- get.omp.bits(dots$omp)
  ## lasso parameters
  lasso <- get.lasso(dots, n, n.xvar)
  weights <- lasso$weights
  nlambda <- lasso$nlambda
  lambda.min.ratio <- lasso$lambda.min.ratio
  lambda <- lasso$lambda
  threshold <- lasso$threshold
  efficiency <- lasso$efficiency
  lasso$eps <- eps
  lasso$maxit <- maxit
  ## hard coded options (some are legacy)
  perf.type <- NULL
  ensemble <- NULL
  empirical.risk <- TRUE
  forest <- TRUE
  m.target.idx <- 1
  subj <- NULL
  xvar.time <- NULL
  subj.time <- NULL
  subj.names <- NULL
  subj.unique.count <- n
  ## initialize samptype
  samptype <- match.arg(samptype, c("swor", "swr"))
  ## bootstrap specifics
  bootstrap <- match.arg(bootstrap, c("by.root", "none", "by.user"))
  if (bootstrap == "by.root") {
    ## Nominal bootstrap at the root node.
    if (!is.function(sampsize) && !is.numeric(sampsize)) {
      stop("sampsize must be a function or number specifying size of subsampled data")
    }
    if (is.function(sampsize)) {
      sampsize.function <- sampsize
    }
    else {
      ## convert user passed sample size to a function
      sampsize.function <- make.samplesize.function(sampsize / n)
    }
    sampsize <- round(sampsize.function(n))
    if (sampsize < 1) {
      stop("sampsize must be greater than zero")
    }
    ## for swor size is limited by the number of cases
    if (samptype == "swor" && (sampsize > n)) {
      sampsize <- n
      sampsize.function <- function(x){x}
    }
    samp <- NULL
  }
  else if (bootstrap == "by.user") {    
    ## check for coherent sample: it will of be dim [.] x [ntree]
    ## where [.] is the number of rows in the data set or unique subjects in
    ## the case of time dependent covariates.
    if (is.null(samp)) {
      stop("samp must not be NULL when bootstrapping by user")
    }
    ## ntree should be coherent with the sample provided
    ntree <- ncol(samp)
    sampsize <- colSums(samp)
    if (sum(sampsize == sampsize[1]) != ntree) {
      stop("sampsize must be identical for each tree")
    }
    ## set the sample size and function
    sampsize <- sampsize[1]
    sampsize.function <- make.samplesize.function(sampsize[1] / n)
  }
  ## This is "none".
  else {
    sampsize <- n
    sampsize.function <- function(x){x}
  }
  ## Turn performance output off unless bootstrapping by root or user.
  if ((bootstrap != "by.root") && (bootstrap != "by.user")) {
    perf.type <- "none"
  }
  ## Set the ensemble option.
  if (!is.null(ensemble)) {
    ## leave the ensemble as is for testing purposes.
  }
  else {
    ensemble <- "all"
    if (bootstrap == "none") {
      ensemble <- "inbag"
    }
  }
  ## perf type
  perf.type <- get.perf(perf.type, family)
  ## Assign low bits for the native code
  bootstrap.bits <- get.bootstrap.bits(bootstrap)
  empirical.risk.bits <- get.empirical.risk.bits(empirical.risk)
  perf.bits <-  get.perf.bits(perf.type)
  ensemble.bits <- get.ensemble.bits(ensemble)
  forest.bits <- get.forest.bits(forest)
  ## Assign high bits for the native code
  membership.bits <-  get.membership.bits(membership)
  samptype.bits <- get.samptype.bits(samptype)
  ## block size
  block.size <- get.block.size.bits(block.size, ntree)
  ## Convert efficiency to C-side parameter:
  ## True is covariance. False is naive.
  efficiency.idx <- as.integer(ifelse(efficiency == "covariance", TRUE, FALSE))
  ## Set the user defined trace.
  do.trace <- get.trace(do.trace)
  ## Start the C external timer.
  ctime.external.start  <- proc.time()
  ################################################ ##
  nativeOutput <- tryCatch({.Call("entryGrow",
                                  as.integer(do.trace),
                                  as.integer(seed),
                                  as.integer(bootstrap.bits +     ## low option word
                                             empirical.risk.bits +
                                             perf.bits +
                                             ensemble.bits +
                                             forest.bits),  
                                  as.integer(membership.bits +    ## high option word
                                             samptype.bits),
                                  as.integer(omp.bits +
                                             experimental.bits),  ## rfsgt (local experimental) option word
                                  as.integer(ntree),
                                  as.integer(n),
                                  list(as.integer(n),
                                       as.integer(sampsize),
                                       if (is.null(samp)) NULL else as.integer(samp)),
                                  as.integer(splitinfo$index),
                                  as.integer(splitinfo$nsplit),
                                  as.integer(mtry),
                                  as.integer(nodesize),
                                  lot,
                                  list(if (is.null(m.target.idx)) as.integer(0) else as.integer(length(m.target.idx)),
                                       if (is.null(m.target.idx)) NULL else as.integer(m.target.idx)),
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
                                  as.double(as.vector(data.matrix(xvar))),
                                  ## Here is the hack for the augmented x-vars.  The C-side needs to know how many there are,
                                  ## so we have two elements, the first is the dimension, the second is the array. You should
                                  ## replace these with the actual values.
                                  augmentXlist,
                                  list(as.integer(nlambda),
                                       if (is.null(lambda)) NULL else as.double(as.vector(lambda)),
                                       as.double(lambda.min.ratio),
                                       as.double(threshold),
                                       as.double(eps),
                                       as.integer(maxit),
                                       as.integer(efficiency.idx),
                                       if (is.null(weights)) NULL else as.double(as.vector(weights)),
                                       as.integer(nfolds)),
                                  as.integer(block.size),
                                  as.integer(get.rf.cores()))}, error = function(e) {NULL})
  ## Stop the C external timer.
  ctime.external.stop <- proc.time()
  ## check for error return condition in the native code
  if (is.null(nativeOutput)) {
    stop("An error has occurred on the C-side.  Please turn trace on for further analysis.")
  }
  nativeArraySize = 0
  nullO <- lapply(1:ntree, function(b) {
    if (nativeOutput$leafCount[b] > 0) {
      ## The tree was not rejected.  Count the number of internal
      ## and external (terminal) nodes in the forest.
      nativeArraySize <<- nativeArraySize + (2 * nativeOutput$leafCount[b]) - 1
    }
    else {
      ## The tree was rejected.  However, it acts as a
      ## placeholder, being a stump topologically and thus
      ## adds to the total node count.
      nativeArraySize <<- nativeArraySize + 1
    }
    NULL
  })
  rm(nullO)  ##memory saver
  ## save the native array
  nativeArray <- as.data.frame(cbind(nativeOutput$treeID[1:nativeArraySize],
                                     nativeOutput$nodeID[1:nativeArraySize],
                                     nativeOutput$nodeSZ[1:nativeArraySize],
                                     nativeOutput$brnodeID[1:nativeArraySize],
                                     nativeOutput$betaZ[1:nativeArraySize],
                                     matrix(nativeOutput$betaP, nrow = nativeArraySize),
                                     nativeOutput$yStar,
                                     nativeOutput$yBar))
  colnames(nativeArray) <- c("treeID", "nodeID", "nodeSZ", "brnodeID",
                             "betaZ", as.character(1:hcutCnt), "yStar", "yBar")
  ## save the lot 
  lot <- c(lot, lasso=lasso, experimental.bits=experimental.bits, tune=tune)
  forest.out  <- list(forest = TRUE,
                      ntree = ntree,
                      n = n,
                      seed = nativeOutput$seed,
                      nativeArray = nativeArray,
                      hcutCnt = hcutCnt,
                      leafCount = nativeOutput$leafCount,
                      totalNodeCount = dim(nativeArray)[1],
                      nodesize = nodesize,
                      perf.type = perf.type,
                      lot = lot,
                      family = family,
                      bootstrap = bootstrap,
                      samptype = samptype,
                      rmbrCount = nativeOutput$rmbrCount,
                      ombrCount = nativeOutput$ombrCount,
                      rmbrIdent = nativeOutput$rmbrIdent,
                      ombrIdent = nativeOutput$ombrIdent,
                      version = "0.0.1.34")
  empr.risk <- NULL
  oob.empr.risk <- NULL
  nodeStat <- NULL
  if (!is.null(nativeOutput$bsfOrder)) {
    bsf.order <- nativeOutput$bsfOrder
    nativeOutput$bsfOrder <- NULL
  }
  if (empirical.risk) {
    if (!is.null(nativeOutput$emprRisk)) {
      empr.risk <- array(nativeOutput$emprRisk, c(lot$treesize, ntree))
      nativeOutput$emprRisk <- NULL
    }
    if (!is.null(nativeOutput$oobEmprRisk)) {
      oob.empr.risk <- array(nativeOutput$oobEmprRisk, c(lot$treesize, ntree))
      nativeOutput$oobEmprRisk <- NULL
    }
    if (!is.null(nativeOutput$nodeStat)) {
      nodeStat <- array(nativeOutput$nodeStat, c(lot$treesize, ntree))
      nativeOutput$nodeStat <- NULL
    }
  }
  if (yvar.types == "R") {
    predicted <- (if (!is.null(nativeOutput$allEnsbRGR))
                    array(nativeOutput$allEnsbRGR, n) else NULL)
    predicted.oob <- (if (!is.null(nativeOutput$oobEnsbRGR))
                        array(nativeOutput$oobEnsbRGR, n) else NULL)
    if (!is.null(nativeOutput$perfRegr)) {
      err.rate <- nativeOutput$perfRegr
    }
    else {
      err.rate  <- NULL
    }
  }
  if (membership) {
    membership.out <- matrix(nativeOutput$nodeMembership, c(n, ntree))
    inbag.out <- matrix(nativeOutput$bootstrapCount, c(n, ntree))
    nativeOutput$nodeMembership <- NULL
    nativeOutput$bootstrapCount <- NULL
  }
  else {
    membership.out  <- NULL
    inbag.out  <- NULL
  }
  rfsgtOutput  <- list(
    family = family,
    n = n,
    bootstrap = bootstrap,
    samptype = samptype,
    sampsize = sampsize,
    ntree = ntree,
    hcut = hcut,
    splitrule = splitinfo$splitrule,
    splitinfo = splitinfo,
    yvar = yvar,
    yvar.names = yvar.names,
    yvar.factor = yfactor,
    yvar.types = yvar.types,
    xvar = xvar,
    xvar.augment = augmentX,
    xvar.names = xvar.names,
    xvar.types = xvar.types,
    xvar.augment.names = colnames(augmentX),
    forest = forest.out,
    nodeStat = nodeStat,
    empr.risk = empr.risk,
    oob.empr.risk = oob.empr.risk,
    bsf.order = bsf.order,
    predicted = predicted,
    predicted.oob = predicted.oob,
    membership = membership.out,
    inbag = inbag.out,
    ensemble = ensemble,
    err.rate = err.rate,
    ctime.internal = nativeOutput$cTimeInternal,
    ctime.external = ctime.external.stop - ctime.external.start
  )
  class(rfsgtOutput) <- c("rfsgt", "grow", family)
  return(rfsgtOutput)
}
