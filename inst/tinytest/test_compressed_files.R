library(RcppSimdJson)
if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

my_temp_dir <- paste0(tempdir(), "/rcppsimdjson-compressed-temp")
if (!dir.exists(my_temp_dir)) dir.create(my_temp_dir)

.read_compress_write_load <- function(file_path) {
    types <- c("gzip", "bzip2", "xz")
    exts <- c("gz",    "bz2",   "xz")

    init <- readBin(file_path, n = file.size(file_path), what = "raw")

    mapply(function(type, ext) {
        target_path <- paste0(my_temp_dir, "/", basename(file_path), ".", ext)
        writeBin(memCompress(init, type = type), target_path)
        fload(target_path)
    }, types, exts, USE.NAMES = FALSE, SIMPLIFY = FALSE)
}

all_files <- dir(
    system.file("jsonexamples", package = "RcppSimdJson"),
    recursive = TRUE,
    pattern = "\\.json$",
    full.names = TRUE
)

sapply(all_files, function(.x) {
    expect_silent(.read_compress_write_load(.x))
})

unlink(my_temp_dir, recursive=TRUE, force=TRUE)
