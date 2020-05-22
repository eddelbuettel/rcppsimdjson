#include <RcppSimdJson.hpp>

// [[Rcpp::export(.check_int64)]]
SEXP check_int64(const bool is_signed, const int integer64) {
  using namespace rcppsimdjson::utils;

  const auto int64_type = static_cast<Int64_R_Type>(integer64);

  if (is_signed) {
    constexpr int64_t big_int = 3'000'000'000;

    switch (int64_type) {
      case Int64_R_Type::Double:
        return resolve_int64<int64_t, Int64_R_Type::Double>(big_int);
      case Int64_R_Type::String:
        return resolve_int64<int64_t, Int64_R_Type::String>(big_int);
      case Int64_R_Type::Integer64:
        return resolve_int64<int64_t, Int64_R_Type::Integer64>(big_int);
    }

  } else {
    constexpr uint64_t big_int = 3'000'000'000;

    switch (int64_type) {
      case Int64_R_Type::Double:
        return resolve_int64<uint64_t, Int64_R_Type::Double>(big_int);
      case Int64_R_Type::String:
        return resolve_int64<uint64_t, Int64_R_Type::String>(big_int);
      case Int64_R_Type::Integer64:
        return resolve_int64<uint64_t, Int64_R_Type::Integer64>(big_int);
    }
  }

  return R_NilValue;
}
