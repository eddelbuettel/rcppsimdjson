#include <RcppSimdJson.hpp>

//' Deserialize JSON into R Objects
//'
//' @param json \code{character(1L)}
//'
//' @param json_pointer \code{character(1L)}, default: \code{""}
//'
//' @param empty_array default: \code{NULL}. Any R object to return for empty JSON arrays.
//'
//' @param empty_object default: \code{NULL}. Any R object to return for empty JSON objects.
//'
//' @param simplify_to default: \code{0}. Maximum simplification level.
//'   0=dataframe, 1=matrix, 2=vector, 3=list
//'
//' @param type_policy default: \code{0}. Level of type strictness.
//'   0=anything goes, 1=merge integers/doubles, 2=strict
//'
//' @param int64_r_type default: \code{0} How to return big integers to R.
//'   0=double, 1=string, 2=bit64::integer64
//'
//' @keywords internal
//'
// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize_json(const Rcpp::String& json,
                      const std::string& json_pointer = "",
                      SEXP empty_array = R_NilValue,
                      SEXP empty_object = R_NilValue,
                      const int simplify_to = 0,
                      const int type_policy = 0,
                      const int int64_r_type = 0) {
  using namespace rcppsimdjson;

  simdjson::dom::parser parser;

#if RCPPSIMDJSON_EXCEPTIONS
  simdjson::dom::element parsed = json_pointer.empty() //
                                      ? parser.parse(json)
                                      : parser.parse(json).at(json_pointer);
#else
  auto [parsed, error] = json_pointer.empty() //
                             ? parser.parse(json).first
                             : parser.parse(json).at(json_pointer);
  if (error) {
    Rcpp::stop(simdjson::error_message(error));
  }
#endif

  return deserialize::deserialize(parsed,
                                  empty_array,
                                  empty_object,
                                  static_cast<deserialize::Simplify_To>(simplify_to),
                                  static_cast<deserialize::Type_Policy>(type_policy),
                                  static_cast<utils::Int64_R_Type>(int64_r_type));
}


// [[Rcpp::export(.exceptions_enabled)]]
bool exceptions_enabled() {
#ifndef SIMDJSON_EXCEPTIONS
  Rcpp::stop("`SIMDJSON_EXCEPTIONS` not defined");
#endif
  return SIMDJSON_EXCEPTIONS == 1;
}
