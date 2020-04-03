
#define STRICT_R_HEADERS
#include <Rcpp.h>

#if __cplusplus >= 201703L
#include "simdjson.h"
#include "simdjson.cpp"
#endif

// [[Rcpp::export(.validateJSON)]]
bool validateJSON(const std::string filename) {
#if __cplusplus >= 201703L
  simdjson::dom::parser parser;
  auto [doc, error] = parser.load(filename.c_str()); // do the parsing
  if ( error ) {
    // something went wrong
    Rcpp::stop(error_message(error));
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

// [[Rcpp::export(.unsupportedArchitecture)]]
bool unsupportedArchitecture() {
  return (simdjson::active_implementation->name() == "unsupported");
}
