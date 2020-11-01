#' @name simdjson-utilities
#'
#' @title simdjson Utilities
#'
#' @param x String(s), or raw vectors representing string(s).
#'
#' @param json JSON string(s), or raw vectors representing JSON string(s)
#'
#' @examples
#' prettified_json <-
#'     '[
#'     {
#'         "b": true,
#'         "c": null
#'     },
#'     {
#'         "b": [
#'             [
#'                 1,
#'                 2,
#'                 3
#'             ],
#'             [
#'                 4,
#'                 5,
#'                 6
#'             ]
#'         ],
#'         "c": "Q"
#'     }
#' ]'
#'
#' example_text <- list(
#'     valid_json = c(json1 = prettified_json,
#'                    json2 = '{\n\t"good_json":true\n}'),
#'     invalid_json = c(bad_json1 = "BAD JSON",
#'                      bad_json2 = `Encoding<-`('"fa\xE7ile"', "latin1")),
#'     mixed_json = c(na = NA_character_, good_json = '{"good_json":true}',
#'                    bad_json =  `Encoding<-`('"fa\xE7ile"', "latin1")),
#'     good_raw_json = charToRaw('{\n\t"good_json":true\n}'),
#'     bad_raw_json = charToRaw("JUNK"),
#'     list_of_raw_json = lapply(
#'         c(na = NA_character_, good_json = '{"good_json":true}',
#'           bad_json =  `Encoding<-`('"fa\xE7ile"', "latin1")),
#'         charToRaw
#'     ),
#'     not_utf8 = `Encoding<-`('"fa\xE7ile"', "latin1")
#' )
#'
#' # UTF-8 validation ==========================================================
#' example_text$valid_json
#' is_valid_utf8(example_text$valid_json)
#'
#' example_text$invalid_json
#' is_valid_utf8(example_text$invalid_json)
#'
#' example_text$mixed_json
#' is_valid_utf8(example_text$mixed_json)
#'
#' example_text$good_raw_json
#' is_valid_utf8(example_text$good_raw_json)
#'
#' example_text$bad_raw_json
#' is_valid_utf8(example_text$bad_raw_json)
#'
#' example_text$list_of_raw_json
#' is_valid_utf8(example_text$list_of_raw_json)
#'
#' example_text$not_utf8
#' is_valid_utf8(example_text$not_utf8)
#' is_valid_utf8(iconv(example_text$not_utf8, from = "latin1", to = "UTF-8"))
#'
#' # JSON validation ===========================================================
#' cat(example_text$valid_json[[1L]])
#' cat(example_text$valid_json[[2L]])
#' is_valid_json(example_text$valid_json)
#'
#' example_text$invalid_json
#' is_valid_json(example_text$invalid_json)
#'
#' example_text$mixed_json
#' is_valid_json(example_text$mixed_json)
#'
#' example_text$good_raw_json
#' cat(rawToChar(example_text$good_raw_json))
#' is_valid_json(example_text$good_raw_json)
#'
#' example_text$bad_raw_json
#' rawToChar(example_text$bad_raw_json)
#' is_valid_json(example_text$bad_raw_json)
#'
#' example_text$list_of_raw_json
#' lapply(example_text$list_of_raw_json, rawToChar)
#' is_valid_json(example_text$list_of_raw_json)
#'
#' example_text$not_utf8
#' Encoding(example_text$not_utf8)
#' is_valid_json(example_text$not_utf8)
#' is_valid_json(iconv(example_text$not_utf8, from = "latin1", to = "UTF-8"))
#'
#' # JSON minification =========================================================
#' cat(example_text$valid_json[[1L]])
#' cat(example_text$valid_json[[2L]])
#' fminify(example_text$valid_json)
#'
#' example_text$invalid_json
#' fminify(example_text$invalid_json)
#'
#' example_text$mixed_json
#' fminify(example_text$mixed_json)
#'
#' example_text$good_raw_json
#' cat(rawToChar(example_text$good_raw_json))
#' fminify(example_text$good_raw_json)
#'
#' example_text$bad_raw_json
#' rawToChar(example_text$bad_raw_json)
#' fminify(example_text$bad_raw_json)
#'
#' example_text$list_of_raw_json
#' lapply(example_text$list_of_raw_json, rawToChar)
#' fminify(example_text$list_of_raw_json)
#'
#' example_text$not_utf8
#' Encoding(example_text$not_utf8)
#' fminify(example_text$not_utf8)
#' fminify(iconv(example_text$not_utf8, from = "latin1", to = "UTF-8"))
NULL




