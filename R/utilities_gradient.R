get.beta <- function(o) {
  ## pull the native array
  nativeArray <- o$forest$nativeArray
  ## yBar!= NA--> CART split
  ar <- nativeArray[is.na(nativeArray$yBar),, drop = FALSE]
  ## remove everything but beta
  if (nrow(ar) > 0) {
    beta <- ar[, !(colnames(ar) %in%
              c("treeID", "nodeID", "nodeSZ", "brnodeID","betaZ", "yStar", "yBar")), drop = FALSE]
  }
  else {
    return(NULL)
  }
  colnames(beta) <- c(o$xvar.names, o$xvar.augment.names) 
  ## return beta 
  beta  
}
gradient.rfsgt <- function(formula, data, max.three=1000)
{
  ## replace "." with "_" to avoid interfering with interaction syntax
  colnames(data) <- gsub("\\.", "_", colnames(data))
  ## stump tree for extraction
  o.stump <- get.stump(formula, data)
  ## hot-encode the data
  x <- get.hotencode(o.stump$xvar)
  ## pull yvar and redefine the data using the hot-encoded X
  yvar.names <- o.stump$yvar.names
  f <- as.formula(paste(yvar.names, "~."))
  data <- data.frame(o.stump$yvar, x)
  colnames(data) <- c(yvar.names, colnames(x))
  ## fast filtering
  filter1 <- filter.rfsgt(f, data, hcut=1)
  filterX <- filter.rfsgt(f, data[, c(yvar.names, filter1), drop=FALSE], hcut=3)
  ## keep only linear terms and pairwise terms
  nms <- unique(c(filter1, filterX))
  nms <- nms[!grepl("sq2", nms)]
  dbl <- make.baselearner(data[, filter1, drop=FALSE], hcut=3, max.three=max.three)[, c(nms), drop=FALSE]
  dbl <- data.frame(data[, yvar.names, drop=FALSE], dbl)
  ## rfsgt: hereafter hcut=1 since we are using the base learner X features
  o <- rfsgt(f, dbl, hcut=1, tune=FALSE, filter=FALSE)
  ## get beta
  beta <- get.beta(o)
  beta.m <- colMeans(beta, na.rm=TRUE)
  names(beta.m) <- colnames(beta)
  ## partial effects: linear terms
  nms.l <- nms[!grepl("\\.", colnames(beta))]
  partial.l <-  t(t(dbl[, nms.l, drop=FALSE]) * beta.m[nms.l])
  ## partial effects: interaction terms
  nms.i <- strsplit(nms[grepl("\\.", nms)], "\\.")
  partial.i <- lapply(1:length(nms.i), function(i) {
    n1 <- nms.i[[i]][1]
    n2 <- nms.i[[i]][2]
    n12 <- paste0(n1, ".", n2)
    theta <- 0
    ## linear effect 1
    if (is.element(n1, colnames(partial.l))) {
      theta <- theta + partial.l[, n1]
    }
    ## linear effect 2
    if (is.element(n2, colnames(partial.l))) {
      theta <- theta + partial.l[, n2]
    }
    ## interaction effect 1-2
    theta <- theta + beta.m[n12]  * dbl[, n1] * dbl[, n2]
    ## return the effect
    theta
  })
  names(partial.i) <- sapply(nms.i, function(n) {paste(n, collapse=".", sep="")})
  ## return the goodies
  list(nms.l=nms.l, nms.i=nms.i, partial.l=partial.l, partial.i=partial.i)
}
