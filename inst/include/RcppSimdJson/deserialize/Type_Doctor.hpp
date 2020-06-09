#ifndef RCPPSIMDJSON__DESERIALIZE__TYPE_DOCTOR_HPP
#define RCPPSIMDJSON__DESERIALIZE__TYPE_DOCTOR_HPP


namespace rcppsimdjson {
namespace deserialize {


enum class Type_Policy : int {
  anything_goes = 0,
  ints_as_dbls = 1,
  strict = 2,
};


template <Type_Policy type_policy> class Type_Doctor {
  bool ARRAY_ = false;
  bool array_ = false;

  bool OBJECT_ = false;
  bool object_ = false;

  bool STRING_ = false;
  bool chr_ = false;

  bool DOUBLE_ = false;
  bool dbl_ = false;

  bool INT64_ = false;
  bool i64_ = false;
  bool i32_ = false;

  bool BOOL_ = false;
  bool lgl_ = false;

  bool NULL_VALUE_ = false;
  bool null_ = false;

  bool UINT64_ = false;
  bool u64_ = false;

public:
  Type_Doctor() = default;
  explicit Type_Doctor<type_policy>(simdjson::dom::array) noexcept;

  constexpr auto reset() noexcept -> void {
    ARRAY_ = false;
    array_ = false;

    OBJECT_ = false;
    object_ = false;

    STRING_ = false;
    chr_ = false;

    DOUBLE_ = false;
    dbl_ = false;

    INT64_ = false;
    i64_ = false;
    i32_ = false;

    BOOL_ = false;
    lgl_ = false;

    NULL_VALUE_ = false;
    null_ = false;

    UINT64_ = false;
    u64_ = false;
  };

  [[nodiscard]] constexpr auto has_ARRAY() const noexcept -> bool { return ARRAY_; };
  [[nodiscard]] constexpr auto has_OBJECT() const noexcept -> bool { return OBJECT_; };
  [[nodiscard]] constexpr auto has_STRING() const noexcept -> bool { return STRING_; };
  [[nodiscard]] constexpr auto has_DOUBLE() const noexcept -> bool { return DOUBLE_; };
  [[nodiscard]] constexpr auto has_INT64() const noexcept -> bool { return INT64_; };
  [[nodiscard]] constexpr auto has_BOOL() const noexcept -> bool { return BOOL_; };
  [[nodiscard]] constexpr auto has_NULL_VALUE() const noexcept -> bool { return NULL_VALUE_; };
  [[nodiscard]] constexpr auto has_UINT64() const noexcept -> bool { return UINT64_; };

  [[nodiscard]] constexpr auto has_array() const noexcept -> bool { return array_; };
  [[nodiscard]] constexpr auto has_object() const noexcept -> bool { return OBJECT_; };
  [[nodiscard]] constexpr auto has_chr() const noexcept -> bool { return chr_; };
  [[nodiscard]] constexpr auto has_dbl() const noexcept -> bool { return dbl_; };
  [[nodiscard]] constexpr auto has_i64() const noexcept -> bool { return i64_; };
  [[nodiscard]] constexpr auto has_i32() const noexcept -> bool { return i32_; };
  [[nodiscard]] constexpr auto has_lgl() const noexcept -> bool { return lgl_; };
  [[nodiscard]] constexpr auto has_null() const noexcept -> bool { return null_; };
  [[nodiscard]] constexpr auto has_u64() const noexcept -> bool { return u64_; };

  [[nodiscard]] constexpr auto common_R_type() const noexcept -> rcpp_T;
  [[nodiscard]] constexpr auto common_element_type() const noexcept -> simdjson::dom::element_type;

  [[nodiscard]] constexpr auto is_homogeneous() const noexcept -> bool;
  [[nodiscard]] constexpr auto is_vectorizable() const noexcept -> bool;

  auto add_element(simdjson::dom::element) noexcept -> void;

  constexpr auto update(Type_Doctor<type_policy>&& type_doctor) noexcept -> void;
};


template <Type_Policy type_policy>
inline Type_Doctor<type_policy>::Type_Doctor(simdjson::dom::array array) noexcept {

  for (auto element : array) {
    switch (element.type()) {
      case simdjson::dom::element_type::ARRAY:
        ARRAY_ = true;
        array_ = true;
        break;

      case simdjson::dom::element_type::OBJECT:
        OBJECT_ = true;
        object_ = true;
        break;

      case simdjson::dom::element_type::STRING:
        STRING_ = true;
        chr_ = true;
        break;

      case simdjson::dom::element_type::DOUBLE:
        DOUBLE_ = true;
        dbl_ = true;
        break;

      case simdjson::dom::element_type::INT64: {
        INT64_ = true;
        if (utils::is_castable_int64(element.get<int64_t>().first)) {
          i32_ = true;
        } else {
          i64_ = true;
        }
        break;
      }

      case simdjson::dom::element_type::BOOL:
        BOOL_ = true;
        lgl_ = true;
        break;

      case simdjson::dom::element_type::NULL_VALUE:
        NULL_VALUE_ = true;
        null_ = true;
        break;

      case simdjson::dom::element_type::UINT64:
        UINT64_ = true;
        u64_ = true;
        break;
    }
  }
}


template <Type_Policy type_policy>
constexpr auto Type_Doctor<type_policy>::is_homogeneous() const noexcept -> bool {
  if (ARRAY_) {
    return !(OBJECT_ || STRING_ || DOUBLE_ || INT64_ || BOOL_ || UINT64_);
  }
  if (OBJECT_) {
    return !(STRING_ || DOUBLE_ || INT64_ || BOOL_ || UINT64_);
  }
  if (STRING_) {
    return !(DOUBLE_ || INT64_ || BOOL_ || UINT64_);
  }
  if (DOUBLE_) {
    return !(INT64_ || BOOL_ || UINT64_);
  }
  if (INT64_) {
    return !(BOOL_ || UINT64_);
  }
  if (BOOL_) {
    return !UINT64_;
  }

  return UINT64_;
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::strict>::common_R_type() const noexcept -> rcpp_T {
  if (object_) {
    return rcpp_T::object;
  }
  if (array_) {
    return rcpp_T::array;
  }

  if (chr_ && !(dbl_ || i64_ || i32_ || lgl_ || u64_)) {
    return rcpp_T::chr;
  }
  if (dbl_ && !(i64_ || i32_ || lgl_ || u64_)) {
    return rcpp_T::dbl;
  }
  if (i64_ && !(i32_ || lgl_ || u64_)) {
    return rcpp_T::i64;
  }
  if (i32_ && !(lgl_ || u64_)) {
    return rcpp_T::i32;
  }
  if (lgl_ && !u64_) {
    return rcpp_T::lgl;
  }
  if (u64_) {
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
