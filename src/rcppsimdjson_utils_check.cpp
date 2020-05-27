#define STRICT_R_HEADERS

#include <RcppSimdJson.hpp>

// [[Rcpp::export(.check_int64)]]
SEXP check_int64() {
  using rcppsimdjson::utils::Int64_R_Type;
  using rcppsimdjson::utils::resolve_int64;

  constexpr int64_t na_integer = std::numeric_limits<int>::min();
  constexpr int64_t castable = std::numeric_limits<int>::max();

  constexpr int64_t big_int = static_cast<int64_t>(std::numeric_limits<int>::max()) + 1;
  constexpr uint64_t big_uint = static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1;

  std::vector<int64_t> uncastable_vec{castable, na_integer};
  std::vector<int64_t> castable_vec{castable, na_integer + 1};

  std::vector<int64_t> big_int_vec{na_integer, big_int};
  std::vector<uint64_t> big_uint_vec{big_uint, big_uint};

  using Rcpp::_;
  return Rcpp::List::create(
      // `NA_INTEGER_equivalent` should NEVER be integers
      _["NA_INTEGER_equivalent"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(na_integer),
          _["String"] = resolve_int64<Int64_R_Type::String>(na_integer),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(na_integer)  //
          ),
      // `castable_int64_t` should ALWAYS be integers
      _["castable_int64_t"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(castable),
          _["String"] = resolve_int64<Int64_R_Type::String>(castable),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(castable)  //
          ),

      // `scalar_uint64_t` follows provided `Int64_R_Type`
      _["scalar_int64_t"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(big_int),
          _["String"] = resolve_int64<Int64_R_Type::String>(big_int),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(big_int)  //
          ),
      // `scalar_uint64_t` should ALWAYS be strings
      _["scalar_uint64_t"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(big_uint),
          _["String"] = resolve_int64<Int64_R_Type::String>(big_uint),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(big_uint)  //
          ),

      // `castable_vec` should ALWAYS be integers
      _["castable_vec"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(castable_vec),
          _["String"] = resolve_int64<Int64_R_Type::String>(castable_vec),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(castable_vec)  //
          ),
      // `uncastable_vec` should NEVER be integers
      _["uncastable_vec"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(uncastable_vec),
          _["String"] = resolve_int64<Int64_R_Type::String>(uncastable_vec),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(uncastable_vec)  //
          ),

      // `vec_int64_t` follows provided `Int64_R_Type`
      _["vec_int64_t"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(big_int_vec),
          _["String"] = resolve_int64<Int64_R_Type::String>(big_int_vec),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(big_int_vec)  //
          ),
      // `vec_uint64_t` should ALWAYS be strings
      _["vec_uint64_t"] = Rcpp::List::create(  //
          _["Double"] = resolve_int64<Int64_R_Type::Double>(big_uint_vec),
          _["String"] = resolve_int64<Int64_R_Type::String>(big_uint_vec),
          _["Integer64"] = resolve_int64<Int64_R_Type::Integer64>(big_uint_vec)  //
          )                                                                      //
  );
}
