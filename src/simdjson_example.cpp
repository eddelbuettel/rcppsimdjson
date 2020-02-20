
#define STRICT_R_HEADERS
#include <Rcpp.h>

#include "simdjson.h"
#include "simdjson.cpp"

using namespace simdjson;

// [[Rcpp::export(.validateJSON)]]
bool validateJSON(const std::string filename) {
#if __cplusplus >= 201703L
  simdjson::padded_string p = get_corpus(filename.c_str());
  simdjson::ParsedJson pj = simdjson::build_parsed_json(p); // do the parsing
  if ( !pj.is_valid() ) {
    // something went wrong
    Rcpp::stop(pj.get_error_message());
  }
  return true;
#else
  Rcpp::stop("The package 'SystemRequirements:' state a need for a C++17 compiler.");
  return true; // not reached
#endif
}

// [[Rcpp::export(.cppVersion)]]
int cppVersion() {
  return __cplusplus;
}
