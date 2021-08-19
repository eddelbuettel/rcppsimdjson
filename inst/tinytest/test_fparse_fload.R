if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")
library(RcppSimdJson)

.read_file <- function(file_path) {
    readChar(file_path, nchars = file.size(file_path))
}
.write_file <- function(x, file_path) {
    writeChar(x, file_path, nchars = nchar(x), eos = NULL)
}

test_file1 <- tempfile(fileext = ".json")
test_file2 <- tempfile(fileext = ".json")

# json =========================================================================
#* invalid ---------------------------------------------------------------------
#** fparse() -------------------------------------------------------------------
expect_error(fparse(1))
expect_error(fparse(integer(0L)))
expect_error(fparse(NULL))
expect_error(fparse(NA_integer_))

expect_error(fparse("1", parse_error_ok = NA))
#** fload() --------------------------------------------------------------------
expect_error(fload(1))
expect_error(fload(NA_integer_))

expect_error(fload("1", parse_error_ok = NA))

.write_file("junk JSON", test_file1)
.write_file("more junk JSON", test_file2)
expect_error(fparse(test_file1))
expect_error(fload(c(test_file1, test_file2)))

#* valid -----------------------------------------------------------------------
#** fparse() -------------------------------------------------------------------
expect_identical(fparse(NA_character_), NA)
expect_identical(fparse("1"),
                 1L)
expect_identical(fparse(c("1", "2")),
                 list(1L, 2L))
expect_identical(fparse(c(json1 = "1", json2 = "2")),
                 list(json1 = 1L, json2 = 2L))
expect_identical(fparse(NA_character_),
                 NA)
expect_identical(fparse(c(NA_character_, NA_character_)),
                 list(NA, NA))
expect_identical(fparse(c("1", NA_character_)),
                 list(1L, NA))
expect_true(fparse("null", single_null = TRUE))

#Change to expect_error. By design, On Demand does not detect broken JSON until it is accessed/parsed.
expect_error(fparse("junk JSON", parse_error_ok = TRUE, on_parse_error = TRUE))
expect_error(fparse(c("junk JSON", "more junk JSON"),
                        parse_error_ok = TRUE, on_parse_error = NA))
#expect_true(fparse("junk JSON", parse_error_ok = TRUE, on_parse_error = TRUE))
#expect_identical(fparse(c("junk JSON", "more junk JSON"),
#                        parse_error_ok = TRUE, on_parse_error = NA),
#                 list(NA, NA))

#** fload() --------------------------------------------------------------------
expect_identical(fload(NA_character_), NA)

.write_file("1", test_file1)
.write_file("2", test_file2)

expect_identical(fload(test_file1), 1L)
expect_identical(
    fload(c(test_file1, test_file2)),
    `names<-`(list(1L, 2L), basename(c(test_file1, test_file2)))
)

expect_identical(fload(c(json1 = test_file1, json2 = test_file2)),
                 list(json1 = 1L, json2 = 2L))
expect_identical(fload(NA_character_),
                 NA)
expect_identical(fload(c(NA_character_, NA_character_)),
                 list(NA, NA))
expect_identical(fload(c(test_file1, NA_character_))[[2L]], NA)

.write_file("null", test_file1)
expect_true(fload(test_file1, single_null = TRUE))

.write_file("junk JSON", test_file1)
.write_file("more junk JSON", test_file2)

#Change to expect_error. By design, On Demand does not detect broken JSON until it is accessed/parsed.
expect_error(fload(test_file1, parse_error_ok = TRUE, on_parse_error = TRUE))
expect_error(
    fload(c(test_file1, test_file2),
          parse_error_ok = TRUE, on_parse_error = NA))
#expect_true(fload(test_file1, parse_error_ok = TRUE, on_parse_error = TRUE))
#expect_identical(
#    fload(c(test_file1, test_file2),
#          parse_error_ok = TRUE, on_parse_error = NA),
#    `names<-`(list(NA, NA), basename(c(c(test_file1, test_file2))))
#)
# _ ============================================================================
# query ========================================================================
#* invalid ---------------------------------------------------------------------
#*** fparse() ------------------------------------------------------------------
expect_error(identical(fparse("1", query = c("0", "0")), list(1L, 1L)))
expect_error(fparse("1", query = (not_chr <- TRUE)))
#*** fload() -------------------------------------------------------------------
expect_error(fload("1", query = c("0", "0")))
expect_error(fload("1", query = (not_chr <- TRUE)))

#* valid -----------------------------------------------------------------------
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse("1", query = NA_character_), NA)
expect_identical(fparse("[0,1]", query = "/0"),
                 0L)
expect_identical(fparse("1", query = NULL),
                 1L)
#*** fload() -------------------------------------------------------------------
.write_file("[0,1]", test_file1)
expect_identical(fload(test_file1, query = NA_character_), NA)
expect_identical(fload(test_file1, query = "/0"),
                 0L)
.write_file("1", test_file1)
expect_identical(fload(test_file1, query = NULL),
                 1L)

# _ ============================================================================
# max_simplify_lvl =============================================================
#* invalid ---------------------------------------------------------------------
#*** fparse() ------------------------------------------------------------------
expect_error(fparse("1", max_simplify_lvl = ".invalid"))
expect_error(fparse("1", max_simplify_lvl = NA_character_))
expect_error(fparse("1", max_simplify_lvl = 10))
expect_error(fparse("1", max_simplify_lvl = (not_chr_or_num <-
                                                 TRUE)))
#*** fload() -------------------------------------------------------------------
expect_error(fload("1", max_simplify_lvl = ".invalid"))
expect_error(fload("1", max_simplify_lvl = NA_character_))
expect_error(fload("1", max_simplify_lvl = 10))
expect_error(fload("1", max_simplify_lvl = (not_chr_or_num <-
                                                TRUE)))
#* valid -----------------------------------------------------------------------
test <- '[
    {"a":1},
    {"a":[1,2]},
    {"a":[[1,2],
    [3,4]]}
]'
.write_file(test, test_file1)

#** max_simplify_lvl = "data_frame" --------------------------------------------
target <- structure(
    list(a = list(
        1L, 1:2,
        matrix(c(1L, 2L, 3L, 4L), nrow = 2L, ncol = 2L, byrow = TRUE)
        )
    ),class = "data.frame",
    row.names = c(NA, 3L)
)
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, max_simplify_lvl = "data_frame"), target)
expect_identical(fparse(test, max_simplify_lvl = 0L), target)
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, max_simplify_lvl = "data_frame"), target)
expect_identical(fload(test_file1, max_simplify_lvl = 0L), target)

#** max_simplify_lvl = "matrix" ------------------------------------------------
target <- list(list(a = 1L),
               list(a = 1:2),
               list(a = matrix(
                   c(1L, 2L, 3L, 4L),
                   nrow = 2L,
                   ncol = 2L,
                   byrow = TRUE
               )))
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, max_simplify_lvl = "matrix"), target)
expect_identical(fparse(test, max_simplify_lvl = 1L), target)
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, max_simplify_lvl = "matrix"), target)
expect_identical(fload(test_file1, max_simplify_lvl = 1L), target)

#** max_simplify_lvl = "vector" ------------------------------------------------
target <- list(list(a = 1L),
               list(a = 1:2),
               list(a = list(1:2, 3:4)))
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, max_simplify_lvl = "vector"),
                 target)
expect_identical(fparse(test, max_simplify_lvl = 2L),
                 target)
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, max_simplify_lvl = "vector"),
                 target)
expect_identical(fload(test_file1, max_simplify_lvl = 2L),
                 target)

#** max_simplify_lvl = "list" --------------------------------------------------
target <- list(list(a = 1L),
               list(a = list(1L, 2L)),
               list(a = list(list(1L, 2L), list(3L, 4L))))
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, max_simplify_lvl = "list"),
                 target)

expect_identical(fparse(test, max_simplify_lvl = 3L),
                 target)
#*** fload() -------------------------------------------------------------------
expect_identical(fparse(test, max_simplify_lvl = "list"),
                 target)

expect_identical(fparse(test, max_simplify_lvl = 3L),
                 target)

expect_identical(fparse("[[null, null],[null,null]]"),
                 matrix(NA, nrow = 2L, ncol = 2L))
expect_identical(fparse("[[true,false],[true,false]]"),
                 matrix(c(TRUE, TRUE, FALSE, FALSE),
                        nrow = 2L, ncol = 2L))
expect_identical(
    fparse(
        "[[9999999999999999999,9999999999999999999],[9999999999999999999,9999999999999999999]]"
    ),
    matrix(
        rep("9999999999999999999", 4L),
        nrow = 2L,
        ncol = 2L
    )
)
expect_identical(fparse("[[9999999999999999999,1],[1,1]]"),
                 matrix(
                     c("9999999999999999999", "1", "1", "1"),
                     nrow = 2L,
                     ncol = 2L
                 ))

# _ ============================================================================
# type_policy ==================================================================
#* invalid ---------------------------------------------------------------------
#*** fparse() ------------------------------------------------------------------
expect_error(fparse("1", type_policy = ".invalid"))
expect_error(fparse("1", type_policy = NA_character_))
expect_error(fparse("1", type_policy = 10L))
expect_error(fparse("1", type_policy = NA_integer_))
expect_error(fparse("1", type_policy = (not_chr_or_num <- TRUE)))
#*** fload() -------------------------------------------------------------------
expect_error(fload("1", type_policy = ".invalid"))
expect_error(fload("1", type_policy = NA_character_))
expect_error(fload("1", type_policy = 10L))
expect_error(fload("1", type_policy = NA_integer_))
expect_error(fload("1", type_policy = (not_chr_or_num <- TRUE)))

#* valid -----------------------------------------------------------------------
test <- '[
[[null,"a"],
["b",  1]]
,
[[2,          3.14, true],
[3000000000, 3,    6.62607004]]
,
[[4,          5, 6.0221409],
[3000000000, 6, null]]
,
[[7,          8,  9 ],
[3000000000, 10, 11]]
]'
.write_file(test, test_file1)

#** type_policy = "anything_goes" ----------------------------------------------
target <- list(
    matrix(
        c(NA, "a", "b", "1"),
        nrow = 2L,
        ncol = 2L,
        byrow = TRUE
    ),
    matrix(
        c(2L, 3.14, TRUE, 3000000000, 3L, 6.62607004),
        nrow = 2L,
        ncol = 3L,
        byrow = TRUE
    ),
    matrix(
        c(4L, 5L, 6.0221409, 3000000000, 6L, NA),
        nrow = 2L,
        ncol = 3L,
        byrow = TRUE
    ),
    matrix(
        c(7L, 8L, 9L, 3000000000, 10L, 11L),
        nrow = 2L,
        ncol = 3L,
        byrow = TRUE
    )
)
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, type_policy = "anything_goes"),
                 target)
expect_identical(fparse(test, type_policy = 0L),
                 target)

expect_identical(fparse("[1,2]", type_policy = "anything_goes"),
                 c(1L, 2L))
expect_identical(fparse("[true,false]", type_policy = "anything_goes"),
                 c(TRUE, FALSE))
expect_identical(
    fparse("[9999999999999999999,9999999999999999999]",
           type_policy = "anything_goes"),
    c("9999999999999999999", "9999999999999999999")
)
expect_identical(fparse(
    "[null,null]",
    single_null = NA,
    type_policy = "anything_goes"
),
c(NA, NA))
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, type_policy = "anything_goes"),
                 target)
expect_identical(fload(test_file1, type_policy = 0L),
                 target)

#** type_policy = "numbers" ----------------------------------------------------
target <- list(
    list(c(NA, "a"), list("b", 1L)),
    list(list(2L, 3.14, TRUE), c(3000000000, 3, 6.62607004)),
    matrix(
        c(4L, 5L, 6.0221409, 3000000000, 6L, NA),
        nrow = 2L,
        ncol = 3L,
        byrow = TRUE
    ),
    matrix(
        c(7L, 8L, 9L, 3000000000, 10L, 11L),
        nrow = 2L,
        ncol = 3L,
        byrow = TRUE
    )
)
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, type_policy = "numbers"),
                 target)
expect_identical(fparse(test, type_policy = 1L),
                 target)

expect_identical(fparse("[1,2]", type_policy = "numbers"),
                 c(1L, 2L))
expect_identical(fparse("[true,false]", type_policy = "numbers"),
                 c(TRUE, FALSE))
expect_identical(
    fparse("[9999999999999999999,9999999999999999999]", type_policy = "numbers"),
    c("9999999999999999999", "9999999999999999999")
)
expect_identical(fparse(
    "[null,null]",
    single_null = NA,
    type_policy = "numbers"
),
list(NA, NA))
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, type_policy = "numbers"),
                 target)
expect_identical(fload(test_file1, type_policy = 1L),
                 target)

#** type_policy = "strict" -----------------------------------------------------
target <- list(list(c(NA, "a"), list("b", 1L)),
               list(list(2L, 3.14, TRUE), list(3000000000, 3L, 6.62607004)),
               list(list(4L, 5L, 6.0221409), list(3000000000, 6L, NULL)),
               list(c(7L, 8L, 9L), list(3000000000, 10L, 11L)))
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, type_policy = "strict"),
                 target)
expect_identical(fparse(test, type_policy = 2L),
                 target)

expect_identical(fparse("[1,2]", type_policy = "strict"),
                 c(1L, 2L))
expect_identical(fparse("[true,false]", type_policy = "strict"),
                 c(TRUE, FALSE))
expect_identical(
    fparse("[9999999999999999999,9999999999999999999]", type_policy = "strict"),
    c("9999999999999999999", "9999999999999999999")
)
expect_identical(fparse(
    "[null,null]",
    single_null = NA,
    type_policy = "strict"
),
list(NA, NA))
#*** fload() ------------------------------------------------------------------
expect_identical(fload(test_file1, type_policy = "strict"),
                 target)
expect_identical(fload(test_file1, type_policy = 2L),
                 target)

# _ ============================================================================
# int64_policy =================================================================
#* invalid ---------------------------------------------------------------------
#*** fparse() ------------------------------------------------------------------
expect_error(fparse("1", int64_policy = ".invalid"))
expect_error(fparse("1", int64_policy = NA_character_))
expect_error(fparse("1", int64_policy = 10))
expect_error(fparse("1", int64_policy = NA_integer_))
expect_error(fparse("1", int64_policy = (not_chr_or_num <- TRUE)))
#*** fload() -------------------------------------------------------------------
expect_error(fload("1", int64_policy = ".invalid"))
expect_error(fload("1", int64_policy = NA_character_))
expect_error(fload("1", int64_policy = 10))
expect_error(fload("1", int64_policy = NA_integer_))
expect_error(fload("1", int64_policy = (not_chr_or_num <- TRUE)))

#* valid -----------------------------------------------------------------------
test <- "[
[false,          8,  null],
[3000000000, 10, true]
]
"
.write_file(test, test_file1)

#** int64_policy = "double" ----------------------------------------------------
target <- matrix(
    c(0, 8, NA, 3000000000, 10, 1),
    nrow = 2L,
    ncol = 3L,
    byrow = TRUE
)
#*** fload() -------------------------------------------------------------------
expect_identical(fparse(test, int64_policy = "double"),
                 target)
expect_identical(fparse(test, int64_policy = 0L),
                 target)
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, int64_policy = "double"),
                 target)
expect_identical(fload(test_file1, int64_policy = 0L),
                 target)

#** int64_policy = "string" ----------------------------------------------------
target <- matrix(
    c("FALSE", "8", NA, "3000000000", "10", "TRUE"),
    nrow = 2L,
    ncol = 3L,
    byrow = TRUE
)
#*** fparse() ------------------------------------------------------------------
expect_identical(fparse(test, int64_policy = "string"),
                 target)
expect_identical(fparse(test, int64_policy = 1L),
                 target)
#*** fload() -------------------------------------------------------------------
expect_identical(fload(test_file1, int64_policy = "string"),
                 target)
expect_identical(fload(test_file1, int64_policy = 1L),
                 target)

#** int64_policy = "integer64" -------------------------------------------------
if (requireNamespace("bit64", quietly = TRUE)) {
    target <-
        structure(bit64::as.integer64(c(0, 3000000000, 8, 10, NA, 1)),
                  .Dim = 2:3)
    #*** fparse() ------------------------------------------------------------------
    expect_identical(fparse(test, int64_policy = "integer64"),
                     target)
    expect_identical(fparse(test, int64_policy = 2L),
                     target)
    #*** fload() -------------------------------------------------------------------
    expect_identical(fload(test_file1, int64_policy = "integer64"),
                     target)
    expect_identical(fload(test_file1, int64_policy = 2L),
                     target)
}
#** int64_policy = "always" -------------------------------------------------
if (requireNamespace("bit64", quietly = TRUE)) {
    target <- structure(bit64::as.integer64(c(0, 3000000000, 8, 10, NA, 1)), .Dim = 2:3)
    #*** fparse() ------------------------------------------------------------------
    expect_identical(fparse(test, int64_policy = "always"),
                     target)
    expect_identical(fparse(test, int64_policy = 3L),
                     target)
    #*** fload() -------------------------------------------------------------------
    expect_identical(fload(test_file1, int64_policy = "always"),
                     target)
    expect_identical(fload(test_file1, int64_policy = 3L),
                     target)
}

#** int64_policy = "string" -------------------------------------------------
expect_identical(fparse("[3000000000, 1]"), c(3000000000, 1))
expect_identical(fparse("[3000000000, 1]", int64_policy = "string"),
                 c("3000000000", "1"))
expect_identical(fparse("[9999999999999999999, 1]"),
                 c("9999999999999999999", "1"))



# _ ============================================================================
# battery ======================================================================
#* vanilla JSON ----------------------------------------------------------------
all_files <- list.files(system.file("jsonexamples", package = "RcppSimdJson"),
                        recursive = TRUE,
                        pattern = "\\.json$",
                        full.names = TRUE)

#*** fparse() ------------------------------------------------------------------
for (i in seq_along(all_files)) {
    expect_silent(
        fparse(json = c(na1 = NA_character_,
                        file = .read_file(all_files[[i]]),
                        na2 = NA_character_))
    )
}



#*** fload() -------------------------------------------------------------------
if (FALSE) {
.write_file("JUNK JSON", test_file1)
.write_file('"VALID JSON"', test_file2)

opts <- expand.grid(type_policy = c("anything_goes", "numbers", "strict"),
                    int64_policy = c("double", "string", "integer64", "always"),
                    max_simplify_lvl = c("data_frame", "matrix", "vector", "list"),
                    parse_error_ok = c(TRUE, FALSE),
                    query_error_ok = c(TRUE, FALSE),
                    file = c(utils::head(all_files), test_file1),
                    stringsAsFactors = FALSE)
opts <- opts[
    opts$file != test_file1 | (opts$file == test_file1 & opts$parse_error_ok),
]


for (i in seq_len(nrow(opts))) {
    if (i %% 2 == 0) {
        json_arg <- c(opts$file[[i]], test_file2)
        if (opts$parse_error_ok[[i]]) {
            json_arg <- c(test_file1, json_arg)
        }
        junk_query <- rep("junk", length(json_arg))
    } else {
        json_arg <- opts$file[[i]]
        junk_query <- list("junk")
    }

    if (!opts$query_error_ok[[i]]) {
        query_arg <- NULL
    } else {
        query_arg <- junk_query
    }

    expect_silent(
        fload(json = json_arg,
              query = query_arg,
              type_policy = opts$type_policy[[i]],
              int64_policy = opts$int64_policy[[i]],
              max_simplify_lvl = opts$max_simplify_lvl[[i]],
              parse_error_ok = opts$parse_error_ok[[i]],
              query_error_ok = opts$query_error_ok[[i]])
    )

    if (length(json_arg) == 1L) {
        json_arg <- readBin(opts$file[[i]], what = "raw",
                            n = file.size(json_arg))
    } else {
        json_arg <- lapply(
            json_arg,
            function(.x) readBin(.x, what = "raw", n = file.size(.x))
        )
    }
    if (is.raw(json_arg)) {
        query_arg <- query_arg[[1L]]
    }

    expect_silent(
        fparse(json = json_arg,
               query = query_arg,
               type_policy = opts$type_policy[[i]],
               int64_policy = opts$int64_policy[[i]],
               max_simplify_lvl = opts$max_simplify_lvl[[i]],
               parse_error_ok = opts$parse_error_ok[[i]],
               query_error_ok = opts$query_error_ok[[i]])
    )
}
}
#* NDJSON ----------------------------------------------------------------------
all_files <- dir(
    system.file("jsonexamples", package = "RcppSimdJson"),
    recursive = TRUE,
    pattern = "\\.ndjson$",
    full.names = TRUE
)
#*** fparse() ------------------------------------------------------------------
sapply(all_files, function(.x) {
    expect_error(fparse(.read_file(.x)))
})
#*** fload() -------------------------------------------------------------------
sapply(all_files, function(.x) expect_error(fload(.x)))



# fload()-specific -------------------------------------------------------------
expect_error(fload("not-a-real-file.rcppsimdjson"))

expect_error(fload("not a real file"))

expect_error(
    fload("not-a-real-file.rcppsimdjson",
          temp_dir = "not/a/real/directory"),
    "`temp_dir=` does not exist."
)

expect_error(
    fload("not-a-real-file.rcppsimdjson",
          keep_temp_files = NA),
    "`keep_temp_files=` must be either `TRUE` or `FALSE`."
)

expect_error(
    fload("not-a-real-file.rcppsimdjson",
          verbose = NA),
    "`verbose=` must be either `TRUE` or `FALSE`."
)

.write_file('{"valid JSON":true}', test_file1)
.write_file('{"valid JSON": true}', test_file2)

expect_identical(
    fload(c(test_file1, test_file2), query = "/valid JSON"),
    `names<-`(list(TRUE, TRUE), basename(c(test_file1, test_file2)))
)

expect_error(
    fload(test_file1, query = list("bad 1", "bad 2"))
)
expect_error(
    fload(test_file1, query_error_ok = NA)
)
expect_error(
    fload(test_file1, compressed_download = NA)
)



# TODO verify CRAN policies for downloading, Travis usage
if (FALSE) {
    expect_error(suppressWarnings(fload("https://not-a-real-url")))
    expect_error(suppressWarnings(fload("https://invalid https")))
    expect_error(suppressWarnings(fload("http://invalid http")))
    expect_error(suppressWarnings(fload("ftps://invalid ftps")))
    expect_error(suppressWarnings(fload("ftp://invalid ftp")))
    expect_error(suppressWarnings(fload("file://invalid file")))

    single_url <-
        "https://raw.githubusercontent.com/eddelbuettel/rcppsimdjson/master/inst/jsonexamples/small/demo.json"
    expect_silent(fload(single_url))

    multiple_urls <- c(
        single_url,
        "https://raw.githubusercontent.com/eddelbuettel/rcppsimdjson/master/inst/jsonexamples/small/smalldemo.json"
    )
    expect_identical(names(fload(multiple_urls)),
                     basename(multiple_urls))
    .write_file("true", test_file1)
    .write_file("false", test_file2)
    expect_silent(fload(
        c(test_file1, test_file2, multiple_urls),
        keep_temp_files = TRUE
    ))
    expect_identical(names(fload(multiple_urls)),
                     basename(multiple_urls))
}
