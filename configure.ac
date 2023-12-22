# Define the package name and version
AC_INIT([koliseo], [0.3.19], [jgabaut@github.com])

# Verify automake version and enable foreign option
AM_INIT_AUTOMAKE([foreign -Wall])

# Detect the host system and set PACK_PREFIX accordingly
AC_CANONICAL_HOST
echo "Host os:  $host_os"
AM_CONDITIONAL([OS_DARWIN], [test "$host_os" = "darwin"])
AM_CONDITIONAL([MINGW32_BUILD], [test "$host_os" = "mingw32"])
#
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
AM_CONDITIONAL([DEBUG_BUILD], [test "$enable_debug" = "yes"])
AM_CONDITIONAL([CURSES_BUILD], [test "$enable_curses" = "yes"])
AM_CONDITIONAL([GULP_BUILD], [test "$enable_gulp" = "yes"])

# Define the include and library paths based on the host system
if test "$host_os" = "mingw32"; then
  echo "Building for mingw32: [$host_cpu-$host_vendor-$host_os]"
  # mingw32 specific flags
  if test "$enable_curses" = "yes"; then
    echo "Building with curses header"
    AC_SUBST([KOLISEO_CFLAGS], ["-I/usr/x86_64-w64-mingw32/include -static -fstack-protector -DMINGW32_BUILD -DNCURSES_STATIC"])
    AC_SUBST([KOLISEO_LDFLAGS], ["-L/usr/x86_64-w64-mingw32/lib -lncursesw"])
  else
    AC_SUBST([KOLISEO_CFLAGS], ["-I/usr/x86_64-w64-mingw32/include -static -fstack-protector -DMINGW32_BUILD"])
    AC_SUBST([KOLISEO_LDFLAGS], ["-L/usr/x86_64-w64-mingw32/lib"])
  fi
  AC_SUBST([CCOMP], ["/usr/bin/x86_64-w64-mingw32-gcc"])
  AC_SUBST([OS], ["w64-mingw32"])
  AC_SUBST([TARGET], ["demo.exe"])
fi
if test "$host_os" = "darwin"; then
  echo "Building for macos: [$host_cpu-$host_vendor-$host_os]"
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
fi
if test "$host_os" = "linux-gnu"; then
  echo "Building for Linux: [$host_cpu-$host_vendor-$host_os]"
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
fi

# Set a default version number if not specified externally
AC_ARG_VAR([VERSION], [Version number])
if test -z "$VERSION"; then
  VERSION="0.3.19"
fi

# Output variables to the config.h header
AC_DEFINE_UNQUOTED([VERSION], ["$VERSION"], [Version number])


AC_CHECK_PROGS([CCOMP], [gcc clang])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_FUNCS([malloc calloc])

# Output the generated files (Makefile and config.h)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
