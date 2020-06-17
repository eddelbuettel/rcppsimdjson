#include <RcppSimdJson.hpp>


SEXP deserialize_single_string(const Rcpp::CharacterVector& json,
                               const std::string& json_pointer,
                               SEXP empty_array,
                               SEXP empty_object,
                               const rcppsimdjson::deserialize::Simplify_To simplify_to,
                               const rcppsimdjson::deserialize::Type_Policy type_policy,
                               const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  using namespace rcppsimdjson;

  simdjson::dom::parser parser;
  auto [parsed, error] = json_pointer.empty() //
                             ? parser.parse(std::string(json[0]))
                             : parser.parse(std::string(json[0])).at(json_pointer);
  if (error) {
    Rcpp::stop(simdjson::error_message(error));
  }

  return deserialize::deserialize( //
      parsed,                      //
      empty_array,                 //
      empty_object,                //
      simplify_to,                 //
      type_policy,                 //
      int64_r_type                 //
  );                               //
}


SEXP deserialize_multiple_strings(const Rcpp::CharacterVector& json,
                                  const std::string& json_pointer,
                                  SEXP empty_array,
                                  SEXP empty_object,
                                  const rcppsimdjson::deserialize::Simplify_To simplify_to,
                                  const rcppsimdjson::deserialize::Type_Policy type_policy,
                                  const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  using namespace rcppsimdjson;

  const auto n = json.length();
  Rcpp::List out(n);
  simdjson::dom::parser parser;

  for (R_xlen_t i = 0; i < n; ++i) {
    auto [parsed, error] = json_pointer.empty() //
                               ? parser.parse(std::string(json[i]))
                               : parser.parse(std::string(json[i])).at(json_pointer);
    if (error) {
      Rcpp::stop("%s\n\nValue affected:\n\t- `json[[%d]]`", simdjson::error_message(error), i + 1);
    }

    out[i] = deserialize::deserialize( //
        parsed,                        //
        empty_array,                   //
        empty_object,                  //
        simplify_to,                   //
        type_policy,                   //
        int64_r_type                   //
    );                                 //
  }

  out.attr("names") = json.attr("names");

  return out;
}


//' Deserialize JSON into R Objects
//'
//' @param json \code{character()} containing one or more strings of JSON data.
//'
//' @param json_pointer \code{character(1L)}, default: \code{""}
//'
//' @param empty_array default: \code{NULL}. Any R object to return for empty JSON arrays.
//'
//' @param empty_object default: \code{NULL}. Any R object to return for empty JSON objects.
//'
//' @param simplify_to \code{integer(1L)}, default: \code{0L}.
//'     Maximum simplification level.
//'     0: data frame, 1: matrix, 2: vector, 3: list (no simplification)
//'
//' @param type_policy \code{integer(1L)}, default: \code{0L}.
//'     Level of type strictness.
//'     0: merge everything, 1: merge numbers, 2: strict (mixed types are not merged)
//'
//' @param int64_r_type \code{integer(1L)} default: \code{0L}
//'     How to return big integers to R.
//'     0: \code{double}, 1: string, 2: \code{bit64::integer64}-compatible number
//'
//' @details
//' Instead of using \code{lapply()} for vectors containing multiple strings/file paths,
//'     just use \code{.deserialize_json()} and \code{.load_json()} directly as they are vectorized
//'     (in the R sense). This is much more efficient as the underlying \code{simdjson::parser} can
//'     reuse internal buffers between parses. Since the overwhelming majority of JSON objects
//'     parsed will not result in R scalars, a \code{list()} is always returned when multiple items
//'     are passed to \code{.deserialize_json()} or \code{.load_json()}. Also in keeping with
//'     \code{lapply()}'s behavior, if the data passed has \code{names()}, the returned object will
//'     have the same names.
//'
//' @keywords internal
//'
//' @examples
//' # .deserialize_json() ======================================================
//' RcppSimdJson:::.deserialize_json('[[1,2,3],[4,5,6]]')
//'
//' RcppSimdJson:::.deserialize_json(
//'   '[{"a":1,"b":true},{"a":2,"b":false,"c":null}]'
//' )
//'
//' RcppSimdJson:::.deserialize_json(
//'   c(
//'     json1 = "[[1,2,3],[4,5,6]]",
//'     json2 = '[{"a":1,"b":true},{"a":2,"b":false,"c":null}]'
//'   )
//' )
//'
// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize_json(const Rcpp::CharacterVector& json,
                      const std::string& json_pointer = "",
                      SEXP empty_array = R_NilValue,
                      SEXP empty_object = R_NilValue,
                      const int simplify_to = 0,
                      const int type_policy = 0,
                      const int int64_r_type = 0) {
  switch (json.length()) {
    case 0:
      return R_NilValue;

    case 1:
      return deserialize_single_string(                                     //
          json,                                                             //
          json_pointer,                                                     //
          empty_array,                                                      //
          empty_object,                                                     //
          static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to), //
          static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy), //
          static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type)      //
      );                                                                    //

    default:
      return deserialize_multiple_strings(                                  //
          json,                                                             //
          json_pointer,                                                     //
          empty_array,                                                      //
          empty_object,                                                     //
          static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to), //
          static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy), //
          static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type)      //
      );                                                                    //
  }

  return R_NilValue;
}


SEXP load_single_file(const Rcpp::String& file_path,
                      const std::string& json_pointer,
                      SEXP empty_array,
                      SEXP empty_object,
                      const rcppsimdjson::deserialize::Simplify_To simplify_to,
                      const rcppsimdjson::deserialize::Type_Policy type_policy,
                      const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  simdjson::dom::parser parser;
  auto [parsed, error] = json_pointer.empty() //
                             ? parser.load(std::string(file_path))
                             : parser.load(std::string(file_path)).at(json_pointer);
  if (error) {
    Rcpp::stop(simdjson::error_message(error));
  }

  return rcppsimdjson::deserialize::deserialize( //
      parsed,                                    //
      empty_array,                               //
      empty_object,                              //
      simplify_to,                               //
      type_policy,                               //
      int64_r_type                               //
  );                                             //
}


SEXP load_multiple_files(const Rcpp::CharacterVector& file_path,
                         const std::string& json_pointer,
                         SEXP empty_array,
                         SEXP empty_object,
                         const rcppsimdjson::deserialize::Simplify_To simplify_to,
                         const rcppsimdjson::deserialize::Type_Policy type_policy,
                         const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  const auto n = file_path.length();
  auto out = Rcpp::List(n);
  simdjson::dom::parser parser;

  for (R_xlen_t i = 0; i < n; ++i) {
    auto [parsed, error] = json_pointer.empty() //
                               ? parser.load(std::string(file_path[i]))
                               : parser.load(std::string(file_path[i])).at(json_pointer);
    if (error) {
      Rcpp::stop("%s\n\nFile affected:\n\t- %s", //
                 simdjson::error_message(error), //
                 std::string(file_path[i]));     //
    }

    out[i] = rcppsimdjson::deserialize::deserialize( //
        parsed,                                      //
        empty_array,                                 //
        empty_object,                                //
        simplify_to,                                 //
        type_policy,                                 //
        int64_r_type                                 //
    );                                               //
  }

  out.attr("names") = file_path.attr("names");
  return out;
}


//' @rdname dot-deserialize_json
//'
//' @param file_path \code{character()} containing one or more paths to files containing
//'    JSON data.
//'
//' @examples
//' # .load_json() =============================================================
//' single_file <- system.file("jsonexamples", "small", "flatadversarial.json",
//'                            package = "RcppSimdJson")
//' RcppSimdJson:::.load_json(single_file)
//'
//' multiple_files <- vapply(
//'   c("flatadversarial.json", "adversarial.json"),
//'   function(.x) {
//'     system.file("jsonexamples/small", .x, package = "RcppSimdJson")
//'   },
//'   character(1L)
//' )
//' RcppSimdJson:::.load_json(multiple_files)
//'
// [[Rcpp::export(.load_json)]]
SEXP load_json(const Rcpp::CharacterVector& file_path,
               const std::string& json_pointer = "",
               SEXP empty_array = R_NilValue,
               SEXP empty_object = R_NilValue,
               const int simplify_to = 0,
               const int type_policy = 0,
               const int int64_r_type = 0) {
  switch (file_path.length()) {
    case 0:
      return R_NilValue;

    case 1:
      return load_single_file(                                              //
          file_path[0],                                                     //
          json_pointer,                                                     //
          empty_array,                                                      //
          empty_object,                                                     //
          static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to), //
          static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy), //
          static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type)      //
      );                                                                    //

    default:
      return load_multiple_files(                                           //
          file_path,                                                        //
          json_pointer,                                                     //
          empty_array,                                                      //
          empty_object,                                                     //
          static_cast<rcppsimdjson::deserialize::Simplify_To>(simplify_to), //
          static_cast<rcppsimdjson::deserialize::Type_Policy>(type_policy), //
          static_cast<rcppsimdjson::utils::Int64_R_Type>(int64_r_type)      //
      );                                                                    //
  }

  return R_NilValue;
}


// [[Rcpp::export(.exceptions_enabled)]]
bool exceptions_enabled() {
#ifndef SIMDJSON_EXCEPTIONS
  Rcpp::stop("`SIMDJSON_EXCEPTIONS` not defined");
#endif
  return SIMDJSON_EXCEPTIONS == 1;
}
