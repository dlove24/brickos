### ==========================================================================
###  $Id: Makefile,v 1.6 2002/10/17 04:30:45 stephmo Exp $
###  FILE: lib/Makefile - make the supporting libraries
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# list of dirctories in this subtree
SUBDIRS=c \
        mint \
		float \
		c++

# targets which get passed to these directories
all install clean realclean depend uninstall::
	@for i in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$i $@ || exit 2; done

# target(s) which get acted upon here (or not)
tag::
	@# nothing to do here but do it silently

### --------------------------------------------------------------------------
###                       End of FILE: lib/Makefile
### ==========================================================================
