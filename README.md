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

A [simple parsing benchmark](demo/simpleBenchmark.R) against four other R-accessible JSON parsers:

```r
R> res
Unit: milliseconds
     expr      min       lq     mean   median       uq       max neval  cld
 simdjson  1.87118  2.03252  2.24351  2.17228  2.27756   6.57145   100 a
  jsonify  8.91694  9.20124  9.58652  9.46077  9.73692  13.41707   100  b
  RJSONIO 10.49187 11.09410 11.69109 11.42555 11.95780  17.93653   100  b
   ndjson 27.04830 28.62251 31.44330 29.51343 32.05847 146.88221   100   c
 jsonlite 34.93334 36.54784 38.67843 37.74890 40.19555  46.32444   100    d
R>
```

Or in chart form:

![](https://eddelbuettel.github.io/rcppsimdjson/rcppsimdjson_parse_benchmark.png)

### Status

All three major OSs are supported, and JSON can be parsed from file and string under a variety of
settings. A C++17 compiler is required for ease of setup (though the upstream can fall back to older
compiler; one edit [src/Makevars](https://github.com/eddelbuettel/rcppsimdjson/blob/master/src/Makevars)
accordingly if need be.

### Contributing

Any problems, bug reports, or features requests for the package can be submitted and handled most
conveniently as [Github issues](https://github.com/eddelbuettel/rcppsimdjson/issues) in the repository.

Before submitting pull requests, it is frequently preferable to first discuss need and scope in such
an issue ticket.  See the file
[Contributing.md](https://github.com/RcppCore/Rcpp/blob/master/Contributing.md) (in the
[Rcpp](https://github.com/RcppCore/Rcpp) repo) for a brief discussion.


### See Also

For standard JSON work on R, as well as for other nicely done C++ libraries, consider these:

- [jsonlite](https://cran.r-project.org/package=jsonlite) by [Jeroen
  Ooms](https://github.com/jeroen) is excellent, very versatile, and probably most-widely used;
- [rapidjsonr](https://cran.r-project.org/package=rapidjsonr) and [jsonify](https://cran.r-project.org/package=jsonify) by [David
  Cooley](https://github.com/dcooley) bringing [RapidJSON](https://rapidjson.org/) to R;
- [ndjson](https://cran.r-project.org/package=ndjson) by [Bob Rudis](https://rud.is/b/) builds on the
  [JSON for Modern C++](https://github.com/nlohmann/json) library by [Niels
  Lohmann](https://github.com/nlohmann);
- [RJSONIO](https://cran.r-project.org/package=RJSONIO) by [Duncan Temple
  Lang](https://www.stat.ucdavis.edu/~duncan/) started all this but could use a little love.

### Author

For the R package, [Dirk Eddelbuettel](https://github.com/eddelbuettel) and [Brendan
Knapp](https://github.com/knapply).

For everything pertaining to simdjson, [Daniel Lemire](https://lemire.me/en/) (and [many
contributors](https://github.com/simdjson/simdjson/graphs/contributors)).
