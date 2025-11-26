#pragma once

#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "include/SDL_image.h"
#include "vector"
#include <SDL3/SDL_time.h>
#include "textures.h"
#include <sstream>
#include <cstdint>


extern int* gFrameBuffer;
extern SDL_Window* sdl_window;
extern SDL_Renderer* sdl_renderer;
extern SDL_Texture* sdl_texture;
extern Game_Textures* textures;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern float RUN_SPEED;
extern float CAR_SPEED;
extern int MAX_WORLD_X;
extern int MAX_WORLD_Y;

float lerp(float a, float b, float t);
float get_distance(float x1, float y1, float x2, float y2);

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
    float life_timer = 10000.0f; // 10 seconds
};

class Rocket {
public:
        float x, y, vx, vy, scale, rotation;
        float life_timer = 10000.0f; // 10 seconds
        SDL_FRect rect;
};

//A car (just one for now)
class Car {
public:
    float x, y, xv_own, yv_own; //xv,yv are for if teto is out of the car. when she is in, it = her xvyv. when out, it will continue to drive.
    bool flip = false; //is bool bc we wanna save it for when teto is not driving
    bool brakes = false;
    SDL_FRect rect;
    void render(int xoff, int yoff);
};
    
//Dropped item
class Item {
public:
    SDL_Texture* texture;
    int id = -1;
    float x, y, vx, vy, scale;
};

//Horse racing 
class Horse {
public:
    float x,y, vx, vy; //vx vy are -1,1
    
    int colour;
};

class World_C {
    public:
        std::vector<Enemy> enemies;
        std::vector<Bullet> bullets;
        std::vector<Item> dropped_items;
        std::vector<Rocket> rockets;
        std::vector<Horse> horses;
        Car teto_car; //Teto's car //A car (just one for now)
};


