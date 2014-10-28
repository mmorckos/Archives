#
# Makefile for documentation.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::

install:: all
	$(call mkdir, $(install_docdir)/html/)
	$(call mkdir, $(install_docdir)/html/search)
	$(call installdata, html/*, $(install_docdir)/html/)
	$(call installdata, html/search/*, $(install_docdir)/html/search)
