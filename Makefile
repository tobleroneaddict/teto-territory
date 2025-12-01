CXX = g++
CXXFLAGS = -O2 -Wall
LIBS = -lSDL3 -lSDL3_image -ltinyxml2 -lSDL3_ttf
HDRS = -I include
SRC = ./src
SRCS = $(wildcard $(SRC)/*.cpp)
OBJS = $(SRCS:.cpp=.o)
OUT = bin/
all: build

build: $(OBJS)
	@mkdir -p $(OUT)
	$(CXX) $(HDRS) $(LIBS) $(OBJS) -o $(OUT)/teto-territory

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(HDRS) -c $< -o $@
run: build
	$(OUT)/teto-territory
clean:
	@rm -rf $(OUT)
	@rm -rf $(OBJS)
