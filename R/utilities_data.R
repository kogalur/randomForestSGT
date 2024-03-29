finalize.data <- function(fnames, data) {
  ## restrict data to the target variables
  data <- data[ , is.element(names(data), fnames), drop = FALSE]
  ## data conversion to numeric mode for factors
  ## no need for conversion if factors are not present
  factor.names <- unlist(lapply(data, is.factor))
  if (sum(factor.names) > 0) {
    data[, factor.names] <- data.matrix(data[, factor.names, drop = FALSE])
  }
  ## convert logicals to 0/1 real (bug reported by John Ehrlinger)
  logical.names <- unlist(lapply(data, is.logical))
  if (sum(logical.names) > 0) {
    data[, logical.names] <- 1 * data[, logical.names, drop = FALSE]
  }
  ## characters not allowed (bug reported by John Ehrlinger)
  character.names <- unlist(lapply(data, is.character))
  if (sum(character.names) > 0) {
    stop("data types cannot be character: please convert all characters to factors")
  }
  return (data)
}
finalize.formula <- function(formula.obj, data) {
    ## parse the formula object
    yvar.names <- formula.obj$yvar.names
    subj.names <- formula.obj$subj.names
    all.names  <- formula.obj$all.names
    fmly       <- formula.obj$family
    ytry       <- formula.obj$ytry
    index <- length(yvar.names)
    ## Adjust the index for the presence of subject names.
    if (fmly == "surv") {
        if (!is.null(subj.names)) {
            index <- index + 1
        }
    }
    ## total number of variables should exceed number of yvars
    if (length(all.names) <= index) {
        stop("formula is misspecified: total number of variables does not exceed total number of y-variables")
    }
    ## extract the xvar names
    if (all.names[index + 1] == ".") {
        if(index == 0) {
            xvar.names <- names(data)
        }
        else {
            xvar.names <- names(data)[!is.element(names(data), all.names[1:index])]
        }
    }
    else {
        if(index == 0) {
            xvar.names <- all.names
        }
        else {
            xvar.names <- all.names[-c(1:index)]
        }
        not.specified <- !is.element(xvar.names, names(data))
        if (sum(not.specified) > 0) {
            stop("formula is misspecified, object ", xvar.names[not.specified], " not found")
        }
    }
    ## return the goodies
  return (list(family=fmly,
               subj.names=subj.names,
               yvar.names=yvar.names,
               xvar.names=xvar.names,
               ytry=ytry))
}
get.numeric.levels <- function(fmly, nlevels, gvar) {
    gvar.numeric.levels  <- lapply(1:length(nlevels),
                                   function(nn) {if(nlevels[nn] > 0) unique(sort(gvar[, nn])) else NULL})
    ## Remove null elements in the list
    gvar.numeric.levels <- gvar.numeric.levels[!sapply(gvar.numeric.levels,is.null)]
    ## We are uncomfortable in sending a ist of length zero into the C-code, so we add an additional check.
    if (length(gvar.numeric.levels) == 0) gvar.numeric.levels = NULL
    gvar.numeric.levels
}
get.stump <- function(f, data) {
  rfsrc(f, data, mtry=1, splitrule="random", nodedepth=0,
                      perf.type="none", save.memory=TRUE, ntree=1)
}
get.yvar.nlevels <- function(fmly, nlevels, yvar.names, yvar) {
  if (fmly == "unsupv") {
    NULL
  }
  else {
    nlevels
  }
}
get.yvar.type <- function(fmly, generic.types, yvar.names) {
  if (fmly == "unsupv") {
    yvar.type <- NULL
  }
    else {
      if (grepl("surv", fmly)) {
          if (length(yvar.names) == 2) {
              yvar.type <- c("T", "S")
          }
          else {
              yvar.type <- c("t", "T", "S")
          }
      }
        else {
          yvar.type <- generic.types
        }
    }
  yvar.type
}
make.samplesize.function <- function(fraction = 1) {
  f <- paste("x * ", paste(eval(fraction)))
  expr <- parse(text = f)
  function(x) eval(expr, list(x = x))
}
normalize <- function(x) {
  x <- cbind(scale(x, center = colMeans(x), scale = FALSE))
  x.norm <- sqrt(colSums(x^2))
  x <- cbind(scale(x, center = FALSE, scale = x.norm))
  zero.norm <- x.norm == 0
  x[, zero.norm] <- 0
  x 
}
parse.formula <- function(f, data, ytry = NULL, coerce.factor = NULL) {
  ## confirm coherency of the formula
  if (!inherits(f, "formula")) {
    stop("'formula' is not a formula object.")
  }
  if (is.null(data)) {
    stop("'data' is missing.")
  }
  if (!is.data.frame(data)) {
    stop("'data' must be a data frame.")
  }
  ## pull the family and y-variable names
  fmly <- all.names(f, max.names = 1e7)[2]
  all.names <- all.vars(f, max.names = 1e7)
  yvar.names <- all.vars(formula(paste(as.character(f)[2], "~ .")), max.names = 1e7)
  yvar.names <- yvar.names[-length(yvar.names)]
  ## Default scenario, no subject information when family is not
  ## time dependent covariates.  Can be overridden later.
  subj.names <- NULL
  ## is coerce.factor at play for the y-outcomes?
  coerce.factor.org <- coerce.factor
  coerce.factor <- vector("list", 2)
  names(coerce.factor) <- c("xvar.names", "yvar.names")
  if (!is.null(coerce.factor.org)) {
    coerce.factor$yvar.names <- intersect(yvar.names, coerce.factor.org)
    if (length(coerce.factor$yvar.names) == 0) {
      coerce.factor$yvar.names <- NULL
    }
    coerce.factor$xvar.names <- intersect(setdiff(colnames(data), yvar.names), coerce.factor.org)
  }
  ## survival forests
  if (fmly == "Surv") {
    ## Survival and competing risk will have 2 slots, namely time and censoring.
    ## Time dependent covariates will have 4 slots, namely id, start, stop, and event.
    ## If TDC is in effect, we remove the id from the yvars, and tag is an the subject identifier.
    if ((sum(is.element(yvar.names, names(data))) != 2) &&
        (sum(is.element(yvar.names, names(data))) != 4)) {
      stop("Survival formula incorrectly specified.")
    }
    else {
      if (sum(is.element(yvar.names, names(data))) == 4) {
        ## Time dependent covariates is in effect.
        subj.names <- yvar.names[1]
        yvar.names <- yvar.names[-1]
      }
    }
    family <- "surv"
    ytry <- 0
  }
  ## multivariate forests
  else if ((fmly == "Multivar" || fmly == "cbind")  && length(yvar.names) > 1) {
    if (sum(is.element(yvar.names, names(data))) < length(yvar.names)) {
      stop("Multivariate formula incorrectly specified: y's listed in formula are not in data.")
    }
    ## determine the family: now handles mixed outcomes
    Y <- data[, yvar.names, drop = FALSE]
    ## convert logical to 0/1 real (bug reported by John Ehrlinger)
    logical.names <- unlist(lapply(Y, is.logical))
    if (sum(logical.names) > 0) {
      Y[, logical.names] <- 1 * Y[, logical.names, drop = FALSE]
    }
    ## are all the responses factors?
    ## caution: ordered factors are factors!
    if ((sum(unlist(lapply(Y, is.factor))) + 
         length(coerce.factor$yvar.names)) == length(yvar.names)) {
      family <- "class+"
    }
    ## are all the responses continuous?
    ## caution: ordered factors are factors!
    else if ((sum(unlist(lapply(Y, is.factor))) + 
              length(coerce.factor$yvar.names)) == 0) {
      family <- "regr+"
    }
    ## are the responses a combination of factors and continuous?
    ## caution: ordered factors are factors!
    else if (((sum(unlist(lapply(Y, is.factor))) +
               length(coerce.factor$yvar.names)) > 0) && 
             ((sum(unlist(lapply(Y, is.factor))) +
               length(coerce.factor$yvar.names)) < length(yvar.names))) {
      family <- "mix+"
    }
    ## failure
    else {
      stop("y-outcomes must be either real or factors in multivariate forests.")
    }
    if (!is.null(ytry)) {
      ## Check that incoming ytry is consistent.
      if ((ytry < 1) || (ytry > length(yvar.names))) {
        stop("invalid value for ytry:  ", ytry)
      }
    }
    else {
      ytry <- length(yvar.names)
    }
  }
  ## unsupervised forests
  else if (fmly == "Unsupervised") {
    ## unsupervised forests
    if (length(yvar.names) != 0) {
      stop("Unsupervised forests require no y-responses")
    }
    family <- "unsupv"
    yvar.names <- NULL
    ## Strip away the family from the formula, leaving ytry.
    temp <- gsub(fmly, "", as.character(f)[2])
    temp <- gsub("\\(|\\)", "", temp)
    ytry <- as.integer(temp)
    if (is.na(ytry)) {
      ytry <- 1
    }
    else {
      if (ytry <= 0) {
        stop("Unsupervised forests require positive ytry value")
      }
    }
  }
  ## univariate forests (regression or classification)
  else {
    ## must be a (univariate) regresssion or classification
    if (sum(is.element(yvar.names, names(data))) != 1) {
      stop("formula is incorrectly specified.")
    }
    Y <- data[, yvar.names]
    ## logicals are treated as 0/1 real (bug reported by John Ehrlinger)
    if (is.logical(Y)) {
      Y <- as.numeric(Y)
    }
    ## check whether we have a factor or a continuous variable
    if (!(is.factor(Y) | is.numeric(Y))) {
      stop("the y-outcome must be either real or a factor.")
    }
    if (is.factor(Y) || length(coerce.factor$yvar.names) == 1) {
      family <- "class"
    }
    else {
      family <- "regr"
    }
    ytry <- 1
  }
  ## done: return the goodies
  return (list(all.names=all.names, family=family, subj.names=subj.names, yvar.names=yvar.names, ytry=ytry,
               coerce.factor = coerce.factor))
}
