#ifndef RCPPSIMDJSON__DESERIALIZE__TYPE_DOCTOR_HPP
#define RCPPSIMDJSON__DESERIALIZE__TYPE_DOCTOR_HPP

#include "../common.hpp"
#include "RcppSimdJson/utils.hpp"
#include "simdjson.h"


namespace rcppsimdjson {
namespace deserialize {


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
class Type_Doctor {
    bool ARRAY_ = false;
    bool array_ = false;

    bool OBJECT_ = false;
    bool object_ = false;

    bool STRING_ = false;
    bool chr_    = false;

    bool NUMBER_ = false;
    bool num_    = false;

    bool BOOL_ = false;
    bool lgl_  = false;

    bool NULL_VALUE_ = false;
    bool null_       = false;


  public:
    Type_Doctor() = default;
    explicit Type_Doctor<type_policy, int64_opt>(simdjson::ondemand::array) noexcept;

    [[nodiscard]] constexpr auto has_null() const noexcept -> bool { return null_; };

    [[nodiscard]] constexpr auto common_R_type() const noexcept -> rcpp_T;
    [[nodiscard]] constexpr auto common_element_type() const noexcept
        -> simdjson::ondemand::json_type;

    [[nodiscard]] constexpr auto is_homogeneous() const noexcept -> bool;
    [[nodiscard]] constexpr auto is_vectorizable() const noexcept -> bool;

    auto add_element(simdjson::ondemand::value) noexcept -> void;

    constexpr auto update(Type_Doctor<type_policy, int64_opt>&&) noexcept -> void;
};


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline Type_Doctor<type_policy, int64_opt>::Type_Doctor(simdjson::ondemand::array array) noexcept {
    for (auto element : array) {
        switch (element.type()) {
            case simdjson::ondemand::json_type::array:
                ARRAY_ = true;
                array_ = true;
                break;

            case simdjson::ondemand::json_type::object:
                OBJECT_ = true;
                object_ = true;
                break;

            case simdjson::ondemand::json_type::string:
                STRING_ = true;
                chr_    = true;
                break;

            case simdjson::ondemand::json_type::number:
                NUMBER_ = true;
                num_    = true;
                break;

            case simdjson::ondemand::json_type::boolean:
                BOOL_ = true;
                lgl_  = true;
                break;

            case simdjson::ondemand::json_type::null:
                NULL_VALUE_ = true;
                null_       = true;
                break;
        }
    }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
constexpr bool Type_Doctor<type_policy, int64_opt>::is_homogeneous() const noexcept {
    if (ARRAY_) {
        return !(OBJECT_ || STRING_ || NUMBER_ || BOOL_); // # nocov
    }
    if (OBJECT_) {
        return !(STRING_ || NUMBER_ || BOOL_); // # nocov
    }
    if (STRING_) {
        return !(NUMBER_ || BOOL_);
    }
    if (NUMBER_) {
        return !(BOOL_);
    }

    return BOOL_;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline constexpr rcpp_T Type_Doctor<type_policy, int64_opt>::common_R_type() const noexcept {
    if (object_) {
        return rcpp_T::object; // # nocov
    }
    if (array_) {
        return rcpp_T::array; // # nocov
    }

    if constexpr (type_policy == Type_Policy::anything_goes) {
        return chr_ ? rcpp_T::chr
                           : num_ ? rcpp_T::dbl
                                        : lgl_ ? rcpp_T::lgl : rcpp_T::null;

    } else {
        if (chr_ && !(num_ || lgl_)) {
            return rcpp_T::chr;
        }

        if constexpr (type_policy == Type_Policy::strict) {
            if (num_ && !(lgl_)) {
                return rcpp_T::dbl;
            }
        }

        if constexpr (type_policy == Type_Policy::ints_as_dbls) {
            if (num_ && !(lgl_)) { // any number will become double
                return rcpp_T::dbl;
            }
        }
    }


    if (lgl_) {
        return rcpp_T::lgl;
    }

    return rcpp_T::null;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline constexpr auto Type_Doctor<type_policy, int64_opt>::is_vectorizable() const noexcept
    -> bool {
    if constexpr (type_policy == Type_Policy::anything_goes) {
        return !(object_ || array_);
    } else {
        if (object_ || array_) {
            return false;
        }
    }

    if (chr_) {
        return !(num_ || lgl_);
    }

    if constexpr (type_policy == Type_Policy::strict) {
        if (num_) {
            return !(lgl_);
        }
    }

    if constexpr (type_policy == Type_Policy::ints_as_dbls) {
        if (num_) {
            return !(lgl_);
        }
    }

    return lgl_;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline constexpr simdjson::ondemand::json_type Type_Doctor<type_policy, int64_opt>::common_element_type() const noexcept {

    using simdjson::ondemand::json_type;

    return ARRAY_
               ?json_type::array
               : OBJECT_ ?json_type::object
                         : STRING_ ?json_type::string
                                             : NUMBER_ ?json_type::number
                                                                : BOOL_ ?json_type::boolean
                                                                        :json_type::null;
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
void Type_Doctor<type_policy, int64_opt>::add_element(simdjson::ondemand::value element) noexcept {
    switch (element.type()) {
        case simdjson::ondemand::json_type::array:
            ARRAY_ = true;
            array_ = true;
            break;

        case simdjson::ondemand::json_type::object:
            OBJECT_ = true;
            object_ = true;
            break;

        case simdjson::ondemand::json_type::string:
            STRING_ = true;
            chr_    = true;
            break;

        case simdjson::ondemand::json_type::number:
            NUMBER_ = true;
            num_    = true;
            break;

        case simdjson::ondemand::json_type::boolean:
            BOOL_ = true;
            lgl_  = true;
            break;

        case simdjson::ondemand::json_type::null:
            NULL_VALUE_ = true;
            null_       = true;
            break;
    }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
inline constexpr void Type_Doctor<type_policy, int64_opt>::update(
    Type_Doctor<type_policy, int64_opt>&& type_doctor2) noexcept {
    this->ARRAY_ = this->ARRAY_ || type_doctor2.ARRAY_;
    this->array_ = this->array_ || type_doctor2.array_;

    this->OBJECT_ = this->OBJECT_ || type_doctor2.OBJECT_;
    this->object_ = this->object_ || type_doctor2.object_;

    this->STRING_ = this->STRING_ || type_doctor2.STRING_;
    this->chr_    = this->chr_ || type_doctor2.chr_;

    this->NUMBER_ = this->NUMBER_ || type_doctor2.NUMBER_;
    this->num_    = this->num_ || type_doctor2.num_;

    this->BOOL_ = this->BOOL_ || type_doctor2.BOOL_;
    this->lgl_  = this->lgl_ || type_doctor2.lgl_;

    this->NULL_VALUE_ = this->NULL_VALUE_ || type_doctor2.NULL_VALUE_;
    this->null_       = this->null_ || type_doctor2.null_;
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
