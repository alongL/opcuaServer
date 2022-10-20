



XX = g++
CXXFLAGS =  -std=c++11  
CLIBS = -lopen62541 -lpthread 
INCLUDE_DIRS = -I.  



# default in debug mode, use 'make ver=release' to compile release bin
ver = debug
ifeq ($(ver), debug)
	CXXFLAGS += -c -g 
else
	CXXFLAGS += -c -O3 
endif





SRC=$(wildcard *.cpp)
OBJECTS:=$(patsubst %.cpp,%.o,$(SRC))


PROGRAM:= uaserver
BINDIR := ./
TARGET = $(BINDIR)/$(PROGRAM)



all:  MD $(TARGET)  


$(TARGET) : $(OBJECTS) 
	$(XX)   -o $(TARGET)  $(OBJECTS)  $(CLIBS)  


$(OBJECTS) : %.o : %.cpp 
	$(XX)  $(CXXFLAGS) $< -o $@ $(INCLUDE_DIRS)  



MD:
	mkdir -p  $(BINDIR)



	

.PHONY : clean

clean:
	rm -rf $(TARGET) $(OBJECTS)

