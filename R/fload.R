.file_extension <- function(x, dot = TRUE) {
  base_name <- basename(x)
  
  captures <- regexpr("(?<!^|[.]|/)[.]([^.]+)$", base_name, perl = TRUE)
  out <- rep(NA_character_, length(x))
  out[captures > 0L] <- substring(base_name[captures > 0L], captures[captures > 0L])
  
  out
}


.url_prefix <- function(x) {
  vapply(x, function(.x) {
    if (substring(.x, 1L, 8L) == "https://") {
      "https://"
    } else if ((prefix <- substring(.x, 1L, 7L)) %in% c("http://", "ftps://", "file://")) {
      prefix
    } else if (substring(.x, 1L, 6L) == "ftp://") {
      "ftp://"
    } else {
      NA_character_
    }
  }, character(1L), USE.NAMES = FALSE)
}


.diagnose_input <- function(x) {
  init <- list(
    input = x,
    url_prefix = .url_prefix(x),
    file_ext = .file_extension(x)
  )
  
  init$compressed <- tolower(init$file_ext) %in% c(".gz", ".bz", ".bz2", ".xz", ".zip")
  if (any(init$compressed)) {
    stop(
      "Compressed files are not yet supported. The following files are affected:",
      sprintf("\n\t- %s", x[init$compressed]),
      call. = FALSE
    )
  }

  if (!anyNA(init$url_prefix)) {
    init$type <- rep("url", length(x))
  } else if (!anyNA(init$file_ext)) {
    init$type <- rep("file", length(x))
  } else {
    init$type <- rep(NA, length(x))
  }
  
  structure(init, class = "data.frame", row.names = seq_along(x))
}


#' @rdname fparse
#' 
#' @order 2
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
#' # load remote JSON ==========================================================
#' \dontrun{
#'
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
#' }
#'
#' @export
fload <- function(json,
                  query = "",
                  empty_array = NULL,
                  empty_object = NULL,
                  max_simplify_lvl = c("data_frame", "matrix", "vector", "list"),
                  type_policy = c("anything_goes", "numbers", "strict"),
                  int64_policy = c("double", "string", "integer64"),
                  verbose = FALSE,
                  temp_dir = tempdir(),
                  keep_temp_files = FALSE) {
  # validate arguments =========================================================
  if (!is.character(json)) {
    stop("`json=` must be a `character`.")
  }
  
  if (is.null(query)) {
    query <- ""
  } else if (!is.character(query) || is.na(query) || length(query) != 1L) {
    stop("`query=` must be a single, non-`NA` `character`.")
  }

  if (!is.logical(verbose) || length(verbose) != 1L || is.na(verbose)) {
    stop("`verbose=` must be either `TRUE` or `FALSE`.")
  }
  if (!is.logical(keep_temp_files) || length(keep_temp_files) != 1L || is.na(keep_temp_files)) {
    stop("`keep_temp_files=` must be either `TRUE` or `FALSE`.")
  }
  if (!length(temp_dir <- Sys.glob(temp_dir))) {
    stop("`temp_dir=` does not exist.")
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
  # files or URLs? =============================================================
  diagnosis <- .diagnose_input(json)
  input_type <- unique(diagnosis$type)
  if (length(input_type) != 1L || is.na(input_type)) {
    stop(
      "`json=` should be either paths to local files or paths to remote files (URLs). Not both."
    )
  }
  # URLs -----------------------------------------------------------------------
  if (identical(input_type, "url")) {
    for (i in seq_along(json)) {
      temp_file <- tempfile(fileext = diagnosis$file_ext[[i]], tmpdir = temp_dir)

      switch(
        diagnosis$url_prefix[[i]],
        "https://" = ,
        "ftps://" = ,
        "http://" = ,
        "ftp://" = download.file(diagnosis$input[[i]], destfile = temp_file, method = getOption("download.file.method", default = "auto"), quiet = !verbose),
        "file://" = download.file(diagnosis$input[[i]], destfile = temp_file, method = "internal", quiet = !verbose),
        stop("Unknown URL prefix")
      )

      diagnosis$input[[i]] <- temp_file
      diagnosis$type[[i]] <- "file"
    }

    input_type <- unique(diagnosis$type)
    stopifnot(length(input_type) == 1L)
    if (!keep_temp_files) {
      on.exit(unlink(diagnosis$input), add = TRUE)
    }
  }
  # file -----------------------------------------------------------------------
  if (length(missing_files <- diagnosis$input[!file.exists(diagnosis$input)])) {
    stop("The following files don't exist:",
         sprintf("\n\t- %s", missing_files))
  }
  input <- Sys.glob(diagnosis$input)
  # prep names =================================================================
  if (length(names(json))) {
    names(input) <- names(json)
  } else {
    names(input) <- basename(json)
  }
  # load =======================================================================
  .load_json(
    file_path = input,
    json_pointer = query,
    empty_array = empty_array,
    empty_object = empty_object,
    simplify_to = max_simplify_lvl,
    type_policy = type_policy,
    int64_r_type = int64_policy
  )
}
