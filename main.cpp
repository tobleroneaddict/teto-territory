#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
//#include "include/PerlinNoise/PerlinNoise.hpp"
#include "include/SDL_image.h"
#include "vector"

#include <SDL3/SDL_time.h>
#include <sstream>

//Game Mechanics
#include "globals.h"
#include "blackjack_engine.h"
#include "phone.h"

//World of all da stuff
World_C world;

bool stop = false; //stop sdl

SDL_Color font_color = {255, 255, 255, 255};

float mx,my;

using namespace std;



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

SDL_FRect tiler; //Tile texturer

SDL_FRect gun_rect;

SDL_FRect ch_rect;

SDL_FRect bullet_rect;

class Teto_C {
public:
    float x,y,xv,yv, gun_out_x,gun_out_y;
    float max_speed = 0.5f;

    int ammo;
    float bullet_cooldown;
    
    //Inventory
    bool holding_weapon = false;
    
    World_C *world;
    bool drunk = false;
    float drunk_timer = 0;
    
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
        //Input vectors
        float inx = 0, iny = 0; float input_scale = 0.01; 
        iny -= (keys[SDL_SCANCODE_W])    * input_scale  * deltaTime;
        iny -= -(keys[SDL_SCANCODE_S])   * input_scale  * deltaTime;
        inx += -(keys[SDL_SCANCODE_A])   * input_scale  * deltaTime;
        inx += (keys[SDL_SCANCODE_D])    * input_scale  * deltaTime;        

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
        xv *= 0.9;
        yv *= 0.9;
        
        

        //Apply accel
        x += xv * deltaTime;
        y += yv * deltaTime;

        //bullet cooldown
        if (bullet_cooldown < 0) { bullet_cooldown = 0;} //if below zero, set to zero
        else {bullet_cooldown -= deltaTime; } //otherwise, decrease

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
    //Depending on the direction teto is facing we have to account for where the muzzle(?) of the gun is, to get proper aiming.
    
    float dx = mx - (WINDOW_WIDTH/2) - ofx;
    float dy = my - (WINDOW_HEIGHT/2) - ofy;
    
    float ebullet_mouse_dist_to_norm = sqrt(dx*dx+dy*dy);

    float nx = dx / ebullet_mouse_dist_to_norm;
    float ny = dy / ebullet_mouse_dist_to_norm;
    
    ebullet.vx = nx * 10;
    ebullet.vy = ny * 10;
    
    

    //Create bullet
    world->bullets.push_back(ebullet);
}


int main() {
    gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
    sdl_window = SDL_CreateWindow("Teto Territory", WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_RESIZABLE);
    
    sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
    sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureScaleMode(sdl_texture,SDL_SCALEMODE_NEAREST);
    textures = new Game_Textures;
    

    if (!gFrameBuffer || !sdl_window || !sdl_renderer || !sdl_texture) 
        return 1;


    //Shitty way of getting rand seed
    srand(std::uintptr_t(&sdl_texture));

    //cout << reinterpret_cast<unsigned int>(&sdl_window) << endl;
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
    

    teto.x = 1200;
    teto.y = 1200;
    

    food.h = 80;
    food.w = 80;

    

    tiler.h = 256;
    tiler.w = 256;

    gun_rect.h = 80;
    gun_rect.w = 180;

    teto.bullet_cooldown = 0;
    
    //dont depend on teto position until while
    
    
    ch_rect.h = textures->ch->h;
    ch_rect.w = textures->ch->w;
    
    
    bullet_rect.h = 32;
    bullet_rect.w = 32;


    SDL_Delay(100); //help stuff not die

    teto.teto_rendering = true;
    //Spawn enemies
    for (int i = 0; i < 500; i++) {
        Enemy enemeeee;
        enemeeee.rendering = true;
        enemeeee.id = rand();
        enemeeee.x  = rand()  % 5000;
        enemeeee.y  = rand()  % 5000;
        world.enemies.push_back(enemeeee);
    }

    SDL_HideCursor();

    //Spawn test obj
    // Item lilitem;               lilitem.texture = textures->capsule;
    // lilitem.x = 300;
    // lilitem.y = 300;
    // lilitem.scale = 0.2f;
    // lilitem.id = 9;
    // world.dropped_items.push_back(lilitem);
    
    
    

    while (!stop)
    {
        SDL_GetWindowSizeInPixels(sdl_window,&WINDOW_WIDTH,&WINDOW_HEIGHT);

        teto.player_rect.x = (WINDOW_WIDTH/2)-40;
        teto.player_rect.y = (WINDOW_HEIGHT/2)-100;

        gun_rect.x = teto.player_rect.x - 100;
        gun_rect.y = teto.player_rect.y + 30;
        
        
        SDL_GetMouseState(&mx,&my);


        //dTs
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
   
        //Run teto user motion 
        teto.run_motion();


        deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );

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
            //Shoot bullet
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {

                //Phone open
                if (mx < WINDOW_WIDTH - 67 && mx > WINDOW_WIDTH - 400 && my > phone.phone_rect.y && my < phone.phone_rect.y + 115) {
                    phone.active = !phone.active;
                    
                } else if (mx < WINDOW_WIDTH - 67 && mx > WINDOW_WIDTH - 400 && my > phone.phone_rect.y + 115)  {
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

                }
                else {
                    //Gun    
                    if (teto.bullet_cooldown < 10 && teto.holding_weapon) teto.fire_weapon();
                }

                

                

            }

            //Add flag on E
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_1) {

                Item lilitem;               lilitem.texture = textures->flag;
                lilitem.x = teto.x - 160;
                lilitem.y = teto.y - 300;
                lilitem.scale = 1.0f; lilitem.id = 1;

                world.dropped_items.push_back(lilitem);
            }

            //Bomb
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_2) {

                Item lilitem;               lilitem.texture = textures->bomb;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 1.0f; lilitem.id = 2;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_3) {

                Item lilitem;               lilitem.texture = textures->big_steppah;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 0.2f; lilitem.id = 3;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_4) {

                Item lilitem;               lilitem.texture = textures->apple;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 0.2f;   lilitem.id = 4;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_5) {

                Item lilitem;               lilitem.texture = textures->expedient;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 0.2f;   lilitem.id = 5;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_6) {

                Item lilitem;               lilitem.texture = textures->bricks;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 0.4f;   lilitem.id = 6;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_7) {

                Item lilitem;               lilitem.texture = textures->gun_texture;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 0.08f;  lilitem.id = 7;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_8) {

                Item lilitem;               lilitem.texture = textures->agent;
                lilitem.x = teto.x - 50;
                lilitem.y = teto.y - 50;
                lilitem.scale = 0.3f;   lilitem.id = 8;
                world.dropped_items.push_back(lilitem);
            }
            if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_G) { teto.holding_weapon = !teto.holding_weapon; }


        }
        

        SDL_RenderClear(sdl_renderer);

        int scale = 256;
        tiler.h = scale;
        tiler.w = scale;

        for (int y = -scale; y < WINDOW_HEIGHT+scale; y += scale) {
                
            for (int x = -scale; x < WINDOW_WIDTH+scale; x += scale ) {
                
                float offx = fmod(-teto.x, (float)scale);
                if (offx < 0) offx += (float)scale;

                float offy = fmod(-teto.y, (float)scale);
                if (offy < 0) offy += (float)scale;
                
                tiler.x = offx + x;
                tiler.y = offy + y;

                SDL_RenderTexture(sdl_renderer,textures->tiling_texture,nullptr,&tiler);

            }
        }

        //Render play field
        SDL_RenderTexture(sdl_renderer,sdl_texture,nullptr,nullptr);


        //Render enemies, run enemy code
        for (int i = 0; i < (int)world.enemies.size(); i++) {

            Enemy* curr = &world.enemies[i];

            //Enemy velocity
            //Drag factor
            curr->xv *= 0.9;
            curr->yv *= 0.9;
            //Apply accel
            curr->x += curr->xv * deltaTime;
            curr->y += curr->yv * deltaTime;

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

            food.x = curr->x - teto.x + WINDOW_WIDTH/2 - 25;
            food.y = curr->y - teto.y + WINDOW_HEIGHT/2 - 25;
            

            if (teto.holding_weapon && scare_factor < 300) {
                food.x += randX * (300-scare_factor) * .06;
                food.y += randY * (300-scare_factor) * .03;
            }

            if (curr->rendering) {
                SDL_RenderTexture(sdl_renderer,textures->enemy_texture,nullptr,&food);
            }
        }

        
        SDL_FRect thingrect;

        //Render items
        for (int i = 0; i < (int)world.dropped_items.size(); i++) {
            Item* thing = &world.dropped_items[i];

            thingrect.x = thing->x - teto.x + WINDOW_WIDTH/2 - 25;
            thingrect.y = thing->y - teto.y + WINDOW_HEIGHT/2 - 25;

            thingrect.h = thing->texture->h * thing->scale;
            thingrect.w = thing->texture->w * thing->scale;

            

            //Drunk
            if (thing->id == 9) {
                //Hit test with player
                float meow = abs(teto.x - thing->x);    float purr = (teto.y - thing->y);
                float distance = sqrtf(meow*meow+purr*purr);
                if (distance < 50) { teto.drunk_timer = 10000; world.dropped_items.erase(world.dropped_items.begin() + i); i--; } //fent for 10 sec  
            }

            if (thing->id == -1) cout << "u fucked up item ID as !" << thing->id << endl;
            SDL_RenderTexture(sdl_renderer,thing->texture,nullptr,&thingrect);

        }


        //Render player (on top)
        if (teto.teto_rendering) {
            if (!teto.drunk) {
                if (teto.xv >= 0) {
                    SDL_RenderTexture(sdl_renderer,textures->teto_texture_r,nullptr,&teto.player_rect);
                }  else {
                    SDL_RenderTexture(sdl_renderer,textures->teto_texture_l,nullptr,&teto.player_rect);
                }
            } else {
                SDL_RenderTexture(sdl_renderer,textures->teto_texture_fold,nullptr,&teto.player_rect);
            }
        }

        
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
        if (teto.holding_weapon && !teto.drunk)    SDL_RenderTexture(sdl_renderer,textures->gun_texture,nullptr,&gun_rect);


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



        //PHONE RENDERING
        SDL_RenderTexture(sdl_renderer,textures->phone,nullptr,&phone.phone_rect);

        //Blackjack ,this will probably have a bug where it shows the bj during the animation
        if (phone.active) {
            phone.play_rainbet();
            
        }

        //Encodes bet into rainbet before locking in
        if (!blackjack->bet_locked_in) blackjack->bet = (int)teto.x % blackjack->balance;

        phone.update_phone();

        
        //crosshair
        ch_rect.x = mx - ch_rect.w + textures->ch->w/2;
        ch_rect.y = my - ch_rect.h + textures->ch->h/2;
        if ( teto.bullet_cooldown < 10) SDL_RenderTexture(sdl_renderer,textures->ch,nullptr,&ch_rect);


        SDL_RenderPresent(sdl_renderer);
        //SDL_GL_SwapWindow(sdl_window);

        SDL_Delay(4); //dont remove this its black magic
    }
    

    return 0;
}