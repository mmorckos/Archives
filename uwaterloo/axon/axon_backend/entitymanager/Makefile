#
# Makefile for entity manager.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     = .

include $(top_srcdir)/config/Make.rules

INSTALL_SUBDIRS	= $(install_bindir) $(install_configdir) $(install_certsdir) $(install_profiledir) \
	$(install_docdir)

SUBDIRS		= $(top_srcdir)/protobuf/ \
			$(top_srcdir)/slice/ \
		  $(top_srcdir)/src/ \
		  $(top_srcdir)/sample_profiles/ \
		  $(top_srcdir)/config/ \
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
