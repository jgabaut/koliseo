# Define the package name and version
AC_INIT([koliseo], [0.5.7], [jgabaut@github.com])

# Verify automake version and enable foreign option
AM_INIT_AUTOMAKE([foreign -Wall])

# Detect the host system and set PACK_PREFIX accordingly
AC_CANONICAL_HOST
build_linux=no
build_windows=no
build_mac=no
echo "Host os:  $host_os"

AC_ARG_ENABLE([all],
  [AS_HELP_STRING([--enable-all], [Enable almost all features, (no debug and exper)])],
  [enable_all=$enableval],
  [enable_all=no])
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [Enable debug build])],
  [enable_debug=$enableval],
  [enable_debug=no])
AC_ARG_ENABLE([locate],
  [AS_HELP_STRING([--enable-locate], [Enable location signatures])],
  [enable_locate=$enableval],
  [enable_locate=no])
AC_ARG_ENABLE([exper],
  [AS_HELP_STRING([--enable-exper], [Enable experimental features])],
  [enable_exper=$enableval],
  [enable_exper=no])
AM_CONDITIONAL([DEBUG_BUILD], [test "$enable_debug" = "yes"])
AM_CONDITIONAL([LOCATE_BUILD], [(test "$enable_locate" = "yes") || (test "$enable_all" = "yes")])
AM_CONDITIONAL([EXPER_BUILD], [test "$enable_exper" = "yes"])

# Define the include and library paths based on the host system
case "${host_os}" in
    mingw*)
        echo "Building for mingw32: [$host_cpu-$host_vendor-$host_os]"
        # mingw32 specific flags
        build_windows=yes
        AC_SUBST([KOLISEO_CFLAGS], ["-I/usr/x86_64-w64-mingw32/include -static -fstack-protector"])
        AC_SUBST([KOLISEO_LDFLAGS], ["-L/usr/x86_64-w64-mingw32/lib"])
        AC_SUBST([CCOMP], ["/usr/bin/x86_64-w64-mingw32-gcc"])
        AC_SUBST([OS], ["w64-mingw32"])
        AC_SUBST([TARGET], ["demo.exe"])
        AC_SUBST([SHARED_LIB], ["libkoliseo.dll"])
    ;;
    darwin*)
        echo "Building for macos: [$host_cpu-$host_vendor-$host_os]"
        build_mac=yes
        # macOS specific flags
        AC_SUBST([KOLISEO_LDFLAGS], [""])
        AC_SUBST([KOLISEO_CFLAGS], [""])
        AC_SUBST([OS], ["darwin"])
        AC_SUBST([TARGET], ["demo"])
        AC_SUBST([SHARED_LIB], ["libkoliseo.so"])
    ;;
    linux*)
        echo "Building for Linux: [$host_cpu-$host_vendor-$host_os]"
        build_linux=yes
        # Linux specific flags
        AC_SUBST([KOLISEO_LDFLAGS], [""])
        AC_SUBST([KOLISEO_CFLAGS], [""])
        AC_SUBST([OS], ["Linux"])
        AC_SUBST([TARGET], ["demo"])
        AC_SUBST([SHARED_LIB], ["libkoliseo.so"])
    ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
    ;;
esac

AM_CONDITIONAL([WINDOWS_BUILD], [test "$build_windows" = "yes"])
AM_CONDITIONAL([DARWIN_BUILD], [test "$build_mac" = "yes"])
AM_CONDITIONAL([LINUX_BUILD], [test "$build_linux" = "yes"])

# Set a default version number if not specified externally
AC_ARG_VAR([VERSION], [Version number])
if test -z "$VERSION"; then
  VERSION="0.5.7"
fi

# Output variables to the config.h header
AC_DEFINE_UNQUOTED([VERSION], ["$VERSION"], [Version number])


AC_CHECK_PROGS([CCOMP], [gcc clang])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_FUNCS([malloc calloc])

# Output the generated files (Makefile and config.h)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
