####################################################################
##
## Time Dependent Covariates Related Functions
##
####################################################################
get.tdc.cov <- function(dta) {
  ## extract covariates
  x <- dta[, !(colnames(dta) %in% c("id", "start", "stop", "event")), drop = FALSE]
  ## for looping across the sorted id
  id <- dta$id
  id.unq <- sort(unique(id))
  ## iterate by id, check if i's data is duplicated - should work for real and character
  tdcm <- do.call(rbind, mclapply(id.unq, function(ii) {
    get.duplicated(x[id == ii,, drop = FALSE])
  }))
  ## return 0 for a covariate if x is duplicated for each id value
  ## value of 1 represents a tdc covariate
  c(apply(tdcm, 2, function(xx) {
    1 * !all(xx == 0)
  }))  
}
get.tdc.subj.time <- function(dta) {
  ## first check if there are time dependent covariates
  if (sum(get.tdc.cov(dta)) == 0) {
    return(rep(0, nrow(dta)))
  }
  ## extract covariates
  x <- dta[, !(colnames(dta) %in% c("id", "start", "stop", "event")), drop = FALSE]
  ## for looping across the id (id should *not* be sorted)
  id <- dta$id
  id.unq <- unique(id)
  ## iterate by id, check if i's data is duplicated - should work for real and character
  tdcm <- do.call(rbind, mclapply(id.unq, function(ii) {
    get.duplicated(x[id == ii,, drop = FALSE])
  }))
  ## return 1 if subject i has any tdc covariate
  c(apply(tdcm, 1, function(xx) {
    1 * any(xx != 0)
  }))
}
get.grow.event.info <- function(yvar, fmly, need.deaths = TRUE, ntime = NULL) {
  if (grepl("surv", fmly)) {
    ##-----------------------------------------------------------
    ## survival, competing risks, or time dependent covariates
    ##-----------------------------------------------------------
    if (dim(yvar)[2] == 2) {
      ##---------------------------------
      ## survival or competing risks:
      ##---------------------------------
      r.dim <- 2
      time <- yvar[, 1]
      cens <- yvar[, 2]
      start.time <- NULL
      ## censoring must be coded coherently
      if (!all(floor(cens) == abs(cens), na.rm = TRUE)) {
        stop("for survival families censoring variable must be coded as a non-negative integer (perhaps the formula is set incorrectly?)")
      }
      ## check if deaths are available (if user specified)
      if (need.deaths && (all(na.omit(cens) == 0))) {
        stop("no deaths in data!")
      }
      ## Check for event time consistency.
      ## we over-ride this now to allow for negative time (see Stute)
      ##if (!all(na.omit(time) >= 0)) {
      ##  stop("time must be  positive")
      ##}
      ## Extract the unique event types.
      event.type <- unique(na.omit(cens))
      ## Ensure they are all greater than or equal to zero.
      if (sum(event.type >= 0) != length(event.type)) {
        stop("censoring variable must be coded as NA, 0, or greater than 0.")
      }
      ## Discard the censored state, if it exists.
      event <- na.omit(cens)[na.omit(cens) > 0]
      event.type <- unique(event)
      ## Set grid of time points.
      nonMissingOutcome <- which(!is.na(cens) & !is.na(time))
      nonMissingDeathFlag <- (cens[nonMissingOutcome] != 0)
      time.interest <- sort(unique(time[nonMissingOutcome[nonMissingDeathFlag]]))
      ## trim the time points if the user has requested it
      ## we also allow the user to pass requested time points
      if (!is.null(ntime) && !((length(ntime) == 1) && ntime == 0)) {
        if (length(ntime) == 1 && length(time.interest) > ntime) {
          time.interest <- time.interest[
            unique(round(seq.int(1, length(time.interest), length.out = ntime)))]
        }
        if (length(ntime) > 1) {
          time.interest <- unique(sapply(ntime, function(tt) {
            time.interest[max(1, sum(tt >= time.interest, na.rm = TRUE))]
          }))
        }
      }
    }
    ##-------------------------------
    ## time dependent covariates:
    ##-------------------------------
    else {
      r.dim <- 3
      start.time <- yvar[, 1]
      time <- yvar[, 2]
      cens <- yvar[, 3]
      ## censoring must be coded coherently
      if (!all(floor(cens) == abs(cens), na.rm = TRUE)) {
        stop("for survival families censoring variable must be coded as a non-negative integer (perhaps the formula is set incorrectly?)")
      }
      ## check if deaths are available (if user specified)
      if (need.deaths && (all(na.omit(cens) == 0))) {
        stop("no deaths in data!")
      }
      ## Check for event time consistency.
      if (!all(na.omit(time) >= 0)) {
        stop("time must be  positive")
      }
      ## Extract the unique event types.
      event.type <- unique(na.omit(cens))
      ## Ensure they are all greater than or equal to zero.
      if (sum(event.type >= 0) != length(event.type)) {
        stop("censoring variable must be coded as NA, 0, or greater than 0.")
      }
      ## Discard the censored state, if it exists.
      event <- na.omit(cens)[na.omit(cens) > 0]
      event.type <- unique(event)
      ## Set grid of time points.
      nonMissingOutcome <- which(!is.na(cens) & !is.na(time))
      nonMissingDeathFlag <- (cens[nonMissingOutcome] != 0)
      time.interest <- sort(unique(time[nonMissingOutcome[nonMissingDeathFlag]]))
      ## trim the time points if the user has requested it
      ## we also allow the user to pass requested time points
      if (!is.null(ntime) && !((length(ntime) == 1) && ntime == 0)) {
        if (length(ntime) == 1 && length(time.interest) > ntime) {
          ## select evenly spaced values over [0,1] and not event times 
          time.interest <- seq(0,  min(1, max(time[nonMissingOutcome])), length = ntime)
          time.interest <- time.interest[time.interest > 0]
        }
        if (length(ntime) > 1) {
          ## over-ride the default setting and allow the user to specify anything they want between [0,1]
          time.pt <- ntime <= min(1, max(time[nonMissingOutcome])) & ntime > 0
          if (sum(time.pt) == 0) {
            stop("the ntime vector supplied must be between [0,1]:", ntime)
          }
          time.interest <- sort(unique(ntime[time.pt]))
        }
      }
    }
  }
  ##---------------------
  ## other families
  ##---------------------
  else {
    if ((fmly == "regr+") | (fmly == "class+") | (fmly == "mix+")) {
      r.dim <- dim(yvar)[2]
    }
    else {
      if (fmly == "unsupv") {
        r.dim <- 0
      }
      else {
        r.dim <- 1
      }
    }
    event <- event.type <- cens <- time.interest <- cens <- time <- start.time <- NULL
  }
  return(list(event = event, event.type = event.type, cens = cens,
              time.interest = time.interest,
              time = time, start.time = start.time, r.dim = r.dim))
}
