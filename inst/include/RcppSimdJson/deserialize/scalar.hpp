#ifndef RCPPSIMDJSON__DESERIALIZE__SCALAR_HPP
#define RCPPSIMDJSON__DESERIALIZE__SCALAR_HPP

#include "Type_Doctor.hpp"

namespace rcppsimdjson {
namespace deserialize {


/*
 * Check for `null`s and return the appropriate `NA`s when found.
 */
static inline constexpr bool HAS_NULLS = true;
/*
 * No `null`s present, so skip checking for them.
 */
static inline constexpr bool NO_NULLS = false;


template <typename in_T, rcpp_T R_Type>
inline auto get_scalar_(simdjson::dom::element) noexcept(is_no_except(R_Type));

template <typename in_T, rcpp_T R_Type, bool has_null>
inline auto get_scalar(simdjson::dom::element element) noexcept(is_no_except(R_Type)) {
  if constexpr (has_null) {
    return element.is_null() ? na_val<R_Type>() : get_scalar_<in_T, R_Type>(element);
  } else {
    return get_scalar_<in_T, R_Type>(element);
  }
}


// bool ============================================================================================
// return Rcpp::String
template <>
inline auto
get_scalar_<bool, rcpp_T::chr>(simdjson::dom::element element) noexcept(is_no_except(rcpp_T::chr)) {
  return element.get<bool>().first ? Rcpp::String("TRUE") : Rcpp::String("FALSE");
}
// return double
template <>
inline auto
get_scalar_<bool, rcpp_T::dbl>(simdjson::dom::element element) noexcept(is_no_except(rcpp_T::dbl)) {
  return element.get<bool>().first ? 1.0 : 0.0;
}
// return int64_t
template <>
inline auto
get_scalar_<bool, rcpp_T::i64>(simdjson::dom::element element) noexcept(is_no_except(rcpp_T::i64)) {
  return element.get<bool>().first ? static_cast<int64_t>(1LL) : static_cast<int64_t>(0LL);
}
// return int
template <>
inline auto
get_scalar_<bool, rcpp_T::i32>(simdjson::dom::element element) noexcept(is_no_except(rcpp_T::i32)) {
  return element.get<bool>().first ? 1 : 0;
}
// return "bool"
template <>
inline auto
get_scalar_<bool, rcpp_T::lgl>(simdjson::dom::element element) noexcept(is_no_except(rcpp_T::lgl)) {
  return element.get<bool>().first ? 1 : 0;
}
// int64_t =========================================================================================
// return Rcpp::String
template <>
inline auto get_scalar_<int64_t, rcpp_T::chr>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::chr)) {

  return Rcpp::String(std::to_string(element.get<int64_t>().first));
}
// return double
template <>
inline auto get_scalar_<int64_t, rcpp_T::dbl>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::dbl)) {
  return element.get<double>().first;
}
// return int64_t
template <>
inline auto get_scalar_<int64_t, rcpp_T::i64>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::i64)) {

  return element.get<int64_t>().first;
}
// return int
template <>
inline auto get_scalar_<int64_t, rcpp_T::i32>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::i32)) {

  return static_cast<int>(element.get<int64_t>().first);
}
// double ==========================================================================================
// return Rcpp::String
template <>
inline auto get_scalar_<double, rcpp_T::chr>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::chr)) {

  auto out = std::to_string(element.get<double>().first);
  out.erase(out.find_last_not_of('0') + 2, std::string::npos);
  return Rcpp::String(out);
}
// return double
template <>
inline auto get_scalar_<double, rcpp_T::dbl>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::dbl)) {

  return element.get<double>().first;
}
// std::string (really std::string_view) ===========================================================
// return Rcpp::String
template <>
inline auto get_scalar_<std::string, rcpp_T::chr>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::chr)) {

  return Rcpp::String(element.get<const char*>().first);
}
// uint64_t ========================================================================================
// return Rcpp::String
template <>
inline auto get_scalar_<uint64_t, rcpp_T::chr>(simdjson::dom::element element) noexcept(
    is_no_except(rcpp_T::chr)) {

  return Rcpp::String(std::to_string(element.get<uint64_t>().first));
}
// dispatchers =====================================================================================
template <int RTYPE>
inline auto get_scalar_dispatch(simdjson::dom::element) noexcept(RTYPE != STRSXP &&
                                                                 RCPPSIMDJSON_NO_EXCEPTIONS);

template <>
inline auto get_scalar_dispatch<STRSXP>(simdjson::dom::element element) noexcept(false) {
  switch (element.type()) {
    case simdjson::dom::element_type::STRING:
      return get_scalar<std::string, rcpp_T::chr, NO_NULLS>(element);

    case simdjson::dom::element_type::DOUBLE:
      return get_scalar<double, rcpp_T::chr, NO_NULLS>(element);

    case simdjson::dom::element_type::INT64:
      return get_scalar<int64_t, rcpp_T::chr, NO_NULLS>(element);

    case simdjson::dom::element_type::BOOL:
      return get_scalar<bool, rcpp_T::chr, NO_NULLS>(element);

    case simdjson::dom::element_type::UINT64:
      return get_scalar<uint64_t, rcpp_T::chr, NO_NULLS>(element);

    default:
      return Rcpp::String(NA_STRING);
  }
}


template <>
inline auto
get_scalar_dispatch<REALSXP>(simdjson::dom::element element) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS) {
  switch (element.type()) {
    case simdjson::dom::element_type::DOUBLE:
      return get_scalar<double, rcpp_T::dbl, NO_NULLS>(element);

    case simdjson::dom::element_type::INT64:
      return get_scalar<int64_t, rcpp_T::dbl, NO_NULLS>(element);

    case simdjson::dom::element_type::BOOL:
      return get_scalar<bool, rcpp_T::dbl, NO_NULLS>(element);

    default:
      return NA_REAL;
  }
}


template <>
inline auto
get_scalar_dispatch<INTSXP>(simdjson::dom::element element) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS) {
  switch (element.type()) {
    case simdjson::dom::element_type::INT64:
      return get_scalar<int64_t, rcpp_T::i32, NO_NULLS>(element);

    case simdjson::dom::element_type::BOOL:
      return get_scalar<bool, rcpp_T::i32, HAS_NULLS>(element);

    default:
      return NA_INTEGER;
  }
}


template <>
inline auto
get_scalar_dispatch<LGLSXP>(simdjson::dom::element element) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS) {
  switch (element.type()) {
    case simdjson::dom::element_type::BOOL:
      return get_scalar<bool, rcpp_T::i32, NO_NULLS>(element);

    default:
      return NA_LOGICAL;
  }
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
