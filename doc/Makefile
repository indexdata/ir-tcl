# $Id: Makefile,v 1.5 1998-01-16 09:24:17 adam Exp $
all: ir-tcl.ps ir-tcl.txt ir-tcl.html

ir-tcl.txt: ir-tcl.sgml
	sgml2txt ir-tcl.sgml

ir-tcl.dvi: ir-tcl.sgml
	sgml2ps -d ir-tcl.sgml

ir-tcl.tex: ir-tcl.sgml
	sgml2latex ir-tcl.sgml

ir-tcl.html: ir-tcl.sgml
	sgml2html ir-tcl.sgml

clean:
	rm -f [0-9]* *.bak
	
