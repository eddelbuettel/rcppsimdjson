if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

target <- list(
  scalar_int64_t = list(
    Double = 3e+09, 
    String = "3000000000", 
    Integer64 = structure(1.48219693752374e-314, class = "integer64")
  ), 
  scalar_uint64_t = list(
    Double = "10000000000000000000", 
    String = "10000000000000000000", 
    Integer64 = "10000000000000000000"
  ), 
  vec_int64_t = list(
    Double = rep(3e+09, 4), 
    String = rep("3000000000", 4), 
    Integer64 = structure(rep(1.48219693752374e-314, 4), class = "integer64")
  ), 
  vec_uint64_t = list(
    Double = rep("10000000000000000000", 4),
    String = rep("10000000000000000000", 4),
    Integer64 = rep("10000000000000000000", 4)
  )
)

expect_equal(target, RcppSimdJson:::.check_int64())
