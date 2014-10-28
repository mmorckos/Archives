#
# Makefile for entity interfacer.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     = .

include $(top_srcdir)/config/Make.rules

SUBDIRS		= $(top_srcdir)/protobuf \
		$(top_srcdir)/src \
		$(top_srcdir)/config \
		$(top_srcdir)/doc

INSTALL_SUBDIRS	= $(install_libdir) $(install_includedir) $(install_configdir) \
	$(install_docdir)

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
