#ifndef RCPPSIMDJSON__DESERIALIZE__SIMPLIFY_HPP
#define RCPPSIMDJSON__DESERIALIZE__SIMPLIFY_HPP

#include "../common.hpp"
#include "Type_Doctor.hpp"
#include "scalar.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "dataframe.hpp"


namespace rcppsimdjson {
namespace deserialize {


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_list(const simdjson::dom::array array,
                          const SEXP empty_array,
                          const SEXP empty_object) -> SEXP {
  Rcpp::List out(r_length(array));

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
  const auto n = r_length(object);
  if (n == 0) {
    return empty_object;
  }

  Rcpp::List out(n);
  Rcpp::CharacterVector out_names(n);

  auto i = R_xlen_t(0L);
  for (auto [key, value] : object) {
    out[i] =
        simplify_element<type_policy, int64_opt, simplify_to>(value, empty_array, empty_object);
    out_names[i++] = std::string(key);
  }

  out.attr("names") = out_names;
  return out;
}


// definition: forward declaration in inst/include/RcppSimdJson/deserialize/simplify.hpp
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
      return Rcpp::wrap(element.get<double>().first);

    case simdjson::dom::element_type::INT64:
      return utils::resolve_int64<int64_opt>(element.get<int64_t>().first);

    case simdjson::dom::element_type::BOOL:
      return Rcpp::wrap(element.get<bool>().first);

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
