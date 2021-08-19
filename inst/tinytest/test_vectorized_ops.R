if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

# .deserialize_json ============================================================
test <- c(
  first = '{"A":[[1,2,3],[4,5,6]]}',
  second = '{"B":[{"a":1,"b":true},{"a":2,"b":false,"c":null}]}'
)

target <- list(
  first = list(
    A = matrix(
      c(
        1L, 2L, 3L,
        4L, 5L, 6L
      ),
      nrow = 2L, ncol = 3L, byrow = TRUE
    )
  ),
  second = list(
    B = data.frame(
      a = c(1L, 2L),
      b = c(TRUE, FALSE),
      c = c(NA, NA)
    )
  )
)

expect_identical(
  RcppSimdJson:::.deserialize_json(test),
  target
)

# confirm errors work ----------------------------------------------------------
test <- c(
  first = '{"A":[[1,2,3],[4,5,6]]}',
  bad_json = '{"B":[{"a":1,"b":JUNK},{"a":2,"b":false,"c":null}]}'
)
#expect_error(
#  RcppSimdJson:::.deserialize_json(test)
#)
# .load_json() =================================================================
test <- c(
  flatadversarial.json = "../jsonexamples/small/flatadversarial.json",
  adversarial.json = "../jsonexamples/small/adversarial.json"
)

if (!all(file.exists(test))) {
  exit_file(
    "flatadversarial.json and/or adversarial.json are missing."
  )
}

target <- list(
  flatadversarial.json = list(
    `"Name` = c("116", "\\\"", "234", "true", "FALSE"), t = 1e+10
  ),
  adversarial.json = list(
    `"Name rue` = structure(
      c("116", "\"", "234", "true", "FALSE"),
      .Dim = c(1L, 5L)
    )
  )
)

expect_identical(
  RcppSimdJson:::.load_json(test),
  target
)

# all files battery ------------------------------------------------------------
all_files <- dir("inst/jsonexamples", pattern = "\\.json$", 
                 recursive = TRUE, full.names = TRUE)
expect_silent(
  RcppSimdJson:::.load_json(all_files)
)
# confirm errors work ----------------------------------------------------------
expect_error(
  RcppSimdJson:::.load_json(c("a/fake/file.json", all_files))
)
expect_error(
  RcppSimdJson:::.load_json(c(all_files, "another/fake/file.json"))
)
