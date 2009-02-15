###############################################
##############                  ###############
#############  Laidout Makefile  ##############
##############                  ###############
###############################################

 # Makefile-toinclude, generated by configure, 
 # defines LAXDIR, PREFIX, and LAIDOUTVERSION
include Makefile-toinclude


 # where the main executable goes
BINDIR=$(PREFIX)/bin
 # like /usr/local/share/, examples go here: $SHAREDIR/laidout
SHAREDIR=$(PREFIX)/share


 ### If you want to be sure that an install does not clobber anything that exists
 ### already, then uncomment the line with the '--backup=t' and comment out the other.
#INSTALL=install -D --backup=t 
INSTALL=install -D

INSTALLDIR=install -d


LAIDOUTNAME=laidout-$(LAIDOUTVERSION)

laidout: 
	cd src && $(MAKE)
	cd src/po && $(MAKE)

all: laidout docs

docs:
	cd docs && doxygen
	

#install: laidout <-- don't want to link twice, so assume 'make install' called separate
install: 
	echo 'Installing to $(BINDIR)/laidout which points to $(BINDIR)/$(LAIDOUTNAME)'
	$(INSTALL) -m755 src/laidout $(BINDIR)/$(LAIDOUTNAME)
	$(INSTALLDIR) $(SHAREDIR)/laidout/$(LAIDOUTVERSION)/examples
	$(INSTALL) -m644 -t $(SHAREDIR)/laidout/$(LAIDOUTVERSION)/examples examples/*
	$(INSTALLDIR) $(SHAREDIR)/laidout/$(LAIDOUTVERSION)/icons
	$(INSTALL) -m644 -t $(SHAREDIR)/laidout/$(LAIDOUTVERSION)/icons src/icons/*.png
	rm -f $(BINDIR)/laidout
	ln -s $(LAIDOUTNAME) $(BINDIR)/laidout
	cd src/po && $(MAKE) install

# ****** TODO!!! this is very primitive make uninstall!!
# should only uninstall things that were actually installed. Any resource added by
# the user to the system wide directories should not be uninstalled. Should use
# config log perhaps. The $(BINDIR)/laidout should be removed only if it points to
# $(BINDIR)/$(LAIDOUTNAME)
uninstall: 
	echo 'Uninstalling laidout.'
	rm -f  $(BINDIR)/laidout
	rm -fr $(SHAREDIR)/laidout/$(LAIDOUTVERSION)
	rm -f  $(BINDIR)/$(LAIDOUTNAME)
	cd src/po && $(MAKE) uninstall

hidegarbage: touchdepends
	cd src && $(MAKE) hidegarbage

unhidegarbage: touchdepends
	cd src && $(MAKE) unhidegarbage

depends: touchdepends
	cd src && $(MAKE) depends

touchdepends:
	touch src/makedepend
	touch src/calculator/makedepend
	touch src/dataobjects/makedepend
	touch src/filetypes/makedepend
	touch src/impositions/makedepend
	touch src/interfaces/makedepend
	touch src/printing/makedepend

.PHONY: all laidout clean docs install uninstall hidegarbage unhidegarbage depends touchdepends
clean:
	cd src && $(MAKE) clean
	
