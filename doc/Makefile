# $Id: Makefile,v 1.9 2004-04-26 09:50:50 adam Exp $
all: ir-tcl.ps ir-tcl.html

ir-tcl.txt: ir-tcl.sgml
	sgml2txt -f ir-tcl.sgml

ir-tcl.ps: ir-tcl.sgml
	sgml2latex --output=ps ir-tcl

ir-tcl.dvi: ir-tcl.tex
	sgml2latex --output=dvi ir-tcl

ir-tcl.tex: ir-tcl.sgml
	sgml2latex --output=tex ir-tcl

ir-tcl.html: ir-tcl.sgml
	sgml2html ir-tcl

ir-tcl.shtml: ir-tcl.html

clean:
	rm -f [0-9]* *.bak *.aux *.tex *.ps *.dvi *.html *.shtml
	
