### ==========================================================================
###  $Id: Makefile,v 1.3 2002/09/20 06:34:53 stephmo Exp $
###  FILE: doc/Makefile - doc (sgml -> html) rules
### --------------------------------------------------------------------------
###  Experimenal make rules for building HTML documentation from SGML source
###   by Stephen M Moraco
###
DOCBASE = INSTALL-cygwin
DOCSRC = $(DOCBASE).sgml

DOCBASE2 = HOWTO
DOCSRC2 = $(DOCBASE2).sgml

all: INSTALL-cygwin  HOWTO

INSTALL-cygwin: $(DOCBASE)/images
	sgmltools -b html $(DOCBASE)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE)/top.html >$(DOCBASE).html

HOWTO: $(DOCBASE2)/images
	sgmltools -b html $(DOCBASE2)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE2)/top.html >$(DOCBASE2).html

$(DOCBASE)/images:
	mkdir -p $(DOCBASE)

$(DOCBASE2)/images:
	mkdir -p $(DOCBASE2)

clean:
	rm -f index.sgml *.html build-stamp *.stderr *.stdout *.FIRST
	rm -f HTML.index 
	rm -rf $(DOCBASE)
	rm -rf $(DOCBASE2)

### --------------------------------------------------------------------------
###    End of file: doc/Makefile
### ==========================================================================
