#ifndef RCPPSIMDJSON__DESERIALIZE__SCALAR_HPP
#define RCPPSIMDJSON__DESERIALIZE__SCALAR_HPP

#include "Type_Doctor.hpp"

namespace rcppsimdjson {
namespace deserialize {


/*
 * Check for `null`s and return the appropriate `NA`s when found.
 */
static inline constexpr bool HAS_NULLS = true;
/*
 * No `null`s present, so skip checking for them.
 */
static inline constexpr bool NO_NULLS = false;


template <typename in_T, rcpp_T R_Type>
inline auto get_scalar_(simdjson::ondemand::value) noexcept(noxcpt<R_Type>());

template <typename in_T, rcpp_T R_Type, bool has_null>
inline auto get_scalar(simdjson::ondemand::value element) noexcept(noxcpt<R_Type>()) {
    if constexpr (has_null) {
        return element.is_null() ? na_val<R_Type>() : get_scalar_<in_T, R_Type>(element);
    } else {
        return get_scalar_<in_T, R_Type>(element);
    }
}


// bool ============================================================================================
// return Rcpp::String
template <>
inline auto
get_scalar_<bool, rcpp_T::chr>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::chr>()) {
    return bool(element) ? Rcpp::String("TRUE") : Rcpp::String("FALSE");
}
// return double
template <>
inline auto
get_scalar_<bool, rcpp_T::dbl>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::dbl>()) {
    return bool(element) ? 1.0 : 0.0;
}
// return int64_t
template <>
inline auto
get_scalar_<bool, rcpp_T::i64>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::i64>()) {
    return bool(element) ? static_cast<int64_t>(1LL) : static_cast<int64_t>(0LL);
}
// return int
template <>
inline auto
get_scalar_<bool, rcpp_T::i32>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::i32>()) {
    return bool(element) ? 1 : 0;
}
// return "bool"
template <>
inline auto
get_scalar_<bool, rcpp_T::lgl>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::lgl>()) {
    return bool(element);
}
// int64_t =========================================================================================
// return Rcpp::String
template <>
inline auto
get_scalar_<int64_t, rcpp_T::chr>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::chr>()) {
    return Rcpp::String(std::to_string(int64_t(element)));
}
// return double
template <>
inline auto
get_scalar_<int64_t, rcpp_T::dbl>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::dbl>()) {
    return double(element);
}
// return int64_t
template <>
inline auto
get_scalar_<int64_t, rcpp_T::i64>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::i64>()) {
    return int64_t(element);
}
// return int
template <>
inline auto
get_scalar_<int64_t, rcpp_T::i32>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::i32>()) {
    return static_cast<int>(int64_t(element));
}
// double ==========================================================================================
// return Rcpp::String
template <>
inline auto
get_scalar_<double, rcpp_T::chr>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::chr>()) {
    auto out = std::to_string(double(element));
    out.erase(out.find_last_not_of('0') + 2, std::string::npos);
    return Rcpp::String(out);
}
// return double
template <>
inline auto
get_scalar_<double, rcpp_T::dbl>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::dbl>()) {
    return double(element);
}
// std::string (really std::string_view) ===========================================================
// return Rcpp::String
template <>
inline auto get_scalar_<std::string, rcpp_T::chr>(simdjson::ondemand::value element) noexcept(
    noxcpt<rcpp_T::chr>()) {
    return Rcpp::String(std::string(std::string_view(element)));
}
// uint64_t ========================================================================================
// return Rcpp::String
template <>
inline auto
get_scalar_<uint64_t, rcpp_T::chr>(simdjson::ondemand::value element) noexcept(noxcpt<rcpp_T::chr>()) {
    return Rcpp::String(std::to_string(uint64_t(element)));
}
// dispatchers =====================================================================================
template <int RTYPE>
inline auto get_scalar_dispatch(simdjson::ondemand::value) noexcept(noxcpt<RTYPE>());

template <>
inline auto get_scalar_dispatch<STRSXP>(simdjson::ondemand::value element) noexcept(false) {
    switch (element.type()) {
        case simdjson::ondemand::json_type::string:
            return get_scalar<std::string, rcpp_T::chr, NO_NULLS>(element);

        case simdjson::ondemand::json_type::number:
            {
                simdjson::ondemand::number num = element.get_number();
                simdjson::ondemand::number_type t = num.get_number_type();
                switch (t) {
                    case simdjson::ondemand::number_type::signed_integer:
                        return get_scalar<int64_t, rcpp_T::chr, NO_NULLS>(element);
                    case simdjson::ondemand::number_type::unsigned_integer:
                        return get_scalar<uint64_t, rcpp_T::chr, NO_NULLS>(element);
                    case simdjson::ondemand::number_type::floating_point_number:
                        return get_scalar<double, rcpp_T::chr, NO_NULLS>(element);
                    default:
                        return Rcpp::String(NA_STRING);
                }
            }
            break;
        case simdjson::ondemand::json_type::boolean:
            return get_scalar<bool, rcpp_T::chr, NO_NULLS>(element);

        default:
            return Rcpp::String(NA_STRING);
    }
}


template <>
inline auto
get_scalar_dispatch<REALSXP>(simdjson::ondemand::value element) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS) {
    switch (element.type()) {
        case simdjson::ondemand::json_type::number:
            {
                simdjson::ondemand::number num = element.get_number();
                simdjson::ondemand::number_type t = num.get_number_type();
                switch (t) {
                    case simdjson::ondemand::number_type::signed_integer:
                        return get_scalar<int64_t, rcpp_T::dbl, NO_NULLS>(element);
                    case simdjson::ondemand::number_type::floating_point_number:
                        return get_scalar<double, rcpp_T::dbl, NO_NULLS>(element);
                    default:
                        return NA_REAL;
                }
            }
            break;
        case simdjson::ondemand::json_type::boolean:
            return get_scalar<bool, rcpp_T::dbl, NO_NULLS>(element);

        default:
            return NA_REAL;
    }
}


template <>
inline auto
get_scalar_dispatch<INTSXP>(simdjson::ondemand::value element) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS) {
    switch (element.type()) {
        case simdjson::ondemand::json_type::number:
            {
                simdjson::ondemand::number num = element.get_number();
                if (num.get_number_type() == simdjson::ondemand::number_type::signed_integer) {
                    return get_scalar<int64_t, rcpp_T::i32, NO_NULLS>(element);
                } else {
                    return NA_INTEGER;
                }
            }
            break;
        case simdjson::ondemand::json_type::boolean:
            return get_scalar<bool, rcpp_T::i32, HAS_NULLS>(element);

        default:
            return NA_INTEGER;
    }
}

// # nocov start
template <>
inline auto
get_scalar_dispatch<LGLSXP>(simdjson::ondemand::value element) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS) {
    switch (element.type()) {
        case simdjson::ondemand::json_type::boolean:
            return get_scalar<bool, rcpp_T::i32, NO_NULLS>(element);

        default:
            return NA_LOGICAL;
    }
}
// # nocov end

} // namespace deserialize
} // namespace rcppsimdjson


#endif
