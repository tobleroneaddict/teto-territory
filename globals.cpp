#include "globals.h"

int* gFrameBuffer = nullptr;
SDL_Window* sdl_window = nullptr;
SDL_Renderer* sdl_renderer = nullptr;
SDL_Texture* sdl_texture = nullptr;
Game_Textures* textures = nullptr;
int WINDOW_WIDTH = 1300;
int WINDOW_HEIGHT = 900;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
