\name{predict.rfsgt}
\alias{predict.rfsgt}
\title{Prediction on Test Data for Super Greedy Forests}

\description{
  Obtain predicted values on test data using a trained super greedy
  forest.
}

\usage{\method{predict}{rfsgt}(object, newdata,  get.tree = NULL,
     block.size = 10, seed = NULL, do.trace = FALSE,...)}

\arguments{
  
  \item{object}{rfsgt object obtained from previous training call using
    \code{rfsgt}.}
  
  \item{newdata}{Test data. If not provided the training data is used
  and the original training forest is restored.}

 
  \item{get.tree}{Vector of integer(s) identifying trees over which the
    ensemble is calculated over.  By default, uses all trees in the
    forest.}

  \item{block.size}{Determines how cumulative error rate is calculated.  To
    obtain the cumulative error rate on every nth tree, set the value to
    an integer between 1 and \code{ntree}.}

  \item{seed}{Negative integer specifying seed for the random number
    generator.}

  \item{do.trace}{Number of seconds between updates to the user on
    approximate time to completion.}

  
  \item{...}{Additional options.}

}

\details{

  Returns the predicted values for a super greedy forest.
  
}

\author{

  Hemant Ishwaran and Udaya B. Kogalur

}

\references{

 Ishwaran H. (2023).  Super greedy regression trees with coordinate descent.
  Technical Report.  


}

\seealso{
  \command{\link{rfsgt}}
}

\examples{
\donttest{
## ------------------------------------------------------------
##
## restore a trained super greedy forest  
##
## ------------------------------------------------------------

## rfsgt applied to boston housing data
data(BostonHousing, package = "mlbench")
o <- rfsgt(medv~., BostonHousing)
print(o)

## restore the forest
print(predict(o))

## ------------------------------------------------------------
##
## coherence check using boston housing with factors
##
## ------------------------------------------------------------

## boston housing data: make factors
data(BostonHousing, package = "mlbench")
Boston <- BostonHousing[1:40,]
Boston$zn <- factor(Boston$zn)
Boston$chas <- factor(Boston$chas)
Boston$lstat <- factor(round(0.2 * Boston$lstat))
Boston$nox <- factor(round(20 * Boston$nox))
Boston$rm <- factor(round(Boston$rm))
     
## grow a single tree - save inbag information
o <- rfsgt(medv~., Boston, hcut=2, filter=FALSE, ntree=1, membership=TRUE, nodesize=3)

## coherence matrix
pred <- data.frame(
      inbag=o$inbag,
      pred.inb=o$predicted,
      pred.oob=o$predicted.oob,
      pred.inb.restore=predict(o)$predicted,
      pred.oob.restore=predict(o)$predicted.oob,
      pred.test=predict(o,Boston)$predicted)
print(pred)

## coherence check
cat("coherence for inbag data:", sum(pred$pred.inb-pred$pred.test,na.rm=TRUE)==0, "\n")
cat("  coherence for oob data:", sum(pred$pred.oob-pred$pred.test,na.rm=TRUE)==0, "\n")


}
}

\keyword{predict rfsgt}
