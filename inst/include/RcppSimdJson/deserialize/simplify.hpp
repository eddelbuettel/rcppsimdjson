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


// forward declaration
template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_element(const simdjson::dom::element, const SEXP, const SEXP) -> SEXP;


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto build_data_frame(const simdjson::dom::array array,
                             const std::map<std::string_view, Column<type_policy>>& cols,
                             const SEXP empty_array,
                             const SEXP empty_object) -> SEXP {

  const auto n_rows = R_xlen_t(std::size(array));
  auto out = Rcpp::List(std::size(cols));
  auto out_names = Rcpp::CharacterVector(std::size(cols));

  for (auto [key, col] : cols) {
    out_names[col.index] = std::string(key);

    switch (col.schema.common_R_type()) {
      case rcpp_T::chr: {
        out[col.index] =
            build_col<STRSXP, std::string, rcpp_T::chr, type_policy>(array, key, col.schema);
        break;
      }

      case rcpp_T::dbl: {
        out[col.index] =
            build_col<REALSXP, double, rcpp_T::dbl, type_policy>(array, key, col.schema);
        break;
      }

      case rcpp_T::i64: {
        out[col.index] = build_col_integer64<type_policy, int64_opt>(array, key, col.schema);
        break;
      }

      case rcpp_T::i32: {
        out[col.index] =
            build_col<INTSXP, int64_t, rcpp_T::i32, type_policy>(array, key, col.schema);
        break;
      }

      case rcpp_T::lgl: {
        out[col.index] = build_col<LGLSXP, bool, rcpp_T::lgl, type_policy>(array, key, col.schema);
        break;
      }

      case rcpp_T::null: {
        out[col.index] = Rcpp::LogicalVector(n_rows, NA_LOGICAL);
        break;
      }

      default: {
        auto this_col = Rcpp::Vector<VECSXP>(n_rows);
        auto i_row = R_xlen_t(0);
        for (auto element : array) {
          auto [value, error] = element.get<simdjson::dom::object>().at_key(key);
          if (error) {
            this_col[i_row++] = NA_LOGICAL;
          } else {
            this_col[i_row++] = simplify_element<type_policy, int64_opt, simplify_to>(
                value, empty_array, empty_object //
            );
          }
        }
        out[col.index] = this_col;
      }
    }
  }

  out.attr("names") = out_names;
  out.attr("row.names") = Rcpp::seq(1, n_rows);
  out.attr("class") = "data.frame";

  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_list(const simdjson::dom::array array,
                          const SEXP empty_array,
                          const SEXP empty_object) -> SEXP {
  Rcpp::List out(std::size(array));

  auto i = R_xlen_t(0);
  for (auto element : array) {
    out[i++] = simplify_element<type_policy, int64_opt, simplify_to>( //
        element,                                                      //
        empty_array,                                                  //
        empty_object                                                  //
    );                                                                //
  }

  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_vector(const simdjson::dom::array array,
                            const SEXP empty_array,
                            const SEXP empty_object) -> SEXP {
  const auto type_doctor = Type_Doctor<type_policy>(array);

  if (type_doctor.is_vectorizable()) {
    return type_doctor.is_homogeneous()
               ? vector::dispatch_typed<int64_opt>( //
                     array,                         //
                     type_doctor.common_R_type(),   //
                     type_doctor.has_null()         //
                     )                              //
               : vector::dispatch_mixed<int64_opt>(array, type_doctor.common_R_type());
  }

  return simplify_list<type_policy, int64_opt, simplify_to>(array, empty_array, empty_object);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_matrix(const simdjson::dom::array array,
                            const SEXP empty_array,
                            const SEXP empty_object) -> SEXP {
  if (const auto matrix = matrix::diagnose<type_policy>(array)) {
    return matrix->is_homogeneous
               ? matrix::dispatch_typed<int64_opt>( //
                     array,                         //
                     matrix->common_element_type,   //
                     matrix->common_R_type,         //
                     matrix->has_nulls,             //
                     matrix->n_cols                 //
                     )                              //
               : matrix::dispatch_mixed<int64_opt>(array, matrix->common_R_type, matrix->n_cols);
  }

  return simplify_vector<type_policy, int64_opt, simplify_to>(array, empty_array, empty_object);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_data_frame(const simdjson::dom::array array,
                                const SEXP empty_array,
                                const SEXP empty_object) -> SEXP {
  if (const auto cols = diagnose_data_frame<type_policy>(array)) {
    return build_data_frame<type_policy, int64_opt, simplify_to>( //
        array,                                                    //
        cols->schema,                                             //
        empty_array,                                              //
        empty_object                                              //
    );                                                            //
  }

  return simplify_matrix<type_policy, int64_opt, simplify_to>(array, empty_array, empty_object);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto dispatch_simplify_array(const simdjson::dom::array array,
                                    const SEXP empty_array,
                                    const SEXP empty_object) -> SEXP {

  if (std::size(array) == 0) {
    return empty_array;
  }

  if constexpr (simplify_to == Simplify_To::data_frame) {
    return simplify_data_frame<type_policy, int64_opt, Simplify_To::data_frame>( //
        array,                                                                   //
        empty_array,                                                             //
        empty_object                                                             //
    );                                                                           //
  }

  if constexpr (simplify_to == Simplify_To::matrix) {
    return simplify_matrix<type_policy, int64_opt, Simplify_To::matrix>( //
        array,                                                           //
        empty_array,                                                     //
        empty_object                                                     //
    );
  }

  if constexpr (simplify_to == Simplify_To::vector) {
    return simplify_vector<type_policy, int64_opt, Simplify_To::vector>( //
        array,                                                           //
        empty_array,                                                     //
        empty_object                                                     //
    );
  }

  if constexpr (simplify_to == Simplify_To::list) {
    return simplify_list<type_policy, int64_opt, Simplify_To::list>( //
        array,                                                       //
        empty_array,                                                 //
        empty_object                                                 //
    );                                                               //
  }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_object(const simdjson::dom::object object,
                            const SEXP empty_array,
                            const SEXP empty_object) -> SEXP {
  const auto n = R_xlen_t(std::size(object));
  if (n == 0) {
    return empty_object;
  }

  Rcpp::List out(n);
  Rcpp::CharacterVector out_names(n);

  auto i = R_xlen_t(0);
  for (auto [key, value] : object) {
    out[i] =
        simplify_element<type_policy, int64_opt, simplify_to>(value, empty_array, empty_object);
    out_names[i++] = std::string(key);
  }

  out.attr("names") = out_names;
  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_element(const simdjson::dom::element element,
                             const SEXP empty_array,
                             const SEXP empty_object) -> SEXP {

  switch (element.type()) {
    case simdjson::dom::element_type::ARRAY:
      return dispatch_simplify_array<type_policy, int64_opt, simplify_to>(
          element.get<simdjson::dom::array>().first, //
          empty_array,                               //
          empty_object                               //
      );

    case simdjson::dom::element_type::OBJECT:
      return simplify_object<type_policy, int64_opt, simplify_to>(
          element.get<simdjson::dom::object>().first, //
          empty_array,                                //
          empty_object                                //
      );

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

    default:
      return R_NilValue;
  }
}


} // namespace deserialize
} // namespace rcppsimdjson

#endif
