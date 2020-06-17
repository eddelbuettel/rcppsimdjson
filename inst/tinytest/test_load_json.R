if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

all_files <- dir("../jsonexamples", pattern = "\\.json$", 
                 recursive = TRUE, full.names = TRUE)

sapply(all_files, function(.x) expect_silent(RcppSimdJson:::.load_json(.x)))

expect_error(
  RcppSimdJson:::.load_json("../jsonexamples/amazon_cellphones.ndjson")
)

expect_error(
  RcppSimdJson:::.load_json("not/a/real/file.json")
)
