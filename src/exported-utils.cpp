#include <RcppSimdJson.hpp>


template <typename vec_T>
Rcpp::LogicalVector is_valid_json(const vec_T json) {
    simdjson::ondemand::parser p; // # nocov

    if constexpr (std::is_same_v<vec_T, Rcpp::RawVector>) {
        simdjson::padded_string padded = simdjson::padded_string(std::string_view(reinterpret_cast<const char*>(&(json[0])), std::size(json)));
        return p.iterate(padded)
                   .error() == simdjson::error_code::SUCCESS;
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::CharacterVector>) {
        return Rcpp::LogicalVector(
            std::cbegin(json), std::cend(json), [&p](const decltype(json[0]) x) {
                simdjson::padded_string padded =simdjson::padded_string(std::string_view(x));
                return x == NA_STRING
                           ? NA_LOGICAL
                           : p.iterate(padded).error() == simdjson::error_code::SUCCESS;
            });
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::ListOf<Rcpp::RawVector>>) {
        return Rcpp::LogicalVector(
            std::cbegin(json), std::cend(json), [&p](const Rcpp::RawVector& x) -> bool {
                simdjson::padded_string padded = simdjson::padded_string(std::string_view(reinterpret_cast<const char*>(&(x[0])), std::size(x)));
                return p.iterate(padded)
                           .error() == simdjson::error_code::SUCCESS;
            });
    }
}
//' @rdname simdjson-utilities
//' @export
// [[Rcpp::export(is_valid_json)]]
Rcpp::LogicalVector dispatch_is_valid_json(SEXP json) {
    if (Rf_xlength(json) == 0) {
        Rcpp::stop("`json=` must be a non-empty character vector, raw vector, or a list containing "
                   "raw vectors.");
    }

    switch (TYPEOF(json)) {
        case STRSXP: {
            auto out          = is_valid_json<Rcpp::CharacterVector>(json);
            out.attr("names") = Rf_getAttrib(json, R_NamesSymbol);
            return out;
        }

        case RAWSXP: {
            auto out          = is_valid_json<Rcpp::RawVector>(json);
            out.attr("names") = Rf_getAttrib(json, R_NamesSymbol);
            return out;
        }

        case VECSXP: {
            for (auto&& element : Rcpp::List(json)) {
                if (TYPEOF(element) != RAWSXP || Rf_xlength(element) == 0) {
                    Rcpp::stop(
                        "If `json=` is a `list`, it should only contain non-empty raw vectors.");
                }
            }
            auto out          = is_valid_json<Rcpp::ListOf<Rcpp::RawVector>>(json);
            out.attr("names") = Rf_getAttrib(json, R_NamesSymbol);
            return out;
        }

        default:
            Rcpp::stop("`json=` must be a non-empty character vector, raw vector, or a list "
                       "containing raw vectors.");
    }
}


template <typename vec_T>
Rcpp::LogicalVector is_valid_utf8(const vec_T x) {
    if constexpr (std::is_same_v<vec_T, Rcpp::RawVector>) {
        return simdjson::validate_utf8(
            std::string_view(reinterpret_cast<const char*>(&(x[0])), std::size(x)));
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::CharacterVector>) {
        return Rcpp::LogicalVector(std::cbegin(x), std::cend(x), [](const auto& val) {
            return val == NA_STRING ? NA_LOGICAL : simdjson::validate_utf8(std::string_view(val));
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
    if (Rf_xlength(x) == 0) {
        Rcpp::stop("`x=` must be a non-empty character vector, raw vector, or a list containing "
                   "raw vectors.");
    }

    switch (TYPEOF(x)) {
        case STRSXP: {
            auto out          = is_valid_utf8<Rcpp::CharacterVector>(x);
            out.attr("names") = Rf_getAttrib(x, R_NamesSymbol);
            return out;
        }

        case RAWSXP: {
            auto out          = is_valid_utf8<Rcpp::RawVector>(x);
            out.attr("names") = Rf_getAttrib(x, R_NamesSymbol);
            return out;
        }

        case VECSXP: {
            for (auto&& element : Rcpp::List(x)) {
                if (TYPEOF(element) != RAWSXP || Rf_xlength(element) == 0) {
                    Rcpp::stop(
                        "If `x=` is a `list`, it should only contain non-empty raw vectors.");
                }
            }
            auto out          = is_valid_utf8<Rcpp::ListOf<Rcpp::RawVector>>(x);
            out.attr("names") = Rf_getAttrib(x, R_NamesSymbol);
            return out;
        }

        default:
            Rcpp::stop("`json=` must be a non-empty character vector, raw vector, or a list "
                       "containing raw vectors.");
    }
}


template <typename vec_T>
Rcpp::CharacterVector fminify(const vec_T json) {
    simdjson::dom::parser p;

    if constexpr (std::is_same_v<vec_T, Rcpp::RawVector>) {
        simdjson::dom::element parsed;
        auto error = p.parse(
            std::string_view(reinterpret_cast<const char*>(&(json[0])), std::size(json))).get(parsed);
        if (!error) {
            return simdjson::minify(parsed);
        }
        return Rcpp::CharacterVector::create(NA_STRING);
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::CharacterVector>) {
        return Rcpp::CharacterVector(
            std::cbegin(json), std::cend(json), [&p](const decltype(json[0]) val) -> Rcpp::String {
                if (val != NA_STRING) {
                    simdjson::dom::element parsed;
                    auto error = p.parse(std::string_view(val)).get(parsed);
                    if (!error) {
                        return simdjson::minify(parsed);
                    }
                }
                return NA_STRING;
            });
    }

    if constexpr (std::is_same_v<vec_T, Rcpp::ListOf<Rcpp::RawVector>>) {
        return Rcpp::CharacterVector(
            std::cbegin(json), std::cend(json), [&p](const Rcpp::RawVector val) -> Rcpp::String {
                simdjson::dom::element parsed;
                auto error = p.parse(
                    std::string_view(reinterpret_cast<const char*>(&(val[0])), std::size(val))).get(parsed);
                if (!error) {
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
    if (Rf_xlength(json) == 0) {
        Rcpp::stop("`json=` must be a non-empty character vector, raw vector, or a list containing "
                   "raw vectors.");
    }

    switch (TYPEOF(json)) {
        case STRSXP: {
            auto out          = fminify<Rcpp::CharacterVector>(json);
            out.attr("names") = Rf_getAttrib(json, R_NamesSymbol);
            return out;
        }

        case RAWSXP: {
            auto out          = fminify<Rcpp::RawVector>(json);
            out.attr("names") = Rf_getAttrib(json, R_NamesSymbol);
            return out;
        }

        case VECSXP: {
            for (auto&& element : Rcpp::List(json)) {
                if (TYPEOF(element) != RAWSXP || Rf_xlength(element) == 0) {
                    Rcpp::stop(
                        "If `json=` is a `list`, it should only contain non-empty raw vectors.");
                }
            }
            auto out          = fminify<Rcpp::ListOf<Rcpp::RawVector>>(json);
            out.attr("names") = Rf_getAttrib(json, R_NamesSymbol);
            return out;
        }

        default:
            Rcpp::stop("`json=` must be a non-empty character vector, raw vector, or a list "
                       "containing raw vectors.");
    }
}
