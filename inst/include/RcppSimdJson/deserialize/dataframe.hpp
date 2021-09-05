#ifndef RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP
#define RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP


#include "RcppSimdJson/utils.hpp"
#include "matrix.hpp"


namespace rcppsimdjson {
namespace deserialize {

template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
struct Column {
    R_xlen_t                            index  = 0L;
    Type_Doctor<type_policy, int64_opt> schema = Type_Doctor<type_policy, int64_opt>();
};

template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
struct Column_Schema {
    std::unordered_map<std::string_view, Column<type_policy, int64_opt>> schema =
        std::unordered_map<std::string_view, Column<type_policy, int64_opt>>();
};


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline auto diagnose_data_frame(simdjson::dom::array array) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS)
    -> std::optional<Column_Schema<type_policy, int64_opt>> {
    // if (std::size(array) == 0) { // already handled in `dispatch_simplify_array()`
    //     return std::nullopt;
    // }

    auto cols      = Column_Schema<type_policy, int64_opt>();
    auto col_index = R_xlen_t(0L);

    for (auto element : array) {
        simdjson::dom::object object;
        if(element.get(object) == simdjson::SUCCESS) {
            for (auto [key, value] : object) {
                if (cols.schema.find(key) == std::end(cols.schema)) {
                    cols.schema[key] = Column<type_policy, int64_opt>{
                        col_index++, Type_Doctor<type_policy, int64_opt>()};
                }
                cols.schema[key].schema.add_element(value);
            }
        } else {
            return std::nullopt;
        }
    }

    return cols;
}


template <int RTYPE,
          typename scalar_T,
          rcpp_T              R_Type,
          Type_Policy         type_policy,
          utils::Int64_R_Type int64_opt>
inline auto build_col(simdjson::dom::array                       array,
                      const std::string_view                     key,
                      const Type_Doctor<type_policy, int64_opt>& type_doc) -> Rcpp::Vector<RTYPE> {

    auto out   = Rcpp::Vector<RTYPE>(std::size(array), na_val<R_Type>());
    auto i_row = R_xlen_t(0L);

    if (type_doc.is_homogeneous()) {
        if (type_doc.has_null()) {
            for (auto object : array) {
                simdjson::dom::element element;
                if(object.get_object().at_key(key).get(element) == simdjson::SUCCESS) {
                    out[i_row] = get_scalar<scalar_T, R_Type, HAS_NULLS>(element);
                }
                i_row++;
            }

        } else {

            for (auto object : array) {
                simdjson::dom::element element;
                if(object.get_object().at_key(key).get(element) == simdjson::SUCCESS) {
                    out[i_row] = get_scalar<scalar_T, R_Type, NO_NULLS>(element);
                }
                i_row++;
            }
        }

    } else {

        for (auto object : array) {
            simdjson::dom::element element;
            if(object.get_object().at_key(key).get(element) == simdjson::SUCCESS) {
                out[i_row] = get_scalar_dispatch<RTYPE>(element);
            }
            i_row++;
        }
    }

    return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline auto build_col_integer64(simdjson::dom::array                      array,
                                const std::string_view                    key,
                                const Type_Doctor<type_policy, int64_opt> type_doc) -> SEXP {

    if constexpr (int64_opt == utils::Int64_R_Type::Double) {
        return build_col<REALSXP, int64_t, rcpp_T::dbl, type_policy>(array, key, type_doc);
    }

    if constexpr (int64_opt == utils::Int64_R_Type::String) {
        return build_col<STRSXP, int64_t, rcpp_T::chr, type_policy>(array, key, type_doc);
    }

    if constexpr (int64_opt == utils::Int64_R_Type::Integer64 ||
                  int64_opt == utils::Int64_R_Type::Always) {
        auto stl_vec = std::vector<int64_t>(std::size(array), NA_INTEGER64);
        auto i_row   = std::size_t(0ULL);

        if (type_doc.is_homogeneous()) {
            if (type_doc.has_null()) {
                for (auto object : array) {
                    simdjson::dom::element element;
                    if(object.get_object().at_key(key).get(element) == simdjson::SUCCESS) {
                        stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, HAS_NULLS>(element);
                    }
                    i_row++;
                }

            } else {
                for (auto object : array) {
                    simdjson::dom::element element;
                    if(object.get_object().at_key(key).get(element) == simdjson::SUCCESS) {
                        stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
                    }
                    i_row++;
                }
            }

        } else {
            for (auto object : array) {
                simdjson::dom::element element;
                if(object.get_object().at_key(key).get(element) == simdjson::SUCCESS) {
                    switch (element.type()) {
                        case simdjson::dom::element_type::INT64:
                            stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
                            break;

                        case simdjson::dom::element_type::BOOL:
                            stl_vec[i_row] = get_scalar<bool, rcpp_T::i64, NO_NULLS>(element);
                            break;

                        default:					// #nocov
                            break;					// #nocov
                    }
                }
                i_row++;
            }
        }
        return utils::as_integer64(stl_vec);
    }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto
build_data_frame(simdjson::dom::array                                                        array,
                 const std::unordered_map<std::string_view, Column<type_policy, int64_opt>>& cols,
                 SEXP empty_array,
                 SEXP empty_object,
                 SEXP single_null) -> SEXP {

    const auto n_rows    = R_xlen_t(std::size(array));
    auto       out       = Rcpp::List(std::size(cols));
    auto       out_names = Rcpp::CharacterVector(std::size(cols));

    for (auto&& [key, col] : cols) {
        out_names[col.index] = Rcpp::String(std::string(key));

        switch (col.schema.common_R_type()) {
            case rcpp_T::chr: {
                out[col.index] = build_col<STRSXP, std::string, rcpp_T::chr, type_policy>(
                    array, key, col.schema);
                break;
            }

            case rcpp_T::dbl: {
                out[col.index] =
                    build_col<REALSXP, double, rcpp_T::dbl, type_policy>(array, key, col.schema);
                break;
            }

            case rcpp_T::i64: {
                out[col.index] =
                    build_col_integer64<type_policy, int64_opt>(array, key, col.schema);
                break;
            }

            case rcpp_T::i32: {
                out[col.index] =
                    build_col<INTSXP, int64_t, rcpp_T::i32, type_policy>(array, key, col.schema);
                break;
            }

            case rcpp_T::lgl: {
                out[col.index] =
                    build_col<LGLSXP, bool, rcpp_T::lgl, type_policy>(array, key, col.schema);
                break;
            }

            case rcpp_T::null: {
                out[col.index] = Rcpp::LogicalVector(n_rows, NA_LOGICAL);
                break;
            }

            case rcpp_T::u64: {
                out[col.index] =
                    build_col<STRSXP, uint64_t, rcpp_T::chr, type_policy>(array, key, col.schema);
                break;
            }

            default: {
                auto this_col = Rcpp::Vector<VECSXP>(n_rows);
                auto i_row    = R_xlen_t(0L);
                for (auto element : array) {
                    simdjson::dom::element lookup;
                    if(element.get_object().at_key(key).get(lookup) == simdjson::SUCCESS) {
                        this_col[i_row++] = simplify_element<type_policy, int64_opt, simplify_to>(
                            lookup, empty_array, empty_object, single_null);
                    } else {
                        this_col[i_row++] = NA_LOGICAL;
                    }
                }
                out[col.index] = this_col;
            }
        }
    }

    out.attr("names")     = out_names;
    out.attr("row.names") = Rcpp::seq(1, n_rows);
    out.attr("class")     = "data.frame";

    return out;
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
