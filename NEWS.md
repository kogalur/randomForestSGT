# randomForestSGT 2.0.0

## New features

* Added node centering (`node.center = TRUE`) before forming polynomial and
  interaction terms, with support in fitting, prediction, filtering, and tuning.
* Added restricted lasso predictions (`node.restrict = TRUE`), which clamp
  predictors and scores to each node's inbag predictor and response ranges.
  Applies to OOB and new-data predictions; `predict()` can override the saved
  setting. Older forests without this setting remain unrestricted by default.
* Added `cv.rfsgt()` and `make.folds()` for cross-validation with reusable folds
  and per-fold seeds. Filtering and optional `hcut` tuning are repeated within
  each training fold, with held-out predictions and error summaries returned.
* Added `plot.rfsgt()` for per-tree OOB risk curves and comparisons of forest
  OOB error across tree sizes.
* Extended `get.beta()` to report centered-model coefficients in the original
  coordinates, including induced lower-order terms. Restricted predictions
  are reconstructed from term contributions plus `response.adjustment`.
* Added relaxed coefficient estimation with `get.beta(..., relaxed = TRUE)`
  using OOB least squares, global or local active terms, standard errors,
  and leave-one-out predictions for training observations.
* Added `partial.beta()` for variable-effect plots and `importance.beta()`
  for marginal and conditional importance summaries based on those effects.

## Changes to defaults and behavior

* Changed the default `hcut` from 1 to 2, enabling quadratic terms and
  variable filtering by default.
* Centered fits now default to `bsf = "inbag"`; uncentered fits with `hcut > 0`
  retain `bsf = "oob"`. `tune.hcut()` now defaults to `bsf = "inbag"`.
  Fitting with a tuning result inherits its centering and BSF settings unless
  explicitly overridden.
* Reduced the default tree-size limit for `hcut >= 2` from `min(100, n/5)`
  to `min(25, n/5)`, and changed the grow `block.size` default from 10 to 1.
* Changed the `get.beta()` default from `bag = "inbag"` to `bag = "oob"`.
* Revised random-seed initialization; results for a fixed seed can differ
  from version 1.0.0.
* Printed fits now report node centering and restriction settings and flag
  potentially optimistic OOB error when variable filtering was used.

## Fixes and robustness

* Preserved source predictors and term maps needed to reconstruct retained
  polynomial terms after filtering, including during tuning and prediction.
* Corrected saved-forest restoration of terminal membership and inbag counts,
  including repeated bootstrap draws, and preserved CART fallback and split
  types when saving trees.
* Corrected forest offsets for predictions using selected trees (`get.tree`).
* Improved `get.beta()` handling of restored objects and observations without
  contributing trees, preserving missing predictions instead of returning zero.
* Strengthened validation of saved forest dimensions and membership output,
  widened native array-length and offset calculations, and released native
  allocations used by custom bootstrap samples.
* Improved the SGT-importance fallback when varPro screening fails or returns
  no result.

# randomForestSGT 1.0.0

* Initial release.
