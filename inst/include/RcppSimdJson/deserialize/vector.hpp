#ifndef RCPPSIMDJSON__DESERIALIZE__VECTOR_HPP
#define RCPPSIMDJSON__DESERIALIZE__VECTOR_HPP

#include "scalar.hpp"

namespace rcppsimdjson {
namespace deserialize {


template <int RTYPE, typename in_T, rcpp_T R_Type, bool has_nulls>
inline auto build_vector_typed(const simdjson::dom::array array) -> Rcpp::Vector<RTYPE> {
  auto out = Rcpp::Vector<RTYPE>(std::size(array));
  R_xlen_t i = 0;
  for (auto element : array) {
    out[i++] = get_scalar<in_T, R_Type, has_nulls>(element);
  }
  return out;
}


template <bool has_nulls>
inline auto build_vector_integer64_typed(const simdjson::dom::array array)
    -> Rcpp::Vector<REALSXP> {

  auto stl_vec_int64 = std::vector<int64_t>(std::size(array));
  std::size_t i = 0;
  for (auto element : array) {
    stl_vec_int64[i++] = get_scalar<int64_t, rcpp_T::i64, has_nulls>(element);
  }

  return utils::as_integer64(stl_vec_int64);
}


template <utils::Int64_R_Type int64_opt>
inline auto dispatch_vector_typed(const simdjson::dom::array array,
                                  const rcpp_T R_Type,
                                  const bool has_nulls) -> SEXP {
  switch (R_Type) {
    case rcpp_T::chr:
      return has_nulls ? build_vector_typed<STRSXP, std::string, rcpp_T::chr, HAS_NULLS>(array)
                       : build_vector_typed<STRSXP, std::string, rcpp_T::chr, NO_NULLS>(array);

    case rcpp_T::dbl:
      return has_nulls ? build_vector_typed<REALSXP, double, rcpp_T::dbl, HAS_NULLS>(array)
                       : build_vector_typed<REALSXP, double, rcpp_T::dbl, NO_NULLS>(array);

    case rcpp_T::i32:
      return has_nulls ? build_vector_typed<INTSXP, int64_t, rcpp_T::i32, HAS_NULLS>(array)
                       : build_vector_typed<INTSXP, int64_t, rcpp_T::i32, NO_NULLS>(array);

    case rcpp_T::i64: {
      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Double) {
        return has_nulls ? build_vector_typed<REALSXP, int64_t, rcpp_T::dbl, HAS_NULLS>(array)
                         : build_vector_typed<REALSXP, int64_t, rcpp_T::dbl, NO_NULLS>(array);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::String) {
        return has_nulls ? build_vector_typed<STRSXP, int64_t, rcpp_T::chr, HAS_NULLS>(array)
                         : build_vector_typed<STRSXP, int64_t, rcpp_T::chr, NO_NULLS>(array);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Integer64) {
        return has_nulls ? build_vector_integer64_typed<HAS_NULLS>(array)
                         : build_vector_integer64_typed<NO_NULLS>(array);
      }
    }

    case rcpp_T::lgl:
      return has_nulls ? build_vector_typed<LGLSXP, bool, rcpp_T::lgl, HAS_NULLS>(array)
                       : build_vector_typed<LGLSXP, bool, rcpp_T::lgl, NO_NULLS>(array);

    case rcpp_T::null:
      return Rcpp::LogicalVector(std::size(array), NA_LOGICAL);

    case rcpp_T::u64:
      return has_nulls ? build_vector_typed<STRSXP, uint64_t, rcpp_T::chr, HAS_NULLS>(array)
                       : build_vector_typed<STRSXP, uint64_t, rcpp_T::chr, NO_NULLS>(array);

    default:
      return R_NilValue;
  }
}


template <int RTYPE>
inline auto build_vector_untyped(const simdjson::dom::array array) -> Rcpp::Vector<RTYPE> {
  Rcpp::Vector<RTYPE> out(std::size(array));
  R_xlen_t i = 0;
  for (auto element : array) {
    out[i++] = get_scalar_dispatch<RTYPE>(element);
  }
  return out;
}

inline auto build_vector_integer64_untyped(const simdjson::dom::array array)
    -> Rcpp::Vector<REALSXP> {

  std::vector<int64_t> stl_vec_int64(std::size(array));
  std::size_t i = 0;
  for (auto element : array) {
    stl_vec_int64[i++] = get_scalar<int64_t, rcpp_T::i64, HAS_NULLS>(element);
  }

  return rcppsimdjson::utils::as_integer64(stl_vec_int64);
}


template <rcppsimdjson::utils::Int64_R_Type int64_opt>
inline auto dispatch_vector_untyped(const simdjson::dom::array array, const rcpp_T common_R_type)
    -> SEXP {

  switch (common_R_type) {
    case rcpp_T::lgl:
      return build_vector_untyped<LGLSXP>(array);

    case rcpp_T::i32:
      return build_vector_untyped<INTSXP>(array);

    case rcpp_T::i64: {
      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Double) {
        return build_vector_untyped<REALSXP>(array);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::String) {
        return build_vector_untyped<STRSXP>(array);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Integer64) {
        return build_vector_integer64_untyped(array);
      }
    }

    case rcpp_T::dbl:
      return build_vector_untyped<REALSXP>(array);

    case rcpp_T::chr:
      return build_vector_untyped<STRSXP>(array);

    default:
      return R_NilValue;
  }
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
