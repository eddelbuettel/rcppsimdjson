if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

target <- list(
  NA_INTEGER_equivalent = list(
    Double = -2147483648, 
    String = "-2147483648", 
    Integer64 = structure(NaN, class = "integer64")
  ),
  castable_int64_t = list(
    Double = 2147483647L, 
    String = 2147483647L, 
    Integer64 = 2147483647L
  ),
  scalar_int64_t = list(
    Double = 2147483648, 
    String = "2147483648", 
    Integer64 = structure(1.06099789548264e-314, class = "integer64")
  ), 
  scalar_uint64_t = list(
    Double = "9223372036854775808", 
    String = "9223372036854775808", 
    Integer64 = "9223372036854775808"
  ),
  castable_vec = list(
    Double = c(2147483647L, -2147483647L), 
    String = c(2147483647L, -2147483647L), 
    Integer64 = c(2147483647L, -2147483647L)
  ), 
  uncastable_vec = list(
    Double = c(2147483647, -2147483648), 
    String = c("2147483647", "-2147483648"), 
    Integer64 = structure(c(1.06099789498857e-314, NaN), class = "integer64")
  ),
  vec_int64_t = list(
    Double = c(-2147483648, 2147483648), 
    String = c("-2147483648", "2147483648"), 
    Integer64 = structure(c(NaN, 1.06099789548264e-314), class = "integer64")
  ), 
  vec_uint64_t = list(
    Double = c("9223372036854775808", "9223372036854775808"), 
    String = c("9223372036854775808", "9223372036854775808"), 
    Integer64 = c("9223372036854775808", "9223372036854775808")
  )
)


expect_identical(target, RcppSimdJson:::.check_int64())
