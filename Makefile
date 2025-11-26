CXX = g++
CXXFLAGS = -I include/TMXLoader -O2 -Wall
LIBS = -lSDL3 -lSDL3_image

SRCS = $(wildcard *.cpp include/TMXLoader/*.cpp)
OBJS = $(SRCS:.cpp=.o)

main: $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o main

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o include/TMXLoader/*.o main
