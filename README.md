## RcppSimdJSON: Rcpp Bindings for the simdjson Header Library

[![Build Status](https://travis-ci.org/eddelbuettel/rcppsimdjson.svg)](https://travis-ci.org/eddelbuettel/rcppsimdjson)
[![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](http://www.gnu.org/licenses/gpl-2.0.html)
[![CRAN](http://www.r-pkg.org/badges/version/RcppSimdJson)](https://cran.r-project.org/package=RcppSimdJson)
[![Dependencies](https://tinyverse.netlify.com/badge/RcppSimdJson)](https://cran.r-project.org/package=RcppSimdJson)
[![Last Commit](https://img.shields.io/github/last-commit/eddelbuettel/rcppsimdjson)](https://github.com/eddelbuettel/rcppsimdjson)

### Motivation

[simdjson](https://github.com/lemire/simdjson) by [Daniel Lemire](https://lemire.me/en/) (with contributions by [Geoff
Langdale](https://twitter.com/geofflangdale),  [John Keiser](https://github.com/jkeiser) and  [many others](https://github.com/lemire/simdjson/graphs/contributors)) is an
engineering marvel.  Through very clever use of [SIMD instructions](https://en.wikipedia.org/wiki/SIMD),
it manages to parse JSON files faster than disc access. Wut? Yes you read that right: parallel
processing with so little overhead that the net throughput is limited only by disk speed.

Moreover, it is implemented in neat modern C++ and can be accessed as a header-only library. (Well,
one library in two files, really.)  Which makes R packaging easy and convenient and compelling. So
here we are.

For further introduction, see the [arXiv paper](https://arxiv.org/abs/1902.08318) by Langdale and Lemire (out/to appear in VLDB
Journal 28(6) as well) and/or the video of the [recent talk by Daniel Lemire at
QCon](http://www.youtube.com/watch?v=wlvKAT7SZIQ) (voted best talk).

### Example

```r
jsonfile <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
validateJSON(jsonfile)
```

### Comparison

A [simple benchmark](demo/simpleBenchmark.R) against four other R-accessible JSON parsers:

```r
R> print(res, order="median")
Unit: microseconds
     expr       min         lq       mean    median         uq        max neval   cld
 simdjson   279.246    332.577    390.815    362.11    427.638    648.652   100 a    
  jsonify  2820.079   2930.945   3064.773   3027.28   3153.427   3986.948   100  b   
 jsonlite  8899.379   9085.685   9273.974   9226.56   9349.513  10820.562   100   c  
  RJSONIO  9685.246   9899.634  10185.272  10105.96  10296.579  11766.177   100    d
   ndjson 99460.979 100381.388 101758.682 100971.75 102613.041 111553.986   100     e
R> print(res, order="median", unit="relative")
Unit: relative
     expr      min        lq      mean    median        uq       max neval   cld
 simdjson   1.0000   1.00000   1.00000   1.00000   1.00000   1.00000   100 a    
  jsonify  10.0989   8.81284   7.84201   8.36011   7.37406   6.14651   100  b   
 jsonlite  31.8693  27.31908  23.72986  25.48003  21.86315  16.68161   100   c  
  RJSONIO  34.6836  29.76649  26.06165  27.90857  24.07779  18.13943   100    d
   ndjson 356.1769 301.82947 260.37585 278.84314 239.95305 171.97817   100     e
R>
```

Or in chart form:

![](https://eddelbuettel.github.io/rcppsimdjson/rcppsimdjson_benchmark.png)

Note that these timings came from the very beginnings of the package.
Admittance to CRAN meant turning off one particular optimisation ('computed
GOTOs') by default resulting in slightly slower performance. You can get the
behaviour back locally by removing the `-DSIMDJSON_NO_COMPUTED_GOTO` term from
[src/Makevars.in](https://github.com/eddelbuettel/rcppsimdjson/blob/master/src/Makevars.in#L5).

### Status

Minimally viable. Right now it builds, wraps the validation test, and checks
cleanly as an R package. As of version 0.0.4, basic parsing is supported, see
`parseExample()`.  Requires a C++17 compiler. Expect changes. But please feel
free to contribute.

### Contributing

Any problems, bug reports, or features requests for the package can be submitted and handled most
conveniently as [Github issues](https://github.com/eddelbuettel/anytime/issues) in the repository.

Before submitting pull requests, it is frequently preferable to first discuss need and scope in such
an issue ticket.  See the file
[Contributing.md](https://github.com/RcppCore/Rcpp/blob/master/Contributing.md) (in the
[Rcpp](https://github.com/RcppCore/Rcpp) repo) for a brief discussion.


### See Also

For standard JSON work on R, as well as for other nicely done C++ libraries, consider these:

- [jsonlite](https://cran.r-project.org/package=jsonlite) by [Jeroen
  Ooms](https://github.com/jeroen) is excellent, very versatile, and probably most-widely used;
- [rapidjsonr](https://cran.r-project.org/package=rapidjsonr) and [jsonify](https://cran.r-project.org/package=jsonify) by [David
  Cooley](https://twitter.com/_davecooley) bringing [RapidJSON](https://rapidjson.org/) to R;
- [ndjson](https://cran.r-project.org/package=ndjson) by [Bob Rudis](https://rud.is/b/) builds on the
  [JSON for Modern C++](https://github.com/nlohmann/json) library by [Niels
  Lohmann](https://github.com/nlohmann);
- [RJSONIO](https://cran.r-project.org/package=RJSONIO) by [Duncan Temple
  Lang](http://www.stat.ucdavis.edu/~duncan/) started all this but could use a little love.

### Author

For the R package wrapper, [Dirk Eddelbuettel](http://github.com/eddelbuettel).

For everything pertaining to simdjson, [Daniel Lemire](https://lemire.me/en/) (and [many  contributors](https://github.com/lemire/simdjson/graphs/contributors)) .
