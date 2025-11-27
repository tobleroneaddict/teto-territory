seeplusplus = g++

seeplusplusflags = -O2 -Wall

liberals = -lSDL3 -lSDL3_image -ltinyxml2



workscited = $(wildcard *.cpp)

objectificationoflouis = $(workscited:.cpp=.o)



main: $(objectificationoflouis)

	$(seeplusplus) $(objectificationoflouis) $(liberals) -o main



%.o: %.cpp

	$(seeplusplus) $(seeplusplusflags) -c $< -o $@



clean: