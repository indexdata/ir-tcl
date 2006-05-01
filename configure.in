dnl IR toolkit for tcl/tk
dnl (c) Index Data 1996-2006
dnl See the file LICENSE for details.
dnl $Id: configure.in,v 1.38 2006-05-01 12:52:51 adam Exp $
AC_INIT(ir-tcl.h)
dnl ------ Substitutions
AC_SUBST(CC)
AC_SUBST(TCLLIB)
AC_SUBST(TKLIB)
AC_SUBST(TCLINC)
AC_SUBST(TKINC)
AC_SUBST(SHLIB_CFLAGS)
AC_SUBST(SHLIB_LD)
AC_SUBST(SHLIB_SUFFIX)
AC_SUBST(SHLIB_VERSION)
AC_SUBST(SHLIB_IRTCL)
AC_SUBST(ALL_SPEC)
AC_SUBST(IRCLIENT_SHELL)
AC_SUBST(RANLIB)
dnl
dnl ------ Preliminary settings
AC_PROG_INSTALL
dnl
dnl ------ YAZ
YAZ_INIT
dnl
dnl ------ look for Tcl
tclconfig=NONE
AC_ARG_WITH(tclconfig, [  --with-tclconfig=DIR    tclConfig.sh in DIR (example /usr/lib)], [tclconfig=$withval])
if test "x$tclconfig" = xNONE; then
        saveprefix=${prefix}
	AC_PREFIX_PROGRAM(tclsh)
	if test "x$prefix" = xNONE; then
		AC_PREFIX_PROGRAM(tclsh8.3)
	fi
	if test "x$prefix" = xNONE; then
		AC_PREFIX_PROGRAM(tclsh8.2)
	fi
	if test "x$prefix" = xNONE; then
		AC_PREFIX_PROGRAM(tclsh8.1)
	fi
	if test "x$prefix" = xNONE; then
		AC_PREFIX_PROGRAM(tclsh8.0)
	fi
	if test "x$prefix" = xNONE; then
		AC_PREFIX_PROGRAM(tclsh7.6)
	fi
	if test "x$prefix" = xNONE; then
		prefix=/usr/local
	fi
        tclconfig=${prefix}/lib
        IRCLIENT_SHELL=${prefix}/bin/wish
        prefix=${saveprefix}
        if test ! -r ${tclconfig}/tclConfig.sh; then
                # Not found, try search for Tcl on Debian systems.
                for d in /usr/lib/tcl*; do
                        if test -f $d/tclConfig.sh; then
                                tclconfig=$d
        			IRCLIENT_SHELL=/usr/bin/wish
                        fi
                done
        fi
else
        IRCLIENT_SHELL=/usr/bin/wish
fi
AC_MSG_CHECKING(for Tcl)
if test -f ${tclconfig}/tclConfig.sh; then
	tclconfig=${tclconfig}/tclConfig.sh
fi
if test -f ${tclconfig}; then
	. ${tclconfig}
	TCLLIB="${TCL_LIB_SPEC} ${TCL_LIBS}"
	if test -d ${TCL_PREFIX}/include/tcl${TCL_VERSION}; then
		TCLINC=-I${TCL_PREFIX}/include/tcl${TCL_VERSION}
	elif test "${TCL_PREFIX}" = "/usr"; then
		TCLINC=""
	else
		TCLINC=-I${TCL_PREFIX}/include
	fi
	RANLIB=$TCL_RANLIB
        SHLIB_CFLAGS=$TCL_SHLIB_CFLAGS
	SHLIB_LD=$TCL_SHLIB_LD
	SHLIB_SUFFIX=$TCL_SHLIB_SUFFIX
	SHLIB_VERSION=$TCL_SHLIB_VERSION
	SHLIB_IRTCL=irtcl${TCL_SHLIB_SUFFIX}
	ALL_SPEC="irtcl${TCL_SHLIB_SUFFIX} libirtcl.a"
	AC_MSG_RESULT($TCL_VERSION)
	CC=$TCL_CC
else
	AC_MSG_RESULT(not found)
	SHLIB_IRTCL=irtcl-shared
	ALL_SPEC="ir-tcl"
	CC=gcc
	RANLIB=ranlib
fi
dnl 
AC_OUTPUT(Makefile)
