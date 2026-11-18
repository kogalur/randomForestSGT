##########################################################################################
##
## utilities_beta_raw.R
## Revision 2026-11-09.1
##
## Helpers that re-express the polynomial of a centered terminal node in the original
## coordinates, so that get.beta reports coefficients and term contributions in the units
## of the data for every forest, centered or not.
##
## A centered node holds beta_0 + sum_j beta_j prod_q (x_{p_q} - c_{p_q})^{k_q}, with c the
## node center.  Expanding every centered monomial by the binomial theorem gives the same
## polynomial in the raw monomials prod_q x_{p_q}^{a_q}, 0 <= a_q <= k_q.  The raw monomials
## are the retained terms and the lower order monomials that divide them (the closure of
## the retained dictionary); make.raw.basis lists them once per forest, and
## expand.centered.polynomial converts the coefficient vector of one node.  The polynomial
## is the same function, so predictions are unchanged; only the coordinates in which the
## coefficients and contributions are reported change.  evaluate.raw.node evaluates the
## raw polynomial term by term with the restriction of node.restrict (clamped coordinates,
## clamped response) exactly as evaluate.beta.node does for the centered form.
##
##########################################################################################
## Combine repeated parents of a term (the cube X1_I_X1_SQ2_ is parsed as parents X1, X1
## with powers 1, 2) and order the variables by their position in the source columns.
.sg.aggregate.monomial <- function(parents, powers, source.names) {
  parents <- as.character(parents)
  powers <- as.integer(powers)
  vars <- unique(parents)
  total <- vapply(vars, function(v) sum(powers[parents == v]), integer(1))
  ord <- order(match(vars, source.names), vars)
  list(vars = vars[ord], powers = unname(total[ord]))
}
## Canonical key of a monomial: variables in source order with their powers.
.sg.monomial.key <- function(vars, powers) {
  paste(paste0(vars, "^", powers), collapse = "*")
}
## Name of a monomial in the package convention: X for power 1, X_SQ2_ for power 2,
## X_I_X_SQ2_ for power 3, X_SQ2__I_X_SQ2_ for power 4, variables joined by _I_.
.sg.monomial.name <- function(vars, powers, sqchar = "_SQ2_", ichar = "_I_") {
  parts <- unlist(lapply(seq_along(vars), function(q) {
    k <- powers[q]
    c(rep(vars[q], k %% 2L), rep(paste0(vars[q], sqchar), k %/% 2L))
  }), use.names = FALSE)
  paste(parts, collapse = ichar)
}
## The raw basis of a forest: every retained term and every monomial that divides one.
## Returns a data frame with one row per basis monomial (term, key, xvar.names,
## xvar.power), with attribute "index" (named integer vector, key to row) and attribute
## "term.col" (row of each retained term, in term.map order).  A monomial that equals a
## retained term keeps the term's name; the others are named in the package convention.
make.raw.basis <- function(term.map, source.names, sqchar = "_SQ2_", ichar = "_I_") {
  if (!is.term.map(term.map)) {
    stop("term.map must be a term map")
  }
  keys <- character(0)
  names.out <- character(0)
  vars.out <- list()
  powers.out <- list()
  term.col <- integer(nrow(term.map))
  add <- function(vars, powers, name = NULL) {
    key <- .sg.monomial.key(vars, powers)
    hit <- match(key, keys)
    if (is.na(hit)) {
      keys <<- c(keys, key)
      names.out <<- c(names.out, if (is.null(name)) .sg.monomial.name(vars, powers, sqchar, ichar) else name)
      vars.out[[length(vars.out) + 1L]] <<- vars
      powers.out[[length(powers.out) + 1L]] <<- powers
      hit <- length(keys)
    }
    else if (!is.null(name)) {
      names.out[hit] <<- name
    }
    hit
  }
  ## the retained terms first, under their own names
  mono <- vector("list", nrow(term.map))
  for (j in seq_len(nrow(term.map))) {
    mono[[j]] <- .sg.aggregate.monomial(term.map$xvar.names[[j]], term.map$xvar.power[[j]],
                                        source.names)
    term.col[j] <- add(mono[[j]]$vars, mono[[j]]$powers, as.character(term.map$term[j]))
  }
  ## then the divisors of every retained term, recording for each term the expansion
  ## template used by expand.centered.polynomial: one row per divisor (the intercept has
  ## column 0), with the product of binomial coefficients and the exponents of the
  ## negated center
  expansion <- vector("list", nrow(term.map))
  for (j in seq_len(nrow(term.map))) {
    m <- mono[[j]]
    grid <- as.matrix(expand.grid(lapply(m$powers, function(k) 0:k)))
    col <- integer(nrow(grid))
    binom <- numeric(nrow(grid))
    for (r in seq_len(nrow(grid))) {
      a <- as.integer(grid[r, ])
      col[r] <- if (all(a == 0L)) 0L else add(m$vars[a > 0L], a[a > 0L])
      binom[r] <- prod(choose(m$powers, a))
    }
    expansion[[j]] <- list(vars = m$vars, col = col, binom = binom,
                           exponent = sweep(-grid, 2L, m$powers, "+"))
  }
  basis <- data.frame(term = names.out, key = keys, stringsAsFactors = FALSE)
  basis$xvar.names <- I(vars.out)
  basis$xvar.power <- I(powers.out)
  attr(basis, "index") <- setNames(seq_along(keys), keys)
  attr(basis, "term.col") <- term.col
  attr(basis, "expansion") <- expansion
  basis
}
## Raw coefficients of one centered node: beta holds the intercept and one coefficient per
## retained term (term.map order); center is the node center named by source variable.
## Returns the intercept and one coefficient per basis monomial.
expand.centered.polynomial <- function(beta, center, term.map, basis) {
  if (length(beta) != nrow(term.map) + 1L || any(!is.finite(beta))) {
    stop("beta must contain a finite intercept and one coefficient per term")
  }
  expansion <- attr(basis, "expansion")
  if (length(expansion) != nrow(term.map)) {
    stop("basis was built from a different term map")
  }
  coef <- numeric(nrow(basis) + 1L)
  coef[1L] <- beta[1L]
  for (j in seq_len(nrow(term.map))) {
    b <- beta[j + 1L]
    if (b == 0) next
    e <- expansion[[j]]
    c.m <- center[e$vars]
    if (anyNA(c.m)) {
      stop("center lacks a parent of term: ", term.map$term[j])
    }
    w <- b * e$binom
    for (q in seq_along(c.m)) {
      w <- w * (-c.m[q])^e$exponent[, q]
    }
    for (r in seq_along(w)) {
      coef[e$col[r] + 1L] <- coef[e$col[r] + 1L] + w[r]
    }
  }
  names(coef) <- c("betaZ", basis$term)
  coef
}
## Raw coefficients of an uncentered node, placed on the basis columns (a term that the
## retained dictionary lists twice adds both coefficients to its column).
place.raw.polynomial <- function(beta, basis) {
  coef <- numeric(nrow(basis) + 1L)
  coef[1L] <- beta[1L]
  term.col <- attr(basis, "term.col")
  for (j in seq_along(term.col)) {
    coef[term.col[j] + 1L] <- coef[term.col[j] + 1L] + beta[j + 1L]
  }
  names(coef) <- c("betaZ", basis$term)
  coef
}
## Evaluate one node in the original coordinates.  coef is the raw coefficient vector on
## the basis; support holds the node bounds (lower, upper, yLower, yUpper); restrict is a
## logical scalar or one value per row.  Restricted rows are clamped to the node's inbag
## box before the monomials are formed and their value is clamped to the inbag response
## range; that second change is returned as response.adjustment.
evaluate.raw.node <- function(source, coef, basis, support, restrict = FALSE) {
  if (length(coef) != nrow(basis) + 1L || any(!is.finite(coef))) {
    stop("coef must contain a finite intercept and one coefficient per basis monomial")
  }
  source.names <- names(support$lower)
  if (is.null(source.names) || !all(source.names %in% colnames(source))) {
    stop("evaluation source columns do not match the node support")
  }
  z <- as.matrix(source[, source.names, drop = FALSE])
  n <- nrow(z)
  if (!is.logical(restrict) || anyNA(restrict) || !(length(restrict) %in% c(1L, n))) {
    stop("restrict must be a logical scalar or one logical value per row")
  }
  restricted <- which(rep_len(restrict, n))
  if (length(restricted) > 0L) {
    for (j in seq_along(source.names)) {
      z[restricted, j] <- pmin(pmax(z[restricted, j], support$lower[j]), support$upper[j])
    }
  }
  partial <- matrix(0, nrow = n, ncol = length(coef),
                    dimnames = list(NULL, c("betaZ", basis$term)))
  partial[, 1L] <- rep.int(coef[1L], n)
  predicted <- rep.int(coef[1L], n)
  for (j in seq_len(nrow(basis))) {
    if (coef[j + 1L] == 0) next
    parents <- match(basis$xvar.names[[j]], source.names)
    powers <- basis$xvar.power[[j]]
    if (anyNA(parents)) {
      stop("source lacks a parent of basis monomial: ", basis$term[j])
    }
    term <- rep.int(1, n)
    for (q in seq_along(parents)) {
      term <- term * z[, parents[q]]^powers[q]
    }
    partial[, j + 1L] <- coef[j + 1L] * term
    predicted <- predicted + partial[, j + 1L]
  }
  adjustment <- numeric(n)
  if (length(restricted) > 0L) {
    clipped <- pmin(pmax(predicted[restricted], support$yLower), support$yUpper)
    adjustment[restricted] <- clipped - predicted[restricted]
    predicted[restricted] <- clipped
  }
  list(partial = partial, response.adjustment = adjustment, predicted = predicted)
}
