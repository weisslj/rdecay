#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(zerfall, 0.0, jargon@hurg.org)
# AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(zerfall, 0.0)

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
PKG_CHECK_MODULES(GTK, gtk+-2.0)
case $host_os in
    *msdos* | *go32* | *mingw* | *cygwin* | *windows*)
        GTK_CFLAGS="$GTK_CFLAGS -mms-bitfields"
        ;;
esac

PKG_CHECK_MODULES(GSL, gsl)

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

rnd_device=yes
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
        AC_MSG_RESULT(no)
    fi
else
    AC_MSG_NOTICE([Random device disabled])
fi

crypto_api=yes
AC_ARG_ENABLE(cryptoapi,
        AC_HELP_STRING([--disable-cryptoapi], [don't use the Microsoft Crypto API (default is yes)]),
        [crypto_api=$enableval])

if test "$crypto_api" = yes ; then
    AC_CHECK_HEADER(windows.h, , [crypto_api=no])
    AC_CHECK_HEADER(wincrypt.h, , [crypto_api=no])
    AC_CHECK_LIB(advapi32, CryptGenRandom, [false], [crypto_api=no])
    AC_MSG_CHECKING([for MS Crypto API])
    if test "$crypto_api" = yes ; then
        AC_DEFINE_UNQUOTED(MS_CRYPTO_API, 1, [Use the Microsoft Crypto API.])
        AC_MSG_RESULT(yes)
    else
        AC_MSG_RESULT(no)
    fi
else
    AC_MSG_NOTICE([Microsoft Crypto API disabled])
fi
    
    

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
