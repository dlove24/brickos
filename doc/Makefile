### ==========================================================================
###  $Id: Makefile,v 1.4 2002/10/11 09:13:04 stephmo Exp $
###  FILE: doc/Makefile - doc (sgml -> html) rules
### --------------------------------------------------------------------------
###  Experimenal make rules for building HTML documentation from SGML source
###   by Stephen M Moraco
###
DOCBASE = INSTALL-cygwin
DOCSRC = $(DOCBASE).sgml

DOCBASE2 = HOWTO
DOCSRC2 = $(DOCBASE2).sgml

#  out default target when doing make in this directory
doc: $(DOCBASE).html $(DOCBASE2).html

# nothing to do for these (documents not built when running top-level build)
all install depend:
	@# nothing to do here, but do this nothing silently

$(DOCBASE).html: $(DOCSRC)
	mkdir -p $(DOCBASE)/images
	sgmltools -b html $(DOCBASE)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE)/top.html >$@

realclean::
	rm -rf $(DOCBASE)
	rm -f $(DOCBASE).html

$(DOCBASE2).html: $(DOCSRC2)
	mkdir -p $(DOCBASE2)/images
	sgmltools -b html $(DOCBASE2)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE2)/t1.html >$@

realclean::
	rm -f $(DOCBASE2).html
	rm -rf $(DOCBASE2)

clean:
	@# nothing to do here, but do this nothing silently
#	rm -f index.sgml *.html build-stamp *.stderr *.stdout *.FIRST
#	rm -f HTML.index 

realclean:: clean

### --------------------------------------------------------------------------
###    End of file: doc/Makefile
### ==========================================================================
