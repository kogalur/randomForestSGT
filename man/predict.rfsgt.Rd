\name{predict.rfsgt}
\alias{predict.rfsgt}
\title{Prediction on Test Data for Super Greedy Forests}

\description{
  Obtain predicted values from a trained super greedy forest.
  Predictions can be made for new data or, when no new data are given,
  for the training data, in which case the original forest is restored.
}

\usage{
\method{predict}{rfsgt}(object, newdata, get.tree = NULL,
        block.size = 10, seed = NULL, do.trace = FALSE,
        node.restrict = NULL, ...)
}

\arguments{

  \item{object}{An object of class \code{(rfsgt, grow)} returned by
    \code{rfsgt}.}

  \item{newdata}{Test data.  If omitted, the training data are used and
    the original training forest is restored (see Details).}

  \item{get.tree}{Vector of integers identifying the trees used for the
    ensemble.  By default all trees in the forest are used.}

  \item{block.size}{Controls how the cumulative error rate is computed.
    The error is recorded after every \code{block.size} trees; use an
    integer between 1 and \code{ntree}.}

  \item{seed}{Negative integer specifying the seed for the random number
    generator.}

  \item{do.trace}{Number of seconds between progress updates on the
    approximate time to completion.}

  \item{node.restrict}{Logical or \code{NULL}, controlling whether lasso
    terminal nodes use the restricted score (see Details).  The default
    \code{NULL} uses the setting saved in the forest.  \code{TRUE} or
    \code{FALSE} overrides the saved setting for this call only; the
    forest itself is not changed.}

  \item{...}{Additional options.}

}

\details{

  To predict an observation, each tree sends the case from the root down
  to a terminal node.  When \code{hcut > 0}, the splits and the terminal
  node predictions are both based on scores.  A score is a sparse lasso
  combination of the dictionary terms, which can include squares,
  products and higher powers of the variables.  With \code{node.center =
  TRUE}, the terms are formed from variables centered at the node.  At
  an internal node, the observation goes to the left daughter if its
  score is at or below the threshold of the split, and to the right
  daughter otherwise.  Once the observation reaches a terminal node, the
  node predicts its response using a sparse polynomial estimated by the
  lasso from the terminal node's inbag cases.  When the tree has
  switched to CART (see \code{rfsgt}), the split is a standard CART cut,
  and the terminal node predicts values using the mean of the inbag
  responses.  With \code{hcut = 0} every node is a CART node, in which
  case the forest is similar to a random forest.

  The function works in two modes.  Given \code{newdata}, it predicts
  the new observations.  Without \code{newdata}, it restores the
  training forest and reproduces the inbag and out-of-bag predictions of
  the grow object.  Restoring is useful for recomputing the OOB
  predictions and error rate with a subset of trees (\code{get.tree}) or
  with a different \code{node.restrict} setting.

  \subsection{Restricted score}{

    A lasso terminal node score uses a polynomial estimated from its
    inbag cases.  An observation that reaches the node can lie outside
    the region those cases occupy, and there a square or product can take
    values that no inbag case produced.  The prediction can then fall far
    outside the range of the response, most often in small trees and at
    larger \code{hcut}.

    The restricted score prevents this.  With \code{node.restrict = TRUE},
    each variable is first clamped to its range among the inbag cases of
    the node, which places the observation at the nearest point of the
    inbag box.  The polynomial is evaluated at that point, and the result
    is then confined to the range of the inbag responses.  The restriction
    applies to every observation in \code{newdata} and, when the training
    forest is restored, to the out-of-bag observations.  Inbag
    observations are evaluated as usual.  The restriction does not change
    which terminal node an observation reaches, because splits are always
    evaluated in the ordinary way, and it has no effect on CART terminal
    nodes.  It works the same way with and without \code{node.center}.

    The default \code{node.restrict = NULL} uses the value originally
    used in the \code{rfsgt} training forest call (\code{TRUE} by default).

  }

  \subsection{Restoring the training forest}{

    When \code{newdata} is omitted, the training data are run down the
    trees again.  For each training observation, \code{predicted}
    averages the trees whose bootstrap sample contains the observation,
    evaluated exactly as during growth, and \code{predicted.oob} averages
    the remaining trees, using the restricted score when it is on.  With
    the saved \code{node.restrict} setting, these reproduce
    \code{predicted} and \code{predicted.oob} of the grow object, and
    \code{err.rate} is the OOB error.

  }

  \subsection{Predicting new data}{

    \code{newdata} must contain the variables used for training.  They
    are encoded, filtered and expanded into model terms exactly as in
    training.  The response is optional; if present, it is used only to
    compute \code{err.rate}.  Every tree in \code{get.tree} contributes to
    the prediction of every observation.

    Passing the training data as \code{newdata} is not the same as
    restoring the forest.  The observations are then treated as new
    data.  Every tree contributes, including the trees for which an
    observation was inbag, and the restricted score is applied to every
    observation.  The result therefore matches neither \code{predicted}
    nor \code{predicted.oob} of the grow object.

  }

  \subsection{Explaining predictions}{

    \code{get.beta} breaks each prediction down into local coefficients
    and the contributions of the individual terms.  For new data, call
    \code{get.beta(pred, bag = "all", grow = fit)}, where \code{pred} is
    the prediction object and \code{fit} is the grow object.

  }

}

\value{

  An object of class \code{c("rfsgt", "predict", family)}.  When
  \code{newdata} is omitted it describes the restored training forest;
  otherwise it describes the predictions for the new data.  The object
  is a list with the following components.

  \describe{

    \item{\code{call}}{The matched call.}

    \item{\code{family}}{Model family of the forest.}

    \item{\code{n}}{Number of observations predicted.}

    \item{\code{samptype}}{Sampling type used to grow the forest.}

    \item{\code{sampsize}}{Bootstrap sample size of each tree.}

    \item{\code{ntree}}{Number of trees in the forest.}

    \item{\code{get.tree}}{Indices of the trees used for the prediction.
      \code{get.beta} also uses them.}

    \item{\code{hcut}}{\code{hcut} value of the forest.}

    \item{\code{node.restrict}}{Restricted score setting used for this
      call.}

    \item{\code{splitrule}}{Split rule of the forest.}

    \item{\code{yvar}}{Response values used to compute the error rate.
      These are the training responses when the forest is restored, and
      the response in \code{newdata} if it is present; otherwise
      \code{NULL}.}

    \item{\code{yvar.names}}{Name of the response.}

    \item{\code{xvar}}{Variables of the predicted observations, after the
      same encoding and filtering as in training.}

    \item{\code{xvar.augment}}{Values of the additional model terms
      (squares, products and higher powers) for the predicted
      observations, or \code{NULL} when the forest uses the variables
      only.}

    \item{\code{xvar.names}}{Names of the retained variables.}

    \item{\code{xvar.augment.names}}{Names of the additional model terms,
      or \code{NULL}.}

    \item{\code{xvar.info}}{Encoding information used to align new data
      with the training data.}

    \item{\code{term.map}}{Map from each model term to the variables and
      powers that form it.}

    \item{\code{leaf.count}}{Number of terminal nodes in each tree.}

    \item{\code{forest}}{The forest used to make the predictions.}

    \item{\code{membership}}{Matrix giving the terminal node of each
      observation in each tree, if membership output was requested;
      otherwise \code{NULL}.}

    \item{\code{inbag}}{Matrix of bootstrap counts for each observation
      and tree, when the forest is restored and membership output was
      requested; otherwise \code{NULL}.}

    \item{\code{block.size}}{Block size used for the cumulative error
      rate.}

    \item{\code{perf.type}}{Internal code for the performance measure.}

    \item{\code{predicted}}{Predicted values.  For new data, the average
      over the trees in \code{get.tree}.  For the restored forest, the
      inbag prediction, which averages the trees whose bootstrap sample
      contains the observation.}

    \item{\code{predicted.oob}}{Out-of-bag predictions when the forest is
      restored; \code{NULL} for new data.}

    \item{\code{ambrOffset}}{Internal matrix locating the terminal node
      of each new observation in each tree, used by \code{get.beta};
      \code{NULL} when the forest is restored.}

    \item{\code{err.rate}}{Cumulative mean squared error, recorded after
      every \code{block.size} trees, when response values are available;
      otherwise \code{NULL}.}

    \item{\code{ctime.internal}}{Computing time reported by the native
      code.}

    \item{\code{ctime.external}}{Elapsed time measured in R with
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
  \command{\link{rfsgt}},
  \command{\link{get.beta}},
  \command{\link{cv.rfsgt}}
}

\examples{

## ------------------------------------------------------------
##
##  mtcars: for CRAN testing
##
## ------------------------------------------------------------

o <- rfsgt(mpg~., mtcars[1:20,], ntree=3, treesize=1)
p <- predict(o, mtcars[-(1:20),])
print(o)
print(p)

\donttest{
## ------------------------------------------------------------
##
## train/test using Friedman 3
##
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {

  ## small forest of three trees
  d.trn <- data.frame(mlbench::mlbench.friedman3(100))
  o <- rfsgt(y ~ ., d.trn, hcut = 1, ntree = 3, treesize = 1)
  print(o)

  ## test set mean squared error
  d.tst <- data.frame(mlbench::mlbench.friedman3(200))
  y.tst <- d.tst$y
  x.tst <- d.tst[, colnames(d.tst) != "y"]
  yhat <- predict(o, x.tst)$predicted
  mean((yhat - y.tst)^2)

## full forest with hcut = 3
d.trn <- data.frame(mlbench::mlbench.friedman3(500))
o <- rfsgt(y~.,d.trn, hcut=3)
print(o)

## test set mean squared error on 1000 new observations
d.tst <- data.frame(mlbench::mlbench.friedman3(1000))
y.tst <- d.tst$y
x.tst <- d.tst[, colnames(d.tst)!= "y"]
yhat <- predict(o, x.tst)$predicted
cat("test set mse:", mean((yhat - y.tst)^2), "\n")

## ------------------------------------------------------------
##
## restore a trained forest using Boston housing
##
## ------------------------------------------------------------

## grow the forest
data(BostonHousing, package = "mlbench")
o <- rfsgt(medv~., BostonHousing)
print(o)

## restore the forest; the OOB error matches the grow object
print(predict(o))

## ------------------------------------------------------------
##
## restored versus test predictions, using Boston housing
## with factors
##
## ------------------------------------------------------------

## turn some of the variables into factors
data(BostonHousing, package = "mlbench")
Boston <- BostonHousing[1:40,]
Boston$zn <- factor(Boston$zn)
Boston$chas <- factor(Boston$chas)
Boston$lstat <- factor(round(0.2 * Boston$lstat))
Boston$nox <- factor(round(20 * Boston$nox))
Boston$rm <- factor(round(Boston$rm))

## grow a single tree and keep the inbag information
o <- rfsgt(medv~., Boston, hcut=2, filter=FALSE, ntree=1, membership=TRUE, nodesize=3)

## predictions from the grow object, from the restored forest, and from
## the training data passed as new data, with and without the
## restricted score
pred <- data.frame(
      inbag=o$inbag,
      pred.inb=o$predicted,
      pred.oob=o$predicted.oob,
      pred.inb.restore=predict(o)$predicted,
      pred.oob.restore=predict(o)$predicted.oob,
      pred.test=predict(o,Boston)$predicted,
      pred.test.unrestricted=predict(o,Boston,node.restrict=FALSE)$predicted)
print(pred)

## Inbag observations are evaluated without the restriction and OOB
## observations with it.  The inbag predictions therefore match the
## unrestricted test predictions, and the OOB predictions match the
## restricted ones.
cat("coherence for inbag data:", sum(pred$pred.inb-pred$pred.test.unrestricted,na.rm=TRUE)==0, "\n")
cat("  coherence for oob data:", sum(pred$pred.oob-pred$pred.test,na.rm=TRUE)==0, "\n")

## train on half of the data and predict the other half
trn <- sample(1:nrow(Boston), nrow(Boston)/2, replace=FALSE)
o.trn <- rfsgt(medv~., Boston[trn,], hcut=2)
print(predict(o.trn,Boston[-trn,]))

## ------------------------------------------------------------
## prediction after tuning hcut and filtering with tune.hcut
## ------------------------------------------------------------

## grow the forest with the tuned hcut and filter
dta <- data.frame(mlbench::mlbench.friedman3(500))
f <- tune.hcut(y~., dta, hcut=5, verbose=TRUE)
o <- rfsgt(y~., dta, filter=f)
print(o)

## test the tuned forest on new data
print(predict(o, data.frame(mlbench::mlbench.friedman3(25000))))

## override the tuned hcut
o2 <- rfsgt(y~., dta, filter=use.tune.hcut(f, hcut=2))
print(o2)
print(predict(o2, data.frame(mlbench::mlbench.friedman3(25000))))

}
}
}

\keyword{predict rfsgt}
