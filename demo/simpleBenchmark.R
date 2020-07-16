#!/usr/bin/env Rscript

if (!require("microbenchmark")) install.packages("microbenchmark")
library(microbenchmark)

if (!require("jsonify")) install.packages("jsonify")
if (!require("jsonlite")) install.packages("jsonlite")
if (!require("RcppSimdJson")) install.packages("RcppSimdJson")
if (!require("ndjson")) install.packages("ndjson")
if (!require("RJSONIO")) install.packages("RJSONIO")

file <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
jsontxt <- readLines(file)

res <- microbenchmark(jsonify = jsonify::validate_json(jsontxt),
                      jsonlite = jsonlite::validate(jsontxt),
                      simdjson = RcppSimdJson::validateJSON(file),
                      ndjson = ndjson::validate(file),
                      RJSONIO = RJSONIO::isValidJSON(file),
                      times = 100L)

print(res, order="median")
print(res, order="median", unit="relative")
