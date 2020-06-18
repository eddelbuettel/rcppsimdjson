if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

type_policy <- list(
  anything_goes = 0,
  ints_as_dbls = 1,
  strict = 2
)

int64_opt <- list(
  double = 0,
  string = 1,
  integer64 = 2
)

simplify_lvl <- list(
  data_frame = 0,
  matrix = 1,
  vector = 2,
  list = 3
)

# scalar =======================================================================
#* integer ---------------------------------------------------------------------
test <- "1"
target <- 1L
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* double-----------------------------------------------------------------------
test <- "1.0"
target <- 1
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* logical ---------------------------------------------------------------------
test <- "true"
target <- TRUE
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* character -------------------------------------------------------------------
test <- '"string"'
target <- "string"
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* null ------------------------------------------------------------------------
test <- "null"
target <- NULL
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* i64 -------------------------------------------------------------------------
test <- "10000000000"
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$double),
  10000000000
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  test
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  structure(4.94065645841247e-314, class = "integer64")
)
#* u64 -------------------------------------------------------------------------
test <- "10000000000000000000"
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  test
)

# homogeneous array ============================================================
#* empty -----------------------------------------------------------------------
test <- "[]"
target <- logical()
expect_identical(
  RcppSimdJson:::.deserialize_json(test, empty_array = target),
  target
)
#* integer ---------------------------------------------------------------------
test <- "[1,2]"
target <- c(1L, 2L)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* double ----------------------------------------------------------------------
test <- "[1.0,2.0]"
target <- c(1, 2)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* logical =====================================================================
test <- "[true,false]"
target <- c(TRUE, FALSE)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* character -------------------------------------------------------------------
test <- '["a","b"]'
target <- c("a", "b")
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* null ------------------------------------------------------------------------
test <- "[null,null]"
target <- c(NA, NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* i64 -------------------------------------------------------------------------
test <- "[10000000000,20000000000]"

target <- c(10000000000, 20000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$double),
  target
)

target <- c("10000000000", "20000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- structure(c(4.94065645841247e-314, 9.88131291682493e-314),
  class = "integer64"
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)
#* u64 -------------------------------------------------------------------------
test <- "[10000000000000000000,10000000000000000001]"
target <- c("10000000000000000000", "10000000000000000001")
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

# homogeneous array w/ nulls ===================================================
#* integer ---------------------------------------------------------------------
#** null first
test <- "[null,1,2]"
target <- c(NA, 1L, 2L)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#** null last
test <- "[1,2,null]"
target <- c(1L, 2L, NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* double ----------------------------------------------------------------------
#** null first
test <- "[null,1.0,2.0]"
target <- c(NA, 1, 2)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#** null last
test <- "[1.0,2.0,null]"
target <- c(1, 2, NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* logical ---------------------------------------------------------------------
#** null first
test <- "[null,true,false]"
target <- c(NA, TRUE, FALSE)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#** null last
test <- "[true,false,null]"
target <- c(TRUE, FALSE, NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* character -------------------------------------------------------------------
#** null first
test <- '[null,"a","b"]'
target <- c(NA, "a", "b")
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#** null last
test <- '["a","b",null]'
target <- c("a", "b", NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* i64 -------------------------------------------------------------------------
#** null first
null_first <- "[null,10000000000,20000000000]"

target <- c(NA, 10000000000, 20000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(null_first, int64_r_type = int64_opt$double),
  target
)

target <- c(NA, "10000000000", "20000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(null_first, int64_r_type = int64_opt$string),
  target
)

target <- structure(c(0, 4.94065645841247e-314, 9.88131291682493e-314),
  class = "integer64"
)
expect_identical(
  RcppSimdJson:::.deserialize_json(null_first, int64_r_type = int64_opt$integer64),
  target
)
#** null last
test <- "[10000000000,20000000000,null]"

target <- c(10000000000, 20000000000, NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$double),
  target
)

target <- c("10000000000", "20000000000", NA)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- structure(c(4.94065645841247e-314, 9.88131291682493e-314, 0),
  class = "integer64"
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)
#* u64 -------------------------------------------------------------------------
#** null first
null_first <- "[null,10000000000000000000,10000000000000000001]"
expect_identical(
  RcppSimdJson:::.deserialize_json(null_first),
  c(NA, "10000000000000000000", "10000000000000000001")
)
# #** null last
null_last <- "[10000000000000000000,10000000000000000001,null]"
expect_identical(
  RcppSimdJson:::.deserialize_json(null_last),
  c("10000000000000000000", "10000000000000000001", NA)
)

# mixed arrays =================================================================
#* all types -------------------------------------------------------------------
test <- '[null,"a",true,1,1.0,10000000000,10000000000000000000]'

target <- c(NA, "a", "TRUE", "1", "1.0", "10000000000", "10000000000000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

target <- list(NULL, "a", TRUE, 1L, 1, 1e+10, "10000000000000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$ints_as_dbls),
  target
)

target <- list(NULL, "a", TRUE, 1L, 1, 10000000000, "10000000000000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$strict),
  target
)
#* mixed integers --------------------------------------------------------------
test <- "[null,1,10000000000]"

target <- c(NA, 1, 10000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

target <- list(NULL, 1L, 10000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$strict),
  target
)

target <- c(NA, "1", "10000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- list(NULL, 1L, "10000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$string
  ),
  target
)

target <- list(NULL, 1L, structure(4.94065645841247e-314, class = "integer64"))
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$integer64
  ),
  target
)
#* logicals and integers -------------------------------------------------------
test <- "[null,1,true,false]"
target <- c(NA, 1L, TRUE, FALSE)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

test <- "[true,false,1]"
target <- list(TRUE, FALSE, 1L)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$strict),
  target
)
#* logicals and integer64 ------------------------------------------------------
test <- "[null,10000000000,true,false]"
target <- structure(c(0, 4.94065645841247e-314, 4.94065645841247e-324, 0),
  class = "integer64"
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)

test <- "[true,false,10000000000]"
target <- list(
  TRUE, FALSE,
  structure(4.94065645841247e-314, class = "integer64")
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$integer64
  ),
  target
)
#* logicals and doubles --------------------------------------------------------
test <- "[null,1.0,true,false]"
target <- c(NA, 1, TRUE, FALSE)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

test <- "[true,false,1.0]"
target <- list(TRUE, FALSE, 1)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$strict),
  target
)
#* doubles and mixed integers --------------------------------------------------
test <- "[null,1,1.1,10000000000]"

target <- c(NA, 1, 1.1, 10000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

target <- list(NULL, 1L, 1.1, 10000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$strict),
  target
)

target <- list(NULL, 1L, 1.1, "10000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$string
  ),
  target
)

target <- list(
  NULL, 1L, 1.1,
  structure(4.94065645841247e-314, class = "integer64")
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$integer64
  ),
  target
)

#* logicals, doubles, and mixed integers ---------------------------------------
test <- "[null,true,false,1,1.1,10000000000]"

target <- c(NA, 1, 0, 1, 1.1, 10000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

target <- list(NULL, TRUE, FALSE, 1L, 1.1, 10000000000)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test,
    type_policy = type_policy$strict
  ),
  target
)

target <- list(NULL, TRUE, FALSE, 1L, 1.1, "10000000000")
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$string
  ),
  target
)

target <- list(
  NULL, TRUE, FALSE, 1L, 1.1,
  structure(4.94065645841247e-314, class = "integer64")
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test,
    type_policy = type_policy$strict,
    int64_r_type = int64_opt$integer64
  ),
  target
)


# homogeneous matrices =========================================================
#* empty -----------------------------------------------------------------------
test <- "[[],
          []]"
target <- structure(logical(0), .Dim = c(2L, 0L))
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* integer ---------------------------------------------------------------------
test <- "[[1,2],
          [3,4]]"
target <- matrix(
  c(
    1L, 2L,
    3L, 4L
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* double ----------------------------------------------------------------------
test <- "[[1.0,2.0],
          [3.0,4.0]]"
target <- matrix(
  c(
    1, 2,
    3, 4
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* logical =====================================================================
test <- "[[true,false],
          [false,true]]"
target <- matrix(
  c(
    TRUE, FALSE,
    FALSE, TRUE
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* character -------------------------------------------------------------------
test <- '[["a","b"],
          ["c","d"]]'
target <- matrix(
  c(
    "a", "b",
    "c", "d"
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* null ------------------------------------------------------------------------
test <- "[[null,null],
          [null,null]]"
target <- matrix(
  c(NA, NA, NA, NA),
  nrow = 2L, ncol = 2L
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* i64 -------------------------------------------------------------------------
test <- "[[10000000000,20000000000],
          [30000000000,40000000000]]"

target <- matrix(
  c(
    10000000000, 20000000000,
    30000000000, 40000000000
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$double),
  target
)

target <- matrix(
  c(
    "10000000000", "20000000000",
    "30000000000", "40000000000"
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- structure(
  c(
    4.94065645841247e-314, 1.48219693752374e-313,
    9.88131291682493e-314, 1.97626258336499e-313
  ),
  class = "integer64", .Dim = c(2L, 2L)
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)
#* u64 -------------------------------------------------------------------------
test <- "[[10000000000000000000,10000000000000000002],
          [10000000000000000003,10000000000000000004]]"
target <- matrix(
  c(
    "10000000000000000000", "10000000000000000002",
    "10000000000000000003", "10000000000000000004"
  ),
  nrow = 2L, ncol = 2L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

# homogeneous matrices w/nulls =================================================
#* empty -----------------------------------------------------------------------
test <- "[[null,null],
          [null,null]]"
target <- matrix(c(NA, NA, NA, NA), nrow = 2L, ncol = 2L)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* integer ---------------------------------------------------------------------
test <- "[[null, 1, 2],
          [3,    4, null]]"
target <- matrix(
  c(
    NA, 1L, 2L,
    3L, 4L, NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* double ----------------------------------------------------------------------
test <- "[[null,1.0,2.0],
          [3.0,4.0,null]]"
target <- matrix(
  c(
    NA, 1, 2,
    3, 4, NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* logical =====================================================================
test <- "[[null,true,false],
          [false,true,null]]"
target <- matrix(
  c(
    NA, TRUE, FALSE,
    FALSE, TRUE, NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* character -------------------------------------------------------------------
test <- '[[null,"a","b"],
          ["c","d",null]]'
target <- matrix(
  c(
    NA, "a", "b",
    "c", "d", NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* i64 -------------------------------------------------------------------------
test <- "[[null,10000000000,20000000000],
          [30000000000,40000000000,null]]"

target <- matrix(
  c(
    NA, 10000000000, 20000000000,
    30000000000, 40000000000, NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$double),
  target
)

target <- matrix(
  c(
    NA, "10000000000", "20000000000",
    "30000000000", "40000000000", NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- structure(
  c(
    0, 1.48219693752374e-313, 4.94065645841247e-314,
    1.97626258336499e-313, 9.88131291682493e-314, 0
  ),
  class = "integer64", .Dim = 2:3
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)
#* u64 -------------------------------------------------------------------------
test <- "[[null,10000000000000000000,10000000000000000002],
          [10000000000000000003,10000000000000000004,null]]"
target <- matrix(
  c(
    NA, "10000000000000000000", "10000000000000000002",
    "10000000000000000003", "10000000000000000004", NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)


# mixed matrices ===============================================================
#* all types -------------------------------------------------------------------
test <- '[[null,"a",true,1,1.0,10000000000,10000000000000000000],
          [10000000000000000000,10000000000,1.0,1,true,"a",null]]'


target <- matrix(
  c(
    NA, "a", "TRUE", "1", "1.0", "10000000000", "10000000000000000000",
    "10000000000000000000", "10000000000", "1.0", "1", "TRUE", "a", NA
  ),
  nrow = 2L, ncol = 7L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

target <- list(
  list(NULL, "a", TRUE, 1L, 1, 1e+10, "10000000000000000000"),
  list("10000000000000000000", 1e+10, 1, 1L, TRUE, "a", NULL)
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, type_policy = type_policy$strict),
  target
)
#* mixed integers --------------------------------------------------------------
test <- "[[null,1,10000000000],
          [10000000000,1,null]]"

target <- matrix(
  c(
    NA, 1, 10000000000,
    10000000000, 1, NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

target <- matrix(
  c(
    NA, "1", "10000000000",
    "10000000000", "1", NA
  ),
  nrow = 2L, ncol = 3L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- structure(
  c(
    0, 4.94065645841247e-314, 4.94065645841247e-324,
    4.94065645841247e-324, 4.94065645841247e-314, 0
  ),
  class = "integer64", .Dim = 2:3
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test,
    int64_r_type = int64_opt$integer64
  ),
  target
)
#* logicals and integers -------------------------------------------------------
test <- "[[null,1,true,false],
          [false,true,1,null]]"
target <- matrix(
  c(
    NA, 1L, 1L, 0L,
    0L, 1L, 1L, NA
  ),
  nrow = 2L, ncol = 4L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* logicals and integer64 ------------------------------------------------------
test <- "[[null,10000000000,true,false],
          [false,true,10000000000,null]]"

target <- matrix(
  c(
    NA, "10000000000", "TRUE", "FALSE",
    "FALSE", "TRUE", "10000000000", NA
  ),
  nrow = 2L, ncol = 4L, byrow = TRUE
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$string),
  target
)

target <- structure(
  c(
    0, 0, 4.94065645841247e-314, 4.94065645841247e-324,
    4.94065645841247e-324, 4.94065645841247e-314, 0, 0
  ),
  class = "integer64", .Dim = c(2L, 4L)
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)
# objects ======================================================================
#* empty object ----------------------------------------------------------------
test <- '{}'
target <- structure(list(), .Names = character(0))
expect_identical(
  RcppSimdJson:::.deserialize_json(test, empty_object = target),
  target
)
#* simple named lists ----------------------------------------------------------
test <- '{"a":[1.0,2.0]}'
target <- list(a = c(1, 2))
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* deeply nested lists ---------------------------------------------------------
test <-
  '{
    "a": [
        1,
        {
            "b": [
                2,
                {
                    "c": 3,
                    "d": {
                        "data_frame": [
                            {
                                "scalar_col": 4,
                                "matrix_col": [
                                    [
                                        5,
                                        6
                                    ],
                                    [
                                        7,
                                        8
                                    ]
                                ]
                            }
                        ]
                    }
                }
            ]
        }
    ]
}'
target <- list(
  a = list(
    1L, list(
      b = list(
        2L,
        list(
          c = 3L,
          d = list(
            data_frame = structure(
              list(
                scalar_col = 4L,
                matrix_col = list(
                  structure(
                    c(5L, 7L, 6L, 8L),
                    .Dim = c(2L, 2L)
                  )
                )
              ),
              row.names = 1L,
              class = "data.frame"
            )
          )
        )
      )
    )
  )
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* simple data frames ----------------------------------------------------------
test <-
  '[
    {
        "these": 1,
        "names": null,
        "arent": true,
        "sorted": null
    },
    {
        "these": null,
        "names": "b",
        "arent": null,
        "sorted": 1.5,
        "col_missing_from_first_row": "face"
    }
]'

target <- data.frame(
  these = c(1L, NA),
  names = c(NA, "b"),
  arent = c(TRUE, NA),
  sorted = c(NA, 1.5),
  col_missing_from_first_row = c(NA, "face"),
  stringsAsFactors = FALSE
)

expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)
#* nested data frames ----------------------------------------------------------
test <-
  '[
    {
        "bool": true,
        "chr": "a",
        "dbl": 1.1,
        "int": 1,
        "int64": 10000000000,
        "list": [
            1,
            2
        ],
        "nested_dfs": [
            {
                "a": 1.1,
                "b": true,
                "c": "a"
            }
        ]
    },
    {
        "bool": false,
        "chr": "b",
        "dbl": 2.2,
        "int": 2,
        "int64": 20000000000,
        "list": [
            "a",
            "b"
        ],
        "nested_dfs": [
            {
                "a": 2.2,
                "b": false,
                "c": "b"
            }
        ]
    },
    {
        "bool": null,
        "chr": null,
        "dbl": null,
        "int": null,
        "int64": null,
        "list": [
            null,
            null
        ],
        "nested_dfs": [
            {
                "a": null,
                "b": null,
                "c": null
            }
        ]
    }
]'

target <- list(
  bool = c(TRUE, FALSE, NA),
  chr = c("a", "b", NA),
  dbl = c(1.1, 2.2, NA),
  int = c(1L, 2L, NA),
  int64 = structure(
    c(4.94065645841247e-314, 9.88131291682493e-314, 0),
    class = "integer64"
  ),
  list = list(c(1L, 2L), c("a", "b"), c(NA, NA)),
  nested_dfs = list(
    data.frame(a = 1.1, b = TRUE, c = "a"),
    data.frame(a = 2.2, b = FALSE, c = "b"),
    data.frame(a = NA, b = NA, c = NA)
  )
)
class(target) <- "data.frame"
rownames(target) <- 1:3

expect_identical(
  RcppSimdJson:::.deserialize_json(test, int64_r_type = int64_opt$integer64),
  target
)
# other simplify levels ========================================================
test <- '[{"a":[1,2],"g":[[3,4],[5,6]]}]'
#* matrix ----------------------------------------------------------------------
target <- list(
  list(
    a = 1:2,
    g = matrix(
      c(3L, 5L, 4L, 6L),
      nrow = 2L, ncol = 2L
    )
  )
)
expect_identical(
  RcppSimdJson:::.deserialize_json(test, simplify_to = simplify_lvl$matrix),
  target
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test, 
    type_policy = type_policy$ints_as_dbls,
    simplify_to = simplify_lvl$matrix
  ),
  target
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test, 
    type_policy = type_policy$strict,
    simplify_to = simplify_lvl$matrix
  ),
  target
)
#* vector ----------------------------------------------------------------------
target <- list(list(a = 1:2, g = list(3:4, 5:6)))
expect_identical(
  RcppSimdJson:::.deserialize_json(test, simplify_to = simplify_lvl$vector),
  target
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test, 
    type_policy = type_policy$ints_as_dbls,
    simplify_to = simplify_lvl$vector
  ),
  target
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test, 
    type_policy = type_policy$strict,
    simplify_to = simplify_lvl$vector
  ),
  target
)
#* list ------------------------------------------------------------------------
target <- list(list(a = list(1L, 2L), g = list(list(3L, 4L), list(5L, 6L))))
expect_identical(
  RcppSimdJson:::.deserialize_json(test, simplify_to = simplify_lvl$list),
  target
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test, 
    type_policy = type_policy$ints_as_dbls,
    simplify_to = simplify_lvl$list
  ),
  target
)
expect_identical(
  RcppSimdJson:::.deserialize_json(
    test, 
    type_policy = type_policy$strict,
    simplify_to = simplify_lvl$list
  ),
  target
)
  
