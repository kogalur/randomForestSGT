print.rfsgt <- function(x, ...) {
  ## check object is interpretable
  if (sum(inherits(x, c("rfsgt", "grow"), TRUE) == c(1, 2)) != 2 &
      sum(inherits(x, c("rfsgt", "predict"), TRUE) == c(1, 2)) != 2) {
    stop("This function only works for objects of class `(rfsgt, grow)' or '(rfsgt, predict)'")
  }
  ## error rate
  err.rate <- serr.rate <- r.squared <- NULL
  if (x$family == "regr" && !is.null(x$yvar)) {
    err.rate <- tail(x$err.rate, 1)
    var.y <- var(x$yvar, na.rm = TRUE)
    serr.rate <- err.rate / var.y
    r.squared <- 1 - err.rate / var.y
  }
  ## nodesize + dimension
  arr <- x$forest$nativeArray[x$forest$nativeArray$brnodeID==0,, drop = FALSE]
  nodeSZ <- mean(tapply(arr$nodeSZ, arr$treeID, mean, na.rm = TRUE), na.rm = TRUE)
  hcut.dim <- sum(!(colnames(arr) %in%
                  c("treeID", "nodeID", "nodeSZ", "brnodeID", "betaZ", "yStar", "yBar")))
  ## output 
  cat("                         Sample size: ", x$n,                                  "\n", sep="")
  cat("                    Tree sample size: ", x$sampsize,                           "\n", sep="")
  cat("                     Number of trees: ", x$ntree,                              "\n", sep="")
  cat("                   Average tree size: ", mean(x$forest$leafCount, na.rm=TRUE), "\n", sep="")
  cat("                   Average node size: ", nodeSZ,                               "\n", sep="")
  cat("              Total no. of variables: ", length(x$xvar.names),                 "\n", sep="")
  cat("                              Family: ", x$family,                             "\n", sep="")
  cat("                           Splitrule: ", x$splitrule,                          "\n", sep="")
  cat("                                hcut: ", x$hcut,                               "\n", sep="")
  cat("                      hcut-dimension: ", hcut.dim,                             "\n", sep="")
  if (!is.null(x$yvar)) {
    cat("                       OOB R-squared: ", r.squared,                            "\n", sep="")
    cat("         OOB standardized error rate: ", serr.rate,                            "\n", sep="")
    cat("                      OOB error rate: ", err.rate,                             "\n", sep="")
  }
}
