#ifndef RCPPSIMDJSON__DESERIALIZE__MATRIX_HPP
#define RCPPSIMDJSON__DESERIALIZE__MATRIX_HPP

#include "vector.hpp"

namespace rcppsimdjson {
namespace deserialize {
namespace matrix {

struct Matrix_Diagnosis {
    bool                        has_nulls           = false;
    bool                        is_homogeneous      = false;
    simdjson::dom::element_type common_element_type = simdjson::dom::element_type::NULL_VALUE;
    rcpp_T                      common_R_type       = rcpp_T::null;
    std::size_t                 n_cols              = 0;
};


template <Type_Policy type_policy>
inline auto diagnose(simdjson::dom::array array) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS)
    -> std::optional<Matrix_Diagnosis> {

    auto n_cols        = std::set<std::size_t>();
    auto matrix_doctor = Type_Doctor<type_policy>();

    for (auto&& element : array) {
        if (element.type() != simdjson::dom::element_type::ARRAY) {
            return std::nullopt;
        }

        matrix_doctor.update(Type_Doctor<type_policy>(element.get<simdjson::dom::array>().first));
        n_cols.insert(std::size(element.get<simdjson::dom::array>().first));

        if (std::size(n_cols) > 1 || !matrix_doctor.is_vectorizable()) {
            return std::nullopt;
        };
    }

    return Matrix_Diagnosis{matrix_doctor.has_null(),
                            matrix_doctor.is_homogeneous(),
                            matrix_doctor.common_element_type(),
                            matrix_doctor.common_R_type(),
                            *std::begin(n_cols)};
}


template <int RTYPE, typename in_T, rcpp_T R_Type, bool has_nulls>
inline auto build_matrix_typed(simdjson::dom::array array, std::size_t n_cols)
    -> Rcpp::Vector<RTYPE> {

    const auto n_rows = r_length(array);
    auto       out    = Rcpp::Matrix<RTYPE>(n_rows, static_cast<R_xlen_t>(n_cols));
    auto       j      = R_xlen_t(0);

#ifdef RCPPSIMDJSON_IS_GCC_7

    for (simdjson::dom::array sub_array : array) {
        auto i = R_xlen_t(0);
        for (auto&& element : sub_array) {
            out[i + j] = get_scalar<in_T, R_Type, has_nulls>(element);
            i += n_rows;
        }
        j++;
    }

#else

    for (auto&& sub_array : array) {
        auto i = R_xlen_t(0);
        for (auto&& element : sub_array.get<simdjson::dom::array>().first) {
            out[i + j] = get_scalar<in_T, R_Type, has_nulls>(element);
            i += n_rows;
        }
        j++;
    }

#endif

    return out;
}

template <bool has_nulls>
inline auto build_matrix_integer64_typed(simdjson::dom::array array, std::size_t n_cols)
    -> Rcpp::Vector<REALSXP> {

    const auto n_rows        = std::size(array);
    auto       stl_vec_int64 = std::vector<int64_t>(n_rows * n_cols);
    auto       j             = std::size_t(0);

#ifdef RCPPSIMDJSON_IS_GCC_7

    for (simdjson::dom::array sub_array : array) {
        auto i = std::size_t(0);
        for (auto&& element : sub_array) {
            stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, has_nulls>(element);
            i += n_rows;
        }
        j++;
    }

#else

    for (auto&& sub_array : array) {
        auto i = std::size_t(0);
        for (auto&& element : sub_array.get<simdjson::dom::array>().first) {
            stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, has_nulls>(element);
            i += n_rows;
        }
        j++;
    }

#endif

    auto out = Rcpp::NumericVector(utils::as_integer64(stl_vec_int64));
    out.attr("dim") =
        Rcpp::IntegerVector::create(static_cast<R_xlen_t>(n_rows), static_cast<R_xlen_t>(n_cols));

    return out;
}


template <utils::Int64_R_Type int64_opt>
inline auto dispatch_typed(simdjson::dom::array        array,
                           simdjson::dom::element_type element_type,
                           const rcpp_T                R_Type,
                           const bool                  has_nulls,
                           const std::size_t           n_cols) -> SEXP {

    switch (element_type) {
        case simdjson::dom::element_type::STRING:
            return has_nulls
                       ? build_matrix_typed<STRSXP, std::string, rcpp_T::chr, HAS_NULLS>(array,
                                                                                         n_cols)
                       : build_matrix_typed<STRSXP, std::string, rcpp_T::chr, NO_NULLS>(array,
                                                                                        n_cols);

        case simdjson::dom::element_type::DOUBLE:
            return has_nulls
                       ? build_matrix_typed<REALSXP, double, rcpp_T::dbl, HAS_NULLS>(array, n_cols)
                       : build_matrix_typed<REALSXP, double, rcpp_T::dbl, NO_NULLS>(array, n_cols);

        case simdjson::dom::element_type::INT64: {
            if (R_Type == rcpp_T::i32) {
                return has_nulls
                           ? build_matrix_typed<INTSXP, int64_t, rcpp_T::i32, HAS_NULLS>(array,
                                                                                         n_cols)
                           : build_matrix_typed<INTSXP, int64_t, rcpp_T::i32, NO_NULLS>(array,
                                                                                        n_cols);
            }

            if constexpr (int64_opt == utils::Int64_R_Type::Double) {
                return has_nulls
                           ? build_matrix_typed<REALSXP, int64_t, rcpp_T::dbl, HAS_NULLS>(array,
                                                                                          n_cols)
                           : build_matrix_typed<REALSXP, int64_t, rcpp_T::dbl, NO_NULLS>(array,
                                                                                         n_cols);
            }

            if constexpr (int64_opt == utils::Int64_R_Type::String) {
                return has_nulls
                           ? build_matrix_typed<STRSXP, int64_t, rcpp_T::chr, HAS_NULLS>(array,
                                                                                         n_cols)
                           : build_matrix_typed<STRSXP, int64_t, rcpp_T::chr, NO_NULLS>(array,
                                                                                        n_cols);
            }

            if constexpr (int64_opt == utils::Int64_R_Type::Integer64) {
                return has_nulls ? build_matrix_integer64_typed<HAS_NULLS>(array, n_cols)
                                 : build_matrix_integer64_typed<NO_NULLS>(array, n_cols);
            }
        }

        case simdjson::dom::element_type::BOOL:
            return has_nulls
                       ? build_matrix_typed<LGLSXP, bool, rcpp_T::lgl, HAS_NULLS>(array, n_cols)
                       : build_matrix_typed<LGLSXP, bool, rcpp_T::lgl, NO_NULLS>(array, n_cols);


        case simdjson::dom::element_type::UINT64:
            return has_nulls
                       ? build_matrix_typed<STRSXP, uint64_t, rcpp_T::chr, HAS_NULLS>(array, n_cols)
                       : build_matrix_typed<STRSXP, uint64_t, rcpp_T::chr, NO_NULLS>(array, n_cols);

            // # nocov start
        case simdjson::dom::element_type::NULL_VALUE:
            return Rcpp::LogicalVector(r_length(array), NA_LOGICAL);

        default:
            return R_NilValue;
            // # nocov end
    }
}

template <int RTYPE>
inline auto build_matrix_mixed(simdjson::dom::array array, std::size_t n_cols) -> SEXP {

    const auto          n_rows = r_length(array);
    Rcpp::Matrix<RTYPE> out(n_rows, static_cast<R_xlen_t>(n_cols));

    auto j = R_xlen_t(0);

#ifdef RCPPSIMDJSON_IS_GCC_7

    for (simdjson::dom::array sub_array : array) {
        auto i = R_xlen_t(0);
        for (auto&& element : sub_array) {
            out[i + j] = get_scalar_dispatch<RTYPE>(element);
            i += n_rows;
        }
        j++;
    }

#else
    for (auto&& sub_array : array) {
        auto i = R_xlen_t(0);
        for (auto&& element : sub_array.get<simdjson::dom::array>().first) {
            out[i + j] = get_scalar_dispatch<RTYPE>(element);
            i += n_rows;
        }
        j++;
    }
#endif

    return out;
}


inline auto build_matrix_integer64_mixed(simdjson::dom::array array, std::size_t n_cols)
    -> Rcpp::Vector<REALSXP> {

    const auto n_rows        = std::size(array);
    auto       stl_vec_int64 = std::vector<int64_t>(n_rows * n_cols);
    auto       j             = std::size_t(0ULL);

#ifdef RCPPSIMDJSON_IS_GCC_7

    for (simdjson::dom::array sub_array : array) {
        std::size_t i = 0;
        for (auto&& element : sub_array) {
            switch (element.type()) {
                case simdjson::dom::element_type::INT64:
                    stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
                    break;

                case simdjson::dom::element_type::BOOL:
                    stl_vec_int64[i + j] = get_scalar<bool, rcpp_T::i64, NO_NULLS>(element);
                    break;

                default:
                    stl_vec_int64[i + j] = NA_INTEGER64;
            }
            i += n_rows;
        }
        j++;
    }

#else

    for (auto&& element : array) {
        std::size_t i = 0;
        for (auto&& sub_element : element.get<simdjson::dom::array>().first) {
            switch (sub_element.type()) {
                case simdjson::dom::element_type::INT64:
                    stl_vec_int64[i + j] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(sub_element);
                    break;

                case simdjson::dom::element_type::BOOL:
                    stl_vec_int64[i + j] = get_scalar<bool, rcpp_T::i64, NO_NULLS>(sub_element);
                    break;

                default:
                    stl_vec_int64[i + j] = NA_INTEGER64;
            }
            i += n_rows;
        }
        j++;
    }

#endif

    auto out = Rcpp::Vector<REALSXP>(utils::as_integer64(stl_vec_int64));
    out.attr("dim") =
        Rcpp::IntegerVector::create(static_cast<R_xlen_t>(n_rows), static_cast<R_xlen_t>(n_cols));

    return out;
}


template <utils::Int64_R_Type int64_opt>
inline auto
dispatch_mixed(simdjson::dom::array array, const rcpp_T R_Type, const std::size_t n_cols) -> SEXP {
    switch (R_Type) {
        case rcpp_T::chr:
            return build_matrix_mixed<STRSXP>(array, n_cols);

        case rcpp_T::dbl:
            return build_matrix_mixed<REALSXP>(array, n_cols);

        case rcpp_T::i64: {
            if constexpr (int64_opt == utils::Int64_R_Type::Double) {
                return build_matrix_mixed<REALSXP>(array, n_cols);
            }

            if constexpr (int64_opt == utils::Int64_R_Type::String) {
                return build_matrix_mixed<STRSXP>(array, n_cols);
            }

            if constexpr (int64_opt == utils::Int64_R_Type::Integer64) {
                return build_matrix_integer64_mixed(array, n_cols);
            }
        }

        case rcpp_T::i32:
            return build_matrix_mixed<INTSXP>(array, n_cols);

        case rcpp_T::lgl:                                     // # nocov
            return build_matrix_mixed<LGLSXP>(array, n_cols); // # nocov

        case rcpp_T::u64:
            return build_matrix_mixed<STRSXP>(array, n_cols);

        default: {
            auto out = Rcpp::LogicalMatrix(r_length(array), static_cast<R_xlen_t>(n_cols));
            out.fill(NA_LOGICAL);
            return out;
        }
    }
}

} // namespace matrix
} // namespace deserialize
} // namespace rcppsimdjson


#endif
