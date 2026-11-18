##########################################################################################
##
## utilities_beta_relaxed.R
## Revision 2026-11-11.1
##
## Relaxed estimation of the local coefficients of a super greedy forest, for
## interpretation; the forest and its predictions are untouched.  The lasso selects the
## terms and least squares on the selected terms, over the out-of-bag cases of each
## terminal node, estimates the coefficients without the penalty (the relaxed lasso with
## the relaxation parameter at zero).  The OOB cases of a tree were routed to the node by
## splits that never used their responses, so the fit is free of the attenuation that
## affects the inbag fits.
##
## active = "global"  the dictionary the forest retained, the same terms in every node
## active = "local"   the terms with a nonzero coefficient in the node's own lasso fit
##
## A node is used when its OOB cases exceed the number of parameters by five; the other
## nodes contribute nothing, and the fraction used is returned.  A term that is a linear
## combination of the other active terms within a node (the square of a binary variable,
## for one) is aliased there: the pivoted QR drops it, its coefficient in that node is
## zero, and the fraction of used nodes in which each term was aliased is returned.  For
## a training case the node value is leave one out when the case is among the OOB cases
## that fit the node, which makes predicted an out-of-sample prediction of the relaxed
## polynomials; fitted holds the in sample values, which the contributions add up to.
##
##########################################################################################
get.beta.relaxed <- function(o, bag, active, min.extra = 5L) {
  if (!inherits(o, "grow") && !inherits(o, "predict")) {
    stop("o must be a grow or prediction object from rfsgt")
  }
  if (!isTRUE(o$hcut > 0)) {
    stop("relaxed estimation needs lasso terminal nodes (hcut > 0)")
  }
  newdata <- inherits(o, "predict") && !is.null(o$ambrOffset)
  forest <- o$forest
  trees <- if (inherits(o, "grow")) seq_len(forest$ntree) else o$get.tree
  if (is.null(trees) || length(trees) == 0L || anyNA(trees) || anyDuplicated(trees) ||
      any(trees < 1L | trees > forest$ntree | trees != trunc(trees))) {
    stop("invalid saved tree selection; call predict() again")
  }
  terms <- c(o$xvar.names, o$xvar.augment.names)
  k <- length(terms)
  if (k != forest$hcutCnt) {
    stop("retained terms do not match the saved coefficient order")
  }
  ## the retained dictionary in the original coordinates: evaluation rows for the
  ## contributions, training rows for the least squares estimates
  Xe <- cbind(1, as.matrix(o$xvar))
  if (!is.null(o$xvar.augment.names)) {
    Xe <- cbind(Xe, as.matrix(o$xvar.augment))
  }
  if (ncol(Xe) != k + 1L) stop("design columns do not match the retained terms")
  colnames(Xe) <- c("betaZ", terms)
  if (newdata) {
    ## the training rows of the dictionary are rebuilt from the training source data that
    ## predict.rfsgt keeps in the forest of a prediction on new data
    training <- get.beta.training(o)
    if (!is.term.map(o$term.map) ||
        !identical(as.character(o$term.map$term), as.character(terms))) {
      stop("retained terms do not match the saved term map")
    }
    Xt <- cbind(1, as.matrix(make.baselearner.from.term.map(training$source, o$term.map)))
    colnames(Xt) <- c("betaZ", terms)
    yt <- training$response
  }
  else {
    Xt <- Xe
    yt <- as.double(o$yvar)
  }
  if (length(yt) != nrow(Xt)) stop("training responses do not match the design rows")
  n <- nrow(Xe)
  idx <- if (newdata) o$ambrOffset else forest$ambrOffset
  if (!is.matrix(idx) || nrow(idx) != n || ncol(idx) != forest$ntree) {
    stop("terminal offsets do not match the evaluation rows and trees")
  }
  ## terminal nodes, their lasso coefficients, and their inbag and OOB members
  a <- forest$nativeArray
  leaves <- which(a$brnodeID == 0)
  lasso.leaf <- is.na(a$yStar[leaves]) & is.na(a$yBar[leaves])
  B <- as.matrix(a[leaves, as.character(seq_len(k)), drop = FALSE])
  members <- function(count, ident) {
    if (length(count) != length(leaves) || sum(count) != length(ident)) {
      stop("saved terminal membership counts and indices disagree")
    }
    off <- c(0, cumsum(as.double(count)))
    lapply(seq_along(leaves), function(s) unique(ident[off[s] + seq_len(count[s])]))
  }
  inbag <- members(forest$rmbrCount, forest$rmbrIdent)
  oob <- members(forest$ombrCount, forest$ombrIdent)
  node.slot <- integer(nrow(a))
  node.slot[leaves] <- seq_along(leaves)
  ## least squares in every node with enough OOB cases; the pivoted QR drops a term that
  ## is a linear combination of the others within the node (its coefficient is zero there)
  coef <- matrix(NA_real_, length(leaves), k + 1L)
  se2 <- matrix(0, length(leaves), k + 1L)
  aliased <- matrix(FALSE, length(leaves), k + 1L)
  loo <- vector("list", length(leaves))
  for (s in seq_along(leaves)) {
    m <- oob[[s]]
    ## the local active set of a CART terminal node is empty: its model is a constant
    cols <- if (active == "global") seq_len(k) else if (lasso.leaf[s]) which(B[s, ] != 0) else integer(0)
    cols <- c(1L, cols + 1L)
    if (length(m) < length(cols) + min.extra) next
    Z <- Xt[m, cols, drop = FALSE]
    dec <- qr(Z)
    r <- dec$rank
    if (r < 1L || length(m) <= r) next
    b <- qr.coef(dec, yt[m])
    dropped <- is.na(b)
    b[dropped] <- 0
    fitted <- qr.fitted(dec, yt[m])
    resid <- yt[m] - fitted
    Q <- qr.Q(dec)[, seq_len(r), drop = FALSE]
    h <- rowSums(Q^2)
    sigma2 <- sum(resid^2) / (length(m) - r)
    kept <- dec$pivot[seq_len(r)]
    R <- qr.R(dec)[seq_len(r), seq_len(r), drop = FALSE]
    variance <- numeric(ncol(Z))
    variance[kept] <- sigma2 * diag(chol2inv(R))
    coef[s, ] <- 0
    coef[s, cols] <- b
    se2[s, cols] <- variance
    aliased[s, cols] <- dropped
    ## leave one out values of the members; a member that alone determines a direction
    ## of the fit (leverage one) has none
    value <- ifelse(h < 1 - 1e-8, fitted - h * resid / (1 - h), NA_real_)
    loo[[s]] <- setNames(value, m)
  }
  used <- !is.na(coef[, 1L])
  ## average over the trees: for a training case the trees selected by bag, for new
  ## data every selected tree
  beta.sum <- partial.sum <- se2.sum <- matrix(0, n, k + 1L)
  predicted.sum <- fitted.sum <- count <- count.predicted <- numeric(n)
  for (b in trees) {
    nodes <- unique(idx[, b])
    nodes <- nodes[!is.na(nodes) & nodes > 0]
    for (node in nodes) {
      s <- node.slot[node]
      if (s == 0L || !used[s]) next
      rows <- which(idx[, b] == node)
      if (!newdata) {
        is.inbag <- rows %in% inbag[[s]]
        rows <- switch(bag, inbag = rows[is.inbag], oob = rows[!is.inbag], all = rows)
      }
      if (length(rows) == 0L) next
      cm <- matrix(coef[s, ], length(rows), k + 1L, byrow = TRUE)
      contribution <- Xe[rows, , drop = FALSE] * cm
      value <- rowSums(contribution)
      beta.sum[rows, ] <- beta.sum[rows, ] + cm
      se2.sum[rows, ] <- se2.sum[rows, ] + matrix(se2[s, ], length(rows), k + 1L, byrow = TRUE)
      partial.sum[rows, ] <- partial.sum[rows, ] + contribution
      fitted.sum[rows] <- fitted.sum[rows] + value
      count[rows] <- count[rows] + 1
      ## the honest value of a training case that helped fit the node
      if (!newdata) {
        hit <- match(rows, as.integer(names(loo[[s]])))
        value[!is.na(hit)] <- loo[[s]][hit[!is.na(hit)]]
      }
      ok <- !is.na(value)
      predicted.sum[rows[ok]] <- predicted.sum[rows[ok]] + value[ok]
      count.predicted[rows[ok]] <- count.predicted[rows[ok]] + 1
    }
  }
  count[count == 0] <- NA_real_
  count.predicted[count.predicted == 0] <- NA_real_
  beta <- beta.sum / count
  partial <- partial.sum / count
  se <- sqrt(se2.sum) / count
  colnames(beta) <- colnames(partial) <- colnames(se) <- c("betaZ", terms)
  rownames(beta) <- rownames(partial) <- rownames(se) <- NULL
  aliased <- if (any(used)) colMeans(aliased[used, , drop = FALSE]) else rep(NA_real_, k + 1L)
  names(aliased) <- c("betaZ", terms)
  list(beta = beta, betaZ = NULL,
       lasso.percent = 100 * mean(lasso.leaf),
       predicted = predicted.sum / count.predicted,
       partial = partial,
       response.adjustment = numeric(n),
       fitted = fitted.sum / count,
       se = se,
       nodes = as.integer(count),
       nodes.used = mean(used),
       aliased = aliased,
       relaxed = list(active = active, min.extra = min.extra))
}
