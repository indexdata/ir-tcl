# $Id: Makefile,v 1.2 1995-05-31 08:37:01 adam Exp $
all: -tcl.ps ir-tcl.txt

ir-tcl.txt: ir-tcl.sgml
	sgml-format -T nroff ir-tcl| qroff >ir-tcl.txt

ir-tcl.dvi: ir-tcl.sgml
	sgml-format ir-tcl| qtex -d >ir-tcl.dvi

ir-tcl.ps: ir-tcl.sgml
	sgml-format ir-tcl| qtex -x
	mv out.ps ir-tcl.ps

ir-tcl.html: ir-tcl.sgml
	sgml-format -T html ir-tcl | prehtml | fixref >tmp.html
	sgml-format -T html ir-tcl | prehtml >>tmp.html
	html2html ir-tcl <tmp.html >ir-tcl.html
	rm tmp.html

clean:
	rm -f [0-9]* *.bak
	
