.is_scalar_lgl <- function(x, na_ok = FALSE) {
    length(x) == 1L && is.logical(x) && (na_ok || !is.na(x))
}

.is_scalar_chr <- function(x, na_ok = FALSE) {
    length(x) == 1L && is.character(x) && (na_ok || !is.na(x))
}
