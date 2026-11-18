predict.rfsgt <- function(object,
                          newdata,
                          get.tree = NULL,
                          block.size = 10,
                          seed = NULL,
                          do.trace = FALSE,
                          node.restrict = NULL,
                          ...)
{
  ## NULL: the setting recorded in the forest; TRUE or FALSE overrides it for this call
  predict.rfsgt.workhorse(object, newdata, get.tree = get.tree, block.size = block.size,
            seed = seed, do.trace = do.trace, node.restrict = node.restrict, ...)
}
