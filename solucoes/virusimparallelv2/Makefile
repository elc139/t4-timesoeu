CXX       = g++
CXXFLAGS  = -Wall -I. -fopenmp

PROGRAM   = virusimparallelv2
SRCS      = virusim.cpp Random.cpp Population.cpp
OBJS      = $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o

.cpp.o: 
	$(CXX) -c $(CXXFLAGS) $<

all: $(PROGRAM) 

$(PROGRAM): $(OBJS)
	$(CXX) -o $(PROGRAM) $(SRCS) $(CXXFLAGS) $(LDFLAGS)

clean:
	/bin/rm -f $(OBJS) $(PROGRAM)
