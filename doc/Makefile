# $Id: Makefile,v 1.1 1995-05-30 10:59:20 adam Exp $
all: -tcl.ps ir-tcl.txt

ir-tcl.txt: ir-tcl.sgml
	sgml-format -T nroff ir-tcl| qroff >ir-tcl.txt

ir-tcl.dvi: ir-tcl.sgml
	sgml-format ir-tcl| qtex -d >ir-tcl.dvi

ir-tcl.ps: ir-tcl.sgml
	sgml-format ir-tcl| qtex -x
	mv out.ps ir-tcl.ps

clean:
	rm -f [0-9]* *.bak
	
