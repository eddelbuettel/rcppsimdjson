#if __cplusplus >= 201703L
#    include <RcppSimdJson.hpp>
#endif

// [[Rcpp::interfaces(r, cpp)]]

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


// # nocov start
// [[Rcpp::export(.exceptions_enabled)]]
bool exceptions_enabled() {
#ifndef SIMDJSON_EXCEPTIONS
    Rcpp::stop("`SIMDJSON_EXCEPTIONS` not defined");
#endif
    return SIMDJSON_EXCEPTIONS == 1;
}
// # nocov end
