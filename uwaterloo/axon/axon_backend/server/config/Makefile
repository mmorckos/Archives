#
# Makefile for configurations files.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::

install:: all
	$(call installdata, entityserver.cfg, $(install_configdir))
	$(call installdata, frontendserver.cfg, $(install_configdir))
	$(call installdata, glacier2router_entity.cfg, $(install_configdir))
	$(call installdata, glacier2router_frontend.cfg, $(install_configdir))
	$(call installdata, icebox.cfg, $(install_configdir))
	$(call installdata, service.cfg, $(install_configdir))
