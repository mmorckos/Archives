#
# Makefile for front-end server.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     	= ../..

FRONTENDSERVER 		= $(top_srcdir)/bin/frontendserver

OBJS0		= commutil.o \
			frontendcomm.o \
			entitytopics.o \
			frontendtopics.o \
		  session_i.o \
		  sessionmanager.o \
		  pollsession_i.o \
		  pollsessionmanager_i.o \
		  entitytofrontendcomm_i.o \
		  clientmonitor.o \
		  frontendserver.o

TARGETS		= $(FRONTENDSERVER)

OBJS1		= ../protobufdata/globalservice.pb.o \
			../util/third_party/pugixml.o \
			../util/xmlglobalserviceparser.o \
			../util/dataformatconverter.o

SRCS		= $(OBJS:.o=.cpp)

SLICE_SRCS	= $(top_srcdir)/slice/commutil.ice $(top_srcdir)/slice/frontendcomm.ice	\
							$(top_srcdir)/slice/entitytopics.ice $(top_srcdir)/slice/frontendtopics.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(ICE_FLAGS)
SLICE2CPPFLAGS  := -I$(top_srcdir)/slice $(SLICE2CPPFLAGS)

$(FRONTENDSERVER): $(OBJS0)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS0) $(OBJS1) -lGlacier2 $(ICE_LIBS)
	
install:: all
	$(call installprogram, $(FRONTENDSERVER), $(install_bindir))
