CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D SHELL_UNIT_SHELL
INCLUDES = 
LIBS =
OBJS = Shell.o
PROGRAM = Shell.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
