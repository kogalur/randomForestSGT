\name{print.rfsgt}
\alias{print.rfsgt}
\alias{print.vimp.rfsgt}
\title{Print Output from a Random Forest Super Greedy Tree Analysis}
\description{
  Print summary output from a Random Forest SGT analysis. This is the default
  print method for the package.
}
\usage{
\method{print}{rfsgt}(x, ...)
\method{print}{vimp.rfsgt}(x, ...)
}
\arguments{
  \item{x}{An object of class \code{"rfsgt", "grow"} or \code{"vimp.rfsgt"}.}
  \item{...}{Further arguments passed to or from other methods.}
}
\value{

  Called for its side effect of printing a summary of an \code{rfsgt}
  grow or predict object.  The return value is \code{NULL}.

}

\author{
  Hemant Ishwaran and Udaya B. Kogalur
}
\keyword{print}
