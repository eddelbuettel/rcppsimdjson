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
#' @param query String used as a JSON Pointer to identify a specific element within \code{json}.
#'   \code{character(1L)}, default: \code{""}
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
#' @param error_ok Whether to allow parsing errors.
#'   default: \code{FALSE}.
#'
#' @param on_error If \code{error_ok} is \code{TRUE}, \code{on_error} is any
#'   R object to return when parsing errors occur.
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
#'     \item \code{"integer64"} or \code{2L}: big integers \code{bit64::integer64}s
#'   }
#'
#' @param verbose Whether to display status messages.
#'   \code{TRUE} or \code{FALSE}, default: \code{FALSE}
#'
#' @param temp_dir Directory path to use for any temporary files.
#'   \code{character(1L)}, default: \code{tempdir()}
#'
#' @param keep_temp_files Whether to remove any temporary files created by
#'   \code{fload()} from \code{temp_dir}.
#'   \code{TRUE} or \code{FALSE}, default: \code{TRUE}
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
#' fparse("junk JSON", error_ok = TRUE)
#' fparse("junk JSON", error_ok = TRUE, on_error = "can't parse invalid JSON")
#' fparse(
#'     c(junk_JSON_1 = "junk JSON 1",
#'       valid_JSON_1 = '"this is valid JSON"',
#'       junk_JSON_2 = "junk JSON 2",
#'       valid_JSON_2 = '"this is also valid JSON"'),
#'     error_ok = TRUE,
#'     on_error = NA
#' )
#'
#' # querying JSON w/ a JSON Pointer ===========================================
#' json_to_query <- c(
#'     json1 = '[
#'     "a",
#'     {
#'         "b": {
#'             "c": [
#'                 [
#'                     1,
#'                     2,
#'                     3
#'                 ],
#'                 [
#'                     4,
#'                     5,
#'                     6
#'                 ]
#'             ]
#'         }
#'     }
#' ]',
#'     json2 = '[
#'     "a",
#'     {
#'         "b": {
#'             "c": [
#'                 [
#'                     7,
#'                     8,
#'                     9
#'                 ],
#'                 [
#'                     10,
#'                     11,
#'                     12
#'                 ]
#'             ]
#'         }
#'     }
#' ]')
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
                   single_null = NULL,
                   error_ok = FALSE,
                   on_error = NULL,
                   max_simplify_lvl = c("data_frame", "matrix", "vector", "list"),
                   type_policy = c("anything_goes", "numbers", "strict"),
                   int64_policy = c("double", "string", "integer64")) {
    # validate arguments =======================================================
    # types --------------------------------------------------------------------
    if (is.list(json)) {
        if (!all(vapply(json, is.raw, logical(1L)))) {
            stop("If `json=` is a list, it must only contain `raw` vectors.")
        }
    } else if (!is.character(json) && !is.raw(json)) {
        stop("`json=` must be a `character` or `raw`.")
    }

    if (is.null(query)) {
        query <- ""
    } else if (!.is_scalar_chr(query)) {
        stop("`query=` must be a single, non-`NA` `character`.")
    }

    if (!.is_scalar_lgl(error_ok)) {
        stop("`error_ok=` must be either `TRUE` or `FALSE`.")
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

    if (int64_policy == 2L &&
        !requireNamespace("bit64", quietly = TRUE)) {
        # nocov start
        stop('`int64_policy="integer64", but the {bit64} package is not installed.')
        # nocov end
    }
    # deserialize ==============================================================
    .deserialize_json(
        json = json,
        json_pointer = query,
        empty_array = empty_array,
        empty_object = empty_object,
        single_null = single_null,
        error_ok = error_ok,
        on_error = on_error,
        simplify_to = max_simplify_lvl,
        type_policy = type_policy,
        int64_r_type = int64_policy
    )
}
