#ifndef RCPPSIMDJSON_COMMON_HPP
#define RCPPSIMDJSON_COMMON_HPP


#define STRICT_R_HEADERS
#include <Rcpp.h>


namespace rcppsimdjson {

/**
 * @brief A container's size as an @c R_xlen_t @c. Otherwise Equivalent to @c std::size() @c.
 */
template <typename _Container>
inline constexpr auto r_length(const _Container& __cont) noexcept -> R_xlen_t {
  return static_cast<R_xlen_t>(std::size(__cont));
}


/**
 * @brief A @c bit64::integer64 @c-compatible @c NA @c.
 */
static inline constexpr int64_t NA_INTEGER64 = LLONG_MIN;


/**
 * @brief Typing arguments that decide how a @c simdjson::dom::element is ultimately returned to R.
 */
enum class rcpp_T : int {
  array = 0,  /**< recursive: individual elements will decide ultimate R type */
  object = 1, /**< recursive: individual elements will decide ultimate R type */
  chr = 2,    /**< always becomes @c Rcpp::String / @c character */
  u64 = 3,    /**< always becomes @c Rcpp::String / @c character */
  dbl = 4,    /**< always becomes @c double */
  i64 = 5,    /**< follows @c Int64_R_Type: @c double, @c character, or @c bit64::integer64 */
  i32 = 6,    /**< always becomes @c int */
  lgl = 7,    /**< always becomes @c bool / @c logical */
  null = 8,   /**< becomes @c NA if returned in a vector, else @c NULL */
};


/**
 * @brief Get a typed @c NA @c.
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


/**
 * Internal flags tracking whether simdjson is compiled with exceptions enabled (the default).
 * If simdjson is compiled w/o exceptions ( @c #define SIMDJSON_EXCEPTIONS 0 @c), operations that
 * do not touch throwing code can be annotated with keyword @c noexcept where appropriate.
 */
// #define SIMDJSON_EXCEPTIONS 0 /* uncomment to disable compiling simdjson w/ exceptions */
#ifdef SIMDJSON_EXCEPTIONS
#define RCPPSIMDJSON_EXCEPTIONS SIMDJSON_EXCEPTIONS
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = SIMDJSON_EXCEPTIONS != 1;
#else
#define RCPPSIMDJSON_EXCEPTIONS 1
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = false;
#endif


/**
 * @brief Whether a function is @c noexcept.
 *
 * If a function does not touch throwing code it can be annotated with @c noexcept().
 * If @c RCPPSIMDJSON_NO_EXCEPTIONS is enabled and the @c rcpp_T template argument is not
 * @c rcpp_T::chr, functions annotated with @c noexcept(is_no_except(rcpp_T)) will be @c noexcept
 * when compiled.
 *
 * Currently, @c rccp_T::chr touches throwing code so functions using it will always be
 * @c noexcept(false).
 *
 * Many examples in @file{inst/include/RcppSimdJson/deserialize/scalar.hpp}.
 */
static inline constexpr auto is_no_except(rcpp_T R_Type) // NOLINT(clang-diagnostic-unused-function)
    -> bool {
  return RCPPSIMDJSON_NO_EXCEPTIONS && R_Type != rcpp_T::chr;
}


namespace deserialize {


/**
 * @brief Determines level of type strictness in combining array elements into R vectors.
 *
 * When arrays are not homogeneous and @c Type_Policy::anything_goes is used, type promotion follows
 * R's behavior.
 */
enum class Type_Policy : int {
  anything_goes = 0, /* Non-recursive arrays always become vectors of the highest present type */
  ints_as_dbls = 1,  /* Non-recursive arrays of only numbers are promoted to highest type */
  strict = 2,        /* No type promotion */
};


/**
 * @brief Maximum simplification level.
 */
enum class Simplify_To : int {
  data_frame = 0, /* If possible, return dataframes. Otherwise return matrices/vectors/lists. */
  matrix = 1,     /* If possible, return matrices. Otherwise return vectors/lists. */
  vector = 2,     /* If possible, return vectors. Otherwise return lists. */
  list = 3,       /* No simplification. */
};


} // namespace deserialize
} // namespace rcppsimdjson


#include "../simdjson.h"
#include "utils.hpp"


namespace rcppsimdjson {
namespace deserialize {


/**
 * @brief Simplify a @c simdjson::dom::element to an R object.
 *
 * @note Forward declaration. See @file inst/include/RcppSimdJson/deserialize/simplify.hpp @file.
 */
template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_element(simdjson::dom::element element, SEXP empty_array, SEXP empty_object)
    -> SEXP;


} // namespace deserialize
} // namespace rcppsimdjson


#endif