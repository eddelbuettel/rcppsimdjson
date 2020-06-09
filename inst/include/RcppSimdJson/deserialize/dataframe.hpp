#ifndef RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP
#define RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP

#include "matrix.hpp"


namespace rcppsimdjson {
namespace deserialize {


template <Type_Policy type_policy> struct Data_Frame_Diagnosis {
  bool is_data_frame_ish = false;
  std::map<std::string_view, Type_Doctor<type_policy>> cols = // TODO don't use map
      std::map<std::string_view, Type_Doctor<type_policy>>();
};


template <Type_Policy type_policy>
inline auto
diagnose_data_frame(const simdjson::dom::array array) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS)
    -> Data_Frame_Diagnosis<type_policy> {

  auto cols = std::map<std::string_view, Type_Doctor<type_policy>>();

  if (std::size(array) == 0) {
    return Data_Frame_Diagnosis<type_policy>();
  }

  auto lengths = std::set<std::size_t>();

  for (auto element : array) {
    const auto [object, error] = element.get<simdjson::dom::object>();

    if (error) {
      return Data_Frame_Diagnosis<type_policy>();
    }

    for (auto [key, value] : object) {
      if (cols.find(key) == std::end(cols)) {
        cols[key] = Type_Doctor<type_policy>();
      }
      cols[key].add_element(value);
    }

    lengths.insert(std::size(object));
  }

  // for (auto [key, value] : cols) {
  //   if (!value.is_vectorizable()) { // TODO how should nested values be handled?
  //     cols.clear();
  //     return Data_Frame_Diagnosis<type_policy>{false, cols};
  //   }
  // }

  return Data_Frame_Diagnosis<type_policy>{true, cols};
}


template <int RTYPE, typename scalar_T, rcpp_T R_Type, Type_Policy type_policy>
inline auto build_col(simdjson::dom::array array,
                      const std::string_view key,
                      const Type_Doctor<type_policy> type_doc) -> Rcpp::Vector<RTYPE> {

  Rcpp::Vector<RTYPE> out(std::size(array));
  out.fill(na_val<R_Type>());

  auto i_row = R_xlen_t(0);

  if (type_doc.is_homogeneous()) {
    if (type_doc.has_null()) {
      for (simdjson::dom::object object : array) {
        auto [element, error] = object.at_key(key);
        if (!error) {
          out[i_row] = get_scalar<scalar_T, R_Type, HAS_NULLS>(element);
        }
        i_row++;
      }

    } else {

      for (simdjson::dom::object object : array) {
        auto [element, error] = object.at_key(key);
        if (!error) {
          out[i_row] = get_scalar<scalar_T, R_Type, NO_NULLS>(element);
        }
        i_row++;
      }
    }

  } else {

    for (simdjson::dom::object object : array) {
      auto [element, error] = object.at_key(key);
      if (!error) {
        out[i_row] = get_scalar_dispatch<RTYPE>(element);
      }
      i_row++;
    }
  }

  return out;
}


template <Type_Policy type_policy, rcppsimdjson::utils::Int64_R_Type int64_opt>
inline auto build_col_integer64(simdjson::dom::array array,
                                const std::string_view key,
                                const Type_Doctor<type_policy> type_doc) -> SEXP {

  if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Double) {
    return build_col<REALSXP, int64_t, rcpp_T::dbl, type_policy>(array, key, type_doc);
  }

  if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::String) {
    return build_col<STRSXP, int64_t, rcpp_T::chr, type_policy>(array, key, type_doc);
  }

  if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Integer64) {
    std::vector<int64_t> stl_vec(std::size(array), NA_INTEGER64);
    std::size_t i_row = 0;

    if (type_doc.is_homogeneous()) {
      if (type_doc.has_null()) {
        for (simdjson::dom::object object : array) {
          auto [element, error] = object.at_key(key);
          if (!error) {
            stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, HAS_NULLS>(element);
          }
          i_row++;
        }

      } else {
        for (simdjson::dom::object object : array) {
          auto [element, error] = object.at_key(key);
          if (!error) {
            stl_vec[i_row] = get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
          }
          i_row++;
        }
      }

    } else {
      for (simdjson::dom::object object : array) {
        auto [element, error] = object.at_key(key);
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
    return rcppsimdjson::utils::as_integer64(stl_vec);
  }
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
