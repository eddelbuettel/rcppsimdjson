#ifndef RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP
#define RCPPSIMDJSON__DESERIALIZE__DATAFRAME_HPP


#include "RcppSimdJson/utils.hpp"
#include "matrix.hpp"


namespace rcppsimdjson {
namespace deserialize {

template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
struct Column {
    R_xlen_t                            index  = 0L;
    Type_Doctor<type_policy, int64_opt> schema = Type_Doctor<type_policy, int64_opt>();
};

template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
struct Column_Schema {
    /* Below this many columns, a linear scan beats hashing (and allocating the hash table). */
    static inline constexpr std::size_t MIN_HASHED_COLUMNS = 16;

    std::unordered_map<std::string_view, R_xlen_t> index; /* only filled for wide schemas */
    std::vector<std::string_view>                  keys;  /* in column order */
    std::vector<Column<type_policy, int64_opt>>    schema;
    /* Every object has exactly the column keys, in column order: field `pos` is column `pos`. */
    bool is_regular = true;

    /* Keys are short: comparing inline beats a call to memcmp(). */
    static inline auto same_key(const std::string_view a, const std::string_view b) noexcept
        -> bool {
        if (std::size(a) != std::size(b)) {
            return false;
        }
        for (std::size_t i = 0; i < std::size(a); ++i) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }

    /* Records are usually written with the same keys in the same order, so we first check whether
     * the key at position `pos` in the object is the key of column `pos`.
     */
    inline auto find(const std::string_view key, const std::size_t pos) const noexcept
        -> R_xlen_t {
        const auto n = std::size(keys);
        if (pos < n && same_key(keys[pos], key)) {
            return static_cast<R_xlen_t>(pos);
        }
        if (n < MIN_HASHED_COLUMNS) {
            for (std::size_t j = 0; j < n; ++j) {
                if (same_key(keys[j], key)) {
                    return static_cast<R_xlen_t>(j);
                }
            }
            return R_xlen_t(-1);
        }
        const auto it = index.find(key);
        return it == std::end(index) ? R_xlen_t(-1) : it->second;
    }

    inline auto add(const std::string_view key) -> R_xlen_t {
        const auto col = static_cast<R_xlen_t>(std::size(keys));
        keys.push_back(key);
        schema.push_back(Column<type_policy, int64_opt>{col, Type_Doctor<type_policy, int64_opt>()});
        if (std::size(keys) == MIN_HASHED_COLUMNS) {
            for (std::size_t j = 0; j < std::size(keys); ++j) {
                index.emplace(keys[j], static_cast<R_xlen_t>(j));
            }
        } else if (std::size(keys) > MIN_HASHED_COLUMNS) {
            index.emplace(key, col);
        }
        return col;
    }
};


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt>
RCPPSIMDJSON_FLATTEN inline auto diagnose_data_frame(simdjson::dom::array array) noexcept(RCPPSIMDJSON_NO_EXCEPTIONS)
    -> std::optional<Column_Schema<type_policy, int64_opt>> {
    // if (std::size(array) == 0) { // already handled in `dispatch_simplify_array()`
    //     return std::nullopt;
    // }

    auto cols = Column_Schema<type_policy, int64_opt>();
    auto first_row_length = std::optional<std::size_t>();

    for (auto element : array) {
        simdjson::dom::object object;
        if (element.get(object) != simdjson::SUCCESS) {
            return std::nullopt;
        }
        if (!first_row_length) {
            cols.keys.reserve(std::size(object));
            cols.schema.reserve(std::size(object));
        }
        auto pos = std::size_t(0ULL);
        for (auto [key, value] : object) {
            auto col = cols.find(key, pos);
            if (col < 0) {
                col = cols.add(key);
            }
            cols.is_regular &= static_cast<std::size_t>(col) == pos++;
            cols.schema[col].schema.add_element(value);
        }
        if (!first_row_length) {
            first_row_length = pos;
        }
        cols.is_regular &= pos == *first_row_length;
    }

    return cols;
}


/* How a column is materialized. */
enum class Col_Kind { chr, dbl, i32, lgl, i64_bits, null, list };


template <utils::Int64_R_Type int64_opt>
inline constexpr auto col_kind(const rcpp_T R_Type) noexcept -> Col_Kind {
    switch (R_Type) {
        case rcpp_T::chr:
        case rcpp_T::u64:
            return Col_Kind::chr;
        case rcpp_T::dbl:
            return Col_Kind::dbl;
        case rcpp_T::i64:
            if constexpr (int64_opt == utils::Int64_R_Type::Double) {
                return Col_Kind::dbl;
            } else if constexpr (int64_opt == utils::Int64_R_Type::String) {
                return Col_Kind::chr;
            } else {
                return Col_Kind::i64_bits;
            }
        case rcpp_T::i32:
            return Col_Kind::i32;
        case rcpp_T::lgl:
            return Col_Kind::lgl;
        case rcpp_T::null:
            return Col_Kind::null;
        default:
            return Col_Kind::list;
    }
}


inline auto get_scalar_i64_bits(simdjson::dom::element element) noexcept -> int64_t {
    switch (element.type()) {
        case simdjson::dom::element_type::INT64:
            return get_scalar<int64_t, rcpp_T::i64, NO_NULLS>(element);
        case simdjson::dom::element_type::BOOL:
            return get_scalar<bool, rcpp_T::i64, NO_NULLS>(element);
        default:
            return NA_INTEGER64;
    }
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
RCPPSIMDJSON_NOINLINE inline void set_list_cell(SEXP                   vec,
                                                const R_xlen_t         i_row,
                                                simdjson::dom::element value,
                                                SEXP                   empty_array,
                                                SEXP                   empty_object,
                                                SEXP                   single_null) {
    SET_VECTOR_ELT(vec,
                   i_row,
                   simplify_element<type_policy, int64_opt, simplify_to>(
                       value, empty_array, empty_object, single_null));
}


template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
RCPPSIMDJSON_FLATTEN inline void fill_data_frame(simdjson::dom::array                         array,
                                                 const Column_Schema<type_policy, int64_opt>& cols,
                                                 const std::vector<Col_Kind>&                 kinds,
                                                 const std::vector<SEXP>&                     vecs,
                                                 SEXP empty_array,
                                                 SEXP empty_object,
                                                 SEXP single_null) {
    auto last_row = std::vector<R_xlen_t>(std::size(kinds), R_xlen_t(-1));
    auto i_row    = R_xlen_t(0L);
    for (auto element : array) {
        auto pos = std::size_t(0ULL);
        for (auto [key, value] : simdjson::dom::object(element)) {
            R_xlen_t j;
            if (cols.is_regular) {
                j = static_cast<R_xlen_t>(pos++);
            } else {
                j = cols.find(key, pos++);
                if (last_row[j] == i_row) {
                    continue;
                }
                last_row[j] = i_row;
            }

            switch (kinds[j]) {
                case Col_Kind::chr:
                    SET_STRING_ELT(vecs[j], i_row, get_scalar_dispatch<STRSXP>(value));
                    break;
                case Col_Kind::dbl:
                    REAL(vecs[j])[i_row] = get_scalar_dispatch<REALSXP>(value);
                    break;
                case Col_Kind::i32:
                    INTEGER(vecs[j])[i_row] = get_scalar_dispatch<INTSXP>(value);
                    break;
                case Col_Kind::lgl:
                    LOGICAL(vecs[j])[i_row] = get_scalar_dispatch<LGLSXP>(value);
                    break;
                case Col_Kind::i64_bits:
                    reinterpret_cast<int64_t*>(REAL(vecs[j]))[i_row] = get_scalar_i64_bits(value);
                    break;
                case Col_Kind::null:
                    break;
                case Col_Kind::list:
                    set_list_cell<type_policy, int64_opt, simplify_to>(
                        vecs[j], i_row, value, empty_array, empty_object, single_null);
                    break;
            }
        }
        ++i_row;
    }
}


/*
 * Builds the data frame in one pass over the rows: each field is routed to its column (see
 * `Column_Schema::find()`), instead of looking up every column in every row with `at_key()`.
 * When an object repeats a key, the first occurrence wins, as with `at_key()`.
 */
template <Type_Policy type_policy, utils::Int64_R_Type int64_opt, Simplify_To simplify_to>
inline auto build_data_frame(simdjson::dom::array                            array,
                             const Column_Schema<type_policy, int64_opt>&    cols,
                             SEXP                                            empty_array,
                             SEXP                                            empty_object,
                             SEXP                                            single_null) -> SEXP {

    const auto n_rows    = R_xlen_t(std::size(array));
    const auto n_cols    = R_xlen_t(std::size(cols.keys));
    auto       out       = Rcpp::List(n_cols);
    auto       out_names = Rcpp::CharacterVector(n_cols);

    auto kinds    = std::vector<Col_Kind>(n_cols);
    auto vecs     = std::vector<SEXP>(n_cols);

    for (R_xlen_t j = 0; j < n_cols; ++j) {
        SET_STRING_ELT(out_names, j, make_charsxp_cached(cols.keys[j]));
        kinds[j] = col_kind<int64_opt>(cols.schema[j].schema.common_R_type());

        SEXP vec = R_NilValue;
        switch (kinds[j]) {
            case Col_Kind::chr:
                vec = Rf_allocVector(STRSXP, n_rows);
                SET_VECTOR_ELT(out, j, vec);
                for (R_xlen_t i = 0; i < n_rows; ++i) {
                    SET_STRING_ELT(vec, i, NA_STRING);
                }
                break;
            case Col_Kind::dbl:
                vec = Rf_allocVector(REALSXP, n_rows);
                SET_VECTOR_ELT(out, j, vec);
                std::fill_n(REAL(vec), n_rows, NA_REAL);
                break;
            case Col_Kind::i32:
                vec = Rf_allocVector(INTSXP, n_rows);
                SET_VECTOR_ELT(out, j, vec);
                std::fill_n(INTEGER(vec), n_rows, NA_INTEGER);
                break;
            case Col_Kind::lgl:
            case Col_Kind::null:
                vec = Rf_allocVector(LGLSXP, n_rows);
                SET_VECTOR_ELT(out, j, vec);
                std::fill_n(LOGICAL(vec), n_rows, NA_LOGICAL);
                break;
            case Col_Kind::i64_bits:
                vec = Rf_allocVector(REALSXP, n_rows);
                SET_VECTOR_ELT(out, j, vec);
                std::fill_n(reinterpret_cast<int64_t*>(REAL(vec)), n_rows, NA_INTEGER64);
                Rf_setAttrib(vec, R_ClassSymbol, Rf_mkString("integer64"));
                break;
            case Col_Kind::list: {
                vec = Rf_allocVector(VECSXP, n_rows);
                SET_VECTOR_ELT(out, j, vec);
                /* missing values have always been an integer NA (`Rcpp::wrap(NA_LOGICAL)`) */
                SEXP na = Rf_ScalarInteger(NA_INTEGER);
                for (R_xlen_t i = 0; i < n_rows; ++i) {
                    SET_VECTOR_ELT(vec, i, na);
                }
                break;
            }
        }
        vecs[j] = vec;
    }

    fill_data_frame<type_policy, int64_opt, simplify_to>(
        array, cols, kinds, vecs, empty_array, empty_object, single_null);

    Rf_setAttrib(out, R_NamesSymbol, out_names);
    /* compact row names, as data.frame() makes them: c(NA_integer_, -n_rows) */
    Rcpp::IntegerVector row_names = Rcpp::IntegerVector::create(NA_INTEGER, -n_rows);
    Rf_setAttrib(out, R_RowNamesSymbol, row_names);
    Rf_setAttrib(out, R_ClassSymbol, Rf_mkString("data.frame"));

    return out;
}


} // namespace deserialize
} // namespace rcppsimdjson


#endif
