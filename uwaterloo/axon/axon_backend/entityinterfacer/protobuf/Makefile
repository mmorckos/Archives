#
# Makefile for protocol buffers messages.
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

protobufdata:	$(PROTOBUF_DIR)/service.proto $(PROTOBUF_DIR)/ctrlparam.proto \
		$(PROTOBUF_DIR)/entityprofile.proto $(PROTOBUF_DIR)/param.proto \
		$(PROTOBUF_DIR)/resource.proto $(PROTOBUF_DIR)/task.proto	$(PROTOBUF_DIR)/position.proto
	$(PROTOC) --cpp_out=$(SRC_DIR)/protobufdata/ \
	$(PROTOBUF_DIR)/service.proto $(PROTOBUF_DIR)/ctrlparam.proto \
	$(PROTOBUF_DIR)/entityprofile.proto $(PROTOBUF_DIR)/param.proto \
	$(PROTOBUF_DIR)/resource.proto $(PROTOBUF_DIR)/task.proto $(PROTOBUF_DIR)/position.proto

clean:
	rm -f $(SRC_DIR)/protobufdata/*.cc
	rm -f $(SRC_DIR)/protobufdata/*.cpp
	rm -f $(SRC_DIR)/protobufdata/*.h
	
install:: all
	