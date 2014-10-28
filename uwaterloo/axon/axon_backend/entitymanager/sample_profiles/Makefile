#
# Makefile for sample entity profiles.
#
# Author: Michael Morckos	<mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::

install:: all
	$(call installdata, peoplebot_basic_profile.xml, $(install_profiledir))
	$(call installdata, peoplebot_service_profile.xml, $(install_profiledir))
	$(call installdata, cyton_basic_profile.xml, $(install_profiledir))
	$(call installdata, cyton_service_profile.xml, $(install_profiledir))
	