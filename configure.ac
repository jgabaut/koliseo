# Define the package name and version
AC_INIT([koliseo], [0.4.3], [jgabaut@github.com])

# Verify automake version and enable foreign option
AM_INIT_AUTOMAKE([foreign -Wall])

# Detect the host system and set PACK_PREFIX accordingly
AC_CANONICAL_HOST
build_linux=no
build_windows=no
build_mac=no
echo "Host os:  $host_os"

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [Enable debug build])],
  [enable_debug=$enableval],
  [enable_debug=no])
AC_ARG_ENABLE([curses],
  [AS_HELP_STRING([--enable-curses], [Enable curses functions])],
  [enable_curses=$enableval],
  [enable_curses=no])
AC_ARG_ENABLE([gulp],
  [AS_HELP_STRING([--enable-gulp], [Enable gulp header])],
  [enable_gulp=$enableval],
  [enable_gulp=no])
AC_ARG_ENABLE([region],
  [AS_HELP_STRING([--enable-region], [Enable region feature])],
  [enable_region=$enableval],
  [enable_region=no])
AC_ARG_ENABLE([title],
  [AS_HELP_STRING([--enable-title], [Enable title feature])],
  [enable_title=$enableval],
  [enable_title=no])
AC_ARG_ENABLE([exper],
  [AS_HELP_STRING([--enable-exper], [Enable experimental features])],
  [enable_exper=$enableval],
  [enable_exper=no])
AM_CONDITIONAL([DEBUG_BUILD], [test "$enable_debug" = "yes"])
AM_CONDITIONAL([CURSES_BUILD], [test "$enable_curses" = "yes"])
AM_CONDITIONAL([GULP_BUILD], [test "$enable_gulp" = "yes"])
AM_CONDITIONAL([REGION_BUILD], [test "$enable_region" = "yes"])
AM_CONDITIONAL([TITLE_BUILD], [test "$enable_title" = "yes"])
AM_CONDITIONAL([EXPER_BUILD], [test "$enable_exper" = "yes"])

# Define the include and library paths based on the host system
case "${host_os}" in
    mingw*)
        echo "Building for mingw32: [$host_cpu-$host_vendor-$host_os]"
        # mingw32 specific flags
        if test "$enable_curses" = "yes"; then
            echo "Building with curses header"
            build_windows=yes
            AC_SUBST([KOLISEO_CFLAGS], ["-I/usr/x86_64-w64-mingw32/include -static -fstack-protector -DNCURSES_STATIC"])
            AC_SUBST([KOLISEO_LDFLAGS], ["-L/usr/x86_64-w64-mingw32/lib -lncursesw"])
        else
            AC_SUBST([KOLISEO_CFLAGS], ["-I/usr/x86_64-w64-mingw32/include -static -fstack-protector"])
            AC_SUBST([KOLISEO_LDFLAGS], ["-L/usr/x86_64-w64-mingw32/lib"])
        fi
        AC_SUBST([CCOMP], ["/usr/bin/x86_64-w64-mingw32-gcc"])
        AC_SUBST([OS], ["w64-mingw32"])
        AC_SUBST([TARGET], ["demo.exe"])
        AC_SUBST([SHARED_LIB], ["libkoliseo.dll"])
    ;;
    darwin*)
        echo "Building for macos: [$host_cpu-$host_vendor-$host_os]"
        build_mac=yes
        # macOS specific flags
        if test "$enable_curses" = "yes"; then
            echo "Building with curses header"
            AC_SUBST([KOLISEO_LDFLAGS], ["-L/opt/homebrew/opt/ncurses/lib -lncurses"])
            AC_SUBST([KOLISEO_CFLAGS], ["-I/opt/homebrew/opt/ncurses/include"])
        else
            AC_SUBST([KOLISEO_LDFLAGS], [""])
            AC_SUBST([KOLISEO_CFLAGS], [""])
        fi
        AC_SUBST([OS], ["darwin"])
        AC_SUBST([TARGET], ["demo"])
        AC_SUBST([SHARED_LIB], ["libkoliseo.so"])
    ;;
    linux*)
        echo "Building for Linux: [$host_cpu-$host_vendor-$host_os]"
        build_linux=yes
        # Linux specific flags
        if test "$enable_curses" = "yes"; then
            echo "Building with curses header"
            AC_SUBST([KOLISEO_LDFLAGS], ["-lncurses"])
        else
            AC_SUBST([KOLISEO_LDFLAGS], [""])
        fi
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
  VERSION="0.4.3"
fi

# Output variables to the config.h header
AC_DEFINE_UNQUOTED([VERSION], ["$VERSION"], [Version number])


AC_CHECK_PROGS([CCOMP], [gcc clang])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_FUNCS([malloc calloc])

# Output the generated files (Makefile and config.h)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
