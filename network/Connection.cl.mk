CXX = clang++ -std=gnu++0x
CXXFLAGS = -Wall -g -D CONNECTION_UNIT_TEST
INCLUDES = 
LIBS = -lboost_system -lboost_thread -lpthread
OBJS = Connection.o
PROGRAM = Connection.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
