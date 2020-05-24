#ifndef RCPPSIMDJSON_UTILS_HPP
#define RCPPSIMDJSON_UTILS_HPP


#include <algorithm>
#include <Rcpp.h>


namespace rcppsimdjson {
namespace utils {

enum class Int64_R_Type : int {
  Double = 0,
  String = 1,
  Integer64 = 2,
};


template <typename... Args>
inline SEXP as_integer64(Args... args);


template <>
inline SEXP as_integer64(int64_t x) {
  auto out = Rcpp::NumericVector(1);
  std::memcpy(&(out[0]), &x, sizeof(double));
  out.attr("class") = "integer64";
  return out;
}


template <typename int_T>
inline constexpr bool is_castable_int64(int_T);


template <>
inline constexpr bool is_castable_int64<int64_t>(int64_t x) {
  return x <= std::numeric_limits<int>::max() && x > NA_INTEGER;
}
template <>
inline constexpr bool is_castable_int64<uint64_t>(uint64_t x) {
  return false;
}


template <typename... Args>
inline constexpr SEXP resolve_int64(Args... args);


template <Int64_R_Type int64_opt>
inline constexpr SEXP resolve_int64(uint64_t x) {
  return Rcpp::wrap(std::to_string(x));
}


template <Int64_R_Type int64_opt>
inline constexpr SEXP resolve_int64(int64_t x) {
#if __cplusplus >= 201402L

  if (is_castable_int64<int64_t>(x)) {
    return Rcpp::wrap<int>(x);
  }

  switch (int64_opt) {
    case Int64_R_Type::Double:
      return Rcpp::wrap<double>(x);

    case Int64_R_Type::String:
      return Rcpp::wrap(std::to_string(x));

    case Int64_R_Type::Integer64:
      return as_integer64(x);
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


template <typename int_T>
inline constexpr bool is_castable_int64_vec(std::vector<int_T>& x) {
  return std::all_of(std::begin(x), std::end(x), is_castable_int64<int_T>);
}


template <Int64_R_Type int64_opt>
inline constexpr SEXP resolve_int64_vec();


template <Int64_R_Type int64_opt>
inline constexpr SEXP resolve_int64_vec(std::vector<int64_t>& x) {
  if (is_castable_int64_vec<int64_t>(x)) {
    return Rcpp::IntegerVector(std::begin(x), std::end(x));
  }

  switch (int64_opt) {
    case Int64_R_Type::Double:
      return Rcpp::NumericVector(std::begin(x), std::end(x));

    case Int64_R_Type::String: {
      Rcpp::CharacterVector out(std::size(x));
      std::transform(std::begin(x), std::end(x), std::begin(out),
                     [](auto val) { return std::to_string(val); });
      return out;
    }

    case Int64_R_Type::Integer64: {
      const auto n = std::size(x);
      Rcpp::NumericVector out(n);
      std::memcpy(&(out[0]), &(x[0]), n * sizeof(double));
      out.attr("class") = "integer64";
      return out;
    }
  }

  return R_NilValue;
}


template <Int64_R_Type int64_opt>
inline constexpr SEXP resolve_int64_vec(std::vector<uint64_t>& x) {
  Rcpp::CharacterVector out(std::size(x));
  std::transform(std::begin(x), std::end(x), std::begin(out),
                 [](auto val) { return std::to_string(val); });
  return out;
}


}  // namespace utils
}  // namespace rcppsimdjson

#endif