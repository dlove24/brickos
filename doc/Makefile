### ==========================================================================
###  $Id: Makefile,v 1.5 2002/10/15 07:02:07 stephmo Exp $
###  FILE: doc/Makefile - build HTML documentation from SGML source
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

DOCBASE = INSTALL-cygwin
DOCSRC = $(DOCBASE).sgml

DOCBASE2 = HOWTO
DOCSRC2 = $(DOCBASE2).sgml

TEXT_DOCS = AvailableDocs.txt \
            Notes_on_intterupts \
            Pros_and_Cons \
            RCXmessageAPI.txt \
            Sensors


include ../Makefile.common

#  out default target when doing make in this directory
doc: $(DOCBASE).html $(DOCBASE2).html

# nothing to do for these (documents not built when running top-level build)
all install depend:
	@# nothing to do here, but do this nothing silently

docs-install: ${pkghtmldir} chkbuild
	-@cp -f ../[CLNRV]* ${pkgdocdir} 2>/dev/null
	cp -f ${TEXT_DOCS} ${pkgdocdir} 
	cp -r images ${pkghtmldir}
	cp -f *.html ${pkghtmldir}
	cp HOWTO/*.html ${pkghtmldir}
	@# oops copied extra file, remove it
	@rm -f $(pkghtmldir)/t1.html

chkbuild:
	@if [ ! -f $(DOCBASE).html -o ! -f $(DOCBASE2).html ] ;then \
		echo ""; \
		echo "---------------------------------------------------------"; \
		echo "  Documents are not built. Please run 'make doc' then "; \
		echo "    rerun 'make doc-install' as root"; \
		echo "---------------------------------------------------------"; \
		echo ""; \
		exit 2; \
	fi

${pkghtmldir}:
	mkdir -p $@

$(DOCBASE).html: $(DOCSRC)
	sgmltools -b html $(DOCBASE)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE)/top.html >$@

realclean::
	rm -rf $(DOCBASE)
	rm -f $(DOCBASE).html

$(DOCBASE2).html: $(DOCSRC2)
	sgmltools -b html $(DOCBASE2)
	#  correct for incorrect image path
	sed -e 's/\/docbook-dsssl/images/' $(DOCBASE2)/t1.html >$@

realclean::
	rm -f $(DOCBASE2).html
	rm -rf $(DOCBASE2)

tag:
	@# nothing to do here but do it silently

clean:
	@# nothing to do here, but do this nothing silently
#	rm -f index.sgml *.html build-stamp *.stderr *.stdout *.FIRST
#	rm -f HTML.index 

realclean:: clean

### --------------------------------------------------------------------------
###                            End of FILE: doc/Makefile
### ==========================================================================
