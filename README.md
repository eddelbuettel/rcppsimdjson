## RcppSimdJson: Rcpp Bindings for the simdjson Header Library

[![CI](https://github.com/eddelbuettel/rcppsimdjson/workflows/ci/badge.svg)](https://github.com/eddelbuettel/rcppsimdjson/actions?query=workflow%3Aci)
[![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](https://www.r-project.org/Licenses/GPL-2)
[![CRAN](https://www.r-pkg.org/badges/version/RcppSimdJson)](https://cran.r-project.org/package=RcppSimdJson)
[![Dependencies](https://tinyverse.netlify.app/badge/RcppSimdJson)](https://cran.r-project.org/package=RcppSimdJson)
[![Downloads](https://cranlogs.r-pkg.org/badges/RcppSimdJson?color=brightgreen)](https://www.r-pkg.org/pkg/RcppSimdJson)
[![Code Coverage](https://codecov.io/gh/eddelbuettel/RcppSimdJson/graph/badge.svg)](https://app.codecov.io/gh/eddelbuettel/RcppSimdJson)
[![Last Commit](https://img.shields.io/github/last-commit/eddelbuettel/rcppsimdjson)](https://github.com/eddelbuettel/rcppsimdjson)

### Motivation

[simdjson](https://github.com/simdjson/simdjson) by [Daniel Lemire](https://lemire.me/en/) (with contributions by [Geoff
Langdale](https://branchfree.org/),  [John Keiser](https://github.com/jkeiser) and  [many others](https://github.com/simdjson/simdjson/graphs/contributors)) is an
engineering marvel.  Through very clever use of [SIMD instructions](https://en.wikipedia.org/wiki/SIMD),
it manages to parse JSON files faster than disc access. Wut? Yes you read that right: parallel
processing with so little overhead that the net throughput is limited only by disk speed.

Moreover, it is implemented in neat modern C++ and can be accessed as a header-only library. (Well,
one library in two files, really.)  Which makes R packaging easy and convenient and compelling. So
here we are.

For further introduction, see the [arXiv paper](https://arxiv.org/abs/1902.08318) by Langdale and Lemire (out/to appear in VLDB
Journal 28(6) as well) and/or the video of the [recent talk by Daniel Lemire at
QCon](https://www.youtube.com/watch?v=wlvKAT7SZIQ) (voted best talk).

### Example

```r
jsonfile <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
library(RcppSimdJson)
validateJSON(jsonfile)                  # validate a JSON file
res <- fload(jsonfile)                  # parse a JSON file
```

### Comparison

A [simple file-oriented parsing benchmark][simpleBenchmark] against the other R-accessible JSON
parsers:

```r
> print(res)
Unit: microseconds
     expr       min        lq      mean    median        uq       max neval   cld
 simdjson   101.991   114.764   127.742   123.070   133.514   202.078   100 a
  yyjsonr   117.089   140.452   157.350   156.107   169.136   281.537   100 a
  jsonify  1092.174  1112.041  1228.807  1137.108  1367.544  1778.297   100  b
 jsonlite  1941.132  1972.886  2022.138  1989.743  2016.072  2766.606   100   c
  RJSONIO  4459.464  4524.275  4633.694  4552.539  4610.932  6516.523   100    d
   ndjson 35795.875 36269.607 36495.140 36378.285 36636.018 44191.303   100     e
>
> print(res, unit="relative")
Unit: relative
     expr       min        lq      mean    median       uq       max neval   cld
 simdjson   1.00000   1.00000   1.00000   1.00000   1.0000   1.00000   100 a
  yyjsonr   1.14803   1.22383   1.23178   1.26844   1.2668   1.39321   100 a
  jsonify  10.70853   9.68976   9.61944   9.23952  10.2427   8.80005   100  b
 jsonlite  19.03239  17.19073  15.82986  16.16757  15.1000  13.69078   100   c
  RJSONIO  43.72409  39.42226  36.27384  36.99146  34.5351  32.24756   100    d
   ndjson 350.97092 316.03507 285.69405 295.59019 274.3973 218.68438   100     e
>
```

Or in chart form, also including the [second benchmark parsing strings][simpleParseBenchmark]:

![](https://eddelbuettel.github.io/images/2026-10-13/rcppsimdjson_both_benchmarks.png)

### Status

All three major OSs are supported, and JSON can be parsed from file and
string under a variety of settings. A C++20 compiler is required (though the
upstream can fall back to older compiler; one can edit / override
[src/Makevars](https://github.com/eddelbuettel/rcppsimdjson/blob/master/src/Makevars.in)
accordingly if need be).

### Contributing

Any problems, bug reports, or features requests for the package can be submitted and handled most
conveniently as [Github issues](https://github.com/eddelbuettel/rcppsimdjson/issues) in the repository.

Before submitting pull requests, it is frequently preferable to first discuss need and scope in such
an issue ticket.  See the file
[Contributing.md](https://github.com/RcppCore/Rcpp/blob/master/Contributing.md) (in the
[Rcpp](https://github.com/RcppCore/Rcpp) repo) for a brief discussion.


### See Also

For standard JSON work on R, as well as for other nicely done C++ libraries, consider these:

- [jsonlite](https://cran.r-project.org/package=jsonlite) by [Jeroen Ooms](https://github.com/jeroen) is excellent, very versatile, and probably most-widely used;
- [rapidjsonr](https://cran.r-project.org/package=rapidjsonr) and [jsonify](https://cran.r-project.org/package=jsonify) by [David Cooley](https://github.com/dcooley) bringing [RapidJSON](https://rapidjson.org/) to R;
- [ndjson](https://cran.r-project.org/package=ndjson) by [Bob Rudis](https://rud.is/b/) builds on the [JSON for Modern C++](https://github.com/nlohmann/json) library by [Niels Lohmann](https://github.com/nlohmann);
- [RJSONIO](https://cran.r-project.org/package=RJSONIO) by [Duncan Temple Lang](https://www.stat.ucdavis.edu/~duncan/) started all this but could use a little love;
- [yyjsonr](https://cran.r-project.org/package=yyjsonr) by [Mike Cheng](https://coolbutuseless.github.io/) is a more recent and performant addition based on [yyjson](https://github.com/ibireme/yyjson).

### Author

For the R package, [Dirk Eddelbuettel](https://github.com/eddelbuettel) and [Brendan
Knapp](https://github.com/knapply).

For everything pertaining to simdjson, [Daniel Lemire](https://lemire.me/en/) (and [many
contributors](https://github.com/simdjson/simdjson/graphs/contributors)).


[simpleBenchmark]: https://github.com/eddelbuettel/rcppsimdjson/blob/master/inst/examples/simpleBenchmark.R
[simpleParseBenchmark]: https://github.com/eddelbuettel/rcppsimdjson/blob/master/inst/examples/simpleParseBenchmark.R
