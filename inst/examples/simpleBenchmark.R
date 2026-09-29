#!/usr/bin/env Rscript

stopifnot(need_microbenchmark=requireNamespace("microbenchmark",quietly=TRUE),
          need_jsonify=requireNamespace("jsonify",quietly=TRUE),
          need_jsonlite=requireNamespace("jsonlite",quietly=TRUE),
          need_RcppSimdJson=requireNamespace("RcppSimdJson",quietly=TRUE),
          need_ndjson=requireNamespace("ndjson", quietly=TRUE),
          need_RJSONIO=requireNamespace("RJSONIO", quietly=TRUE),
          need_yyjsonr=requireNamespace("yyjsonr", quietly=TRUE))

file <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
jsontxt <- readLines(file)

res <- microbenchmark::microbenchmark(jsonify = jsonify::validate_json(jsontxt),
                                      jsonlite = jsonlite::validate(jsontxt),
                                      simdjson = RcppSimdJson::validateJSON(file),
                                      ndjson = ndjson::validate(file),
                                      RJSONIO = RJSONIO::isValidJSON(file),
                                      yyjsonr = yyjsonr::validate_json_file(file),
                                      times = 100L)

res$expr <- with(res, reorder(expr, time, median))
print(res)
print(res, unit="relative")
p1 <- ggplot2::autoplot(res) +
    ggplot2::labs(title="Validating JSON file (or string)", caption="See 'demo/simpeBenchmark.R' in package RcppSimdJson") +
    tinythemes::theme_ipsum_rc()
p1
