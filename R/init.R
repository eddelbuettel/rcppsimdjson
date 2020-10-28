
.onAttach <- function(libname, pkgname) {
    if (.cppVersion() < 201703L) {      		# #nocov start
        packageStartupMessage(paste("This package requires a C++17 compiler for compilation but none",
                                    "was found so the package is not fully functional.\nPlease",
                                    "consider reinstalling with a better compiler."))
    }
    if (.unsupportedArchitecture()) {
        packageStartupMessage(paste("This package requires a recent CPU type and supports several ",
                                    "models but not the one on this machine. You will likely ",
                                    "experience failure when trying to parse JSON documents."))
    }							# #nocov end

}
