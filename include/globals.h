#pragma once

#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../include/SDL_image.h"
#include "vector"
#include <SDL3/SDL_time.h>
#include "textures.h"
#include <sstream>
#include <cstdint>
#include "tmxparse.h"
#include "SDL3_ttf/SDL_ttf.h"

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
        float tx,ty; //target x, target y. doesnt explode until near tgt
        float life_timer = 10000.0f; // 10 seconds
        SDL_FRect rect;
};

enum ITEM_ID {
    ID_BLUE_BARREL,
    ID_ITEM_MAX
};

//Objects used for cooking, tools, etc.
//The car also has one item slot
class Item {
public:
    ITEM_ID id;
    int count = 1;
    SDL_FRect rect;
    SDL_Texture* texture;
    void set_texture(SDL_Texture* texas);
};




//A car (just one for now)
class Car {
public:
    float x, y, xv_own, yv_own; //xv,yv are for if teto is out of the car. when she is in, it = her xvyv. when out, it will continue to drive.
    bool flip = false; //is bool bc we wanna save it for when teto is not driving
    bool brakes = false;
    SDL_FRect rect;
    //Inventory (rendered in back of car)
    Item* stored_item;

    void render(int xoff, int yoff);
};

//Dropped item
class Drop {
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

//UI block show container
class UI_Blocks {
public:
    // at top of screen shows [] [] [] [] [] [] [[]] [] [] [] [] [] []
    SDL_FRect thisblock;
    TMX* tiles;
    SDL_Texture* atlas;
    void render_UI_Block(int center_block_id,bool drawfg_thing);
};

//Lotsa things
class World_C {
    public:
        std::vector<Enemy> enemies;
        std::vector<Bullet> bullets;
        std::vector<Drop> dropped_Drops;
        std::vector<Rocket> rockets;
        std::vector<Horse> horses;
        std::vector<Item> items; //all the items in the world, handle inventories as pointers to some item in ths.

        int selected_block = 0;

        //Top UI bar
        UI_Blocks ui_block_selector; bool draw_on_foreground = false;
        SDL_FRect highlight; //block highlighter
        TMX* tiles;
        SDL_FRect tiler; //Tile texturer

        bool hide_top = false; //hide top layer?

        Car teto_car; //Teto's car //A car (just one for now)
        void renderLayer(float player_x, float player_y,int layer, TMX* tiles);
        void renderBlockHighlight(float player_x, float player_y, float mx, float my);
};
