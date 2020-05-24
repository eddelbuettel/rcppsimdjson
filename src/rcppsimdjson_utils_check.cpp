#include <RcppSimdJson.hpp>

// [[Rcpp::export(.check_int64)]]
SEXP check_int64() {
  using namespace rcppsimdjson::utils;

  constexpr int64_t big_int = 3'000'000'000;
  constexpr uint64_t big_uint = 10'000'000'000'000'000'000UL;

  std::vector<int64_t> big_int_vec({big_int, big_int, big_int, big_int});
  std::vector<uint64_t> big_uint_vec({big_uint, big_uint, big_uint, big_uint});

  using Rcpp::_;
  return Rcpp::List::create(                                                         //
      _["scalar_int64_t"] = Rcpp::List::create(                                      //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(big_int),                //
          _["String"] = resolve_int64<Int64_R_Type::String>(big_int),                //
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(big_int)           //
          ),                                                                         //
      _["scalar_uint64_t"] = Rcpp::List::create(                                     //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(big_uint),               //
          _["String"] = resolve_int64<Int64_R_Type::String>(big_uint),               //
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(big_uint)          //
          ),                                                                         //
      _["vec_int64_t"] = Rcpp::List::create(                                         //
          _["Double"] = resolve_int64_vec<Int64_R_Type::Double>(big_int_vec),        //
          _["String"] = resolve_int64_vec<Int64_R_Type::String>(big_int_vec),        //
          _["Integer64"] = resolve_int64_vec<Int64_R_Type::Integer64>(big_int_vec)   //
          ),                                                                         //
      _["vec_uint64_t"] = Rcpp::List::create(                                        //
          _["Double"] = resolve_int64_vec<Int64_R_Type::Double>(big_uint_vec),       //
          _["String"] = resolve_int64_vec<Int64_R_Type::String>(big_uint_vec),       //
          _["Integer64"] = resolve_int64_vec<Int64_R_Type::Integer64>(big_uint_vec)  //
          )                                                                          //
  );                                                                                 //


  return R_NilValue;
}
