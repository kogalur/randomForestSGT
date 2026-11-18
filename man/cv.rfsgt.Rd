\name{cv.rfsgt}
\alias{cv.rfsgt}
\alias{make.folds}
\alias{print.cv.rfsgt}
\title{Cross-Validation for Super Greedy Forests}

\description{
  Estimates the prediction error of a super greedy forest by M-fold
  cross-validation.  Every step of training, including the variable
  filter and, if requested, the tuning of \code{hcut}, is repeated within
  each training fold.  The result is an honest estimate of prediction
  error, free of the optimism that the OOB error of \code{rfsgt} can
  have.  \code{make.folds} creates folds that can be reused across calls.
}

\usage{
cv.rfsgt(formula, data, M = 10, folds = NULL, tune = FALSE,
         verbose = FALSE, fold.seeds = NULL, ...)

make.folds(n, M = 10)

\method{print}{cv.rfsgt}(x, ...)
}

\arguments{

  \item{formula}{Model formula, as in \code{rfsgt}.}

  \item{data}{Data frame containing the response and the features.
    Missing and non-finite values are not allowed.}

  \item{M}{Number of folds.  Ignored when \code{folds} is given.}

  \item{folds}{Optional fold assignment, given as the output of
    \code{make.folds}, as a vector of fold labels of length \code{n}, or
    as a list with the indices of the held-out observations in each fold.
    Use the same folds in different calls to compare settings on
    identical splits of the data.  The folds used are returned.}

  \item{tune}{If \code{TRUE}, \code{tune.hcut} is run on each training
    fold and its result is used as the filter for the forest grown on
    that fold.  A list can be given instead to pass options to
    \code{tune.hcut}.  See Details.}

  \item{verbose}{Logical.  Print the error of each fold as it is
    computed?}

  \item{fold.seeds}{Optional vector of positive integers, one per fold.
    Each fold uses its seed for both the R and the native random number
    generators during tuning, training and prediction, which makes
    repeated calls on the same folds reproducible.  Cannot be combined
    with a \code{seed} passed in \code{...}.}

  \item{...}{Further arguments passed to \code{rfsgt}, such as
    \code{hcut}, \code{ntree}, \code{treesize} and \code{node.restrict}.
    \code{samp} is not allowed, because each training fold draws its own
    bootstrap samples.}

  \item{n}{Sample size, or the data frame itself.}

  \item{x}{Object of class \code{cv.rfsgt}.}

}

\details{

  The OOB error printed by \code{rfsgt} is a quick estimate of
  prediction error, but it can be optimistic.  The variable filter,
  which is on by default for \code{hcut > 1}, screens the model terms
  using all of the data, including the observations that are out-of-bag
  for a given tree.  In our tests this made the OOB error too small by
  about 5 to 10 percent.  The OOB error is also optimistic when
  \code{bsf = "oob"}, because the out-of-bag data are then used to
  compare candidate splits while each tree is grown.

  Cross-validation avoids both problems.  The data are split into
  \code{M} folds.  For each fold, \code{rfsgt} is run on the other
  \code{M - 1} folds with the arguments given in \code{...}, and the
  resulting forest predicts the held-out fold.  Filtering, and the
  tuning of \code{hcut} when \code{tune} is used, therefore see only the
  training folds, and the responses of the held-out fold play no part in
  training.  When all folds are done, every observation has exactly one
  held-out prediction.  The cross-validated error \code{err} is the mean
  squared error of these predictions, and its standard error \code{serr}
  is computed from the \code{M} fold errors.

  Because the folds can be fixed in advance, \code{cv.rfsgt} is also a
  good way to compare settings, for example different values of
  \code{hcut}, \code{treesize} or \code{node.restrict}.  Running every
  setting on the same folds removes the variation that comes from
  splitting the data differently (see the examples).  Currently only
  regression is supported.

  \subsection{Folds and reproducibility}{

    \code{make.folds} assigns the observations to \code{M} folds at
    random and returns a list with the indices of the held-out
    observations in each fold.  When \code{folds} is not given,
    \code{cv.rfsgt} calls \code{make.folds} before doing anything else,
    and calling \code{set.seed()} first makes the folds reproducible.  To
    compare settings on identical splits, create the folds once and pass
    them to every call.

    Within a fold, the results also depend on the random number
    generators used by \code{tune.hcut} and \code{rfsgt}.  To make a run
    completely reproducible, give \code{fold.seeds}.  Each fold then sets
    both the R and the native random number generators from its own seed
    before tuning, training and prediction.

  }

  \subsection{Tuning hcut within the folds}{

    With \code{tune = TRUE}, \code{tune.hcut} is run on each training
    fold, and the forest for that fold is grown with the filter it
    returns.  What happens next depends on whether \code{hcut} is given
    in \code{...}.

    If \code{hcut} is not given, each fold uses the \code{hcut} chosen by
    its own tuner.  The choice of \code{hcut} is then part of what is
    cross-validated, and the folds can end up with different values,
    which are returned in \code{hcut}.

    If \code{hcut} is given, every fold uses that value together with the
    filter the tuner found for it (see \code{use.tune.hcut}), and the
    search of the tuner is extended to include that value if necessary.
    The value each tuner would have chosen is still returned in
    \code{hcut.selected}.

    To control the tuner, pass a list instead of \code{TRUE}.  For
    example, \code{tune = list(hcut = 5)} sets the largest \code{hcut}
    the tuner considers, and other list elements are passed on to
    \code{tune.hcut}.  Unless they are set in the list, the values of
    \code{node.center} and \code{bsf} given in \code{...} are also used
    for tuning.

    A \code{tune.hcut} object can also be passed directly as
    \code{filter}, without \code{tune}.  The same filter is then used in
    every fold, and \code{cv.rfsgt} issues a warning.  If the filter was
    tuned on the full data, it has already seen the held-out
    observations, and the cross-validated error is optimistic.

  }

  \subsection{Errors in a fold}{

    If a fold fails, \code{cv.rfsgt} stops with an error of class
    \code{"cv.rfsgt.error"}.  The message names the fold and the stage
    that failed (\code{tune.hcut}, \code{use.tune.hcut}, \code{rfsgt} or
    \code{predict}).  The error object also contains the original error,
    the training and test rows of the fold, the filter, and the arguments
    of the failed call.  Catch it with \code{tryCatch} to examine or
    reproduce the failure.

  }

}

\value{

  An object of class \code{cv.rfsgt}, a list with the following
  components.

  \item{err}{Cross-validated mean squared error over all \code{n}
    held-out predictions.}

  \item{serr}{Standard error of \code{err}, computed from the fold
    errors.}

  \item{err.fold}{Mean squared error in each fold.}

  \item{hcut}{The \code{hcut} used in each fold.  It can differ between
    folds when the tuner chooses \code{hcut}.}

  \item{predicted}{Held-out prediction for each observation.}

  \item{yvar}{Response values.}

  \item{yvar.names}{Name of the response.}

  \item{folds}{List with the indices of the held-out observations in
    each fold.}

  \item{fold.seeds}{The fold seeds used, or \code{NULL}.}

  \item{M}{Number of folds.}

  \item{n}{Sample size.}

  \item{tuning}{The \code{tune.hcut} result for each fold, or
    \code{NULL} without tuning.}

  \item{filter}{The \code{tune.hcut} filter used by the forest of each
    fold, or \code{NULL}.}

  \item{hcut.selected}{The \code{hcut} chosen by the tuner in each fold,
    before any override by an \code{hcut} given in \code{...};
    \code{NA} without tuning.}

  \item{hcut.requested}{The \code{hcut} given in \code{...}, or
    \code{NULL}.}

}

\author{
  Hemant Ishwaran and Udaya B. Kogalur
}

\seealso{
  \command{\link{rfsgt}},
  \command{\link{predict.rfsgt}},
  \command{\link{tune.hcut}}
}

\examples{

\donttest{
## ------------------------------------------------------------
##
##  boston housing: OOB error versus cross-validated error
##
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {

data(BostonHousing, package = "mlbench")

## the OOB error is optimistic because the filter used every observation
print(rfsgt(medv ~ ., BostonHousing, hcut = 3))

## cross-validation repeats the filter inside each training fold
print(cv.rfsgt(medv ~ ., BostonHousing, hcut = 3))

## ------------------------------------------------------------
##
##  compare settings on identical folds
##
## ------------------------------------------------------------

set.seed(1)
folds <- make.folds(BostonHousing, M = 10)

## cross-validated error for hcut = 0 to 4
err <- sapply(0:4, function(h) {
  cv.rfsgt(medv ~ ., BostonHousing, folds = folds, hcut = h)$err
})
print(setNames(err, paste0("hcut=", 0:4)))

## restricted score (the default) versus ordinary evaluation of the
## lasso terminal nodes
print(c(restricted = cv.rfsgt(medv ~ ., BostonHousing, folds = folds, hcut = 3)$err,
        ordinary = cv.rfsgt(medv ~ ., BostonHousing, folds = folds, hcut = 3,
                            node.restrict = FALSE)$err))

## ------------------------------------------------------------
##
##  tune hcut inside every fold, using mtcars for speed
##
## ------------------------------------------------------------

## the tuner considers hcut up to 4, and each fold uses the value its
## tuner selects
cvt <- cv.rfsgt(mpg ~ ., mtcars, tune = list(hcut = 4), verbose = TRUE)
print(cvt)

## the hcut selected in each fold
print(cvt$hcut)

}
}
}

\keyword{cv.rfsgt}
