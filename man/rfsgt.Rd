\name{rfsgt}
\alias{rfsgt}
\alias{filter.rfsgt}
\alias{filter.custom.rfsgt}
\alias{get.beta}
\alias{tune.hcut}
\alias{use.tune.hcut}
\alias{vimp.rfsgt}
\alias{make.baselearner}
\title{Random Forest Super Greedy Trees}
\description{
  Grow a forest of Super Greedy Trees (SGTs) using lasso.
}

\usage{
rfsgt(formula,
      data,
      ntree = 100,
      hcut = 1,
      treesize = NULL,
      nodesize = NULL,
      tune.treesize = FALSE,
      filter = (hcut > 1),
      keep.only = NULL,
      fast = TRUE,
      pure.lasso = FALSE,
      eps = .005,
      maxit = 500,
      nfolds = 10,
      block.size = 10,
      bootstrap = c("by.root", "none", "by.user"),
      samptype = c("swor", "swr"),  samp = NULL, membership = TRUE,
      sampsize = if (samptype == "swor") function(x){x * .632} else function(x){x},
      seed = NULL,
      do.trace = FALSE,
      ...)
}

\arguments{

  \item{formula}{Formula describing the model to be fit.}

  \item{data}{Data frame containing response and features.}

  \item{ntree}{Number of trees to grow.}

  \item{hcut}{Integer value indexing type of parametric regression model
    to use for splitting.  See details below.}

  \item{treesize}{Function specifying size of tree (number of terminal
    nodes) where first input is \code{n} sample size and second input is
    \code{hcut}.  Can also be supplied as an integer value and defaults
    to an internal function if unspecified.  If
    \code{tune.treesize}=TRUE, this is the maximum number of allowable
    splits.  If \code{tune.treesize}=FALSE, this is the target number of
    tree splits.}

  \item{nodesize}{Minumum size of terminal node.  Set internally if
    not specified.}

  \item{tune.treesize}{Adaptively determine the optimal tree size using
    out-of-bag empirical risk?}
    
  \item{filter}{Logical value specifying whether dimension reduction
    (filtering) of features should be performed.Can also be specified
    using the helper function \code{tune.hcut} which performs dimension
    reduction prior to fitting.  See examples below.}

  \item{keep.only}{Character vector specifying the features of interest.
  The data is pre-filtered to keep only these requested variables.
  Ignored if filter is specified using \code{tune.hcut}.}

  \item{fast}{Use fast filtering?}
    
  \item{pure.lasso}{Logical value specifying whether lasso splitting
  should be strictly adhered to.  In general, lasso splits are replaced
  with CART whenever numerical instability occurs (for example, small
  node sample sizes may make it impossible to obtain the cross-validated
  lasso parameter).  This option will generally produce shallow trees which
  not be appropriate in all settings.}
  
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

  \item{...}{Further arguments passed to \command{cdlasso} and
    \command{rfsrc}.}
  

}

\details{

  A flexible class of parametric models are used for tree splitting
  using lasso.  This includes CART splits, hyperplane, ellipsoid and
  hyperboloid cuts.  Coordinate descent is used for fast calculation of
  the penalized lasso parametric models.  Cross-validation is employed
  to obtain the lasso regularization parameter.

  These trees are called super greedy trees (SGTs) and are constructed
  using best split first (BSF) where cuts are made sequentially in order
  of greatest empirical risk reduction.

  Parametric linear models used for splitting are indexed by parameter
  \code{hcut} corresponding to the following geometric regions:

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
 random forests.

}

\value{

  A forest of SGTs trained on the learning data which can be used for
  prediction.
  
}

\author{

  Hemant Ishwaran and Udaya B. Kogalur

}

\references{
  
  Ishwaran H. (2023).  Super greedy regression trees with coordinate descent.
  Technical Report.  
  
}

\seealso{

  \command{\link{cdlasso}}
  
}


\examples{
\donttest{
  
## ------------------------------------------------------------
##
##  boston housing
##
## ------------------------------------------------------------

## load the data
data(BostonHousing, package = "mlbench")

## default basic call
print(rfsgt(medv~., BostonHousing))

## variable selection
sort(vimp.rfsgt(medv~.,BostonHousing))

## examples of hcut=0 (similar to random forests ... but using BSF)
print(rfsgt(medv~., BostonHousing, hcut=0))
print(rfsgt(medv~., BostonHousing, hcut=0, nodesize=1))

## hcut=1 with smaller nodesize
print(rfsgt(medv~., BostonHousing, nodesize=1))

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
print(rfsgt(medv~., Boston, hcut=0, nodesize=1, ntree=500))

## hcut=3
print(rfsgt(medv~., Boston, hcut=3))


## ------------------------------------------------------------
##
##  ozone
##
## ------------------------------------------------------------

## load the data
data(Ozone, package = "mlbench")

print(rfsgt(V4~., na.omit(Ozone), hcut=0, nodesize=1, ntree=500))
print(rfsgt(V4~., na.omit(Ozone), hcut=1))
print(rfsgt(V4~., na.omit(Ozone), hcut=2))
print(rfsgt(V4~., na.omit(Ozone), hcut=3))


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
rO <- lapply(0:4, function(hcut) {
  cat("hcut", hcut, "\n")
  rfsgt(y~., nl2d, ntree=1, hcut=hcut, treesize=treesize, bootstrap="none",
         tune.treesize=TRUE, nodesize=1, strikeout=10000, filter=FALSE)
})

## nice little wrapper for plotting results
if (library("interp", logical.return = TRUE)) {
  ## nice little wrapper for plotting results
  plot.image <- function(x, y, z, linear=TRUE, nlevels=40, points=FALSE) {
    xo <- x; yo <- y
    so <- interp(x=x, y=y, z=z, linear=linear, nx=nlevels, ny=nlevels)
    x <- so$x; y <- so$y; z <- so$z
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

  par(mfrow=c(3,2))
  image(x1, x2, truth, xlab="", ylab="")
  contour(x1, x2, truth, nlevels = 15, add = TRUE, drawlabels = FALSE)
  mtext(expression(x[1]),1,line=2)
  mtext(expression(x[2]),2,line=2)
  mtext(expression("truth"),3,line=1)

  lapply(0:4, function(j) {
    plot.image(nl2d.tst[,"X1"],nl2d.tst[,"X2"], predict(rO[[j+1]], nl2d.tst)$predicted)
    contour(x1, x2, truth, nlevels = 15, add = TRUE, drawlabels = FALSE)
    mtext(expression(x[1]),1,line=2)
    mtext(expression(x[2]),2,line=2)
    mtext(paste0("hcut=",j),3,line=1)
  })

 
}

## ------------------------------------------------------------
##
##  friedman illustration of OOB empirical risk
##  (note: tune.treesize=TRUE required to obtain empirical risk)
##
## ------------------------------------------------------------

## simulate friedman
n <- 500
dta <- data.frame(mlbench:::mlbench.friedman1(n, sd=0))

## rf versus rfsgt
o1 <- rfsgt(y~., dta, hcut=0, tune.treesize=TRUE, block.size=1)
o2 <- rfsgt(y~., dta, hcut=3, tune.treesize=TRUE, block.size=1)

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
par(mfrow=c(2,2))

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


## ------------------------------------------------------------
##
## synthetic regression examples with different hcut
##
## ------------------------------------------------------------

## simulation functions
sim <- list(
     friedman1=function(n){data.frame(mlbench:::mlbench.friedman1(n))},
     friedman2=function(n){data.frame(mlbench:::mlbench.friedman2(n))},
     friedman3=function(n){data.frame(mlbench:::mlbench.friedman3(n))},
     peak=function(n){data.frame(mlbench:::mlbench.peak(n, 10))},       
     linear=function(n, sd=.1){
           x=matrix(runif(n*10), n)
           y=3*x[,1]^3-2*x[,2]^2+3*x[,3]+rnorm(n,sd=sd)
           data.frame(y=y,x)
})

## run rfsgt on the simulations
n <- 500
max.hcut <- 3
results <- lapply(names(sim), function(nm) {
  cat("simulation:", nm, "\n")
  d <- sim[[nm]](n=n)
  rO <- data.frame(do.call(rbind, lapply(0:max.hcut, function(hcut) {
   cat("     hcut:", hcut, "\n")
   o <- rfsgt(y~.,d,hcut=hcut)
   c(hcut, tail(o$err.rate, 1), tail(o$err.rate, 1) / var(o$yvar))
  })))
  colnames(rO) <- c("hcut", "mse", "smse")
  rO
})
names(results) <- names(sim)

## print results
print(results)

## ------------------------------------------------------------
##
## iowa housing data 
##
## ------------------------------------------------------------

data(housing, package = "randomForestSRC")

## remove PID 
housing$PID <- NULL

## rough missing data imputation
d <- data.frame(randomForestSRC:::get.na.roughfix(data.matrix(housing)))
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
print(sort(vimp.rfsgt(y~.,d)))

## internal filtering function can also be used
cat("variables selected by filter.rfsgt:\n")
print(filter.rfsgt(y~.,d, method="conserve"))

## ------------------------------------------------------------
##
## pre-filtering using keep.only
##
## ------------------------------------------------------------

## simulate the data
n <- 100
p <- 50
noise <- matrix(runif(n * p), ncol=p)
dta <- data.frame(mlbench:::mlbench.friedman1(n, sd=0), noise=noise)

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
dta <- data.frame(mlbench:::mlbench.friedman1(n, sd=0), noise=noise)

## tune hcut
f <- tune.hcut(y~., dta, hcut=3)

## use the optimized hcut
print(rfsgt(y~.,dta, filter=f))

## over-ride the tuned hcut value
print(rfsgt(y~.,dta, filter=use.tune.hcut(f, hcut=1)))
print(rfsgt(y~.,dta, filter=use.tune.hcut(f, hcut=2)))
print(rfsgt(y~.,dta, filter=use.tune.hcut(f, hcut=3)))


}
}

\keyword{rfsgt}



  


