#' Fast, Friendly, and Flexible JSON Parsing
#'
#' Parse JSON strings and files to R objects.
#'
#' @order 1
#'
#' @param json JSON strings, file paths, or raw vectors.
#'   \itemize{
#'     \item \code{fparse()}
#'       \itemize{
#'         \item \code{character}: One or more JSON strings.
#'         \item \code{raw}: \code{json} is interpreted as the bytes of a single
#'                JSON string.
#'         \item \code{list} Every element must be of type \code{"raw"} and each
#'               is individually interpreted as the bytes of a single JSON string.
#'       }
#'     \item \code{fload()}
#'       \itemize{
#'         \item \code{character}: One or more paths to files (local or remote)
#'               containing JSON.
#'     }
#'   }
#'
#' @param query If not \code{NULL}, JSON Pointer(s) used to identify and extract
#'   specific elements within \code{json}. See Details and Examples.
#'   \code{NULL}, \code{character()}, or \code{list()} of \code{character()}. default: \code{NULL}
#'
#' @param empty_array Any R object to return for empty JSON arrays.
#'   default: \code{NULL}
#'
#' @param empty_object Any R object to return for empty JSON objects.
#'   default: \code{NULL}.
#'
#' @param single_null Any R object to return for single JSON nulls.
#'   default: \code{NULL}.
#'
#' @param parse_error_ok Whether to allow parsing errors.
#'   default: \code{FALSE}.
#'
#' @param on_parse_error If \code{parse_error_ok} is \code{TRUE}, \code{on_parse_error} is any
#'   R object to return when query errors occur.
#'   default: \code{NULL}.
#'
#' @param query_error_ok Whether to allow parsing errors.
#'   default: \code{FALSE}.
#'
#' @param on_query_error If \code{query_error_ok} is \code{TRUE}, \code{on_query_error} is any
#'   R object to return when query errors occur.
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
#'     \item \code{"anything_goes"} or \code{0L}: non-recursive arrays always become atomic vectors
#'     \item \code{"numbers"} or \code{1L}: non-recursive arrays containing only numbers always become atomic vectors
#'     \item \code{"strict"} or \code{2L}: non-recursive arrays containing mixed types never become atomic vectors
#'    }
#'
#' @param int64_policy How to return big integers to R.
#'   \code{character(1L)} or \code{integer(1L)}, default: \code{"double"}.
#'   \itemize{
#'     \item \code{"double"} or \code{0L}: big integers become \code{double}s
#'     \item \code{"string"} or \code{1L}: big integers become \code{character}s
#'     \item \code{"integer64"} or \code{2L}: big integers become \code{bit64::integer64}s
#'     \item \code{"always"} or \code{3L}: all integers become \code{bit64::integer64}s
#'   }
#'
#' @param always_list Whether a \code{list} should always be returned, even when \code{length(json) == 1L}.
#'   default: \code{FALSE}.
#'
#'
#' @details
#' \itemize{
#'
#'   \item Instead of using \code{lapply()} to parse multiple values, just use
#'   \code{fparse()} and \code{fload()} directly.
#'   \itemize{
#'     \item They are vectorized in order to leverage the underlying
#'           \code{simdjson::dom::parser}'s ability to reuse its internal buffers
#'            between parses.
#'     \item Since the overwhelming majority of JSON parsed will not result in
#'           scalars, a \code{list()} is always returned if \code{json} contains
#'           more than one value.
#'     \item If \code{json} contains multiple values and has \code{names()}, the
#'           returned object will have the same names.
#'     \item If \code{json} contains multiple values and is unnamed, \code{fload()}
#'           names each returned element using the file's \code{basename()}.
#'    }
#'
#'    \item \code{query}'s goal is to minimize te amount of data that must be
#'    materialized as R objects (the main performance bottleneck) as well as
#'    facilitate any post-parse processing.
#'    \itemize{
#'      \item To maximize flexibility, there are two approaches to consider when designing \code{query} arguments.
#'      \itemize{
#'        \item \code{character} vectors are interpreted as containing queries that
#'        meant to be applied to all elements of \code{json=}.
#'        \itemize{
#'          \item If \code{json=} contains 3 strings and \code{query=} contains
#'          3 strings, the returned object will be a list of 3 elements (1 for each element
#'          of \code{json=}), which themselves each contain 3 lists (1 for each element
#'          of \code{query=}).
#'        }
#'        \item \code{list}s of \code{character} vectors are interpreted as containing
#'        queries meant to be applied  to \code{json} in a zip-like fashion.
#'      }
#'    }
#'
#' }
#'
#' @author Brendan Knapp
#'
#' @examples
#' # simple parsing ============================================================
#' json_string <- '{"a":[[1,null,3.0],["a","b",true],[10000000000,2,3]]}'
#' fparse(json_string)
#'
#' raw_json <- as.raw(
#'     c(0x22, 0x72, 0x61, 0x77, 0x20, 0x62, 0x79, 0x74, 0x65, 0x73, 0x20, 0x63,
#'       0x61, 0x6e, 0x20, 0x62, 0x65, 0x63, 0x6f, 0x6d, 0x65, 0x20, 0x4a, 0x53,
#'       0x4f, 0x4e, 0x20, 0x74, 0x6f, 0x6f, 0x21, 0x22)
#' )
#' fparse(raw_json)
#'
#' # ensuring a list is always returned ========================================
#' fparse(json_string, always_list = TRUE)
#' fparse(c(named_single_element_character = json_string), always_list = TRUE)
#'
#' # controlling type-strictness ===============================================
#' fparse(json_string, type_policy = "numbers")
#' fparse(json_string, type_policy = "strict")
#' fparse(json_string, type_policy = "numbers", int64_policy = "string")
#'
#' if (requireNamespace("bit64", quietly = TRUE)) {
#'     fparse(json_string, type_policy = "numbers", int64_policy = "integer64")
#' }
#'
#' # vectorized parsing ========================================================
#' json_strings <- c(
#'     json1 = '[{"b":true,
#'                "c":null},
#'               {"b":[[1,2,3],
#'                     [4,5,6]],
#'                "c":"Q"}]',
#'     json2 = '[{"b":[[7, 8, 9],
#'                    [10,11,12]],
#'               "c":"Q"},
#'               {"b":[[13,14,15],
#'                    [16,17,18]],
#'               "c":null}]'
#' )
#' fparse(json_strings)
#'
#' fparse(
#'     list(
#'         raw_json1 = as.raw(c(0x74, 0x72, 0x75, 0x65)),
#'         raw_json2 = as.raw(c(0x66, 0x61, 0x6c, 0x73, 0x65))
#'     )
#' )
#'
#' # controlling simplification ================================================
#' fparse(json_strings, max_simplify_lvl = "matrix")
#' fparse(json_strings, max_simplify_lvl = "vector")
#' fparse(json_strings, max_simplify_lvl = "list")
#'
#' # customizing what `[]`, `{}`, and single `null`s return ====================
#' empties <- "[[],{},null]"
#' fparse(empties)
#' fparse(empties,
#'        empty_array = logical(),
#'        empty_object = `names<-`(list(), character()),
#'        single_null = NA_real_)
#'
#' # handling invalid JSON and parsing errors ==================================
#' # fparse("junk JSON", parse_error_ok = TRUE)
#' # fparse("junk JSON", parse_error_ok = TRUE,
#' #       on_parse_error = "can't parse invalid JSON")
#' # fparse(
#' #    c(junk_JSON_1 = "junk JSON 1",
#' #      valid_JSON_1 = '"this is valid JSON"',
#' #      junk_JSON_2 = "junk JSON 2",
#' #      valid_JSON_2 = '"this is also valid JSON"'),
#' #    parse_error_ok = TRUE,
#' #    on_parse_error = NA
#' #)
#'
#' # querying JSON w/ a JSON Pointer ===========================================
#' json_to_query <- c(
#'     json1 = '[
#'     "a",
#'     {
#'         "b": {
#'             "c": [[1,2,3],
#'                   [4,5,6]]
#'         }
#'     }
#' ]',
#'     json2 = '[
#'     "a",
#'     {
#'         "b": {
#'             "c": [[7,8,9],
#'                   [10,11,12]],
#'            "d": [1,2,3,4]
#'         }
#'     }
#' ]')
#' fparse(json_to_query, query = "/1")
#' fparse(json_to_query, query = "/1/b")
#' fparse(json_to_query, query = "/1/b/c")
#' fparse(json_to_query, query = "/1/b/c/1")
#' fparse(json_to_query, query = "/1/b/c/1/0")
#'
#' # handling invalid queries ==================================================
#' fparse(json_to_query, query = "/1/b/d",
#'        query_error_ok = TRUE,
#'        on_query_error = "d isn't a key here!")
#'
#' # multiple queries applied to EVERY element =================================
#' fparse(json_to_query, query = c(query1 = "/1/b/c/1/0",
#'                                 query2 = "/1/b/c/1/1",
#'                                 query3 = "/1/b/c/1/2"))
#'
#' # multiple queries applied to EACH element ==================================
#' fparse(json_to_query,
#'        query = list(queries_for_json1 = c(c1 = "/1/b/c/1/0",
#'                                           c2 = "/1/b/c/1/1"),
#'                     queries_for_json2 = c(d1 = "/1/b/d/1",
#'                                           d2 = "/1/b/d/2")))
#'
#' @export
fparse <- function(json,
                   query = NULL,
                   empty_array = NULL,
                   empty_object = NULL,
                   single_null = NULL,
                   parse_error_ok = FALSE,
                   on_parse_error = NULL,
                   query_error_ok = FALSE,
                   on_query_error = NULL,
                   max_simplify_lvl = c("data_frame", "matrix", "vector", "list"),
                   type_policy = c("anything_goes", "numbers", "strict"),
                   int64_policy = c("double", "string", "integer64", "always"),
                   always_list = FALSE) {
    # validate arguments =======================================================
    # types --------------------------------------------------------------------
    if (!.is_valid_json_arg(json)) {
        stop("`json=` must be a non-empty character vector, raw vector, or a list containing raw vectors.")
    }
    if (!.is_valid_query_arg(query)) {
        stop("`query=` must be `NULL`, a non-empty character vector, or a list containing non-empty character vectors.")
    }

    if (is.list(query) && length(json) != length(query)) {
        stop("`query=` is a list (nested query), but is not the same length as `json=`.")
    }

    if (!.is_scalar_lgl(parse_error_ok)) {
        stop("`parse_error_ok=` must be either `TRUE` or `FALSE`.")
    }
    if (!.is_scalar_lgl(query_error_ok)) {
        stop("`query_error_ok=` must be either `TRUE` or `FALSE`.")
    }
    if (!.is_scalar_lgl(always_list)) {
        stop("`always_list=` must be either `TRUE` or `FALSE`.")
    }
    # prep options =============================================================
    # max_simplify_lvl ---------------------------------------------------------
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
    # type_policy --------------------------------------------------------------
    if (is.character(type_policy)) {
        type_policy <- switch(
            match.arg(type_policy, c(
                "anything_goes", "numbers", "strict"
            )),
            anything_goes = 0L,
            numbers = 1L,
            strict = 2L,
            stop("Unknown `type_policy=`.")
        )
    } else if (is.numeric(type_policy)) {
        stopifnot(type_policy %in% 0:2)
    } else {
        stop("`type_policy=` must be of type `character` or `numeric`.")
    }
    # int64_policy -------------------------------------------------------------
    if (is.character(int64_policy)) {
        int64_policy <- switch(
            match.arg(int64_policy, c("double", "string", "integer64", "always")),
            double = 0L,
            string = 1L,
            integer64 = 2L,
            always = 3L,
            stop("Unknown `int64_policy=`.")
        )
    } else if (is.numeric(int64_policy)) {
        stopifnot(int64_policy %in% 0:3)
    } else {
        stop("`int64_policy` must be of type `character` or `numeric`.")
    }

    if (int64_policy == 2L && !requireNamespace("bit64", quietly = TRUE)) {
        # nocov start
        stop('`int64_policy="integer64", but the {bit64} package is not installed.')
        # nocov end
    }

    # deserialize ==============================================================
    out <- .deserialize_json(
        json = json,
        query = query,
        empty_array = empty_array,
        empty_object = empty_object,
        single_null = single_null,
        parse_error_ok = parse_error_ok,
        on_parse_error = on_parse_error,
        query_error_ok = query_error_ok,
        on_query_error = on_query_error,
        simplify_to = max_simplify_lvl,
        type_policy = type_policy,
        int64_r_type = int64_policy
    )

    if (always_list && length(json) == 1L) {
        `names<-`(list(out), names(json))
    } else {
        out
    }
}
