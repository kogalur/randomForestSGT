\name{plot.rfsgt}
\alias{plot.rfsgt}
\title{Tree Size Diagnostics for Random Forest Super Greedy Trees}
\description{

  Two diagnostics for choosing the tree size of a random forest of
  super greedy trees: the out-of-bag (OOB) empirical risk of each tree
  as it grows, and the OOB error of whole forests grown with different
  tree sizes.

}
\usage{
\method{plot}{rfsgt}(x,
  which = c("risk", "forest"),
  treesize = c(1, 2, 3, 6, 12, 25),
  lag = 8,
  smooth = TRUE,
  verbose = TRUE,
  ...)
}
\arguments{

  \item{x}{An object of class \code{(rfsgt, grow)}.}

  \item{which}{Which plots to draw: \code{"risk"} for the OOB
    empirical risk of each tree by tree size, \code{"forest"} for the
    forest OOB error by tree size, or both (the default).}

  \item{treesize}{Tree sizes (upper bounds on the number of terminal
    nodes) at which forests are grown for the \code{"forest"} plot.}

  \item{lag}{Window of the running average applied to each tree's OOB
    empirical risk.  Use \code{1} for no smoothing.}

  \item{smooth}{Logical. If \code{TRUE}, the average risk curve is
    smoothed by \code{loess} and the lowest point is taken from the
    smooth.}

  \item{verbose}{Logical. If \code{TRUE}, report each forest as it is
    grown.}

  \item{...}{Further arguments passed to \code{rfsgt} when growing the
    forests for the \code{"forest"} plot, for example \code{ntree} or
    \code{seed}.  These take precedence over the settings copied from
    \code{x}.}

}
\details{

  \emph{OOB empirical risk by tree size.}  Each tree is grown best split
  first, one terminal node at a time, and its OOB empirical risk is
  recorded at each size (\code{x$oob.empr.risk}).  Each tree's risk is
  shown as a running average over \code{lag} sizes (grey) together with
  the average over trees (thin dark line) and its smooth (thick line).
  The average at a given size is over the trees that reached it. It is
  drawn where at least a fifth of the trees with recorded risk contributed,
  with a minimum of three contributors when available. If only one or two
  trees have recorded risk, all of them must contribute at that size.

  \emph{Forest OOB error by tree size.}  The per-tree risk does not
  account for the averaging of the forest: deeper trees are individually
  worse but vary more from tree to tree, and averaging removes much of
  that variation.  The forest is therefore often best at a larger tree
  size than the per-tree curve suggests.  The second plot grows a forest
  at each value of \code{treesize} and shows its OOB error against the
  average number of terminal nodes (log scale); the forest \code{x} is
  shown in red at its average tree size.  Use this plot to choose the
  tree size.  The forests are grown from common random numbers; hence
  the differences between tree sizes are not confounded with run-to-run
  variation.  Supplying \code{seed} in \code{...} turns this off.  The
  OOB error of a forest still varies from run to run, more for small
  data sets; the overall level of the curve can therefore shift between
  runs, and a larger \code{ntree} gives a more stable curve.

  The forest-size comparison requires a finite OOB error for the fitted
  forest and each comparison forest. It is unavailable with
  \code{bootstrap="none"}, which provides no OOB observations.

  The forests are grown exactly as \code{x} was grown, apart from the
  tree size: from the response and source predictors stored in \code{x},
  including parents of retained polynomial terms, with the basis terms
  of \code{x} (rebuilt from \code{x$term.map}; hence any
  filtering applied to \code{x}, for example by \code{tune.hcut}, is
  retained), and with the number of trees, \code{hcut}, node size,
  node centering, node restriction, best split first setting and sampling scheme of
  \code{x}.  Grown at its own tree size, this reproduces \code{x}.
  Settings that are not stored in \code{x} (\code{pure.lasso},
  \code{eps}, \code{maxit}, \code{fast}) take their defaults; pass them
  in \code{...} if \code{x} used other values.
}
\value{

  Invisibly, a list with components \code{risk} (the running averages
  for each tree, \code{curves}; the average and smoothed average by tree
  size, \code{average}; and the size with the lowest smoothed average,
  \code{lowest}) and \code{forest} (a data frame of tree size, average
  number of terminal nodes and OOB error for each forest, with
  \code{treesize = NA} for \code{x}).  

}
\author{
  Hemant Ishwaran and Udaya B. Kogalur
}
\references{

  Ishwaran H. (2026).  Super greedy trees.  \emph{Artificial
  Intelligence Review}, 59:142.  \doi{10.1007/s10462-026-11541-6}

}
\seealso{
  \command{\link{rfsgt}},
  \command{\link{tune.hcut}}
}
\examples{
\donttest{
## ------------------------------------------------------------
##
##  ozone
##
## ------------------------------------------------------------

## load the data, calendar variables as integers
data(Ozone, package = "mlbench")
o <- data.frame(data.matrix(na.omit(Ozone)))

## fit the forest and plot both diagnostics
fit <- rfsgt(V4 ~ ., o, hcut = 3)
plot(fit)

## per-tree risk only (no forests are grown)
plot(fit, which = "risk")

## the forest error at more tree sizes, keeping the results
res <- plot(fit, which = "forest", treesize = c(1, 2, 4, 8, 16, 32, 64))
print(res$forest)

## ------------------------------------------------------------
##
##  boston housing, grown using a tune.hcut filter
##
## ------------------------------------------------------------

data(BostonHousing, package = "mlbench")
f <- tune.hcut(medv ~ ., BostonHousing, hcut = 3)
fit <- rfsgt(medv ~ ., BostonHousing, filter = use.tune.hcut(f, hcut = 3))

## the filtered basis terms of fit are retained by the forests
plot(fit)
}
}
\keyword{plot}
