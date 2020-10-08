\name{NEWS}
\title{News for Package \pkg{RcppSimdJson}}
\newcommand{\ghpr}{\href{https://github.com/eddelbuettel/rcppsimdjson/pull/#1}{##1}}
\newcommand{\ghit}{\href{https://github.com/eddelbuettel/rcppsimdjson/issues/#1}{##1}}

\section{Changes in version 0.1.2 (2020-10-07)}{
  \itemize{
    \item Upgraded to simdjson 0.5.0 (Dirk \ghpr{49})
  }
}

\section{Changes in version 0.1.1 (2020-08-10)}{
  \itemize{
    \item Corrected incorrect file deletion when mixing local and remote files
    (Brendan in \ghpr{34}) closing \ghit{33}.
    \item Added support for raw vectors, compressed files, and compressed
    downloads (Dirk and Brendan in \ghpr{36}, \ghpr{39}, and \ghpr{45} closing
    \ghit{35} and addressing issues raised in \ghit{40} and \ghit{44}).
    \item Examples in two demos are now more self-sufficient (Daniel
    Lemire and Dirk in \ghpr{42}).
    \item Expanded query functionality to include single, flat, and nested queries
    (Brendan in \ghpr{45} closing \ghit{43}).
    \item Split error handling parameters from \code{error_ok}/\code{on_error}
    into \code{parse_error_ok}/\code{on_parse_error} and
    \code{query_error_ok}/\code{on_query_error} (Brendan in \ghpr{45}).
    \item One-line upstream change to address sanitizer error on cast.
  }
}

\section{Changes in version 0.1.0 (2020-07-07)}{
  \itemize{
    \item Upgraded to simdjson 0.4.1 which adds upstream Windows support
    (Dirk in \ghpr{27} closing \ghit{26} and \ghit{14}, plus extensive
    work by Brendan helping upstream with mingw tests).
    \item Upgraded to simdjson 0.4.6 with further upstream improvements
    (Dirk in \ghpr{30}).
    \item Change Travis CI to build matrix over g++ 7, 8, 9, and 10
    (Dirk in \ghpr{31}; and also Brendan in \ghpr{32}).
    \item New JSON functions \code{fparse} and \code{fload}
    (Brendan in \ghpr{32}) closing \ghit{18} and \ghit{10}).
  }
}

\section{Changes in version 0.0.6 (2020-06-25)}{
  \itemize{
    \item Created C++ integer-handling utilities for safe downcasting
    and integer return (Brendan in \ghpr{16} closing \ghit{13}).
    \item New JSON functions \code{.deserialize_json} and
    \code{.load_json} (Brendan in \ghpr{16}, \ghpr{17}, \ghpr{20}, \ghpr{21}).
    \item Upgrade Travis CI to 'bionic', extract package and version
    from \code{DESCRIPTION} (Dirk in \ghpr{23}).
    \item Upgraded to simdjson 0.4.0 (Dirk in \ghpr{25} closing \ghit{24}).
  }
}

\section{Changes in version 0.0.5 (2020-05-23)}{
  \itemize{
    \item Add \code{parseExample} from earlier upstream announcement (Dirk).
    \item Synced with upstream (Brendan in \ghpr{12}) closing \ghit{11}).
    \item Updated example \code{parseExample} to API changes (Brendan).
  }
}

\section{Changes in version 0.0.4 (2020-04-03)}{
  \itemize{
    \item Upgraded to new upstream releases 0.3 and 0.3.1 (Dirk in \ghpr{9}
    closing \ghit{8})
    \item Updated example \code{validateJSON} to API changes.
  }
}

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

