### ==========================================================================
###  $Id: Makefile,v 1.1 2002/08/20 03:04:27 stephmo Exp $
###  FILE: doc/Makefile - doc (sgml -> html) rules
### --------------------------------------------------------------------------
###  Experimenal make rules for building HTML documentation from SGML source
###   by Stephen M Moraco
###
DOCBASE = INSTALL-cygwin
DOCSRC = $(DOCBASE).sgml

all: $(DOCBASE)/images
	sgmltools -b html $(DOCSRC)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE)/top.html >$(DOCBASE).html

$(DOCBASE)/images:
	mkdir -p $(DOCBASE)

clean:
	rm -f index.sgml *.html $(DOCBASE)/*.html build-stamp *.stderr *.stdout *.FIRST
	rm -f HTML.index 
	rm -rf $(DOCBASE)

### --------------------------------------------------------------------------
###    End of file: doc/Makefile
### ==========================================================================
