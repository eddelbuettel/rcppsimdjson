#' @rdname fparse
#'
#' @order 2
#'
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
#' @param compressed_download Whether to request server-side compression on
#'   the downloaded document, default: \code{FALSE}
#'
#'
#' @examples
#' # load JSON files ===========================================================
#' single_file <- system.file("jsonexamples/small/demo.json", package = "RcppSimdJson")
#' fload(single_file)
#'
#' multiple_files <- c(
#'   single_file,
#'   system.file("jsonexamples/small/smalldemo.json", package = "RcppSimdJson")
#' )
#' fload(multiple_files)
#'
#'
#' \dontrun{
#'
#' # load remote JSON ==========================================================
#' a_url <- "https://api.github.com/users/lemire"
#' fload(a_url)
#'
#' multiple_urls <- c(
#'   a_url,
#'   "https://api.github.com/users/eddelbuettel",
#'   "https://api.github.com/users/knapply",
#'   "https://api.github.com/users/dcooley"
#' )
#' fload(multiple_urls, query = "name", verbose = TRUE)
#'
#' # download compressed (faster) JSON =========================================
#' fload(multiple_urls, query = "name", verbose = TRUE,
#'       compressed_download = TRUE)
#' }
#'
#' @export
fload <- function(json,
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
                  int64_policy = c("double", "string", "integer64"),
                  verbose = FALSE,
                  temp_dir = tempdir(),
                  keep_temp_files = FALSE,
                  compressed_download = FALSE) {
    # validate arguments =======================================================
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
    if (!.is_scalar_lgl((verbose))) {
        stop("`verbose=` must be either `TRUE` or `FALSE`.")
    }
    if (!.is_scalar_lgl(keep_temp_files)) {
        stop("`keep_temp_files=` must be either `TRUE` or `FALSE`.")
    }
    if (!.is_scalar_lgl(compressed_download)) {
        stop("`compressed_download=` must be either `TRUE` or `FALSE`.")
    }

    if (!length(temp_dir <- Sys.glob(temp_dir))) {
        stop("`temp_dir=` does not exist.")
    }
    # prep options =============================================================
    # max_simplify_lvl ---------------------------------------------------------
    if (is.character(max_simplify_lvl)) {
        max_simplify_lvl <- switch(
            match.arg(
                max_simplify_lvl,
                c("data_frame", "matrix", "vector", "list")
            ),
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
        stopifnot(max_simplify_lvl %in% 0:2)
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

    diagnosis <- .prep_input(json,
                             temp_dir = temp_dir,
                             compressed_download = compressed_download,
                             verbose = verbose)
    if (!keep_temp_files) {
        on.exit(unlink(diagnosis$input[diagnosis$is_from_url]), add = TRUE)
    }

    input <- diagnosis$input

    # prep names ===============================================================
    if (length(names(json))) {
        names(input) <- names(json)
    } else if (!anyNA(base_names <- basename(json))) {
            names(input) <- base_names
    }

    # load =====================================================================
    .load_json(
        json = input,
        query = query,
        empty_array = empty_array,
        empty_object = empty_object,
        parse_error_ok = parse_error_ok,
        on_parse_error = on_parse_error,
        query_error_ok = query_error_ok,
        on_query_error = on_query_error,
        single_null = single_null,
        simplify_to = max_simplify_lvl,
        type_policy = type_policy,
        int64_r_type = int64_policy
    )
}
