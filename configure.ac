#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(rdecay, 1.0, johannes.weissl@gmx.de)
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(rdecay, 1.0)

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
AC_PROG_CC
if test $GCC = yes ; then
    if test $platform = "windows" ; then
        WARN_FLAGS=""
    else
        WARN_FLAGS="-ansi -pedantic -W -Wall"
    fi
fi
AC_SUBST(WARN_FLAGS)

AC_PROG_INSTALL

AM_GNU_GETTEXT

# Checks for libraries.
PKG_CHECK_MODULES(GTK, gtk+-2.0)
GTK_CFLAGS="$GTK_CFLAGS -DGTK_DISABLE_DEPRECATED"
if test $platform = "windows" ; then
        GTK_CFLAGS="$GTK_CFLAGS -mms-bitfields -mno-cygwin -mwindows"
fi

PKG_CHECK_MODULES(GSL, gsl)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

if test $platform = "windows" ; then
    rnd_device=no
else
    rnd_device=yes
fi

AC_ARG_ENABLE(rnd-device,
        AC_HELP_STRING([--enable-rnd-device=DEVICE], [path to random device (default is /dev/urandom)]),
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

if test $platform = "windows" ; then
    crypto_api=yes
else
    crypto_api=no
fi

AC_ARG_ENABLE(cryptoapi,
        AC_HELP_STRING([--disable-cryptoapi], [don't use the Microsoft Crypto API (default is yes)]),
        [crypto_api=$enableval])

if test "$crypto_api" = yes ; then
#    AC_CHECK_HEADER(windows.h, , [crypto_api=no])
    AC_CHECK_HEADERS([windows.h]) AC_CHECK_HEADERS([wincrypt.h], [], [], [#if HAVE_WINDOWS_H # include <wincrypt.h> # endif ])

    AC_CHECK_LIB(advapi32, CryptGenRandom, [false], [crypto_api=no])
#    AC_SEARCH_LIBS([CryptGenRandom], [advapi32], [], [], [windows])
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

AC_CONFIG_FILES([Makefile intl/Makefile po/Makefile.in m4/Makefile 
                 src/Makefile])

AC_OUTPUT

echo
echo "summary"
echo
echo -n "random seed: "
if test $rnd_device != no ; then
    echo "device ($rnd_device)"
elif test $crypto_api = yes ; then
    echo "Microsoft Crypto-API"
else
    echo "time"
fi
echo
