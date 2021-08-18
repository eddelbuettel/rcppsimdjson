if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")
library(RcppSimdJson)

# parse errors =================================================================
expect_error(fparse("junk", query = ""))
expect_error(fparse("junk", query = c("", "")))
expect_error(fparse(c("junk", "junk"), query = list("", "")))
expect_identical(
    fparse("junk", query = "", parse_error_ok = TRUE),
    NULL
)

#Change to expect_error. By design, On Demand does not detect broken JSON until it is accessed/parsed.
expect_error(
    fparse("junk", query = c("", ""), parse_error_ok = TRUE)
)
expect_error(
    fparse(c("junk", "junk"), query = list("", ""), parse_error_ok = TRUE)
)
expect_error(
    fparse(c("junk", "junk"), query = list("", ""), parse_error_ok = TRUE,
           on_parse_error = NA),
)
#expect_identical(
#    fparse("junk", query = c("", ""), parse_error_ok = TRUE),
#    NULL
#)
#expect_identical(
#    fparse(c("junk", "junk"), query = list("", ""), parse_error_ok = TRUE),
#    list(NULL, NULL)
#)
#expect_identical(
#    fparse(c("junk", "junk"), query = list("", ""), parse_error_ok = TRUE,
#           on_parse_error = NA),
#    list(NA, NA)
#)

# query errors =================================================================
expect_error(fparse("null", query = TRUE))
expect_error(fparse("null", query = character(0L)))
expect_error(fparse("null", query_error_ok = NA))
expect_error(fparse("null", query = "junk"))
expect_error(fparse("null", query = c("junk", "junk")))
expect_error(fparse(c("null", "null"), query = list("junk", "junk")))
expect_error(fparse("null", query = list("junk", "junk")))
expect_error(fparse(c("null", "null"), query = list(TRUE, FALSE)))


expect_identical(fparse("null", query = "junk", query_error_ok = TRUE), NULL)
expect_identical(
    fparse("null", query = "junk", query_error_ok = TRUE),
    NULL
)
expect_identical(
    fparse(c("null", "null"),
           query = list(c("junk", "junk"), c("junk", "junk")),
           query_error_ok = TRUE),
    list(list(NULL, NULL), list(NULL, NULL))
)
expect_identical(
    fparse(c("null", "null"),
           query = list(c("junk", "junk"), c("junk", "junk")),
           query_error_ok = TRUE,
           on_query_error = NA),
    list(list(NA, NA), list(NA, NA))
)

# single json ==================================================================
js <- c(single_json = '[{"a":[[1,2],[3,4]]},{"b":[[5,6],[7,8]]}]')
expect_identical(
    fparse(js),
    fparse(js, query = "")
)
expect_identical(
    fparse(js, NA_character_),
    NA
)
expect_identical(
    fparse(js, c(na1 = NA_character_, na2 = NA_character_)),
    list(na1 = NA, na2 = NA)
)
expect_identical(
    fparse(js, list(NA_character_)),
    list(single_json = list(NA))
)
expect_identical(
    fparse(js, list(na = NA_character_)),
    list(na = list(NA))
)

#* flat query ------------------------------------------------------------------
#** single query ---------------------------------------------------------------
expect_identical(
    fparse(js, query = "/0/a/0"),
    1:2
)
expect_identical(
    fparse(js, query = c(a = "/0/a/0")),
    1:2
)
expect_identical(
    fparse(js, query = "/1/b/1"),
    7:8
)
#** multi query ----------------------------------------------------------------
expect_identical(
    fparse(js, query = c("/0/a/0", "/1/b/1")),
    list(1:2, 7:8)
)
expect_identical(
    fparse(js, query = c(a = "/0/a/0", b = "/1/b/1")),
    list(a = 1:2, b = 7:8)
)

#* nested query ----------------------------------------------------------------
q <- list(c("/0/a/0", "/1/b/1"))
expect_identical(
    fparse(js, query = q),
    list(single_json = list(1:2, 7:8))
)
expect_identical(
    fparse(unname(js), query = q),
    list(list(1:2, 7:8))
)

q <- list(queries = c("/0/a/0", "/1/b/1"))
expect_identical(
    fparse(js, query = q),
    list(queries = list(1:2, 7:8))
)

q <- list(c(a = "/0/a/0", b = "/1/b/1"))
expect_identical(
    fparse(js, query = q),
    list(single_json = list(a = 1:2, b = 7:8))
)
expect_identical(
    fparse(unname(js), query = q),
    list(list(a = 1:2, b = 7:8))
)

q <- list(queries = c(a = "/0/a/0", b = "/1/b/1"))
expect_identical(
    fparse(js, query = q),
    list(queries = list(a = 1:2, b = 7:8))
)

#** multi query ----------------------------------------------------------------
q <- list(c("/0/a/0", "/0/a/1"))
target <- list(single_json = list(1:2, 3:4))
expect_identical(
    fparse(js, query = q),
    target
)
expect_identical(
    fparse(unname(js), query = q),
    unname(target)
)

q <- list(queries = c(a1 = "/0/a/0", a2 = "/0/a/1",
                      b1 = "/1/b/0", b2 = "/1/b/1"))
expect_identical(
    fparse(js, query = q),
    list(queries = list(a1 = 1:2, a2 = 3:4, b1 = 5:6, b2 = 7:8))
)

# multi json ===================================================================

js <- c(A = '{"a":[[1,2],[3,4]]}', B = '{"a":[[5,6],[7,8]]}')
#* flat query ------------------------------------------------------------------
#** single query ---------------------------------------------------------------
expect_identical(
    fparse(js, query = "/a/0"),
    list(A = 1:2, B = 5:6)
)
#** multi query ----------------------------------------------------------------
expect_identical(
    fparse(js, query = c("/a/0", "/a/1")),
    list(A = list(1:2, 3:4), B = list(5:6, 7:8))
)
expect_identical(
    fparse(js, query = c(a = "/a/0", b = "/a/1")),
    list(A = list(a = 1:2, b = 3:4), B = list(a = 5:6, b = 7:8))
)

#* nested query ----------------------------------------------------------------
js <- c(A = '{"a":[[1,2],[3,4]]}', B = '{"b":[[5,6],[7,8]]}')
q <- list(c("/a/0", "/a/1"),
          c("/b/1", "/b/0"))
expect_identical(
    fparse(js, query = q),
    list(A = list(1:2, 3:4), B = list(7:8, 5:6))
)

q <- list(a = c("/a/0", "/a/1"),
          b = c("/b/1", "/b/0"))
expect_identical(
    fparse(js, query = q),
    list(a = list(1:2, 3:4), b = list(7:8, 5:6))
)

q <- list(a = c(a1 = "/a/0", a2 = "/a/1"),
          b = c(b1 = "/b/0", b2 = "/b/1"))
expect_identical(
    fparse(js, query = q),
    list(a = list(a1 = 1:2, a2 = 3:4), b = list(b1 = 5:6, b2 = 7:8))
)

q <- list(a = c(a1 = "/a/0", a2 = "/a/1"),
          b = c(b1 = "/b/0", b2 = "/b/1"))
expect_identical(
    fparse(js, query = q),
    list(a = list(a1 = 1:2, a2 = 3:4), b = list(b1 = 5:6, b2 = 7:8))
)

