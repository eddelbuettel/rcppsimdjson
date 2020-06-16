#ifndef RCPPSIMDJSON__DESERIALIZE__TYPE_DOCTOR_HPP
#define RCPPSIMDJSON__DESERIALIZE__TYPE_DOCTOR_HPP

#include "../common.hpp"


namespace rcppsimdjson {
namespace deserialize {


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

  constexpr auto update(Type_Doctor<type_policy>&&) noexcept -> void;
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

  if (object_) {
    return rcpp_T::object;
  }
  if (array_) {
    return rcpp_T::array;
  }

  if (chr_ && !(dbl_ || i64_ || i32_ || lgl_ || u64_)) {
    return rcpp_T::chr;
  }
  
  if (dbl_ && !(lgl_ || u64_)) { // any number will become double
    return rcpp_T::dbl;
  }
  if (i64_ && !(lgl_ || u64_)) {
    // only 64/32-bit integers: will follow selected Int64_R_Type option
    return rcpp_T::i64;
  }
  if (i32_ && !(lgl_ || u64_)) { // only 32-bit integers remaining: will become int
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
inline constexpr auto Type_Doctor<Type_Policy::anything_goes>::common_R_type() const noexcept
    -> rcpp_T {

  return object_ ? rcpp_T::object                                                   //
                 : array_ ? rcpp_T::array                                           //
                          : chr_ ? rcpp_T::chr                                      //
                                 : dbl_ ? rcpp_T::dbl                               //
                                        : i64_ ? rcpp_T::i64                        //
                                               : i32_ ? rcpp_T::i32                 //
                                                      : lgl_ ? rcpp_T::lgl          //
                                                             : u64_ ? rcpp_T::u64   //
                                                                    : rcpp_T::null; //
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::strict>::is_vectorizable() const noexcept -> bool {
  if (object_ || array_) {
    return false;
  }

  if (chr_) {
    return !(dbl_ || i64_ || i32_ || lgl_ || u64_);
  }
  if (dbl_) {
    return !(i64_ || i32_ || lgl_ || u64_);
  }
  if (i64_) {
    return !(i32_ || lgl_ || u64_);
  }
  if (i32_) {
    return !(lgl_ || u64_);
  }
  if (lgl_) {
    return !u64_;
  }

  return u64_;
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::ints_as_dbls>::is_vectorizable() const noexcept
    -> bool {

  if (object_ || array_) {
    return false;
  }

  if (chr_) {
    return !(dbl_ || i64_ || i32_ || lgl_ || u64_);
  }
  if (dbl_ || i64_ || i32_) {
    return !(lgl_ || u64_);
  }
  if (lgl_) {
    return !u64_;
  }

  return u64_;
}


template <>
inline constexpr auto Type_Doctor<Type_Policy::anything_goes>::is_vectorizable() const noexcept
    -> bool {

  return !(object_ || array_);
}


template <Type_Policy type_policy>
inline constexpr auto Type_Doctor<type_policy>::common_element_type() const noexcept
    -> simdjson::dom::element_type {

  using simdjson::dom::element_type;

  return ARRAY_ ? element_type::ARRAY
                : OBJECT_ ? element_type::OBJECT
                          : STRING_ ? element_type::STRING
                                    : DOUBLE_ ? element_type::DOUBLE
                                              : INT64_ ? element_type::INT64
                                                       : BOOL_ ? element_type::BOOL
                                                               : UINT64_ ? element_type::UINT64
                                                                         : element_type::NULL_VALUE;
}


template <Type_Policy type_policy>
auto Type_Doctor<type_policy>::add_element(simdjson::dom::element element) noexcept -> void {
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


template <Type_Policy type_policy>
inline constexpr auto
Type_Doctor<type_policy>::update(Type_Doctor<type_policy>&& type_doctor2) noexcept -> void {
  this->ARRAY_ = this->ARRAY_ || type_doctor2.ARRAY_;
  this->array_ = this->array_ || type_doctor2.array_;

  this->OBJECT_ = this->OBJECT_ || type_doctor2.OBJECT_;
  this->object_ = this->object_ || type_doctor2.object_;

  this->STRING_ = this->STRING_ || type_doctor2.STRING_;
  this->chr_ = this->chr_ || type_doctor2.chr_;

  this->DOUBLE_ = this->DOUBLE_ || type_doctor2.DOUBLE_;
  this->dbl_ = this->dbl_ || type_doctor2.dbl_;

  this->INT64_ = this->INT64_ || type_doctor2.INT64_;
  this->i64_ = this->i64_ || type_doctor2.i64_;

  this->i32_ = this->i32_ || type_doctor2.i32_;
  this->BOOL_ = this->BOOL_ || type_doctor2.BOOL_;

  this->lgl_ = this->lgl_ || type_doctor2.lgl_;
  this->NULL_VALUE_ = this->NULL_VALUE_ || type_doctor2.NULL_VALUE_;

  this->null_ = this->null_ || type_doctor2.null_;

  this->UINT64_ = this->UINT64_ || type_doctor2.UINT64_;
  this->u64_ = this->u64_ || type_doctor2.u64_;
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
