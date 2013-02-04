CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D ONFAILEDFUNC_UNIT_TEST
INCLUDES = 
LIBS = -lboost_system
OBJS = OnFailedFunc.o
PROGRAM = OnFailedFunc.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
