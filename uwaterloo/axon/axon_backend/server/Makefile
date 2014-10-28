#
# Makefile for server.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     = .

include $(top_srcdir)/config/Make.rules

INSTALL_SUBDIRS	= $(install_certsdir) $(install_configdir) $(install_globalservicesdir) \
	$(install_slicedir) $(install_bindir) $(install_dbdir) $(install_docdir)

SUBDIRS		= $(top_srcdir)/protobuf/ \
		  $(top_srcdir)/slice/ \
		  $(top_srcdir)/src/ \
			$(top_srcdir)/config \
			$(top_srcdir)/globalservices/ \
			$(top_srcdir)/certs/ \
			$(top_srcdir)/doc/
			
install::
	@if test ! -d $(prefix) ; \
	then \
	    echo "Creating $(prefix)..." ; \
	    $(call mkdir, $(prefix)) ; \
	fi
	@for subdir in $(INSTALL_SUBDIRS); \
	do \
	    if test ! -d $$subdir ; \
	    then \
		echo "Creating $$subdir..." ; \
		mkdir -p $$subdir ; \
		chmod a+rx $$subdir ; \
	    fi ; \
	done

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done
	
clean::
	-rm -rf db/*
	
cleandb:
		-rm -rf db/*
