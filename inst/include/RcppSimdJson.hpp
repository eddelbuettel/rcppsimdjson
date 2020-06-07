#ifndef RCPPSIMDJSON_HPP
#define RCPPSIMDJSON_HPP

#define STRICT_R_HEADERS
#include <Rcpp.h>


namespace rcppsimdjson {

// #define SIMDJSON_EXCEPTIONS 0
#ifdef SIMDJSON_EXCEPTIONS
static inline constexpr auto SIMDJSON_NOEXCEPT = SIMDJSON_EXCEPTIONS == 0;
#else
static inline constexpr auto SIMDJSON_NOEXCEPT = false;
#endif

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

static inline constexpr auto is_no_except(rcpp_T R_Type) -> bool {
  /* it looks like all scalars can be generically extracted w/o touching Rcpp API except
   * for strings
   */
  return SIMDJSON_NOEXCEPT && R_Type != rcpp_T::chr;
};

static inline constexpr int64_t NA_INTEGER64 = LLONG_MIN;

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

} // namespace rcppsimdjson

#include <simdjson.h>

#include "RcppSimdJson/utils.hpp"
#include "RcppSimdJson/deserialize.hpp"

#endif
