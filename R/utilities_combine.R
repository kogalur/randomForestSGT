rfsgt.combine <- function(formula,
                          data,
                          hcut = 3,
                          ntree = 10,
                          treesize = NULL,
                          tune = TRUE,
                          cart.lasso = FALSE,
                          filter = NULL,##hot-encoded hcut base-learner features to keep
                          ...
                          )
{
  ## fast filter the variables (unless user has provided their own list)
  if (is.null(filter)) {
    filter <- filter.custom.rfsgt(formula, data, hcut=hcut, method="liberal", fast=TRUE)
  }
  ## hot-encode the data: then make the filtered base-learner X matrix
  stump <- get.stump(formula, data)
  dta <- data.frame(stump$yvar,
     make.baselearner(get.hotencode(stump$xvar), hcut=hcut)[, filter, drop=FALSE])
  colnames(dta)[1] <- stump$yvar.names
  ## options
  dots <- list(...)
  dots$omp <- NULL
  ## make nfold-parallelized single SGT calls
  lapply(1:ntree, function(nt) {
    do.call("rfsgt", c(list(formula, dta,
               hcut=1, ntree=1, treesize=treesize, tune=tune, cart.lasso=cart.lasso), dots))
  })
}
