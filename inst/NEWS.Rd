\name{NEWS}
\title{News for Package \pkg{RcppSimdJson}}
\newcommand{\ghpr}{\href{https://github.com/eddelbuettel/rcppsimdjson/pull/#1}{##1}}
\newcommand{\ghit}{\href{https://github.com/eddelbuettel/rcppsimdjson/issues/#1}{##1}}

\section{Changes in version 0.0.3 (2020-03-03)}{
  \itemize{
    \item Sychronized once more with upstream.
    \item Created new C++ function to check for unsupported
    architecture, and report the status on package load.
    \item Only run example and unit tests if supported architecture is
    found. 
    \item Created small \code{configure} script to see if R was built
    with C++17 support, and record it in \code{src/Makevars}.
  }
}

\section{Changes in version 0.0.2 (2020-02-21)}{
  \itemize{
    \item Sychronized with upstream (Dirk in \ghpr{4} and \ghpr{5}).
    \item The R side of \code{validateJSON} now globs the file argument,
    expanding symbols like \code{~} appropriately. 
    \item C++ code in \code{validateJSON} now conditional on C++17
    allowing (incomplete) compilation on lesser systems.
    \item New helper function returning value of \code{__cplusplus}
    macro, used in package startup to warn if insufficient compiler used.
  }
}

\section{Changes in version 0.0.1 (2020-01-24)}{
  \itemize{
    \item Initial CRAN upload of first version
    \item Comment-out use of \code{stdout} (now updated upstream)
    \item Deactivate use computed GOTOs for compiler compliance and CRAN
  Policy via \code{#define}
  } 
}

