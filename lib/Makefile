### ==========================================================================
###  $Id: Makefile,v 1.4 2002/10/15 03:23:24 stephmo Exp $
###  FILE: lib/Makefile - make the supporting libraries
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# list of dirctories in this subtree
SUBDIRS=c \
        mint \
		float \
		c++

# targets which get passed to these directories
all install clean realclean depend::
	@for i in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$i $@ || exit 2; done

# target(s) which get acted upon here (or not)
tag::
	@# nothing to do here but do it silently

### --------------------------------------------------------------------------
###                       End of FILE: lib/Makefile
### ==========================================================================
