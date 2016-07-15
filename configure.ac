##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [2])

m4_define([v_ver], [v_maj.v_min.v_mic])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)

AC_INIT([gotham-module-services], [v_ver], [guillaume@friloux.me])

AC_PREREQ(2.01)
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_PROG_CXX
AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_PROG_CC
AM_PROG_CC_C_O

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "${MODULE_ARCH}", "Module architecture")

EFL_CHECK_FUNCS([services], [fnmatch])
EFL_CHECK_TESTS([services], [enable_tests="yes"], [enable_tests="no"])

if test "x${enable_tests}" = "xyes" ; then
   CFLAGS="-W -Wall -g -O0 -DDEBUG -fprofile-arcs -ftest-coverage"
   LDFLAGS="-lgcov -rdynamic -fvisibility=default"
else
   CFLAGS="$CFLAGS -W -Wall -O2"
   LDFLAGS="-Wl,--as-needed -rdynamic -fvisibility=default"
fi

AC_CONFIG_FILES([
Makefile
])

SERVICES_REQUIRES=

case "$host_os" in
  mingw*)
    build_windows="yes"
    SERVICES_REQUIRES="evil"
    AC_DEFINE([HAVE_EVIL], [1], [Set to 1 if Evil package is installed])
  ;;
  *)
    build_unix="yes"
  ;;
esac

SERVICES_REQUIRES="${SERVICES_REQUIRES} eina ecore shotgun gotham"
PKG_CHECK_MODULES(SERVICES, [$SERVICES_REQUIRES], [build_services=yes], [build_services=no])

AM_CONDITIONAL(BUILD_SERVICES, [test "x${build_services}" = "xyes"])
AM_CONDITIONAL(BUILD_WINDOWS, [test "x${build_windows}" = "xyes"])
AM_CONDITIONAL(BUILD_UNIX, [test "x${build_unix}" = "xyes"])

AC_SUBST([SERVICES_REQUIRES])
AC_SUBST([SERVICES_CFLAGS])
AC_SUBST([SERVICES_LIBS])

AC_CONFIG_HEADER([config.h])

AC_OUTPUT([
])

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  prefix..........: $prefix"
echo "  tests...........: ${enable_tests} (Coverage: ${efl_enable_coverage})"
echo "  CFLAGS..........: $CFLAGS"
echo "  LDFLAGS.........: $LDFLAGS"
