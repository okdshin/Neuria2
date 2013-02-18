CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D SHELLFUNC_UNIT_TEST
INCLUDES = 
LIBS =
OBJS = ShellFunc.o
PROGRAM = ShellFunc.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
