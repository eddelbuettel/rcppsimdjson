#!/usr/bin/env Rscript

stopifnot(need_microbenchmark=requireNamespace("microbenchmark",quietly=TRUE),
          need_jsonify=requireNamespace("jsonify",quietly=TRUE),
          need_jsonlite=requireNamespace("jsonlite",quietly=TRUE),
          need_RcppSimdJson=requireNamespace("RcppSimdJson",quietly=TRUE),
          need_ndjson=requireNamespace("ndjson", quietly=TRUE),
          need_RJSONIO=requireNamespace("RJSONIO", quietly=TRUE))

file <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
jsontxt <- readLines(file)

res <- microbenchmark::microbenchmark(jsonify = jsonify::validate_json(jsontxt),
                                      jsonlite = jsonlite::validate(jsontxt),
                                      simdjson = RcppSimdJson::validateJSON(file),
                                      ndjson = ndjson::validate(file),
                                      RJSONIO = RJSONIO::isValidJSON(file),
                                      times = 100L)

print(res, order="median")
print(res, order="median", unit="relative")
