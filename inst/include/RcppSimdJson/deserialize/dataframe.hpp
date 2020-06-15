#ifndef RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP
#define RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP

#include "matrix.hpp"


namespace rcppsimdjson {
namespace deserialize {

template <Type_Policy type_policy> struct Column {
  R_xlen_t index = 0L;
  Type_Doctor<type_policy> schema = Type_Doctor<type_policy>();
};

template <Type_Policy type_policy> struct Column_Schema {
  std::map<std::string_view, Column<type_policy>> schema =
      std::map<std::string_view, Column<type_policy>>();
};


template <Type_Policy type_policy>
inline auto
diagnose_data_frame(const simdjson::dom::array array) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS)
    -> std::optional<Column_Schema<type_policy>> {

  auto cols = Column_Schema<type_policy>();
  auto col_index = R_xlen_t(0L);

  if (std::size(array) == 0) {
    return std::nullopt;
  }

  auto lengths = std::set<std::size_t>();

  for (auto element : array) {
    const auto [object, error] = element.get<simdjson::dom::object>();

    if (error) {
      return std::nullopt;
    }

    for (auto [key, value] : object) {
      if (cols.schema.find(key) == std::end(cols.schema)) {
        cols.schema[key] = Column<type_policy>{col_index++, Type_Doctor<type_policy>()};
      }
      cols.schema[key].schema.add_element(value);
    }

    lengths.insert(std::size(object));
  }

  return cols;
}


template <int RTYPE, typename scalar_T, rcpp_T R_Type, Type_Policy type_policy>
inline auto build_col(const simdjson::dom::array array,
                      const std::string_view key,
                      const Type_Doctor<type_policy>& type_doc) -> Rcpp::Vector<RTYPE> {

  auto out = Rcpp::Vector<RTYPE>(std::size(array), na_val<R_Type>());
  auto i_row = R_xlen_t(0L);

  if (type_doc.is_homogeneous()) {
    if (type_doc.has_null()) {
      for (auto object : array) {
        auto [element, error] = object.get<simdjson::dom::object>().at_key(key);
        if (!error) {
          out[i_row] = get_scalar<scalar_T, R_Type, HAS_NULLS>(element);
        }
        i_row++;
      }

    } else {

      for (auto object : array) {
        auto [element, error] = object.get<simdjson::dom::object>().at_key(key);
        if (!error) {
          out[i_row] = get_scalar<scalar_T, R_Type, NO_NULLS>(element);
        }
        i_row++;
      }
    }

  } else {

    for (auto object : array) {
      auto [element, error] = object.get<simdjson::dom::object>().at_key(key);
      if (!error) {
        out[i_row] = get_scalar_dispatch<RTYPE>(element);
      }
      i_row++;
    }
  }

  return out;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline auto build_col_integer64(const simdjson::dom::array array,
                                const std::string_view key,
                                const Type_Doctor<type_policy> type_doc) -> SEXP {

  if constexpr (int64_opt == utils::Int64_R_Type::Double) {
    return build_col<REALSXP, int64_t, rcpp_T::dbl, type_policy>(array, key, type_doc);
  }

  if constexpr (int64_opt == utils::Int64_R_Type::String) {
    return build_col<STRSXP, int64_t, rcpp_T::chr, type_policy>(array, key, type_doc);
  }

  if constexpr (int64_opt == utils::Int64_R_Type::Integer64) {
    auto stl_vec = std::vector<int64_t>(std::size(array), NA_INTEGER64);
    auto i_row = std::size_t(0ULL);

    if (type_doc.is_homogeneous()) {
      if (type_doc.has_null()) {
        for (auto object : array) {
          auto [element, error] = object.get<simdjson::dom::object>().at_key(key);
          if (!error) {
            stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, HAS_NULLS>(element);
          }
          i_row++;
        }

      } else {
        for (auto object : array) {
          auto [element, error] = object.get<simdjson::dom::object>().at_key(key);
          if (!error) {
            stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
          }
          i_row++;
        }
      }

    } else {
      for (auto object : array) {
        auto [element, error] = object.get<simdjson::dom::object>().at_key(key);
        if (!error) {
          switch (element.type()) {
            case simdjson::dom::element_type::INT64:
              stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
              break;

            case simdjson::dom::element_type::BOOL:
              stl_vec[i_row] = get_scalar<bool, rcpp_T::i64, NO_NULLS>(element);
              break;

            default:
              break;
          }
        }
        i_row++;
      }
    }
    return utils::as_integer64(stl_vec);
  }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto build_data_frame(const simdjson::dom::array array,
                             const std::map<std::string_view, Column<type_policy>>& cols,
                             SEXP empty_array,
                             SEXP empty_object) -> SEXP {

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
        auto i_row = R_xlen_t(0L);
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


} // namespace deserialize
} // namespace rcppsimdjson


#endif
