if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

# https://github.com/simdjson/simdjson/blob/master/tests/unicode_tests.cpp
valid_utf8 <- c(
    "a",
    "\xc3\xb1",
    "\xe2\x82\xa1",
    "\xf0\x90\x8c\xbc",
    "\xc2\x80",
    "\xf0\x90\x80\x80",
    "\xee\x80\x80",
    "\xef\xbb\xbf"
)
names(valid_utf8) <- paste0("valid_utf8_", seq_along(valid_utf8))

invalid_utf8 <- c(
    "\xc3\x28",
    "\xa0\xa1",
    "\xe2\x28\xa1",
    "\xe2\x82\x28",
    "\xf0\x28\x8c\xbc",
    "\xf0\x90\x28\xbc",
    "\xf0\x28\x8c\x28",
    "\xc0\x9f",
    "\xf5\xff\xff\xff",
    "\xed\xa0\x81",
    "\xf8\x90\x80\x80\x80",
    "123456789012345\xed",
    "123456789012345\xf1",
    "123456789012345\xc2",
    "\xC2\x7F",
    "\xce",
    "\xce\xba\xe1",
    "\xce\xba\xe1\xbd",
    "\xce\xba\xe1\xbd\xb9\xcf",
    "\xce\xba\xe1\xbd\xb9\xcf\x83\xce",
    "\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce",
    "\xdf",
    "\xef\xbf",
    "\x80",
    "\x91\x85\x95\x9e",
    "\x6c\x02\x8e\x18"
)
names(invalid_utf8) <- paste0("invalid_utf8_", seq_along(invalid_utf8))


expect_identical(
    `names<-`(rep(TRUE, length(valid_utf8)),  paste0("valid_utf8_", seq_along(valid_utf8))),
    is_valid_utf8(valid_utf8)
)
expect_identical(
    `names<-`(rep(FALSE, length(invalid_utf8)),  paste0("invalid_utf8_", seq_along(invalid_utf8))),
    is_valid_utf8(invalid_utf8)
)

expect_true(
    all(is_valid_utf8(valid_utf8)) &&
    all(vapply(valid_utf8, function(.x) is_valid_utf8(charToRaw(.x)), logical(1L))) &&
    is_valid_utf8(lapply(valid_utf8, charToRaw)) &&
    all(validUTF8(valid_utf8))
)

expect_false(
    any(is_valid_utf8(invalid_utf8)) ||
    any(vapply(invalid_utf8, function(.x) is_valid_utf8(charToRaw(.x)), logical(1L))) ||
    is_valid_utf8(lapply(invalid_utf8, charToRaw)) ||
    any(validUTF8(invalid_utf8))
)

expect_identical(is_valid_utf8(NA_character_), NA)
expect_error(is_valid_utf8(NULL))
expect_error(is_valid_utf8(1L))
expect_error(is_valid_utf8(3.14))
expect_error(is_valid_utf8(TRUE))
expect_error(is_valid_utf8(list(charToRaw('"VALID JSON"'), FALSE)))


prettified_json <- c(
'{
    "Image": {
        "Width":  800,
        "Height": 600,
        "Title":  "View from 15th Floor",
        "Thumbnail": {
            "Url":    "http://www.example.com/image/481989943",
            "Height": 125,
            "Width":  100
        },
        "Animated" : false,
        "IDs": [116, 943, 234, 38793]
    }
}',
'{
    "Width": 800,
    "Height": 600,
    "Title": "View from my room",
    "Url": "http://ex.com/img.png",
    "Private": false,
    "Thumbnail": {
        "Url": "http://ex.com/th.png",
        "Height": 125,
        "Width": 100
    },
    "array": [
        116,
        943,
        234
    ],
    "Owner": null
}',
"THIS IS JUNK!"
)
minified_json <- c(
'{"Image":{"Width":800,"Height":600,"Title":"View from 15th Floor","Thumbnail":{"Url":"http://www.example.com/image/481989943","Height":125,"Width":100},"Animated":false,"IDs":[116,943,234,38793]}}',
'{"Width":800,"Height":600,"Title":"View from my room","Url":"http://ex.com/img.png","Private":false,"Thumbnail":{"Url":"http://ex.com/th.png","Height":125,"Width":100},"array":[116,943,234],"Owner":null}',
NA_character_
)

expect_identical(
    fminify(prettified_json),
    minified_json
)
expect_identical(
    fminify(charToRaw(prettified_json[[1L]])),
    minified_json[[1L]]
)
expect_identical(
    fminify(charToRaw("junk")),
    NA_character_
)
expect_identical(
    fminify(charToRaw(prettified_json[[1L]])),
    minified_json[[1L]]
)
expect_identical(
    fminify(lapply(prettified_json, charToRaw)),
    minified_json
)
expect_error(fminify(NULL))
expect_error(fminify(1L))
expect_error(fminify(1.0))
expect_error(fminify(FALSE))
expect_identical(fminify(NA_character_), NA_character_)

expect_identical(fminify(NA_character_), NA_character_)
expect_error(fminify(NULL))
expect_error(fminify(1L))
expect_error(fminify(3.14))
expect_error(fminify(TRUE))
expect_error(fminify(list(charToRaw('\n"VALID JSON"'), FALSE)))


# is_valid_json ===============================================================
expect_true(all(is_valid_json(minified_json[!is.na(minified_json)])))
expect_true(is_valid_json(charToRaw(minified_json[[1L]])))
expect_true(
    all(
        is_valid_json(
            lapply(minified_json[!is.na(minified_json)], charToRaw)
        )
    )
)

expect_identical(is_valid_json(NA_character_), NA)
expect_error(is_valid_json(NULL))
expect_error(is_valid_json(1L))
expect_error(is_valid_json(1.0))
expect_error(is_valid_json(FALSE))
expect_error(is_valid_json(list(charToRaw('"VALID JSON"'), FALSE)))

# Change to expect_true since valid json is only detected when parsed/accessed.
#expect_false(any(is_valid_json(valid_utf8)))
expect_true(any(is_valid_json(valid_utf8)))
expect_false(any(is_valid_json(invalid_utf8)))

