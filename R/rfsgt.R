rfsgt <- function(formula,
                  data,
                  ntree = 100,
                  hcut = 1,
                  treesize = NULL,
                  nodesize = NULL,
                  tune.treesize = FALSE,
                  filter = (hcut > 1),
                  keep.only = NULL,
                  fast = TRUE,
                  pure.lasso = FALSE,
                  eps = .005,
                  maxit = 500,
                  nfolds = 10,
                  block.size = 10,
                  bootstrap = c("by.root", "none", "by.user"),
                  samptype = c("swor", "swr"),  samp = NULL, membership = TRUE,
                  sampsize = if (samptype == "swor") function(x){x * .632} else function(x){x},
                  seed = NULL,
                  do.trace = FALSE,
                  ...
                  )
{
  bootstrap <- match.arg(bootstrap, c("by.root", "none", "by.user"))
  samptype <- match.arg(samptype, c("swor", "swr"))
  rO <- do.call("rfsgt.workhorse", c(list(
                  formula=formula,
                  data=data,
                  ntree = ntree,
                  hcut = hcut,
                  treesize = treesize,
                  nodesize = nodesize,
                  tune.treesize = tune.treesize,
                  filter = filter,
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
                  seed = seed,
                  do.trace = do.trace),
                  list(...)))
  ## restore xvar quantities that are over-ridden during filtering
  if ((is.logical(filter) && filter == TRUE) || is.character(filter)) {
    formula.prelim <- parse.formula(formula, data, NULL)
    formula.detail <- finalize.formula(formula.prelim, data)
    family <- formula.detail$family
    ## hotencode original data
    xvar.org.names <- formula.detail$xvar.names
    xvar <- get.hotencode(data[, xvar.org.names, drop=FALSE])
    xvar.org.names <- xvar.org.names[sapply(xvar.org.names, function(xv) {
      any(grepl(xv, rO$xvar.names))})]   
    xvar <- get.hotencode.subset(xvar, xvar.org.names)
    rO$xvar.names <- colnames(rO$xvar)
    rO$xvar.types <- rep("R", ncol(rO$xvar))
    rO$xvar.nlevels <- rep(0, ncol(rO$xvar))
  }
  ## return the goodies
  rO
}
