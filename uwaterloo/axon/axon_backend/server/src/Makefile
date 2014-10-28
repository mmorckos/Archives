#
# Makefile for server.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#
top_srcdir     = ../

include $(top_srcdir)/config/Make.rules

SUBDIRS		= protobufdata \
		  util \
		  frontendserver \
		  entityserver

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done
	
install:: all
	$(call installprogram, $(top_srcdir)/bin/run_server.sh, $(install_bindir))
