
library(microbenchmark)

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
