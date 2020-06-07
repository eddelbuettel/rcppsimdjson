#include <RcppSimdJson.hpp>

// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize_json(const Rcpp::String& json,
                      const std::string& json_pointer = "",
                      const int simplify_to = 0,    // 0=dataframe, 1=matrix, 2=vector, 3=list
                      const int type_policy = 0,    // 0=anything goes, 1=merge ints/dbls, 2=strict
                      const int int64_r_type = 0) { // 0=dbl, 1=string, 2=bit64::integer64
  using namespace rcppsimdjson;

  simdjson::dom::parser parser;
  simdjson::dom::element parsed;

  if constexpr (SIMDJSON_EXCEPTIONS) {
    parsed = json_pointer.empty() //
                 ? parser.parse(json)
                 : parser.parse(json).at(json_pointer);
  } else {
    parsed = json_pointer.empty() //
                 ? parser.parse(json).first
                 : parser.parse(json).at(json_pointer).first;
  }

  return deserialize::deserialize(parsed,
                                  static_cast<deserialize::Simplify_To>(simplify_to),
                                  static_cast<deserialize::Type_Policy>(type_policy),
                                  static_cast<utils::Int64_R_Type>(int64_r_type));
}


// [[Rcpp::export(.exceptions_enabled)]]
bool exceptions_enabled() {
  if constexpr (SIMDJSON_EXCEPTIONS) {
    return SIMDJSON_EXCEPTIONS == 1;
  } else {
    Rcpp::stop("SIMDJSON_EXCEPTIONS not defined");
  }
}
