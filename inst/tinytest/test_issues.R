if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

my_temp_dir <- paste0(tempdir(), "/rcppsimdjson-temp")
if (!dir.exists(my_temp_dir)) dir.create(my_temp_dir)

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


# json_url <- "https://raw.githubusercontent.com/eddelbuettel/rcppsimdjson/master/inst/jsonexamples/small/demo.json"
json_url <- paste0("file:///", system.file("jsonexamples/small/demo.json", package = "RcppSimdJson"))

fload(json_url, temp_dir = my_temp_dir, keep_temp_files = TRUE)
expect_true(
    length(dir(my_temp_dir)) == 1L
)
unlink(dir(my_temp_dir, full.names = TRUE))
fload(json_url, temp_dir = my_temp_dir)
expect_true(
    length(dir(my_temp_dir)) == 0L
)

# 35: accept raw vectors =======================================================
# https://github.com/eddelbuettel/rcppsimdjson/issues/35
expect_identical(
    fparse(charToRaw('[true,false]')),
    c(TRUE, FALSE)
)
expect_identical(
    fparse(lapply(c('[true,false]', '[false,true]'), charToRaw)),
    list(c(TRUE, FALSE), c(FALSE, TRUE))
)
# ensure lists only work if all raw
expect_error(
    fparse(list('[true,false]', '[false,true]')),
)
expect_error(
    fparse(list('[true,false]', charToRaw('[false,true]')))
)


unlink(my_temp_dir, recursive=TRUE, force=TRUE)
