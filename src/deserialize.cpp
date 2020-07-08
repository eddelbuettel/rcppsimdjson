#include <RcppSimdJson.hpp>


template <bool error_ok>
SEXP deserialize_single_raw(const Rcpp::RawVector&                       json,
                            const std::string&                           json_pointer,
                            SEXP                                         empty_array,
                            SEXP                                         empty_object,
                            SEXP                                         single_null,
                            SEXP                                         on_error,
                            const rcppsimdjson::deserialize::Simplify_To simplify_to,
                            const rcppsimdjson::deserialize::Type_Policy type_policy,
                            const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {
    using namespace rcppsimdjson;

    simdjson::dom::parser parser;
    auto [parsed, error] =
        json_pointer.empty()
            ? parser.parse(std::string(std::cbegin(json), std::cend(json)))
            : parser.parse(std::string(std::cbegin(json), std::cend(json))).at(json_pointer);

    if (error) {
        if constexpr (error_ok) {
            return on_error;
        } else {
            Rcpp::stop(simdjson::error_message(error));
        }
    }

    return deserialize::deserialize(parsed, //
                                    empty_array,
                                    empty_object,
                                    single_null,
                                    simplify_to,
                                    type_policy,
                                    int64_r_type);
}

template <bool error_ok>
SEXP deserialize_multiple_raw(const Rcpp::ListOf<Rcpp::RawVector>&         json,
                              const std::string&                           json_pointer,
                              SEXP                                         empty_array,
                              SEXP                                         empty_object,
                              SEXP                                         single_null,
                              SEXP                                         on_error,
                              const rcppsimdjson::deserialize::Simplify_To simplify_to,
                              const rcppsimdjson::deserialize::Type_Policy type_policy,
                              const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {
    using namespace rcppsimdjson;

    const auto n = std::size(json);
    Rcpp::List out(n);

    simdjson::dom::parser parser;
    for (R_xlen_t i = 0; i < n; ++i) {
        out[i] = deserialize_single_raw<error_ok>(json[i],
                                                  json_pointer,
                                                  empty_array,
                                                  empty_object,
                                                  single_null,
                                                  on_error,
                                                  simplify_to,
                                                  type_policy,
                                                  int64_r_type);
    }

    out.attr("names") = json.attr("names");
    return out;
}


template <bool error_ok>
SEXP deserialize_single_chr(const Rcpp::CharacterVector&                 json,
                            const std::string&                           json_pointer,
                            SEXP                                         empty_array,
                            SEXP                                         empty_object,
                            SEXP                                         single_null,
                            SEXP                                         on_error,
                            const rcppsimdjson::deserialize::Simplify_To simplify_to,
                            const rcppsimdjson::deserialize::Type_Policy type_policy,
                            const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {
    using namespace rcppsimdjson;

    if (json[0].get() == NA_STRING) {
        return Rcpp::CharacterVector::create(NA_STRING);
    }

    simdjson::dom::parser parser;
    auto [parsed, error] = json_pointer.empty()
                               ? parser.parse(std::string_view(json[0]))
                               : parser.parse(std::string_view(json[0])).at(json_pointer);

    if (error) {
        if constexpr (error_ok) {
            return on_error;
        } else {
            Rcpp::stop(simdjson::error_message(error));
        }
    }

    return deserialize::deserialize(parsed, //
                                    empty_array,
                                    empty_object,
                                    single_null,
                                    simplify_to,
                                    type_policy,
                                    int64_r_type);
}


template <bool error_ok>
SEXP deserialize_multiple_chr(const Rcpp::CharacterVector&                 json,
                              const std::string&                           json_pointer,
                              SEXP                                         empty_array,
                              SEXP                                         empty_object,
                              SEXP                                         single_null,
                              SEXP                                         on_error,
                              const rcppsimdjson::deserialize::Simplify_To simplify_to,
                              const rcppsimdjson::deserialize::Type_Policy type_policy,
                              const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {
    using namespace rcppsimdjson;

    const auto n = json.length();
    Rcpp::List out(n);

    simdjson::dom::parser parser;
    for (R_xlen_t i = 0; i < n; ++i) {
        if (json[i].get() == NA_STRING) {
            out[i] = Rcpp::CharacterVector::create(NA_STRING);
            continue;
        }

        auto [parsed, error] = json_pointer.empty()
                                   ? parser.parse(std::string_view(json[i]))
                                   : parser.parse(std::string_view(json[i])).at(json_pointer);
        if (error) {
            if constexpr (error_ok) {
                out[i] = on_error;
                continue;
            } else {
                Rcpp::stop(simdjson::error_message(error));
            }
        }

        out[i] = deserialize::deserialize(parsed, //
                                          empty_array,
                                          empty_object,
                                          single_null,
                                          simplify_to,
                                          type_policy,
                                          int64_r_type);
    }

    out.attr("names") = json.attr("names");
    return out;
}


template <int RTYPE>
SEXP dispatch_deserialize(const Rcpp::Vector<RTYPE>&                   json,
                          const std::string&                           json_pointer,
                          SEXP                                         empty_array,
                          SEXP                                         empty_object,
                          SEXP                                         single_null,
                          const bool                                   error_ok,
                          SEXP                                         on_error,
                          const rcppsimdjson::deserialize::Simplify_To simplify_to,
                          const rcppsimdjson::deserialize::Type_Policy type_policy,
                          const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {

    if constexpr (RTYPE == RAWSXP) {
        return error_ok ? deserialize_single_raw<true>(json,
                                                       json_pointer,
                                                       empty_array,
                                                       empty_object,
                                                       single_null,
                                                       on_error,
                                                       simplify_to,
                                                       type_policy,
                                                       int64_r_type)
                        : deserialize_single_raw<false>(json,
                                                        json_pointer,
                                                        empty_array,
                                                        empty_object,
                                                        single_null,
                                                        on_error,
                                                        simplify_to,
                                                        type_policy,
                                                        int64_r_type);

    } else if constexpr (RTYPE == VECSXP) {
        return error_ok ? deserialize_multiple_raw<true>(json,
                                                         json_pointer,
                                                         empty_array,
                                                         empty_object,
                                                         single_null,
                                                         on_error,
                                                         simplify_to,
                                                         type_policy,
                                                         int64_r_type)
                        : deserialize_multiple_raw<false>(json,
                                                          json_pointer,
                                                          empty_array,
                                                          empty_object,
                                                          single_null,
                                                          on_error,
                                                          simplify_to,
                                                          type_policy,
                                                          int64_r_type);

    } else {
        switch (json.length()) {
            case 0L:
                return R_NilValue; // # nocov

            case 1L:
                return error_ok ? deserialize_single_chr<true>(json,
                                                               json_pointer,
                                                               empty_array,
                                                               empty_object,
                                                               single_null,
                                                               on_error,
                                                               simplify_to,
                                                               type_policy,
                                                               int64_r_type)
                                : deserialize_single_chr<false>(json,
                                                                json_pointer,
                                                                empty_array,
                                                                empty_object,
                                                                single_null,
                                                                on_error,
                                                                simplify_to,
                                                                type_policy,
                                                                int64_r_type);

            default:
                return error_ok ? deserialize_multiple_chr<true>(json,
                                                                 json_pointer,
                                                                 empty_array,
                                                                 empty_object,
                                                                 single_null,
                                                                 on_error,
                                                                 simplify_to,
                                                                 type_policy,
                                                                 int64_r_type)
                                : deserialize_multiple_chr<false>(json,
                                                                  json_pointer,
                                                                  empty_array,
                                                                  empty_object,
                                                                  single_null,
                                                                  on_error,
                                                                  simplify_to,
                                                                  type_policy,
                                                                  int64_r_type);
        }
    }
}


// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize_json(SEXP               json,
                      const std::string& json_pointer = "",
                      SEXP               empty_array  = R_NilValue,
                      SEXP               empty_object = R_NilValue,
                      SEXP               single_null  = R_NilValue,
                      const bool         error_ok     = false,
                      SEXP               on_error     = R_NilValue,
                      const int          simplify_to  = 0,
                      const int          type_policy  = 0,
                      const int          int64_r_type = 0) {
    switch (TYPEOF(json)) {
        case STRSXP:
            return dispatch_deserialize<STRSXP>(
                json,
                json_pointer,
                empty_array,
                empty_object,
                single_null,
                error_ok,
                on_error,
                static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
            );

        case RAWSXP:
            return dispatch_deserialize<RAWSXP>(
                json,
                json_pointer,
                empty_array,
                empty_object,
                single_null,
                error_ok,
                on_error,
                static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
            );

        case VECSXP:
            return dispatch_deserialize<VECSXP>(
                json,
                json_pointer,
                empty_array,
                empty_object,
                single_null,
                error_ok,
                on_error,
                static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
            );

        default:
            return R_NilValue;
    }
}


template <bool error_ok>
SEXP load_single_file(const Rcpp::String&                          file_path,
                      const std::string&                           json_pointer,
                      SEXP                                         empty_array,
                      SEXP                                         empty_object,
                      SEXP                                         single_null,
                      SEXP                                         on_error,
                      const rcppsimdjson::deserialize::Simplify_To simplify_to,
                      const rcppsimdjson::deserialize::Type_Policy type_policy,
                      const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {
    simdjson::dom::parser parser;
    auto [parsed, error] = json_pointer.empty()
                               ? parser.load(std::string(file_path))
                               : parser.load(std::string(file_path)).at(json_pointer);

    if (error) {
        if constexpr (error_ok) {
            return on_error;
        } else {
            Rcpp::stop(simdjson::error_message(error));
        }
    }

    return rcppsimdjson::deserialize::deserialize(parsed, //
                                                  empty_array,
                                                  empty_object,
                                                  single_null,
                                                  simplify_to,
                                                  type_policy,
                                                  int64_r_type);
}


template <bool error_ok>
SEXP load_multiple_files(const Rcpp::CharacterVector&                 file_path,
                         const std::string&                           json_pointer,
                         SEXP                                         empty_array,
                         SEXP                                         empty_object,
                         SEXP                                         single_null,
                         SEXP                                         on_error,
                         const rcppsimdjson::deserialize::Simplify_To simplify_to,
                         const rcppsimdjson::deserialize::Type_Policy type_policy,
                         const rcppsimdjson::utils::Int64_R_Type      int64_r_type) {
    const auto n   = file_path.length();
    auto       out = Rcpp::List(n);

    simdjson::dom::parser parser;
    for (R_xlen_t i = 0; i < n; ++i) {
        if (file_path[i].get() == NA_STRING) {
            out[i] = Rcpp::CharacterVector::create(NA_STRING);
            continue;
        }

        auto [parsed, error] = json_pointer.empty()
                                   ? parser.load(std::string(file_path[i]))
                                   : parser.load(std::string(file_path[i])).at(json_pointer);

        if (error) {
            if constexpr (error_ok) {
                out[i] = on_error;
                continue;
            } else {
                Rcpp::stop(simdjson::error_message(error));
            }
        }

        out[i] = rcppsimdjson::deserialize::deserialize(parsed, //
                                                        empty_array,
                                                        empty_object,
                                                        single_null,
                                                        simplify_to,
                                                        type_policy,
                                                        int64_r_type);
    }

    out.attr("names") = file_path.attr("names");
    return out;
}


// [[Rcpp::export(.load_json)]]
SEXP load_json(const Rcpp::CharacterVector& file_path,
               const std::string&           json_pointer = "",
               SEXP                         empty_array  = R_NilValue,
               SEXP                         empty_object = R_NilValue,
               SEXP                         single_null  = R_NilValue,
               const bool                   error_ok     = false,
               SEXP                         on_error     = R_NilValue,
               const int                    simplify_to  = 0,
               const int                    type_policy  = 0,
               const int                    int64_r_type = 0) {
    switch (file_path.length()) {
        case 0:                // # nocov
            return R_NilValue; // # nocov

        case 1:
            return error_ok ? load_single_file<true>(
                                  file_path[0],
                                  json_pointer,
                                  empty_array,
                                  empty_object,
                                  single_null,
                                  on_error,
                                  static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                                  static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                                  static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
                                  )
                            : load_single_file<false>(
                                  file_path[0],
                                  json_pointer,
                                  empty_array,
                                  empty_object,
                                  single_null,
                                  on_error,
                                  static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                                  static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                                  static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
                              );

        default:
            return error_ok ? load_multiple_files<true>(
                                  file_path,
                                  json_pointer,
                                  empty_array,
                                  empty_object,
                                  single_null,
                                  on_error,
                                  static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                                  static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                                  static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
                                  )
                            : load_multiple_files<false>(
                                  file_path,
                                  json_pointer,
                                  empty_array,
                                  empty_object,
                                  single_null,
                                  on_error,
                                  static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                                  static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                                  static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type) //
                              );
    }

    return R_NilValue;
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
