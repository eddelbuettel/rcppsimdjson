
if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

library(RcppSimdJson)

jsonfiles <- list.files("../jsonexamples", pattern="\\.json$", recursive=TRUE, full.names=TRUE)

expect_true(length(jsonfiles) >= 23)

sapply(jsonfiles, function(jf) expect_true(validateJSON(jf), info=jf))

expect_error(validateJSON("../jsonexamples/amazon_cellphones.ndjson"))
