#
# Makefile for entity manager security certificates.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

all::

install:: all
	$(call installdata, dev_ca_cert.jks, $(install_certsdir))
	$(call installdata, dev_ca_cert.pem, $(install_certsdir))
	$(call installdata, glacier2_cert.pem, $(install_certsdir))
	$(call installdata, glacier2_key.pem, $(install_certsdir))
	$(call installdata, zeroc_ca_cert.pem, $(install_certsdir))
	$(call installdata, zeroc_glacier2_cert.pem, $(install_certsdir))
 