rfsgt <- function(formula,
                  data,
                  ntree = if (hcut==0) 500 else 100,
                  hcut = 2,
                  treesize = NULL,
                  nodesize = NULL,
                  filter = (hcut > 1),
                  bsf = if (hcut > 0 && isFALSE(node.center)) "oob" else "inbag",
                  keep.only = NULL,
                  fast = TRUE,
                  pure.lasso = FALSE,
                  eps = .005,
                  maxit = 500,
                  nfolds = 10,
                  block.size = 1,
                  bootstrap = c("by.root", "none", "by.user"),
                  samptype = c("swor", "swr"),  samp = NULL, membership = TRUE,
                  sampsize = if (samptype == "swor") function(x){x * .632} else function(x){x},
                  node.center = TRUE,
                  seed = NULL,
                  do.trace = FALSE,
                  node.restrict = TRUE,
                  ...
                  )
{
  ## Inherit the tuning basis unless explicitly overridden. Filters without
  ## a centering attribute use the current default. Resolve hcut before the
  ## tree-count and BSF defaults are evaluated, for either basis.
  if (inherits(filter, "tune.hcut")) {
    if (missing(node.center) && !is.null(attr(filter, "node.center"))) {
      node.center <- attr(filter, "node.center")
    }
    hcut <- attr(filter, "hcut")
  }
  if (!is.logical(node.center) || length(node.center) != 1L || is.na(node.center)) {
    stop("node.center must be TRUE or FALSE")
  }
  bootstrap <- match.arg(bootstrap, c("by.root", "none", "by.user"))
  samptype <- match.arg(samptype, c("swor", "swr"))
  ## A saved tuning setting takes precedence over the automatic BSF default.
  bsf.default <- if (hcut > 0 && !node.center) "oob" else "inbag"
  if (missing(bsf)) {
    if (inherits(filter, "tune.hcut") && !is.null(attr(filter, "bsf"))) {
      bsf <- attr(filter, "bsf")
    }
    else {
      bsf <- bsf.default
    }
  }
  bsf <- get.bsf(bsf, default = bsf.default, warn = TRUE)
  rO <- do.call("rfsgt.workhorse", c(list(
                  formula=formula,
                  data=data,
                  ntree = ntree,
                  hcut = hcut,
                  treesize = treesize,
                  nodesize = nodesize,
                  filter = filter,
                  bsf = bsf,
                  keep.only = keep.only,
                  fast = fast,
                  pure.lasso = pure.lasso,
                  eps = eps,
                  maxit = maxit,
                  nfolds = nfolds,
                  block.size = block.size,
                  bootstrap = bootstrap,
                  samptype = samptype,
                  samp = samp,
                  membership = membership,
                  sampsize = sampsize,
                  node.center = node.center,
                  seed = seed,
                  do.trace = do.trace,
                  node.restrict = node.restrict),
                  list(...)))
  ## how the dictionary was screened, for print.rfsgt
  rO$filter.type <- if (inherits(filter, "tune.hcut")) "user" else if (isTRUE(filter)) "varpro" else "none"
  ## save xvar information needed later by predict.rfsgt
  if (!is.null(attr(rO$xvar, "hotencode"))) {
    if (!is.null(rO$term.map)) {
      schema.hot.names <- unique(c(
        rO$xvar.names,
        get.term.map.parents(rO$term.map, type = "xvar.names")
      ))
      xvar.info.names <- get.xvar.org.names(
        schema.hot.names, attr(rO$xvar, "hotencode")$map
      )
    }
    else {
      xvar.info.names <- get.hotencode.filter(rO$xvar,
                                              unique(c(rO$xvar.names, rO$xvar.augment.names)))
    }
    if (length(xvar.info.names) == 0) {
      xvar.info.names <- get.hotencode.filter(rO$xvar,
                                              unique(c(rO$xvar.names, rO$xvar.augment.names)))
    }
    rO$xvar.info <- get.hotencode.info(rO$xvar, xvar.info.names)
  }
  ## restore xvar quantities that are over-ridden during filtering
  if ((is.logical(filter) && filter == TRUE) || inherits(filter, "tune.hcut")) {
    rO$xvar.names <- colnames(rO$xvar)
    rO$xvar.types <- rep("R", ncol(rO$xvar))
    rO$xvar.nlevels <- rep(0, ncol(rO$xvar))
  }
  ## return the goodies
  rO
}
