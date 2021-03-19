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
inline SEXP
simplify_list(simdjson::dom::array array, SEXP empty_array, SEXP empty_object, SEXP single_null) {
    Rcpp::List out(r_length(array));
    auto i = R_xlen_t(0);
    for (auto element : array) {
        out[i++] = simplify_element<type_policy, int64_opt, simplify_to>(
            element, empty_array, empty_object, single_null);
    }
    return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP
simplify_vector(simdjson::dom::array array, SEXP empty_array, SEXP empty_object, SEXP single_null) {
    if (const auto type_doctor = Type_Doctor<type_policy, int64_opt>(array);
        type_doctor.is_vectorizable()) {
        return type_doctor.is_homogeneous()
                   ? vector::dispatch_typed<int64_opt>(
                         array, type_doctor.common_R_type(), type_doctor.has_null())
                   : vector::dispatch_mixed<int64_opt>(array, type_doctor.common_R_type());
    }
    return simplify_list<type_policy, int64_opt, simplify_to>(
        array, empty_array, empty_object, single_null);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP
simplify_matrix(simdjson::dom::array array, SEXP empty_array, SEXP empty_object, SEXP single_null) {
    if (const auto matrix = matrix::diagnose<type_policy, int64_opt>(array)) {
        return matrix->is_homogeneous
                   ? matrix::dispatch_typed<int64_opt>(array,
                                                       matrix->common_element_type,
                                                       matrix->common_R_type,
                                                       matrix->has_nulls,
                                                       matrix->n_cols)
                   : matrix::dispatch_mixed<int64_opt>(
                         array, matrix->common_R_type, matrix->n_cols);
    }
    return simplify_vector<type_policy, int64_opt, simplify_to>(
        array, empty_array, empty_object, single_null);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP simplify_data_frame(simdjson::dom::array array,
                                SEXP                 empty_array,
                                SEXP                 empty_object,
                                SEXP                 single_null) {
    if (const auto cols = diagnose_data_frame<type_policy, int64_opt>(array)) {
        return build_data_frame<type_policy, int64_opt, simplify_to>(
            array, cols->schema, empty_array, empty_object, single_null);
    }
    return simplify_matrix<type_policy, int64_opt, simplify_to>(
        array, empty_array, empty_object, single_null);
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP dispatch_simplify_array(simdjson::dom::array array,
                                    SEXP                 empty_array,
                                    SEXP                 empty_object,
                                    SEXP                 single_null) {
    if (std::size(array) == 0) {
        return empty_array;
    }

    if constexpr (simplify_to == Simplify_To::data_frame) {
        return simplify_data_frame<type_policy, int64_opt, Simplify_To::data_frame>(
            array, empty_array, empty_object, single_null);
    }

    if constexpr (simplify_to == Simplify_To::matrix) {
        return simplify_matrix<type_policy, int64_opt, Simplify_To::matrix>(
            array, empty_array, empty_object, single_null);
    }

    if constexpr (simplify_to == Simplify_To::vector) {
        return simplify_vector<type_policy, int64_opt, Simplify_To::vector>(
            array, empty_array, empty_object, single_null);
    }

    if constexpr (simplify_to == Simplify_To::list) {
        return simplify_list<type_policy, int64_opt, Simplify_To::list>(
            array, empty_array, empty_object, single_null);
    }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP simplify_object(const simdjson::dom::object object,
                            SEXP                        empty_array,
                            SEXP                        empty_object,
                            SEXP                        single_null) {
    const auto n = r_length(object);
    if (n == 0) {
        return empty_object;
    }

    Rcpp::List            out(n);
    Rcpp::CharacterVector out_names(n);

    auto i = R_xlen_t(0L);
    for (auto [key, value] : object) {
        out[i] = simplify_element<type_policy, int64_opt, simplify_to>(
            value, empty_array, empty_object, single_null);
        out_names[i++] = Rcpp::String(std::string(key));
    }

    out.attr("names") = out_names;
    return out;
}


/**
 * @brief Simplify a @c simdjson::dom::element to an R object.
 *
 *
 * @tparam type_policy The @c Type_Policy specifying type strictness in combining mixed-type array
 * elements into R vectors.
 *
 * @tparam int64_opt The @c Int64_R_Type specifying how big integers are returned to R.
 *
 * @tparam simplify_to The @c Simplify_To specifying the maximum level of simplification.
 *
 *
 * @param element @c simdjson::dom::element to simplify.
 *
 * @param empty_array R object to return when encountering an empty JSON array.
 *
 * @param empty_object R object to return when encountering an empty JSON object.
 *
 *
 * @return The simplified R object ( @c SEXP ).
 *
 *
 * @note definition: forward declaration in @file inst/include/RcppSimdJson/common.hpp @file.
 */
template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline SEXP simplify_element(simdjson::dom::element element,
                             SEXP                   empty_array,
                             SEXP                   empty_object,
                             SEXP                   single_null) {
    switch (element.type()) {
        case simdjson::dom::element_type::ARRAY:
            return dispatch_simplify_array<type_policy, int64_opt, simplify_to>(
                simdjson::dom::array(element), empty_array, empty_object, single_null);

        case simdjson::dom::element_type::OBJECT:
            return simplify_object<type_policy, int64_opt, simplify_to>(
                simdjson::dom::object(element), empty_array, empty_object, single_null);

        case simdjson::dom::element_type::DOUBLE:
            return Rcpp::wrap(double(element));

        case simdjson::dom::element_type::INT64:
            return utils::resolve_int64<int64_opt>(int64_t(element));

        case simdjson::dom::element_type::BOOL:
            return Rcpp::wrap(bool(element));

        case simdjson::dom::element_type::STRING:
            return Rcpp::wrap(Rcpp::String(std::string(std::string_view(element))));

        case simdjson::dom::element_type::NULL_VALUE:
            return single_null;

        case simdjson::dom::element_type::UINT64:
            return Rcpp::wrap(std::to_string(uint64_t(element)));
    }

    return R_NilValue; // # nocov
}


} // namespace deserialize
} // namespace rcppsimdjson

#endif
