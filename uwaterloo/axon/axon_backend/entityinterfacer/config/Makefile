#
# Makefile for configuration files.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::

install:: all
	$(call installdata, ports.cfg, $(install_configdir))
	