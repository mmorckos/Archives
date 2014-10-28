#
# Makefile for utility tools.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

##### Source directory #####
SRC_DIR		= .

##### Compiler, tools and options #####
CXX		= @echo compiling $< && g++

##### Targets #####
all:	configparser.o	logger.o dataformatconverter.o

configparser.o:	$(SRC_DIR)/configparser.cpp
	$(CXX) -c $(SRC_DIR)/configparser.cpp

logger.o:	$(SRC_DIR)/logger.cpp
	$(CXX) -c $(SRC_DIR)/logger.cpp
	
dataformatconverter.o:	$(SRC_DIR)/dataformatconverter.cpp
	$(CXX) -c $(SRC_DIR)/dataformatconverter.cpp

clean:
	rm -f *.o
	
install:: all
