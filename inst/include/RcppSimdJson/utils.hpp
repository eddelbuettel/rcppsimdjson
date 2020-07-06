#ifndef RCPPSIMDJSON_UTILS_HPP
#define RCPPSIMDJSON_UTILS_HPP

#include <Rcpp.h>
#include <algorithm> // std::all_of


namespace rcppsimdjson {
namespace utils {

// options for returning big-ints
enum class Int64_R_Type : int {
    Double    = 0,
    String    = 1,
    Integer64 = 2,
};


// Convert `int64_t` to `bit64::integer64`.
inline SEXP as_integer64(int64_t x) {
    auto out = Rcpp::NumericVector(1);
    std::memcpy(&(out[0]), &x, sizeof(double));
    out.attr("class") = "integer64";
    return out;
}
// Convert `std::vector<int64_t>` to `bit64::integer64`.
inline SEXP as_integer64(const std::vector<int64_t>& x) {
    const auto          n = std::size(x);
    Rcpp::NumericVector out(n);
    std::memcpy(&(out[0]), &(x[0]), n * sizeof(double));
    out.attr("class") = "integer64";
    return out;
}


// Check if big-ints can be losslessly downcasted to R integers.
// `int64_t` can be downcasted when between`INT_MIN`/`NA_INTEGER` + 1 and `INT_MAX`
inline constexpr bool is_castable_int64(const int64_t x) noexcept {
    return x <= std::numeric_limits<int>::max() && x > std::numeric_limits<int>::min();
}
// always returns false as `uint64_t` should only be numbers that exeed `int64_t's` capacity
inline constexpr bool is_castable_int64(const uint64_t x) noexcept {
    return false;
}


// check if a `std::vector` containing big-ints can be losslessly copied into an
// `Rcpp::IntegerVector` via iterators
// true if `is_castable_int64<int64_t>()` is true for all elements
inline bool is_castable_int64_vec(std::vector<int64_t>::const_iterator first,
                                  std::vector<int64_t>::const_iterator last) noexcept {
    return std::all_of(first, last, [](const int64_t x) { return is_castable_int64(x); });
}
// always returns false
inline bool is_castable_int64_vec(std::vector<uint64_t>::const_iterator first,
                                  std::vector<uint64_t>::const_iterator last) noexcept {
    return false;
}


// Convert `int64_t` scalar to `SEXP`.
// - returns an integer if `x` can be safely downcasted
// - otherwise `x` is coerced following the provided `Int64_R_Type`
template <Int64_R_Type int64_opt>
inline SEXP resolve_int64(int64_t x) {
    if (is_castable_int64(x)) {
        return Rcpp::wrap<int>(x);
    }

    if constexpr (int64_opt == Int64_R_Type::Double) {
        return Rcpp::wrap<double>(x);
    }

    if constexpr (int64_opt == Int64_R_Type::String) {
        return Rcpp::wrap(std::to_string(x));
    }

    if constexpr (int64_opt == Int64_R_Type::Integer64) {
        return as_integer64(x);
    }
}
// Convert `std::vector<int64_t>` to `SEXP`.
// - returns an `Rcpp::IntegerVector` if `x` can be safely downcasted
// - otherwise `x` is coerced to an `Rcpp::Vector` following the provided `Int64_R_Type`
template <Int64_R_Type int64_opt>
inline SEXP resolve_int64(const std::vector<int64_t>& x) {
    if (is_castable_int64_vec(std::begin(x), std::end(x))) {
        return Rcpp::IntegerVector(std::begin(x), std::end(x));
    }

    if constexpr (int64_opt == Int64_R_Type::Double) {
        return Rcpp::NumericVector(std::begin(x), std::end(x));
    }

    if constexpr (int64_opt == Int64_R_Type::String) {
        return Rcpp::CharacterVector(
            std::begin(x), std::end(x), [](int64_t val) { return std::to_string(val); });
    }

    if constexpr (int64_opt == Int64_R_Type::Integer64) {
        return as_integer64(x);
    }
}


// converts `uint64_t` to `SEXP` (always string)
template <Int64_R_Type int64_opt>
inline SEXP resolve_int64(uint64_t x) {
    return Rcpp::wrap(std::to_string(x));
}
// converts `std::vector<uint64_t>` to `SEXP` (always strings)
template <Int64_R_Type int64_opt>
inline SEXP resolve_int64(const std::vector<uint64_t>& x) {
    return Rcpp::CharacterVector(
        std::begin(x), std::end(x), [](uint64_t val) { return std::to_string(val); });
}


} // namespace utils
} // namespace rcppsimdjson

#endif
