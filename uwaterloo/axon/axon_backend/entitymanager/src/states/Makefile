#
# Makefile for states.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

##### Source directory #####
SRC_DIR		= .

##### Compiler, tools and options #####
CXX		= @echo compiling $< && g++

##### Targets #####
all:	state.o	stateUNAV.o	stateINIT.o	stateENVCONN.o	stateIDLE.o	stateREG.o stateCOLLAB.o \
			stateSLAVE.o stateBUSY.o stateERR.o

state.o:	$(SRC_DIR)/state.cpp	$(SRC_DIR)/state.h
	$(CXX) -c $(SRC_DIR)/state.cpp

stateUNAV.o:	$(SRC_DIR)/stateUNAV.cpp
	$(CXX) -c $(SRC_DIR)/stateUNAV.cpp
	
stateINIT.o:	$(SRC_DIR)/stateINIT.cpp
	$(CXX) -c $(SRC_DIR)/stateINIT.cpp
	
stateENVCONN.o:	$(SRC_DIR)/stateENVCONN.cpp
	$(CXX) -c $(SRC_DIR)/stateENVCONN.cpp

stateIDLE.o:	$(SRC_DIR)/stateIDLE.cpp
	$(CXX) -c $(SRC_DIR)/stateIDLE.cpp
	
stateREG.o:	$(SRC_DIR)/stateREG.cpp
	$(CXX) -c $(SRC_DIR)/stateREG.cpp
		
stateCOLLAB.o:	$(SRC_DIR)/stateCOLLAB.cpp
	$(CXX) -c $(SRC_DIR)/stateCOLLAB.cpp
	
stateSLAVE.o:	$(SRC_DIR)/stateSLAVE.cpp
	$(CXX) -c $(SRC_DIR)/stateSLAVE.cpp
	
stateBUSY.o:	$(SRC_DIR)/stateBUSY.cpp
	$(CXX) -c $(SRC_DIR)/stateBUSY.cpp
	
stateERR.o:	$(SRC_DIR)/stateERR.cpp
	$(CXX) -c $(SRC_DIR)/stateERR.cpp

clean:
	rm -f *.o
	
install:: all
