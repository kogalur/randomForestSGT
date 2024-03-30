get.beta <- function(o, bag=c("inbag", "oob", "all"), papply = mclapply) {
  ## pull the native array and beta values
  nativeArray <- o$forest$nativeArray
  beta <- nativeArray[, !(colnames(nativeArray) %in%
       c("treeID", "nodeID", "nodeSZ", "brnodeID", "yStar", "yBar")), drop = FALSE]
  ybar <- nativeArray[, "yBar"]
  ## pull the terminal node offsets by case id
  idx <- o$forest$ambrOffset
  ## set the bag type
  if (is.null(o$membership)) {
    bag <- "all"
  }
  ## iterate over trees extract the beta 
  blist <- papply(1:o$ntree, function(b) {
    cart <- !is.na(ybar[idx[, b]])    ## yBar!= NA--> CART split
    beta <- beta[idx[, b],, drop=FALSE]
    beta[cart, ] <- 0
    beta[cart, 1] <- ybar[idx[cart, b]]
    if (bag == "all") {
      beta
    }
    else if (bag == "inbag") {
      beta[o$inbag[, b] == 0,] <- NA
      beta
    }
    else {##oob
      beta[o$inbag[, b] != 0,] <- NA
      beta
    }
  })
  ## calculate the mean (list of matrices)
  beta <- matrixMeans(blist)
  colnames(beta) <- c("betaZ", o$xvar.names, o$xvar.augment.names) 
  rownames(beta) <- NULL
  ## build the x design matrix
  x <- cbind(1, o$xvar)
  if (!is.null(o$xvar.augment.names)) {
    x <- cbind(x, o$xvar.augment)
  }
  ## add useful attributes
  attr(beta, "lasso.percent") <- 100 * mean(is.na(ybar[c(idx)]))
  attr(beta, "predicted") <- rowSums(x * beta, na.rm=TRUE)
  attr(beta, "partial") <- x * beta
  ## return the beta
  beta 
}
## calculates mean of list of matrices with NA's
matrixMeans <- function(mm) {
  Reduce("+", mclapply(mm, function(x) replace(x, is.na(x), 0))) / Reduce("+", mclapply(mm, Negate(is.na)))
}
