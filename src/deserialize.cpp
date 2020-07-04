#include <RcppSimdJson.hpp>


SEXP deserialize_single_string(const Rcpp::CharacterVector& json,
                               const std::string& json_pointer,
                               SEXP empty_array,
                               SEXP empty_object,
                               SEXP single_null,
                               const rcppsimdjson::deserialize::Simplify_To simplify_to,
                               const rcppsimdjson::deserialize::Type_Policy type_policy,
                               const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  using namespace rcppsimdjson;

  if (json[0].get() == NA_STRING) {
    return Rcpp::CharacterVector(1, NA_STRING);
  }

  simdjson::dom::parser parser;
  auto [parsed, error] = json_pointer.empty() //
                             ? parser.parse(std::string_view(json[0]))
                             : parser.parse(std::string_view(json[0])).at(json_pointer);
  if (error) {
    Rcpp::stop(simdjson::error_message(error));
  }

  return deserialize::deserialize( //
      parsed,                      //
      empty_array,                 //
      empty_object,                //
      single_null,                 //
      simplify_to,                 //
      type_policy,                 //
      int64_r_type                 //
  );                               //
}


SEXP deserialize_multiple_strings(const Rcpp::CharacterVector& json,
                                  const std::string& json_pointer,
                                  SEXP empty_array,
                                  SEXP empty_object,
                                  SEXP single_null,
                                  const rcppsimdjson::deserialize::Simplify_To simplify_to,
                                  const rcppsimdjson::deserialize::Type_Policy type_policy,
                                  const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  using namespace rcppsimdjson;

  const auto n = json.length();
  Rcpp::List out(n);
  simdjson::dom::parser parser;

  for (R_xlen_t i = 0; i < n; ++i) {
    if (json[i].get() == NA_STRING) {
      out[i] = Rcpp::CharacterVector(1, NA_STRING);
      continue;
    }
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
        single_null,                   //
        simplify_to,                   //
        type_policy,                   //
        int64_r_type                   //
    );                                 //
  }

  out.attr("names") = json.attr("names");

  return out;
}


// [[Rcpp::export(.deserialize_json)]]
SEXP deserialize_json(const Rcpp::CharacterVector& json,
                      const std::string& json_pointer = "",
                      SEXP empty_array                = R_NilValue,
                      SEXP empty_object               = R_NilValue,
                      SEXP single_null                = R_NilValue,
                      const int simplify_to           = 0,
                      const int type_policy           = 0,
                      const int int64_r_type          = 0) {
  switch (json.length()) {
    case 0:
      return R_NilValue;

    case 1:
      return deserialize_single_string(                                     //
          json,                                                             //
          json_pointer,                                                     //
          empty_array,                                                      //
          empty_object,                                                     //
          single_null,                                                      //
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
          single_null,                                                      //
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
                      SEXP single_null,
                      const rcppsimdjson::deserialize::Simplify_To simplify_to,
                      const rcppsimdjson::deserialize::Type_Policy type_policy,
                      const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  if (file_path == NA_STRING) {
    return Rcpp::CharacterVector(1, NA_STRING);
  }

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
      single_null,                               //
      simplify_to,                               //
      type_policy,                               //
      int64_r_type                               //
  );                                             //
}


SEXP load_multiple_files(const Rcpp::CharacterVector& file_path,
                         const std::string& json_pointer,
                         SEXP empty_array,
                         SEXP empty_object,
                         SEXP single_null,
                         const rcppsimdjson::deserialize::Simplify_To simplify_to,
                         const rcppsimdjson::deserialize::Type_Policy type_policy,
                         const rcppsimdjson::utils::Int64_R_Type int64_r_type) {
  const auto n = file_path.length();
  auto out     = Rcpp::List(n);
  simdjson::dom::parser parser;

  for (R_xlen_t i = 0; i < n; ++i) {
    if (file_path[i].get() == NA_STRING) {
      out[i] = Rcpp::CharacterVector(1, NA_STRING);
      continue;
    }

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
        single_null,                                 //
        simplify_to,                                 //
        type_policy,                                 //
        int64_r_type                                 //
    );                                               //
  }

  out.attr("names") = file_path.attr("names");
  return out;
}


// [[Rcpp::export(.load_json)]]
SEXP load_json(const Rcpp::CharacterVector& file_path,
               const std::string& json_pointer = "",
               SEXP empty_array                = R_NilValue,
               SEXP empty_object               = R_NilValue,
               SEXP single_null                = R_NilValue,
               const int simplify_to           = 0,
               const int type_policy           = 0,
               const int int64_r_type          = 0) {
  switch (file_path.length()) {
    case 0:
      return R_NilValue;

    case 1:
      return load_single_file(                                              //
          file_path[0],                                                     //
          json_pointer,                                                     //
          empty_array,                                                      //
          empty_object,                                                     //
          single_null,                                                      //
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
          single_null,                                                      //
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
