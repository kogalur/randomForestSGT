\name{internal.utils}


\alias{filter.rfsgt}
\alias{filter.custom.rfsgt}

\alias{make.baselearner}

\alias{tune.hcut}
\alias{use.tune.hcut}

\alias{vimp.rfsgt}

\title{Utility Functions for Random Forest Super Greedy Trees}

\description{
  These are internal utility functions exported for advanced usage. This
  Rd file is used solely to register aliases.
}

\value{

  Return values depend on the helper function used:

  \describe{

    \item{\code{filter.rfsgt} and \code{filter.custom.rfsgt}}{Character
      vector of variables or generated base-learner terms retained by the
      filtering step.}

    \item{\code{make.baselearner}}{A data frame or matrix containing the
      original predictors together with generated polynomial or
      interaction base-learner terms.}

    \item{\code{tune.hcut}}{An object of class \code{"tune.hcut"}.  This
      is a character vector of selected terms with attributes including
      \code{all}, \code{formula}, \code{formula.all}, \code{term.map},
      \code{term.map.all}, \code{cv}, \code{bsf}, \code{hcut}, and
      \code{hcutSeq}.}

    \item{\code{use.tune.hcut}}{A \code{"tune.hcut"} object restricted to
      the requested \code{hcut}, with tuning attributes preserved.}

    \item{\code{vimp.rfsgt}}{Variable-importance scores, typically
      returned as a named numeric vector.}

  }

}

\keyword{internal}
