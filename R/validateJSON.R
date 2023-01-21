
##' Validate a JSON file, fast
##'
##' By relying on simd-parallel 'simdjson' header-only library JSON
##' files can be parsed very quickly.
##' @param jsonfile A character variable with a path and filename
##' @return A boolean value indicating whether the JSON content was
##'     parsed successfully
##' @examples
##' if (!RcppSimdJson:::.unsupportedArchitecture()) {
##'     jsonfile <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
##'     validateJSON(jsonfile)
##' }
validateJSON <- function(jsonfile) {
    stopifnot("The given file was not found" = file.exists(jsonfile))
    .validateJSON(Sys.glob(jsonfile))
}
