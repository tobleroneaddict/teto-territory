CXX = g++
CXXFLAGS = -O2 -Wall
LIBS = -lSDL3 -lSDL3_image -ltinyxml2
HDRS = -I include
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
