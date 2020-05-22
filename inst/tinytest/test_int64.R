expect_identical(
  RcppSimdJson:::.check_int64(is_signed = TRUE, integer64 = 0),
  3e9
)
expect_identical(
  RcppSimdJson:::.check_int64(is_signed = FALSE, integer64 = 0),
  3e9
)

expect_identical(
  RcppSimdJson:::.check_int64(is_signed = TRUE, integer64 = 1),
  "3000000000"
)
expect_identical(
  RcppSimdJson:::.check_int64(is_signed = FALSE, integer64 = 1),
  "3000000000"
)

expect_identical(
  RcppSimdJson:::.check_int64(is_signed = TRUE, integer64 = 2),
  bit64::as.integer64("3000000000")
)
expect_identical(
  RcppSimdJson:::.check_int64(is_signed = FALSE, integer64 = 2),
  bit64::as.integer64("3000000000")
)

