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
//What kind of Item
enum ITEM_ID {
    ID_ITEM_BLUE_BARREL,
    ID_ITEM_WATER,
    ID_ITEM_LAMINE,
    ID_ITEM_RED_PHOSPHOROUS,
    ID_ITEM_LYE,
    ID_ITEM_PSEUDO,
    ID_ITEM_IODINE,
    ID_ITEM_ACID,
    ID_ITEM_PEROXIDE,
    ID_ITEM_MAX
};
//What kind of machine
enum MACHINE_ID {
    ID_MACHINE_CHEST,
    ID_MACHINE_CENTRIFUGE,
    ID_MACHINE_BOILER_TANK,
    ID_MACHINE_PLC,
    ID_MACHINE_OVEN,
    ID_MACHINE_MAX
};

//Objects used for cooking, tools, etc.
//The car also has one item slot
class Item {
public:
    ITEM_ID id;
    float count = 1; //how many units of this stuff
    SDL_FRect rect;
    SDL_Texture* texture;
    void set_texture(SDL_Texture* texas);
};
//CAN CONTAIN MULTIPLE ITEMS
class Machine{
public:
    //Depending on the machine id, it processes ONE OR TWO items into an output.
    MACHINE_ID id;
    
    //BEHAVIOR:
    //An input (A or B) can be either an item, or a machine's output.
    //If it is an item, the item will be displayed.
    //If it is a machine, the pipe will instead be displayed, and logic will use the item from the parent output.
    //Likewise, the product will show an item OR a pipe.
    //If a pipe is removed, nothing happens because the product is still there. 

    //Resources 
    Item* item_A; //Not compatibile with pipes
    Item* item_B; //Not compatibile with pipes
    
    Item* product;

    //Routing
    Machine* input_A_machine;
    Machine* input_B_machine;
    Machine* output_machine;
    //As host machine, i can connect my singular output to a child machine. 
    void connect_output(Machine* child, bool a_or_b); //Connect output pipe to a or b of a child machine
    void sever_output(); //Sever output pipe to the child machine 

    //World
    float x,y;
    //Graphics
    SDL_FRect rect = {0,0,256,256}; //Size of machine TEXTURE (might will depending on machine).
    SDL_Texture* texture;
    void render(int xoff, int yoff, int mode); //Renders this & the objects. maybe the output pipe too, down the road. Takes player pos
    void render_pipe_to_player(float xoff, float yoff);
private:
    void render_pipe(float wx1, float wy1,float dx, float dy, float length, float angle); //screen space piping
};

struct Machine_Selecting {
    Machine* nearby; //Machine nearby (one that triggers hittable)
    Machine* last; //Last machine clicked
    //PIPING
    bool output_hittable;
    bool inputa_hittable;
    bool inputb_hittable;
    bool removable; //can remove link
    bool inputa_removable;
    bool inputb_removable;
    //STATIC ITEM
    bool item_a_hittable;
    bool item_b_hittable;
    


};

//CAN CONTAIN ITEM
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
        std::vector<Machine> machines; //YEAHH FACTORIOOO
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
