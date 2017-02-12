#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(rdecay, 1.1.1, jargon@molb.org)
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST
case $host_os in
    *linux*)
        platform="linux"
        AC_DEFINE_UNQUOTED(OS_LINUX, 1, [OS is Linux.])
        ;;
    *msdos* | *go32* | *mingw* | *cygwin* | *windows*)
        platform="windows"
        AC_DEFINE_UNQUOTED(OS_WINDOWS, 1, [OS is Windows.])
        ;;
    *)
        platform=""
        ;;
esac

# Checks for programs.
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
if test "$GCC" = yes ; then
    if test "$platform" = "windows" ; then
        WARN_FLAGS=""
    else
        WARN_FLAGS="-W -Wall"
    fi
fi
AC_SUBST(WARN_FLAGS)
AC_PROG_INSTALL
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION([0.19])

# Checks for libraries.
PKG_CHECK_MODULES(GTK, gtk+-2.0)
if test "$platform" = "windows" ; then
        GTK_CFLAGS="$GTK_CFLAGS -mms-bitfields -mno-cygwin -mwindows"
fi

PKG_CHECK_MODULES(GSL, gsl)

# Checks for header files.
AC_HEADER_TIME
AC_CHECK_HEADERS([sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

if test "$platform" = "windows" ; then
    rnd_device=no
else
    rnd_device=yes
fi

AC_ARG_ENABLE(rnd-device,
        AC_HELP_STRING([--enable-rnd-device=DEVICE],
        [path to random device (default is /dev/urandom)]),
        [rnd_device=$enableval])

if test "$rnd_device" = yes ; then
    rnd_device="/dev/urandom"
fi

if test "$rnd_device" != no ; then
    AC_MSG_CHECKING([for random device $rnd_device])
    if test -r "$rnd_device" ; then
        AC_DEFINE_UNQUOTED(RND_DEVICE, "$rnd_device", [Path to random device.])
        AC_MSG_RESULT(yes)
    else
        rnd_device=no
        AC_MSG_RESULT(no)
    fi
else
    AC_MSG_NOTICE([Random device disabled])
fi

if test "$platform" = "windows" ; then
    crypto_api=yes
else
    crypto_api=no
fi

AC_ARG_ENABLE(cryptoapi,
        AC_HELP_STRING([--disable-cryptoapi],
        [don't use the Microsoft Crypto API (default is yes)]),
        [crypto_api=$enableval])

if test "$crypto_api" = yes ; then
    AC_CHECK_HEADERS([windows.h], [], [crypto_api=no])
    AC_CHECK_HEADERS([wincrypt.h], [], [crypto_api=no],
[#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif])
    AC_MSG_CHECKING([for MS Crypto API])
    if test "$crypto_api" = yes ; then
        AC_DEFINE_UNQUOTED(MS_CRYPTO_API, 1, [Use the Microsoft Crypto API.])
        AC_MSG_RESULT(yes)
    else
        crypto_api=no
        AC_MSG_RESULT(no)
    fi
else
    AC_MSG_NOTICE([Microsoft Crypto API disabled])
fi

AC_CONFIG_FILES([Makefile
                 intl/Makefile
                 po/Makefile.in
                 lib/Makefile
                 src/Makefile
                 doc/Makefile
                 doc/de/Makefile])

AC_OUTPUT

echo
echo "summary"
echo
echo -n "random seed: "
if test "$rnd_device" != no ; then
    echo "device ($rnd_device)"
elif test "$crypto_api" = yes ; then
    echo "Microsoft Crypto-API"
else
    echo "time"
fi
echo
