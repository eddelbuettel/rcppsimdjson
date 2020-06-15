#ifndef RCPPSIMDJSON_COMMON_HPP
#define RCPPSIMDJSON_COMMON_HPP


#define STRICT_R_HEADERS
#include <Rcpp.h>


namespace rcppsimdjson {

/*
 * `bit64::integer64`-compatible `NA`
 */
static inline constexpr int64_t NA_INTEGER64 = LLONG_MIN;


/*
 * Typing arguments that decide how `simdjson::dom::element`s are ultimate return to R.
 */
enum class rcpp_T : int {
  array = 0,  /* recursive: individual elements will decide ultimate R type */
  object = 1, /* recursive: individual elements will decide ultimate R type */
  chr = 2,    /* always becomes `Rcpp::String`/`character(1L)` */
  u64 = 3,    /* always becomes `Rcpp::String`/`character(1L)` */
  dbl = 4,    /* always becomes `double` */
  i64 = 5,    /* follows Int64_R_Type: `double`, `character(1L)`, or `bit64::integer64` */
  i32 = 6,    /* always becomes `int` */
  lgl = 7,    /* always becomes `bool */
  null = 8,   /* becomes `NA` if returned in a vector, else `NULL */
};


/*
 * Generic, typed `NA` inserter.
 */
template <rcpp_T R_Type> static inline constexpr auto na_val() {
  if constexpr (R_Type == rcpp_T::chr) {
    return NA_STRING;
  }
  if constexpr (R_Type == rcpp_T::dbl) {
    return NA_REAL;
  }
  if constexpr (R_Type == rcpp_T::i64) {
    return NA_INTEGER64;
  }
  if constexpr (R_Type == rcpp_T::i32) {
    return NA_INTEGER;
  }
  if constexpr (R_Type == rcpp_T::lgl) {
    return NA_LOGICAL;
  }
}


/*
 * Internal flags tracking whether simdjson is compiled with exceptions enabled (the default).
 * If simdjson is compiled w/o exceptions (`#define SIMDJSON_EXCEPTIONS 0`), operations that do not
 * touch throwing code can be annotated with keyword `noexcept` where appropriate.
 * See inst/include/RcppSimdJson/deserialize/scalar.hpp for examples.
 */
// #define SIMDJSON_EXCEPTIONS 0 /* uncomment to disable compiling simdjson w/ exceptions */
#ifdef SIMDJSON_EXCEPTIONS
#define RCPPSIMDJSON_EXCEPTIONS SIMDJSON_EXCEPTIONS
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = SIMDJSON_EXCEPTIONS != 1;
#else
#define RCPPSIMDJSON_EXCEPTIONS 1 // NOLINT(cppcoreguidelines-macro-usage)
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = false;
#endif


/*
 * All scalar-getter functions are annotated with `is_no_except()`, which will be false if
 * `RCPPSIMDJSON_NO_EXCEPTIONS` is enabled and their `rcpp_T` template argument is not
 * `rcpp_T::chr` (strings are not currently extractable w/o touching throwing code).
 */
static inline constexpr auto is_no_except(rcpp_T R_Type) noexcept -> bool {
  return RCPPSIMDJSON_NO_EXCEPTIONS && R_Type != rcpp_T::chr;
}


namespace deserialize {


static inline constexpr bool HAS_NULLS = true;
static inline constexpr bool NO_NULLS = false;

/*
 * Determines level of type strictness in combining array elements into R vectors.
 *
 * When arrays are not homogeneous and `Type_Policy::anything_goes` is used, type promotion follows
 * R's behavior.
 */
enum class Type_Policy : int {
  anything_goes = 0, /* Non-recursive arrays always become vectors */
  ints_as_dbls = 1,  /* Combines `rcpp_T::i32`s, `::i64`s, and `::dbl`s */
  strict = 2,        /* No type promotions */
};


/*
 * Maximum simplification level. `Simplify_To::list` results in no simplification.
 */
enum class Simplify_To : int {
  data_frame = 0,
  matrix = 1,
  vector = 2,
  list = 3,
};


} // namespace deserialize


} // namespace rcppsimdjson


#include <simdjson.h>
#include "RcppSimdJson/utils.hpp"


#endif