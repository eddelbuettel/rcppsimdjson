if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

# 33: prevent deletion of local files ==========================================
# https://github.com/eddelbuettel/rcppsimdjson/issues/33
temp_file1 <- tempfile(".json")
temp_file2 <- tempfile(".json")
writeLines('"json"', temp_file1)
writeLines('"json"', temp_file2)

fload(c(temp_file1, temp_file2), keep_temp_files = TRUE)
expect_true(all(file.exists(c(temp_file1, temp_file2))))
fload(c(temp_file1, temp_file2), keep_temp_files = FALSE)
expect_true(all(file.exists(c(temp_file1, temp_file2))))
