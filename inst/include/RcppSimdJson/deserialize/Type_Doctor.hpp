#ifndef RCPPSIMDJSON_TYPE_DOCTOR_HPP
#define RCPPSIMDJSON_TYPE_DOCTOR_HPP


namespace rcppsimdjson {
namespace deserialize {


template <Type_Policy type_policy> class Type_Doctor {
  // clang-format off
   std::array<bool, 16> type_matrix{
      /*    simdjson::dom::element_type     |     rcpp_T                         */
      /* ___________________________________|___________________________________ */
      /*        ARRAY      | */  false,  /* | */  false,        /* | array       */
      /*       OBJECT      | */  false,  /* | */  false,        /* | object      */
      /*       STRING      | */  false,  /* | */  false,        /* | chr         */
      /*       DOUBLE      | */  false,  /* | */  false,        /* | dbl         */
      /*        INT64      | */  false,  /* | */  false,        /* | i64         */
      /*            _      |     _          | */  false,        /* | i32         */
      /*         BOOL      | */  false,  /* | */  false,        /* | lgl         */
      /*   NULL_VALUE      | */  false,  /* | */  false,        /* | null        */
      /*       UINT64      | */  false,  /* |     _                | _           */ };
   enum index : int{ /*     |  ============  |  ============        |             */
                ARRAY   /* | */  = 0,    /* | */  array  = 1,   /* |             */
               OBJECT   /* | */  = 2,    /* | */  object = 3,   /* |             */
               STRING   /* | */  = 4,    /* | */  chr    = 5,   /* |             */
               DOUBLE   /* | */  = 6,    /* | */  dbl    = 7,   /* |             */
                INT64   /* | */  = 8,    /* | */  i64    = 9,   /* |             */
      /*            _      |     _          | */  i32    = 10,  /* |             */
                 BOOL   /* | */  = 11,   /* | */  lgl    = 12,  /* |             */
           NULL_VALUE   /* | */  = 13,   /* | */  null   = 14,  /* |             */
               UINT64   /* | */  = 15,   /* |     _                | _           */
  };
  // clang-format on

  constexpr auto has_ARRAY() const noexcept -> bool { return type_matrix[index::ARRAY]; };
  constexpr auto has_OBJECT() const noexcept -> bool { return type_matrix[index::OBJECT]; };
  constexpr auto has_STRING() const noexcept -> bool { return type_matrix[index::STRING]; };
  constexpr auto has_DOUBLE() const noexcept -> bool { return type_matrix[index::DOUBLE]; };
  constexpr auto has_INT64() const noexcept -> bool { return type_matrix[index::INT64]; };
  constexpr auto has_BOOL() const noexcept -> bool { return type_matrix[index::BOOL]; };
  constexpr auto has_NULL_VALUE() const noexcept -> bool { return type_matrix[index::NULL_VALUE]; };
  constexpr auto has_UINT64() const noexcept -> bool { return type_matrix[index::UINT64]; };


public:
  Type_Doctor() = default;
  Type_Doctor<type_policy>(simdjson::dom::array) noexcept;

  constexpr auto reset() noexcept -> void {
    // clang-format off
    type_matrix = std::array<bool, 16>{false, false, false, false, false, false,
                                       false, false, false, false, false, false,
                                       false, false, false, false};
    // clang-format on
  };

  constexpr auto is_homogeneous() const noexcept -> bool {
    return 1 == static_cast<int>(type_matrix[index::ARRAY])         //
                    + static_cast<int>(type_matrix[index::OBJECT])  //
                    + static_cast<int>(type_matrix[index::STRING])  //
                    + static_cast<int>(type_matrix[index::DOUBLE])  //
                    + static_cast<int>(type_matrix[index::INT64])   //
                    + static_cast<int>(type_matrix[index::BOOL])    //
                    + static_cast<int>(type_matrix[index::UINT64]); //
  };

  constexpr auto has_array() const noexcept -> bool { return type_matrix[index::array]; };
  constexpr auto has_object() const noexcept -> bool { return type_matrix[index::OBJECT]; };
  constexpr auto has_chr() const noexcept -> bool { return type_matrix[index::chr]; };
  constexpr auto has_dbl() const noexcept -> bool { return type_matrix[index::dbl]; };
  constexpr auto has_i64() const noexcept -> bool { return type_matrix[index::i64]; };
  constexpr auto has_i32() const noexcept -> bool { return type_matrix[index::i32]; };
  constexpr auto has_lgl() const noexcept -> bool { return type_matrix[index::lgl]; };
  constexpr auto has_null() const noexcept -> bool { return type_matrix[index::null]; };
  constexpr auto has_u64() const noexcept -> bool { return type_matrix[index::UINT64]; };

  constexpr auto common_R_type() const noexcept -> rcpp_T;
  constexpr auto common_element_type() const noexcept -> simdjson::dom::element_type;
  constexpr auto is_vectorizable() const noexcept -> bool;

  static auto from_set(const std::set<rcpp_T>& type_set) noexcept -> Type_Doctor<type_policy>;
  static auto from_object(simdjson::dom::object type_set) noexcept -> Type_Doctor<type_policy>;

  auto add_element(simdjson::dom::element) noexcept -> void;
};


template <Type_Policy type_policy>
inline Type_Doctor<type_policy>::Type_Doctor(simdjson::dom::array array) noexcept {
  for (auto element : array) {
    switch (element.type()) {
      case simdjson::dom::element_type::ARRAY:
        type_matrix[index::ARRAY] = true;
        type_matrix[index::array] = true;
        break;

      case simdjson::dom::element_type::OBJECT:
        type_matrix[index::OBJECT] = true;
        type_matrix[index::object] = true;
        break;

      case simdjson::dom::element_type::STRING:
        type_matrix[index::STRING] = true;
        type_matrix[index::chr] = true;
        break;

      case simdjson::dom::element_type::DOUBLE:
        type_matrix[index::DOUBLE] = true;
        type_matrix[index::dbl] = true;
        break;

      case simdjson::dom::element_type::INT64: {
        type_matrix[index::INT64] = true;
        if (rcppsimdjson::utils::is_castable_int64(element.get<int64_t>().first)) {
          type_matrix[index::i32] = true;
        } else {
          type_matrix[index::i64] = true;
        }
        break;
      }

      case simdjson::dom::element_type::BOOL:
        type_matrix[index::BOOL] = true;
        type_matrix[index::lgl] = true;
        break;

      case simdjson::dom::element_type::NULL_VALUE:
        type_matrix[index::NULL_VALUE] = true;
        type_matrix[index::null] = true;
        break;

      case simdjson::dom::element_type::UINT64:
        type_matrix[index::UINT64] = true;
        break;
    }
  }
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::strict>::common_R_type() const noexcept -> rcpp_T {
  if (has_object()) {
    return rcpp_T::object;
  }
  if (has_array()) {
    return rcpp_T::array;
  }

  if (has_chr() && !(has_dbl() || has_i64() || has_i32() || has_lgl() || has_u64())) {
    return rcpp_T::chr;
  }
  if (has_dbl() && !(has_i64() || has_i32() || has_lgl() || has_u64())) {
    return rcpp_T::dbl;
  }
  if (has_i64() && !(has_i32() || has_lgl() || has_u64())) {
    return rcpp_T::i64;
  }
  if (has_i32() && !(has_lgl() || has_u64())) {
    return rcpp_T::i32;
  }
  if (has_lgl() && !has_u64()) {
    return rcpp_T::lgl;
  }
  if (has_u64()) {
    return rcpp_T::u64;
  }

  return rcpp_T::null;
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::ints_as_dbls>::common_R_type() const noexcept
    -> rcpp_T {

  if (has_object()) {
    return rcpp_T::object;
  }
  if (has_array()) {
    return rcpp_T::array;
  }

  if (has_chr() && !(has_dbl() || has_i64() || has_i32() || has_lgl() || has_u64())) {
    return rcpp_T::chr;
  }

  if (has_dbl() && !(has_lgl() || has_u64())) { // any number will become double
    return rcpp_T::dbl;
  }
  if (has_i64() && !(has_i32() || has_lgl() || has_u64())) {
    // only 64/32-bit integers: will follow selected Int64_R_Type option
    return rcpp_T::i64;
  }
  if (has_i32() && !(has_lgl() || has_u64())) {
    // only 32-bit integers remaining: will become int
    return rcpp_T::i32;
  }

  if (has_lgl() && !has_u64()) {
    return rcpp_T::lgl;
  }
  if (has_u64()) {
    return rcpp_T::u64;
  }

  return rcpp_T::null;
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::anything_goes>::common_R_type() const noexcept
    -> rcpp_T {

  return has_object()
             ? rcpp_T::object
             : has_array()
                   ? rcpp_T::array
                   : has_chr()
                         ? rcpp_T::chr
                         : has_dbl() ? rcpp_T::dbl
                                     : has_i64() ? rcpp_T::i64
                                                 : has_i32() ? rcpp_T::i32
                                                             : has_lgl() ? rcpp_T::lgl
                                                                         : has_u64() ? rcpp_T::chr
                                                                                     : rcpp_T::null;
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::strict>::is_vectorizable() const noexcept -> bool {
  if (has_object() || has_array()) {
    return false;
  }

  if (has_chr()) {
    return !(has_dbl() || has_i64() || has_i32() || has_lgl() || has_u64());
  }
  if (has_dbl()) {
    return !(has_i64() || has_i32() || has_lgl() || has_u64());
  }
  if (has_i64()) {
    return !(has_i32() || has_lgl() || has_u64());
  }
  if (has_i32()) {
    return !(has_lgl() || has_u64());
  }
  if (has_lgl()) {
    return !has_u64();
  }

  return true; // only u64 and null are left
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::ints_as_dbls>::is_vectorizable() const noexcept
    -> bool {
  if (has_object() || has_array()) {
    return false;
  }

  if (has_chr()) {
    return !(has_dbl() || has_i64() || has_i32() || has_lgl() || has_u64());
  }
  if (has_dbl() || has_i64() || has_i32()) {
    return !(has_lgl() || has_u64());
  }
  if (has_lgl()) {
    return !has_u64();
  }

  return true; // only u64 and null are left
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::anything_goes>::is_vectorizable() const noexcept
    -> bool {
  return !has_object() && !has_array();
}


template <Type_Policy type_policy>
inline constexpr auto Type_Doctor<type_policy>::common_element_type() const noexcept
    -> simdjson::dom::element_type {

  return
      // has_ARRAY() ? simdjson::dom::element_type::ARRAY : has_OBJECT() ?
      // simdjson::dom::element_type::OBJECT :
      has_STRING()
          ? simdjson::dom::element_type::STRING
          : has_DOUBLE()
                ? simdjson::dom::element_type::DOUBLE
                : has_INT64() ? simdjson::dom::element_type::INT64
                              : has_BOOL() ? simdjson::dom::element_type::BOOL
                                           : has_UINT64() ? simdjson::dom::element_type::UINT64
                                                          : simdjson::dom::element_type::NULL_VALUE;
}


template <Type_Policy type_policy>
auto Type_Doctor<type_policy>::add_element(simdjson::dom::element element) noexcept -> void {
  switch (element.type()) {
    case simdjson::dom::element_type::ARRAY:
      type_matrix[index::ARRAY] = true;
      type_matrix[index::array] = true;
      break;

    case simdjson::dom::element_type::OBJECT:
      type_matrix[index::OBJECT] = true;
      type_matrix[index::object] = true;
      break;

    case simdjson::dom::element_type::STRING:
      type_matrix[index::STRING] = true;
      type_matrix[index::chr] = true;
      break;

    case simdjson::dom::element_type::DOUBLE:
      type_matrix[index::DOUBLE] = true;
      type_matrix[index::dbl] = true;
      break;

    case simdjson::dom::element_type::INT64: {
      type_matrix[index::INT64] = true;
      if (rcppsimdjson::utils::is_castable_int64(element.get<int64_t>().first)) {
        type_matrix[index::i32] = true;
      } else {
        type_matrix[index::i64] = true;
      }
      break;
    }

    case simdjson::dom::element_type::BOOL:
      type_matrix[index::BOOL] = true;
      type_matrix[index::lgl] = true;
      break;

    case simdjson::dom::element_type::NULL_VALUE:
      type_matrix[index::NULL_VALUE] = true;
      type_matrix[index::null] = true;
      break;

    case simdjson::dom::element_type::UINT64:
      type_matrix[index::UINT64] = true;
      break;
  }
}


template <Type_Policy type_policy>
inline auto Type_Doctor<type_policy>::from_set(const std::set<rcpp_T>& type_set) noexcept
    -> Type_Doctor<type_policy> {
  auto out = Type_Doctor();

  for (auto element_type : type_set) {
    switch (element_type) {
      case rcpp_T::array:
        out.type_matrix[index::ARRAY] = true;
        out.type_matrix[index::array] = true;
        break;

      case rcpp_T::object:
        out.type_matrix[index::OBJECT] = true;
        out.type_matrix[index::object] = true;
        break;

      case rcpp_T::chr:
        out.type_matrix[index::STRING] = true;
        out.type_matrix[index::chr] = true;
        break;

      case rcpp_T::dbl:
        out.type_matrix[index::DOUBLE] = true;
        out.type_matrix[index::dbl] = true;
        break;

      case rcpp_T::i64:
        out.type_matrix[index::INT64] = true;
        out.type_matrix[index::i64] = true;

      case rcpp_T::i32:
        out.type_matrix[index::INT64] = true;
        out.type_matrix[index::i32] = true;

      case rcpp_T::lgl:
        out.type_matrix[index::BOOL] = true;
        out.type_matrix[index::lgl] = true;
        break;

      case rcpp_T::null:
        out.type_matrix[index::NULL_VALUE] = true;
        out.type_matrix[index::null] = true;
        break;

      case rcpp_T::u64:
        out.type_matrix[index::UINT64] = true;
        break;
    }
  }

  return out;
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
