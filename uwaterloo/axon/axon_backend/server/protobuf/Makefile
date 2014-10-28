#
# Makefile for GPB messages.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

##### Source directory #####
PROTOBUF_DIR	= .
SRC_DIR		= ../src

##### Compiler, tools and options #####
PROTOC		= @echo compiling $< && protoc

##### Targets #####
all:	protobufdata

protobufdata:	$(PROTOBUF_DIR)/globalservice.proto
	$(PROTOC) --cpp_out=$(SRC_DIR)/protobufdata/ $(PROTOBUF_DIR)/globalservice.proto

clean:
	rm -f $(SRC_DIR)/protobufdata/*.cc
	rm -f $(SRC_DIR)/protobufdata/*.cpp
	rm -f $(SRC_DIR)/protobufdata/*.h
	
install:: all
