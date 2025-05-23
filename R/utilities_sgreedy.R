##########################################################################
### experimental bits
###
### The following are mutually exclusive, and you must set one and only one bit.
###
### experimental.bits = 0
### This is pure lasso - the original implementation - good for variable selection but not prediction
### 
### experimental.bits = 2^2
### This gets you Eqn. 4 in the note. The virtual split to a cell A is only allowed to switch from lasso to cart daughter predictors when (4) is met.
###
### experimental.bits = 2^3
### This gets you the "more flexible strategy right after Eqn. 4.
### We can call this Eqn. 4a. One, both, or no daughters are switched from lasso to cart predictors in this scenario.
###
### experimental.bits = 2^4
### This gets you Section 3 instead of Section 2.
### More specifically it gets you Eqn. 6 instead of Eqn. 4.  We switch from a lasso split to a cart split if Eqn. 6 is satisfied.
### We can do this because we always test both cart and lasso virtual splits at every node. It gives us a lot of flexibility.
###
### experimental.bits = 2^5.
### Finally, in rare cases, you want the root node to start with a cart predictor and stay a cart tree.
### This is not mutually exclusive of the other experimental bits.
###
### experimental.bits = 2^6
### This will swap out the lasso predictor with a cart predictor for terminal nodes only, if the cart risk is less than the lasso risk.
### This is not mutually exclusive of the other experimental bits.
###
###  experimental.bits = 2^7
###
###  When this bit is low, we check the betas at lambdaMinIndx, and
###  ensure they are not all zero.  If they are zero we fail the
###  master, otherwise we indicate success and use lambdaMinIndx as
###  target.  When the bit is high, we attempt the previous check,
###  but if the result is false, we check the betas at
###  lambda1SEIndxMin.  If the betas at this back-up value are
###  non-zero, we indicate success and use lambda1SEIndxMin as the
###  target.  Otherwise, if both checks fail, we fail the master.
##########################################################################
get.experimental.bits  <- function(experimental.bits, hcut) {
  if (!is.null(experimental.bits)) {
    ## zero --> 2^7
    if (experimental.bits == 0) {##pure lasso+lambda1SEIndxMin
      as.integer(2^7)
    }
    experimental.bits
  }
  else {
    if (hcut < 2) {
      2^6 + 2^3
    }
    else {
      #2^6 + 2^4#doesn't seem to be any advantage and reduces % lasso split
      2^4
    }
  }
}
get.lasso <- function(user.option, n, n.xvar) {
  if (is.null(user.option$nlambda)) {
    weights <- NULL
  }
  else {
    weights <- user.option$weights
  }
  if (is.null(user.option$nlambda)) {
    nlambda <- 100
  }
  else {
    nlambda <- user.option$nlambda
  }
  if (is.null(user.option$lambda.min.ratio)) {
    lambda.min.ratio <- ifelse(n < n.xvar, 0.01, 1e-04)
  }
  else {
    lambda.min.ratio <- user.option$lambda.min.ratio
  }
  if (is.null(user.option$lambda)) {
    lambda <- NULL
  }
  else {
    lambda <- user.option$lambda
  }
  if (is.null(user.option$threshold)) {
    threshold <- 1e-7
  }
  else {
    threshold <- user.option$threshold
  }
  if (is.null(user.option$efficiency)) {
    efficiency <- ifelse(n.xvar < 500, "covariance", "naive")
  }
  else {
    efficiency <- user.option$efficiency
  }
  list(weights = weights,
       nlambda = nlambda,
       lambda.min.ratio = lambda.min.ratio,
       lambda = lambda,
       threshold = threshold,
       efficiency = efficiency)
}
get.lot <- function(hcut = 1,
                    treesize = NULL,
                    tune = TRUE,
                    lag = NULL,
                    strikeout = NULL,
                    max.two   = NULL,
                    max.three = NULL,
                    max.four  = NULL) {
  if (is.null(treesize) || (!is.function(treesize) && !is.numeric(treesize))) {
    if (tune) {
      treesize <- function(n, hcut) {
        if (hcut > 1) {
          min(100, n / 5)
        }
        else {
          max(100, n / 5)
        }
      }
    }
    else {
      treesize <- function(n, hcut) {
        maxsize <- 1000
        min(maxsize, max(200, n / 5))
      }
    }
  }
  if (is.null(lag)) {
    lag <- 50
  }
  if (tune == FALSE) {
    lag <- 0
  }
  if (is.null(strikeout)) {
    strikeout <- 7
  }
  if (is.null(max.two)) {
    max.two <- 200
  }
  if (is.null(max.three)) {
    max.three <- 60
  }
  if (is.null(max.four)) {
    max.four <- 30
  }
  list(hcut = as.integer(hcut),  
       treesize = ifelse(is.numeric(treesize), as.integer(treesize), treesize),
       lag = as.integer(lag),
       strikeout = as.integer(strikeout),
       max.two = as.integer(max.two),
       max.three = as.integer(max.three),
       max.four = as.integer(max.four)
       )
}
get.mtry <- function(fmly, n.xvar, mtry) {
  if (!is.null(mtry)) {
    mtry <- round(mtry)
    if (mtry < 1 | mtry > n.xvar) {
      mtry <- max(1, min(mtry, n.xvar))
    }
  }
  else {
    if (fmly == "regr") {
      mtry <- max(ceiling(n.xvar/3), 1)
    }
    else {
      mtry <- max(ceiling(sqrt(n.xvar)), 1)
    }
  }
  mtry
}
get.nodesize <- function(fmly, nodesize) {
  ## Default node size for classification
  if (fmly == "class") {
    if (is.null(nodesize)) {
      nodesize <- 1
    }
  }
  ## Default node size for regression
  else if (fmly == "regr") {
    if (is.null(nodesize)) {
      nodesize <- 5
    }
  }
  ## Default node size for right-censored survival
  else if (fmly == "surv"){
    if (is.null(nodesize)) {
      nodesize <- 15
    }
  }
  ## The family is misspecified
  else if (is.null(nodesize)) {
    stop("family is misspecified")
  }
  ## Nodesize should be rounded if non-integer
  nodesize <- round(nodesize)
}
get.omp.bits  <- function(omp) {
  if (is.null(omp)) {
    ## default is openMP along ntree
    omp.bits  <- 1
  }
  ## valid values are c("ntree", "nfolds")
  else if (omp == "nfolds") {
    ## openMP along nfolds
    omp.bits  <- 0
  }
  else if (omp == "ntree") {
    ## openMP along ntree
    omp.bits  <- 1
  }
  else {
    ## default is openMP along ntree
    omp.bits  <- 1
  }
  omp.bits
}
get.splitinfo <- function(formula.detail, splitrule, hcut, nsplit, nfolds) {
  ## CAUTION: HARD CODED ON NATIVE SIDE
  splitrule.names <- c("sg.regr",             ##  1
                       "sg.class",            ##  2
                       "cart.regr",           ##  3
                       "cart.class",          ##  4
                       "cart.random",         ##  5
                       "sg.tdc")              ##  6  
  ## set the family
  fmly <- formula.detail$family
  ## set hcut
  hcut <- max(0, as.integer(hcut))
  ## if splitrule is present, confirm it is correctly set
  if (!is.null(splitrule)) {
    splitrule <- match.arg(splitrule, splitrule.names)
    ## cart splitting --> hcut is zero
    if (grepl("cart", splitrule) || (splitrule == "sg.tdc")) {
      hcut <- 0
    }
  }
  else {
    if (hcut == 0) {
      if (fmly == "class") {
        splitrule <- "cart.class"
      }
      if (fmly == "regr") {
        splitrule <- "cart.regr"
      }
    }
  }
  ## survival tdc --> 
  if (fmly == "surv") {
    if (is.null(splitrule)) {
      splitrule.idx <- which(splitrule.names == "sg.tdc")
      splitrule <- splitrule.names[splitrule.idx]
    }
    else {
      splitrule.idx <- which(splitrule.names == splitrule)
      if (length(splitrule.idx) != 1) {
        stop("invalid split rule specified for survival tdc:  ", splitrule)
      }
      if (splitrule != "sg.tdc") {
        stop("invalid split rule specified for regression:  ", splitrule)
      }
    }
  }
  ## class --> not currently implemented
  if (fmly == "class") {
    if (is.null(splitrule)) {
      splitrule.idx <- which(splitrule.names == "sg.class")
      splitrule <- splitrule.names[splitrule.idx]
    }
    else {
      splitrule.idx <- which(splitrule.names == splitrule)
      if (length(splitrule.idx) != 1) {
        stop("invalid split rule specified for classification:  ", splitrule)
      }
      if (!grepl("class", splitrule)) {
        stop("invalid split rule specified for classification:  ", splitrule)
      }
    }
  }
  ## regression --> default is sg.regr
  if (fmly == "regr") {
    if (is.null(splitrule)) {
      splitrule.idx <- which(splitrule.names == "sg.regr")
      splitrule <- splitrule.names[splitrule.idx]
    }
    else {
      splitrule.idx <- which(splitrule.names == splitrule)
      if (length(splitrule.idx) != 1) {
        stop("invalid split rule specified for regression:  ", splitrule)
      }
      if (!grepl("regr", splitrule)) {
        stop("invalid split rule specified for regression:  ", splitrule)
      }
    }
  }
  ## set nsplit
  if(!is.null(nsplit)) {
    nsplit <- round(nsplit)    
    if (nsplit < 0) {
      stop("invalid nsplit value: set nsplit >= 0")
    }
  }
  else {
    ## pure random
    if (splitrule == "cart.random") {
      nsplit <- 1
    }
    ## fast families -- revert back to 10 due to factors
    else if ((splitrule == "cart.regr")  | (splitrule == "cart.class")) {  
      nsplit <- 10
    }
    else {## currrently ignored
      nsplit <- 10
    }
  }
  ## set nfolds
  if (hcut == 0) {
    nfolds <- 0
  }
  list(splitrule = splitrule,
       index = splitrule.idx,
       hcut = hcut,
       nsplit = nsplit,
       nfolds = nfolds)
}
make.baselearner <- function (x,
                              hcut = 2,
                              max.two = 200,
                              max.three = 60,
                              max.four = 30,
                              pretty.names = TRUE,
                              standardize = FALSE,
                              sqchar = "_SQ2_",
                              ichar = "_I_") {
  ## colnames for x
  if (!pretty.names) {
    xnms <- 1:ncol(x)
  }
  else {
    xnms <- colnames(x)
  }
  ## rescale x?
  if (standardize) {
    x <- data.frame(scale(x, center=FALSE, scale=TRUE))
  }
  ## set hcut correctly
  hcut <- max(1, as.integer(hcut))
  ## hcut = 1: hyperplane
  if (hcut == 1) {
    return(x)
  }
  ## hcut = 2: + quadratic
  xmod <- cbind(x, x^2)
  xsqnms <- paste0(xnms, sqchar)
  colnames(xmod) <- c(xnms, xsqnms) 
  ## hcut = 3: + pair interactions
  ## higher order interactions (2x) not allowed if p is too high
  if (length(xnms) >= 2 && hcut >= 3 && length(xnms) <= max.two) {
    two <- do.call(cbind, lapply(combn(xnms, 2, simplify = FALSE), function(j) {
      x[, j[1]] * x[, j[2]]
    }))
    nm <- unlist(lapply(combn(xnms, 2, simplify = FALSE), function(j) {
      if (!pretty.names) {
        paste("two", ichar, paste(j, collapse = ichar), sep = "")
      }
      else {
        paste(j, collapse = ichar)
      }
    }))
    colnames(two) <- nm
    xmod <- cbind(xmod, two)
  }
  ## hcut = 4: + polynomials of degree 3 for all pairs of variables
  ## higher order interactions (2x) not allowed if p is too high
  if (length(xnms) >= 2 && hcut >= 4 && length(xnms) <= max.two) {
    poly3 <- do.call(cbind, lapply(1:length(xnms), function(j) {
      xmod[, xnms[j]] * xmod[,  xsqnms]
    }))
    nm <- unlist(lapply(1:length(xnms), function(j) {
      paste(paste0(xnms[j], ichar), xsqnms, sep="")
    }))
    colnames(poly3) <- nm
    xmod <- cbind(xmod, poly3)
  }
  ## hcut = 5: + polynomials of degree 4 for all three-tuple variables
  ## higher order interactions (2x) not allowed if p is too high
  if (length(xnms) >= 2 && hcut >= 5 && length(xnms) <= max.two) {
    poly4 <- do.call(cbind, lapply(1:length(xsqnms), function(j) {
      xmod[, xsqnms[j]] * xmod[, colnames(two)]
    }))
    nm <- unlist(lapply(1:length(xsqnms), function(j) {
      paste(paste0(xsqnms[j], ichar), colnames(two), sep="")
    }))
    colnames(poly4) <- nm
    xmod <- cbind(xmod, poly4)
  }
  ## hcut = 6: + three interactions
  ## higher order interactions (3x) not allowed if p is too high
  if (length(xnms) >= 3 && hcut >= 6 && length(xnms) <= max.three) {
    three <- do.call(cbind, mclapply(combn(xnms, 3, simplify = FALSE), function(j) {
      x[, j[1]] * x[, j[2]] * x[, j[3]]
    }))
    nm <- unlist(mclapply(combn(xnms, 3, simplify = FALSE), function(j) {
      if (!pretty.names) {
        paste("three", "ichar", paste(j, collapse = ichar), sep = "")
      }
      else {
        paste(j, collapse = ichar)
      }
    }))
    colnames(three) <- nm
    xmod <- cbind(xmod, three)
  }
  ## hcut = 7: + four interactions
  ## higher order interactions (4x) not allowed if p is too high
  if (length(xnms) >= 4 && hcut >= 7 && length(xnms) <= max.four) {
    four <- do.call(cbind, mclapply(combn(xnms, 4, simplify = FALSE), function(j) {
      x[, j[1]] * x[, j[2]] * x[, j[3]] * x[, j[4]]
    }))
    nm <- unlist(mclapply(combn(xnms, 4, simplify = FALSE), function(j) {
      if (!pretty.names) {
        paste("four", ichar, paste(j, collapse = ichar), sep = "")
      }
      else {
        paste(j, collapse = ichar)
      }
    }))
    colnames(four) <- nm
    xmod <- cbind(xmod, four)
  }
  ## return the modified finalized x matrix - ie. the base learner matrix
  xmod
}
