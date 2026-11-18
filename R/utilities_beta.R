get.beta <- function(o, bag = c("oob", "inbag", "all")[1], papply = lapply,
                     relaxed = FALSE, active = c("global", "local")) {
  bag <- match.arg(bag, c("inbag", "oob", "all"))
  ## relaxed estimation: least squares on the active terms over the OOB cases of each
  ## terminal node replaces the node's lasso polynomial (utilities_beta_relaxed.R)
  if (isTRUE(relaxed)) {
    return(get.beta.relaxed(o, bag, match.arg(active)))
  }
  basis.active <- isTRUE(o$forest$node.center) ||
    isTRUE(bitwAnd(as.integer(o$forest$experimental.bits),
                   as.integer(get.basis.active.bits(TRUE))) != 0L)
  restrict.active <- isTRUE(o$hcut > 0) &&
    isTRUE(if (is.null(o$node.restrict)) o$forest$node.restrict else o$node.restrict)
  if (basis.active || restrict.active) {
    return(get.beta.local(o, bag, papply, basis.active, restrict.active))
  }
  ## pull the native array and get beta and related values
  nativeArray <- o$forest$nativeArray
  betaNative <- as.matrix(nativeArray[, !(colnames(nativeArray) %in%
       c("treeID", "nodeID", "nodeSZ", "brnodeID", "yStar", "yBar", "prnodeID")), drop = FALSE])
  ybar <- nativeArray[, "yBar"]
  ystar <- nativeArray[, "yStar"]
  ## embedded split information for CART terminal nodes
  betaZNative <- betaNative[, -1, drop = FALSE]
  betaZ.pt <- nativeArray$brnodeID == 0 & is.na(ystar) & !is.na(ybar)
  betaZNative[!betaZ.pt, ] <- NA
  betaZNative[betaZ.pt, ] <- ybar[betaZ.pt] * (betaZNative[betaZ.pt, ] != 0)
  ## pull the terminal node offsets by case id
  ## depends on grow/predict mode
  if (isTRUE(inherits(o, "grow"))) {
    idx <- o$forest$ambrOffset
  }
  else if (isTRUE(inherits(o, "predict"))) {
    idx <- o$ambrOffset
    if (is.null(idx)) idx <- o$forest$ambrOffset
  }
  else {
    stop("class structure is wrong in get.beta:", class(o), "\n")
  }
  ## set the bag type
  if (is.null(o$membership) || (inherits(o, "predict") && is.null(o$inbag))) {
    bag <- "all"
  }
  ## helper used when a non-default papply is supplied
  pull.tree.beta <- function(b) {
    tree.idx <- idx[, b]
    tree.ybar <- ybar[tree.idx]
    cart <- !is.na(tree.ybar)    ## yBar != NA --> CART split
    beta <- betaNative[tree.idx, , drop = FALSE]
    betaZ <- betaZNative[tree.idx, , drop = FALSE]
    if (any(cart)) {
      beta[cart, ] <- 0
      beta[cart, 1] <- tree.ybar[cart]
    }
    if (bag == "all") {
      ## no bag filtering
    }
    else if (bag == "inbag") {
      omit <- o$inbag[, b] == 0
      if (any(omit)) {
        beta[omit, ] <- NA
        betaZ[omit, ] <- NA
      }
    }
    else { ## oob
      omit <- o$inbag[, b] != 0
      if (any(omit)) {
        beta[omit, ] <- NA
        betaZ[omit, ] <- NA
      }
    }
    list(beta = beta, betaZ = betaZ)
  }
  if (identical(papply, lapply)) {
    ## Default path: accumulate elementwise sums and non-NA counts directly.
    ## This avoids forked parallelism, avoids storing one matrix per tree, and is usually
    ## faster/more memory efficient than building blist and reducing it later.
    beta.sum <- matrix(0, nrow = nrow(idx), ncol = ncol(betaNative))
    beta.n <- matrix(0L, nrow = nrow(idx), ncol = ncol(betaNative))
    betaZ.sum <- matrix(0, nrow = nrow(idx), ncol = ncol(betaZNative))
    betaZ.n <- matrix(0L, nrow = nrow(idx), ncol = ncol(betaZNative))
    for (b in seq_len(o$ntree)) {
      tree.idx <- idx[, b]
      tree.ybar <- ybar[tree.idx]
      cart <- !is.na(tree.ybar)    ## yBar != NA --> CART split
      beta <- betaNative[tree.idx, , drop = FALSE]
      betaZ <- betaZNative[tree.idx, , drop = FALSE]
      if (any(cart)) {
        beta[cart, ] <- 0
        beta[cart, 1] <- tree.ybar[cart]
      }
      if (bag == "all") {
        ## no bag filtering
      }
      else if (bag == "inbag") {
        omit <- o$inbag[, b] == 0
        if (any(omit)) {
          beta[omit, ] <- NA
          betaZ[omit, ] <- NA
        }
      }
      else { ## oob
        omit <- o$inbag[, b] != 0
        if (any(omit)) {
          beta[omit, ] <- NA
          betaZ[omit, ] <- NA
        }
      }
      beta.ok <- !is.na(beta)
      if (all(beta.ok)) {
        beta.sum <- beta.sum + beta
        beta.n <- beta.n + 1L
      }
      else if (any(beta.ok)) {
        beta.sum[beta.ok] <- beta.sum[beta.ok] + beta[beta.ok]
        beta.n[beta.ok] <- beta.n[beta.ok] + 1L
      }
      betaZ.ok <- !is.na(betaZ)
      if (all(betaZ.ok)) {
        betaZ.sum <- betaZ.sum + betaZ
        betaZ.n <- betaZ.n + 1L
      }
      else if (any(betaZ.ok)) {
        betaZ.sum[betaZ.ok] <- betaZ.sum[betaZ.ok] + betaZ[betaZ.ok]
        betaZ.n[betaZ.ok] <- betaZ.n[betaZ.ok] + 1L
      }
    }
    beta <- beta.sum / beta.n
    betaZ <- betaZ.sum / betaZ.n
  }
  else {
    ## Backward-compatible path for callers that pass a custom papply.
    ## matrixMeans is now serial and CRAN-safe. Users can still opt into
    ## their own parallel papply.
    blist <- papply(seq_len(o$ntree), pull.tree.beta)
    beta <- matrixMeans(lapply(blist, "[[", "beta"))
    betaZ <- matrixMeans(lapply(blist, "[[", "betaZ"))
  }
  colnames(beta) <- c("betaZ", o$xvar.names, o$xvar.augment.names)
  colnames(betaZ) <- c(o$xvar.names, o$xvar.augment.names) 
  rownames(beta) <- rownames(betaZ) <- NULL
  ## build the x design matrix
  x <- cbind(1, as.matrix(o$xvar))
  if (!is.null(o$xvar.augment.names)) {
    x <- cbind(x, as.matrix(o$xvar.augment))
  }
  ## add useful attributes
  lasso.percent <- 100 * mean(is.na(ystar[nativeArray$brnodeID == 0]) & is.na(ybar[nativeArray$brnodeID == 0]))
  predicted <- rowSums(x * beta, na.rm = TRUE)
  partial <- x * beta
  predicted[rowSums(!is.na(partial)) == 0L] <- NA_real_
  ## return the goodies
  list(beta = beta,
       betaZ = betaZ,
       lasso.percent = lasso.percent,
       predicted = predicted,
       partial = partial)
}
## Average node-local contributions after evaluating each tree.
get.beta.local <- function(o, bag, papply, node.center, node.restrict) {
  bag <- match.arg(bag, c("inbag", "oob", "all"))
  if (!inherits(o, "grow") && !inherits(o, "predict")) {
    stop("o must be a grow or prediction object from rfsgt")
  }
  newdata <- inherits(o, "predict") && !is.null(o$ambrOffset)
  forest <- o$forest
  idx <- if (newdata) o$ambrOffset else forest$ambrOffset
  trees <- if (inherits(o, "grow")) seq_len(forest$ntree) else o$get.tree
  if (is.null(trees)) {
    stop("prediction object lacks the saved tree selection; call predict() again")
  }
  if (length(trees) == 0L || anyNA(trees) || anyDuplicated(trees) ||
      any(trees < 1L | trees > forest$ntree | trees != trunc(trees))) {
    stop("invalid saved tree selection")
  }
  terms <- c(o$xvar.names, o$xvar.augment.names)
  k <- length(terms)
  if (!is.term.map(o$term.map) ||
      !identical(as.character(o$term.map$term), as.character(terms)) ||
      k != forest$hcutCnt) {
    stop("retained terms do not match the saved coefficient order")
  }
  a <- forest$nativeArray
  beta.native <- as.matrix(a[, c("betaZ", as.character(seq_len(k))), drop = FALSE])
  ## Coefficients and contributions are reported in the original coordinates.  A centered
  ## node's polynomial is expanded over the raw basis: the retained terms and the lower
  ## order monomials that divide them (utilities_beta_raw.R).
  source.all <- o$xvar.basis
  if (is.null(source.all)) {
    source.all <- o$xvar
  }
  basis <- make.raw.basis(o$term.map, colnames(source.all))
  kb <- nrow(basis)
  term.col <- attr(basis, "term.col")
  leaves <- which(a$brnodeID == 0)
  cart <- a$brnodeID == 0 & !is.na(a$yBar)
  betaZ.native <- beta.native[, -1L, drop = FALSE]
  betaZ.native[!cart, ] <- NA_real_
  betaZ.native[cart, ] <- a$yBar[cart] * (betaZ.native[cart, , drop = FALSE] != 0)
  source <- o$xvar.basis
  if (is.null(source)) {
    source <- o$xvar
  }
  n <- nrow(source)
  if (!is.matrix(idx) || nrow(idx) != n || ncol(idx) != forest$ntree) {
    stop("terminal offsets do not match the evaluation rows and trees")
  }
  support.reader <- NULL
  if (any(!cart[leaves])) {
    support.reader <- make.beta.node.support(o)
  }
  design <- NULL
  if (!node.center) {
    design <- cbind(1, as.matrix(o$xvar))
    if (!is.null(o$xvar.augment.names)) {
      design <- cbind(design, as.matrix(o$xvar.augment))
    }
    if (ncol(design) != k + 1L) {
      stop("raw design columns do not match the saved coefficients")
    }
  }
  count <- forest$rmbrCount
  if (length(count) != length(leaves) || any(!is.finite(count)) ||
      any(count < 0 | count != trunc(count))) {
    stop("saved terminal membership counts do not match the forest")
  }
  offset <- c(0, cumsum(as.double(count)))
  if (tail(offset, 1L) != length(forest$rmbrIdent)) {
    stop("saved terminal membership counts and indices disagree")
  }
  node.slot <- integer(nrow(a))
  node.slot[leaves] <- seq_along(leaves)
  pull.tree <- function(b) {
    out <- list(beta = matrix(0, n, kb + 1L),
                betaZ = matrix(NA_real_, n, k),
                partial = matrix(0, n, kb + 1L),
                adjustment = numeric(n), predicted = numeric(n),
                included = rep.int(FALSE, n))
    nodes <- unique(idx[, b])
    nodes <- nodes[!is.na(nodes) & nodes > 0]
    if (any(nodes > nrow(a) | nodes != trunc(nodes))) {
      stop("invalid terminal offsets in tree ", b)
    }
    if (any(a$treeID[nodes] != b | a$brnodeID[nodes] != 0)) {
      stop("terminal offsets refer to the wrong tree or an internal node")
    }
    for (node in nodes) {
      rows <- which(idx[, b] == node)
      restrict.rows <- rep.int(node.restrict, length(rows))
      if (!newdata) {
        slot <- node.slot[node]
        members <- forest$rmbrIdent[offset[slot] + seq_len(count[slot])]
        inbag <- rows %in% members
        keep <- switch(bag, inbag = inbag, oob = !inbag, all = rep.int(TRUE, length(rows)))
        rows <- rows[keep]
        restrict.rows <- node.restrict & !inbag[keep]
      }
      if (length(rows) == 0L) {
        next
      }
      beta <- beta.native[node, ]
      if (cart[node]) {
        coef <- numeric(kb + 1L)
        coef[1L] <- a$yBar[node]
        out$partial[rows, 1L] <- a$yBar[node]
        out$predicted[rows] <- a$yBar[node]
        out$betaZ[rows, ] <- matrix(betaZ.native[node, ], length(rows), k, byrow = TRUE)
      }
      else {
        support <- support.reader(node)
        if (node.center) {
          ## the node's polynomial in the original coordinates, then evaluated there
          coef <- expand.centered.polynomial(beta, support$center, o$term.map, basis)
          value <- evaluate.raw.node(source[rows, , drop = FALSE], coef, basis,
                                     support, restrict.rows)
        }
        else if (anyDuplicated(term.col)) {
          ## a dictionary that lists a term twice: evaluate on the basis directly
          coef <- place.raw.polynomial(beta, basis)
          value <- evaluate.raw.node(source[rows, , drop = FALSE], coef, basis,
                                     support, restrict.rows)
        }
        else {
          ## Ordinary raw rows retain the existing precomputed dictionary.
          coef <- place.raw.polynomial(beta, basis)
          partial <- matrix(0, length(rows), kb + 1L)
          partial[, c(1L, term.col + 1L)] <- sweep(design[rows, , drop = FALSE], 2L, beta, "*")
          predicted <- rep.int(beta[1L], length(rows))
          for (j in seq_len(k)) {
            predicted <- predicted + partial[, term.col[j] + 1L]
          }
          value <- list(partial = partial, predicted = predicted,
                        response.adjustment = numeric(length(rows)))
          if (any(restrict.rows)) {
            restricted <- evaluate.raw.node(source[rows[restrict.rows], , drop = FALSE],
                                            coef, basis, support, TRUE)
            value$partial[restrict.rows, ] <- restricted$partial
            value$predicted[restrict.rows] <- restricted$predicted
            value$response.adjustment[restrict.rows] <- restricted$response.adjustment
          }
        }
        out$partial[rows, ] <- value$partial
        out$adjustment[rows] <- value$response.adjustment
        out$predicted[rows] <- value$predicted
      }
      out$beta[rows, ] <- matrix(coef, length(rows), kb + 1L, byrow = TRUE)
      out$included[rows] <- TRUE
    }
    out
  }
  beta.sum <- partial.sum <- matrix(0, n, kb + 1L)
  betaZ.sum <- matrix(0, n, k)
  betaZ.n <- matrix(0L, n, k)
  denominator <- adjustment.sum <- predicted.sum <- numeric(n)
  serial <- identical(papply, lapply)
  tree.values <- if (serial) NULL else papply(trees, pull.tree)
  for (i in seq_along(trees)) {
    value <- if (serial) pull.tree(trees[i]) else tree.values[[i]]
    beta.sum <- beta.sum + value$beta
    partial.sum <- partial.sum + value$partial
    adjustment.sum <- adjustment.sum + value$adjustment
    predicted.sum <- predicted.sum + value$predicted
    denominator <- denominator + value$included
    ok <- !is.na(value$betaZ)
    betaZ.sum[ok] <- betaZ.sum[ok] + value$betaZ[ok]
    betaZ.n[ok] <- betaZ.n[ok] + 1L
  }
  beta <- beta.sum / denominator
  partial <- partial.sum / denominator
  betaZ <- betaZ.sum / betaZ.n
  colnames(beta) <- colnames(partial) <- c("betaZ", basis$term)
  colnames(betaZ) <- terms
  list(beta = beta, betaZ = betaZ,
       lasso.percent = 100 * mean(is.na(a$yStar[leaves]) & is.na(a$yBar[leaves])),
       predicted = predicted.sum / denominator,
       partial = partial,
       response.adjustment = adjustment.sum / denominator)
}
## The training source data and the training response of a forest.  The source data are
## the hot-encoded variables that enter a retained term.  A grow object and a restored
## forest (a prediction without new data) hold them as xvar.basis (xvar in older objects)
## and yvar.  A prediction on new data holds the new data in those components, and
## predict.rfsgt keeps the training values in the forest of the prediction object.
get.beta.training <- function(o) {
  if (!inherits(o, "rfsgt") || !(inherits(o, "grow") || inherits(o, "predict"))) {
    stop("o must be a grow or prediction object from rfsgt")
  }
  if (inherits(o, "predict") && !is.null(o$ambrOffset)) {
    source <- o$forest$xvar.basis
    response <- o$forest$yvar
    if (is.null(source) || is.null(response)) {
      stop("the prediction object does not hold the training data; ",
           "call predict() again with this version of the package")
    }
  }
  else {
    source <- o$xvar.basis
    if (is.null(source)) {
      source <- o$xvar
    }
    response <- o$yvar
  }
  response <- as.double(response)
  if (length(response) != nrow(source) ||
      (!is.null(o$forest$n) && nrow(source) != o$forest$n)) {
    stop("training responses and source rows do not match the forest")
  }
  list(source = source, response = response)
}
## Prepare a reader that reconstructs one terminal node at a time: its center, its inbag
## box and its inbag response range, from the inbag training cases of the node.
make.beta.node.support <- function(o) {
  forest <- o$forest
  training <- get.beta.training(o)
  source <- training$source
  parents <- get.term.map.parents(o$term.map, type = "xvar.names")
  if (length(parents) == 0L || !all(parents %in% colnames(source))) {
    stop("training source columns for the retained terms are missing")
  }
  source <- source[, colnames(source) %in% parents, drop = FALSE]
  response <- training$response
  node.center <- isTRUE(forest$node.center) ||
    isTRUE(bitwAnd(as.integer(forest$experimental.bits),
                   as.integer(get.basis.active.bits(TRUE))) != 0L)
  weights <- forest$lot[["lasso.weights"]]
  if (node.center && !is.null(weights) && length(weights) != nrow(source)) {
    stop("lasso weights do not match the training source rows")
  }
  ## Keep every terminal slot, including rejected-tree placeholders.
  leaves <- which(forest$nativeArray$brnodeID == 0)
  count <- forest$rmbrCount
  if (length(count) != length(leaves) || any(!is.finite(count)) ||
      any(count < 0 | count != trunc(count))) {
    stop("saved terminal membership counts do not match the forest")
  }
  offset <- c(0, cumsum(as.double(count)))
  if (tail(offset, 1L) != length(forest$rmbrIdent)) {
    stop("saved terminal membership counts and indices disagree")
  }
  node.slot <- integer(nrow(forest$nativeArray))
  node.slot[leaves] <- seq_along(leaves)
  function(node) {
    if (!is.numeric(node) || length(node) != 1L || is.na(node) ||
        node < 1 || node > length(node.slot) || node != trunc(node)) {
      stop("node must identify one row of the saved native array")
    }
    slot <- node.slot[node]
    if (slot == 0L || count[slot] == 0L) {
      stop("node must be a terminal node with inbag members")
    }
    ## Preserve bootstrap repetitions and their saved accumulation order.
    members <- forest$rmbrIdent[offset[slot] + seq_len(count[slot])]
    if (anyNA(members) || any(members < 1L | members > nrow(source))) {
      stop("saved inbag members are outside the training source rows")
    }
    weight.norm <- NULL
    if (node.center) {
      if (is.null(weights)) {
        weight.norm <- rep.int(1 / length(members), length(members))
      }
      else {
        weight.sum <- 0
        for (i in members) {
          weight.sum <- weight.sum + weights[i]
        }
        if (!is.finite(weight.sum) || weight.sum <= 0) {
          stop("node has a nonpositive or nonfinite lasso weight sum")
        }
        weight.norm <- weights[members] / weight.sum
      }
    }
    center <- numeric(ncol(source))
    names(center) <- colnames(source)
    lower <- upper <- center
    y <- response[members]
    if (any(!is.finite(y))) {
      stop("node has nonfinite inbag responses")
    }
    for (j in seq_len(ncol(source))) {
      values <- source[members, j]
      if (any(!is.finite(values))) {
        stop("node has nonfinite inbag source values")
      }
      ## Bounds include all members, including those with zero lasso weight.
      lower[j] <- min(values)
      upper[j] <- max(values)
      if (node.center) {
        for (k in seq_along(members)) {
          center[j] <- center[j] + values[k] * weight.norm[k]
        }
      }
    }
    if (any(!is.finite(center))) {
      stop("node has a nonfinite source center")
    }
    list(members = members, center = center, lower = lower, upper = upper,
         yLower = min(y), yUpper = max(y))
  }
}
## Evaluate one lasso leaf; beta follows the retained term.map order.
evaluate.beta.node <- function(source, beta, term.map, support, restrict = FALSE) {
  if (!is.term.map(term.map) || !is.numeric(beta) || is.complex(beta) ||
      length(beta) != nrow(term.map) + 1L || any(!is.finite(beta))) {
    stop("beta must contain a finite intercept and one coefficient per term")
  }
  source.names <- names(support$center)
  if (is.null(source.names) || !all(source.names %in% colnames(source))) {
    stop("evaluation source columns do not match the node support")
  }
  z <- as.matrix(source[, source.names, drop = FALSE])
  n <- nrow(z)
  if (!is.logical(restrict) || anyNA(restrict) ||
      !(length(restrict) %in% c(1L, n))) {
    stop("restrict must be a logical scalar or one logical value per row")
  }
  restricted <- which(rep_len(restrict, n))
  ## Project original coordinates before subtracting the node center.
  for (j in seq_along(source.names)) {
    if (length(restricted) > 0L) {
      z[restricted, j] <- pmin(pmax(z[restricted, j], support$lower[j]),
                              support$upper[j])
    }
    z[, j] <- z[, j] - support$center[j]
  }
  partial <- matrix(0, nrow = n, ncol = length(beta),
                    dimnames = list(NULL, c("betaZ", as.character(term.map$term))))
  partial[, 1L] <- rep.int(beta[1L], n)
  predicted <- rep.int(beta[1L], n)
  for (j in seq_len(nrow(term.map))) {
    ## Match native evaluation: unused terms cannot introduce 0 * Inf or NA.
    if (beta[j + 1L] == 0) {
      next
    }
    parents <- match(as.character(term.map$xvar.names[[j]]), source.names)
    powers <- term.map$xvar.power[[j]]
    if (length(parents) == 0L || anyNA(parents) ||
        length(powers) != length(parents) || !is.numeric(powers) ||
        any(!is.finite(powers) | powers < 1 | powers != trunc(powers) |
            powers > .Machine$integer.max)) {
      stop("invalid parents or powers for term: ", term.map$term[j])
    }
    term <- rep.int(1, n)
    for (q in seq_along(parents)) {
      base <- z[, parents[q]]
      power <- powers[q]
      value <- rep.int(1, n)
      ## Use the native integer-power multiplication order.
      while (power > 0) {
        if (power %% 2 == 1) {
          value <- value * base
        }
        power <- power %/% 2
        if (power > 0) {
          base <- base * base
        }
      }
      term <- term * value
    }
    partial[, j + 1L] <- beta[j + 1L] * term
    predicted <- predicted + partial[, j + 1L]
  }
  ## Keep response clipping separate from the polynomial contributions.
  adjustment <- numeric(n)
  if (length(restricted) > 0L) {
    clipped <- pmin(pmax(predicted[restricted], support$yLower), support$yUpper)
    adjustment[restricted] <- clipped - predicted[restricted]
    predicted[restricted] <- clipped
  }
  list(partial = partial, response.adjustment = adjustment, predicted = predicted)
}
## calculates elementwise mean of a list of matrices with NA's ignored
matrixMeans <- function(mm) {
  if (!is.list(mm) || length(mm) == 0L) {
    stop("mm must be a non-empty list of matrices")
  }
  first <- as.matrix(mm[[1L]])
  d <- dim(first)
  if (length(d) != 2L) {
    stop("all elements of mm must be two-dimensional")
  }
  total <- matrix(0, nrow = d[1L], ncol = d[2L], dimnames = dimnames(first))
  count <- matrix(0L, nrow = d[1L], ncol = d[2L], dimnames = dimnames(first))
  for (i in seq_along(mm)) {
    x <- if (i == 1L) first else as.matrix(mm[[i]])
    if (!identical(dim(x), d)) {
      stop("all matrices in mm must have the same dimensions")
    }
    ok <- !is.na(x)
    if (all(ok)) {
      total <- total + x
      count <- count + 1L
    }
    else if (any(ok)) {
      total[ok] <- total[ok] + x[ok]
      count[ok] <- count[ok] + 1L
    }
  }
  total / count
}
