get.tree.index <- function(get.tree, ntree) {
  ## NULL --> default setting
  if (is.null(get.tree)) {
    rep(1, ntree)
  }
  ## the user has specified a subset of trees
  else {
    pt <- get.tree >=1 & get.tree <= ntree
    if (sum(pt) > 0) {
      get.tree <- get.tree[pt]
      get.tree.temp <- rep(0, ntree)
      get.tree.temp[get.tree] <- 1
      get.tree.temp
    }
    else {
      rep(1, ntree)
    }
  }
}
get.block.size.bits <- function (block.size, ntree) {
    ## Check for user silliness.
    if (!is.null(block.size)) {
        if ((block.size < 1) || (block.size > ntree)) {
            block.size <- ntree
        }
        else {
            block.size <- round(block.size)
        }
    }
    else {
        block.size <- ntree
    }
    return (block.size)
}
get.bootstrap.bits <- function (bootstrap) {
  if (bootstrap == "none") {
    bootstrap <- 0
  }
  else if (bootstrap == "by.root") {
    bootstrap <- 2^19
  }
  else if (bootstrap == "by.user") {
    bootstrap <- 2^20
  }
  else {
    stop("Invalid choice for 'bootstrap' option:  ", bootstrap)
  }
  return (bootstrap)
}
## convert ensemble option into native code parameter.
get.ensemble.bits <- function (ensemble) {
  if (ensemble == "oob") {
    ensemble <- 2^1
  }
  else if (ensemble == "inbag") {
    ensemble <- 2^0
  }
  else if (ensemble == "all") {
    ensemble <- 2^0 + 2^1
  }    
  else {
      ## For testing purposes, we allow the ensemble to be omitted altogether.
      ensemble <- 0
  }
  return (ensemble)
}
get.empirical.risk.bits <-  function (empirical.risk) {
  if (empirical.risk) {
    return (2^18)
  }
  else {
    return (0)
  }
}
get.forest.bits <- function (forest) {
  ## Convert forest option into native code parameter.
  if (!is.null(forest)) {
    if (forest == TRUE) {
      forest <- 2^5
    }
    else if (forest == FALSE) {
      forest <- 0
    }
    else {
      stop("Invalid choice for 'forest' option:  ", forest)
    }
  }
  else {
    stop("Invalid choice for 'forest' option:  ", forest)
  }
  return (forest)
}
get.membership.bits <- function (membership) {
  ## Convert option into native code parameter.
  bits <- 0
  if (!is.null(membership)) {
    if (membership == TRUE) {
      bits <- 2^6
    }
    else if (membership != FALSE) {
      stop("Invalid choice for 'membership' option:  ", membership)
    }
  }
  else {
    stop("Invalid choice for 'membership' option:  ", membership)
  }
  return (bits)
}
get.perf <-  function (perf, family) {
  ## Deal with non-classification
  if (family != "class") {
    if (is.null(perf)) {
      return("default")
    }
    perf <- match.arg(perf, c("none", "default", "standard"))
    if (perf == "standard") {
      perf <- "default"
    }
    return(perf)
  }
  else {
      ## Deal with classification
      if (is.null(perf)) {
          return("default")
      }
      perf <- match.arg(perf, c("none", "default", "standard", "misclass", "brier", "gmean"))
      if (perf == "standard" || perf == "misclass") {
          perf <- "default"
      }
      return(perf)
  }
}
get.perf.bits <- function (perf) {
  if (perf == "default") {
    return (2^2)
  }
  else if (perf == "gmean") {
    return (2^2 + 2^14)
  }
  else if (perf == "brier") {
    return (2^2 + 2^3)
  }
  else {#everything else becomes "none"
    return (0)
  }
}
get.rf.cores <- function () {
  if (is.null(getOption("rf.cores"))) {
    if(!is.na(as.numeric(Sys.getenv("RF_CORES")))) {
      options(rf.cores = as.integer(Sys.getenv("RF_CORES")))
    }
  }
  return (getOption("rf.cores", -1L))
}
## Real time predicton option:
is.hidden.rt <-  function(dots) {
  if (is.null(dots$real.time)) {
    FALSE
  }
  else {
    as.logical(as.character(dots$real.time))
  }
}
get.rt.bits  <- function(real.time) {
  if (real.time == TRUE) {
    bits  <- 2^7
  }
  else {
    bits  <- 0
  }
  return (bits)
}
is.hidden.rt.opt  <- function(dots) {
  if (is.null(dots$real.time.options)) {
    list(port = 6666, time.out = 15)      
  }
  else {
    dots$real.time.options
  }
}
## convert samptype option into native code parameter.
get.samptype.bits <- function (samptype) {
  if (samptype == "swr") {
    bits <- 0
  }
  else if (samptype == "swor") {
    bits <- 2^12
  }
  else {
    stop("Invalid choice for 'samptype' option:  ", samptype)
  }
  return (bits)
}
get.seed <- function (seed) {
  if ((is.null(seed)) || (abs(seed) < 1)) {
    seed <- runif(1,1,1e6)
  }
  seed <- -round(abs(seed))
  return (seed)
}
get.trace <- function (do.trace) {
  ## Convert trace into native code parameter.
  if (!is.logical(do.trace)) {
    if (do.trace >= 1) {
      do.trace <- round(do.trace)
    }
    else {
      do.trace <- 0
    }
  }
  else {
    do.trace <- 1 * do.trace
  }
  return (do.trace)
}
