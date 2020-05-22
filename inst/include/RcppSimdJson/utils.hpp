#ifndef RCPPSIMDJSON_UTILS_HPP
#define RCPPSIMDJSON_UTILS_HPP

// #if __cplusplus >= 202002L
// #define RCPPSIMDJSON_AT_LEAST_CPP20
// #define RCPPSIMDJSON_AT_LEAST_CPP17
// #define RCPPSIMDJSON_AT_LEAST_CPP14
// #define RCPPSIMDJSON_AT_LEAST_CPP11
// #elif __cplusplus >= 201703L
// #define RCPPSIMDJSON_AT_LEAST_CPP17
// #define RCPPSIMDJSON_AT_LEAST_CPP14
// #define RCPPSIMDJSON_AT_LEAST_CPP11
// #elif __cplusplus >= 201402L
// #define RCPPSIMDJSON_AT_LEAST_CPP14
// #define RCPPSIMDJSON_AT_LEAST_CPP11
// #elif __cplusplus >= 201103L
// #define RCPPSIMDJSON_AT_LEAST_CPP11
// #endif

namespace rcppsimdjson {
namespace utils {

enum class Int64_R_Type : int {
  Double = 0,
  String = 1,
  Integer64 = 2,
};


inline SEXP as_scalar_integer64(int64_t obj) {
  auto out = Rcpp::NumericVector(1);
  std::memcpy(&(out[0]), &obj, sizeof(double));
  out.attr("class") = "integer64";
  return out;
}


template <typename int_T>
inline constexpr bool is_castable_int64(int_T);

template <>
inline constexpr bool is_castable_int64<uint64_t>(uint64_t x) {
  return x <= std::numeric_limits<int>::max();
}


template <>
inline constexpr bool is_castable_int64<int64_t>(int64_t x) {
  return x <= std::numeric_limits<int>::max() && x > NA_INTEGER;
}


template <typename int_T, Int64_R_Type int64_opt>
inline constexpr SEXP resolve_int64(int_T x) {
#if __cplusplus >= 201402L

  if (is_castable_int64<int_T>(x)) {
    return Rcpp::wrap<int>(x);
  }

  switch (int64_opt) {
    case Int64_R_Type::Double:
      return Rcpp::wrap<double>(x);

    case Int64_R_Type::String:
      return Rcpp::wrap(std::to_string(x));

    case Int64_R_Type::Integer64:
      return as_scalar_integer64(x);
  }

#else

  return is_castable_int64<int_T>(x)                      //
             ? Rcpp::wrap<int>(x)                         //
             : int64_opt == Int64_R_Type::Double          //
                   ? Rcpp::wrap<double>(x)                //
                   : int64_opt == Int64_R_Type::String    //
                         ? Rcpp::wrap(std::to_string(x))  //
                         : as_scalar_integer64(x);        //

#endif

  return R_NilValue;
}


}  // namespace utils
}  // namespace rcppsimdjson

#endif