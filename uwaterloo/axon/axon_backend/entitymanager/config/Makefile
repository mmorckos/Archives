#
# Makefile for configurations files.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::

install:: all
	$(call installdata, entitygl2client.cfg, $(install_configdir))
	$(call installdata, general.cfg, $(install_configdir))
	$(call installdata, ports.cfg, $(install_configdir))
