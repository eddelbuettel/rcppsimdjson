#ifndef RCPPSIMDJSON__DESERIALIZE_HPP
#define RCPPSIMDJSON__DESERIALIZE_HPP


#include "deserialize/simplify.hpp"


namespace rcppsimdjson {
namespace deserialize {


/**
 * @brief Deserialize a parsed  simdjson::dom::element to R objects.
 *
 *
 * @param element  simdjson::dom::element to deserialize.
 *
 * @param empty_array R object to return when encountering an empty JSON array.
 *
 * @param empty_object R object to return when encountering an empty JSON object.
 *
 * @param type_policy  Type_Policy specifying type strictness in combining mixed-type array
 * elements into R vectors.
 *
 * @param int64_opt  Int64_R_Type specifying how big integers are returned to R.
 *
 * @param simplify_to  Simplify_To specifying the maximum level of simplification.
 *
 *
 * @return The simplified R object ( SEXP ).
 */
inline auto deserialize(simdjson::dom::element    parsed,
                        SEXP                      empty_array,
                        SEXP                      empty_object,
                        SEXP                      single_null,
                        const Simplify_To         simplify_to,
                        const Type_Policy         type_policy,
                        const utils::Int64_R_Type int64_opt) -> SEXP {
    using Int64_R_Type = utils::Int64_R_Type;

    // THE GREAT DISPATCHER
    switch (type_policy) {
        case Type_Policy::anything_goes: {
            switch (int64_opt) {
                case Int64_R_Type::Double: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::Double
                case Int64_R_Type::String: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::String
                case Int64_R_Type::Integer64: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    }
                } // simplify_to
            }     // Int64_R_Type::Integer64
        }         // Type_Policy::anything_goes


        case Type_Policy::ints_as_dbls: {
            switch (int64_opt) {
                case Int64_R_Type::Double: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::Double
                case Int64_R_Type::String: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::String
                case Int64_R_Type::Integer64: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    }
                } // simplify_to
            }     // Int64_R_Type::Integer64
        }         // Type_Policy::ints_as_dbls


        case Type_Policy::strict: {
            switch (int64_opt) {
                case Int64_R_Type::Double: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::strict, //
                                                    Int64_R_Type::Double,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::Double
                case Int64_R_Type::String: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::String,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::String,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::String,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::strict, //
                                                    Int64_R_Type::String,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::String
                case Int64_R_Type::Integer64: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    }
                } // simplify_to
            }     // Int64_R_Type::Integer64
        }         // Type_Policy::strict
    }

    return R_NilValue;
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
