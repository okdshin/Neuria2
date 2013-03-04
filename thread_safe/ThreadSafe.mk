CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D THREADSAFE_UNIT_TEST
INCLUDES = 
LIBS =
OBJS = ThreadSafe.o
PROGRAM = ThreadSafe.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
