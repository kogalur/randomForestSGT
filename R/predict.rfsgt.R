predict.rfsgt <- function(object,
                          newdata,
                          get.tree = NULL,
                          block.size = 10,
                          seed = NULL,
                          do.trace = FALSE,
                          ...)
{
  predict.rfsgt.workhorse(object, newdata, get.tree = get.tree, block.size = block.size,
            seed = seed, do.trace = do.trace, ...)
}
