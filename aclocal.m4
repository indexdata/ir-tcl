dnl aclocal.m4 generated automatically by aclocal 1.4-p4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

# Use this m4 funciton for autoconf if you use YAZ in your own
# configure script.
# YAZ_INIT

AC_DEFUN([YAZ_INIT],
[
	AC_SUBST(YAZLIB)
	AC_SUBST(YAZLALIB)
	AC_SUBST(YAZINC)
	AC_SUBST(YAZVERSION)
	yazconfig=NONE
	yazpath=NONE
	AC_ARG_WITH(yazconfig, [  --with-yazconfig=DIR    yaz-config in DIR (example /home/yaz-1.7)], [yazpath=$withval])
	if test "x$yazpath" != "xNONE"; then
		yazconfig=$yazpath/yaz-config
	else
		if test "x$srcdir" = "x"; then
			yazsrcdir=.
		else
			yazsrcdir=$srcdir
		fi
		for i in ${yazsrcdir}/../yaz* ${yazsrcdir}/../yaz ../yaz* ../yaz; do
			if test -d $i; then
				if test -r $i/yaz-config; then
					yazconfig=$i/yaz-config
				fi
			fi
		done
		if test "x$yazconfig" = "xNONE"; then
			AC_PATH_PROG(yazconfig, yaz-config, NONE)
		fi
	fi
	AC_MSG_CHECKING(for YAZ)
	if $yazconfig --version >/dev/null 2>&1; then
		YAZLIB=`$yazconfig --libs $1`
		# if this is empty, it's a simple version YAZ 1.6 script
		# so we have to source it instead...
		if test "X$YAZLIB" = "X"; then
			. $yazconfig
		else
			YAZLALIB=`$yazconfig --lalibs $1`
			YAZINC=`$yazconfig --cflags $1`
			YAZVERSION=`$yazconfig --version`
		fi
		AC_MSG_RESULT($yazconfig)
	else
		AC_MSG_RESULT(Not found)
		YAZVERSION=NONE
	fi
])
	

