AC_INIT([zvmail], [0.1-0])

AC_PREREQ([2.69])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_FILES([Makefile])

# clear default C++ flags (such as -O2)
: ${CXXFLAGS=""}
AC_PROG_CXX
AC_PROG_RANLIB

PKG_CHECK_MODULES(SSL, openssl >= 1.0.1,
                  [],
                  [echo "OpenSSL is required"
                   exit -1])

PKG_CHECK_MODULES(PCRE, libpcrecpp >= 8.31,
                  [],
                  [echo "libpcre3-dev >= 8.31 is required"
                   exit -1])

AC_ARG_VAR(GTEST_DIR,
           Google Test Framework directory)

AS_IF([test "$GTEST_DIR" != ""],
      [echo "Google Test Framework found"],
      [echo "Google Test Framework not found"
       echo -e "\tYou must pass GTEST_DIR=<dir> to compile testcases"])
AM_CONDITIONAL([GOOGLE_TEST], [test "$GTEST_DIR" != ""])

AC_ARG_VAR(GMOCK_DIR,
           Google Mock Framework directory)

AS_IF([test "$GMOCK_DIR" != ""],
      [echo "Google Mock Framework found"],
      [echo "Google Mock Framework not found"
       echo -e "\tYou must pass GMOCK_DIR=<dir> to compile testcases"])
AM_CONDITIONAL([GOOGLE_MOCK], [test "$GMOCK_DIR" != ""])

AC_OUTPUT
