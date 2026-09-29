#ifndef RCPPSIMDJSON_COMMON_HPP
#define RCPPSIMDJSON_COMMON_HPP


#define STRICT_R_HEADERS
#include <Rcpp.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>


namespace rcppsimdjson {

/**
 * @brief A container's size as an R_xlen_t . Otherwise Equivalent to std::size() .
 */
template <typename _Container>
inline constexpr auto r_length(const _Container& __cont) noexcept -> R_xlen_t {
    return static_cast<R_xlen_t>(std::size(__cont));
}


/**
 * @brief A bit64::integer64 -compatibleN A .
 */
static inline constexpr int64_t NA_INTEGER64 = LLONG_MIN;


/**
 * @brief Typing arguments that decide how a simdjson::dom::element is ultimately returned to R.
 */
enum class rcpp_T : int {
    array  = 0, /**< recursive: individual elements will decide ultimate R type */
    object = 1, /**< recursive: individual elements will decide ultimate R type */
    chr    = 2, /**< always becomes Rcpp::String /character */
    u64    = 3, /**< always becomes Rcpp::String /character */
    dbl    = 4, /**< always becomes double */
    i64    = 5, /**< follows Int64_R_Type: double, character, or bit64::integer64 */
    i32    = 6, /**< always becomes int */
    lgl    = 7, /**< always becomes bool /logical */
    null   = 8, /**< becomes NA if returned in a vector, else NULL */
};


/**
 * @brief Get a typed NA .
 */
template <rcpp_T R_Type>
inline constexpr auto na_val() {
    if constexpr (R_Type == rcpp_T::chr) {
        return NA_STRING;
    }
    if constexpr (R_Type == rcpp_T::dbl) {
        return NA_REAL;
    }
    if constexpr (R_Type == rcpp_T::i64) {
        return NA_INTEGER64;
    }
    if constexpr (R_Type == rcpp_T::i32) {
        return NA_INTEGER;
    }
    if constexpr (R_Type == rcpp_T::lgl) {
        return NA_LOGICAL;
    }
}


/**
 * Internal flags tracking whether simdjson is compiled with exceptions enabled (the
 * default). If simdjson is compiled w/o exceptions (#define SIMDJSON_EXCEPTIONS 0),
 * operations that do not touch throwing code can be annotated with keyword
 * noexcept where appropriate.
 */

// #define DISABLE_SIMDJSON_EXCEPTIONS 1 /* uncomment to disable simdjson exceptions */

#if __GNUC__ && __GNUC__ <= 7
#    define RCPPSIMDJSON_IS_GCC_7 1
#endif

#if DISABLE_SIMDJSON_EXCEPTIONS && !RCPPSIMDJSON_IS_GCC_7
#    define SIMDJSON_EXCEPTIONS 0
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = true;
#else
#    define SIMDJSON_EXCEPTIONS 1
static inline constexpr auto RCPPSIMDJSON_NO_EXCEPTIONS = false;
#endif
static inline constexpr auto RCPPSIMDJSON_EXCEPTIONS = !RCPPSIMDJSON_NO_EXCEPTIONS;


/**
 * @brief Whether a function is noexcept.
 *
 * If a function does not touch throwing code it can be annotated with noexcept().
 * If RCPPSIMDJSON_NO_EXCEPTIONS is enabled and the rcpp_T template argument is not
 * rcpp_T::chr, functions annotated with noexcept(noxcpt<rcpp_T>()) will be noexcept
 * when compiled.
 *
 * Currently,rccp_T::chr touches throwing code so functions using it will always be
 * noexcept(false).
 *
 * Many examples in @file{inst/include/RcppSimdJson/deserialize/scalar.hpp}.
 */

template <rcpp_T R_Type>
inline constexpr auto noxcpt() -> bool {
    return RCPPSIMDJSON_NO_EXCEPTIONS && R_Type != rcpp_T::chr;
}
template <int RTYPE>
inline constexpr auto noxcpt() -> bool {
    return RCPPSIMDJSON_NO_EXCEPTIONS && RTYPE != STRSXP;
}


namespace deserialize {


/**
 * @brief Determines level of type strictness in combining array elements into R vectors.
 *
 * When arrays are not homogeneous and Type_Policy::anything_goes is used, type promotion follows
 * R's behavior.
 */
enum class Type_Policy : int {
    anything_goes = 0, /* Non-recursive arrays always become vectors of the highest present type */
    ints_as_dbls  = 1, /* Non-recursive arrays of only numbers are promoted to highest type */
    strict        = 2, /* No type promotion */
};


/**
 * @brief Maximum simplification level.
 */
enum class Simplify_To : int {
    data_frame = 0, /* If possible, return dataframes. Otherwise return matrices/vectors/lists. */
    matrix     = 1, /* If possible, return matrices. Otherwise return vectors/lists. */
    vector     = 2, /* If possible, return vectors. Otherwise return lists. */
    list       = 3, /* No simplification. */
};


} // namespace deserialize
} // namespace rcppsimdjson


#include "../simdjson.h"
#include "utils.hpp"


/*
 * simdjson's DOM accessors (element::type(), object/array iteration, ...) are plain `inline`
 * functions that compilers tend to leave out of line at -O2. On the tight, non-recursive loops that
 * visit every value, we ask for everything to be inlined.
 */
#if defined(__GNUC__) || defined(__clang__)
#    define RCPPSIMDJSON_FLATTEN __attribute__((flatten))
#    define RCPPSIMDJSON_NOINLINE __attribute__((noinline))
#else
#    define RCPPSIMDJSON_FLATTEN
#    define RCPPSIMDJSON_NOINLINE
#endif


namespace rcppsimdjson {

/**
 * @brief Make a UTF-8 CHARSXP directly from a view, skipping the std::string and Rcpp::String
 * temporaries (Rcpp::String also preserves/releases its SEXP, which is costly in hot loops).
 *
 * Like Rcpp::String, throws on an embedded nul.
 */
inline SEXP make_charsxp(std::string_view sv) {
    if (std::memchr(sv.data(), '\0', sv.size()) != nullptr) {
        throw Rcpp::embedded_nul_in_string();
    }
    return Rf_mkCharLenCE(sv.data(), static_cast<int>(sv.size()), CE_UTF8);
}


/*
 * JSON documents repeat the same short strings over and over (object keys, categories, flags...),
 * and looking each one up in R's global CHARSXP table is comparatively slow. We keep a small
 * direct-mapped cache of the CHARSXPs we made for short strings. The cache copies the bytes (the
 * parser's string buffer is overwritten by the next document) and keeps its CHARSXPs alive in a
 * preserved list, so an entry stays valid across calls. R calls us from a single thread.
 */
class Charsxp_Cache {
    static inline constexpr std::size_t N_SLOTS        = 1024;
    static inline constexpr std::size_t MAX_KEY_LENGTH = 24;

    struct Slot {
        unsigned char length = 0;
        char          bytes[MAX_KEY_LENGTH];
    };

    std::array<Slot, N_SLOTS> slots{};
    std::array<SEXP, N_SLOTS> values{};
    SEXP                      keep_alive = nullptr; /* VECSXP holding `values` */

    static inline auto hash(const std::string_view sv) noexcept -> std::size_t {
        auto h = std::uint32_t(2166136261U) ^ static_cast<std::uint32_t>(std::size(sv));
        for (const char c : sv) {
            h = (h ^ static_cast<unsigned char>(c)) * 16777619U;
        }
        return (h ^ (h >> 16)) % N_SLOTS;
    }

    static inline auto same(const Slot& slot, const std::string_view sv) noexcept -> bool {
        if (slot.length != std::size(sv)) {
            return false;
        }
        for (std::size_t i = 0; i < std::size(sv); ++i) {
            if (slot.bytes[i] != sv[i]) {
                return false;
            }
        }
        return true;
    }

  public:
    inline auto get(const std::string_view sv) -> SEXP {
        if (std::size(sv) > MAX_KEY_LENGTH) {
            return make_charsxp(sv);
        }
        const auto i = hash(sv);
        if (values[i] != nullptr && same(slots[i], sv)) {
            return values[i];
        }
        if (keep_alive == nullptr) {
            keep_alive = Rf_allocVector(VECSXP, N_SLOTS);
            R_PreserveObject(keep_alive);
        }
        SEXP charsxp = make_charsxp(sv);
        SET_VECTOR_ELT(keep_alive, static_cast<R_xlen_t>(i), charsxp);
        values[i]        = charsxp;
        slots[i].length  = static_cast<unsigned char>(std::size(sv));
        std::memcpy(slots[i].bytes, std::data(sv), std::size(sv));
        return charsxp;
    }

    static auto shared() -> Charsxp_Cache& {
        static Charsxp_Cache cache;
        return cache;
    }
};


/* Like make_charsxp(), through the shared Charsxp_Cache. */
inline SEXP make_charsxp_cached(std::string_view sv) { return Charsxp_Cache::shared().get(sv); }

} // namespace rcppsimdjson


namespace rcppsimdjson {
namespace deserialize {


/**
 * @brief Simplify asimdjson::dom::element to an R object.
 *
 * @note Forward declaration. See inst/include/RcppSimdJson/deserialize/simplify.hpp.
 */
template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto simplify_element(simdjson::dom::element element,
                             SEXP                   empty_array,
                             SEXP                   empty_object,
                             SEXP                   single_null) -> SEXP;


} // namespace deserialize
} // namespace rcppsimdjson


#endif
