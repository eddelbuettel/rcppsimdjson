#ifndef RCPPSIMDJSON__DESERIALIZE_HPP
#define RCPPSIMDJSON__DESERIALIZE_HPP


#include "deserialize/simplify.hpp"


namespace rcppsimdjson {
namespace deserialize {


inline static constexpr auto IS_FILE     = true;
inline static constexpr auto IS_NOT_FILE = false;

inline static constexpr auto PARSE_ERROR_OK     = true;
inline static constexpr auto PARSE_ERROR_NOT_OK = false;

inline static constexpr auto QUERY_ERROR_OK     = true;
inline static constexpr auto QUERY_ERROR_NOT_OK = false;

inline static constexpr auto SINGLE_JSON     = true;
inline static constexpr auto NOT_SINGLE_JSON = false;

inline static constexpr auto SINGLE_QUERY     = true;
inline static constexpr auto NOT_SINGLE_QUERY = false;

inline static constexpr auto YES_DEBUG = true;
inline static constexpr auto NO_DEBUG  = false;


struct Parse_Opts {
    rcppsimdjson::deserialize::Simplify_To simplify_to;
    rcppsimdjson::deserialize::Type_Policy type_policy;
    rcppsimdjson::utils::Int64_R_Type      int64_r_type;
    SEXP                                   empty_array;
    SEXP                                   empty_object;
    SEXP                                   single_null;
};


/**
 * @brief Deserialize a parsed  simdjson::dom::element to R objects.
 *
 *
 * @param element  simdjson::dom::element to deserialize.
 *
 * @param empty_array R object to return when encountering an empty JSON array.
 *
 * @param empty_object R object to return when encountering an empty JSON object.
 *
 * @param type_policy  Type_Policy specifying type strictness in combining mixed-type array
 * elements into R vectors.
 *
 * @param int64_opt  Int64_R_Type specifying how big integers are returned to R.
 *
 * @param simplify_to  Simplify_To specifying the maximum level of simplification.
 *
 *
 * @return The simplified R object ( SEXP ).
 */
inline auto deserialize(simdjson::dom::element parsed, const Parse_Opts& parse_opts) -> SEXP {
    using Int64_R_Type = utils::Int64_R_Type;

    auto& [simplify_to, type_policy, int64_opt, empty_array, empty_object, single_null] =
        parse_opts;

    // THE GREAT DISPATCHER
    switch (type_policy) {
        case Type_Policy::anything_goes: {
            switch (int64_opt) {
                case Int64_R_Type::Double: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::Double
                case Int64_R_Type::String: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::String,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::String
                case Int64_R_Type::Integer64: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::anything_goes,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    }
                } // simplify_to
            }     // Int64_R_Type::Integer64
        }         // Type_Policy::anything_goes


        case Type_Policy::ints_as_dbls: {
            switch (int64_opt) {
                case Int64_R_Type::Double: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::Double
                case Int64_R_Type::String: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::String,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::String
                case Int64_R_Type::Integer64: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::ints_as_dbls,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    }
                } // simplify_to
            }     // Int64_R_Type::Integer64
        }         // Type_Policy::ints_as_dbls


        case Type_Policy::strict: {
            switch (int64_opt) {
                case Int64_R_Type::Double: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Double,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::strict, //
                                                    Int64_R_Type::Double,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::Double
                case Int64_R_Type::String: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::String,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::String,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::String,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::strict, //
                                                    Int64_R_Type::String,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    } // simplify_to
                }     // Int64_R_Type::String
                case Int64_R_Type::Integer64: {
                    switch (simplify_to) {
                        case Simplify_To::data_frame:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::data_frame>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::matrix:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::matrix>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::vector:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::vector>(
                                parsed, empty_array, empty_object, single_null);

                        case Simplify_To::list:
                            return simplify_element<Type_Policy::strict,
                                                    Int64_R_Type::Integer64,
                                                    Simplify_To::list>(
                                parsed, empty_array, empty_object, single_null);
                    }
                } // simplify_to
            }     // Int64_R_Type::Integer64
        }         // Type_Policy::strict
    }

    return R_NilValue;
}


template <typename json_T, bool is_file>
inline simdjson::simdjson_result<simdjson::dom::element> parse(simdjson::dom::parser& parser,
                                                               const json_T&          json) {
    if constexpr (utils::resembles_vec_raw<json_T>()) {
        /* if `json` is a raw (unsigned char) vector, we can cheat */
        return parser.parse(
            std::string_view(reinterpret_cast<const char*>(&(json[0])), std::size(json)));
    }

    if constexpr (utils::resembles_vec_chr<json_T>()) {
        /* if `json` is a character vector, we're only parsing the first element */
        return parse<decltype(json[0]), is_file>(parser, json[0]);
    }

    if constexpr (utils::resembles_r_string<json_T>()) {
        if constexpr (is_file) { /* if `json` is a string and file path...*/
            /* ... check for a `memDecompress()`-compatible file extension... */
            if (const auto file_type = utils::get_memDecompress_type(std::string_view(json))) {
                return parse<Rcpp::RawVector, IS_NOT_FILE>(
                    parser, /* ... and decompress to a RawVector if so, then parse that */
                    utils::decompress(std::string(json), Rcpp::String(std::string(*file_type))));
            }
            return parser.load(std::string(json)); /* otherwise, just `parser::load()` the file */
        } else {
            return parser.parse(std::string_view(json)); /* if not file, just parse the string */
        }
    }
}


template <bool query_error_ok>
inline SEXP query_and_deserialize(simdjson::dom::element                       parsed,
                                  const Rcpp::String::const_StringProxy&       query,
                                  SEXP                                         on_query_error,
                                  const rcppsimdjson::deserialize::Parse_Opts& parse_opts) {
    if (rcppsimdjson::utils::is_na_string(query)) {
        return Rcpp::LogicalVector(1, NA_LOGICAL);
    }

    if (query.empty()) {
        /* if `query` is empty (""), parse/deserialize as if there's no query */
        return deserialize(parsed, parse_opts);
    }

    if constexpr (query_error_ok) {
        if (auto [queried, query_error] = parsed.at_pointer(std::string_view(query)); !query_error) {
            return deserialize(queried, parse_opts);
        }
        return on_query_error;

    } else { /* !query_error_ok */
        auto [queried, query_error] = parsed.at_pointer(std::string_view(query));
        if (query_error) {
            Rcpp::stop(simdjson::error_message(query_error));
        }
        return deserialize(queried, parse_opts);
    }
}


template <typename json_T, bool is_file, bool parse_error_ok>
inline SEXP parse_and_deserialize(simdjson::dom::parser&                       parser,
                                  const json_T&                                json,
                                  SEXP                                         on_parse_error,
                                  const rcppsimdjson::deserialize::Parse_Opts& parse_opts) {
    if (utils::is_na_string(json)) {
        return Rcpp::LogicalVector(1, NA_LOGICAL);
    }

    if constexpr (parse_error_ok) {
        if (auto [parsed, parse_error] = parse<json_T, is_file>(parser, json); !parse_error) {
            return deserialize(parsed, parse_opts);
        }
        return on_parse_error;

    } else {
        auto [parsed, parse_error] = parse<json_T, is_file>(parser, json);
        if (parse_error) {
            Rcpp::stop(simdjson::error_message(parse_error));
        }
        return deserialize(parsed, parse_opts);
    }
}


template <typename json_T, bool is_file, bool parse_error_ok, bool query_error_ok>
inline SEXP parse_query_and_deserialize(simdjson::dom::parser&                 parser,
                                        const json_T&                          json,
                                        const Rcpp::String::const_StringProxy& query,
                                        SEXP                                   on_parse_error,
                                        SEXP                                   on_query_error,
                                        const Parse_Opts&                      parse_opts) {
    if (utils::is_na_string(json)) {
        return Rcpp::LogicalVector(1, NA_LOGICAL);
    }

    if constexpr (parse_error_ok) {
        if (auto [parsed, parse_error] = parse<json_T, is_file>(parser, json); !parse_error) {
            return query_and_deserialize<query_error_ok>(parsed, query, on_query_error, parse_opts);
        }
        return on_parse_error;

    } else {
        auto [parsed, parse_error] = parse<json_T, is_file>(parser, json);
        if (parse_error) {
            Rcpp::stop(simdjson::error_message(parse_error));
        }
        return query_and_deserialize<query_error_ok>(parsed, query, on_query_error, parse_opts);
    }
}


template <typename json_T,
          bool is_file,
          bool is_single_json,
          bool parse_error_ok,
          bool query_error_ok>
inline SEXP no_query(const json_T&                                json,
                     SEXP                                         on_parse_error,
                     const rcppsimdjson::deserialize::Parse_Opts& parse_opts) {
    simdjson::dom::parser parser;

    if constexpr (is_single_json) {
        return parse_and_deserialize<json_T, is_file, parse_error_ok>(
            parser, json, on_parse_error, parse_opts);

    } else { /* !single_json */
        const R_xlen_t n = std::size(json);
        Rcpp::List     out(n);

        for (R_xlen_t i = 0; i < n; ++i) {
            out[i] = parse_and_deserialize<decltype(json[i]), is_file, parse_error_ok>(
                parser, json[i], on_parse_error, parse_opts);
        }

        out.attr("names") = json.attr("names");
        return out;
    }
}


template <typename json_T,
          bool is_file,
          bool is_single_json,
          bool is_single_query,
          bool parse_error_ok,
          bool query_error_ok>
inline SEXP flat_query(const json_T&                                json,
                       const Rcpp::CharacterVector&                 query,
                       SEXP                                         on_parse_error,
                       SEXP                                         on_query_error,
                       const rcppsimdjson::deserialize::Parse_Opts& parse_opts) {
    simdjson::dom::parser parser;

    if constexpr (is_single_json) {
        if constexpr (is_single_query) {
            return parse_query_and_deserialize<json_T, is_file, parse_error_ok, query_error_ok>(
                parser, json, query[0], on_parse_error, on_query_error, parse_opts);

        } else { /* !single_query */
            const R_xlen_t n = std::size(query);
            Rcpp::List     out(n);

            if constexpr (parse_error_ok) {
                if (auto [parsed, parse_error] = parse<json_T, is_file>(parser, json);
                    !parse_error) {
                    for (R_xlen_t i = 0; i < n; ++i) {
                        out[i] = query_and_deserialize<query_error_ok>(
                            parsed, query[i], on_query_error, parse_opts);
                    }
                    out.attr("names") = query.attr("names");
                    return out;
                }
                return on_parse_error;

            } else { /* !parse_error_ok */
                auto [parsed, parse_error] = parse<json_T, is_file>(parser, json);
                if (parse_error) {
                    Rcpp::stop(simdjson::error_message(parse_error));
                }
                for (R_xlen_t i = 0; i < n; ++i) {
                    out[i] = query_and_deserialize<query_error_ok>(
                        parsed, query[i], on_query_error, parse_opts);
                }
                out.attr("names") = query.attr("names");
                return out;
            }
        }

    } else { /* !single_json */
        const R_xlen_t n = std::size(json);
        Rcpp::List     out(n);

        if constexpr (is_single_query) {
            for (R_xlen_t i = 0; i < n; ++i) {
                out[i] = parse_query_and_deserialize<decltype(json[i]),
                                                     is_file,
                                                     parse_error_ok,
                                                     query_error_ok>(
                    parser, json[i], query[0], on_parse_error, on_query_error, parse_opts);
            }
            out.attr("names") = json.attr("names");
            return out;

        } else { /* !single_query */
            for (R_xlen_t i = 0; i < n; ++i) {
                const R_xlen_t n_queries = std::size(query);
                Rcpp::List     res(n_queries);

                for (R_xlen_t j = 0; j < n_queries; ++j) {
                    res[j] = parse_query_and_deserialize<decltype(json[i]),
                                                         is_file,
                                                         parse_error_ok,
                                                         query_error_ok>(
                        parser, json[i], query[j], on_parse_error, on_query_error, parse_opts);
                }
                res.attr("names") = query.attr("names");
                out[i]            = res;
            }
        }

        out.attr("names") = json.attr("names");
        return out;
    }
}


template <typename json_T,
          bool is_file,
          bool is_single_json,
          bool is_single_query,
          bool parse_error_ok,
          bool query_error_ok>
inline SEXP nested_query(const json_T&                                json,
                         const Rcpp::ListOf<Rcpp::CharacterVector>&   query,
                         SEXP                                         on_parse_error,
                         SEXP                                         on_query_error,
                         const rcppsimdjson::deserialize::Parse_Opts& parse_opts) {
    const R_xlen_t        n = std::size(json); /* query already checked to be the same size */
    Rcpp::List            out(n);
    simdjson::dom::parser parser;

    if constexpr (is_single_json) {
        if constexpr (parse_error_ok) {
            if (auto [parsed, parse_error] = parse<json_T, is_file>(parser, json); !parse_error) {
                for (R_xlen_t i = 0; i < n; ++i) {
                    const R_xlen_t n_queries = std::size(query[i]);
                    Rcpp::List     res(n_queries);
                    for (R_xlen_t j = 0; j < n_queries; ++j) {
                        res[j] = query_and_deserialize<query_error_ok>(
                            parsed, query[i][j], on_query_error, parse_opts);
                    }
                    res.attr("names") = query[i].attr("names");
                    out[i]            = res;
                }
            }

            return on_parse_error;

        } else { /* !parse_error_ok */
            auto [parsed, parse_error] = parse<json_T, is_file>(parser, json);
            if (parse_error) {
                Rcpp::stop(simdjson::error_message(parse_error));
            }
            for (R_xlen_t i = 0; i < n; ++i) {
                const R_xlen_t n_queries = std::size(query[i]);
                Rcpp::List     res(n_queries);
                for (R_xlen_t j = 0; j < n_queries; ++j) {
                    res[j] = query_and_deserialize<query_error_ok>(
                        parsed, query[i][j], on_query_error, parse_opts);
                }
                res.attr("names") = query[i].attr("names");
                out[i]            = res;
            }
        }

    } else { /* !is_single_json */
        for (R_xlen_t i = 0; i < n; ++i) {
            const R_xlen_t n_queries = std::size(query[i]);
            if constexpr (parse_error_ok) {
                if (auto [parsed, parse_error] = parse<decltype(json[i]), is_file>(parser, json[i]);
                    !parse_error) {
                    Rcpp::List res(n_queries);
                    for (R_xlen_t j = 0; j < n_queries; ++j) {
                        res[j] = query_and_deserialize<query_error_ok>(
                            parsed, query[i][j], on_query_error, parse_opts);
                    }
                    res.attr("names") = query[i].attr("names");
                    out[i]            = res;
                }
                out[i] = on_parse_error;

            } else { /* !parse_error_ok */
                auto [parsed, parse_error] = parse<decltype(json[i]), is_file>(parser, json[i]);
                if (parse_error) {
                    Rcpp::stop(simdjson::error_message(parse_error));
                }
                Rcpp::List res(n_queries);
                for (R_xlen_t j = 0; j < n_queries; ++j) {
                    res[j] = query_and_deserialize<query_error_ok>(
                        parsed, query[i][j], on_query_error, parse_opts);
                }
                res.attr("names") = query[i].attr("names");
                out[i]            = res;
            }
        }
    }

    if (utils::is_named(query)) {
        out.attr("names") = query.attr("names");
    } else {
        out.attr("names") = json.attr("names");
    }
    return out;
}


template <bool is_file,
          bool is_single_json,
          bool is_single_query,
          bool parse_error_ok,
          bool query_error_ok>
inline SEXP dispatch_deserialize(
    SEXP json, SEXP query, SEXP on_parse_error, SEXP on_query_error, const Parse_Opts& parse_opts) {

    switch (TYPEOF(json)) {
        case STRSXP: {
            switch (TYPEOF(query)) {
                case NILSXP:
                    return no_query<Rcpp::CharacterVector,
                                    is_file,
                                    is_single_json,
                                    parse_error_ok,
                                    query_error_ok>(json, on_parse_error, parse_opts);

                case STRSXP:
                    return flat_query<Rcpp::CharacterVector,
                                      is_file,
                                      is_single_json,
                                      is_single_query,
                                      parse_error_ok,
                                      query_error_ok>(
                        json, query, on_parse_error, on_query_error, parse_opts);

                case VECSXP:
                    return nested_query<Rcpp::CharacterVector,
                                        is_file,
                                        is_single_json,
                                        NOT_SINGLE_QUERY, /* VECSXP query always NOT_SINGLE_QUERY */
                                        parse_error_ok,
                                        query_error_ok>(
                        json, query, on_parse_error, on_query_error, parse_opts);

                default:
                    return R_NilValue;
            }
        }

        case RAWSXP: {
            switch (TYPEOF(query)) {
                case NILSXP:
                    return no_query<Rcpp::RawVector,
                                    is_file,
                                    SINGLE_JSON, /* RAWSXP json must be SINGLE_JSON */
                                    parse_error_ok,
                                    query_error_ok>(json, on_parse_error, parse_opts);

                case STRSXP:
                    return flat_query<Rcpp::RawVector,
                                      is_file,
                                      SINGLE_JSON, /* RAWSXP json must be SINGLE_JSON */
                                      is_single_query,
                                      parse_error_ok,
                                      query_error_ok>(
                        json, query, on_parse_error, on_query_error, parse_opts);

                case VECSXP:
                    return nested_query<Rcpp::RawVector,
                                        is_file,
                                        SINGLE_JSON,      /* RAWSXP json always SINGLE_JSON */
                                        NOT_SINGLE_QUERY, /* VECSXP query always NOT_SINGLE_QUERY */
                                        parse_error_ok,
                                        query_error_ok>(
                        json, query, on_parse_error, on_query_error, parse_opts);

                default:
                    return R_NilValue;
            }
        }

        case VECSXP: {
            switch (TYPEOF(query)) {
                case NILSXP:
                    return no_query<Rcpp::ListOf<Rcpp::RawVector>,
                                    is_file,
                                    NOT_SINGLE_JSON, /* VECSXP json always NOT_SINGLE_JSON */
                                    parse_error_ok,
                                    query_error_ok>(json, on_parse_error, parse_opts);

                case STRSXP:
                    return flat_query<Rcpp::ListOf<Rcpp::RawVector>,
                                      is_file,
                                      NOT_SINGLE_JSON, /* VECSXP json always NOT_SINGLE_JSON */
                                      is_single_query,
                                      parse_error_ok,
                                      query_error_ok>(
                        json, query, on_parse_error, on_query_error, parse_opts);

                case VECSXP:
                    return nested_query<Rcpp::ListOf<Rcpp::RawVector>,
                                        is_file,
                                        NOT_SINGLE_JSON,  /* VECSXP json always NOT_SINGLE_JSON */
                                        NOT_SINGLE_QUERY, /* VECSXP query always NOT_SINGLE_QUERY */
                                        parse_error_ok,
                                        query_error_ok>(
                        json, query, on_parse_error, on_query_error, parse_opts);

                default:
                    return R_NilValue;
            }
        }
        default:
            return R_NilValue;
    }

    return R_NilValue;
}


template <bool is_file, bool is_single_json, bool is_single_query>
inline SEXP start(SEXP       json,
                  SEXP       query,
                  SEXP       empty_array,
                  SEXP       empty_object,
                  SEXP       single_null,
                  const bool parse_error_ok,
                  SEXP       on_parse_error,
                  const bool query_error_ok,
                  SEXP       on_query_error,
                  const int  simplify_to,
                  const int  type_policy,
                  const int  int64_r_type) {
    const auto parse_opts = Parse_Opts{static_cast<Simplify_To>(simplify_to),
                                       static_cast<Type_Policy>(type_policy),
                                       static_cast<utils::Int64_R_Type>(int64_r_type),
                                       empty_array,
                                       empty_object,
                                       single_null};

    if (parse_error_ok) {
        return query_error_ok ? dispatch_deserialize<is_file,
                                                     is_single_json,
                                                     is_single_query,
                                                     PARSE_ERROR_OK,
                                                     QUERY_ERROR_OK>(
                                    json, query, on_parse_error, on_query_error, parse_opts)
                              : dispatch_deserialize<is_file,
                                                     is_single_json,
                                                     is_single_query,
                                                     PARSE_ERROR_OK,
                                                     QUERY_ERROR_NOT_OK>(
                                    json, query, on_parse_error, on_query_error, parse_opts);
    } else { /* !parse_error_ok*/
        return query_error_ok ? dispatch_deserialize<is_file,
                                                     is_single_json,
                                                     is_single_query,
                                                     PARSE_ERROR_NOT_OK,
                                                     QUERY_ERROR_OK>(
                                    json, query, on_parse_error, on_query_error, parse_opts)
                              : dispatch_deserialize<is_file,
                                                     is_single_json,
                                                     is_single_query,
                                                     PARSE_ERROR_NOT_OK,
                                                     QUERY_ERROR_NOT_OK>(
                                    json, query, on_parse_error, on_query_error, parse_opts);
    }
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
