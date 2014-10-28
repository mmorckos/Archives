#
# Makefile for entity manager.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     	= ..

include $(top_srcdir)/config/Make.rules

SUBDIRS		= protobufdata \
			sockets \
		  states \
		  util

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done

ENTITYMANAGER 		= $(top_srcdir)/bin/entitymanager

TARGETS		= $(ENTITYMANAGER)

OBJS0		= commutil.o \
			entitycomm.o \
			backendinterfacer.o \
			serviceprofiler.o \
			entitymanager.o \
			entitycommunicator.o
			
OBJS1		= ./protobufdata/entityprofile.pb.o \
			./protobufdata/ctrlparam.pb.o \
			./protobufdata/param.pb.o \
			./protobufdata/resource.pb.o \
			./protobufdata/service.pb.o \
			./protobufdata/task.pb.o \
			./protobufdata/position.pb.o \
			./util/third_party/pugixml.o \
			./util/configparser.o \
			./util/xmlprofileparser.o \
			./util/xmlserviceparser.o \
			./util/dataformatconverter.o \
			./util/datarecord.o \
			./sockets/sendersckt.o \
			./states/state.o \
			./states/stateUNAV.o \
			./states/stateINIT.o \
			./states/stateENVCONN.o \
			./states/stateIDLE.o \
			./states/stateREG.o \
			./states/stateCOLLAB.o \
			./states/stateSLAVE.o \
			./states/stateBUSY.o \
			./states/stateERR.o

SRCS		= $(OBJS0:.o=.cpp)

SLICE_SRCS	= $(top_srcdir)/slice/commutil.ice	$(top_srcdir)/slice/entitycomm.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(ICE_FLAGS)
SLICE2CPPFLAGS  := -I$(top_srcdir)/slice $(SLICE2CPPFLAGS)

$(ENTITYMANAGER): $(OBJS0)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS0) $(OBJS1) -lGlacier2 $(ICE_LIBS)
	
install:: all
	$(call installprogram, $(ENTITYMANAGER), $(install_bindir))
