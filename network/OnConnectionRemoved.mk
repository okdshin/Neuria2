CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D ONCONNECTIONREMOVED_UNIT_TEST
INCLUDES = 
LIBS =
OBJS = OnConnectionRemoved.o
PROGRAM = OnConnectionRemoved.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
