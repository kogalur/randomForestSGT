rfsgt.news <- function(...) {
  newsfile <- file.path(system.file(package="randomForestSGT"), "NEWS")
  file.show(newsfile)
}
