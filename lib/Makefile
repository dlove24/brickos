### ==========================================================================
###  $Id: Makefile,v 1.5 2002/10/16 14:47:18 stephmo Exp $
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
tag uninstall::
	@# nothing to do here but do it silently

### --------------------------------------------------------------------------
###                       End of FILE: lib/Makefile
### ==========================================================================
