# $Id: Makefile,v 1.7 1999-02-11 10:57:20 adam Exp $
all: ir-tcl.ps ir-tcl.txt ir-tcl.html

ir-tcl.txt: ir-tcl.sgml
	sgml2txt ir-tcl.sgml

ir-tcl.ps: ir-tcl.sgml
	sgml2latex --output=ps ir-tcl

ir-tcl.dvi: ir-tcl.tex
	sgml2latex --output=dvi ir-tcl

ir-tcl.tex: ir-tcl.sgml
	sgml2latex --output=tex ir-tcl

ir-tcl.html: ir-tcl.sgml
	sgml2html ir-tcl

ir-tcl.shtml: ir-tcl.html
	for f in ir-tcl*.html; do \
		out=`echo $$f | sed -e 's/.html/.shtml/'`; \
		sed -e 's@\(ir-tcl.*\).html@\1.shtml@' $$f | \
		sed -e 's@^<BODY>@<!--#include virtual="top.shtml"-->@' | \
		sed -e 's@^</BODY>@<!--#include virtual="bottom.shtml"-->@' >$$out; \
	done

clean:
	rm -f [0-9]* *.bak *.aux *.tex *.ps *.dvi *.html *.shtml
	
