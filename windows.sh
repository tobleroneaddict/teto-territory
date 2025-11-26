x86_64-w64-mingw32-g++ \
  -Iwindowslibs/SDL3/include -Iinclude \
  *.cpp \
  -lSDL3 -Llib \
  -mwindows \
  -O2 -Wall \
  -o main.exe