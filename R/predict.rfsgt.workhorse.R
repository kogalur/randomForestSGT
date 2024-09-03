predict.rfsgt.workhorse <-  function(object,
                                     newdata,
                                     get.tree = NULL,
                                     block.size = NULL,
                                     seed = NULL,
                                     membership = FALSE,
                                     do.trace = FALSE,
                                     ...)
{
  ## confirm this is a rfsgt object
  if (!inherits(object, "rfsgt")) {
    stop("this function only works for objects of class 'rfsgt'")
  }
  ## hidden options (used later)
  dots <- list(...)
  ## initialize the seed
  seed <- get.seed(seed)
  ## hard coded options (some are legacy)
  perf.type <- NULL
  ## set restore.mode and the ensemble option
  if (missing(newdata)) {##restore: no data
    restore.mode <- TRUE
    ensemble <- "all"
  }
  else {##not restore: test data present
    restore.mode <- FALSE
    ensemble <- "inbag"
  }
  ## check if this is an anonymous object and process accordingly
  if (inherits(object, "anonymous")) {
    anonymize.bits <- 2^26
    if (restore.mode) {
      stop("in order to predict with anonymous forests please provide a test data set")
    }
  }
  else {
    anonymize.bits <- 0
  }
  ## set the family
  family <- object$family
  ## respect the training options related to bootstrapping:
  samptype <- object$samptype
  sampsize <- object$sampsize
  samp <- object$samp
  ## Ignore TDC for now in predict mode.
  subj.unique.count  <- object$n
  case.wt  <- get.weight(NULL, subj.unique.count)
  subj  <- NULL;
  event.info  <- NULL
  ## obtain y-outcome information from the grow object 
  yvar <- object$yvar
  yvar.names <- object$yvar.names
  yvar.types <- object$yvar.types
  yvar.nlevels  <- object$yvar.factor$nlevels
  yvar.numeric.levels  <- NULL
  ## obtain x information from the grow object 
  xvar <- object$xvar
  xvar.names <- object$xvar.names
  xvar.augment <- object$xvar.augment
  xvar.augment.names <- object$xvar.augment.names
  xvar.types <- object$xvar.types
  xvar.nlevels  <- rep(0, length(xvar.types))
  xvar.numeric.levels  <- NULL
  ## default setting for augmented test data is NULL
  xvar.augment.newdata <- NULL
  ## recover hcut
  hcut  <- object$hcut
  ##augmented x - convert to list for C-call
  if (hcut>1 && !is.null(xvar.augment.names)) {
    augmentXlist <- list(length(xvar.augment.names),  as.double(as.vector(data.matrix(xvar.augment))))
  }
  else {
    augmentXlist <- NULL
  }
  ## recover the split information
  splitinfo <- object$splitinfo
  splitrule <- object$splitinfo$splitrule
  ## initialize the seed
  seed <- get.seed(seed)
  ## REASSIGN OBJECT: hereafter we only need the forest 
  object <- object$forest
  ## confirm version coherence
  if (is.null(object$version)) {
    cat("\n  This function only works with objects created with the following minimum version of the package:")
    cat("\n    Minimum version:  ")
    cat("0.0.0.0")
    cat("\n    Your version:     ")
    cat("unknown")
    cat("\n")
    stop()
  }
  else {
    object.version <- as.integer(unlist(strsplit(object$version, "[.]")))
    installed.version <- as.integer(unlist(strsplit("0.0.1.54", "[.]")))
    minimum.version <- as.integer(unlist(strsplit("0.0.0.0", "[.]")))
    object.version.adj <- object.version[1] + (object.version[2]/10) + (object.version[3]/100)
    installed.version.adj <- installed.version[1] + (installed.version[2]/10) + (installed.version[3]/100)
    minimum.version.adj <- minimum.version[1] + (minimum.version[2]/10) + (minimum.version[3]/100)
    ## Minimum object version must be satisfied for us to proceed.  This is the only way
    ## terminal node restoration is guaranteed, due to RNG coherency.
    if (object.version.adj >= minimum.version.adj) {
      ## We are okay
    }
    else {
      cat("\n  This function only works with objects created with the following minimum version of the package:")
      cat("\n    Minimum version:  ")
      cat("0.0.0.0")
      cat("\n    Your version:     ")
      cat(object$version)
      cat("\n")
      stop()
    }
  }
  ##--------------------------------------------------------
  ##
  ## process x and y: test data is present
  ##
  ##--------------------------------------------------------
  if (!restore.mode) {
    ## obtain the dimension
    n.newdata <- nrow(newdata)
    ## hot encode test xvar 
    xvar.newdata <- get.hotencode.test(xvar, newdata)
    ## create test augmented data (if necessary)
    if (hcut>1 && !is.null(xvar.augment.names)) {
      xvar.augment.newdata <- filter.baselearner(xvar.newdata, xvar.augment.names)$x
    }
    ## restrict xvar to the training xvar.names
    xvar.newdata <- xvar.newdata[, intersect(colnames(xvar.newdata), xvar.names), drop=FALSE]
    ## extract test yvar (if present)
    yvar.present <- sum(is.element(yvar.names, names(newdata))) > 0
    if (yvar.present) {
      yvar.newdata <- as.matrix(newdata[, yvar.names, drop = FALSE])
      perf.type <- get.perf(perf.type, family)
    }
    else {
      yvar.newdata <-  NULL
      perf.type <- "none"
    }
  }
  ##--------------------------------------------------------
  ##
  ##  process x and y: no test data is present
  ##
  ##--------------------------------------------------------
  else {
    ## There cannot be test data in restore mode
    ## The native code switches based on n.newdata being zero (0).  Be careful.
    n.newdata <- 0
    xvar.newdata <- yvar.newdata <-  NULL
    ## perf type
    perf.type <- get.perf(perf.type, family)
  }
  ## ------------------------------------------------------------
  ##
  ## restore/non-restore x/y processing completed
  ## finalize and make C call
  ##
  ## ------------------------------------------------------------
  ## set the performance bits
  perf.bits <-  get.perf.bits(perf.type)
  ## initialize the number of trees in the forest
  ntree <- object$ntree
  ## process the get.tree vector that specifies which trees we want
  ## to extract from the forest.  This is only relevant to restore mode.
  ## The parameter is ignored in predict mode.
  get.tree <- get.tree.index(get.tree, ntree)
  bootstrap.bits <- get.bootstrap.bits(object$bootstrap)
  ## initialize the low bits
  ensemble.bits <- get.ensemble.bits(ensemble)
  ## Initalize the high bits
  samptype.bits <- get.samptype.bits(object$samptype)
  membership.bits <-  get.membership.bits(membership)
  ##  jitt.bits <- get.jitt.bits(jitt)
  ## We over-ride block-size in the case that get.tree is user specified
  block.size <- min(get.block.size.bits(block.size, ntree), sum(get.tree))
  ## Default target. We don't support more than one, and this is a bit of a legacy issue.
  m.target.idx <- 1
  ## OpenMP vector defaults to ntree in predict mode.
  omp.bits  <- get.omp.bits(NULL)
  ## do.trace
  do.trace <- get.trace(do.trace)
  ## WARNING: Note that the maximum number of slots in the following
  ## foreign function call is 64.  Ensure that this limit is not
  ## exceeded.  Otherwise, the program will error on the call.
  hcutCnt <- object$hcutCnt
  ## Real time prediction option:
  real.time  <- is.hidden.rt(dots)    
  real.time.bits  <- get.rt.bits(real.time)
  if (real.time) {
    real.time.options  <- is.hidden.rt.opt(dots)
  }
  else {
    real.time.options  <- NULL
  }
  pivot = 6
  ## Start the C external timer.
  ctime.external.start  <- proc.time()
  nativeOutput <- tryCatch({.Call("entryPred",
                                  as.integer(do.trace),
                                  as.integer(seed),
                                  as.integer(bootstrap.bits +   
                                             perf.bits +
                                             ensemble.bits +
                                             anonymize.bits),   ## low option word
                                  as.integer(samptype.bits +    
                                             membership.bits),  ## high option word
                                  as.integer(omp.bits +
                                             real.time.bits),  ## rfsgt (local and experimental) option word
                                  as.integer(ntree),
                                  as.integer(object$n),
                                  list(as.integer(subj.unique.count),
                                       if (is.null(case.wt)) NULL else as.double(case.wt),
                                       as.integer(sampsize),
                                       if (is.null(samp)) NULL else as.integer(samp)),
                                  as.integer(hcut),
                                  as.integer(splitinfo$index),
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
                                  if (is.null(yvar.types)) NULL else as.double(as.vector(yvar)),
                                  list(as.integer(ncol(xvar)),
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
                                  ## assigmnent of augmented variables (list of two values: dimension, data)
                                  augmentXlist,
                                  as.integer(n.newdata),
                                  if (is.null(yvar.newdata)) NULL else as.double(as.vector(yvar.newdata)),
                                  if (is.null(xvar.newdata)) NULL else as.double(as.vector(data.matrix(xvar.newdata))),
                                  if (is.null(xvar.augment.newdata)) NULL else as.double(as.vector(data.matrix(xvar.augment.newdata))),
                                  as.integer(object$totalNodeCount),
                                  as.integer(object$leafCount),
                                  as.integer(object$seed),
                                  as.integer((object$nativeArray)$treeID),
                                  as.integer((object$nativeArray)$nodeID),
                                  as.integer((object$nativeArray)$nodeSZ),
                                  as.integer((object$nativeArray)$brnodeID),
                                  as.double((object$nativeArray)$yBar),
                                  as.double((object$nativeArray)$yStar),
                                  as.double((object$nativeArray)$betaZ),
                                  as.double(t(as.matrix(object$nativeArray[, pivot:(pivot + hcutCnt - 1)]))),
                                  as.integer(object$hcutCnt),
                                  as.integer(object$rmbrCount),
                                  as.integer(object$ombrCount),
                                  as.integer(object$rmbrIdent),
                                  as.integer(object$ombrIdent),
                                  as.integer(get.tree),
                                  as.integer(block.size),
                                  if (real.time) list(as.integer(real.time.options$port), as.integer(real.time.options$time.out)) else NULL,
                                  as.integer(get.rf.cores()))}, error = function(e){NULL})
  ## Stop the C external timer.
  ctime.external.stop <- proc.time()
  ## check for error return condition in the native code
  if (is.null(nativeOutput)) {
    if (real.time == TRUE) {
      ## This is acceptable, for now.  Real time mode returns null,
      ## but we can revist this as the code evolves.
      return (NULL)  
    }
    else {
      stop("An error has occurred in prediction.  Please turn trace on for further analysis.")
    }
  }
  ## sample size used for the return predict object
  n.observed = ifelse(restore.mode, nrow(xvar), n.newdata)
  ## membership
  if (membership) {
    membership.out <- matrix(nativeOutput$nodeMembership, c(n.observed, ntree))
    nativeOutput$nodeMembership <- NULL
    if (restore.mode) {
      inbag.out <- matrix(nativeOutput$bootMembership, c(n.observed, ntree))
      nativeOutput$bootMembership <- NULL
    }
    else {
      inbag.out <- NULL
    }
  }
  else {
    membership.out <- NULL
    inbag.out <- NULL
  }
  if (yvar.types == "R") {
    predicted <- (if (!is.null(nativeOutput$allEnsbRGR))
                       nativeOutput$allEnsbRGR else NULL)
    predicted.oob <- (if (!is.null(nativeOutput$oobEnsbRGR))
                       nativeOutput$oobEnsbRGR else NULL)
    if (!is.null(nativeOutput$perfRegr)) {
      err.rate <- nativeOutput$perfRegr
    }
    else {
      err.rate  <- NULL
    }
  }
  ## add yvar to forest in non-restore mode when y is absent in test data
  if (!restore.mode && is.null(yvar.newdata)) {
    object$yvar <- yvar
  }
  ## make the output object
  rfsgtOutput <- list(
    call = match.call(),
    family = family,
    n = n.observed,
    samptype = samptype,
    sampsize = sampsize,
    ntree = ntree,
    hcut = hcut,
    splitrule = splitrule,
    yvar = (if (restore.mode) yvar else yvar.newdata),
    yvar.names = yvar.names,
    xvar = (if (restore.mode) xvar else xvar.newdata),
    xvar.names = xvar.names,
    leaf.count = nativeOutput$leafCount,
    forest = object,
    membership = membership.out,
    inbag = inbag.out,
    block.size = block.size,
    perf.type = perf.type,
    predicted = predicted,
    predicted.oob = predicted.oob,
    err.rate = err.rate,
    ctime.internal = nativeOutput$cTimeInternal,
    ctime.external = ctime.external.stop - ctime.external.start
  )
  ## memory management
  nativeOutput$leafCount <- NULL
  remove(object)
  remove(membership.out)
  remove(inbag.out)
  class(rfsgtOutput) <- c("rfsgt", "predict",   family)
  return(rfsgtOutput)
}
