#ifndef RCPPSIMDJSON_MATRIX_HPP
#define RCPPSIMDJSON_MATRIX_HPP

#include "vector.hpp"

namespace rcppsimdjson {
namespace deserialize {


/**
 * Whether or not an array-of-arrays can be mapped to an R matrix along with all
 * the pieces of information necessary to do so downstream.
 */
struct Matrix_Diagnosis {
  bool is_matrix_ish;  /**  whether an array can be mapped to an R matrix */
  bool has_nulls;      /** whether `nulls`s are present */
  bool is_homogeneous; /** whether `dom::element_type`s all the same, besides `NULL_VALUE`*/
  simdjson::dom::element_type common_element_type; /** common `element_type when `is_homogeneous` */
  rcpp_T common_R_type; /** common R type to which non-homogeneous values are casted */
  std::size_t n_cols;   /**  shared length of sub arrays */
};


template <Type_Policy type_policy>
inline auto diagnose_matrix(simdjson::dom::array array) noexcept(SIMDJSON_NOEXCEPT)
    -> Matrix_Diagnosis {
  auto mat_has_nulls = false;
  std::set<rcpp_T> mat_R_types;
  std::set<std::size_t> n_cols;

  for (auto element : array) {
    // matrices are made from arrays-of-arrays, so return early if any elements
    // are not arrays
    if (element.type() != simdjson::dom::element_type::ARRAY) {
      return Matrix_Diagnosis{false,                                   // early return:
                              HAS_NULLS,                               // throw...
                              false,                                   // all...
                              simdjson::dom::element_type::NULL_VALUE, // of...
                              rcpp_T::null,                            // this...
                              0UL};                                    // away
    }

    auto type_doc = Type_Doctor<type_policy>();
    if constexpr (SIMDJSON_EXCEPTIONS) {
      type_doc = Type_Doctor<type_policy>(element);
    } else {
      type_doc = Type_Doctor<type_policy>(element.get<simdjson::dom::array>().first);
    }

    const auto vec_common_element_type = type_doc.common_element_type();

    // if common elements aren't scalars, return early
    if (vec_common_element_type == simdjson::dom::element_type::ARRAY ||
        vec_common_element_type == simdjson::dom::element_type::OBJECT) {
      return Matrix_Diagnosis{false,                   // early return:
                              HAS_NULLS,               // throw...
                              false,                   // all...
                              vec_common_element_type, // of...
                              rcpp_T::null,            // this...
                              0UL};                    // away
    }

    // `mat_has_nulls` will be true if `vec_has_nulls` is ever true
    mat_has_nulls = mat_has_nulls || type_doc.has_null();
    // track R types
    mat_R_types.insert(type_doc.common_R_type());

    if constexpr (SIMDJSON_EXCEPTIONS) {
      n_cols.insert(std::size(simdjson::dom::array(element)));
    } else {
      n_cols.insert(std::size(element.get<simdjson::dom::array>().first));
    }
  }

  const auto mat_type_doc = Type_Doctor<type_policy>::from_set(mat_R_types);

  return Matrix_Diagnosis{
      mat_type_doc.is_vectorizable() && std::size(n_cols) == 1,     // only 1 rcpp_T and 1 n_cols
      mat_has_nulls,                                                //
      mat_type_doc.is_homogeneous() && std::size(mat_R_types) == 1, //
      mat_type_doc.common_element_type(),                           //
      mat_type_doc.common_R_type(),                                 //
      *std::begin(n_cols)                                           // only 1 if `is_matrix_ish`
  };
}


template <int RTYPE, typename in_T, rcpp_T R_Type, bool has_nulls>
inline auto build_matrix_typed(simdjson::dom::array array, const std::size_t n_cols)
    -> Rcpp::Vector<RTYPE> {

  const auto n_rows = std::size(array);
  Rcpp::Matrix<RTYPE> out(n_rows, n_cols);
  R_xlen_t j = 0;

  if constexpr (SIMDJSON_EXCEPTIONS) {
    for (simdjson::dom::array sub_array : array) {
      R_xlen_t i = 0;
      for (auto element : sub_array) {
        out[i + j] = get_scalar<in_T, R_Type, has_nulls>(element);
        i += n_rows;
      }
      j++;
    }

  } else {

    for (auto sub_array : array) {
      R_xlen_t i = 0;
      for (auto element : sub_array.get<simdjson::dom::array>().first) {
        out[i + j] = get_scalar<in_T, R_Type, has_nulls>(element);
        i += n_rows;
      }
      j++;
    }
  }

  return out;
}

template <bool has_nulls>
inline auto build_matrix_integer64_typed(simdjson::dom::array array, const std::size_t n_cols)
    -> Rcpp::Vector<REALSXP> {

  const auto n_rows = std::size(array);
  std::vector<int64_t> stl_vec_int64(n_rows * n_cols);
  std::size_t j = 0;

  if constexpr (SIMDJSON_EXCEPTIONS) {
    for (simdjson::dom::array sub_array : array) {
      std::size_t i = 0;
      for (auto element : sub_array) {
        stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, has_nulls>(element);
        i += n_rows;
      }
      j++;
    }

  } else {

    for (auto sub_array : array) {
      std::size_t i = 0;
      for (auto element : sub_array.get<simdjson::dom::array>().first) {
        stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, has_nulls>(element);
        i += n_rows;
      }
      j++;
    }
  }

  Rcpp::NumericVector out = rcppsimdjson::utils::as_integer64(stl_vec_int64);
  out.attr("dim") = Rcpp::IntegerVector::create(n_rows, n_cols);

  return out;
}


template <rcppsimdjson::utils::Int64_R_Type int64_opt>
inline auto dispatch_matrix_typed(simdjson::dom::array array,
                                  simdjson::dom::element_type element_type,
                                  const bool has_nulls,
                                  const std::size_t n_cols) -> SEXP {

  switch (element_type) {
    case simdjson::dom::element_type::STRING:
      return has_nulls
                 ? build_matrix_typed<STRSXP, std::string, rcpp_T::chr, HAS_NULLS>(array, n_cols)
                 : build_matrix_typed<STRSXP, std::string, rcpp_T::chr, NO_NULLS>(array, n_cols);

    case simdjson::dom::element_type::DOUBLE:
      return has_nulls ? build_matrix_typed<REALSXP, double, rcpp_T::dbl, HAS_NULLS>(array, n_cols)
                       : build_matrix_typed<REALSXP, double, rcpp_T::dbl, NO_NULLS>(array, n_cols);

    case simdjson::dom::element_type::INT64: {
      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Double) {
        return has_nulls
                   ? build_matrix_typed<REALSXP, int64_t, rcpp_T::dbl, HAS_NULLS>(array, n_cols)
                   : build_matrix_typed<REALSXP, int64_t, rcpp_T::dbl, NO_NULLS>(array, n_cols);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::String) {
        return has_nulls
                   ? build_matrix_typed<STRSXP, int64_t, rcpp_T::chr, HAS_NULLS>(array, n_cols)
                   : build_matrix_typed<STRSXP, int64_t, rcpp_T::chr, NO_NULLS>(array, n_cols);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Integer64) {
        return has_nulls ? build_matrix_integer64_typed<HAS_NULLS>(array, n_cols)
                         : build_matrix_integer64_typed<NO_NULLS>(array, n_cols);
      }
    }

    case simdjson::dom::element_type::BOOL:
      return has_nulls ? build_matrix_typed<LGLSXP, bool, rcpp_T::lgl, HAS_NULLS>(array, n_cols)
                       : build_matrix_typed<LGLSXP, bool, rcpp_T::lgl, NO_NULLS>(array, n_cols);

    case simdjson::dom::element_type::NULL_VALUE:
      return Rcpp::LogicalVector(std::size(array), NA_LOGICAL);

    case simdjson::dom::element_type::UINT64:
      return has_nulls ? build_matrix_typed<STRSXP, uint64_t, rcpp_T::chr, HAS_NULLS>(array, n_cols)
                       : build_matrix_typed<STRSXP, uint64_t, rcpp_T::chr, NO_NULLS>(array, n_cols);

    default:
      return R_NilValue;
  }
}

template <int RTYPE>
inline auto build_matrix_untyped(simdjson::dom::array array, const std::size_t n_cols) -> SEXP {
  const auto n_rows = std::size(array);
  Rcpp::Matrix<RTYPE> out(n_rows, n_cols);

  R_xlen_t j = 0;

  if constexpr (SIMDJSON_EXCEPTIONS) {
    for (simdjson::dom::array sub_array : array) {
      std::size_t i = 0;
      for (auto element : sub_array) {
        out[i + j] = get_scalar_dispatch<RTYPE>(element);
        i += n_rows;
      }
      j++;
    }

  } else {

    for (auto sub_array : array) {
      R_xlen_t i = 0;
      for (auto element : sub_array.get<simdjson::dom::array>().first) {
        out[i + j] = get_scalar_dispatch<RTYPE>(element);
        i += n_rows;
      }
      j++;
    }
  }

  return out;
}


inline auto build_matrix_integer64_untyped(simdjson::dom::array array, std::size_t n_cols)
    -> Rcpp::Vector<REALSXP> {

  const auto n_rows = std::size(array);
  std::vector<int64_t> stl_vec_int64(n_rows * n_cols);
  std::size_t j = 0;

  if constexpr (SIMDJSON_EXCEPTIONS) {
    for (simdjson::dom::array sub_array : array) {
      std::size_t i = 0;
      for (auto element : sub_array) {
        stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, HAS_NULLS>(element);
        i += n_rows;
      }
      j++;
    }

  } else {

    for (auto element : array) {
      std::size_t i = 0;
      for (auto sub_element : element.get<simdjson::dom::array>().first) {
        stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, HAS_NULLS>(sub_element);
        i += n_rows;
      }
      j++;
    }
  }

  Rcpp::Vector<REALSXP> out = rcppsimdjson::utils::as_integer64(stl_vec_int64);
  out.attr("dim") = Rcpp::IntegerVector::create(n_rows, n_cols);

  return out;
}


template <rcppsimdjson::utils::Int64_R_Type int64_opt>
inline auto dispatch_matrix_untyped(simdjson::dom::array array,
                                    const rcpp_T common_type,
                                    const std::size_t n_cols) -> SEXP {
  switch (common_type) {
    case rcpp_T::lgl:
      return build_matrix_untyped<LGLSXP>(array, n_cols);

    case rcpp_T::i32:
      return build_matrix_untyped<INTSXP>(array, n_cols);

    case rcpp_T::i64: {
      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Double) {
        return build_matrix_untyped<REALSXP>(array, n_cols);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::String) {
        return build_matrix_untyped<STRSXP>(array, n_cols);
      }

      if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Integer64) {
        return build_matrix_integer64_untyped(array, n_cols);
      }
    }

    case rcpp_T::dbl:
      return build_matrix_untyped<REALSXP>(array, n_cols);

    case rcpp_T::chr:
      return build_matrix_untyped<STRSXP>(array, n_cols);

    case rcpp_T::null: {
      auto out = Rcpp::LogicalMatrix(std::size(array), n_cols);
      out.fill(NA_LOGICAL);
      return out;
    }

    case rcpp_T::u64:
      return build_matrix_untyped<STRSXP>(array, n_cols);

    default:
      return R_NilValue;
  }
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
