.is_scalar_lgl <- function(x, na_ok = FALSE) {
    length(x) == 1L && is.logical(x) && (na_ok || !is.na(x))
}

.is_scalar_chr <- function(x, na_ok = FALSE) {
    length(x) == 1L && is.character(x) && (na_ok || !is.na(x)) 		# #nocov
}

.drop_file_ext <- function(file_path, file_ext) {
    mapply(function(.file_path, .file_ext) {
        if (nchar(.file_ext) == 0L) .file_path				# #nocov
        else sub(sprintf("%s$", .file_ext), "", .file_path)
    }, file_path, file_ext, USE.NAMES = FALSE)
}

.prep_input <- function(input,
                        temp_dir,
                        compressed_download,
                        verbose,
                        headers = NULL,
                        ...) {
    input[!is.na(input)] <- path.expand(input[!is.na(input)])
    diagnosis <- .diagnose_input(input)
    .headers <- headers

    if (length(missing_files <- diagnosis$input[!is.na(diagnosis$input) &
                                                !diagnosis$is_from_url &
                                                !file.exists(diagnosis$input)])) {
        stop("The following non-URL files don't exist:",
             sprintf("\n\t- %s", missing_files))
    }

    if (any(diagnosis$is_from_url)) {
        if (compressed_download) {      				# #nocov start
            .headers <- c(.headers, `Accept-Encoding` = "gzip")
            # for local files, don't attach .gz
            .fileext <- rep(NA_character_, nrow(diagnosis))
            .fileext[diagnosis$is_local_file_url] <- diagnosis$file_ext[diagnosis$is_local_file_url]
            .fileext[diagnosis$is_remote_file_url] <- sprintf("%s.gz", diagnosis$file_ext[diagnosis$is_remote_file_url])
        } else {      							# #nocov end
            .fileext <-  diagnosis$file_ext[diagnosis$is_from_url]
        }

        temp_files <- rep(NA_character_, nrow(diagnosis))
        temp_files[diagnosis$is_from_url] <- tempfile(
            pattern = .drop_file_ext(basename(gsub("[?]", "_", diagnosis$input[diagnosis$is_from_url])),
                                     diagnosis$file_ext[diagnosis$is_from_url]),
            tmpdir = normalizePath(temp_dir),
            fileext = .fileext[diagnosis$is_from_url]
        )

        if (any(diagnosis$is_local_file_url)) {
            .url <- diagnosis$input[diagnosis$is_local_file_url]
            .destfile <- temp_files[diagnosis$is_local_file_url]

            download.file(
                url = .url,
                method = "internal",
                destfile = .destfile,
                quiet = !verbose,
            )
        }

        if (any(diagnosis$is_remote_file_url)) {			# #nocov start
            .url <- diagnosis$input[diagnosis$is_remote_file_url]
            .destfile <- temp_files[diagnosis$is_remote_file_url]

            if ((.method <- getOption("download.file.method", default = "auto")) == "libcurl") {
                download.file(
                    url = .url,
                    method = .method,
                    destfile = .destfile,
                    quiet = !verbose,
                    headers = .headers					# #nocov end
                )
            } else {							# #nocov start
                for (i in seq_along(diagnosis$input[diagnosis$is_remote_file_url])) {
                    .url <- diagnosis$input[diagnosis$is_remote_file_url][[i]]
                    .destfile <- temp_files[diagnosis$is_remote_file_url][[i]]
                    download.file(
                        url = .url,
                        destfile = .destfile,
                        method = .method,
                        quiet = !verbose,
                        headers = .headers				# #nocov end
                    )
                }
            }
        }

        diagnosis$input[diagnosis$is_from_url] <- temp_files[diagnosis$is_from_url]
    }

    diagnosis
}
