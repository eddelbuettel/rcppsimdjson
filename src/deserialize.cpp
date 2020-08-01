// #define RCPPSIMDJSON_DEBUG 1

#include <RcppSimdJson.hpp>


// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize(SEXP       json,
                 SEXP       query          = R_NilValue,
                 SEXP       empty_array    = R_NilValue,
                 SEXP       empty_object   = R_NilValue,
                 SEXP       single_null    = R_NilValue,
                 const bool parse_error_ok = false,
                 SEXP       on_parse_error = R_NilValue,
                 const bool query_error_ok = false,
                 SEXP       on_query_error = R_NilValue,
                 const int  simplify_to    = 0,
                 const int  type_policy    = 0,
                 const int  int64_r_type   = 0) {

    using namespace rcppsimdjson;

    if (utils::is_single_json_arg(json)) {
        return utils::is_single_query_arg(query)
                   ? deserialize::start<deserialize::IS_NOT_FILE,
                                        deserialize::SINGLE_JSON,
                                        deserialize::SINGLE_QUERY>(json,
                                                                   query,
                                                                   empty_array,
                                                                   empty_object,
                                                                   single_null,
                                                                   parse_error_ok,
                                                                   on_parse_error,
                                                                   query_error_ok,
                                                                   on_query_error,
                                                                   simplify_to,
                                                                   type_policy,
                                                                   int64_r_type)
                   : deserialize::start<deserialize::IS_NOT_FILE,
                                        deserialize::SINGLE_JSON,
                                        deserialize::NOT_SINGLE_QUERY>(json,
                                                                       query,
                                                                       empty_array,
                                                                       empty_object,
                                                                       single_null,
                                                                       parse_error_ok,
                                                                       on_parse_error,
                                                                       query_error_ok,
                                                                       on_query_error,
                                                                       simplify_to,
                                                                       type_policy,
                                                                       int64_r_type);
    } else { /* !is_single_json */
        return utils::is_single_query_arg(query)
                   ? deserialize::start<deserialize::IS_NOT_FILE,
                                        deserialize::NOT_SINGLE_JSON,
                                        deserialize::SINGLE_QUERY>(json,
                                                                   query,
                                                                   empty_array,
                                                                   empty_object,
                                                                   single_null,
                                                                   parse_error_ok,
                                                                   on_parse_error,
                                                                   query_error_ok,
                                                                   on_query_error,
                                                                   simplify_to,
                                                                   type_policy,
                                                                   int64_r_type)
                   : deserialize::start<deserialize::IS_NOT_FILE,
                                        deserialize::NOT_SINGLE_JSON,
                                        deserialize::NOT_SINGLE_QUERY>(json,
                                                                       query,
                                                                       empty_array,
                                                                       empty_object,
                                                                       single_null,
                                                                       parse_error_ok,
                                                                       on_parse_error,
                                                                       query_error_ok,
                                                                       on_query_error,
                                                                       simplify_to,
                                                                       type_policy,
                                                                       int64_r_type);
    }
}


// [[Rcpp::export(.load_json)]]
SEXP load(const Rcpp::CharacterVector& json,
          SEXP                         query          = R_NilValue,
          SEXP                         empty_array    = R_NilValue,
          SEXP                         empty_object   = R_NilValue,
          SEXP                         single_null    = R_NilValue,
          const bool                   parse_error_ok = false,
          SEXP                         on_parse_error = R_NilValue,
          const bool                   query_error_ok = false,
          SEXP                         on_query_error = R_NilValue,
          const int                    simplify_to    = 0,
          const int                    type_policy    = 0,
          const int                    int64_r_type   = 0) {
    using namespace rcppsimdjson;

    if (utils::is_single_json_arg(json)) {
        return utils::is_single_query_arg(query)
                   ? deserialize::start<deserialize::IS_FILE,
                                        deserialize::SINGLE_JSON,
                                        deserialize::SINGLE_QUERY>(json,
                                                                   query,
                                                                   empty_array,
                                                                   empty_object,
                                                                   single_null,
                                                                   parse_error_ok,
                                                                   on_parse_error,
                                                                   query_error_ok,
                                                                   on_query_error,
                                                                   simplify_to,
                                                                   type_policy,
                                                                   int64_r_type)
                   : deserialize::start<deserialize::IS_FILE,
                                        deserialize::SINGLE_JSON,
                                        deserialize::NOT_SINGLE_QUERY>(json,
                                                                       query,
                                                                       empty_array,
                                                                       empty_object,
                                                                       single_null,
                                                                       parse_error_ok,
                                                                       on_parse_error,
                                                                       query_error_ok,
                                                                       on_query_error,
                                                                       simplify_to,
                                                                       type_policy,
                                                                       int64_r_type);
    } else { /* !is_single_json */
        return utils::is_single_query_arg(query)
                   ? deserialize::start<deserialize::IS_FILE,
                                        deserialize::NOT_SINGLE_JSON,
                                        deserialize::SINGLE_QUERY>(json,
                                                                   query,
                                                                   empty_array,
                                                                   empty_object,
                                                                   single_null,
                                                                   parse_error_ok,
                                                                   on_parse_error,
                                                                   query_error_ok,
                                                                   on_query_error,
                                                                   simplify_to,
                                                                   type_policy,
                                                                   int64_r_type)
                   : deserialize::start<deserialize::IS_FILE,
                                        deserialize::NOT_SINGLE_JSON,
                                        deserialize::NOT_SINGLE_QUERY>(json,
                                                                       query,
                                                                       empty_array,
                                                                       empty_object,
                                                                       single_null,
                                                                       parse_error_ok,
                                                                       on_parse_error,
                                                                       query_error_ok,
                                                                       on_query_error,
                                                                       simplify_to,
                                                                       type_policy,
                                                                       int64_r_type);
    }
}


// [[Rcpp::export(.is_valid_json_arg)]]
bool is_valid_json_arg(SEXP json) {
    switch (TYPEOF(json)) {
        case STRSXP:
            [[fallthrough]];
        case RAWSXP:
            return Rf_xlength(json) > 0;

        case VECSXP: {
            for (auto&& element : Rcpp::List(json)) {
                if (TYPEOF(element) != RAWSXP) {
                    return false;
                }
            }
            return true;
        }

        default:
            return false;
    }
}


// [[Rcpp::export(.is_valid_query_arg)]]
bool is_valid_query_arg(SEXP query) {
    switch (TYPEOF(query)) {
        case NILSXP:
            return true;

        case STRSXP:
            return Rf_xlength(query) > 0;

        case VECSXP: {
            if (Rf_xlength(query) == 0) {
                return false;
            } else {
                for (auto&& element : Rcpp::List(query)) {
                    if (TYPEOF(element) != STRSXP) {
                        return false;
                    }
                }
                return true;
            }
        }

        default:
            return false;
    }
}


// [[Rcpp::export(.diagnose_input)]]
SEXP diagnose_input(const Rcpp::CharacterVector& x) {
    const R_xlen_t n = std::size(x);

    Rcpp::CharacterVector url_prefix(n, NA_STRING);
    Rcpp::CharacterVector file_ext(n);
    Rcpp::CharacterVector type(n, NA_STRING);
    Rcpp::LogicalVector   is_from_url(n, false);
    Rcpp::LogicalVector   is_local_file_url(n, false);

    for (R_xlen_t i = 0; i < n; ++i) {
        if (x[i].get() != NA_STRING) {
            const auto str = std::string_view(x[i]);
            if (const auto a_url_prefix = rcppsimdjson::utils::get_url_prefix(str)) {
                url_prefix[i]        = Rcpp::String(std::string(*a_url_prefix));
                is_from_url[i]       = true;
                is_local_file_url[i] = a_url_prefix == "file://";
            }
            if (const auto a_file_ext = rcppsimdjson::utils::get_file_ext(str)) {
                file_ext[i] = Rcpp::String(std::string(*a_file_ext));
            }
        }
    }

    using Rcpp::_;
    Rcpp::List out = Rcpp::List::create(_["input"]             = x,
                                        _["url_prefix"]        = url_prefix,
                                        _["file_ext"]          = file_ext,
                                        _["is_from_url"]       = is_from_url,
                                        _["is_local_file_url"] = is_local_file_url);

    out.attr("class")     = "data.frame";
    out.attr("row.names") = Rcpp::seq_len(n);
    return out;
}


// # nocov start
// [[Rcpp::export(.exceptions_enabled)]]
bool exceptions_enabled() {
#ifndef SIMDJSON_EXCEPTIONS
    Rcpp::stop("`SIMDJSON_EXCEPTIONS` not defined");
#endif
    return SIMDJSON_EXCEPTIONS == 1;
}
// # nocov end
