## RcppSimdJson: Rcpp Bindings for the simdjson Header Library

[![CI](https://github.com/eddelbuettel/rcppsimdjson/workflows/ci/badge.svg)](https://github.com/eddelbuettel/rcppsimdjson/actions?query=workflow%3Aci)
[![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](https://www.gnu.org/licenses/gpl-2.0.html)
[![CRAN](https://www.r-pkg.org/badges/version/RcppSimdJson)](https://cran.r-project.org/package=RcppSimdJson)
[![Dependencies](https://tinyverse.netlify.com/badge/RcppSimdJson)](https://cran.r-project.org/package=RcppSimdJson)
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

A [simple file-oriented parsing benchmark](demo/simpleBenchmark.R) against the other R-accessible
JSON parsers:

```r
> print(res)
Unit: microseconds
     expr       min        lq       mean    median        uq       max neval   cld
 simdjson   279.651   311.028    347.192   332.916   370.018    721.52   100 a
  yyjsonr  1716.109  1829.722   1966.880  1911.338  2003.459   6231.36   100 ab
  jsonify  2791.985  2952.584   3110.254  3028.082  3237.061   4827.51   100  bc
 jsonlite  4364.009  4535.436   4727.582  4637.322  4799.579   7427.36   100   c
  RJSONIO  9102.826  9419.470   9990.286  9622.371  9911.310  20346.18   100    d
   ndjson 95197.993 96559.521 100487.450 97583.962 99954.224 165033.93   100     e
>
```

Or in chart form, also including the [second benchmark parsing strings](demo/simpleParseBenchmark.R):

![](https://eddelbuettel.github.io/rcppsimdjson/rcppsimdjson_both_benchmarks.png)

### Status

All three major OSs are supported, and JSON can be parsed from file and string under a variety of
settings. A C++17 compiler is required for ease of setup (though the upstream can fall back to older
compiler; one can edit [src/Makevars](https://github.com/eddelbuettel/rcppsimdjson/blob/master/src/Makevars)
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
