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

my_temp_dir <- paste0(tempdir(), "/rcppsimdjson-temp")
dir.create(my_temp_dir)
json_url <- "https://raw.githubusercontent.com/eddelbuettel/rcppsimdjson/master/inst/jsonexamples/small/demo.json"

fload(json_url, temp_dir = my_temp_dir, keep_temp_files = TRUE)
expect_true(
    length(dir(my_temp_dir)) == 1L
)
unlink(dir(my_temp_dir, full.names = TRUE))
fload(json_url, temp_dir = my_temp_dir)
expect_true(
    length(dir(my_temp_dir)) == 0L
)
