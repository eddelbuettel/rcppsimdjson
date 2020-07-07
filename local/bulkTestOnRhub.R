#!/usr/bin/env Rscript

library(rhub)

selected <- c("debian-clang-devel",
              "debian-gcc-devel",
              "fedora-clang-devel",
              "fedora-gcc-devel",
              #"ubuntu-gcc-devel",  ## uses 16.04 and compiler groans as not C++17 ready
              "macos-highsierra-release",
              "macos-highsierra-release-cran",
              "windows-x86_64-devel",
              "windows-x86_64-release")

check(".", platform = selected, email = getOption("email", "edd@debian.org"))
