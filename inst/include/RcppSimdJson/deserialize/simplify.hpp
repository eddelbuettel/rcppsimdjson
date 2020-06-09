#ifndef RCPPSIMDJSON__DESERIALIZE__SIMPLIFY_HPP
#define RCPPSIMDJSON__DESERIALIZE__SIMPLIFY_HPP


namespace rcppsimdjson {
namespace deserialize {


static inline constexpr bool HAS_NULLS = true;
static inline constexpr bool NO_NULLS = false;

} // namespace deserialize
} // namespace rcppsimdjson


#include "Type_Doctor.hpp"
#include "scalar.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "dataframe.hpp"

namespace rcppsimdjson {
namespace deserialize {


enum class Simplify_To : int {
  data_frame = 0,
  matrix = 1,
  vector = 2,
  list = 3,
};


template <Type_Policy type_policy,
          utils::Int64_R_Type int64_opt,
          Simplify_To simplify_to>
inline SEXP simplify_element(const simdjson::dom::element); // forward declaration


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_data_frame(const simdjson::dom::array array,
                                const std::map<std::string_view, Type_Doctor<type_policy>>& cols)
    -> SEXP {

  const auto n_rows = R_xlen_t(std::size(array));
  Rcpp::List out(std::size(cols));
  auto j_col = R_xlen_t(0);

  for (auto [key, value] : cols) {
    switch (value.common_R_type()) {
      case rcpp_T::chr: {
        out[j_col++] = build_col<STRSXP, std::string, rcpp_T::chr, type_policy>(array, key, value);
        break;
      }

      case rcpp_T::dbl: {
        out[j_col++] = build_col<REALSXP, double, rcpp_T::dbl, type_policy>(array, key, value);
        break;
      }

      case rcpp_T::i64: {
        out[j_col++] = build_col_integer64<type_policy, int64_opt>(array, key, value);
        break;
      }

      case rcpp_T::i32: {
        out[j_col++] = build_col<INTSXP, int64_t, rcpp_T::i32, type_policy>(array, key, value);
        break;
      }

      case rcpp_T::lgl: {
        out[j_col++] = build_col<LGLSXP, bool, rcpp_T::lgl, type_policy>(array, key, value);
        break;
      }

      case rcpp_T::null: {
        out[j_col++] = Rcpp::LogicalVector(n_rows, NA_LOGICAL);
        break;
      }

      default: {
        auto this_col = Rcpp::Vector<VECSXP>(n_rows);
        R_xlen_t i_row = 0;
        for (auto element : array) {
          auto [value, error] = element.get<simdjson::dom::object>().at_key(key);
          if (error) {
            this_col[i_row++] = NA_LOGICAL;
          } else {
            this_col[i_row++] = simplify_element<type_policy, int64_opt, simplify_to>(value);
          }
        }
        out[j_col++] = this_col;
      }
    }
  }

  out.attr("names") = Rcpp::CharacterVector( //
      std::begin(cols),
      std::end(cols),
      [](auto key_value) { return Rcpp::String(std::string(key_value.first)); } //
  );
  out.attr("row.names") = Rcpp::seq(1, n_rows);
  out.attr("class") = "data.frame";

  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_list(const simdjson::dom::array array) -> SEXP {
  Rcpp::List out(std::size(array));

  auto i = R_xlen_t(0);
  for (auto element : array) {
    out[i++] = simplify_element<type_policy, int64_opt, simplify_to>(element);
  }

  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_vector(const simdjson::dom::array& array) -> SEXP {
  const auto type_doctor = Type_Doctor<type_policy>(array);

  if (type_doctor.is_vectorizable()) {
    return type_doctor.is_homogeneous()
               ? dispatch_vector_typed<int64_opt>(
                     array, type_doctor.common_R_type(), type_doctor.has_null())
               : dispatch_vector_untyped<int64_opt>(array, type_doctor.common_R_type());
  }

  return simplify_list<type_policy, int64_opt, simplify_to>(array);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_matrix(const simdjson::dom::array array) -> SEXP {
  auto [is_matrix_ish,
        mat_has_nulls,
        mat_is_homogeneous,
        mat_common_element_type,
        mat_common_R_type,
        n_cols] = diagnose_matrix<type_policy>(array);
  // Rcpp::Rcout << "mat_is_homogeneous " << mat_is_homogeneous << std::endl;

  if (is_matrix_ish) {
    return mat_is_homogeneous
               ? dispatch_matrix_typed<int64_opt>(
                     array, mat_common_element_type, mat_common_R_type, mat_has_nulls, n_cols)
               : dispatch_matrix_untyped<int64_opt>(array, mat_common_R_type, n_cols);
  }

  return simplify_vector<type_policy, int64_opt, simplify_to>(array);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_data_frame(const simdjson::dom::array& array) -> SEXP {
  auto [is_data_frame_ish, cols] = diagnose_data_frame<type_policy>(array);

  if (is_data_frame_ish) {
    return simplify_data_frame<type_policy, int64_opt, simplify_to>(array, cols);
  }

  return simplify_matrix<type_policy, int64_opt, simplify_to>(array);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto dispatch_simplify_array(const simdjson::dom::array& array) -> SEXP {
  if (std::size(array) == 0) {
    return Rcpp::LogicalVector(0);
  }

  if constexpr (simplify_to == Simplify_To::data_frame) {
    return simplify_data_frame<type_policy, int64_opt, Simplify_To::data_frame>(array);
  }

  if constexpr (simplify_to == Simplify_To::matrix) {
    return simplify_matrix<type_policy, int64_opt, Simplify_To::matrix>(array);
  }

  if constexpr (simplify_to == Simplify_To::vector) {
    return simplify_vector<type_policy, int64_opt, Simplify_To::vector>(array);
  }

  if constexpr (simplify_to == Simplify_To::list) {
    return simplify_list<type_policy, int64_opt, Simplify_To::list>(array);
  }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP simplify_object(const simdjson::dom::object object) {
  const auto n = R_xlen_t(std::size(object));

  Rcpp::List out(n);
  Rcpp::CharacterVector out_names(n);

  auto i = R_xlen_t(0);
  for (auto [key, value] : object) {
    out[i] = simplify_element<type_policy, int64_opt, simplify_to>(value);
    out_names[i++] = std::string(key);
  }

  out.attr("names") = out_names;
  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_element(const simdjson::dom::element element) -> SEXP {

  switch (element.type()) {
#if RCPPSIMDJSON_EXCEPTIONS
    case simdjson::dom::element_type::ARRAY:
      return dispatch_simplify_array<type_policy, int64_opt, simplify_to>(element);

    case simdjson::dom::element_type::OBJECT:
      return simplify_object<type_policy, int64_opt, simplify_to>(element);

    case simdjson::dom::element_type::DOUBLE:
      return Rcpp::wrap<double>(element);

    case simdjson::dom::element_type::INT64:
      return utils::resolve_int64<int64_opt>(int64_t(element));

    case simdjson::dom::element_type::BOOL:
      return Rcpp::wrap<bool>(element);

    case simdjson::dom::element_type::STRING:
      return Rcpp::wrap(std::string(element));

    case simdjson::dom::element_type::NULL_VALUE:
      return R_NilValue;

    case simdjson::dom::element_type::UINT64:
      return Rcpp::wrap(std::to_string(uint64_t(element)));
#else
    case simdjson::dom::element_type::ARRAY:
      return dispatch_simplify_array<type_policy, int64_opt, simplify_to>(
          element.get<simdjson::dom::array>().first);

    case simdjson::dom::element_type::OBJECT:
      return simplify_object<type_policy, int64_opt, simplify_to>(
          element.get<simdjson::dom::object>().first);

    case simdjson::dom::element_type::DOUBLE:
      return Rcpp::wrap<double>(element.get<double>().first);

    case simdjson::dom::element_type::INT64:
      return utils::resolve_int64<int64_opt>(element.get<int64_t>().first);

    case simdjson::dom::element_type::BOOL:
      return Rcpp::wrap<bool>(element.get<bool>().first);

    case simdjson::dom::element_type::STRING:
      return Rcpp::wrap(element.get<const char*>().first);

    case simdjson::dom::element_type::NULL_VALUE:
      return R_NilValue;

    case simdjson::dom::element_type::UINT64:
      return Rcpp::wrap(std::to_string(element.get<uint64_t>().first));
#endif

    default:
      return R_NilValue;
  }
}


} // namespace deserialize
} // namespace rcppsimdjson

#endif
