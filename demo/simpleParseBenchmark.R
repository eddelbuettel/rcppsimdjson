
library(microbenchmark)

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
