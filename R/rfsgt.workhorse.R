rfsgt.workhorse <- function(formula,
                            data,
                            ntree = 100,
                            hcut = 1,
                            treesize = NULL,
                            tune.treesize = FALSE,
                            nodesize = NULL,
                            filter = TRUE,
                            keep.only = NULL,
                            fast = TRUE,
                            pure.lasso = FALSE,
                            eps = .005,
                            maxit = 500,
                            nfolds = 10,
                            block.size = 10,
                            bootstrap = c("by.root", "none", "by.user"),
                            samptype = c("swor", "swr"),
                            samp = NULL,
                            case.wt = NULL,
                            membership = FALSE,
                            sampsize = if (samptype == "swor") function(x){x * .632} else function(x){x},
                            ntime = 150,
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
  subj.names <- formula.detail$subj.names
  ## coherence check
  if (!((family == "regr") || (family == "surv"))) {
    stop("this function (currently) only works for regression")
  }
  if (length(xvar.org.names) == 0) {
    stop("something seems wrong: your formula did not define any x-variables")
  }
  if (length(yvar.names) == 0) {
    stop("something seems wrong: your formula did not define any y-variables")
  }
  ## save hotencode information of the original variables
  hotencode <- attr(get.hotencode(data[, xvar.org.names, drop = FALSE]), "hotencode")
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
  ## flag for building agumented x baselearner
  baselearner.flag <- TRUE
  augmentX <- augmentXlist <- hcutCnt <- NULL
  ## keep only a requested subset of the features? ignored if filter is of type "tune.hcut"
  xvar.keep <- xvar.org.names
  if (!is.null(keep.only) && !inherits(filter, "tune.hcut")) {
    xvar.keep <- intersect(xvar.org.names, keep.only)
    if (length(xvar.keep) == 0) {
      stop("user specified filtering variables do not match original xvariable names\n")
    }
  }
  ## hot-encode the data (which can be subsetted)
  xvar <- get.hotencode(data[, xvar.keep, drop=FALSE])
  ## dimension reduction using tune.hcut filtering variables
  ## filtering is switched off and we build the augmented x here
  if (inherits(filter, "tune.hcut")) {
    hcut <- attr(filter, "hcut")
    if (!is.null(attr(filter, "pure.lasso"))) {
      pure.lasso <- attr(filter, "pure.lasso")
    }
    baselearnerO <- filter.baselearner(xvar, filter)
    xvar <- baselearnerO$x[, baselearnerO$xvar.names, drop = FALSE]
    hcutCnt <- ncol(xvar)
    if (length(baselearnerO$xvar.augment.names) > 0) {
      augmentX <- baselearnerO$x[, baselearnerO$xvar.augment.names, drop = FALSE]
      augmentXlist <- list(ncol(augmentX), as.double(as.vector(data.matrix(augmentX))))
      hcutCnt <- hcutCnt + ncol(augmentX)
    }
    else {
      ## no augmented variables were selected: need to re-initalize hcut
      hcut <- min(hcut, 1) ## hcut can be 0
      augmentX <- augmentXlist <- NULL
    }
    filter <- baselearner.flag <- FALSE
  }
  ## dimension reduction using custom filtering via filter.custom.rfsgt
  ## does not apply if tune.hcut has been used
  if (filter) {
    df <- data.frame(yvar, xvar)
    colnames(df) <- c(yvar.names, colnames(xvar))
    xvar.keep.custom <- filter.custom.rfsgt(formula, df, hcut=hcut, method="liberal", fast=fast)
    if (sum(colnames(xvar) %in% xvar.keep.custom) > 0) {
      xvar <- xvar[, colnames(xvar) %in% xvar.keep.custom, drop = FALSE]
    }
    rm(df)
  }
  ## attach the hotencode information for later downstream calculations
  attr(xvar, "hotencode") <- hotencode
  ## final processing of original x
  xvar.names <- colnames(xvar)
  n.xvar <- length(xvar.names)
  n <- nrow(xvar)
  xvar.types <- rep("R", n.xvar)
  xvar.nlevels <- rep(0, n.xvar)
  xvar.numeric.levels <- NULL
  if (is.null(hcutCnt)) {
    hcutCnt <- n.xvar
  }
  ## build augmented x - careful if filtering has been applied
  if (hcut > 1 && baselearner.flag) {
    augmentX <- make.baselearner(xvar, hcut)[, -(1:n.xvar), drop = FALSE]
    if (filter) {
      if (sum(colnames(augmentX) %in% xvar.keep.custom) > 0) {
        augmentX <- augmentX[, colnames(augmentX) %in% xvar.keep.custom, drop = FALSE]
        hcutCnt <- hcutCnt + ncol(augmentX)
        augmentXlist <- list(ncol(augmentX), as.double(as.vector(data.matrix(augmentX))))
      }
      else {
        ## no augmented variables were selected: need to re-initalize hcut
        hcut <- 1
        augmentX <- augmentXlist <- NULL
      }
    }
    else {
      hcutCnt <- hcutCnt + ncol(augmentX)
      augmentXlist <- list(ncol(augmentX), as.double(as.vector(data.matrix(augmentX))))
    }
  }
  ## pure lasso?
  if (pure.lasso && hcut>0) {
    dots$experimental.bits <- as.integer(0)
  }
  experimental.bits <- as.integer(get.experimental.bits(dots$experimental.bits, hcut))
  ## tree grow options
  mtry <- get.mtry(family, n.xvar, dots$mtry)
  nodesize <- get.nodesize(family, nodesize)    
  lot <- get.lot(hcut=hcut,
                 treesize=treesize,
                 tune=tune.treesize,
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
  event.info <- NULL
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
      sampsize.function <- make.samplesize.function(sampsize / subj.unique.count)
    }
    sampsize <- round(sampsize.function(subj.unique.count))
    if (sampsize < 1) {
      stop("sampsize must be greater than zero")
    }
    ## for swor size is limited by the number of cases
    if (samptype == "swor" && (sampsize > subj.unique.count)) {
      sampsize <- subj.unique.count
      sampsize.function <- function(x){x}
    }
    samp <- NULL
    case.wt  <- get.weight(case.wt, subj.unique.count)
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
    sampsize.function <- make.samplesize.function(sampsize[1] / subj.unique.count)
    case.wt  <- get.weight(NULL, subj.unique.count)
  }
  ## This is "none".
  else {
    sampsize <- subj.unique.count
    sampsize.function <- function(x){x}
    case.wt  <- get.weight(case.wt, sampsize)
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
                                  list(as.integer(subj.unique.count),
                                       if (is.null(case.wt)) NULL else as.double(case.wt),
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
                                       if (is.null(yvar.numeric.levels)) NULL else sapply(1:length(yvar.numeric.levels), function(nn) {as.integer(length(yvar.numeric.levels[[nn]]))}),
                                       if (is.null(subj)) NULL else as.integer(subj),
                                       if (is.null(event.info)) as.integer(0) else as.integer(length(event.info$event.type)),
                                       if (is.null(event.info)) NULL else as.integer(event.info$event.type)),
                                  if (is.null(yvar.numeric.levels)) {
                                    NULL
                                  }
                                  else {
                                    lapply(1:length(yvar.numeric.levels),
                                           function(nn) {as.integer(yvar.numeric.levels[[nn]])})
                                  },
                                  if (is.null(yvar.types)) NULL else as.double(as.vector(data.matrix(yvar))),
                                  list(if(is.null(event.info$time.interest)) as.integer(0) else as.integer(length(event.info$time.interest)),
                                       if(is.null(event.info$time.interest)) NULL else as.double(event.info$time.interest)),
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
                                     nativeOutput$yBar,
                                     nativeOutput$prnodeID[1:nativeArraySize]
                                     ))
  colnames(nativeArray) <- c("treeID", "nodeID", "nodeSZ", "brnodeID",
                             "betaZ", as.character(1:hcutCnt), "yStar", "yBar", "prnodeID")
  ## save the lot 
  lot <- c(lot, lasso=lasso, experimental.bits=experimental.bits, tune=tune.treesize)
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
                      experimental.bits = experimental.bits,
                      rmbrCount = nativeOutput$rmbrCount,
                      ombrCount = nativeOutput$ombrCount,
                      rmbrIdent = nativeOutput$rmbrIdent,
                      ombrIdent = nativeOutput$ombrIdent,
                      ambrOffset = matrix(nativeOutput$ambrOffset, nrow = n),
                      version = "0.0.1.63")
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
