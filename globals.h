#pragma once

#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "include/SDL_image.h"
#include "vector"
#include <SDL3/SDL_time.h>
#include "textures.h"


extern int* gFrameBuffer;
extern SDL_Window* sdl_window;
extern SDL_Renderer* sdl_renderer;
extern SDL_Texture* sdl_texture;
extern Game_Textures* textures;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
float lerp(float a, float b, float t);


class Enemy {
public:
    float x, y, xv, yv;
    int id; bool rendering = true;
    float health = 100;
    float damage_cooldown = 1000; //milliseconds
};
    
class Bullet {
public:
    float x, y, vx, vy;
    float life_timer = 1000.0f; // 10 seconds
};
    
//Dropped item
class Item {
public:
    
    SDL_Texture* texture;
    int id = -1;
    float x, y, vx, vy, scale;
};

class World_C {
    public:
        std::vector<Enemy> enemies;
        std::vector<Bullet> bullets;
        std::vector<Item> dropped_items;
};


