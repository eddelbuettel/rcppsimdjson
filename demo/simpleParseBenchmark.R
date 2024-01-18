#!/usr/bin/env Rscript

stopifnot(need_microbenchmark=requireNamespace("microbenchmark",quietly=TRUE),
          need_jsonify=requireNamespace("jsonify",quietly=TRUE),
          need_jsonlite=requireNamespace("jsonlite",quietly=TRUE),
          need_RcppSimdJson=requireNamespace("RcppSimdJson",quietly=TRUE),
          need_ndjson=requireNamespace("ndjson", quietly=TRUE),
          need_RJSONIO=requireNamespace("RJSONIO", quietly=TRUE),
          need_ggplot2=requireNamespace("ggplot2", quietly=TRUE),
          need_yyjsonr=requireNamespace("yyjsonr", quietly=TRUE))

file <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
json <- paste(readLines(file), collapse="")

res <- microbenchmark::microbenchmark(jsonify = jsonify::from_json(json),
                                      jsonlite = jsonlite::fromJSON(json),
                                      simdjson = RcppSimdJson::fparse(json),
                                      ndjson = ndjson::flatten(json),
                                      RJSONIO = RJSONIO::fromJSON(json),
                                      yyjsonr = yyjsonr::read_json_str(json),
                                      times = 100L)

res$expr <- with(res, reorder(expr, time, median))
print(res)
print(res, unit="relative")
ggplot2::autoplot(res) + ggplot2::labs(title="Parsing JSON string", caption="See 'demo/simpeParseBenchmark.R' in package RcppSimdJson") + tinythemes::theme_ipsum_rc()
