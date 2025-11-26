#include "globals.h"
float RUN_SPEED = 0.5f;
float CAR_SPEED = 1.5f;

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
float get_distance(float x1, float y1, float x2, float y2) {
    float dx = (x1 - x2);
    float dy = (y1 - y2);

    float ddddd = sqrtf(dx*dx+dy*dy);
    return ddddd;
}

                //p.x      //p.y  passed in
void Car::render(int xoff, int yoff) {
    rect.x = x - xoff + WINDOW_WIDTH/2 - rect.w/2 ;
    rect.y = y - yoff + WINDOW_HEIGHT/2 - rect.h/2 ;
    
    //rect.x = x - xoff + rect.w/2 + 100; //draw in center
    //rect.y = y - yoff + rect.h/2 + 150;
    if (flip) {
        SDL_RenderTextureRotated(sdl_renderer,textures->car,nullptr,&rect,0,nullptr,SDL_FLIP_HORIZONTAL);
    } else {
    SDL_RenderTexture(sdl_renderer,textures->car,nullptr,&rect); //Draw car regardless
    }
}