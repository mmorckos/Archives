#
# Makefile for ICE IDL definitions.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::
	
install:: all
	$(call installdata, commutil.ice, $(install_slicedir))
	$(call installdata, entitycomm.ice, $(install_slicedir))
	$(call installdata, entitytopics.ice, $(install_slicedir))
	$(call installdata, frontendcomm.ice, $(install_slicedir))
	$(call installdata, frontendtopics.ice, $(install_slicedir))
