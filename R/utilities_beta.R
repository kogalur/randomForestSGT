get.beta <- function(o, bag = c("inbag", "oob", "all")[1], papply = lapply) {
  ## pull the native array and get beta and related values
  nativeArray <- o$forest$nativeArray
  beta <- nativeArray[, !(colnames(nativeArray) %in%
       c("treeID", "nodeID", "nodeSZ", "brnodeID", "yStar", "yBar", "prnodeID")), drop = FALSE]
  ybar <- nativeArray[, "yBar"]
  ystar <- nativeArray[, "yStar"]
  ## embedded split information for CART terminal nodes
  betaZ <- beta[, -1, drop=FALSE]
  betaZ.pt <- nativeArray$brnodeID == 0 & is.na(ystar) & !is.na(ybar)
  betaZ[!betaZ.pt, ] <- NA
  betaZ[betaZ.pt, ] <- ybar[betaZ.pt] * (betaZ[betaZ.pt, ] != 0)
  ## pull the terminal node offsets by case id
  idx <- o$forest$ambrOffset
  ## set the bag type
  if (is.null(o$membership)) {
    bag <- "all"
  }
  ## iterate over trees extract the beta - separate betaZ into those features that split the node
  blist <- papply(1:o$ntree, function(b) {
    cart <- !is.na(ybar[idx[, b]])    ## yBar!= NA--> CART split
    beta <- beta[idx[, b],, drop=FALSE]
    betaZ <- betaZ[idx[, b],, drop=FALSE]
    beta[cart, ] <- 0
    beta[cart, 1] <- ybar[idx[cart, b]]
    if (bag == "all") {
      list(beta=beta, betaZ=betaZ)
    }
    else if (bag == "inbag") {
      beta[o$inbag[, b] == 0,] <- NA
      betaZ[o$inbag[, b] == 0,] <- NA
      list(beta=beta, betaZ=betaZ)
    }
    else {##oob
      beta[o$inbag[, b] != 0,] <- NA
      betaZ[o$inbag[, b] != 0,] <- NA
      list(beta=beta, betaZ=betaZ)
    }
  })
  ## calculate the mean (list of matrices)
  beta <- matrixMeans(lapply(blist, "[[", 1))
  betaZ <- matrixMeans(lapply(blist, "[[", 2))
  colnames(beta) <- c("betaZ", o$xvar.names, o$xvar.augment.names)
  colnames(betaZ) <- c(o$xvar.names, o$xvar.augment.names) 
  rownames(beta) <- rownames(betaZ) <- NULL
  ## build the x design matrix
  x <- cbind(1, o$xvar)
  if (!is.null(o$xvar.augment.names)) {
    x <- cbind(x, o$xvar.augment)
  }
  ## add useful attributes
  lasso.percent <- 100 * mean(is.na(ystar[c(idx)]))
  predicted <- rowSums(x * beta, na.rm=TRUE)
  partial <- x * beta
  ## return the goodies
  list(beta=beta,
       betaZ=betaZ,
       lasso.percent=lasso.percent,
       predicted=predicted,
       partial=partial)
}
## calculates mean of list of matrices with NA's
matrixMeans <- function(mm) {
  Reduce("+", mclapply(mm, function(x) replace(x, is.na(x), 0))) / Reduce("+", mclapply(mm, Negate(is.na)))
}
