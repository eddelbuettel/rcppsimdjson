#include <RcppSimdJson/utils.hpp>

// [[Rcpp::export(.is_valid_json_arg)]]
bool is_valid_json_arg(SEXP json) {
    switch (TYPEOF(json)) {
        case STRSXP:
            [[fallthrough]];
        case RAWSXP:
            return Rf_xlength(json) > 0;
        case VECSXP: {
            for (auto&& element : Rcpp::List(json)) {
                if (TYPEOF(element) != RAWSXP) {
                    return false;
                }
            }
            return true;
        }
        default:
            return false;
    }
}


// [[Rcpp::export(.is_valid_query_arg)]]
bool is_valid_query_arg(SEXP query) {
    switch (TYPEOF(query)) {
        case NILSXP:
            return true;
        case STRSXP:
            return Rf_xlength(query) > 0;
        case VECSXP: {
            if (Rf_xlength(query) == 0) {
                return false;   // #nocov
            } else {
                for (auto&& element : Rcpp::List(query)) {
                    if (TYPEOF(element) != STRSXP) {
                        return false;
                    }
                }
                return true;
            }
        }
        default:
            return false;
    }
}


// [[Rcpp::export(.diagnose_input)]]
SEXP diagnose_input(const Rcpp::CharacterVector& x) {
    const R_xlen_t n = std::size(x);

    Rcpp::CharacterVector url_prefix(n, NA_STRING);
    Rcpp::CharacterVector file_ext(n);
    Rcpp::CharacterVector type(n, NA_STRING);
    Rcpp::LogicalVector   is_from_url(n, false);
    Rcpp::LogicalVector   is_local_file_url(n, false);
    Rcpp::LogicalVector   is_remote_file_url(n, false);

    for (R_xlen_t i = 0; i < n; ++i) {
        if (x[i].get() != NA_STRING) {
            const auto str = std::string_view(x[i]);
            if (const auto a_url_prefix = rcppsimdjson::utils::get_url_prefix(str)) {
                url_prefix[i]  = Rcpp::String(std::string(*a_url_prefix));
                is_from_url[i] = true;
                if (a_url_prefix == std::string_view("file://")) {
                    is_local_file_url[i] = true;
                } else {
                    is_remote_file_url[i] = true; // # nocov
                }
            }
            if (const auto a_file_ext = rcppsimdjson::utils::get_file_ext(str)) {
                file_ext[i] = Rcpp::String(std::string(*a_file_ext));
            }
        }
    }

    using Rcpp::_;
    Rcpp::List out = Rcpp::List::create(_["input"]              = x,
                                        _["url_prefix"]         = url_prefix,
                                        _["file_ext"]           = file_ext,
                                        _["is_from_url"]        = is_from_url,
                                        _["is_local_file_url"]  = is_local_file_url,
                                        _["is_remote_file_url"] = is_remote_file_url);

    out.attr("class")     = "data.frame";
    out.attr("row.names") = Rcpp::seq_len(n);
    return out;
}
