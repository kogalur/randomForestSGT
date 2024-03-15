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
          \code{lambda.min.ratio}.  For experts only.}

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

  Lasso solution path with the following values.

  \item{beta}{Matrix containing beta values for the lasso solution path.}

  \item{lambda}{The sequence of \code{lambda} values used.}

  \item{lambda.min.indx}{Index for value of \code{lambda} that gives the
  minimum cross-validation error.  Only applies if \code{nfolds} is
  greater than 1.}

  \item{lambda.1se.indx}{Index for largest value of \code{lambda} such
    that error is within 1 standard error of the minimum
    cross-validation error. Only applies if \code{nfolds} is greater
    than 1.}

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

## load the data
data(BostonHousing, package = "mlbench")

## 10-fold validation
o <- cdlasso(medv ~., BostonHousing, nfolds=10)

## lasso solution
bhat <- data.frame(bhat.min=o$beta[o$lambda.min.indx,],
                   bhat.1se=o$beta[o$lambda.1se.indx[1],])
print(bhat)

## compare with glmnet
if (library("glmnet", logical.return = TRUE)) {

  oo <- cv.glmnet(data.matrix(o$xvar), o$yvar, nfolds=10)
  bhat2 <- cbind(data.matrix(coef(oo, s=oo$lambda.min)),
                 data.matrix(coef(oo, s=oo$lambda.1se)))
  rownames(bhat2) <- rownames(bhat)
  print(bhat2)

}

}
}
\keyword{lasso}



  


