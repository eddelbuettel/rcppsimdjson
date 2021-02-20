#ifndef RCPPSIMDJSON_UTILS_HPP
#define RCPPSIMDJSON_UTILS_HPP


#include <Rcpp.h>


#include <algorithm>   /* std::all_of */
#include <fstream>     /* std::ifstream */
#include <type_traits> /* std::remove_cv_t or std::remove_reference_t */
#include <optional>    /* std::optional */

#ifndef __cpp_lib_remove_cvref
namespace std {


template <class T>
struct remove_cvref {
    typedef remove_cv_t<std::remove_reference_t<T>> type;
};


} // namespace std
#endif


namespace rcppsimdjson {
namespace utils {

// options for returning big-ints
enum class Int64_R_Type : int {
    Double    = 0,
    String    = 1,
    Integer64 = 2,
    Always    = 3,
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
    if constexpr (int64_opt == Int64_R_Type::Always) {
        return as_integer64(x);
    } else {
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
}
// Convert `std::vector<int64_t>` to `SEXP`.
// - returns an `Rcpp::IntegerVector` if `x` can be safely downcasted
// - otherwise `x` is coerced to an `Rcpp::Vector` following the provided `Int64_R_Type`
template <Int64_R_Type int64_opt>
inline SEXP resolve_int64(const std::vector<int64_t>& x) {
    if constexpr (int64_opt == Int64_R_Type::Always) {
        return as_integer64(x);
    } else {
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


inline constexpr std::optional<std::string_view>
get_memDecompress_type(const std::string_view& file_path) {
    if (const auto dot = std::string_view(file_path).rfind('.'); dot != std::string_view::npos) {
        if (const auto ext = file_path.substr(dot + 1); std::size(ext) >= 2) {
            if (ext == "gz") {
                return "gzip";
            }
            if (ext == "xz") {
                return "xz";
            }
            if (ext == "bz" || ext == "bz2") {
                return "bzip2";
            }
        }
    }
    return std::nullopt;
}
static_assert(get_memDecompress_type("test.gz") == "gzip");
static_assert(get_memDecompress_type("test.xz") == "xz");
static_assert(get_memDecompress_type("test.bz") == "bzip2");
static_assert(get_memDecompress_type("test.bz2") == "bzip2");
static_assert(get_memDecompress_type("no-file-ext") == std::nullopt);
static_assert(get_memDecompress_type("no-file-ext.badext") == std::nullopt);


template <typename T1, typename T2>
inline constexpr bool is_same_ish() noexcept {
    return std::is_same_v<std::remove_cv_t<T1>, std::remove_cv_t<T2>>;
}
template <typename T>
inline constexpr bool resembles_r_string() noexcept {
    return is_same_ish<T, Rcpp::String>() || is_same_ish<T, Rcpp::String::const_StringProxy>() ||
           is_same_ish<T, Rcpp::String::StringProxy>();
}
template <typename T>
inline constexpr bool resembles_vec_raw() noexcept {
    return is_same_ish<T, Rcpp::ChildVector<Rcpp::RawVector>>() ||
           is_same_ish<T, Rcpp::RawVector>() ||
           is_same_ish<T, Rcpp::Vector<RAWSXP, Rcpp::PreserveStorage>>();
}
template <typename T>
inline constexpr bool resembles_vec_chr() noexcept {
    return is_same_ish<T, Rcpp::ChildVector<Rcpp::CharacterVector>>() ||
           is_same_ish<T, Rcpp::CharacterVector>() ||
           is_same_ish<T, Rcpp::Vector<STRSXP, Rcpp::PreserveStorage>>();
}


template <typename T>
inline constexpr bool is_na_string(const T& x) {
    if constexpr (resembles_r_string<T>()) {
        return x == NA_STRING;
    } else if constexpr (resembles_vec_chr<T>()) {
        return x[0].get() == NA_STRING;
    } else {
        return false;
    }
}


template <typename file_path_T>
inline Rcpp::RawVector decompress(const file_path_T& file_path, const Rcpp::String& file_type) {
    std::ifstream stream(file_path, std::ios::binary | std::ios::ate);
    if (!stream) {
      Rcpp::stop("There's a problem with this file:\n\t-%s", file_path);		// #nocov
    }

    const auto end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    const std::size_t n(end - stream.tellg());
    if (n == 0) { /* avoid undefined behavior */
        return Rcpp::RawVector(1); // # nocov 
    }

    Rcpp::RawVector buffer(n);
    stream.read(reinterpret_cast<char*>(&(buffer[0])), n);

    return Rcpp::Function("memDecompress")(buffer, file_type, false);
}


inline constexpr std::optional<std::string_view> get_url_prefix(const std::string_view& str) {
    if (std::size(str) > 8) {
        if (const auto prefix = std::string_view(str).substr(0, 8); prefix == "https://") {
            return prefix;
        } else if (const auto prefix = std::string_view(str).substr(0, 7);
                   prefix == "http://" || prefix == "ftps://" || prefix == "file://") {
            return prefix;
        } else if (const auto prefix = std::string_view(str).substr(0, 6); prefix == "ftp://") {
            return prefix; // # nocov 
        }
    }
    return std::nullopt;
}
static_assert(get_url_prefix("https://test.com") == "https://");
static_assert(get_url_prefix("http://test.com") == "http://");
static_assert(get_url_prefix("ftps://test.com") == "ftps://");
static_assert(get_url_prefix("file:///test.com") == "file://");
static_assert(get_url_prefix("ftp://test.com") == "ftp://");
static_assert(get_url_prefix("bad12://test.com") == std::nullopt);
static_assert(get_url_prefix("no-prefix.com") == std::nullopt);
static_assert(get_url_prefix("short") == std::nullopt);


inline constexpr std::optional<std::string_view> get_file_ext(const std::string_view& str) {
    if (const auto dot = str.rfind('.'); dot != std::string_view::npos) {
        if (const auto out = str.substr(dot); /* `with_dot ? dot : dot + 1` */
            /* if the file path is a URL without an extension, we need to ensure that that we don't
             * extract everything after the domain by checking for '/'
             */
            out.find("/") == std::string_view::npos) {
            return str.substr(dot);
        }
    }
    return std::nullopt;
}
static_assert(get_file_ext("ftp://test.com/no-file-ext") == std::nullopt);
static_assert(get_file_ext("ftp://test.com/test.json") == ".json");
static_assert(get_file_ext("ftp://test.com/test.json.gz") == ".gz");


inline bool is_named(SEXP x) {
    return !Rf_isNull(Rf_getAttrib(x, R_NamesSymbol)) ||
           Rf_xlength(Rf_getAttrib(x, R_NamesSymbol)) != 0;
}


inline bool is_single_json_arg(SEXP json) {
    return (TYPEOF(json) == RAWSXP || (TYPEOF(json) == STRSXP && Rf_xlength(json) == 1));
}


inline bool is_single_query_arg(SEXP query) {
    return TYPEOF(query) == STRSXP && Rf_xlength(query) == 1;
}


} // namespace utils
} // namespace rcppsimdjson

#endif
