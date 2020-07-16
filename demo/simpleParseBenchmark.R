#!/usr/bin/env Rscript

if (!require("microbenchmark")) install.packages("microbenchmark")
library(microbenchmark)

if (!require("jsonify")) install.packages("jsonify")
if (!require("jsonlite")) install.packages("jsonlite")
if (!require("RcppSimdJson")) install.packages("RcppSimdJson")
if (!require("ndjson")) install.packages("ndjson")
if (!require("RJSONIO")) install.packages("RJSONIO")
if (!require("ggplot2")) install.packages("ggplot2")

file <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
json <- paste(readLines(file), collapse="")

res <- microbenchmark(jsonify = jsonify::from_json(json),
                      jsonlite = jsonlite::fromJSON(json),
                      simdjson = RcppSimdJson::fparse(json),
                      ndjson = ndjson::flatten(json),
                      RJSONIO = RJSONIO::fromJSON(json),
                      times = 100L)

res$expr <- with(res, reorder(expr, time, median))
res
ggplot2::autoplot(res)
