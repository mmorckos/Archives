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
all:	globalservice.pb.o

globalservice.pb.o:	$(SRC_DIR)/globalservice.pb.cc
	$(CXX) -c $(SRC_DIR)/globalservice.pb.cc

clean:
	rm -f *.o
	
install:: all
