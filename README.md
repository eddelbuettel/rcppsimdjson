## RcppSimdJSON: Rcpp Bindings for the simdjson Header Library

[![Build
Status](https://travis-ci.org/eddelbuettel/rcppsimdjson.svg)](https://travis-ci.org/eddelbuettel/simdjson)
[![License](https://eddelbuettel.github.io/badges/GPL2+.svg)](http://www.gnu.org/licenses/gpl-2.0.html)
[![CRAN](http://www.r-pkg.org/badges/version/rcppsimdjson)](https://cran.r-project.org/package=rcppsimdjson)
[![Dependencies](https://tinyverse.netlify.com/badge/rcppsimdjson)](https://cran.r-project.org/package=rcppsimdjson)

### Motivation

[simdjson](https://github.com/lemire/simdjson) by [Geoff
Langdale](https://twitter.com/geofflangdale) and [Daniel Lemire](https://lemire.me/en/) is an
engineering marvel.  Through very clever use of [SIMD instructions](https://en.wikipedia.org/wiki/SIMD),
it manages to parse JSON file faster than disc access. Wut? Yes you read that right: parallel
processing with so little overhead that the net throughput is limited only by disk speed.

Moreover, it is implemented in neat modern C++ and can be accessed as a header-only library. (Well, two
of them, really.)  Which makes R packaging easy and convenient and compelling. So here we are.

For further introduction, see the [arXiv paper](https://arxiv.org/abs/1902.08318) (out/to appear in VLDB
Journal 28(6) as well) and/or the video of the [recent talk by Daniel Lemire at
QCon](http://www.youtube.com/watch?v=wlvKAT7SZIQ) (voted best talk).

### Example

```r
jsonfile <- system.file("jsonexamples", "twitter.json", package="RcppSimdJson")
validateJSON(jsonfile)
```

### Status

Minimally viable. Right now it builds, and wraps the validation test.  So highly incomplete.  Expect
changes.  Feel free to contribute.

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
- [rapidjsonr](https://cran.r-project.org/package=rapidjsonr) by [David
  Cooley](https://twitter.com/_davecooley) brings [RapidJSON](https://rapidjson.org/) to R;
- [ndjson](https://cran.r-project.org/package=ndjson) by [Bob Rudis](https://rud.is/b/) brings the
  [JSON for Modern C++](https://github.com/nlohmann/json) by [Niels
  Lohmanm](https://github.com/nlohmann) to R;
- [RJSONIO](https://cran.r-project.org/package=RJSONIO) by [Duncan Temple
  Lang](http://www.stat.ucdavis.edu/~duncan/) started all this but could use a little love.

### Author

For the R package wrapper, [Dirk Eddelbuettel](http://github.com/eddelbuettel).

For everything pertaining to simdjson, [Geoff
Langdale](https://twitter.com/geofflangdale) and [Daniel Lemire](https://lemire.me/en/).
