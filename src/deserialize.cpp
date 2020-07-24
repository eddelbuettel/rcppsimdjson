


//
//
//
#include <RcppSimdJson.hpp>
#include <fstream>
//
//

struct Parse_Opts {
    rcppsimdjson::deserialize::Simplify_To simplify_to;
    rcppsimdjson::deserialize::Type_Policy type_policy;
    rcppsimdjson::utils::Int64_R_Type      int64_r_type;
    SEXP                                   empty_array;
    SEXP                                   empty_object;
    SEXP                                   single_null;
};

inline static constexpr auto IS_FILE     = true;
inline static constexpr auto IS_NOT_FILE = false;

inline static constexpr auto ERROR_OK     = true;
inline static constexpr auto ERROR_NOT_OK = false;

inline static constexpr auto RECYCLE_QUERY      = true;
inline static constexpr auto DONT_RECYCLE_QUERY = false;

template <typename file_path_T>
inline std::optional<std::string_view> get_memDecompress_type(const file_path_T& file_path) {
    if (const auto dot = std::string_view(file_path).rfind('.'); dot != std::string_view::npos) {
        const auto ext = file_path.substr(dot + 1);
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
    return std::nullopt;
}


template <typename T1, typename T2>
inline constexpr bool is_same_t() {
    return std::is_same_v<std::add_const_t<std::remove_reference_t<T1>>,
                          std::add_const_t<std::remove_reference_t<T2>>>;
}

template <typename T>
inline constexpr bool is_rcpp_string() {
    return is_same_t<T, Rcpp::String>() || is_same_t<T, Rcpp::String::const_StringProxy>() ||
           is_same_t<T, Rcpp::String::StringProxy>();
}


template <typename T>
inline constexpr bool is_json_na(const T& x) {
    if constexpr (is_rcpp_string<T>()) {
        return x == NA_STRING;
    }
    if constexpr (is_same_t<T, Rcpp::CharacterVector>()) {
        return x[0].get() == NA_STRING;
    }
    return false;
}


template <typename file_path_T>
inline Rcpp::RawVector decompress(const file_path_T& file_path, const Rcpp::String& file_type) {
    std::ifstream stream(file_path, std::ios::binary | std::ios::ate);
    if (!stream) {
        Rcpp::stop("");
    }

    const auto end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    const std::size_t n(end - stream.tellg());
    if (n == 0) {
        Rcpp::stop("");
    }

    std::vector<Rbyte> buffer(n);
    stream.read((char*)buffer.data(),
                n); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)

    // Rcpp::RawVector buffer(n);
    // stream.read(((char*)&(buffer[0])), n); //
    // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)

    return Rcpp::Function("memDecompress")(buffer, file_type, false);
}


template <typename inner_json_T, bool is_file>
inline simdjson::simdjson_result<simdjson::dom::element> parse(simdjson::dom::parser& parser,
                                                               const inner_json_T&    json) {
    if constexpr (is_same_t<inner_json_T, Rcpp::ChildVector<Rcpp::RawVector>>() ||
                  is_same_t<inner_json_T, Rcpp::RawVector>() ||
                  is_same_t<inner_json_T, Rcpp::Vector<RAWSXP, Rcpp::PreserveStorage>>()) {
        return parser.parse(std::string(std::cbegin(json), std::cend(json)));
    }

    if constexpr (is_same_t<inner_json_T, Rcpp::CharacterVector>()) {
        return parse<decltype(json[0]), is_file>(parser, json[0]);
    }

    if constexpr (is_same_t<inner_json_T, Rcpp::String>() ||
                  is_same_t<inner_json_T, Rcpp::String::const_StringProxy>() ||
                  is_same_t<inner_json_T, Rcpp::String::StringProxy>()) {
        if constexpr (is_file) {
            if (const auto file_type = get_memDecompress_type(std::string(json))) {
                return parse<Rcpp::RawVector, IS_NOT_FILE>(
                    parser, decompress(std::string(json), Rcpp::String((*file_type).data())));
            } else {
                return parser.load(std::string(json));
            }
        } else {
            return parser.parse(std::string_view(json));
        }
    }
}


template <typename json_T, bool error_ok, bool is_file>
SEXP single_json_no_query(const json_T& json, SEXP on_error, const Parse_Opts parse_opts) {
    if (is_json_na(json)) {
        return Rcpp::LogicalVector(1, NA_LOGICAL);
    }
    // if constexpr (is_same_t<json_T, Rcpp::CharacterVector>()) {
    //     if (json[0].get() == NA_STRING) {
    //         return Rcpp::LogicalVector(1, NA_LOGICAL);
    //     }
    // }
    simdjson::dom::parser parser;
    Rcpp::CharacterVector test;

    if constexpr (error_ok) {
        if (auto [parsed, error] = parse<json_T, is_file>(parser, json); !error) {
            return rcppsimdjson::deserialize::deserialize(parsed,
                                                          parse_opts.empty_array,
                                                          parse_opts.empty_object,
                                                          parse_opts.single_null,
                                                          parse_opts.simplify_to,
                                                          parse_opts.type_policy,
                                                          parse_opts.int64_r_type);
        } else {
            return on_error;
        }

    } else {
        auto [parsed, error] = parse<json_T, is_file>(parser, json);
        if (error) {
            Rcpp::stop(simdjson::error_message(error));
        } else {
            return rcppsimdjson::deserialize::deserialize(parsed,
                                                          parse_opts.empty_array,
                                                          parse_opts.empty_object,
                                                          parse_opts.single_null,
                                                          parse_opts.simplify_to,
                                                          parse_opts.type_policy,
                                                          parse_opts.int64_r_type);
        }
    }
}


template <typename json_T, bool error_ok, bool is_file>
SEXP multi_json_no_query(const json_T& json, SEXP on_error, const Parse_Opts parse_opts) {
    const R_xlen_t        n = std::size(json);
    Rcpp::List            out(n);
    simdjson::dom::parser parser;

    for (R_xlen_t i = 0; i < n; ++i) {
        if (is_json_na(json[i])) {
            out[i] = Rcpp::LogicalVector(1, NA_LOGICAL);

        } else {
            if constexpr (error_ok) {
                if (auto [parsed, error] = parse<decltype(json[i]), is_file>(parser, json[i]);
                    !error) {
                    out[i] = rcppsimdjson::deserialize::deserialize(parsed,
                                                                    parse_opts.empty_array,
                                                                    parse_opts.empty_object,
                                                                    parse_opts.single_null,
                                                                    parse_opts.simplify_to,
                                                                    parse_opts.type_policy,
                                                                    parse_opts.int64_r_type);
                } else {
                    out[i] = on_error;
                }

            } else {
                auto [parsed, error] = parse<decltype(json[i]), is_file>(parser, json[i]);
                if (error) {
                    Rcpp::stop(simdjson::error_message(error));
                } else {
                    out[i] = rcppsimdjson::deserialize::deserialize(parsed,
                                                                    parse_opts.empty_array,
                                                                    parse_opts.empty_object,
                                                                    parse_opts.single_null,
                                                                    parse_opts.simplify_to,
                                                                    parse_opts.type_policy,
                                                                    parse_opts.int64_r_type);
                }
            }
        }
    }

    if (json.hasAttribute("names")) {
        out.attr("names") = json.attr("names");
    }

    return out;
}


template <typename json_T, bool error_ok, bool is_file, bool recycle_query>
SEXP single_json_flat_query(const json_T&                json,
                            const Rcpp::CharacterVector& query,
                            SEXP                         on_error,
                            const Parse_Opts             parse_opts) {
    if (is_json_na(json[0])) {
        return Rcpp::LogicalVector(1, NA_LOGICAL);
    }

    simdjson::dom::parser parser;

    if constexpr (recycle_query) {
        if constexpr (error_ok) {
            if (auto [parsed, error] =
                    parse<json_T, is_file>(parser, json).at(std::string(query[0]));
                !error) {
                return rcppsimdjson::deserialize::deserialize(parsed,
                                                              parse_opts.empty_array,
                                                              parse_opts.empty_object,
                                                              parse_opts.single_null,
                                                              parse_opts.simplify_to,
                                                              parse_opts.type_policy,
                                                              parse_opts.int64_r_type);
            } else {
                return on_error;
            }

        } else {
            auto [parsed, error] = parse<json_T, is_file>(parser, json).at(std::string(query[0]));

            if (error) {
                Rcpp::stop(simdjson::error_message(error));
            } else {
                return rcppsimdjson::deserialize::deserialize(parsed,
                                                              parse_opts.empty_array,
                                                              parse_opts.empty_object,
                                                              parse_opts.single_null,
                                                              parse_opts.simplify_to,
                                                              parse_opts.type_policy,
                                                              parse_opts.int64_r_type);
            }
        }

    } else {
        const R_xlen_t n_queries = std::size(query);
        Rcpp::List     out(n_queries);
        if constexpr (error_ok) {
            if (auto [parsed, error] = parse<json_T, is_file>(parser, json); !error) {
                for (R_xlen_t i = 0; i < n_queries; ++i) {
                    if (auto [inner_parsed, inner_error] = parsed.at(std::string_view(query[i]));
                        !inner_error) {
                        out[i] = rcppsimdjson::deserialize::deserialize(inner_parsed,
                                                                        parse_opts.empty_array,
                                                                        parse_opts.empty_object,
                                                                        parse_opts.single_null,
                                                                        parse_opts.simplify_to,
                                                                        parse_opts.type_policy,
                                                                        parse_opts.int64_r_type);
                    } else {
                        out[i] = on_error;
                    }
                }
            }
        } else {
            auto [parsed, error] = parse<json_T, is_file>(parser, json);
            if (error) {
                Rcpp::stop(simdjson::error_message(error));
            } else {
                for (R_xlen_t i = 0; i < n_queries; ++i) {
                    auto [inner_parsed, inner_error] = parsed.at(std::string_view(query[i]));
                    if (inner_error) {
                        Rcpp::stop(simdjson::error_message(inner_error));
                    } else {
                        out[i] = rcppsimdjson::deserialize::deserialize(inner_parsed,
                                                                        parse_opts.empty_array,
                                                                        parse_opts.empty_object,
                                                                        parse_opts.single_null,
                                                                        parse_opts.simplify_to,
                                                                        parse_opts.type_policy,
                                                                        parse_opts.int64_r_type);
                    }
                }
            }
        }
        out.attr("names") = query.attr("names");
        return out;
    }
}


template <typename json_T, bool error_ok, bool is_file, bool recycle_query>
SEXP multi_json_flat_query(const json_T&                json,
                           const Rcpp::CharacterVector& query,
                           SEXP                         on_error,
                           const Parse_Opts             parse_opts) {
    const R_xlen_t        n = std::size(json);
    Rcpp::List            out(n);
    simdjson::dom::parser parser;

    std::string_view target_query;
    if constexpr (recycle_query) {
        target_query = std::string_view(query[0]);
    }

    for (R_xlen_t i = 0; i < n; ++i) {
        if (is_json_na(json[i])) {
            out[i] = Rcpp::LogicalVector(1, NA_LOGICAL);

        } else {
            if constexpr (!recycle_query) {
                target_query = std::string_view(query[i]);
            }

            if constexpr (error_ok) {
                if (auto [parsed, error] =
                        parse<decltype(json[i]), is_file>(parser, json[i]).at(target_query);
                    !error) {
                    out[i] = rcppsimdjson::deserialize::deserialize(parsed,
                                                                    parse_opts.empty_array,
                                                                    parse_opts.empty_object,
                                                                    parse_opts.single_null,
                                                                    parse_opts.simplify_to,
                                                                    parse_opts.type_policy,
                                                                    parse_opts.int64_r_type);
                } else {
                    out[i] = on_error;
                }

            } else {
                auto [parsed, error] =
                    parse<decltype(json[i]), is_file>(parser, json[i]).at(target_query);

                if (error) {
                    Rcpp::stop(simdjson::error_message(error));
                } else {
                    out[i] = rcppsimdjson::deserialize::deserialize(parsed,
                                                                    parse_opts.empty_array,
                                                                    parse_opts.empty_object,
                                                                    parse_opts.single_null,
                                                                    parse_opts.simplify_to,
                                                                    parse_opts.type_policy,
                                                                    parse_opts.int64_r_type);
                }
            }
        }
    }

    if constexpr (!recycle_query) {
        out.attr("names") = query.attr("names");
    }

    return out;
}


template <typename json_T, bool error_ok, bool is_file, bool recycle_query>
SEXP single_json_nested_query(const json_T&                              json,
                              const Rcpp::ListOf<Rcpp::CharacterVector>& query,
                              SEXP                                       on_error,
                              const Parse_Opts                           parse_opts) {
    const R_xlen_t        n = std::size(query);
    Rcpp::List            out(n);
    simdjson::dom::parser parser;

    Rcpp::CharacterVector target_queries;
    R_xlen_t              inner_n;
    if constexpr (recycle_query) {
        target_queries = query[0];
        inner_n        = std::size(target_queries);
    }

    for (R_xlen_t i = 0; i < n; ++i) {
        if (is_json_na(json)) {
            out[i] = Rcpp::LogicalVector(1, NA_LOGICAL);

        } else {
            if constexpr (!recycle_query) {
                target_queries = query[i];
                inner_n        = std::size(target_queries);
            }
            Rcpp::List inner_res(inner_n);

            if constexpr (error_ok) {
                if (auto [parsed, error] = parse<json_T, is_file>(parser, json); !error) {
                    for (R_xlen_t j = 0; j < inner_n; ++j) {
                        if (auto [inner_parsed, inner_error] =
                                parsed.at(std::string_view(target_queries[j]));
                            !inner_error) {
                            inner_res[j] =
                                rcppsimdjson::deserialize::deserialize(inner_parsed,
                                                                       parse_opts.empty_array,
                                                                       parse_opts.empty_object,
                                                                       parse_opts.single_null,
                                                                       parse_opts.simplify_to,
                                                                       parse_opts.type_policy,
                                                                       parse_opts.int64_r_type);
                        } else {
                            inner_res[j] = on_error;
                        }
                    }
                    inner_res.attr("names") = target_queries.attr("names");
                    out[i]                  = inner_res;
                }

            } else {
                auto [parsed, error] = parse<json_T, is_file>(parser, json);
                if (error) {
                    Rcpp::stop(simdjson::error_message(error));
                }

                for (R_xlen_t j = 0; j < inner_n; ++j) {
                    auto [inner_parsed, inner_error] =
                        parsed.at(std::string_view(target_queries[j]));

                    if (inner_error) {
                        Rcpp::stop(simdjson::error_message(inner_error));
                    } else {
                        inner_res[j] =
                            rcppsimdjson::deserialize::deserialize(inner_parsed,
                                                                   parse_opts.empty_array,
                                                                   parse_opts.empty_object,
                                                                   parse_opts.single_null,
                                                                   parse_opts.simplify_to,
                                                                   parse_opts.type_policy,
                                                                   parse_opts.int64_r_type);
                    }
                }
                inner_res.attr("names") = target_queries.attr("names");
                out[i]                  = inner_res;
            }
        }
    }

    out.attr("names") = query.attr("names");
    return out;
}


template <typename json_T, bool error_ok, bool is_file, bool recycle_query>
SEXP multi_json_nested_query(const json_T&                              json,
                             const Rcpp::ListOf<Rcpp::CharacterVector>& query,
                             SEXP                                       on_error,
                             const Parse_Opts                           parse_opts) {
    const R_xlen_t        n = std::size(json);
    Rcpp::List            out(n);
    simdjson::dom::parser parser;

    Rcpp::CharacterVector target_queries;
    R_xlen_t              inner_n;
    if constexpr (recycle_query) {
        target_queries = query[0];
        inner_n        = std::size(target_queries);
    }

    for (R_xlen_t i = 0; i < n; ++i) {
        if (is_json_na(json[i])) {
            out[i] = Rcpp::LogicalVector(1, NA_LOGICAL);

        } else {
            if constexpr (!recycle_query) {
                target_queries = query[i];
                inner_n        = std::size(target_queries);
            }

            Rcpp::List inner_res(inner_n);

            if constexpr (error_ok) {
                if (auto [parsed, error] = parse<decltype(json[i]), is_file>(parser, json[i]);
                    !error) {
                    for (R_xlen_t j = 0; j < inner_n; ++j) {
                        if (auto [inner_parsed, inner_error] =
                                parsed.at(std::string_view(target_queries[j]));
                            !inner_error) {
                            inner_res[j] = on_error;
                        } else {
                            inner_res[j] =
                                rcppsimdjson::deserialize::deserialize(inner_parsed,
                                                                       parse_opts.empty_array,
                                                                       parse_opts.empty_object,
                                                                       parse_opts.single_null,
                                                                       parse_opts.simplify_to,
                                                                       parse_opts.type_policy,
                                                                       parse_opts.int64_r_type);
                        }
                    }
                    inner_res.attr("names") = target_queries.attr("names");
                    out[i]                  = inner_res;
                }

            } else {
                auto [parsed, error] = parse<decltype(json[i]), is_file>(parser, json[i]);
                if (error) {
                    Rcpp::stop(simdjson::error_message(error));
                }

                for (R_xlen_t j = 0; j < inner_n; ++j) {
                    auto [inner_parsed, inner_error] =
                        parsed.at(std::string_view(target_queries[j]));

                    if (inner_error) {
                        Rcpp::stop(simdjson::error_message(inner_error));
                    } else {
                        inner_res[j] =
                            rcppsimdjson::deserialize::deserialize(inner_parsed,
                                                                   parse_opts.empty_array,
                                                                   parse_opts.empty_object,
                                                                   parse_opts.single_null,
                                                                   parse_opts.simplify_to,
                                                                   parse_opts.type_policy,
                                                                   parse_opts.int64_r_type);
                    }
                }
                inner_res.attr("names") = target_queries.attr("names");
                out[i]                  = inner_res;
            }
        }
    }
    if constexpr (!recycle_query) {
        out.attr("names") = query.attr("names");
    }
    return out;
}

template <bool is_file>
SEXP dispatch_deserialize(
    SEXP json, SEXP query, const bool error_ok, SEXP on_error, const Parse_Opts parse_opts) {
    const auto single_json = Rf_xlength(json) == 1 || TYPEOF(json) == RAWSXP;
    const auto no_query    = TYPEOF(query) == NILSXP;
    if (!no_query && !(Rf_xlength(query) == 1 || Rf_xlength(json) == Rf_xlength(query))) {
        Rcpp::stop("`query` must be `NULL`, a single value (that will be "
                   "recycled), or of the "
                   "same length "
                   "as `json`.");
    }
    const auto flat_query    = TYPEOF(query) == STRSXP;
    const auto nested_query  = TYPEOF(query) == VECSXP;
    const auto recycle_query = Rf_xlength(query) == 1;

    if (single_json) {
        // Rcpp::Rcout << "single" << std::endl;
        switch (TYPEOF(json)) {
            case STRSXP: {
                // Rcpp::Rcout << "STRSXP" << std::endl;
                if (no_query) {
                    return error_ok
                               ? single_json_no_query<Rcpp::CharacterVector, ERROR_OK, is_file>(
                                     json, on_error, parse_opts)
                               : single_json_no_query<Rcpp::CharacterVector, ERROR_NOT_OK, is_file>(
                                     json, on_error, parse_opts);
                }

                if (flat_query) {
                    // Rcpp::Rcout << "flat_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? single_json_flat_query<Rcpp::CharacterVector,
                                                                 ERROR_OK,
                                                                 is_file,
                                                                 RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_flat_query<Rcpp::CharacterVector,
                                                                 ERROR_NOT_OK,
                                                                 is_file,
                                                                 RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? single_json_flat_query<Rcpp::CharacterVector,
                                                                 ERROR_OK,
                                                                 is_file,
                                                                 DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_flat_query<Rcpp::CharacterVector,
                                                                 ERROR_NOT_OK,
                                                                 is_file,
                                                                 DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }

                if (nested_query) {
                    // Rcpp::Rcout << "nested_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? single_json_nested_query<Rcpp::CharacterVector,
                                                                   ERROR_OK,
                                                                   is_file,
                                                                   RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_nested_query<Rcpp::CharacterVector,
                                                                   ERROR_NOT_OK,
                                                                   is_file,
                                                                   RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? single_json_nested_query<Rcpp::CharacterVector,
                                                                   ERROR_OK,
                                                                   is_file,
                                                                   DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_nested_query<Rcpp::CharacterVector,
                                                                   ERROR_NOT_OK,
                                                                   is_file,
                                                                   DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }
            } // case STRSXP

            case RAWSXP: {
                // Rcpp::Rcout << "RAWSXP" << std::endl;
                if (no_query) {
                    // Rcpp::Rcout << "no_query" << std::endl;
                    return error_ok ? single_json_no_query<Rcpp::RawVector, ERROR_OK, is_file>(
                                          json, on_error, parse_opts)
                                    : single_json_no_query<Rcpp::RawVector, ERROR_NOT_OK, is_file>(
                                          json, on_error, parse_opts);
                }

                if (flat_query) {
                    // Rcpp::Rcout << "flat_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? single_json_flat_query<Rcpp::RawVector,
                                                                 ERROR_OK,
                                                                 is_file,
                                                                 RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_flat_query<Rcpp::RawVector,
                                                                 ERROR_NOT_OK,
                                                                 is_file,
                                                                 RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? single_json_flat_query<Rcpp::RawVector,
                                                                 ERROR_OK,
                                                                 is_file,
                                                                 DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_flat_query<Rcpp::RawVector,
                                                                 ERROR_NOT_OK,
                                                                 is_file,
                                                                 DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }

                if (nested_query) {
                    // Rcpp::Rcout << "nested_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? single_json_nested_query<Rcpp::RawVector,
                                                                   ERROR_OK,
                                                                   is_file,
                                                                   RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_nested_query<Rcpp::RawVector,
                                                                   ERROR_NOT_OK,
                                                                   is_file,
                                                                   RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? single_json_nested_query<Rcpp::RawVector,
                                                                   ERROR_OK,
                                                                   is_file,
                                                                   DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : single_json_nested_query<Rcpp::RawVector,
                                                                   ERROR_NOT_OK,
                                                                   is_file,
                                                                   DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }
            } // case RAWSXP

            default:
                return R_NilValue;
        }
    } else { // if multi...
        // Rcpp::Rcout << "multi" << std::endl;
        switch (TYPEOF(json)) {
            case STRSXP: {
                // Rcpp::Rcout << "STRSXP" << std::endl;
                if (no_query) {
                    // Rcpp::Rcout << "no_query" << std::endl;
                    return error_ok
                               ? multi_json_no_query<Rcpp::CharacterVector, ERROR_OK, is_file>(
                                     json, on_error, parse_opts)
                               : multi_json_no_query<Rcpp::CharacterVector, ERROR_NOT_OK, is_file>(
                                     json, on_error, parse_opts);
                }

                if (flat_query) {
                    // Rcpp::Rcout << "flat_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? multi_json_flat_query<Rcpp::CharacterVector,
                                                                ERROR_OK,
                                                                is_file,
                                                                RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_flat_query<Rcpp::CharacterVector,
                                                                ERROR_NOT_OK,
                                                                is_file,
                                                                RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? multi_json_flat_query<Rcpp::CharacterVector,
                                                                ERROR_OK,
                                                                is_file,
                                                                DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_flat_query<Rcpp::CharacterVector,
                                                                ERROR_NOT_OK,
                                                                is_file,
                                                                DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }

                if (nested_query) {
                    // Rcpp::Rcout << "nested_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? multi_json_nested_query<Rcpp::CharacterVector,
                                                                  ERROR_OK,
                                                                  is_file,
                                                                  RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_nested_query<Rcpp::CharacterVector,
                                                                  ERROR_NOT_OK,
                                                                  is_file,
                                                                  RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? multi_json_nested_query<Rcpp::CharacterVector,
                                                                  ERROR_OK,
                                                                  is_file,
                                                                  DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_nested_query<Rcpp::CharacterVector,
                                                                  ERROR_NOT_OK,
                                                                  is_file,
                                                                  DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }
            }

            case VECSXP:
                // Rcpp::Rcout << "VECSXP" << std::endl;
                if (no_query) {
                    // Rcpp::Rcout << "no_query" << std::endl;
                    return error_ok ? multi_json_no_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                          ERROR_OK,
                                                          is_file>(json, on_error, parse_opts)
                                    : multi_json_no_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                          ERROR_NOT_OK,
                                                          is_file>(json, on_error, parse_opts);
                }

                if (flat_query) {
                    // Rcpp::Rcout << "flat_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? multi_json_flat_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                ERROR_OK,
                                                                is_file,
                                                                RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_flat_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                ERROR_NOT_OK,
                                                                IS_NOT_FILE,
                                                                RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? multi_json_flat_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                ERROR_OK,
                                                                is_file,
                                                                DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_flat_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                ERROR_NOT_OK,
                                                                IS_NOT_FILE,
                                                                DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }

                if (nested_query) {
                    // Rcpp::Rcout << "nested_query" << std::endl;
                    if (recycle_query) {
                        return error_ok ? multi_json_nested_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                  ERROR_OK,
                                                                  is_file,
                                                                  RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_nested_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                  ERROR_NOT_OK,
                                                                  is_file,
                                                                  RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    } else {
                        return error_ok ? multi_json_nested_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                  ERROR_OK,
                                                                  is_file,
                                                                  DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts)
                                        : multi_json_nested_query<Rcpp::ListOf<Rcpp::RawVector>,
                                                                  ERROR_NOT_OK,
                                                                  is_file,
                                                                  DONT_RECYCLE_QUERY>(
                                              json, query, on_error, parse_opts);
                    }
                }

            default:
                return R_NilValue;
        }
    }

    return R_NilValue;
}

// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize(SEXP       json,
                 SEXP       query        = R_NilValue,
                 SEXP       empty_array  = R_NilValue,
                 SEXP       empty_object = R_NilValue,
                 SEXP       single_null  = R_NilValue,
                 const bool error_ok     = false,
                 SEXP       on_error     = R_NilValue,
                 const int  simplify_to  = 0,
                 const int  type_policy  = 0,
                 const int  int64_r_type = 0) {
    return dispatch_deserialize<IS_NOT_FILE>(
        json,
        query,
        error_ok,
        on_error,
        Parse_Opts{static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                   static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                   static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type),
                   empty_array,
                   empty_object,
                   single_null});
}


// [[Rcpp::export(.load_json)]]
SEXP load(const Rcpp::CharacterVector& json,
          SEXP                         query        = R_NilValue,
          SEXP                         empty_array  = R_NilValue,
          SEXP                         empty_object = R_NilValue,
          SEXP                         single_null  = R_NilValue,
          const bool                   error_ok     = false,
          SEXP                         on_error     = R_NilValue,
          const int                    simplify_to  = 0,
          const int                    type_policy  = 0,
          const int                    int64_r_type = 0) {

    return dispatch_deserialize<IS_FILE>(
        json,
        query,
        error_ok,
        on_error,
        Parse_Opts{static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to),
                   static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy),
                   static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type),
                   empty_array,
                   empty_object,
                   single_null});
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
