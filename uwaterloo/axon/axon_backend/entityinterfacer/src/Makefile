#
# Makefile for entity interfacer.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

top_srcdir     	= ../

include $(top_srcdir)/config/Make.rules

SUBDIRS		= sockets \
					protobufdata \
					util

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done

ENTITYINTERFACER 		= $(top_srcdir)lib/libEntityInterfacer.so

TARGETS		= $(ENTITYINTERFACER)

OBJS0		= communicator.o \
			entityinterfacer.o
			
OBJS1		= ./sockets/listenersckt.o \
			./protobufdata/param.pb.o \
			./protobufdata/resource.pb.o \
			./protobufdata/task.pb.o \
			./protobufdata/position.pb.o \
			./util/configparser.o \
			./util/logger.o \
			./util/dataformatconverter.o

SRCS		= $(OBJS0:.o=.cpp)

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(ICE_FLAGS)

$(ENTITYINTERFACER): $(OBJS0)
	rm -f $@
	$(CXX) -shared -Wl,-soname,libEntityInterfacer.so -o $@ $(OBJS0) $(OBJS1) -lprotobuf
	
install:: all
	$(call installprogram, $(ENTITYINTERFACER), $(install_libdir))
	$(call installdata, entityinterfacer.h, $(install_includedir))
	