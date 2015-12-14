AC_INIT([zvmail], [0.1-0])

AC_PREREQ([2.69])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror])

AC_CONFIG_FILES([Makefile])

# clear default C++ flags (such as -O2)
: ${CXXFLAGS=""}
AC_PROG_CXX

PKG_CHECK_MODULES(SSL, openssl >= 1.0.1,
                  [],
                  [echo "OpenSSL is required"
                   exit -1])

AC_OUTPUT
