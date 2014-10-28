#
# Makefile for entity server.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     	= ../..

ENTITYSERVER 		= $(top_srcdir)/bin/entityserver

TARGETS		= $(ENTITYSERVER)

OBJS		= commutil.o \
			entitycomm.o \
			entitytopics.o \
			frontendtopics.o \
		  session_i.o \
		  sessionmanager.o \
		  pollsession_i.o \
		  pollsessionmanager_i.o \
		  frontendtoentitycomm_i.o \
		  entitymonitor.o \
		  entityserver.o

SRCS		= $(OBJS:.o=.cpp)

SLICE_SRCS	= $(top_srcdir)/slice/commutil.ice $(top_srcdir)/slice/entitycomm.ice	\
							$(top_srcdir)/slice/entitytopics.ice $(top_srcdir)/slice/frontendtopics.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(ICE_FLAGS)
SLICE2CPPFLAGS  := -I$(top_srcdir)/slice $(SLICE2CPPFLAGS)

$(ENTITYSERVER): $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) -lGlacier2 $(ICE_LIBS)
	
install:: all
	$(call installprogram, $(ENTITYSERVER), $(install_bindir))
