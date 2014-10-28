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
all:	listenersckt.o

listenersckt.o:	$(SRC_DIR)/listenersckt.cpp
	$(CXX) -c $(SRC_DIR)/listenersckt.cpp

clean:
	rm -f *.o
	
install:: all
	