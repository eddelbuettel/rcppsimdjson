
library(RcppSimdJson)

expect_stdout(parseExample())

# release_json_memory() ========================================================
invisible(fparse('{"a": [1, 2, 3], "b": "text"}'))
freed <- release_json_memory()
expect_true(freed > 0)
expect_identical(release_json_memory(), 0)                       # nothing left to free
expect_identical(fparse('{"a": [1, 2, 3], "b": "text"}'),         # still works afterwards
                 list(a = 1:3, b = "text"))
