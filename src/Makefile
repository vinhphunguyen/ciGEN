PROGRAM = interface-elem
#CXX     = /opt/local/bin/g++

LIBS= 
LIBDIRS = -L/usr/local/lib/ \

INCLUDEDIRS = \
          -I/usr/local/include \

CFLAGS = -O3 -g -Wall -std=c++0x $(INCLUDEDIRS)
LFLAGS = $(LIBS) $(LIBDIRS)

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LFLAGS)

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	rm $(PROGRAM) $(OBJECTS)


