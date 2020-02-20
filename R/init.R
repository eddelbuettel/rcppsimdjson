
.onAttach <- function(libname, pkgname) {
    if (.cppVersion() < 201703L) {
        packageStartupMessage(paste("This package requires a C++17 compiler for compilation but none",
                                    "was found so the package is not fully functional.\nPlease",
                                    "consider reinstalling with a better compiler."))
    }
}
