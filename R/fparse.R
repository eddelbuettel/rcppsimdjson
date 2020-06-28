#' Fast and Friendly JSON String Parser
#'
#' @param json One or more \code{character}s of JSON data. 
#'
#' @param query String used as a JSON Pointer to identify a specific element within \code{json}.
#'   \code{character(1L)}, default: \code{""}
#'
#' @param empty_array Any R object to return for empty JSON arrays. 
#'   default: \code{NULL}
#'
#' @param empty_object Any R object to return for empty JSON objects.
#'   default: \code{NULL}. 
#'
#' @param max_simplify_lvl Maximum simplification level.
#'   \code{character(1L)} or \code{integer(1L)}, default: \code{"data_frame"}
#'   \itemize{
#'     \item \code{"data_frame"} or \code{0L}
#'     \item \code{"matrix"} or \code{1L}
#'     \item \code{"vector"} or \code{2L}
#'     \item \code{"list"} or \code{3L} (no simplification)
#'  }
#'
#' @param type_policy Level of type strictness.
#'   \code{character(1L)} or \code{integer(1L)}, default: \code{"anything_goes"}.
#'   \itemize{
#'     \item \code{"anything_goes"} or \code{0L}: arrays always become atomic vectors
#'     \item \code{"numbers"} or \code{1L}: arrays containing only numbers always become atomic vectors
#'     \item \code{"strict"} or \code{2L}: arrays containing mixed types never become atomic vectors
#'    }
#'
#' @param int64_policy How to return big integers to R.
#'   \code{character(1L)} or \code{integer(1L)}, default: \code{"double"}.
#'   \itemize{
#'     \item \code{"double"} or \code{0L}: big integers become \code{double}s
#'     \item \code{"string"} or \code{1L}: big integers become \code{character}s
#'     \item \code{"integer64"} or \code{2L}: big integers \code{bit64::integer64}s
#'   }
#'
#' @details
#' Instead of using \code{lapply()} for vectors containing multiple strings,
#'   just use \code{fparse()} directly as it is vectorized This is much more 
#'   efficient as the underlying \code{simdjson::dom::parser} can reuse internal
#'   buffers between parses. Since the overwhelming majority of JSON objects
#'   parsed will not result in R scalars, a \code{list()} is always returned 
#'   when multiple items are passed. Also in keeping with \code{lapply()}'s 
#'   behavior, if the data passed has \code{names()}, the returned object will 
#'   have the same names.
#'
#' @examples
#' # simple parsing ============================================================
#' json_string <- '{"a":[[1,null,3.0],["a","b",true],[10000000000,2,3]]}'
#' fparse(json_string)
#' 
#' # controlling type-strictness ===============================================
#' fparse(json_string, type_policy = "numbers")
#' fparse(json_string, type_policy = "strict")
#' fparse(json_string, type_policy = "numbers", int64_policy = "string")
#' fparse(json_string, type_policy = "numbers", int64_policy = "integer64")
#' 
#' # vectorized parsing ========================================================
#' json_strings <- c(
#'   json1 = '[{"b":true,
#'              "c":null},
#'             {"b":[[1,2,3],
#'                   [4,5,6]],
#'              "c":"Q"}]',
#'   json2 = '[{"b":[[7,8,9],
#'                   [10,11,12]],
#'              "c":"Q"},
#'             {"b":[[13,14,15],
#'                   [16,17,18]],
#'              "c":null}]'
#' )
#' fparse(json_strings)
#' 
#' # controlling simplification ================================================
#' fparse(json_strings, max_simplify_lvl = "matrix")
#' fparse(json_strings, max_simplify_lvl = "vector")
#' fparse(json_strings, max_simplify_lvl = "list")
#' 
#' # customizing what `[]` and `{}` return =====================================
#' empties <- '[[],{}]'
#' fparse(empties)
#' fparse(empties, empty_array = NA, empty_object = FALSE)
#' 
#' # querying JSON w/ a JSON Pointer ===========================================
#' json_to_query <- c(
#'   json1 = '[
#'       "a",
#'       {
#'           "b": {
#'               "c": [
#'                   [
#'                       1,
#'                       2,
#'                       3
#'                   ],
#'                   [
#'                       4,
#'                       5,
#'                       6
#'                   ]
#'               ]
#'           }
#'       }
#'   ]
#'   ',
#'   json2 = '[
#'       "a",
#'       {
#'           "b": {
#'               "c": [
#'                   [
#'                       7,
#'                       8,
#'                       9
#'                   ],
#'                   [
#'                       10,
#'                       11,
#'                       12
#'                   ]
#'               ]
#'           }
#'       }
#'   ]'
#' )
#' fparse(json_to_query, query = "1")
#' fparse(json_to_query, query = "1/b")
#' fparse(json_to_query, query = "1/b/c")
#' fparse(json_to_query, query = "1/b/c/1")
#' fparse(json_to_query, query = "1/b/c/1/0")
#' 
#' @export
fparse <- function(json,
                   query = "",
                   empty_array = NULL,
                   empty_object = NULL,
                   max_simplify_lvl = c("data_frame", "matrix", "vector", "list"),
                   type_policy = c("anything_goes", "numbers", "strict"),
                   int64_policy = c("double", "string", "integer64")) {
  # validate arguments =========================================================
  # types ----------------------------------------------------------------------
  if (!is.character(json)) {
    stop("`json=` must be a `character`.")
  }
  if (!is.character(query) || is.na(query) || length(query) != 1L) {
    stop("`query=` must be a single, non-`NA` `character`.")
  }
  # prep options ===============================================================
  # max_simplify_lvl -----------------------------------------------------------
  if (is.character(max_simplify_lvl)) {
    max_simplify_lvl <- switch(
      match.arg(max_simplify_lvl, c("data_frame", "matrix", "vector", "list")),
      data_frame = 0L,
      matrix = 1L,
      vector = 2L,
      list = 3L,
      stop("Unknown `max_simplify_lvl=`.")
    )
  } else if (is.numeric(max_simplify_lvl)) {
    stopifnot(max_simplify_lvl %in% 0:3)
  } else {
    stop("`max_simplify_lvl=` must be of type `character` or `numeric`.")
  }
  # type_policy ----------------------------------------------------------------
  if (is.character(type_policy)) {
    type_policy <- switch(
      match.arg(type_policy, c("anything_goes", "numbers", "strict")),
      anything_goes = 0L,
      numbers = 1L,
      strict = 2L,
      stop("Unknown `type_policy=`.")
    )
  } else if (is.numeric(type_policy)) {
    stopifnot(max_simplify_lvl %in% 0:2)
  } else {
    stop("`type_policy=` must be of type `character` or `numeric`.")
  }
  # int64_policy ---------------------------------------------------------------
  if (is.character(int64_policy)) {
    int64_policy <- switch(
      match.arg(int64_policy, c("double", "string", "integer64")),
      double = 0L,
      string = 1L,
      integer64 = 2L,
      stop("Unknown `int64_policy=`.")
    )
  } else if (is.numeric(int64_policy)) {
    stopifnot(int64_policy %in% 0:2)
  } else {
    stop("`int64_policy` must be of type `character` or `numeric`.")
  }
  
  if (int64_policy == 2L && !requireNamespace("bit64", quietly = TRUE)) {
    stop('`int64_policy="integer64", but the {bit64} package is not installed.')
  }
  # deserialize ================================================================
  .deserialize_json(
    json = json,
    json_pointer = query,
    empty_array = empty_array,
    empty_object = empty_object,
    simplify_to = max_simplify_lvl,
    type_policy = type_policy,
    int64_r_type = int64_policy
  )
}
