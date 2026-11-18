plot.rfsgt <- function(x,
                       which = c("risk", "forest"),
                       treesize = c(1, 2, 3, 6, 12, 25),
                       lag = 8,
                       smooth = TRUE,
                       verbose = TRUE,
                       ...)
{
  ## check object is interpretable
  if (sum(inherits(x, c("rfsgt", "grow"), TRUE) == c(1, 2)) != 2) {
    stop("This function only works for objects of class `(rfsgt, grow)'")
  }
  which <- match.arg(which, c("risk", "forest"), several.ok = TRUE)
  ## ------------------------------------------------------------
  ##
  ## OOB empirical risk of each tree by tree size
  ##
  ## ------------------------------------------------------------
  ## running average of a tree's OOB empirical risk
  runavg <- function(r, lag) {
    r <- c(na.omit(r))
    if (length(r) < 2 || lag <= 1) {
      return(r)
    }
    lag <- min(lag, length(r))
    cr <- c(0, cumsum(r))
    rr <- cr[2:lag] / (1:(lag - 1))
    c(rr, (cr[(lag + 1):length(cr)] - cr[1:(length(cr) - lag)]) / lag)
  }
  risk <- NULL
  if ("risk" %in% which) {
    if (is.null(x$oob.empr.risk) || all(is.na(x$oob.empr.risk))) {
      warning("no OOB empirical risk to plot (trees with a single terminal node?)")
    }
    else {
      curves <- lapply(data.frame(x$oob.empr.risk), runavg, lag = max(1, lag))
      curves <- curves[sapply(curves, length) > 0]
      size <- unlist(lapply(curves, seq_along))
      value <- unlist(curves)
      ## average over the trees that reached each size; the largest sizes,
      ## reached by only a few trees, are dropped (at least a fifth of the trees)
      n.size <- tapply(value, size, length)
      avg <- tapply(value, size, mean)
      keep <- n.size >= min(length(curves), max(3, 0.2 * length(curves)))
      avg <- data.frame(size = as.numeric(names(avg))[keep], risk = avg[keep], n = n.size[keep])
      ## smooth of the average (loess, weighted by the number of trees);
      ## the lowest point is read off the smooth
      avg$smooth <- avg$risk
      if (smooth && nrow(avg) >= 8) {
        avg$smooth <- tryCatch(predict(loess(risk ~ size, data = avg, weights = avg$n, span = 0.4, degree = 2)),
                               error = function(e) avg$risk)
      }
      risk <- list(curves = curves, average = avg, lowest = avg$size[which.min(avg$smooth)])
    }
  }
  ## ------------------------------------------------------------
  ##
  ## forest OOB error by tree size
  ##
  ## ------------------------------------------------------------
  forest <- NULL
  if ("forest" %in% which) {
    get.oob.error <- function(o) {
      value <- tail(na.omit(o$err.rate), 1)
      if (length(value) != 1L || !is.finite(value)) {
        stop("Forest-size plots require a finite OOB error for every forest.", call. = FALSE)
      }
      value
    }
    fitted.error <- get.oob.error(x)
    ## the forests are grown exactly as x was: from the response and
    ## x-variables stored in x and a filter rebuilt from x$term.map (the
    ## basis terms of x, with their parent variables and powers), passed as
    ## a tune.hcut object, so the design is rebuilt term by term
    ## Keep source parents even when their main effects were filtered out.
    xvar <- if (x$hcut > 0L) x$xvar.basis else x$xvar
    if (is.null(xvar) || is.null(x$yvar)) {
      stop("x does not contain the source predictors and response needed to grow forests at other tree sizes")
    }
    data <- data.frame(x$yvar, xvar)
    colnames(data)[1] <- x$yvar.names
    if (is.term.map(x$term.map)) {
      filter <- as.character(x$term.map$term)
      attr(filter, "term.map") <- x$term.map
      attr(filter, "hcut") <- x$hcut
      attr(filter, "node.center") <- x$node.center
      attr(filter, "bsf") <- x$forest$bsf
      class(filter) <- "tune.hcut"
    }
    else {
      filter <- FALSE
    }
    ## the settings of x; arguments in ... take precedence
    args <- list(formula = as.formula(paste(x$yvar.names, "~ .")), data = data,
                 ntree = x$ntree, hcut = x$hcut, filter = filter,
                 nodesize = x$forest$nodesize, node.center = x$node.center, bsf = x$forest$bsf,
                 node.restrict = x$forest$node.restrict,
                 nfolds = x$splitinfo$nfolds, bootstrap = x$bootstrap, samptype = x$samptype,
                 sampsize = x$sampsize)
    if (x$bootstrap == "by.user") {
      args$samp <- x$samp
    }
    dots <- list(...)
    args[names(dots)] <- dots
    treesize <- sort(unique(round(treesize[is.finite(treesize) & treesize >= 1])))
    ## common random numbers: every forest is grown from the same seeds (R's
    ## generator, used when filtering, and the native one), so the
    ## differences between tree sizes are not blurred by run-to-run noise.
    ## The seed is drawn from R's generator (set.seed controls it), and R's
    ## generator is restored afterwards
    seed <- NULL
    if (is.null(dots$seed)) {
      seed <- sample.int(1e6, 1)
      if (exists(".Random.seed", envir = globalenv(), inherits = FALSE)) {
        rng.state <- get(".Random.seed", envir = globalenv(), inherits = FALSE)
        on.exit(assign(".Random.seed", rng.state, envir = globalenv()), add = TRUE)
      }
    }
    forest <- do.call(rbind, lapply(treesize, function(ts) {
      if (verbose) {
        cat("growing a forest with tree size", ts, "\n")
      }
      if (!is.null(seed)) {
        set.seed(seed)
        args$seed <- -seed
      }
      o <- do.call("rfsgt", c(args, list(treesize = ts)))
      data.frame(treesize = ts, leaves = mean(o$forest$leafCount, na.rm = TRUE),
                 oob.error = get.oob.error(o))
    }))
    ## the fitted forest at its average size
    forest <- rbind(forest, data.frame(treesize = NA, leaves = mean(x$forest$leafCount, na.rm = TRUE),
                                       oob.error = fitted.error))
    forest <- forest[order(forest$leaves), ]
    rownames(forest) <- NULL
  }
  ## ------------------------------------------------------------
  ##
  ## plots
  ##
  ## ------------------------------------------------------------
  nplot <- (!is.null(risk)) + (!is.null(forest))
  if (nplot == 0) {
    return(invisible(NULL))
  }
  if (nplot == 2) {
    oldpar <- par(mfrow = c(1, 2))
    on.exit(par(oldpar), add = TRUE)
  }
  if (!is.null(risk)) {
    ## a few erratic trees would stretch the axis: it covers 98% of the
    ## curve values and the average
    ylim <- range(quantile(unlist(risk$curves), c(0.01, 0.99), na.rm = TRUE), risk$average$risk)
    plot(c(1, max(sapply(risk$curves, length))), ylim, type = "n",
         xlab = "Tree size (terminal nodes)", ylab = "OOB empirical risk",
         main = "OOB empirical risk of each tree")
    for (r in risk$curves) {
      lines(seq_along(r), r, col = grey(0.8))
    }
    lines(risk$average$size, risk$average$risk, col = grey(0.3))
    lines(risk$average$size, risk$average$smooth, lwd = 3)
    best <- risk$average[which.min(risk$average$smooth), ]
    points(best$size, best$smooth, pch = 16, col = 2, cex = 1.4)
    ## (to the right of the point near the left edge, so the label stays inside)
    text(best$size, best$smooth, paste("lowest at", best$size), col = 2, xpd = TRUE,
         pos = if (best$size <= 0.15 * max(risk$average$size)) 4 else 3)
  }
  if (!is.null(forest)) {
    fitted <- is.na(forest$treesize)
    ## (the line joins the forests grown here, from common seeds; x was not)
    plot(forest$leaves, forest$oob.error, type = "n", log = "x", xaxt = "n",
         xlab = "Tree size (average terminal nodes)", ylab = "OOB error",
         main = "Forest OOB error by tree size")
    lines(forest$leaves[!fitted], forest$oob.error[!fitted], col = grey(0.5))
    mtext("red: the fitted forest", side = 3, line = 0.3, cex = 0.8, col = 2)
    axis(1, at = sort(unique(round(forest$leaves))))
    points(forest$leaves, forest$oob.error, pch = 21, cex = 1.4,
           bg = ifelse(fitted, 2, grey(0.75)))
    best <- forest[which.min(forest$oob.error), ]
    text(best$leaves, best$oob.error, paste("lowest:", signif(best$oob.error, 3)), col = 2, xpd = TRUE,
         pos = if (best$leaves == min(forest$leaves)) 4 else 3)
  }
  invisible(list(risk = risk, forest = forest))
}
