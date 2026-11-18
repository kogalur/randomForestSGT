##-------------------------------------------------------------------
##
## filter variables - calls the main function "vimp.rfsgt"
##
##-------------------------------------------------------------------
## custom filtering used by workhorse
filter.custom.rfsgt <- function(f, dta, hcut=1, method="liberal", fast=TRUE,
                                node.center=TRUE, ...) {
  if (!is.logical(node.center) || length(node.center) != 1L || is.na(node.center)) {
    stop("node.center must be TRUE or FALSE")
  }
  ## hidden options
  dots <- list(...)
  dots$node.center <- node.center
  dots$membership <- FALSE
  if (fast) {
    dots$ntree <- 1
  }
  ## hcut=1 call
  keep <- do.call("filter.rfsgt", c(list(f, dta, hcut=min(1, hcut), method="liberal"), dots))
  ## higher order hcut values
  if (hcut >= 2) {    
    keepXtra <- unique(unlist(lapply(2:hcut, function(j) {
      do.call("filter.rfsgt", c(list(f, dta, hcut=j, method=method), dots))
    })))
    term.map <- get.term.map(get.hotencode(get.stump(f, dta)$xvar), hcut=hcut)
    keepMain <- get.term.map.parents(term.map, keepXtra, type = "xvar.names")
    keepMain <- keepMain[keepMain %in% term.map$term[!term.map$is.augment]]
    keep <- unique(c(keep, keepMain, keepXtra))
  }
  ## return the goodies
  keep
}
## filter variables using varpro+sgt
filter.rfsgt <- function(f, dta, ntree=50, hcut=1, treesize=3, nfolds=5,
                         method = c("min", "conserve", "liberal")[3],
                         fast = TRUE, original = FALSE, 
                         maxit=100, eps=.05, names.only=TRUE,
                         node.center=TRUE, ...) {
  if (!is.logical(node.center) || length(node.center) != 1L || is.na(node.center)) {
    stop("node.center must be TRUE or FALSE")
  }
  ## hot-encode x and create the base learner x matrix
  stump <- get.stump(f, dta)
  x <- get.hotencode(stump$xvar)
  term.map <- get.term.map(x, hcut=hcut)
  dta <- data.frame(stump$yvar, make.baselearner(x, hcut=hcut))
  colnames(dta)[1] <- stump$yvar.names
  ##hidden options
  dots <- list(...)
  if (fast) {
    ntree <- 1
  }
  if (ntree == 1) {
    if (is.null(dots$imp)) {
      dots$omp <- "nfolds"
    }
    dots$bootstrap <- "none"
  }
  dots$membership <- FALSE
  dots$node.center <- node.center && hcut > 0L
  ## Obtain SGT split weights for the existing global varPro screen.
  ## Centered SGT fits must receive source columns and the actual hcut:
  ## completed raw powers/products cannot stand in for their parents.
  if (isTRUE(dots$node.center)) {
    source.data <- data.frame(stump$yvar, x)
    colnames(source.data) <- c(stump$yvar.names, colnames(x))
    dots$p <- nrow(term.map)
    source.formula <- as.formula(paste(stump$yvar.names, "~."))
    swt <- do.call("vimp.rfsgt", c(list(
      source.formula, source.data, ntree=ntree, hcut=hcut,
      treesize=treesize, nfolds=nfolds, maxit=maxit, eps=eps), dots))
  }
  else {
    ## Uncentered screening uses the prepared term columns.
    swt <- do.call("vimp.rfsgt", c(list(f, dta, ntree=ntree, hcut=1,
      treesize=treesize, nfolds=nfolds, maxit=maxit, eps=eps), dots))
  }
  ## varPro still screens the global raw term columns; node.center changes
  ## the SGT fits supplying its weights, not the varPro model definition.
  ## The native strength routine can return a C NULL pointer. R then
  ## warns, and varpro.strength stops. Defer only that warning while the
  ## existing fallback handles the failed screen; preserve other warnings.
  ## If cv.varpro nevertheless succeeds, re-signal the original warning.
  vmp <- NULL
  native.warnings <- list()
  checkfail <- tryCatch({
    capture.output(vmp <- withCallingHandlers(
      cv.varpro(f, dta, split.weight.custom=swt),
      warning = function(w) {
        cl <- conditionCall(w)
        if (identical(conditionMessage(w), "converting NULL pointer to R NULL") &&
            is.call(cl) && length(cl) >= 2L &&
            identical(cl[[1L]], quote(.Call)) &&
            identical(cl[[2L]], "varProStrength")) {
          native.warnings[[length(native.warnings) + 1L]] <<- w
          invokeRestart("muffleWarning")
        }
      }
    ))
  }, error=function(ex){NULL})
  ## return vimp.rfsgt if varpro fails or returns no result
  if (is.null(checkfail) || is.null(vmp)) {
    if (original) {
      attr(swt, "term.map") <- get.term.map.subset(term.map, names(swt))
      attr(swt, "xvar.org.names") <- stump$xvar.names
      swt <- get.orgvimp.rfsgt(swt)
    }
    if (sum(swt>0, na.rm=TRUE) > 0) {
      return(names(swt[swt>0]))
    }
    else {
      return(names(swt[order(swt, decreasing = TRUE)])[1])
    }
  }
  ## A successful CV result must not silently hide a native warning.
  for (w in native.warnings) {
    warning(w)
  }
  ## extract cv-varpro vimp
  vmp <- get.vimp(vmp, pretty=FALSE)
  vmp.names <- rownames(vmp)
  colnames(vmp) <- c("min", "conserve", "liberal")
  vmp <- vmp[, match(method, c("min", "conserve", "liberal"))]
  names(vmp) <- vmp.names
  if (original) {
    attr(vmp, "term.map") <- get.term.map.subset(term.map, vmp.names)
    attr(vmp, "xvar.org.names") <- stump$xvar.names
    attr(vmp, "xvar.names") <- vmp.names
    vmp <- get.orgvimp.rfsgt(vmp)
  }
  ## return selected variables
  if (sum(vmp>0, na.rm=TRUE) > 0) {
    keep <- vmp[vmp>0]
  }
  else {
    keep <- (vmp[order(vmp, decreasing=TRUE)])[1]
  }
  if (names.only) {
    names(keep)
  }
  else {
    keep
  }
}
##-------------------------------------------------------------------
##
## main function used for variable selection uses varpro + sgt
##
##-------------------------------------------------------------------
vimp.rfsgt <- function(f, dta, ntree=50, hcut=1, treesize=3,
                       nfolds=5, maxit=100, eps=.05, p=NULL,
                       node.center=TRUE, ...) {
  if (!is.logical(node.center) || length(node.center) != 1L || is.na(node.center)) {
    stop("node.center must be TRUE or FALSE")
  }
  ##-------------------------------------------------------------------
  ##
  ## initialize
  ##
  ##-------------------------------------------------------------------
  ## stump tree used to probe and clean the data
  stump <- get.stump(f, dta)
  ## extract x and  hotencode it 
  x <- get.hotencode(stump$xvar)
  term.map <- get.term.map(x, hcut=hcut)
  xvar.names <- term.map$term
  if (is.null(p)) {
    p <- ncol(x)
  }
  p <- min(p, length(xvar.names))
  ## rescale x,y and assemble the new data
  y <- stump$yvar
  ysd <- sd(y, na.rm = TRUE)
  y <- y / (1 * (ysd==0) + ysd)
  yvar.names <- stump$yvar.names
  dta <- data.frame(y, normalize(x))
  colnames(dta) <- c(yvar.names, colnames(x))
  ## Centered coefficients are fitted on products of normalized SOURCES.
  ## Put their screening magnitudes on fixed, comparable term scales.
  ## These are reference scales only; no node coefficients are translated
  ## or averaged as though they represented one common polynomial origin.
  if (isTRUE(node.center) && hcut > 0L) {
    reference.x <- make.baselearner.from.term.map(
      dta[, colnames(x), drop=FALSE], term.map)
    reference.x <- as.matrix(reference.x)
    reference.x <- sweep(reference.x, 2L, colMeans(reference.x), "-")
    beta.scale <- sqrt(colSums(reference.x^2))
    if (any(!is.finite(beta.scale))) {
      stop("nonfinite reference term scales in centered variable screening")
    }
  }
  ##-------------------------------------------------------------------
  ##
  ## hidden options
  ## enfore pure lasso with the over-riding 0 experimental bit
  ##
  ##-------------------------------------------------------------------
  dots <- list(...)
  dots$node.center <- node.center
  dots$tune <- dots$filter <- FALSE
  dots$perf.type <- "none"
  dots$experimental.bits <- as.integer(0)
  ##-------------------------------------------------------------------
  ##
  ## super greedy forest call 
  ##
  ##-------------------------------------------------------------------
  nativeArray <- do.call("rfsgt.workhorse", c(list(
                                 formula=f,
                                 data=dta,
                                 ntree=ntree, 
                                 hcut=hcut,
                                 treesize=treesize,
                                 nfolds=nfolds,
                                 maxit=maxit,
                                 eps=eps), dots))$forest$nativeArray
  ##-------------------------------------------------------------------
  ##
  ## get beta
  ##
  ##-------------------------------------------------------------------
  ## yBar!= NA--> CART split
  ar <- nativeArray[is.na(nativeArray$yBar),, drop = FALSE]
  ## remove everything but beta
  if (nrow(ar) > 0) {
    beta <- ar[, !(colnames(ar) %in%
      c("treeID", "nodeID", "nodeSZ", "brnodeID","betaZ", "yStar", "yBar", "prnodeID")), drop = FALSE]
    if (isTRUE(node.center) && hcut > 0L &&
        (ncol(beta) != length(xvar.names) ||
         !identical(colnames(beta), as.character(seq_along(xvar.names))))) {
      stop("native coefficient columns do not match the centered screening term map")
    }
    beta <- beta[, 1:p, drop = FALSE]
    if (isTRUE(node.center) && hcut > 0L) {
      beta <- sweep(as.matrix(beta), 2L, beta.scale[seq_len(p)], "*")
      beta[, beta.scale[seq_len(p)] == 0] <- 0
    }
    beta <- colMeans(abs(beta), na.rm = TRUE)
  }
  else {
    beta <- rep(NA, p)
  }
  names(beta) <- xvar.names[1:p]
  ##-------------------------------------------------------------------
  ##
  ## return beta with hidden attributes
  ##
  ##-------------------------------------------------------------------
  class(beta) <- "vimp.rfsgt"
  attr(beta, "term.map") <- get.term.map.subset(term.map, names(beta))
  attr(beta, "xvar.names") <- xvar.names
  attr(beta, "xvar.org.names") <- stump$xvar.names
  beta  
}
##----------------------------------------------------------------------
##
## custom print object for vimp.rfsgt to make attributes invisible
##
##----------------------------------------------------------------------
print.vimp.rfsgt <- function(x, ...) {
  attr(x, "class") <- attr(x, "xvar.names") <-
    attr(x, "xvar.org.names") <- attr(x, "term.map") <- NULL
  print(x)
}
##----------------------------------------------------------------------
##
## custom function for extracting beta from vimp.rfsgt
##
##----------------------------------------------------------------------
get.orgvimp.rfsgt <- function(beta, pretty=FALSE) {
  ## pull xvar names (original and hot-encoded)
  xvar.org.names <- attr(beta, "xvar.org.names")
  xvar.names <- attr(beta, "xvar.names")
  term.map <- attr(beta, "term.map")
  ## exact path using the term.map object
  if (is.term.map(term.map)) {
    term.map <- get.term.map.subset(term.map, names(beta))
    if (is.null(xvar.org.names)) {
      xvar.org.names <- unique(unlist(term.map$xvar.org.names, use.names = FALSE))
    }
    vars <- xvar.org.names[sapply(xvar.org.names, function(nn) {
      any(sapply(term.map$xvar.org.names, function(xx) {nn %in% xx}))
    })]
    vars.beta <- sapply(xvar.org.names, function(nn) {
      pt <- sapply(term.map$xvar.org.names, function(xx) {nn %in% xx})
      if (any(pt)) {
        if (!all(is.na(beta[term.map$term[pt]]))) {
          max(beta[term.map$term[pt]], na.rm = TRUE)
        }
        else {
          0
        }
      }
      else {
        0
      }
    })
    names(vars.beta) <- xvar.org.names
    if (pretty) {
      topvars <- data.frame(variable = vars, beta = vars.beta[vars])
      topvars[order(topvars$beta, decreasing = TRUE),, drop = FALSE]
    }
    else {
      vars.beta
    }
  }
  ## backward compatible fallback
  else {
    ## match original variable names to varpro names which uses hot encode data
    vars <- xvar.org.names[which(unlist(lapply(xvar.org.names, function(nn) {
      if (any(grepl(nn, names(beta)))) {
        TRUE
      }
      else {
        FALSE
      }
    })))]
    ## obtain beta for mapped variables
    vars.beta <- lapply(xvar.org.names, function(nn) {
      if (any((pt <- grepl(nn, names(beta))))) {
        if (!all(is.na(beta[pt]))) {
          max(beta[pt], na.rm = TRUE)
        }
        else {
          0
        }
      }
      else {
        NULL
      }
    })
    ## remove NULL entries
    vars.beta <- unlist(vars.beta[!sapply(vars.beta, is.null)])
    ## make nice table for return
    if (pretty) {
      topvars <- data.frame(variable = vars, beta = vars.beta)
      topvars[order(topvars$beta, decreasing = TRUE),, drop = FALSE]
    }
    ## return named vector with beta values (0 if not selected)
    else {
      beta <- rep(0, length(xvar.org.names))
      names(beta) <- xvar.org.names
      beta[vars] <- vars.beta
      beta
    }
  }
}
