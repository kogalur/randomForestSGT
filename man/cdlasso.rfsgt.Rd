\name{cdlasso.rfsgt}
\alias{cdlasso.rfsgt}
\alias{cdlasso}
\title{Coordinate Descent Lasso}
\description{
  Fit lasso for regression using coordinate descent.
}

\usage{
cdlasso(formula,
      data,
      nfolds = 0,
      weights = NULL,
      nlambda = 100,
      lambda.min.ratio = ifelse(n < n.xvar, 0.01, 1e-04),
      lambda = NULL,
      threshold = 1e-7,
      eps = .0001,
      maxit = 5000,
      efficiency = ifelse(n.xvar < 500, "covariance", "naive"),
      seed = NULL,
      do.trace = FALSE)
}

\arguments{

  \item{formula}{Formula describing the model to be fit.}

  \item{data}{Data frame containing response and features.}

  \item{nfolds}{Number of cross-validation folds where default is 0
    corresponding to no cross-validation.}

  \item{weights}{Observation weights. Default is 1 for each observation.}

  \item{nlambda}{The number of \code{lambda} values; default is 100.}
	  
  \item{lambda.min.ratio}{Smallest value for \code{lambda}, as a
    fraction of \code{lambda.max} which equals smallest value for which
    all coefficients are zero.  A very small value of
    \code{lambda.min.ratio} will lead to a saturated fit in if number of
    observations \code{n} is less than number of features
    \code{n.xvar}.}

  \item{lambda}{Lasso \code{lambda} sequence.  Default is an internally
    selected sequence based on \code{nlambda} and
    \code{lambda.min.ratio}. For experts only.}

  \item{threshold}{Convergence threshold for coordinate descent. Each
    inner coordinate-descent loop continues until the maximum change in
    the objective after any coefficient update is less than
    \code{threshold} times the null deviance.}

  \item{eps}{Multiplication factor applied to \code{lambda.min.ratio}
    used to define the smallest \code{lambda} value.}

  \item{maxit}{Maximum number of passes over the data for all
    \code{lambda} values.}
  
  \item{efficiency}{Switches the algorithm to efficiency or naive mode
    depending on number of variables.  Efficiency \code{covariance}
    saves all inner-products and can be significantly faster in certain
    settings than \code{naive} which loops through all values \code{n}
    each time an inner-product is formed.} 

  \item{seed}{Negative integer specifying seed for the random number
    generator.}

  \item{do.trace}{Number of seconds between updates to the user on
    approximate time to completion.}

}

\details{

Use coordinate descent to fit lasso to a regression model.  
  
}

\value{

  A list containing the fitted lasso solution path.  The list contains:

  \describe{

    \item{\code{convgCount}}{Convergence counter returned by the
      coordinate-descent routine.}

    \item{\code{lambdaCount}}{Number of \code{lambda} values in the
      fitted solution path.}

    \item{\code{lambda}}{The sequence of \code{lambda} values used.}

    \item{\code{beta}}{Matrix of regression coefficients for the lasso
      solution path.  Rows correspond to values in \code{lambda}; columns
      contain the intercept followed by the encoded predictor variables.}

    \item{\code{xvar}}{Numeric predictor matrix used in the fit, after
      any internal encoding of the supplied data.}

    \item{\code{yvar}}{Response vector used in the fit.}

    \item{\code{yHat}}{Cross-validated fitted values or predictions by
      \code{lambda} and observation.  Returned only when cross-validation
      output is available, such as when \code{nfolds} is greater than 1.}

    \item{\code{lambda.min.indx}}{Index of the \code{lambda} value with
      minimum cross-validation error.  Returned only when cross-validation
      output is available.}

    \item{\code{lambda.1se.min.indx}}{Index of the minimum
      \code{lambda} value within one standard error of the minimum
      cross-validation error.  Returned only when cross-validation output
      is available.}

    \item{\code{lambda.1se.max.indx}}{Index of the maximum
      \code{lambda} value within one standard error of the minimum
      cross-validation error.  Returned only when cross-validation output
      is available.}

    \item{\code{lambda.cvm}}{Mean cross-validation error for each
      \code{lambda}.  Returned only when cross-validation output is
      available.}

    \item{\code{lambda.cvsd}}{Cross-validation standard-error values for
      each \code{lambda}.  Returned only when cross-validation output is
      available.}

    \item{\code{ctime.internal}}{Timing information reported by the
      native coordinate-descent routine.}

    \item{\code{ctime.external}}{Elapsed R-side timing, computed from
      \code{proc.time()}.}

  }

}

\author{
 Hemant Ishwaran and Udaya B. Kogalur
}

\references{
  
  Friedman, J., Hastie, T. and Tibshirani, R. (2010) Regularization
  paths for generalized linear models via coordinate descent,
  \emph{J. of Statistical Software}, 33(1):1-22.

  
}

\seealso{

  \command{\link{rfsgt}}
  
}


\examples{
\donttest{

## ------------------------------------------------------------
## regression example: boston housing
## ------------------------------------------------------------

if (requireNamespace("mlbench", quietly = TRUE)) {
## load the data
data(BostonHousing, package = "mlbench")

## 10-fold validation
o <- cdlasso(medv ~., BostonHousing, nfolds=10)

## lasso solution
bhat <- data.frame(bhat.min=o$beta[o$lambda.min.indx,],
                   bhat.1se=o$beta[o$lambda.1se.max.indx[1],])
print(bhat)

## compare to results from glmnet
if (library("glmnet", logical.return = TRUE)) {

  oo <- cv.glmnet(data.matrix(o$xvar), o$yvar, nfolds=10)
  bhat2 <- cbind(data.matrix(coef(oo, s=oo$lambda.min)),
                 data.matrix(coef(oo, s=oo$lambda.1se)))
  rownames(bhat2) <- rownames(bhat)
  print(bhat2)

}
}
}
}
\keyword{lasso}



  


