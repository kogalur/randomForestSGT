\name{rfsgt}
\alias{rfsgt}
\title{Random Forest Super Greedy Trees}
\description{
  Grow a forest of Super Greedy Trees (SGTs) using lasso.  In addition
  to prediction, the fitted forest supports local beta-value and
  partial-contribution summaries that show how predictions are
  assembled.  
}

\usage{
rfsgt(formula,
      data,
      ntree = if (hcut == 0) 500 else 100,
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
      ...)
}

\arguments{

  \item{formula}{Formula describing the model to be fit.}

  \item{data}{Data frame containing response and features.}

  \item{ntree}{Number of trees to grow.}

  \item{hcut}{Integer value indexing type of parametric regression model
    to use for splitting.  The default 2 uses linear and quadratic terms;
    3 and 4 add pairwise interactions and cubic terms and were the most
    accurate in benchmarks.  See details below.}

  \item{treesize}{Upper bound for the size of a tree (number of
    terminal nodes): an integer, or a function of the sample size
    \code{n} and of \code{hcut}.  The default is
    \code{min(1000, max(200, n/5))} for \code{hcut} 0 and 1, and
    \code{min(25, n/5)} for \code{hcut} 2 and above: with the restricted
    score, trees of 25 terminal nodes were as accurate on average as trees
    of 100 over twelve datasets and grow faster.}

  \item{nodesize}{Minumum size of terminal node.  Set internally if
    not specified.}

  \item{filter}{Logical value specifying whether dimension reduction
    (filtering) of features should be performed. Can also be specified
    using the helper function \code{tune.hcut} which performs dimension
    reduction prior to fitting.  See examples below.}

  \item{bsf}{Best split first (BSF) empirical risk minimization
    strategy. Accepted values are \code{"oob"} and \code{"inbag"}.
    The default uses "inbag" unless \code{hcut}>0 and \code{node.center}
    is \code{FALSE}.  Setting \code{bsf="inbag"} uses 
    inbag empirical risk minimization and therefore produces the most
    releastic OOB error rates.  This setting is suitable when
    \code{node.center} is \code{TRUE} as this is the most robust version
    of SGTs.  Otherwise, use \code{bsf="oob"} which is a guardrail to
    avoid numerical problems especially for larger \code{hcut} values;
    however this yields optimistic OOB error.}
  
  \item{keep.only}{Character vector specifying the features of interest.
  The data is pre-filtered to keep only these requested variables.
  Ignored if filter is specified using \code{tune.hcut}.}

  \item{fast}{Use fast filtering?}
    
  \item{pure.lasso}{Logical value specifying whether lasso splitting
  should be strictly adhered to.  In general, lasso splits are replaced
  with CART whenever numerical instability occurs (for example, small
  node sample sizes may make it impossible to obtain the cross-validated
  lasso parameter).  This option will generally produce shallow trees
  which may not be appropriate in all settings.}
  
  \item{eps}{Parameter used by \code{cdlasso}.}

  \item{maxit}{Parameter used by \code{cdlasso}.}

  \item{nfolds}{Number of cross-validation folds to be used for the lasso.}

  \item{block.size}{Determines how cumulative error rate is calculated.  To
    obtain the cumulative error rate on every nth tree, set the value to
    an integer between 1 and \code{ntree}.}
  
  \item{bootstrap}{Bootstrap protocol.  Default is \code{by.root} which
    bootstraps the data by sampling with or without replacement (without
    replacement is the default; see the option \code{samptype} below).
    If \code{none}, the data is not bootstrapped (it is not possible to
    return OOB ensembles or prediction error in this case).  If
    \code{by.user}, the bootstrap specified by \code{samp} is used.}

  \item{samptype}{Type of bootstrap used when \code{by.root} is in
    effect.  Choices are \code{swor} (sampling without replacement; 
    the default) and \code{swr} (sampling with replacement).}

  \item{samp}{Bootstrap specification when \code{by.user} is in
    effect.  Array of dim \code{n x ntree} specifying how many
    times each record appears inbag in the bootstrap for each tree.}

  \item{membership}{Should terminal node membership and inbag
    information be returned?}

  \item{sampsize}{Function specifying bootstrap size when \code{by.root}
    is in effect.  For sampling without replacement, it is the requested
    size of the sample, which by default is .632 times the sample size.
    For sampling with replacement, it is the sample size.  Can also be
    specified using a number.}

  \item{seed}{Negative integer specifying seed for the random number
    generator.}

  \item{do.trace}{Number of seconds between updates to the user on
    approximate time to completion.}

  \item{node.center}{Logical value specifying whether to center covariates
    within each node before forming powers and products for the lasso.
    Default is \code{TRUE}. When \code{filter} is supplied by
    \code{tune.hcut}, an omitted value is inherited from the tuning
    result. Ignored for \code{hcut=0}.}

  \item{node.restrict}{Logical value enabling restricted lasso terminal
    scores. Default is \code{TRUE}. Source coordinates are clamped to
    the node's inbag ranges before powers and products are formed, and
    the resulting score is clamped to its inbag response range. In grow
    mode this applies to OOB predictions and, with \code{bsf="oob"},
    candidate OOB risks. It applies to both raw and centered dictionaries.
    Inbag scores, CART terminal predictions, and internal routing scores
    use ordinary evaluation. Ignored for \code{hcut=0}. The setting is
    saved in the forest and inherited by \code{predict.rfsgt} unless
    overridden.}

  \item{...}{Further arguments passed to \command{cdlasso} and
    \command{rfsrc}.}
  

}

\details{

  Super Greedy Trees (SGTs) are tree-based models that extend ordinary
  CART-style splitting in a fundamental way.  In a standard tree, a
  split typically tests one variable at a time; hence tilted, curved, or
  interaction-driven decision boundaries must be approximated by many
  small axis-aligned cuts.  SGTs instead estimate a sparse \emph{score}
  inside a node and then split the node using that score.  This allows
  a split to depend on several variables at once; the fitted tree can
  therefore represent hyperplane, elliptical, hyperboloid, and other
  higher-order geometric boundaries much more directly.

  Operationally, the procedure is organized in stages.  First, a family of
  candidate score functions is chosen through \code{hcut}.  Second, within
  each node, a lasso model is fit by coordinate descent.  Third, the fitted
  node-wise score is used to order observations, and an allowable threshold
  on that score is searched for the best split.  The resulting daughter
  nodes are then re-fit and compared using empirical risk.  In this way,
  the difficult multivariate split problem is converted into a manageable
  one-dimensional threshold search along an estimated score.

  Tree growth uses a \emph{best split first} (BSF) strategy.  Rather than
  expanding nodes in strict depth-first or breadth-first order, BSF scans
  the current terminal nodes, measures the reduction in empirical risk for
  each candidate split, and grows the node giving the largest gain.  This
  makes the search aggressive but focused: computation is directed to the
  part of the tree that is most promising at the current stage of growth.
  In a forest, repeating this over bootstrap samples yields an ensemble of
  trees with the flexibility of multivariate cuts and the stabilizing
  effect of aggregation.

  The main user control for split geometry is \code{hcut}.  Smaller
  values give simpler cut families; larger values allow richer
  polynomial and interaction structure.  Thus \code{rfsgt} is able to
  span random-forest-like axis-aligned splits all the way to
  genuinely multivariate geometric partitioning.  When the signal is
  approximately linear or only mildly nonlinear, smaller \code{hcut}
  values may be sufficient.  When the signal involves curvature or
  interactions, larger \code{hcut} values can reduce bias and often
  achieve the same structural fit with fewer splits.

  An equally important feature of SGTs is that the fitted forest is not
  only a prediction device.  Because each split score and each terminal-node
  predictor is a sparse lasso model, the forest also stores local
  coefficient information.  For a given observation, each tree contributes
  the coefficient vector from the terminal node containing that
  observation, and averaging across trees yields forest-level beta
  summaries that are usually more stable than the coefficients from any
  single tree.  These beta values are local and data-adaptive: they can
  change from one region of the feature space to another; they should
  therefore be viewed as coefficient functions rather than a single
  global regression vector.

  This gives SGTs a genuinely hybrid character.  The partition of the
  feature space is nonparametric, as in a tree or forest, but within each
  local region the fitted response is represented by a sparse parametric
  expansion.  Built-in helpers such as \code{get.beta} expose this
  structure by returning both beta summaries and corresponding partial
  term contributions.  For a main effect, the contribution is the local
  beta multiplied by the covariate value; for an interaction, it is the
  local interaction coefficient multiplied by the associated product term.
  In many applications, these summaries can be as informative as the
  prediction itself because they show which variables, nonlinear terms,
  and interactions are driving the fitted value near the observation of
  interest.  For centered fits, \code{get.beta} expands each node
  polynomial in the original coordinates before averaging; hence the
  coefficients and contributions are in the units of the data, and the
  change made by the restricted score is reported as a separate
  \code{response.adjustment}.  For new-data explanations, pass the
  original grow object through \code{get.beta(pred, grow = fit)}.

  
  Parametric models used for splitting are indexed by \code{hcut}
  corresponding to the following geometric regions:

\enumerate{

   \item \code{hcut}=1 (hyperplane) linear model using all variables.
   \item \code{hcut}=2 (ellipse) plus all quadratic terms.
   \item \code{hcut}=3 (oblique ellipse) plus all pairwise interactions.
   \item \code{hcut}=4 plus all polynomials of degree 3 of two variables.
   \item \code{hcut}=5 plus all polynomials of degree 4 of three variables.
   \item \code{hcut}=6 plus all three-way interactions.
   \item \code{hcut}=7 plus all four-way interactions.

 }

  Setting \code{hcut}=0 gives CART splits where cuts are parallel to the
  coordinate axis (axis-aligned cuts).  Thus, \code{hcut}=0 is similar to
  random forests and can be viewed as the baseline case of the SGT
  framework.

  A major part of the implementation is devoted to regularization and
  stabilization, because richer cut families can otherwise become unstable
  in small nodes or in the presence of collinearity.  The first safeguard
  is the lasso itself.  At each node, the split-defining score is fit with
  an \eqn{\ell_1} penalty, and the penalty is chosen by cross-validation.
  This induces sparsity, controls local complexity, and lets the procedure
  adapt to the amount of information available in the node.  Near the root,
  where sample sizes are larger, the fitted score can support richer
  geometry.  Deeper in the tree, lasso sparsity and smaller node sizes tend
  to simplify the split automatically.

  A second safeguard is feature filtering.  When the predictor dimension is
  moderate or large, the candidate dictionary implied by \code{hcut} can be
  very large.  The implementation can therefore pre-filter variables using
  shallow pilot fits and retain only variables that appear with nonzero
  lasso coefficients.  This reduces runtime and variance before the final
  forest is grown.  The helper \code{tune.hcut} is the intended front-end
  for this step: it can pre-filter variables and also choose a suitable
  \code{hcut} value before the final call to \code{rfsgt}.  In practice,
  this is often the safest workflow when interactions or higher-order terms
  are being considered.

  A third safeguard is automatic simplification of a branch when the
  lasso modeling is no longer paying off.  The algorithm then replaces
  the lasso-induced split by the best CART coordinate-threshold split at
  that node and in place of local lasso node estimators, simple
  CART-style sample-average predictors are used along that branch.  In
  other words, in the presence of numerical instability, the procedure
  can simplify both the split geometry and the local node model, which
  prevents unstable or unnecessary parametric structure from being
  pushed deeper into the tree.

  The argument \code{pure.lasso} controls whether this simplification is
  allowed.  With the default behavior, a branch may switch from the
  richer lasso-based representation to a simpler CART-style
  representation when the latter is more stable or gives better
  empirical risk reduction.  Setting \code{pure.lasso=TRUE} shuts this
  switch off.  This is useful when a user wants a fully lasso-defined
  tree for methodological reasons, but in difficult data settings it can
  also lead to shallower trees because branches that would otherwise be
  stabilized by CART are instead left unsplit.

  The argument \code{bsf}, although related, addresses a different
  question.  It does not turn CART fallback on or off.  Instead, it
  decides which data drive empirical risk minimization during BSF
  search.  With \code{bsf="inbag"}, the split comparison is based on
  in-bag data.  With \code{bsf="oob"}, the same comparison uses
  out-of-bag (OOB) data as a held-out check.  This indirectly has
  implications when deciding on CART fallback, however, because it
  allows the algorithm to use out of sample data to decide whether the
  current SGT candidate really improves generalization relative to the
  best CART candidate at the same node.  If the CART candidate wins
  under the chosen risk criterion, the branch is simplified exactly as
  described above: CART split geometry is used and CART-style node
  estimators replace the local lasso fits down that branch.  This guard
  is especially helpful under potentially numerically unstable models
  when \code{hcut > 0}.  As with any model-selection procedure that
  reuses held-out data, however, users should remember that the reported
  OOB error can then be mildly optimistic.

  A fourth safeguard is the restricted score, \code{node.restrict}.  A
  lasso terminal node predicts with a polynomial estimated from its
  inbag cases.  An observation routed to the node can lie outside the
  region those cases occupy, and there a retained square or product
  takes values no inbag case produced; the prediction can then leave the
  response range by orders of magnitude, most often in small trees and
  at larger \code{hcut}.  With \code{node.restrict=TRUE} (the default)
  the polynomial is evaluated at the nearest point of the node's inbag
  box and the result is confined to the node's inbag response range.
  Inbag cases are evaluated as before; growth, routing and inbag
  predictions are therefore unchanged, and the restriction acts on the OOB ensemble
  and error rate, on the OOB risks of candidate daughters when
  \code{bsf="oob"}, and on new data.  Over twelve regression datasets
  it removed every failure of small trees and left the error of large
  trees unchanged; hence tree size can be chosen for speed.  With it,
  \code{hcut} values of 2 to 4 were more accurate than \code{hcut=1}
  and than CART on average.  \code{node.restrict=FALSE} reproduces the
  evaluation of earlier versions.

  From a practical point of view, the main tuning parameters have clear
  roles.  Use \code{hcut} to control cut richness, \code{treesize} and
  \code{nodesize} to control tree complexity, \code{filter} or
  \code{tune.hcut} when the feature space is large, and
  \code{pure.lasso} only when you explicitly want to over-ride CART
  fallback.  Users new to the method can usually start with the
  defaults.  Users working with interaction-heavy or high-dimensional
  data will typically benefit from filtering, \code{tune.hcut}, and
  OOB-based BSF.  Users interested mainly in prediction can simply call
  \code{predict.rfsgt}.  Users interested in the local parametric
  structure can query the same fitted forest with \code{get.beta} to
  recover beta summaries and partial contributions without fitting a
  separate surrogate model.  For that purpose grow shallow trees, with
  nodes of about 100 cases or more: the terminal coefficients are
  shrunk by the lasso and attenuated by the selection of the splits in
  small nodes, while the prediction error is insensitive to tree size
  with the restricted score (see \code{get.beta}).

}




\value{

  An object of class \code{c("rfsgt", "grow", family)} containing the
  trained super greedy forest and associated training-data summaries.  The
  object is a list with the following components:

  \describe{

    \item{\code{family}}{Model family.}

    \item{\code{n}}{Number of observations in the training data.}

    \item{\code{bootstrap}}{Bootstrap protocol used to grow the forest.}

    \item{\code{samptype}}{Sampling type used for root-node bootstrap
      samples.}

    \item{\code{sampsize}}{Tree sample size used by the bootstrap
      protocol.}

    \item{\code{ntree}}{Number of trees grown.}

    \item{\code{hcut}}{Final \code{hcut} value used to construct the
      splitting model.}

    \item{\code{node.center}}{Value of the \code{node.center} option.}

    \item{\code{node.restrict}}{Value of the \code{node.restrict} option,
      also saved in \code{forest$node.restrict}.}

    \item{\code{splitrule}}{Split rule used by the forest.}

    \item{\code{splitinfo}}{Internal split-rule metadata, including the
      processed \code{hcut}, split-rule index, number of random split
      points, and lasso cross-validation setting.}

    \item{\code{yvar}}{Training response values.}

    \item{\code{yvar.names}}{Response variable name.}

    \item{\code{yvar.factor}}{Factor-level metadata for the response.}

    \item{\code{yvar.types}}{Internal response type code.}

    \item{\code{xvar}}{Training predictor data after hot encoding,
      optional filtering, and any user-requested variable restriction.}

    \item{\code{xvar.augment}}{Augmented base-learner data used for
      higher-order \code{hcut} terms, or \code{NULL} when no augmented
      terms are used.}

    \item{\code{xvar.names}}{Names of the retained predictor variables.}

    \item{\code{xvar.types}}{Internal predictor type codes.}

    \item{\code{xvar.info}}{Predictor-encoding metadata used internally;
      this is \code{NULL} for standard grow objects.}

    \item{\code{xvar.augment.names}}{Names of augmented base-learner
      terms, or \code{NULL} when no augmented terms are used.}

    \item{\code{term.map}}{Term map describing how generated
      base-learner terms correspond to original variables and powers.}

    \item{\code{forest}}{Stored forest object used by
      \code{predict.rfsgt}.  This component contains the native node
      array, per-tree leaf counts, terminal-node offsets, bootstrap
      membership identifiers, fitting options, and package-version
      metadata.}

    \item{\code{nodeStat}}{Node statistics returned when empirical-risk
      output is available; otherwise \code{NULL}.}

    \item{\code{empr.risk}}{Inbag empirical-risk values by candidate tree
      size and tree, or \code{NULL} when unavailable.}

    \item{\code{oob.empr.risk}}{Out-of-bag empirical-risk values by
      candidate tree size and tree, or \code{NULL} when unavailable.}

    \item{\code{empr.risk.cart}}{Inbag empirical-risk values for CART
      fallback splits, or \code{NULL} when unavailable.}

    \item{\code{oob.empr.risk.cart}}{Out-of-bag empirical-risk values for
      CART fallback splits, or \code{NULL} when unavailable.}

    \item{\code{bsf}}{Best-split-first empirical-risk strategy used.}

    \item{\code{bsf.order}}{Best-split-first node expansion order, or
      \code{NULL} when unavailable.}

    \item{\code{predicted}}{Training-data ensemble predictions.}

    \item{\code{predicted.oob}}{Out-of-bag training-data predictions when
      available; otherwise \code{NULL}.}

    \item{\code{membership}}{Matrix of terminal-node membership by
      observation and tree when \code{membership = TRUE}; otherwise
      \code{NULL}.}

    \item{\code{inbag}}{Matrix of bootstrap counts by observation and
      tree when \code{membership = TRUE}; otherwise \code{NULL}.}

    \item{\code{ensemble}}{Internal ensemble type used for prediction
      summaries.}

    \item{\code{err.rate}}{Cumulative prediction error, typically the
      out-of-bag mean squared error for regression when available;
      otherwise \code{NULL}.}

    \item{\code{ctime.internal}}{Timing information reported by the
      native grow routine.}

    \item{\code{ctime.external}}{Elapsed R-side timing, computed from
      \code{proc.time()}.}

  }

}

\author{

  Hemant Ishwaran and Udaya B. Kogalur

}

\references{

  Ishwaran H. (2026).  Super greedy trees.  \emph{Artificial
  Intelligence Review}, 59:142.  \doi{10.1007/s10462-026-11541-6}

}

\seealso{

  \command{\link{cdlasso}},
  \command{\link{cv.rfsgt}},
  \command{\link{get.beta}},
  \command{\link{plot.rfsgt}},
  \command{\link{predict.rfsgt}}

}


\examples{


## ------------------------------------------------------------
##
##  mtcars: for CRAN testing
##
## ------------------------------------------------------------

print(rfsgt(mpg~., mtcars, ntree=3, treesize=1))


\donttest{
  
## ------------------------------------------------------------
##
##  boston housing
##
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {

## load the data
data(BostonHousing, package = "mlbench")

## hcut=4: linear, quadratic, pairwise, plus all degree 3 polynomials
## (the default is hcut=2; both apply the filter and the restricted score)
o <- rfsgt(medv~., BostonHousing, hcut=4)
print(o)

## variable selection
sort(vimp.rfsgt(medv~.,BostonHousing))

## examples of hcut=0 (similar to random forests ... but using BSF)
print(rfsgt(medv~., BostonHousing, hcut=0))
print(rfsgt(medv~., BostonHousing, hcut=0, nodesize=1))

## hcut=3 with smaller nodesize
print(rfsgt(medv~., BostonHousing, hcut=3, nodesize=1))

## the OOB error above is optimistic because the filter saw every case;
## cross-validation refits the filter inside each fold (see cv.rfsgt)
print(cv.rfsgt(medv~., BostonHousing, hcut=3))

## ------------------------------------------------------------
##
## boston housing with factors
##
## ------------------------------------------------------------
     
## load the data
data(BostonHousing, package = "mlbench")
     
## make some features into factors
Boston <- BostonHousing
Boston$zn <- factor(Boston$zn)
Boston$chas <- factor(Boston$chas)
Boston$lstat <- factor(round(0.2 * Boston$lstat))
Boston$nox <- factor(round(20 * Boston$nox))
Boston$rm <- factor(round(Boston$rm))
     
## random forest: hcut=0 
print(rfsgt(medv~., Boston, hcut=0, nodesize=1))

## hcut=3
print(rfsgt(medv~., Boston, hcut=3))


## ------------------------------------------------------------
##
##  ozone
##
## ------------------------------------------------------------

## load the data
data(Ozone, package = "mlbench")

## remove missing values, convert factors to integers
o <- data.frame(data.matrix(na.omit(Ozone)))

## try different hcut values
## note the shallow trees; but performance is good
print(rfsgt(V4~., o, hcut=0, treesize=6))
print(rfsgt(V4~., o, hcut=1, treesize=6))
print(rfsgt(V4~., o, hcut=2, treesize=6))
print(rfsgt(V4~., o, hcut=3, treesize=6))
print(rfsgt(V4~., o, hcut=4, treesize=6))
print(rfsgt(V4~., o, hcut=5, treesize=6))

}

## ------------------------------------------------------------
##
##  non-linear boundary illustrates hcut using single tree
##
## ------------------------------------------------------------

## simulate non-linear boundary
n <- 500
p <- 5
signal <- 10
treesize <- 10
ngrid <- 200

## train
x <- matrix(runif(n * p), ncol = p)
fx <- signal * sin(pi * x[, 1] * x[, 2])
nl2d <- data.frame(y = fx, x)

## truth
x1 <- x2 <- seq(0, 1, length = ngrid)
truth <- signal * sin(outer(pi * x1, x2, "*"))

## test
x.tst <- do.call(rbind, lapply(x1, function(x1j) {
  cbind(x1j, x2, matrix(runif(length(x2) * (p-2)), ncol=(p-2)))
}))
colnames(x.tst) <- colnames(x)
fx.tst <- signal * sin(pi * x.tst[, 1] * x.tst[, 2])
nl2d.tst <- data.frame(y = fx.tst, x.tst)

## SGT for different hcut values
## Enforce pure lasso
rO <- lapply(0:4, function(hcut) {
  cat("hcut", hcut, "\n")
  rfsgt(y~., nl2d, ntree=1, hcut=hcut, treesize=treesize, bootstrap="none",
         pure.lasso = TRUE, nodesize=1, filter=FALSE)
})

## nice little wrapper for plotting results
plot.image <- function(x, y, z, nlevels=40, points=FALSE) {
  xo <- x; yo <- y
  x <- unique(x); y <- unique(y)
  ## X2 varies fastest within each X1 block of x.tst.
  z <- matrix(z, nrow=length(x), ncol=length(y), byrow=TRUE)
  xlim <- ylim <- range(c(x, y), na.rm = TRUE, finite = TRUE)
  z[is.infinite(z)] <- NA
  zlim <- q <- quantile(z, c(.01, .99), na.rm = TRUE) 
  z[z<=q[1]] <- q[1]
  z[z>=q[2]] <- q[2]
  levels <- pretty(zlim, nlevels)
  col <- hcl.colors(length(levels)-1, "YlOrRd", rev = TRUE)
  plot.new()
  plot.window(xlim, ylim, "", xaxs = "i", yaxs = "i", asp = NA)
  .filled.contour(x, y, z, levels, col)
  axis(1);axis(2)
  if (points) 
    points(xo,yo ,pch=16, cex=.25)
  box()
  invisible()
}

oldpar <- par(mfrow=c(3,2))
image(x1, x2, truth, xlab="", ylab="")
contour(x1, x2, truth, nlevels = 15, add = TRUE, drawlabels = FALSE)
mtext(expression(x[1]),1,line=2)
mtext(expression(x[2]),2,line=2)
mtext(expression("truth"),3,line=1)

pO <- lapply(0:4, function(j) {
  plot.image(nl2d.tst[,"X1"],nl2d.tst[,"X2"], predict(rO[[j+1]], nl2d.tst)$predicted)
  contour(x1, x2, truth, nlevels = 15, add = TRUE, drawlabels = FALSE)
  mtext(expression(x[1]),1,line=2)
  mtext(expression(x[2]),2,line=2)
  mtext(paste0("hcut=",j),3,line=1)
  NULL
})

par(oldpar)

## ------------------------------------------------------------
##
##  friedman illustration of OOB empirical risk
##
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {

## simulate friedman
n <- 500
dta <- data.frame(mlbench::mlbench.friedman1(n, sd=0))

## rf versus rfsgt
o1 <- rfsgt(y~., dta, hcut=0)
o2 <- rfsgt(y~., dta, hcut=3)

## compute running average of OOB empirical risk
runavg <- function(x, lag = 8) {
  x <- c(na.omit(x))
  lag <- min(lag, length(x))
  cx <- c(0,cumsum(x))
  rx <- cx[2:lag] / (1:(lag-1))
  c(rx, (cx[(lag+1):length(cx)] - cx[1:(length(cx) - lag)]) / lag)
}
risk1 <- lapply(data.frame(o1$oob.empr.risk), runavg)
leaf1 <- o1$forest$leafCount
risk2 <- lapply(data.frame(o2$oob.empr.risk),  runavg)
leaf2 <- o2$forest$leafCount

## compare OOB empirical tree risk to OOB forest error
oldpar <- par(mfrow=c(2,2))

plot(c(1,max(leaf1)), range(c(risk1)), type="n",
     xlab="Tree size", ylab="RF OOB empirical risk")
l1 <- do.call(rbind, lapply(risk1, function(rsk){
  lines(rsk,col=grey(0.8))
  cbind(1:length(rsk), rsk)
}))
lines(tapply(l1[,2], l1[,1], mean), lwd=3)


plot(c(1,max(leaf2)), range(c(risk2)), type="n",
     xlab="Tree size", ylab="SGF OOB empirical risk")
l2 <- do.call(rbind, lapply(risk2, function(rsk){
  lines(rsk,col=grey(0.8))
  cbind(1:length(rsk), rsk)
}))
lines(tapply(l2[,2], l2[,1], mean), lwd=3)

plot(1:o1$ntree, o1$err.rate, type="s", xlab="Trees", ylab="RF OOB error")

plot(1:o2$ntree, o2$err.rate, type="s", xlab="Trees", ylab="SGF OOB error")

par(oldpar)

}

## ------------------------------------------------------------
##
## synthetic regression examples with different hcut
##
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {

## simulation functions
sim <- list(
     friedman1=function(n){data.frame(mlbench::mlbench.friedman1(n))},
     friedman2=function(n){data.frame(mlbench::mlbench.friedman2(n))},
     friedman3=function(n){data.frame(mlbench::mlbench.friedman3(n))},
     peak=function(n){data.frame(mlbench::mlbench.peak(n, 10))},       
     linear=function(n, sd=.1){
           x=matrix(runif(n*10), n)
           y=3*x[,1]^3-2*x[,2]^2+3*x[,3]+rnorm(n,sd=sd)
           data.frame(y=y,x)
})

## run rfsgt on the simulations
n <- 500
max.hcut <- 3
results <- setNames(lapply(names(sim), function(nm) {
  cat("simulation:", nm, "\n")
  d <- sim[[nm]](n=n)
  rO <- data.frame(do.call(rbind, lapply(0:max.hcut, function(hcut) {
   cat("     hcut:", hcut, "\n")
   o <- rfsgt(y~.,d,hcut=hcut)
   c(hcut, tail(o$err.rate, 1), tail(o$err.rate, 1) / var(o$yvar))
  })))
  colnames(rO) <- c("hcut", "mse", "smse")
  rO
}), names(sim))

## print results
print(results)

## ------------------------------------------------------------
##
## synthetic regression example showing how to tune hcut
##
## ------------------------------------------------------------

hcut.opt <- setNames(sapply(names(sim), function(nm) {
  cat("optimize hcut for simulation:", nm, "\n")
  f <- tune.hcut(y~., sim[[nm]](n=n), hcut=4)
  attr(f, "hcut")
}), names(sim))

## print the optimal hcut
print(hcut.opt)

}

## ------------------------------------------------------------
##
## iowa housing data 
##
## ------------------------------------------------------------

data(housing, package = "randomForestSRC")

## remove PID 
housing$PID <- NULL

## rough missing data imputation
d <- randomForestSRC::impute(data = data.frame(data.matrix(housing)))

d$SalePrice <- log(d$SalePrice)
d <- data.frame(data.matrix(d))

print(rfsgt(SalePrice~.,d))

## ------------------------------------------------------------
##
## high-dimensional model with variable selection
##
## ------------------------------------------------------------

## simulate big p small n data
n <- 50
p <- 500
d <- data.frame(y = rnorm(n), x = matrix(rnorm(n * p), n))

## we have a big p small n pure noise setting: let's see how well we do
cat("variables selected by vimp.rfsgt:\n")
vmp <- sort(vimp.rfsgt(y~.,d))
print(vmp[vmp > .05])

## internal filtering function can also be used
cat("variables selected by filter.rfsgt:\n")
print(filter.rfsgt(y~.,d, method="conserve"))

## ------------------------------------------------------------
##
## pre-filtering using keep.only
##
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {

## simulate the data
n <- 100
p <- 50
noise <- matrix(runif(n * p), ncol=p)
dta <- data.frame(mlbench::mlbench.friedman1(n, sd=0), noise=noise)

## filter the variables
f <- filter.rfsgt(y~., dta)

## use keep.only to pre-filter the features
print(rfsgt(y~.,dta, keep.only=f, hcut=1))
print(rfsgt(y~.,dta, keep.only=f, hcut=2))
print(rfsgt(y~.,dta, keep.only=f, hcut=3))


## ------------------------------------------------------------
##
## tuning hcut and pre-filtering using tune.hcut
##
## ------------------------------------------------------------

## simulate the data
n <- 100
p <- 50
noise <- matrix(runif(n * p), ncol=p)
dta <- data.frame(mlbench::mlbench.friedman1(n, sd=0), noise=noise)

## tune hcut
f <- tune.hcut(y~., dta, hcut=3)

## use the optimized hcut
print(rfsgt(y~.,dta, filter=f))

## over-ride the tuned hcut value
print(rfsgt(y~.,dta, filter=use.tune.hcut(f, hcut=1)))
print(rfsgt(y~.,dta, filter=use.tune.hcut(f, hcut=2)))
print(rfsgt(y~.,dta, filter=use.tune.hcut(f, hcut=3)))

## ------------------------------------------------------------
##
## get local beta values and partial contributions
##
## SGTs are not only predictive; the same fit can be queried
## for local parametric summaries.  We use friedman 1 for
## illustration.  See get.beta for details
##
## ------------------------------------------------------------

n <- 100
dta <- data.frame(mlbench::mlbench.friedman1(n))
o <- rfsgt(y~., dta, hcut=3, pure.lasso=TRUE, treesize=10)
b <- get.beta(o)
## print beta, partial values - remove intercept
print(head(b$beta[, -1]))
print(head(b$partial[, -1]))
partial.beta(b, o)
}
}
}

\keyword{rfsgt}



  


