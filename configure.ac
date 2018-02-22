AC_INIT([fb64],[1.0.0])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CC
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX_17

AC_ARG_ENABLE([benchmarks],
              [AS_HELP_STRING([--enable-benchmarks],[Enable benchmarks])],
              [case "${enableval}" in
                  yes) benchmarks=true;;
              esac])
AM_CONDITIONAL([ENABLE_BENCHMARKS],[test x$benchmarks = xtrue])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
