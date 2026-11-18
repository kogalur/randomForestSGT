\name{get.beta}
\alias{get.beta}
\alias{partial.beta}
\alias{importance.beta}
\title{Local Coefficients and Term Contributions of a Super Greedy Forest}

\description{
  Extracts the local regression coefficients (beta values) that a super
  greedy forest uses to predict each observation, and breaks each
  prediction down into the contributions of the individual terms.  Works
  on the training data (a grow object) and on new data (a prediction
  object).  An optional relaxed estimate replaces the lasso coefficients
  by least squares estimates computed from out-of-bag data.  The helpers
  \code{partial.beta} and \code{importance.beta} use these coefficients
  to plot the partial effect of each variable and to compare it with the
  response.
}

\usage{
get.beta(o, bag = c("oob", "inbag", "all")[1], papply = lapply, 
         relaxed = FALSE, active = c("global", "local"))

partial.beta(b, o, xvar.names = NULL, by = NULL, granule = 5,
             same.scale = TRUE, max.plots = 12, ...)

importance.beta(b, o)
}

\arguments{

  \item{o}{An object of class \code{(rfsgt, grow)} or \code{(rfsgt,
    predict)}.  For \code{partial.beta} and \code{importance.beta}, the
    object that was passed to \code{get.beta}, which holds the values of
    the variables.  For \code{importance.beta}, new data must contain
    the response.}

  \item{bag}{Which trees to use for each training observation.  The
    default \code{"oob"} uses the trees for which the observation is
    out-of-bag and reproduces the OOB predictions of the forest
    (\code{predicted.oob}).  \code{"inbag"} uses the trees for which the
    observation is inbag and reproduces \code{predicted}.  \code{"all"}
    uses every tree.  For new data all trees are used, which is the same
    as \code{bag = "all"}.}

  \item{papply}{Function used to loop over the trees, either
    \code{lapply} (default) or \code{mclapply} for parallel processing.}

  \item{relaxed}{Logical.  If \code{TRUE}, the coefficients of each
    terminal node are re-estimated by least squares, without the lasso
    penalty, using the out-of-bag cases that fall in the node.  See
    Details.}

  \item{active}{Terms used by the least squares estimate when
    \code{relaxed = TRUE}.  \code{"global"} (default) uses all terms
    retained by the forest, the same set in every node.  \code{"local"}
    uses only the terms with a nonzero lasso coefficient in the node.}

  \item{b}{The value of \code{get.beta}, with or without
    \code{relaxed = TRUE}.  For \code{importance.beta} it must be
    computed without \code{relaxed = TRUE} and, for the training data,
    with \code{bag = "oob"} (the default).}

  \item{xvar.names}{Names of the variables to plot.  By default all
    variables with a nonzero partial effect are plotted, ordered by the
    standard deviation of the effect, up to \code{max.plots} of them.}

  \item{by}{Optional name of a second variable.  The observations are
    divided into at most three groups of this variable, and each group
    gets its own curve.  See Details.}

  \item{granule}{A variable with at most this many distinct values is
    shown by boxplots instead of a scatter plot.}

  \item{same.scale}{Logical.  If \code{TRUE}, all panels use the same
    vertical scale, which shows which variables have the larger effects.
    The scale covers the central 99 percent of the effects.}

  \item{max.plots}{Maximum number of panels when \code{xvar.names} is
    not specified.}

  \item{...}{Further graphical parameters passed to \code{plot}.}

}

\details{

  The terminal node of a super greedy tree contains the results from a
  lasso fit to the inbag cases (in the case of a lasso failure, standard
  CART prediction is used).  \code{get.beta} collects the polynomials of
  these successful lasso models for each observation and averages their
  coefficients over the trees.  The result, \code{beta}, contains one
  set of local regression coefficients per observation.  These
  coefficients describe how the prediction of the forest depends on each
  term near that observation, which can change from one region of the
  feature space to another.

  \code{get.beta} also breaks each prediction down by term.  The
  contribution of a term is its coefficient times the value of the term
  for the observation, for example \eqn{\beta_1 x_1}{b1 x1} for a main
  effect or \eqn{\beta_{12} x_1 x_2}{b12 x1 x2} for an interaction.  The
  contributions are averaged over trees and returned in \code{partial},
  whose first column is the intercept.  Each row of \code{partial} adds
  up to the prediction, apart from an adjustment for the restricted
  score described below.  Plotting the contribution of a term against
  its variable shows the partial effect of that term, and the column
  means of \code{beta} summarize which terms the forest uses overall.

  Interpreting these coefficients needs some care.  The lasso shrinks them
  toward zero, especially in deep trees.  Growing shallow trees (a small
  \code{treesize}) reduces the shrinkage, and \code{relaxed = TRUE}
  further mitigates this problem by re-estimating the coefficients by
  least squares on out-of-bag data.  Both options are explained below.

  \subsection{Which trees are averaged}{

    The option \code{bag} decides which trees are used for the
    calculations.  With the default \code{bag = "oob"}, only the trees
    for which the observation is out-of-bag are used, and the
    reconstructed prediction \code{predicted} equals the OOB prediction
    of the forest (\code{predicted.oob}).  With \code{bag = "inbag"} it
    equals the inbag prediction (\code{predicted}), and \code{bag =
    "all"} uses every tree.  

    For new data, every tree used by the prediction contributes (all
    trees, or the trees chosen with \code{get.tree} in
    \code{predict.rfsgt}), and \code{predicted} equals the prediction
    stored in the prediction object.  

    A tree can also contain CART terminal nodes, which the algorithm uses
    in place of lasso nodes where the lasso becomes unstable (see
    \code{rfsgt}).  A CART terminal node predicts with the mean response
    of its inbag cases.  \code{get.beta} treats this mean as the
    intercept and sets all other coefficients of the node to zero.

  }

  \subsection{Coefficients in the original units}{

    With \code{node.center = TRUE} (the default in \code{rfsgt}), each
    node estimates its polynomial after centering the variables at the
    node.  Coefficients on this centered scale depend on the node center
    and cannot be compared or averaged across nodes.  \code{get.beta}
    therefore rewrites each node polynomial in terms of the original,
    uncentered variables before averaging, and all coefficients and
    contributions are reported on the scale of the data.

    Rewriting a centered square or product produces additional lower
    order terms.  For example, a product term in a node with centers
    \eqn{c_1}{c1} and \eqn{c_2}{c2} expands as
    \deqn{(x_1 - c_1)(x_2 - c_2) = x_1 x_2 - c_2 x_1 - c_1 x_2 + c_1 c_2.}{(x1 - c1)(x2 - c2) = x1 x2 - c2 x1 - c1 x2 + c1 c2.}
    The expansion contains the main effects \eqn{x_1}{x1} and
    \eqn{x_2}{x2}, and the constant is added to the intercept.  For this
    reason \code{beta} and \code{partial} can have more columns than the
    forest has retained terms.  In this example they include columns for
    \code{X1} and \code{X2} even if the forest retained only the product
    \code{X1_I_X2}.

    The columns are labeled by the dictionary terms.  The first column
    of \code{beta} (labeled \code{betaZ}) is the intercept.  A square
    ends in \code{_SQ2_}, as in \code{X3_SQ2_} for the square of
    \code{X3}, and a product joins the two variable names with
    \code{_I_}, as in \code{X1_I_X2} for the product of \code{X1} and
    \code{X2}.

  }

  \subsection{Restricted score}{

    When the forest uses the restricted score (\code{node.restrict = TRUE},
    the default in \code{rfsgt}), out-of-bag and new observations are
    evaluated in two steps (see \code{predict.rfsgt}).  The node
    polynomial is evaluated at the nearest point of the box spanned by
    the inbag cases of the node, and the result is then confined to the
    range of the inbag responses.  The contributions in \code{partial}
    are computed at that nearest point.  The second step is not part of
    the polynomial, and \code{get.beta} reports its effect separately as
    \code{response.adjustment}.  With this adjustment included,
    \code{rowSums(partial) + response.adjustment} equals
    \code{predicted} exactly.

  }

  \subsection{Shrinkage of the coefficients}{

    In deep trees lasso coefficients are biased toward zero for two
    reasons.  First, the lasso penalty, chosen by cross-validation
    within the node, shrinks the coefficients.  In a small node a square
    or a product varies little, and the penalty tends to drop these
    terms altogether.  Second, the splits that formed the node were
    chosen using the same responses that are then used to estimate its
    polynomial, which attenuates the coefficients further.  Note that
    this biasing does not degrade prediction, it is merely an artifact
    of penalization/regularization.

    When the true regression function is a polynomial (or can be
    approximated by the dictionary terms), the column means of
    \code{beta} estimate the coefficients.  In our runs of the example
    below, with about 100 inbag cases per terminal node the column means
    were within 15 percent of the true values for the linear and square
    terms and within 25 percent for the interaction.  With about 30
    cases per node (\code{treesize = 10}) they fell to about half the
    true values, while the prediction error stayed the same.  If the
    coefficients are of primary interest, grow shallow trees by setting
    a small \code{treesize}.  The default tree size of \code{rfsgt} is
    chosen for prediction, and its terminal nodes are too small to
    estimate the coefficients well.

  }

  \subsection{Relaxed estimation}{

    Setting \code{relaxed = TRUE} keeps the terms of each node but
    replaces their lasso coefficients by ordinary least squares
    estimates, as in the relaxed lasso with relaxation parameter zero.
    In each terminal node, least squares uses the out-of-bag cases of
    that tree that fall in the node.  With the default
    \code{bsf = "inbag"}, these cases played no part in growing the tree,
    and the estimates are free of both the lasso shrinkage and the
    attenuation caused by the splits.  In our runs of the example below,
    the relaxed coefficients were within 5 percent of the true values for
    trees with 3 and 5 terminal nodes, and the relaxed polynomials
    predicted as well as the forest.

    The argument \code{active} decides which terms enter the least
    squares estimate.  With \code{active = "global"}, every node uses all
    terms retained by the forest.  With \code{active = "local"}, each node
    uses only the terms its own lasso kept.  The local version has fewer
    coefficients and needs fewer cases, but a term that the node lasso
    dropped stays out.

    Least squares needs enough data.  A node is used only if its number
    of out-of-bag cases exceeds its number of coefficients (the intercept
    plus the active terms) by at least five, and the other nodes are
    skipped.  Shallow trees, and the large nodes of deeper trees, meet
    this requirement.  The component \code{nodes.used} reports the
    fraction of nodes that were used, and \code{nodes} gives the number
    of nodes averaged for each observation.  An observation that falls
    only in skipped nodes gets \code{NA}.

    A term that is an exact linear combination of the other active terms
    within a node, such as the square of a binary variable, cannot be
    estimated there.  Least squares drops it from that node and its
    coefficient in the node is zero.  The component \code{aliased}
    reports for each term the fraction of used nodes in which this
    happened.

    The relaxed polynomials are evaluated directly, without the
    restricted score, and \code{response.adjustment} is zero.  For a
    training observation, \code{predicted} is a leave-one-out prediction.
    In each node where the observation was one of the out-of-bag cases
    used by least squares, its value is computed from the estimate with
    that observation left out, and the error of \code{predicted} is
    therefore an out-of-sample error.  The in-sample values are in
    \code{fitted}, and the contributions in \code{partial} add up to
    \code{fitted}.  For new data, \code{predicted} and \code{fitted} are
    the same.

    The standard errors in \code{se} come from the least squares
    estimates.  If a coefficient is averaged over \eqn{B}{B} nodes whose
    least squares standard errors are \eqn{s_1, \ldots, s_B}{s1, ..., sB},
    its reported standard error is
    \deqn{\sqrt{s_1^2 + \cdots + s_B^2} / B,}{sqrt(s1^2 + ... + sB^2) / B,}
    which is the standard error of an average of \eqn{B}{B} independent
    estimates.
    To check whether a coefficient varies over the feature space, compare
    its spread over the observations with its standard error.  If the
    standard deviation of a column of \code{beta} is no larger than its
    typical standard error, the data give no evidence that the
    coefficient changes from one region to another.

  }

  \subsection{Partial effect plots}{

    \code{partial.beta} plots the partial effect of each original
    variable from the value of \code{get.beta}.  A variable can enter
    several terms, namely its linear term, its powers and its products
    with other variables, and its partial effect combines all of them.
    For an observation with local coefficients \eqn{\beta}{b}, the
    partial effect of a variable \eqn{x}{x} is the value of its local
    polynomial minus the value of the same polynomial with \eqn{x}{x}
    set to its mean, while the other variables keep their observed
    values.  For example, for the polynomial
    \deqn{\beta_1 x + \beta_{11} x^2 + \beta_{12} x z}{b1 x + b11 x^2 + b12 x z}
    the partial effect of \eqn{x}{x} is
    \deqn{\beta_1 (x - m) + \beta_{11} (x^2 - m^2) + \beta_{12} (x - m) z,}{b1 (x - m) + b11 (x^2 - m^2) + b12 (x - m) z,}
    where \eqn{m}{m} is the mean of \eqn{x}{x}.  Terms that do not
    involve \eqn{x}{x} cancel.  Unlike a sum of the contributions in
    \code{partial}, the partial effect does not depend on where the
    origin of a variable lies.  For a binary variable or a factor, whose
    mean is not one of its values, the average of the polynomial over
    the values of \eqn{x}{x} in the data replaces the polynomial at the
    mean.

    Each panel plots the partial effect against the variable and adds a
    smooth curve.  The points form a band around the curve because the
    coefficients are local and because product terms involve other
    variables.  A variable with at most \code{granule} distinct values
    is shown by boxplots.  A factor is treated as one variable, and its
    effect combines the terms of all its levels.  Levels that the filter
    did not retain are shown together as \code{other}.  By default the
    panels are ordered by the standard deviation of the effect and use
    the same vertical scale.

    With \code{by}, the observations are divided into groups of a second
    variable and each group gets its own curve.  The groups are the
    values of the variable if it has at most three, and otherwise its
    low, middle and high thirds.  Curves with different slopes or shapes
    indicate an interaction between the two variables.

    The coefficients can come from the lasso or from the relaxed
    estimate.  Partial effects computed from lasso coefficients inherit
    their shrinkage.

  }

  \subsection{Importance from the partial effects}{

    \code{importance.beta} compares the partial effect of each variable
    with the response.  Let \eqn{y}{y} be the response, \eqn{r}{r} the
    response minus \code{predicted}, and \eqn{e}{e} the partial effect
    of the variable, with \eqn{y}{y} and \eqn{e}{e} centered at their
    means.  The marginal value is
    \deqn{\frac{\mathrm{mean}\,(y - e)^2}{\mathrm{mean}\,y^2},}{mean((y - e)^2) / mean(y^2),}
    which compares the partial effect of the variable alone with the
    response.  The conditional value is
    \deqn{1 - \frac{\mathrm{mean}\,(r + e)^2 - \mathrm{mean}\,r^2}{\mathrm{mean}\,y^2},}{1 - [mean((r + e)^2) - mean(r^2)] / mean(y^2),}
    one minus the increase in the standardized prediction error when the
    partial effect is removed from the prediction.  Both values equal 1
    for a variable with no term in the forest and are smaller for a
    larger effect.  The marginal value is the smaller of the two when
    the partial effect is positively correlated with the rest of the
    prediction, as for correlated variables with effects of the same
    sign.

  }

}

\value{

  A list with the following components.

  \item{beta}{Matrix of local coefficients with one row per observation.
    Each entry is the coefficient of a term, averaged over the terminal
    nodes the observation falls in and expressed on the original scale
    of the data.  The first column (\code{betaZ}) is the intercept.}

  \item{betaZ}{Matrix with one row per observation and one column per
    retained term, describing the CART terminal nodes.  When an
    observation falls in a CART terminal node, the node mean is recorded
    for each term used in the split that created the node, and zero for
    the other terms.  These values are averaged over the trees in which
    the observation falls in a CART terminal node.  \code{NA} if the
    observation falls only in lasso terminal nodes, and \code{NULL} when
    \code{relaxed = TRUE}.}

  \item{lasso.percent}{Percentage of the terminal nodes in the forest
    that are lasso nodes.  The rest are CART terminal nodes.}

  \item{predicted}{Prediction reconstructed from the terminal node
    models.  It equals the forest prediction for the trees selected by
    \code{bag}, for example \code{predicted.oob} of the grow object when
    \code{bag = "oob"}.  When \code{relaxed = TRUE}, it is the prediction
    of the relaxed polynomials, computed by leave-one-out for the
    training data.}

  \item{partial}{Matrix of term contributions with the same layout as
    \code{beta}.  Each entry is a coefficient times the value of its
    term, averaged over trees.  The first column is the intercept.}

  \item{response.adjustment}{Change in the prediction caused by
    confining it to the inbag response range of the node, averaged over
    trees.  Zero when the restricted score is off or never takes effect,
    and when \code{relaxed = TRUE}.}

  When \code{relaxed = TRUE}, the list also contains the following
  components.

  \item{fitted}{In-sample values of the relaxed polynomials, equal to
    \code{rowSums(partial)}.}

  \item{se}{Matrix of standard errors for the coefficients in
    \code{beta}.}

  \item{nodes}{Number of terminal nodes averaged for each observation.}

  \item{nodes.used}{Fraction of the terminal nodes with enough
    out-of-bag cases for least squares.}

  \item{aliased}{Fraction of the used nodes in which each term was an
    exact linear combination of the other active terms and was dropped
    from the least squares estimate.}

  \item{relaxed}{List with the settings used, namely \code{active} and
    \code{min.extra} (the number of out-of-bag cases a node needs beyond
    its number of coefficients).}

  \code{partial.beta} is called for its plot.  It returns, invisibly, a
  list with the following components.

  \item{effect}{Matrix of partial effects with one row per observation
    and one column per plotted variable.}

  \item{spread}{Standard deviation of each partial effect over the
    observations, in the units of the response.}

  \item{group}{Factor with the group of each observation when \code{by}
    is used, and \code{NULL} otherwise.}

  \code{importance.beta} returns a data frame with one row per original
  variable and the columns \code{marginal} and \code{conditional}
  defined in Details, ordered by \code{marginal}.  A variable that the
  filter removed has the value 1 in both columns.

}

\author{
  Hemant Ishwaran and Udaya B. Kogalur
}

\seealso{
  \command{\link{rfsgt}},
  \command{\link{predict.rfsgt}},
  \command{\link{vimp.rfsgt}}
}

\examples{
\donttest{
## ------------------------------------------------------------
## local coefficients and partial effects
## ------------------------------------------------------------

## The true model is y = 2 x1 - x2 + 1.5 x1 x2 + x3^2 + noise, and x4 and
## x5 have no effect.  With hcut = 3 the model terms include all linear,
## square and pairwise product terms, which lets the terminal node
## polynomials represent the true model.
set.seed(1106)
n <- 500
d <- data.frame(matrix(runif(n * 5, -1, 1), n, 5))
d$y <- with(d, 2 * X1 - X2 + 1.5 * X1 * X2 + X3^2) + rnorm(n, sd = 0.25)

## Keep all 20 terms (filter = FALSE), use lasso terminal nodes
## everywhere (pure.lasso = TRUE), and grow shallow trees with 3 terminal
## nodes.  Each node then holds about 100 inbag cases, enough to estimate
## the coefficients well.
o <- rfsgt(y ~ ., d, hcut = 3, filter = FALSE, pure.lasso = TRUE, treesize = 3)
print(o)

## Coefficients and contributions behind the OOB predictions (the
## default bag = "oob").  The reconstructed predictions match the forest.
b <- get.beta(o)
print(all.equal(b$predicted, o$predicted.oob))

## One row per observation and one column per term.  betaZ is the
## intercept, X3_SQ2_ is the square of X3, and X1_I_X2 is the product of
## X1 and X2.
print(round(head(b$beta), 2))

## The column means estimate the true coefficients of x1, x2, x1 x2 and
## x3^2 (2, -1, 1.5 and 1), shrunk somewhat toward zero by the lasso, and
## the means of the other terms are near zero.  Deeper trees shrink the
## coefficients more (try treesize = 10).
print(round(sort(colMeans(b$beta[, -1], na.rm = TRUE), decreasing = TRUE), 2))

## Each contribution is a coefficient times the value of its term.  The
## contributions plus the response adjustment add up to the prediction.
print(round(head(b$partial), 2))
print(all.equal(rowSums(b$partial) + b$response.adjustment, b$predicted))

## Partial effects, shown by plotting the contribution of a term against
## its variable.
oldpar <- par(mfrow = c(1, 3))
plot(d$X1, b$partial[, "X1"], xlab = "x1", ylab = "contribution of x1")
plot(d$X3, b$partial[, "X3_SQ2_"], xlab = "x3", ylab = "contribution of x3^2")
plot(d$X1 * d$X2, b$partial[, "X1_I_X2"], xlab = "x1 x2",
     ylab = "contribution of x1 x2")
par(oldpar)

## Prediction example
new <- data.frame(matrix(runif(5 * 5, -1, 1), 5, 5))
p <- predict(o, new)
bn <- get.beta(p, bag = "all")
print(round(cbind(predicted = p$predicted, bn$partial), 2))

## Relaxed estimation by least squares on the OOB cases of each node.
## Without the lasso penalty and the selection effect of the splits, the
## column means are close to the true coefficients.
r <- get.beta(o, relaxed = TRUE)
print(round(sort(colMeans(r$beta[, -1], na.rm = TRUE), decreasing = TRUE), 2))
cat("fraction of nodes with enough OOB cases:", round(r$nodes.used, 2), "\n")

## OOB error of the forest compared with the leave-one-out error of the
## relaxed polynomials.
c(forest = mean((d$y - o$predicted.oob)^2, na.rm = TRUE),
  relaxed = mean((d$y - r$predicted)^2, na.rm = TRUE))

## Spread of the x1 coefficient over the observations compared with its
## standard error.  The true coefficient is constant and equal to 2.
print(c(sd = sd(r$beta[, "X1"], na.rm = TRUE), se = median(r$se[, "X1"], na.rm = TRUE)))

## Partial effect plots of the variables, here from the relaxed
## coefficients.  The effect of a variable combines all terms that
## involve it.  The true effects are a line of slope 2 for X1, a line of
## slope -1 for X2 and a parabola for X3, and X4 and X5 have no effect.
pe <- partial.beta(r, o)
print(round(pe$spread, 2))

## Effect of X1 for low, middle and high values of X2.  The product term
## 1.5 x1 x2 gives slopes near 1, 2 and 3.
partial.beta(r, o, "X1", by = "X2")

## Partial effect importance of each variable compared with the response, from the
## OOB lasso coefficients.  X4 and X5 have no effect and values near 1.
print(round(importance.beta(b, o), 3))
}
}

\keyword{regression}
