#!/bin/sh
# $Id: buildconf.sh,v 1.1 2004-04-26 09:12:01 adam Exp $
set -x
aclocal -I .
autoconf
if [ -f config.cache ]; then
	rm config.cache
fi
