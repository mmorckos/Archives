#
# Makefile for sockets.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

##### Source directory #####
SRC_DIR		= .

##### Compiler, tools and options #####
CXX		= @echo compiling $< && g++

##### Targets #####
all:	sendersckt.o

sendersckt.o:	$(SRC_DIR)/sendersckt.cpp
	$(CXX) -c $(SRC_DIR)/sendersckt.cpp

clean:
	rm -f *.o
	
install:: all
