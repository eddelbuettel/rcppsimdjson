#ifndef RCPPSIMDJSON_HPP
#define RCPPSIMDJSON_HPP

#define STRICT_R_HEADERS
#include <Rcpp.h>


namespace rcppsimdjson {

static inline constexpr int64_t NA_INTEGER64 = LLONG_MIN;


enum class rcpp_T : int {
  array = 0,
  object = 1,
  chr = 2,
  u64 = 3,
  dbl = 4,
  i64 = 5,
  i32 = 6,
  lgl = 7,
  null = 8,
};


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


// #define SIMDJSON_EXCEPTIONS 0
#ifdef SIMDJSON_EXCEPTIONS
#define RCPPSIMDJSON_EXCEPTIONS SIMDJSON_EXCEPTIONS
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = SIMDJSON_EXCEPTIONS != 1;
#else
#define RCPPSIMDJSON_EXCEPTIONS 1
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = false;
#endif


static inline constexpr auto is_no_except(rcpp_T R_Type) -> bool {
  // all scalars seem to be extractable w/o touching throwing code except for strings
  return RCPPSIMDJSON_NO_EXCEPTIONS && R_Type != rcpp_T::chr;
}


} // namespace rcppsimdjson

#include <simdjson.h>

#include "RcppSimdJson/utils.hpp"
#include "RcppSimdJson/deserialize.hpp"

#endif
