#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../include/SDL_image.h"
#include "vector"

#include <SDL3/SDL_time.h>
#include <sstream>
#include <cstdint>
//Game Mechanics
#include "../include/globals.h"
#include "blackjack_engine.h"
#include "phone.h"
#include "legacyitemplacementcode.h"
//World
#include "tmxparse.h"

bool fullscreen = false;

Machine_Selecting mach_selecting;

//World of all da stuff
World_C world;

bool stop = false; //stop sdl

SDL_Color font_color = {255, 255, 255, 255};

float mx,my;

using namespace std;

//TO IMPLEMENT:::::::::::::::::::::::::::
//Hint tooltips for possible interactions
std::vector<std::string> hint_stack;


//Max bullets before start popping them from a list
const int MAX_BULLETS_ALLOWED = 15;
float TIME_BETWEEN_GUNSHOTS = 100; //10 rps


Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0; //milliseconds



//Visual, checker scale and spacing scale
int checker_scale = 32;
int space_scale = 2;
uint32_t checker_fill_color = 0xFF330077;
uint32_t line_color = 0xFF110044;
uint32_t checker_big_line_color = 0xFFAA44AA;



SDL_FRect food;

SDL_FRect gun_rect;

SDL_FRect ch_rect; //crosshair


SDL_FRect bullet_rect;

SDL_FRect horse_rect;


SDL_FRect vingette_rect;

class Teto_C {
public:
    //Physics
    float x,y,xv,yv, gun_out_x,gun_out_y;
    float max_speed = RUN_SPEED;

    //Weapon
    int ammo;
    float bullet_cooldown;

    //Inventory
    bool holding_weapon = false;
    Item* inventory = nullptr; //One item teto can hold

    World_C *world;

    //Flags and status timers
    bool drunk = false;
    float drunk_timer = 0;
    bool driving = false;
    bool alt = false;

    //Rendering
    bool teto_rendering = true;
    SDL_Texture* teto_textureL;
    SDL_Texture* teto_textureR;
    SDL_FRect player_rect;

    void run_motion();
    void fire_weapon();
};

void Teto_C::run_motion() { //also handles bullet cooldown
    //Run bio
    if (drunk_timer < 10) {
        drunk_timer = 0; drunk = false;
        player_rect.h = 200;
        player_rect.w = 80;
    } //if below zero, set to zero
    else {
        player_rect.h = 200;
        player_rect.w = 110;
        drunk_timer -= deltaTime; drunk = true;
     } //otherwise, decrease

    //Keyboard inputs
    SDL_PumpEvents(); //pump events??? that sounds nasty

    const bool *keys = SDL_GetKeyboardState(NULL);
    float max_speed_f = drunk ? max_speed * 0.01f : max_speed; //slower when drunk
    if (driving) max_speed_f = CAR_SPEED;

    //Input vectors
    float inx = 0, iny = 0; float input_scale = 0.01;
    iny -= (keys[SDL_SCANCODE_W])    * input_scale  * deltaTime;
    iny -= -(keys[SDL_SCANCODE_S])   * input_scale  * deltaTime;
    inx += -(keys[SDL_SCANCODE_A])   * input_scale  * deltaTime;
    inx += (keys[SDL_SCANCODE_D])    * input_scale  * deltaTime;

    //DUI handling
    if (drunk && driving) {
        if (((int)drunk_timer >> 9) % 2) {
            inx = -iny;
        }
        xv += sin(x) * cos(y);
        yv += (sin(x) * cos(y))/2;
    }


    //Calc accel

    //Clamping
    float velocity = sqrt((xv * xv) + (yv * yv));

    if (velocity > max_speed_f)
    {
        //Normalize n clamp
        float nx = xv / velocity;
        float ny = yv / velocity;

        xv = max_speed_f * nx;
        yv = max_speed_f * ny;

    }

    xv += inx * deltaTime;
    yv += iny * deltaTime;

    //Drag factor
    float drag = (driving) ? 0.99f: 0.9f;
    if (driving) {drag = (world->teto_car.brakes) ? 0.75f: drag;} //HIT DA BRAKES!! DURING Spacebar
    xv *= drag;
    yv *= drag;

    //if not top, left edge
    bool left_edge_passed = ((x/* - WINDOW_WIDTH/2*/) + xv * deltaTime < 0);
    bool top_edge_passed  = ((y/* - WINDOW_HEIGHT/2*/)+ yv * deltaTime < 0);

    //ONLY RESTRICT IF GOING UP/LEFT WARDS, scoop player back in. one way valve
    if (xv > 0) left_edge_passed = false;
    if (yv > 0) top_edge_passed = false;

    //Apply accel
    if (!left_edge_passed) x += xv * deltaTime;
    if (!top_edge_passed) y += yv * deltaTime;

    //bullet cooldown
    if (bullet_cooldown < 0) { bullet_cooldown = 0;} //if below zero, set to zero
    else {bullet_cooldown -= deltaTime; } //otherwise, decreases
    return;
}

void Teto_C::fire_weapon() {
    if (drunk) return; //cant shoot when feld

    bullet_cooldown = TIME_BETWEEN_GUNSHOTS; //350 ms
    //remove one if too many, max of 4
    if (world->bullets.size() > MAX_BULLETS_ALLOWED) {
        world->bullets.pop_back(); // Pop it - Andora
    }
    Bullet ebullet;

    //Center first.... then we...
    ebullet.x = x;
    ebullet.y = y;

    float ofx, ofy; //realy bad formatting, these are for the muzzle accounting
    //....offset!
    if (mx > WINDOW_WIDTH/2) {
        ebullet.x += 120; ofx = 120;
        ebullet.y -= 35; ofy = -35;
    } else {
        ebullet.x -= 80; ofx = -80;
        ebullet.y -= 35; ofy = -35;
    }
    //Depending on the direction player is facing we have to account for where the muzzle(?) of the gun is, to get proper aiming.

    float dx = mx - (WINDOW_WIDTH/2) - ofx;
    float dy = my - (WINDOW_HEIGHT/2) - ofy;

    float ebullet_mouse_dist_to_norm = sqrt(dx*dx+dy*dy);

    float nx = dx / ebullet_mouse_dist_to_norm;
    float ny = dy / ebullet_mouse_dist_to_norm;
                            //Tack on player v
    ebullet.vx = (nx * 10) +xv;
    ebullet.vy = (ny * 10) +yv;

    //add bullet
    world->bullets.push_back(ebullet);
    return;
}


int main() {
    //Setup window
    gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
    sdl_window = SDL_CreateWindow("Rainbet 2", WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_RESIZABLE);
    //Setup font
    if (!TTF_Init()) {cout << "TTF Error!\n"; return 1;}
    TTF_Font* font = TTF_OpenFont("Assets/InclusiveSans-Medium.ttf", 24);
    //Setup renderer
    sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
    SDL_SetRenderVSync(sdl_renderer, 0); //i had to do this to fix stuttying
    sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);


    SDL_SetTextureScaleMode(sdl_texture,SDL_SCALEMODE_NEAREST);
    textures = new Game_Textures;
    TMX tiles;
    world.tiles = &tiles;
    //Load map
    if (!tiles.load("Tiled/map.tmx")) return 1;

    //Load tiles class into the block selecter UI
    world.ui_block_selector.tiles = world.tiles;
    world.selected_block = 13; //set default to brick

    if (!gFrameBuffer || !sdl_window || !sdl_renderer || !sdl_texture) {
        cout << "SDL3 init error!\n";
        return 1;
    }
    if (font == nullptr) { cout << "Font error!\n"; return 1;}

    //Shitty way of getting rand seed
    srand(std::uintptr_t(&sdl_texture));

    //Contains all the textures (call after the sdl; init stuff)

    textures->load(sdl_renderer);
    SDL_Delay(100); //help stuff not die
    Phone_C phone; phone.phone_rect.y = WINDOW_HEIGHT - 100;
    phone.phone_rect.h = textures->phone->h;
    phone.phone_rect.w = textures->phone->w;
    phone.rainbet_hs.h = textures->hitstand->h;
    phone.rainbet_hs.w = textures->hitstand->w;
    phone.blackjack = new blackjack_engine;
    blackjack_engine* blackjack = phone.blackjack; //to help referencing in here

    blackjack->dealer.dealer = true;
    blackjack->bet_locked_in = false;

    Teto_C teto;
    teto.world = &world;


    teto.x = 20*64;
    teto.y = 23*64;

    world.teto_car.x = teto.x;
    world.teto_car.y = teto.y + 300;

    food.h = 80;
    food.h = 120;
    food.w = 230;

    vingette_rect.x = 0;
    vingette_rect.y = 0;


    world.tiler.h = 256;
    world.tiler.w = 256;

    gun_rect.h = 80;
    gun_rect.w = 180;

    teto.bullet_cooldown = 0;

    world.teto_car.rect.w = 600;
    world.teto_car.rect.h = 300;


    ch_rect.h = textures->ch->h;
    ch_rect.w = textures->ch->w;
    world.highlight.h = 64;
    world.highlight.w = 64;


    bullet_rect.h = 32;
    bullet_rect.w = 32;
    horse_rect.h = 29*2;
    horse_rect.w = 33*2;

    SDL_Delay(100); //help stuff not die

    teto.teto_rendering = true;
    //Spawn enemies
    for (int i = 0; i < 100; i++) {
        Enemy enemeeee;
        enemeeee.rendering = true;
        enemeeee.id = rand();
        enemeeee.x  = rand()  % MAX_WORLD_X;
        enemeeee.y  = rand()  % MAX_WORLD_Y;
        world.enemies.push_back(enemeeee);
    }


    SDL_HideCursor();

    //Randomly spawn beers
    for (int i = 0; i < 30; i++) {
        Drop lilitem;               lilitem.texture = textures->capsule;
        lilitem.x = rand()  % MAX_WORLD_X;
        lilitem.y = rand()  % MAX_WORLD_Y;
        lilitem.scale = 0.2f;
        lilitem.id = 9;
        //world.dropped_Drops.push_back(lilitem);
    }

    //Randomly spawn horses

    for (int i = 0; i < 10; i++) {
        Horse lehorse;  lehorse.colour = rand() % 10;
        lehorse.vx = (rand() % 3)-1; lehorse.vy = (rand() % 3)-1;
        if (lehorse.vx == 0) lehorse.vx = 1;
        if (lehorse.vy == 0) lehorse.vy = 1;

        lehorse.x = rand()  % MAX_WORLD_X;
        lehorse.y = rand()  % MAX_WORLD_Y;
        world.horses.push_back(lehorse);
    }



    //DEBUG: add barrel to car
    Item thisitem;
    thisitem.id = ID_BLUE_BARREL;
    thisitem.set_texture(textures->cooking_barrel);


    world.items.emplace_back(thisitem);

    world.teto_car.stored_item = &world.items[0];

    //How to make a machine!:
    for (int i = 1; i <= 2; i++) {
        Machine mach;

        //Very important note:::::::::: This is the machine top texture.
        //Renders MACHINE BASIC mode 0    PIPES mode 1  MACHINE TEXTURE (top) mode 2
        mach.texture = textures->machine_tank;
        mach.x = mach.y = teto.x + (300*i);
        mach.x -= fmod(mach.x, 64);         //Really cool way of constraining to tile (ALWAYS DO THIS ON INSTANTIATION)
        mach.y -= fmod(mach.y, 64);
        mach.id = ID_CHEST; //sample
        world.machines.push_back(mach);

    }
    world.machines[0].connect_output(&world.machines[1],1);

    


    while (!stop)
    {
        //dTs
        LAST = NOW;NOW = SDL_GetPerformanceCounter(); deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );
        //Clean the hint stack
        hint_stack.clear();

        SDL_GetWindowSizeInPixels(sdl_window,&WINDOW_WIDTH,&WINDOW_HEIGHT); //Resizing
        //Center player & gun
        teto.player_rect.x = (WINDOW_WIDTH/2)-40;
        teto.player_rect.y = (WINDOW_HEIGHT/2)-100;
        gun_rect.x = teto.player_rect.x - 100;
        gun_rect.y = teto.player_rect.y + 30;


        SDL_GetMouseState(&mx,&my);

        SDL_SetRenderDrawColor(sdl_renderer, 0, 150, 150, SDL_ALPHA_OPAQUE);


        //Run teto user motion
        teto.run_motion();


        /*
        _   _ ___ _   _ _____ ____  
        | | | |_ _| \ | |_   _/ ___| 
        | |_| || ||  \| | | | \___ \ 
        |  _  || || |\  | | |  ___) | 
        |_| |_|___|_| \_| |_| |____/        (and pipe select calcs)
                                    
        */
        //Append hint possibilities. this is derived from the input stuff, so if u update anything in there do them here as well
        //Machine input selections is also calculated here, to prevent time wasting double handling.
        {
            float carinventorydist = get_distance(teto.x,teto.y,(world.teto_car.flip) ? world.teto_car.x + 100 : world.teto_car.x - 100,world.teto_car.y);
            float cardist = get_distance(teto.x,teto.y,world.teto_car.x,world.teto_car.y);
            
            //PIPE HINTS (AND PIPE SELECT CALCULATIONS)
            if (mach_selecting.last != nullptr) { hint_stack.emplace_back("[Holding Pipe] R: Drop Pipe");}
            else if (carinventorydist < 90) hint_stack.emplace_back("E: Swap item in car");

            //Machines
            if (!teto.driving) {
                for (int i = 0; i < (int)world.machines.size(); i++) {
                    Machine* thismach = &world.machines[i];
                    float distance = get_distance(teto.x,teto.y,thismach->x,thismach->y);
                    if (distance < 300) {
                        
                        //input A dist
                        float distanceA = get_distance(teto.x,teto.y,thismach->x-96,thismach->y-160);
                        //input B dist
                        float distanceB = get_distance(teto.x,teto.y,thismach->x+96,thismach->y-160);
                        //output dist
                        float distanceO = get_distance(teto.x,teto.y,thismach->x,thismach->y+160);


                        //Output first, then input

                        //Allow connect pipe action
                        if (distanceA < 95 && mach_selecting.last != nullptr)  {(thismach->input_B_machine == nullptr && thismach->input_A_machine == nullptr) ? hint_stack.emplace_back("E: Connect Input A") : hint_stack.emplace_back("E: Reroute Input A"); mach_selecting.inputa_hittable = true; mach_selecting.nearby = thismach; } else {mach_selecting.inputa_hittable = false;}
                        if (distanceB < 95 && mach_selecting.last != nullptr)  {(thismach->input_B_machine == nullptr && thismach->input_A_machine == nullptr) ? hint_stack.emplace_back("E: Connect Input B") : hint_stack.emplace_back("E: Reroute Input B"); mach_selecting.inputb_hittable = true; mach_selecting.nearby = thismach; } else {mach_selecting.inputb_hittable = false;}
                        if (distanceO < 140 && mach_selecting.last == nullptr) {(thismach->output_machine  == nullptr) ? hint_stack.emplace_back("E: Connect Output") : hint_stack.emplace_back("E: Reroute Output"); mach_selecting.output_hittable = true; mach_selecting.nearby = thismach; } else {mach_selecting.output_hittable = false;}
                        if (mach_selecting.inputa_hittable + mach_selecting.inputb_hittable + mach_selecting.output_hittable == 0) {mach_selecting.nearby = nullptr;} //if none selectable, nearby is null.
                        //Allow remove pipe action (must not be holding a pipe) (THIS IS NOT DROP PIPE)
                        mach_selecting.removable = false; //clear flag for rewrite

                        //f (distanceA < 95 && thismach->input_A_machine != nullptr) {mach_selecting.inputa_removable = true;} else { mach_selecting.inputa_removable = false;}
                        //if (distanceB < 95 && thismach->input_B_machine != nullptr) {mach_selecting.inputb_removable = true;} else { mach_selecting.inputb_removable = false;}
                        //Feeds down
                        if (distanceO < 140&& mach_selecting.last == nullptr && thismach->output_machine != nullptr) {hint_stack.emplace_back("R: Remove Pipe");mach_selecting.removable = true;}
                        //if (distanceA < 95 && mach_selecting.last == nullptr && mach_selecting.inputa_removable) {hint_stack.emplace_back("R: Remove Pipe");mach_selecting.removable = true;}
                        //if (distanceB < 95 && mach_selecting.last == nullptr && mach_selecting.inputb_removable) {hint_stack.emplace_back("R: Remove Pipe");mach_selecting.removable = true;}
                        //TODO: allow this behavior from the child side
                        //Doesnt work yet
                    }
                }
            }
           

            if (cardist < 100) {
                if (teto.driving) {
                    hint_stack.emplace_back("C: Exit Car");    } else {hint_stack.emplace_back("C: Enter Car");}
            }
        }
        //cout << mach_selecting.inputa_hittable << mach_selecting.inputb_hittable << mach_selecting.output_hittable << endl;

        //Oneshot key events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
            return false;
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE)
            {
            return false;
            }


            //THIS IS WHERE TO PUT CLICK EVENTS!!!!!!!!!!!!!!!!!
            //CLICK EVENTS
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {

                //Phone open/close
                if (mx < WINDOW_WIDTH - 67 && mx > WINDOW_WIDTH - 400 && my > phone.phone_rect.y && my < phone.phone_rect.y + 115) {
                    phone.active = !phone.active;

                } //END phone toggle
                //Phone screenactions
                else if (mx < WINDOW_WIDTH - 67 && mx > WINDOW_WIDTH - 400 && my > phone.phone_rect.y + 115)  {
                    //Phone screen events
                    //phone.rainbet_hs.y = phone.phone_rect.y + 500;
                    //phone.rainbet_hs.x = WINDOW_WIDTH - 365;
                    if (my > phone.phone_rect.y + 505 && my <= phone.phone_rect.y + 551 && mx > WINDOW_WIDTH - 365 && mx < WINDOW_WIDTH - 104) { //button Y (twoside), distinguish later
                        if (blackjack->bet_locked_in) {
                            //in gameplay or won
                            if (blackjack->won || blackjack->bust || blackjack->dealer_won || blackjack->tie) {
                                blackjack->bet_locked_in = false;
                            } else { //still gaming
                                //Distinguish between buttons
                                if (mx < WINDOW_WIDTH - 261) {
                                    blackjack->hit();
                                } else { blackjack->stand();}
                            }

                        }  else {
                            //play button
                            blackjack->start_game(); //button that shows on start, or win /lose condition
                        }
                    }

                } //END phone screen actions
                else { //WORLD ACTIONS
                    //Gun
                    if (teto.bullet_cooldown < 10 && teto.holding_weapon) teto.fire_weapon();


                    //PLACE BLOCK
                    else if (!teto.holding_weapon) {
                        //Block place
                        //mouse to world space with anchoring to 0.5 | 0.5
                        float screen_anchor_x = teto.x - WINDOW_WIDTH  / 2;
                        float screen_anchor_y = teto.y - WINDOW_HEIGHT / 2;
                        world.highlight.x = mx - world.highlight.w;
                        world.highlight.y = my - world.highlight.h;
                        float wx = mx + screen_anchor_x;
                        float wy = my + screen_anchor_y;
                        wx = floor(wx / 64.0f);
                        wy = floor(wy / 64.0f);

                        //cout <<wx << endl;

                        //Drop the selected block in
                        tiles.setblock(wx,wy,(world.draw_on_foreground) ? 1 : 0,world.selected_block);

                    }

                }

            }
            //MOUSE DONE NOW KEYS

            //Wraps the long, old placement code thats completely incomatible with everything else....which will probably be nuked someday
            legacy_placement_code(&world,teto.x,teto.y,e);

            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_K) { teto.alt = !teto.alt;}
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_G) { teto.holding_weapon = !teto.holding_weapon; }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_F) {
                Rocket newrocket; newrocket.x = teto.x; newrocket.y = teto.y + 80;
                newrocket.scale = 1; newrocket.vy -= 3.0f;
                float dx = mx - (WINDOW_WIDTH/2);
                float dy = my - (WINDOW_HEIGHT/2);
                float nx = dx / 1300 * 2.25f;
                newrocket.vy += (dy / WINDOW_HEIGHT) * 2;
                newrocket.vx = nx * 1;
                //Target (for explosion)
                newrocket.tx = teto.x + mx;
                newrocket.ty = teto.y + my;
                //cout << newrocket.tx << endl;
                world.rockets.emplace_back(newrocket);
            }

            //Swap item with inventory ( never deletes )
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_E) {
                //Pipe takes priority over Item transfer
                //Can swap with:
                //On ground   |    Car     |     Machine    | ....anything else....
                float cardist = get_distance(teto.x,teto.y,(world.teto_car.flip) ? world.teto_car.x + 100 : world.teto_car.x - 100      ,world.teto_car.y);
                
                if (mach_selecting.inputa_hittable || mach_selecting.inputb_hittable) //Place pipe
                {
                    if (mach_selecting.last != nullptr && mach_selecting.nearby != nullptr              && mach_selecting.last != mach_selecting.nearby) {
                        if (mach_selecting.inputa_hittable) {
                            mach_selecting.last->connect_output(mach_selecting.nearby, false);
                        } else if (mach_selecting.inputb_hittable) {
                            mach_selecting.last->connect_output(mach_selecting.nearby, true);
                        }
                        mach_selecting.last = nullptr; mach_selecting.nearby = nullptr; //clean up
                    } else {
                        if (mach_selecting.last == nullptr) cout << "last was null\n";
                        if (mach_selecting.nearby == nullptr) cout << "nearby was null\n";
                        
                    }    
                } else if (mach_selecting.output_hittable) { //Check machine output flag
                    
                    mach_selecting.last = mach_selecting.nearby; //Grab pipe
                } else if (cardist < 90) {
                    Item* item_temp;
                    item_temp = world.teto_car.stored_item;
                    world.teto_car.stored_item = teto.inventory;
                    teto.inventory = item_temp;
                }
            }
            //Remove pipe
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_R) { 
                if (mach_selecting.last != nullptr) { mach_selecting.last = nullptr;} //if holding pipe? DROP PIPE.
                else if (mach_selecting.removable) {
                    //Distinguish if this is the host or child
                    if (mach_selecting.inputa_removable || mach_selecting.inputb_removable) {
                        if (mach_selecting.inputa_removable) { //its A
                            mach_selecting.nearby->input_A_machine->sever_output();
                        } else if (mach_selecting.inputb_removable) { //its B
                            mach_selecting.nearby->input_B_machine->sever_output();
                        }
                    } else if (mach_selecting.output_hittable) {
                        mach_selecting.nearby->sever_output();
                    }
                    
                }
            }

            //Car exit/entry
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_C) {
                float cardist = get_distance(teto.x,teto.y,world.teto_car.x,world.teto_car.y);
                if (cardist < 100){ //Get in car
                teto.driving = !teto.driving; teto.x = world.teto_car.x;teto.y = world.teto_car.y;           }
            }
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_SPACE) { world.teto_car.brakes = true; teto.xv = 0; teto.yv = 0;} //BRAKES!!!!!!    // USES KEY DOWN / UP
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_SPACE) { world.teto_car.brakes = false;} //BRAKES!!!!!!    // USES KEY DOWN / UP
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_H) { //spawn horse
                Horse lehorse;  lehorse.colour = rand() % 10;
                lehorse.vx = (rand() % 3)-1; lehorse.vy = (rand() % 3)-1;
                if (lehorse.vx == 0) lehorse.vx = 1;
                if (lehorse.vy == 0) lehorse.vy = 1;
                //cout << world.horses.size() << endl;

                lehorse.x = teto.x;
                lehorse.y = teto.y;
                world.horses.push_back(lehorse);
            }
            //Show/hide top layer
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_T) {world.hide_top = !world.hide_top;}
            //Switch draw on foreground
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_Q) {world.draw_on_foreground = !world.draw_on_foreground;}
            //Fullscreen
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_F11) {fullscreen = !fullscreen; SDL_SetWindowFullscreen(sdl_window,fullscreen);}


            //Scrolling block
            if (e.type == SDL_EVENT_MOUSE_WHEEL) {
                if (e.wheel.y > 0 && world.selected_block > 1) {
                    world.selected_block--;
                } else if (e.wheel.y < 0  && world.selected_block < 99) {
                    world.selected_block++;
                }
            }
        }


        SDL_RenderClear(sdl_renderer);

        //Tiling (BACK LAYER)
        world.renderLayer(teto.x,teto.y,0,world.tiles);


        //End of tiling
        //Render play field
        //SDL_RenderTexture(sdl_renderer,sdl_texture,nullptr,nullptr);


        //Render enemies, run enemy code
        for (int i = 0; i < (int)world.enemies.size(); i++) {

            Enemy* curr = &world.enemies[i];

            //Enemy velocity
            //Drag factor
            curr->xv *= 0.9;
            curr->yv *= 0.9;
            //Apply accel
            bool left_edge_passed = ((curr->x/* - WINDOW_WIDTH/2*/) + curr->xv * deltaTime < 0);
            bool top_edge_passed = ((curr->y/* - WINDOW_HEIGHT/2*/)+ curr->yv * deltaTime < 0);

            //ONLY RESTRICT IF GOING UP/LEFT WARDS, scoop player back in. one way valve
            if (curr->xv > 0) left_edge_passed = false;
            if (curr->yv > 0) top_edge_passed = false;

            //Apply accel
            if (!left_edge_passed) curr->x += curr->xv * deltaTime;
            if (!top_edge_passed) curr->y += curr->yv * deltaTime;

            if (curr->damage_cooldown < 0) { curr->damage_cooldown = 0;}
            else { curr->damage_cooldown -= deltaTime; }

            //Making this a pointer let this actually decrease!
            //curr->damage_cooldown = curr->damage_cooldown - deltaTime;


            float dx = abs(teto.x - curr->x);
            float dy = (teto.y - curr->y);

            float scare_factor = sqrtf(dx*dx+dy*dy);
            //Random walk
            float randX = 0.5-(float)rand() / RAND_MAX;
            float randY = 0.5-(float)rand() / RAND_MAX;

            food.x = curr->x - teto.x + WINDOW_WIDTH/2 - food.w/2;
            food.y = curr->y - teto.y + WINDOW_HEIGHT/2 - food.h/2;


            if (teto.holding_weapon && scare_factor < 300) {
                food.x += randX * (300-scare_factor) * .06;
                food.y += randY * (300-scare_factor) * .03;
            }

            if (curr->rendering) {
                if (teto.alt) {
                    if (curr->id % 2) {
                        SDL_RenderTextureRotated(sdl_renderer,textures->agent,nullptr,&food,0,nullptr,SDL_FLIP_NONE);
                    } else { SDL_RenderTextureRotated(sdl_renderer,textures->agent,nullptr,&food,0,nullptr,SDL_FLIP_HORIZONTAL); }
                } else {
                    if (curr->id % 2) {
                        SDL_RenderTextureRotated(sdl_renderer,textures->enemy_texture,nullptr,&food,0,nullptr,SDL_FLIP_NONE);
                    } else { SDL_RenderTextureRotated(sdl_renderer,textures->enemy_texture,nullptr,&food,0,nullptr,SDL_FLIP_HORIZONTAL); }
                }
            }
        }


        SDL_FRect thingrect;

        //Render items
        for (int i = 0; i < (int)world.dropped_Drops.size(); i++) {
            Drop* thing = &world.dropped_Drops[i];

            thingrect.x = thing->x - teto.x + WINDOW_WIDTH/2 - 25;
            thingrect.y = thing->y - teto.y + WINDOW_HEIGHT/2 - 25;

            thingrect.h = thing->texture->h * thing->scale;
            thingrect.w = thing->texture->w * thing->scale;



            //Drunk
            if (thing->id == 9) {
                //Hit test with player
                float dx = abs(teto.x - thing->x);    float dy = (teto.y - thing->y);
                float distance = sqrtf(dx*dx+dy*dy);
                if (distance < 50) { //Collides with pill
                    //phone.blackjack->balance -= 44; //Fee
                    teto.drunk_timer = 10000;
                    world.dropped_Drops.erase(world.dropped_Drops.begin() + i); i--;
                }
            }

            if (thing->id == -1) cout << "u broke item ID as !" << thing->id << endl;
            SDL_RenderTexture(sdl_renderer,thing->texture,nullptr,&thingrect);

        }

        //Render Machines

        //BASIC PASS
        for (int i = 0; i < (int)world.machines.size(); i++) {
            Machine* thismach = &world.machines[i];
            thismach->render(teto.x,teto.y);
        }

        //Render player (on top)
        if (teto.teto_rendering) {
            //if (!teto.drunk) {
                if (teto.xv >= 0) {
                    if (teto.alt) {
                        SDL_RenderTextureRotated(sdl_renderer,textures->teto_texture_alt,nullptr,&teto.player_rect,0,nullptr,SDL_FLIP_NONE);
                    } else {
                        SDL_RenderTextureRotated(sdl_renderer,textures->teto_texture_r,nullptr,&teto.player_rect,0,nullptr,SDL_FLIP_NONE);
                    }
                    //SDL_RenderTexture(sdl_renderer,textures->teto_texture_r,nullptr,&teto.player_rect);
                }  else {
                    if (teto.alt) {
                        SDL_RenderTextureRotated(sdl_renderer,textures->teto_texture_alt,nullptr,&teto.player_rect,0,nullptr,SDL_FLIP_HORIZONTAL);
                    } else {
                        SDL_RenderTextureRotated(sdl_renderer,textures->teto_texture_r,nullptr,&teto.player_rect,0,nullptr,SDL_FLIP_HORIZONTAL);
                    }

                    //SDL_RenderTexture(sdl_renderer,textures->teto_texture_l,nullptr,&teto.player_rect);
                }
            //} else {
            //    SDL_RenderTexture(sdl_renderer,textures->teto_texture_fold,nullptr,&teto.player_rect);
            //}
        }

        //STARTCAR

        //If player isnt driving the car has to ZOOM, otherwise do car physics
        if (teto.driving) {
            world.teto_car.xv_own = teto.xv;
            world.teto_car.yv_own = teto.yv;
        } else {
            //ONLY WHEN PLAYER IS OUT OF ZE CAR
            //Drag factor
            world.teto_car.xv_own *= 0.995;
            world.teto_car.yv_own *= 0.995;
            //Apply accel
            world.teto_car.x += world.teto_car.xv_own * deltaTime;
            world.teto_car.y += world.teto_car.yv_own * deltaTime;
        }

        //Render car (on top of player)
        if (teto.driving) { //If driving, strap in
            world.teto_car.x = teto.x;
            world.teto_car.y = teto.y;
            world.teto_car.flip = teto.xv < 0;
        }
        //Now do car physics (regardless of driving)
        // Respawn
        if (world.teto_car.x < 0 || world.teto_car.y < 0 || world.teto_car.x > tiles.map->width*64 || world.teto_car.y > tiles.map->height*64) {world.teto_car.x = 600;world.teto_car.y = 600; world.teto_car.xv_own = 0; world.teto_car.yv_own = 0;}
        Enemy* curr;
        float total_car_v = sqrt((world.teto_car.xv_own * world.teto_car.xv_own) + (world.teto_car.yv_own * world.teto_car.yv_own));
        //Loop thru each enemy
        for (int plush = 0; plush < (int)world.enemies.size(); plush++) {
            curr = &world.enemies[plush];
            float dx = (world.teto_car.x - curr->x);
            float dxL = (world.teto_car.x - curr->x) - 100;
            float dxR = (world.teto_car.x - curr->x) + 100;
            float dy = (world.teto_car.y - curr->y) + 100; //car collider

            float distance = sqrtf(dxL*dx+dy*dy);
            float distanceL = sqrtf(dxL*dxL+dy*dy);
            float distanceR = sqrtf(dxR*dxR+dy*dy);

            //Handle player's car collision with enemy car
            if (distance < 0.1f) {continue;}if (distanceL < 0.1f) {continue;}if (distanceR < 0.1f) {continue;}
            if (distanceL < 100 || distance < 100 || distanceR < 100) { //car is like [o o o]

                //normalize...
                float nx = dx / distance;
                float ny = dy / distance;
                if (nx != nx || ny != ny) continue; //NaN
                curr->xv -= nx * total_car_v * 2.5f;
                curr->yv -= ny * total_car_v * 2.5f;
                //Apply 3rd law
                world.teto_car.xv_own += total_car_v * nx / 2;
                world.teto_car.yv_own += total_car_v * ny / 2;
                teto.xv += total_car_v * nx;
                teto.yv += total_car_v * ny;

                curr->damage_cooldown = 1000; //RESET COOLDOWN
            }
        }

        world.teto_car.render(teto.x,teto.y);

        //END CAR

        //START HORSE RACE TESTS

        SDL_FRect horse_srcRect;
        horse_srcRect.h = 30;
        horse_srcRect.w = 29;

        //cout <<world.horses.size() << endl;
        for (int i = 0; i < (int)world.horses.size(); i++) {
            Horse* b = &world.horses[i];
            if (b == nullptr) { break;}
            //Velocity
            b->x += b->vx;
            b->y += b->vy;
            //float offy = b->colour;
            //horse_srcRect.y = offy; //type
            horse_srcRect.x = 0;
            horse_srcRect.y = b->colour * 33;
            //Set to rect
            horse_rect.x = b->x - teto.x + WINDOW_WIDTH/2 -horse_rect.w/2;
            horse_rect.y = b->y - teto.y + WINDOW_HEIGHT/2 -horse_rect.h/2;
            SDL_RenderTexture(sdl_renderer,textures->horse,&horse_srcRect,&horse_rect);

            //Hit testing
            Enemy* curr;
            //Loop thru each enemy


            //Wall collision
            if (b->x > MAX_WORLD_X || b->y > MAX_WORLD_Y) {
                //cout << "e\n";
                b->vx = -b->vx; b->vy = -b->vy;
            }


            for (int plush = 0; plush < (int)world.enemies.size(); plush++) {
                curr = &world.enemies[plush];
                float dx = (b->x - curr->x);
                float dy = (b->y - curr->y);

                float distance = sqrtf(dx*dx+dy*dy);

                if (distance < 0.0001f) continue;
                if (distance < 80) {

                    //Horse Test type bouncing
                    if (fabs(dx) > fabs(dy)) {
                        b->vx = -b->vx;
                    } else {
                        b->vy = -b->vy;
                    }

                    //curr->damage_cooldown = 1000; //RESET COOLDOWN
                }
            }

        }



        //END HORSE

        //RENDER UNREGISTERED FIREARM
        if (mx > WINDOW_WIDTH/2) {
            gun_rect.w = -180;
            gun_rect.x = teto.player_rect.x + 150;
            //recoil
            gun_rect.x += (10-(teto.bullet_cooldown / 10));
        } else {
            gun_rect.w = 180;
            gun_rect.x = teto.player_rect.x - 50;
            gun_rect.x -= (10-(teto.bullet_cooldown / 10));
        }
        //angle to cursor
        { //scope
            float dx = mx - (WINDOW_WIDTH/2);
            float dy = my - (WINDOW_HEIGHT/2);

            float ebullet_mouse_dist_to_norm = sqrt(dx*dx+dy*dy);

            float nx = dx / ebullet_mouse_dist_to_norm;
            float ny = dy / ebullet_mouse_dist_to_norm;
            float isonright = (mx > (WINDOW_WIDTH/2)) ? 195.0f : 0.0f;

            if (teto.holding_weapon && !teto.drunk)    SDL_RenderTextureRotated(sdl_renderer,textures->gun_texture,nullptr,&gun_rect,((atan2(ny,nx)+90.0f)*57.9f)+isonright,nullptr,SDL_FLIP_NONE); //The gun rect is just flipped
            //if (teto.holding_weapon && !teto.drunk) SDL_RenderTexture(sdl_renderer,textures->gun_texture,nullptr,&gun_rect);
        }

        //Render bullet & do hit physics
        for (int i = 0; i < (int)world.bullets.size(); i++) {
            Bullet* b = &world.bullets[i];
            if (b == nullptr) { break;}
            //Velocity
            b->x += b->vx;
            b->y += b->vy;
            b->life_timer -= deltaTime;
            //Set to rect
            bullet_rect.x = b->x - teto.x + WINDOW_WIDTH/2 -bullet_rect.w/2;
            bullet_rect.y = b->y - teto.y + WINDOW_HEIGHT/2 -bullet_rect.h/2;
            SDL_RenderTexture(sdl_renderer,textures->bullet,nullptr,&bullet_rect);

            //Hit testing
            Enemy* curr;
            //Loop thru each enemy
            for (int plush = 0; plush < (int)world.enemies.size(); plush++) {
                curr = &world.enemies[plush];
                float dx = (b->x - curr->x);
                float dy = (b->y - curr->y);

                float distance = sqrtf(dx*dx+dy*dy);

                if (distance < 0.0001f) continue;
                if (distance < 30) {

                    //normalize...
                    float nx = dx / distance;
                    float ny = dy / distance;

                    curr->xv -= nx * 3;
                    curr->yv -= ny * 3;
                    curr->damage_cooldown = 1000; //RESET COOLDOWN
                    //erase kept segfaulting
                    b->x = 999999;
                }
            }
            //Bullet life
            if (b->life_timer <= 0) {
                world.bullets.erase(world.bullets.begin() + i); i--;
            }


        }

        //Render rockets
        //Render bullet & do hit physics
        for (int i = 0; i < (int)world.rockets.size(); i++) {
            Rocket* r = &world.rockets[i];
            if (r == nullptr) { break;}

            r->rect.h = 100 * r->scale;
            r->rect.w = 100 * r->scale;
            //Gravity
            r->vy += deltaTime * 0.01f;
            //Velocity
            r->x += r->vx * deltaTime;
            r->y += r->vy * deltaTime;
            r->life_timer -= deltaTime; //advance life timer
            //Rotation
            r->rotation = atan2(r->vy,r->vx) * 57.29f + 90.0f;


            //Set to rect
            r->rect.x = r->x - teto.x + WINDOW_WIDTH/2- 50;
            r->rect.y = r->y - teto.y + WINDOW_HEIGHT/2 - 50;
            //SDL_RenderTexture(sdl_renderer,textures->rocket,nullptr,&r->rect);

            SDL_RenderTextureRotated(sdl_renderer,textures->rocket,nullptr,&r->rect,r->rotation,NULL,SDL_FLIP_NONE);
            //Hit testing
            Enemy* curr;

            //Loop thru each enemy
            for (int plush = 0; plush < (int)world.enemies.size(); plush++) {
                curr = &world.enemies[plush];
                float dx = (r->x - curr->x);
                float dy = (r->y - curr->y);

                //General plush detection distance
                float distance = sqrtf(dx*dx+dy*dy);


                if (distance < 0.0001f) continue;

                if (distance < 30) {

                    //normalize...
                    float nx = dx / distance;
                    float ny = dy / distance;

                    curr->xv -= nx * 3;
                    curr->yv -= ny * 3;
                    curr->damage_cooldown = 1000; //RESET COOLDOWN
                    //erase kept segfaulting
                    r->x = 999999;
                }
            }
            //Bullet life
            if (r->life_timer <= 0) {
                world.rockets.erase(world.rockets.begin() + i); i--;
            }
        }



        //WORLD RENDERING Tiling (FRONT LAYER)(FRONT LAYER)(FRONT LAYER)(FRONT LAYER)(FRONT LAYER)(FRONT LAYER)(FRONT LAYER)(FRONT LAYER)
        if (!world.hide_top) world.renderLayer(teto.x,teto.y,1,world.tiles);

        world.renderBlockHighlight(teto.x,teto.y,mx,my);

        //END BLOCK HL

        //PHONE RENDERING
        SDL_RenderTexture(sdl_renderer,textures->phone,nullptr,&phone.phone_rect);

        //Blackjack ,this will probably have a bug where it shows the bj during the animation
        if (phone.active) {
            phone.play_rainbet();

        }

        //Encodes bet into rainbet before locking in
        if (!blackjack->bet_locked_in) blackjack->bet = abs((int)teto.x) % blackjack->balance;

        phone.update_phone();

        //RENDER BLOCK UI
        world.ui_block_selector.render_UI_Block(world.selected_block,world.draw_on_foreground);


        //crosshair
        ch_rect.x = mx - ch_rect.w + textures->ch->w/2;
        ch_rect.y = my - ch_rect.h + textures->ch->h/2;
        if (teto.bullet_cooldown < 10) SDL_RenderTexture(sdl_renderer,textures->ch,nullptr,&ch_rect);

        //Methinks that player item rendering should go here
        
        if (teto.inventory != nullptr) {
            teto.inventory->rect.x = 0;
            teto.inventory->rect.y = WINDOW_HEIGHT-128;
            SDL_RenderTexture(sdl_renderer,teto.inventory->texture,nullptr,&teto.inventory->rect);
        }

        //Vingette if drunk
        if (teto.drunk) {
            vingette_rect.h = WINDOW_HEIGHT;
            vingette_rect.w = WINDOW_WIDTH;

            SDL_RenderTexture(sdl_renderer,textures->vingette,nullptr,&vingette_rect);
        }

        //test font
        SDL_Color color = {255, 255, 255, 255}; // white
        //HINTS

        //Loop thru and append hints at bottom of screen
        float lastx = 0;
        for (int ti = 0; ti < (float)hint_stack.size(); ti++) { 
            SDL_Surface* textSurface = TTF_RenderText_Blended(font,hint_stack[ti].c_str(),0, color);
        
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
            SDL_DestroySurface(textSurface);
            
            SDL_FRect dstRect = {lastx,(float)(WINDOW_HEIGHT-textTexture->h),(float)textTexture->w,(float)textTexture->h};
            lastx += 10 + textTexture->w;    //Accumulate widths + margin
            dstRect.w += 10; //Push
            SDL_RenderTexture(sdl_renderer, textures->hintbg, nullptr, &dstRect); //FONT
            dstRect.w -= 10; //Pop
            SDL_RenderTexture(sdl_renderer, textTexture, nullptr, &dstRect); //FONT
        }
        

        //Final
        


        SDL_RenderPresent(sdl_renderer);

        SDL_Delay(4); //dont remove this its black magic
    }

    //Termination
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    TTF_Quit(); SDL_Quit();
    free(textures);


    return 0;
}
