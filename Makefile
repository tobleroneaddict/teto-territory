CXX = g++

CXXFLAGS = -O2 -Wall

LIBS = -lSDL3 -lSDL3_image -ltinyxml2



SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)



main: $(OBJS)

	$(CXX) $(OBJS) $(LIBS) -o main



%.o: %.cpp

	$(CXX) $(CXXFLAGS) -c $< -o $@



clean: