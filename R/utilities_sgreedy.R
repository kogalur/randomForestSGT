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
###  When this bit is low, we check the betas at lambdaMinIndx, and
###  ensure they are not all zero.  If they are zero we fail the
###  master, otherwise we indicate success and use lambdaMinIndx as
###  target.  When the bit is high, we attempt the previous check,
###  but if the result is false, we check the betas at
###  lambda1SEIndxMin.  If the betas at this back-up value are
###  non-zero, we indicate success and use lambda1SEIndxMin as the
###  target.  Otherwise, if both checks fail, we fail the master.
###
###  experimental.bits = 2^8
###  When this bit is high, we use the OOB emperical risk in place of
###  the in-bag for determining tree growth.  Note that the split rule
###  always uses the in-bag subset, but in terms of best split first,
###  and all the eBits 2^2, ... 2^6, we use the OOB subset.
###
##########################################################################
## normalize the user-facing BSF option
get.bsf <- function(bsf = NULL, default = "inbag", warn = FALSE) {
  if (is.null(bsf)) {
    bsf <- default
  }
  if (!is.character(bsf) || length(bsf) != 1) {
    stop("bsf must be one of 'oob' or 'inbag'")
  }
  if (bsf == "ibg") {
    if (warn) {
      warning("bsf='ibg' is deprecated: use bsf='inbag'")
    }
    bsf <- "inbag"
  }
  if (!(bsf %in% c("oob", "inbag"))) {
    stop("bsf must be one of 'oob' or 'inbag'")
  }
  bsf
}
## additional experimental bit used for OOB empirical risk minimization
get.bsf.bits <- function(bsf = "inbag") {
  if (bsf == "oob") {
    as.integer(2^8)
  }
  else {
    as.integer(0)
  }
}
get.experimental.bits  <- function(experimental.bits, hcut) {
  if (!is.null(experimental.bits)) {
    ## zero --> 2^7
    if (experimental.bits == 0) {##pure lasso+lambda1SEIndxMin
        result <- 2^7
    }
    experimental.bits
  }
  else {
    if (hcut < 2) {
      result <- 2^6 + 2^3
    }
    else {
      #2^6 + 2^4#doesn't seem to be any advantage and reduces % lasso split
      result <- 2^4
    }
  }
  return (result)
}
get.basis.active.bits <- function(basis.active) {
    if (isTRUE(basis.active)) {
        result <- 2^9
    }
    else {
        result <- 0
    }
    return (result)
}
## option bit for support-restricted lasso terminal predictions
get.node.restrict.bits <- function(node.restrict) {
  if (!is.logical(node.restrict) || length(node.restrict) != 1L || is.na(node.restrict)) {
    stop("node.restrict must be TRUE or FALSE")
  }
  if (node.restrict) as.integer(2^10) else 0L
}
## serialize source values and retained terms for node-local basis construction
make.basis.payload <- function(xvar.source, term.map, term.names, hcutCnt,
                               node.center = TRUE) {
  if (!is.logical(node.center) || length(node.center) != 1L || is.na(node.center)) {
    stop("node.center must be TRUE or FALSE")
  }
  if (!is.term.map(term.map) || nrow(term.map) == 0L) {
    stop("term.map is required for the node basis")
  }
  if (nrow(term.map) != hcutCnt ||
      !identical(as.character(term.map$term), as.character(term.names))) {
    stop("term.map does not match the native beta count and order")
  }
  source.names <- colnames(xvar.source)
  if (is.null(source.names) || anyNA(source.names) ||
      any(!nzchar(source.names)) || anyDuplicated(source.names)) {
    stop("node basis source columns must have unique, nonempty names")
  }
  parent.list <- lapply(term.map$xvar.names, as.character)
  power.list <- term.map$xvar.power
  q <- lengths(parent.list, use.names = FALSE)
  if (any(q < 1L) ||
      !identical(q, lengths(power.list, use.names = FALSE)) ||
      !is.numeric(term.map$order) || anyNA(term.map$order) ||
      any(q != term.map$order)) {
    stop("term.map order, parent and power lengths disagree")
  }
  valid.power <- vapply(power.list, function(power) {
    (is.integer(power) || is.double(power)) &&
      all(is.finite(power) & power >= 1 & power == trunc(power) &
          power <= .Machine$integer.max)
  }, logical(1))
  if (!all(valid.power)) {
    stop("term.map parent powers must be positive integers")
  }
  degree <- vapply(power.list, sum, numeric(1))
  if (!is.numeric(term.map$degree) || anyNA(term.map$degree) ||
      any(degree != term.map$degree)) {
    stop("term.map degree disagrees with its parent powers")
  }
  parent.names <- unlist(parent.list, use.names = FALSE)
  if (anyNA(parent.names) || any(!nzchar(parent.names))) {
    stop("term.map parent names must be nonempty and nonmissing")
  }
  missing.parents <- setdiff(parent.names, source.names)
  if (length(missing.parents) > 0L) {
    stop("cannot construct the node basis; missing source columns: ",
         paste(missing.parents, collapse = ", "))
  }
  ## Keep the original hot-encoded source order, including filtered-out parents.
  basis.source.names <- source.names[source.names %in% parent.names]
  xvar.basis <- xvar.source[, basis.source.names, drop = FALSE]
  parent.index <- as.integer(match(parent.names, basis.source.names))
  parent.power <- as.integer(unlist(power.list, use.names = FALSE))
  term.offset <- c(1, 1 + cumsum(as.double(q)))
  if (tail(term.offset, 1L) > .Machine$integer.max) {
    stop("node basis parent count exceeds the native integer limit")
  }
  list(
    active = 1L,
    p.source = as.integer(ncol(xvar.basis)),
    xvar.basis = as.double(as.vector(data.matrix(xvar.basis))),
    p.term = as.integer(nrow(term.map)),
    term.offset = as.integer(term.offset),
    parent.index = parent.index,
    parent.power = parent.power,
    center.mask = rep.int(as.integer(node.center), ncol(xvar.basis))
  )
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
                    lag = NULL,
                    strikeout = NULL,
                    max.two   = NULL,
                    max.three = NULL,
                    max.four  = NULL) {
  if (is.null(treesize) || (!is.function(treesize) && !is.numeric(treesize))) {
    treesize <- function(n, hcut) {
      maxsize <- 1000
      if (hcut < 2) {
        min(maxsize, max(200, n / 5))
      }
      else {
        ## with the restricted score, 25 terminal nodes matched larger trees on
        ## average over twelve datasets and grow faster
        min(25, n / 5)
      }
    }
  }
  if (is.null(lag)) {
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
## Fast serial helper for interaction columns.  idx is the usual
## combn() output: k rows by number-of-combinations columns.
.sg.interaction.matrix <- function(xmat, idx, chunk.size = 5000L) {
  ncomb <- ncol(idx)
  out <- matrix(NA_real_, nrow = nrow(xmat), ncol = ncomb)
  if (ncomb == 0L) {
    return(out)
  }
  chunk.size <- max(1L, min(as.integer(chunk.size), ncomb))
  for (lo in seq.int(1L, ncomb, by = chunk.size)) {
    hi <- min(lo + chunk.size - 1L, ncomb)
    cols <- lo:hi
    block <- xmat[, idx[1L, cols], drop = FALSE]
    if (nrow(idx) > 1L) {
      for (j in 2L:nrow(idx)) {
        block <- block * xmat[, idx[j, cols], drop = FALSE]
      }
    }
    out[, cols] <- block
  }
  out
}
## Fast serial helper for names matching combn() column order.
.sg.interaction.names <- function(xnms,
                                  idx,
                                  pretty.names = TRUE,
                                  prefix = NULL,
                                  ichar = "_I_",
                                  legacy.literal.ichar = FALSE) {
  xnms <- as.character(xnms)
  parts <- lapply(seq_len(nrow(idx)), function(j) {
    xnms[idx[j, ]]
  })
  nm <- do.call(paste, c(parts, sep = ichar))
  if (!pretty.names) {
    ## legacy.literal.ichar preserves the original hcut = 6 naming
    ## behavior where the string "ichar" was pasted literally.
    sep.char <- ifelse(legacy.literal.ichar, "ichar", ichar)
    nm <- paste(prefix, sep.char, nm, sep = "")
  }
  nm
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
  ## Work with a numeric matrix internally.  Keep xmod construction as
  ## cbind(x, x^2) so the return class remains close to the original
  ## implementation: data.frame in, data.frame out; matrix in, matrix out.
  xmat <- as.matrix(x)
  p <- length(xnms)
  x2 <- xmat^2
  ## hcut = 2: + quadratic
  xmod <- cbind(x, x^2)
  xsqnms <- paste0(xnms, sqchar)
  colnames(xmod) <- c(xnms, xsqnms)
  ## Precompute pair interactions once.  These are used for hcut = 3 and
  ## reused for hcut = 5.
  two <- NULL
  two.idx <- NULL
  two.nms <- NULL
  ## hcut = 3: + pair interactions
  ## higher order interactions (2x) not allowed if p is too high
  if (p >= 2L && hcut >= 3L && p <= max.two) {
    two.idx <- combn(p, 2L)
    two <- .sg.interaction.matrix(xmat, two.idx)
    two.nms <- .sg.interaction.names(xnms,
                                      two.idx,
                                      pretty.names = pretty.names,
                                      prefix = "two",
                                      ichar = ichar)
    colnames(two) <- two.nms
    xmod <- cbind(xmod, two)
  }
  ## hcut = 4: + polynomials of degree 3 for all pairs of variables
  ## higher order interactions (2x) not allowed if p is too high
  if (p >= 2L && hcut >= 4L && p <= max.two) {
    poly3 <- matrix(NA_real_, nrow = nrow(xmat), ncol = p * p)
    for (j in seq_len(p)) {
      cols <- ((j - 1L) * p + 1L):(j * p)
      poly3[, cols] <- xmat[, j] * x2
    }
    nm <- paste(rep(paste0(xnms, ichar), each = p),
                rep(xsqnms, times = p),
                sep = "")
    colnames(poly3) <- nm
    xmod <- cbind(xmod, poly3)
  }
  ## hcut = 5: + polynomials of degree 4 for all three-tuple variables
  ## higher order interactions (2x) not allowed if p is too high
  if (p >= 2L && hcut >= 5L && p <= max.two) {
    if (is.null(two)) {
      two.idx <- combn(p, 2L)
      two <- .sg.interaction.matrix(xmat, two.idx)
      two.nms <- .sg.interaction.names(xnms,
                                        two.idx,
                                        pretty.names = pretty.names,
                                        prefix = "two",
                                        ichar = ichar)
      colnames(two) <- two.nms
    }
    ntwo <- ncol(two)
    poly4 <- matrix(NA_real_, nrow = nrow(xmat), ncol = p * ntwo)
    for (j in seq_len(p)) {
      cols <- ((j - 1L) * ntwo + 1L):(j * ntwo)
      poly4[, cols] <- x2[, j] * two
    }
    nm <- paste(rep(paste0(xsqnms, ichar), each = ntwo),
                rep(colnames(two), times = p),
                sep = "")
    colnames(poly4) <- nm
    xmod <- cbind(xmod, poly4)
  }
  ## hcut = 6: + three interactions
  ## higher order interactions (3x) not allowed if p is too high
  if (p >= 3L && hcut >= 6L && p <= max.three) {
    three.idx <- combn(p, 3L)
    three <- .sg.interaction.matrix(xmat, three.idx)
    nm <- .sg.interaction.names(xnms,
                                three.idx,
                                pretty.names = pretty.names,
                                prefix = "three",
                                ichar = ichar,
                                legacy.literal.ichar = TRUE)
    colnames(three) <- nm
    xmod <- cbind(xmod, three)
  }
  ## hcut = 7: + four interactions
  ## higher order interactions (4x) not allowed if p is too high
  if (p >= 4L && hcut >= 7L && p <= max.four) {
    four.idx <- combn(p, 4L)
    four <- .sg.interaction.matrix(xmat, four.idx)
    nm <- .sg.interaction.names(xnms,
                                four.idx,
                                pretty.names = pretty.names,
                                prefix = "four",
                                ichar = ichar)
    colnames(four) <- nm
    xmod <- cbind(xmod, four)
  }
  ## return the modified finalized x matrix - ie. the base learner matrix
  xmod
}
get.baselearner.dim <- function(p = 5, hcut.max = 5, ...) {
  x <- data.frame(matrix(rnorm(100 * p), ncol = p))
  setNames(sapply(1:hcut.max, function(h) {
    ncol(make.baselearner(x, hcut = h, ...))
  }), 1:hcut.max)
}
