#
# configure.in
#
# See the LICENSE file for the license associated with this file.

AC_INIT([libtpms], [0.5.1])
AC_PREREQ(2.12)
AC_CONFIG_SRCDIR(Makefile.am)
AM_CONFIG_HEADER(config.h)

AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign 1.6])

LIBTPMS_VER_MAJOR=`echo $PACKAGE_VERSION | awk -F. '{print $1}'`
LIBTPMS_VER_MINOR=`echo $PACKAGE_VERSION | awk -F. '{print $2}'`
LIBTPMS_VER_MICRO=`echo $PACKAGE_VERSION | awk -F. '{print $3}'`
LIBTPMS_VERSION=$PACKAGE_VERSION
LIBTPMS_VERSION_INFO=`expr $LIBTPMS_VER_MAJOR + $LIBTPMS_VER_MINOR`:$LIBTPMS_VER_MICRO:$LIBTPMS_VER_MINOR

AC_SUBST([LIBTPMS_VER_MAJOR])
AC_SUBST([LIBTPMS_VER_MINOR])
AC_SUBST([LIBTPMS_VER_MICRO])
AC_SUBST([LIBTPMS_VERSION])
AC_SUBST([LIBTPMS_VERSION_INFO])

DEBUG=""
AC_MSG_CHECKING([for debug-enabled build])
AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug], [create a debug build]),
  [if test "$enableval" = "yes"; then
     DEBUG="yes"
     AC_MSG_RESULT([yes])
   else
     DEBUG="no"
     AC_MSG_RESULT([no])
   fi],
  [DEBUG="no",
   AC_MSG_RESULT([no])])

# If the user has not set CFLAGS, do something appropriate
test_CFLAGS=${CFLAGS+set}
if test "$test_CFLAGS" != set; then
	if test "$DEBUG" == "yes"; then
		CFLAGS="-O0 -g -DDEBUG"
	else
		CFLAGS="-g -O2"
	fi
elif test "$DEBUG" == "yes"; then
	CFLAGS="$CFLAGS -O0 -g -DDEBUG"
fi

debug_defines=
if test "$DEBUG" == "yes"; then
	debug_defines="-DTPM_DEBUG -DTPM_VOLATILE_STORE"
fi
AC_SUBST(DEBUG_DEFINES, $debug_defines)

cryptolib=freebl

AC_ARG_WITH([openssl],
            AC_HELP_STRING([--with-openssl],
                           [build libtpms with openssl library]),
              [AC_CHECK_LIB(crypto,
                            [AES_set_encrypt_key],
                            [],
                            AC_MSG_ERROR(Faulty openssl crypto library))
               AC_CHECK_HEADERS([openssl/aes.h])
               AC_MSG_RESULT([Building with openssl crypto library])
               cryptolib=openssl
              ]
)

case "$cryptolib" in
freebl)
	AM_CONDITIONAL(LIBTPMS_USE_FREEBL, true)
	AM_CONDITIONAL(LIBTPMS_USE_OPENSSL, false)
        AC_DEFINE([USE_FREEBL_CRYPTO_LIBRARY],
                  [1],
                  [use freebl crypto library])
	;;
openssl)
	AM_CONDITIONAL(LIBTPMS_USE_FREEBL, false)
	AM_CONDITIONAL(LIBTPMS_USE_OPENSSL, true)
        AC_DEFINE([USE_OPENSSL_CRYPTO_LIBRARY],
                  [1],
                  [use openssl crypto library])
	;;
esac

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

#AM_GNU_GETTEXT_VERSION([0.15])
#AM_GNU_GETTEXT([external])

AC_HEADER_STDC
AC_C_CONST
AC_C_INLINE

AC_TYPE_SIZE_T


CFLAGS="$CFLAGS -Wall -Werror -Wreturn-type -Wsign-compare"

AC_CONFIG_FILES(Makefile                   \
		dist/libtpms.spec          \
		include/Makefile           \
		include/libtpms/Makefile   \
		include/libtpms/tpm_library.h \
		man/Makefile               \
		man/man3/Makefile          \
		src/Makefile               \
		libtpms.pc                 \
		tests/Makefile)
AC_OUTPUT

echo "CFLAGS=$CFLAGS"
echo "LDFLAGS=$LDFLAGS"