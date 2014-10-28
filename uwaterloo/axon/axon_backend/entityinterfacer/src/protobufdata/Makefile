#
# Makefile for GPB generated classes.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

##### Source directory #####
SRC_DIR		= .

##### Compiler, tools and options #####
CXX		= @echo compiling $< && g++

##### Targets #####
all:	entityprofile.pb.o	ctrlparam.pb.o	param.pb.o	resource.pb.o	service.pb.o	task.pb.o \
			position.pb.o

entityprofile.pb.o:	$(SRC_DIR)/entityprofile.pb.cc
	$(CXX) -c $(SRC_DIR)/entityprofile.pb.cc

ctrlparam.pb.o:	$(SRC_DIR)/ctrlparam.pb.cc
	$(CXX) -c $(SRC_DIR)/ctrlparam.pb.cc
	
param.pb.o:	$(SRC_DIR)/param.pb.cc
	$(CXX) -c $(SRC_DIR)/param.pb.cc
	
resource.pb.o:	$(SRC_DIR)/resource.pb.cc
	$(CXX) -c $(SRC_DIR)/resource.pb.cc
	
service.pb.o:	$(SRC_DIR)/service.pb.cc
	$(CXX) -c $(SRC_DIR)/service.pb.cc
	
task.pb.o:	$(SRC_DIR)/task.pb.cc
	$(CXX) -c $(SRC_DIR)/task.pb.cc
	
position.pb.o:	$(SRC_DIR)/position.pb.cc
	$(CXX) -c $(SRC_DIR)/position.pb.cc

clean:
	rm -f *.o
	
install:: all
