# Tell make that you're using g++
CXX=g++

# Give makefile permission to create objects in directory
CXXFLAGS=$(shell root-config --cflags)

# Point makefile to repository for root functions
LIBS=$(shell root-config --libs) 
LD=g++

# Something Gianluca recommended
DEBUGFLAGS=-O0 -ggdb -D _DEBUG

# Make it easier to add targets...see below
TARGETNAMES=energy_cal prepare_histos langausFit

# If using class objects, modify and uncomment the following line
# Assuming .h and .cxx are called histogram_functions
# FUNCTIONCLASSOBJECT=histogram_functions.o

# Why it's easier to add more targets
TARGETS=$(TARGETNAMES:%=%.exe)
OBJECTS=$(TARGETNAMES:%=%.o)
SOURCES=$(TARGETNAMES:%=%.cc)

all: $(TARGETS)

clean:
	rm -f $(OBJECTS) $(TARGETS)

# Create object files from .C files
%.o: %.C
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c -o "$@" $^

# Create .exe file
%.exe : %.o $(FUNCTIONCLASSOBJECT) 
	$(LD) $(LIBS) -o "$@" $^

# If using FUNCTIONCLASSOBJECT, add "$(FUNCTIONCLASSOBJECT)" after %.o
