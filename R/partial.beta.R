##########################################################################################
##
## partial.beta and importance.beta: two helpers for the value of get.beta
## Revision 2026-11-11.8
##
## Both helpers take the value of get.beta (b) and the object that was passed to get.beta
## (o), and both use the partial effects of the original variables.  Neither uses the
## forest again.
##
## Partial effect.  A variable can enter several dictionary terms: its linear term, its
## powers, and its products with other variables.  Its partial effect collects all of
## them.  For an observation with local coefficients beta (its row of get.beta's beta),
## the partial effect of the variable x is the change in the value of the local
## polynomial when x is changed from a reference to the value the observation has, while
## the other variables stay at the values the observation has:
##
##     effect = sum over the terms that involve x of
##              coefficient * (term at the observed x  -  term at the reference).
##
## The reference is the term with x at its mean.  For b1 x + b11 x^2 + b12 x z the effect
## is b1 (x - m) + b11 (x^2 - m^2) + b12 (x - m) z, with m the mean of x.  Written in the
## variable centered at its mean, this sets to zero the coefficient of every term that
## contains the variable.  The terms that do not involve x cancel, and the effect does
## not depend on where the origin of any variable lies.
##
## A binary variable (a variable with two values, or the indicator columns of a factor) is
## treated differently.  Its mean is not a value it takes, and a power of it is the same
## function of its two values written another way.  For such a variable the reference is
## the average of the term over its values.
##
## A factor that was hot encoded is treated as one variable: its effect collects the terms
## of all its indicator columns.
##
## Terms are identified from the column names of beta: X1_SQ2_ is the square of X1 and
## X1_I_X2 is the product of X1 and X2.
##
## partial.beta      plots the partial effect of each variable against the variable
## importance.beta   compares the partial effect of each variable with the response
##
##########################################################################################
## Partial effects of the original variables from the value of get.beta (internal).
## Returns the effects as a named list with one vector per variable that has a column in
## the forest, the encoded variables, the encoded columns of every variable, the hot
## encoding, and the names of all original variables (those the filter removed included).
get.partial.effect <- function(b, o) {
  ## the local coefficients and the encoded variables they multiply
  if (!is.list(b) || !is.matrix(b$beta) || is.null(colnames(b$beta))) {
    stop("b must be the value of get.beta")
  }
  x <- o$xvar.basis
  if (is.null(x)) {
    x <- o$xvar
  }
  if (is.null(x) || nrow(x) != nrow(b$beta)) {
    stop("o must be the object that was passed to get.beta")
  }
  x <- as.data.frame(x)
  beta <- b$beta[, colnames(b$beta) != "betaZ", drop = FALSE]
  n <- nrow(beta)
  ## the variables and powers of every term
  term <- lapply(strsplit(colnames(beta), "_I_", fixed = TRUE), function(part) {
    square <- grepl("_SQ2_$", part)
    variable <- sub("_SQ2_$", "", part)
    power <- tapply(ifelse(square, 2, 1), variable, sum)
    list(variable = names(power), power = as.numeric(power))
  })
  usable <- vapply(term, function(t) all(t$variable %in% colnames(x)), logical(1))
  ## the encoded columns of every original variable (a factor has several)
  hot <- attr(o$xvar, "hotencode")
  map <- if (is.null(hot$map)) as.list(setNames(colnames(x), colnames(x))) else hot$map
  map <- lapply(map, function(columns) intersect(columns, colnames(x)))
  all.names <- names(map)
  map <- map[lengths(map) > 0L]
  ## partial effect of an original variable: every term that involves one of its columns,
  ## at the observed values minus the same term at the reference of the variable, with the
  ## other variables at their observed values.  The reference is the term at the mean of
  ## the variable.  For a binary variable (two values, or the indicator columns of a
  ## factor) it is the average of the term over the values of the variable
  effect.of <- function(v) {
    columns <- map[[v]]
    binary <- all(vapply(columns, function(column) length(unique(x[[column]])) <= 2L, logical(1)))
    at <- colMeans(x[, columns, drop = FALSE], na.rm = TRUE)
    involved <- which(usable & vapply(term, function(t) any(t$variable %in% columns), logical(1)))
    e <- numeric(n)
    for (j in involved) {
      own <- other <- rep(1, n)
      fixed <- 1
      for (q in seq_along(term[[j]]$variable)) {
        column <- term[[j]]$variable[q]
        value <- x[[column]]^term[[j]]$power[q]
        if (column %in% columns) {
          own <- own * value
          fixed <- fixed * at[[column]]^term[[j]]$power[q]
        }
        else {
          other <- other * value
        }
      }
      if (binary) {
        fixed <- mean(own, na.rm = TRUE)
      }
      e <- e + beta[, j] * other * (own - fixed)
    }
    e
  }
  effect <- lapply(names(map), effect.of)
  names(effect) <- names(map)
  list(effect = effect, x = x, map = map, hot = hot, all.names = all.names, n = n)
}
##########################################################################################
##
## partial.beta: partial effect plots of the original variables
##
## The function plots the effect against x, one panel per variable, with a smooth curve.
## Because the coefficients are local and because of the products with other variables,
## the points form a band around the curve and not a single line.  A factor is shown by
## boxplots by level.
##
## With by = w the observations are divided into up to three groups of a second variable
## w (its values when w has at most three, thirds of its distribution otherwise) and each
## group has its own curve.  Curves that differ in slope or shape show an interaction
## between the variable and w.
##
## b            value of get.beta (with or without relaxed = TRUE)
## o            the object passed to get.beta (grow or prediction object); it holds the
##              values of the variables
## xvar.names   original variables to plot.  The default is every variable with a nonzero
##              partial effect, ordered by the standard deviation of its effect, largest
##              first (at most max.plots of them)
## by           optional second variable defining the groups
## granule      a variable with at most this many distinct values is shown by boxplots
##              (group means joined by lines when by is used)
## same.scale   use one vertical scale for all panels?  The panels then show which
##              variables have the larger effects.  The scale covers the central 99
##              percent of the effects
## max.plots    largest number of panels when xvar.names is not given
## ...          graphical parameters passed to plot
##
## Returns, invisibly, a list with effect (matrix of partial effects, one column per
## plotted variable), spread (standard deviation of each effect, in the units of the
## response), and group (the groups of by, or NULL).
##
##########################################################################################
partial.beta <- function(b, o, xvar.names = NULL, by = NULL, granule = 5,
                         same.scale = TRUE, max.plots = 12, ...) {
  ## partial effect of every variable of the forest
  pe <- get.partial.effect(b, o)
  x <- pe$x
  map <- pe$map
  hot <- pe$hot
  every <- pe$effect
  ## the value of an original variable for the horizontal axis: the variable itself, or
  ## its level for a factor.  A factor with two levels is one 0/1 column; a factor with
  ## more levels is one indicator column per level, of which the filter may have kept
  ## only some, and the observations at the other levels are shown together as "other"
  value.of <- function(v) {
    columns <- map[[v]]
    if (length(hot$map[[v]]) <= 1L) {
      value <- x[[columns]]
      if (length(hot$levels[[v]]) == 2L) {
        value <- factor(value, levels = c(0, 1), labels = hot$levels[[v]])
      }
      return(value)
    }
    level <- ifelse(startsWith(columns, v), substring(columns, nchar(v) + 1L), columns)
    z <- as.matrix(x[, columns, drop = FALSE])
    which <- max.col(z, ties.method = "first")
    droplevels(factor(ifelse(rowSums(z) > 0, level[which], "other"), levels = c(level, "other")))
  }
  ## short labels for the distinct values of a discrete variable: three significant
  ## digits, or as many more as are needed to keep the labels distinct
  short <- function(u) {
    if (!is.numeric(u)) {
      return(as.character(u))
    }
    for (digits in 3:15) {
      label <- vapply(u, function(z) format(signif(z, digits)), character(1))
      if (!anyDuplicated(label)) {
        break
      }
    }
    label
  }
  as.levels <- function(value) {
    if (is.factor(value)) {
      return(droplevels(value))
    }
    u <- sort(unique(value))
    factor(value, levels = u, labels = short(u))
  }
  ## groups of the conditioning variable: its values when there are at most three,
  ## thirds of its distribution otherwise
  group <- NULL
  if (!is.null(by)) {
    if (length(by) != 1L || !(by %in% names(map))) {
      stop("by must name one variable of the forest")
    }
    w <- value.of(by)
    u <- sort(unique(w[!is.na(w)]))
    if (length(u) < 2L) {
      stop("by has a single value")
    }
    if (length(u) <= 3L) {
      group <- factor(w, levels = u, labels = paste(by, "=", short(u)))
    }
    else if (is.factor(w)) {
      stop("by must be numeric or have at most three values")
    }
    else {
      breaks <- unique(quantile(w, c(0, 1 / 3, 2 / 3, 1), na.rm = TRUE))
      if (length(breaks) < 4L) {
        breaks <- unique(quantile(w, c(0, 0.5, 1), na.rm = TRUE))
      }
      if (length(breaks) < 3L) {
        breaks <- c(u[1L], mean(u[1:2]), u[length(u)])
      }
      labels <- if (length(breaks) == 4L) c("low", "middle", "high") else c("low", "high")
      group <- cut(w, breaks, include.lowest = TRUE, labels = paste(labels, by))
    }
  }
  ## the variables to plot
  if (is.null(xvar.names)) {
    vars <- setdiff(names(map), by)
  }
  else {
    if (!all(xvar.names %in% names(map))) {
      stop("xvar.names not found: ", paste(setdiff(xvar.names, names(map)), collapse = ", "))
    }
    vars <- setdiff(xvar.names, by)
  }
  effect <- every[vars]
  spread <- vapply(effect, function(e) sd(e, na.rm = TRUE), numeric(1))
  keep <- is.finite(spread) & spread > 0
  if (!is.null(xvar.names) && any(!keep)) {
    warning("no partial effect for: ", paste(vars[!keep], collapse = ", "))
  }
  vars <- vars[keep]
  if (length(vars) == 0L) {
    stop("no variable has a nonzero partial effect")
  }
  if (is.null(xvar.names)) {
    vars <- vars[order(spread[vars], decreasing = TRUE)]
    if (length(vars) > max.plots) {
      message("showing the ", max.plots, " variables with the largest effects of ", length(vars),
              "; use xvar.names to choose others")
      vars <- vars[seq_len(max.plots)]
    }
  }
  effect <- effect[vars]
  spread <- spread[vars]
  ## layout and scales
  ink <- "grey20"
  series <- c("#2a78d6", "#eb6834", "#1baf7a")
  if (length(vars) > 1L) {
    nr <- floor(sqrt(length(vars)))
    nc <- ceiling(length(vars) / nr)
    op <- par(mfrow = c(nr, nc), mar = c(4, 4, 2.5, 1), mgp = c(2.4, 0.8, 0))
    on.exit(par(op))
  }
  ## vertical limits from the central 99 percent of the effects: a few extreme points
  ## must not flatten every panel
  central <- function(e) quantile(e, c(0.005, 0.995), na.rm = TRUE, names = FALSE)
  ylim.all <- range(vapply(effect, central, numeric(2)))
  ylab <- "partial effect"
  first <- TRUE
  for (v in vars) {
    e <- effect[[v]]
    xv <- value.of(v)
    ok <- is.finite(e) & !is.na(xv)
    if (!is.null(group)) {
      ok <- ok & !is.na(group)
    }
    ylim <- if (same.scale) ylim.all else central(e[ok])
    discrete <- is.factor(xv) || length(unique(xv[ok])) <= granule
    if (discrete && is.null(group)) {
      ## boxplots of the effect at each value of the variable
      boxplot(e[ok] ~ as.levels(xv[ok]), ylim = ylim, xlab = v, ylab = ylab, main = v,
                        col = "grey92", border = ink, outline = FALSE, frame.plot = FALSE, ...)
      abline(h = 0, col = "grey80")
    }
    else if (discrete) {
      ## mean effect at each value of the variable, one line per group
      xf <- as.levels(xv[ok])
      plot(c(0.5, nlevels(xf) + 0.5), ylim, type = "n", xaxt = "n", bty = "l",
                     xlab = v, ylab = ylab, main = v, ...)
      axis(1, at = seq_len(nlevels(xf)), labels = levels(xf))
      abline(h = 0, col = "grey80")
      for (g in seq_len(nlevels(group))) {
        pt <- as.integer(group[ok]) == g
        m <- tapply(e[ok][pt], xf[pt], mean)
        lines(seq_len(nlevels(xf)), m, col = series[g], lwd = 2)
        points(seq_len(nlevels(xf)), m, col = series[g], pch = 16, cex = 1.2)
      }
    }
    else {
      ## the effect against the variable, with a smooth curve (one per group)
      plot(xv[ok], e[ok], type = "n", ylim = ylim, bty = "l",
                     xlab = v, ylab = ylab, main = v, ...)
      abline(h = 0, col = "grey80")
      if (is.null(group)) {
        points(xv[ok], e[ok], pch = 16, cex = 0.45, col = "#00000030")
        lines(lowess(xv[ok], e[ok]), col = series[1L], lwd = 2)
      }
      else {
        for (g in seq_len(nlevels(group))) {
          pt <- ok & as.integer(group) == g
          points(xv[pt], e[pt], pch = 16, cex = 0.45, col = paste0(series[g], "40"))
        }
        for (g in seq_len(nlevels(group))) {
          pt <- ok & as.integer(group) == g
          if (sum(pt) > 2L) {
            lines(lowess(xv[pt], e[pt]), col = series[g], lwd = 2)
          }
        }
      }
    }
    if (first && !is.null(group)) {
      legend("topleft", legend = levels(group), col = series[seq_len(nlevels(group))],
                       lwd = 2, bty = "n", cex = 0.85, text.col = ink)
    }
    first <- FALSE
  }
  invisible(list(effect = do.call(cbind, effect), spread = spread, group = group))
}
##########################################################################################
##
## importance.beta: the partial effect of each variable compared with the response
##
## A side product of get.beta.  It describes the regression models of the terminal nodes
## that get.beta returns, and it is not an importance of the variable for the forest (see
## vimp.rfsgt for that).  With y the response, p the prediction reconstructed by get.beta,
## r = y - p the residual, e the partial effect of the variable, and y and e centered at
## their means,
##
##   marginal     mean((y - e)^2) / mean(y^2).  The partial effect of the variable alone,
##                compared with the response
##   conditional  1 - (mean((r + e)^2) - mean(r^2)) / mean(y^2).  One minus the increase
##                in the standardized error of the prediction when the partial effect is
##                removed from it.  Removing the partial effect sets to zero the
##                coefficients of the variable (centered at its mean) in the regression
##                model of the observation
##
## Both values equal 1 for a variable with no term in the forest and are smaller for a
## variable with a larger effect.  conditional equals marginal plus twice the covariance of
## e with the rest of the prediction, relative to the variance of y.  For variables whose
## effects are positively correlated with the rest of the prediction, marginal is the
## smaller of the two.
##
## The values use the terminal node models only.  They do not include what a variable does
## through the splits, and they approach 1 when the terminal coefficients are near zero
## (deep trees, a response with much noise), as the coefficients of get.beta and the plots
## of partial.beta do.
##
## b   value of get.beta with the default bag = "oob" on a grow object (the coefficients
##     of an observation then come from the trees that did not use it), or on a prediction
##     object for data with the response.  Not available for relaxed = TRUE, where the
##     least squares estimate of a node uses the response of the observation
## o   the object passed to get.beta
##
## Returns a data frame with one row per original variable, the variables the filter
## removed included, and the columns marginal and conditional.  The rows are ordered by
## marginal.
##
##########################################################################################
importance.beta <- function(b, o) {
  pe <- get.partial.effect(b, o)
  n <- pe$n
  if (!is.null(b$relaxed)) {
    stop("importance.beta is not available for relaxed coefficients, ",
         "which are estimated with the response of the observation")
  }
  y <- as.numeric(o$yvar)
  p <- as.numeric(b$predicted)
  if (length(y) != n || length(p) != n) {
    stop("importance.beta needs the response of every observation: ",
         "o must be a grow object, or a prediction object for data with the response")
  }
  ## the partial effect compared with the response (marginal) and with the response less
  ## the rest of the prediction (conditional)
  value <- t(vapply(pe$all.names, function(v) {
    e <- if (is.null(pe$effect[[v]])) numeric(n) else pe$effect[[v]]
    ok <- is.finite(e) & is.finite(y)
    dev <- y[ok] - mean(y[ok])
    e <- e[ok] - mean(e[ok])
    residual <- y[ok] - p[ok]
    c(marginal = mean((dev - e)^2) / mean(dev^2),
      conditional = 1 - (mean((residual + e)^2, na.rm = TRUE) -
                         mean(residual^2, na.rm = TRUE)) / mean(dev^2))
  }, numeric(2)))
  value <- as.data.frame(value)
  value[order(value$marginal, value$conditional), , drop = FALSE]
}
