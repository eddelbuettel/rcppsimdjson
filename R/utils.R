.is_scalar_lgl <- function(x, na_ok = FALSE) {
    length(x) == 1L && is.logical(x) && (na_ok || !is.na(x))
}

.is_scalar_chr <- function(x, na_ok = FALSE) {
    length(x) == 1L && is.character(x) && (na_ok || !is.na(x))
}

.prep_input <- function(input,
                        temp_dir,
                        compressed_download,
                        verbose) {
    input[!is.na(input)] <- path.expand(input[!is.na(input)])
    diagnosis <- .diagnose_input(input)

    if (length(missing_files <- diagnosis$input[!is.na(diagnosis$input) &
                                                !diagnosis$is_from_url &
                                                !file.exists(diagnosis$input)])) {
        stop("The following non-URL files don't exist:",
             sprintf("\n\t- %s", missing_files))
    }

    if (any(diagnosis$is_from_url)) {
        temp_files <- tempfile(
            pattern = basename(diagnosis$input[diagnosis$is_from_url]),
            tmpdir = temp_dir,
            fileext = if (compressed_download) sprintf("%s.gz", diagnosis$file_ext[diagnosis$is_from_url]) else diagnosis$file_ext[diagnosis$is_from_url]
        )

        if (any(diagnosis$is_local_file_url)) {
            download.file(
                url = diagnosis$input[diagnosis$is_local_file_url],
                method = "internal",
                destfile = temp_files[diagnosis$is_local_file_url],
                quiet = !verbose,
            )
        }

        if (any(diagnosis$is_from_url & !diagnosis$is_local_file)) {
            if ((.method <- getOption("download.file.method", default = "auto")) == "libcurl") {
                download.file(
                    url = diagnosis$input[diagnosis$is_from_url & !diagnosis$is_local_file],
                    method = .method,
                    destfile = temp_files[diagnosis$is_from_url & !diagnosis$is_local_file],
                    quiet = !verbose,
                    headers = if (compressed_download) c(`Accept-Encoding` = "gzip") else NULL
                )
            } else {
                for (i in seq_along(diagnosis$input[diagnosis$is_from_url & !diagnosis$is_local_file])) {
                    download.file(
                        url = diagnosis$input[diagnosis$is_from_url & !diagnosis$is_local_file][[i]],
                        destfile = temp_files[diagnosis$input[diagnosis$is_from_url & !diagnosis$is_local_file][[i]]],
                        method = .method,
                        quiet = !verbose,
                        headers = if (compressed_download) c(`Accept-Encoding` = "gzip") else NULL
                    )
                }
            }
        }

        diagnosis$input[diagnosis$is_from_url] <- temp_files
    }

    diagnosis
}


