#include <RcppSimdJson.hpp>


template <typename vec_T>
Rcpp::LogicalVector is_valid_json(const vec_T json) {
    simdjson::dom::parser p; // # nocov

    if constexpr (std::is_same_v<vec_T, Rcpp::RawVector>) {
        return p.parse(std::string_view(reinterpret_cast<const char*>(&(json[0])), std::size(json)))
                   .error() == simdjson::error_code::SUCCESS;
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::CharacterVector>) {
        return Rcpp::LogicalVector(
            std::cbegin(json), std::cend(json), [&p](const decltype(json[0]) x) -> bool {
                return p.parse(std::string_view(x)).error() == simdjson::error_code::SUCCESS;
            });
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::ListOf<Rcpp::RawVector>>) {
        return Rcpp::LogicalVector(
            std::cbegin(json), std::cend(json), [&p](const Rcpp::RawVector& x) -> bool {
                return p.parse(
                            std::string_view(reinterpret_cast<const char*>(&(x[0])), std::size(x)))
                           .error() == simdjson::error_code::SUCCESS;
            });
    }
}
//' @rdname simdjson-utilities
//' @export
// [[Rcpp::export(is_valid_json)]]
Rcpp::LogicalVector dispatch_is_valid_json(SEXP json) {
    switch (TYPEOF(json)) {
        case STRSXP:
            return is_valid_json<Rcpp::CharacterVector>(json);

        case RAWSXP:
            return is_valid_json<Rcpp::RawVector>(json);

        case VECSXP:
            return is_valid_json<Rcpp::ListOf<Rcpp::RawVector>>(json);

        default:
            Rcpp::stop("invalid input");
    }
}


template <typename vec_T>
Rcpp::LogicalVector is_valid_utf8(const vec_T x) {
    if constexpr (std::is_same_v<vec_T, Rcpp::RawVector>) {
        return simdjson::validate_utf8(
            std::string_view(reinterpret_cast<const char*>(&(x[0])), std::size(x)));
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::CharacterVector>) {
        return Rcpp::LogicalVector(std::cbegin(x), std::cend(x), [](const auto& val) -> bool {
            return simdjson::validate_utf8(std::string_view(val));
        });
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::ListOf<Rcpp::RawVector>>) {
        return Rcpp::LogicalVector(
            std::cbegin(x), std::cend(x), [](const Rcpp::RawVector& val) -> bool {
                if constexpr (std::is_same_v<vec_T, Rcpp::ListOf<Rcpp::RawVector>>) {
                    return simdjson::validate_utf8(
                        std::string_view(reinterpret_cast<const char*>(&(val[0])), std::size(val)));
                    ;
                }
            });
    }
}
//' @rdname simdjson-utilities
//' @export
// [[Rcpp::export(is_valid_utf8)]]
Rcpp::LogicalVector dispatch_is_valid_utf8(SEXP x) {
    switch (TYPEOF(x)) {
        case STRSXP:
            return is_valid_utf8<Rcpp::CharacterVector>(x);

        case RAWSXP:
            return is_valid_utf8<Rcpp::RawVector>(x);

        case VECSXP:
            return is_valid_utf8<Rcpp::ListOf<Rcpp::RawVector>>(x);

        default:
            Rcpp::stop("invalid input");
    }
}


template <typename vec_T>
Rcpp::CharacterVector fminify(const vec_T json) {
    simdjson::dom::parser p;

    if constexpr (std::is_same_v<vec_T, Rcpp::RawVector>) {
        if (auto [parsed, error] = p.parse(
                std::string_view(reinterpret_cast<const char*>(&(json[0])), std::size(json)));
            !error) {
            return simdjson::minify(parsed);
        }
        return Rcpp::CharacterVector::create(NA_STRING);
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::CharacterVector>) {
        return Rcpp::CharacterVector(
            std::cbegin(json), std::cend(json), [&p](const decltype(json[0]) val) -> Rcpp::String {
                if (auto [parsed, error] = p.parse(std::string_view(val)); !error) {
                    return simdjson::minify(parsed);
                }
                return NA_STRING;
            });
    }


    if constexpr (std::is_same_v<vec_T, Rcpp::ListOf<Rcpp::RawVector>>) {
        return Rcpp::CharacterVector(
            std::cbegin(json), std::cend(json), [&p](const Rcpp::RawVector val) -> Rcpp::String {
                if (auto [parsed, error] = p.parse(
                        std::string_view(reinterpret_cast<const char*>(&(val[0])), std::size(val)));
                    !error) {
                    return simdjson::minify(parsed);
                }
                return NA_STRING;
            });
    }
}
//' @rdname simdjson-utilities
//' @export
// [[Rcpp::export(fminify)]]
Rcpp::CharacterVector dispatch_fminify(SEXP json) {
    switch (TYPEOF(json)) {
        case STRSXP:
            return fminify<Rcpp::CharacterVector>(json);

        case RAWSXP:
            return fminify<Rcpp::RawVector>(json);

        case VECSXP:
            return fminify<Rcpp::ListOf<Rcpp::RawVector>>(json);

        default:
            Rcpp::stop("invalid input");
    }
}
