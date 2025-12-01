#include "../include/legacyitemplacementcode.h"
void legacy_placement_code(World_C* world, float x, float y, SDL_Event e) {
    //FLAG
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_1) {

        Drop lilitem;               lilitem.texture = textures->flag;
        lilitem.x = x - 160;
        lilitem.y = y - 300;
        lilitem.scale = 1.0f; lilitem.id = 1;

        world->dropped_Drops.push_back(lilitem);
    }
    //Bomb
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_2) {

        Drop lilitem;               lilitem.texture = textures->bomb;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 1.0f; lilitem.id = 2;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_3) {

        Drop lilitem;               lilitem.texture = textures->big_steppah;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 0.2f; lilitem.id = 3;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_4) {

        Drop lilitem;               lilitem.texture = textures->apple;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 0.2f;   lilitem.id = 4;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_5) {

        Drop lilitem;               lilitem.texture = textures->expedient;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 0.2f;   lilitem.id = 5;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_6) {

        Drop lilitem;               lilitem.texture = textures->bricks;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 0.4f;   lilitem.id = 6;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_7) {

        Drop lilitem;               lilitem.texture = textures->gun_texture;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 0.08f;  lilitem.id = 7;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_8) {

        Drop lilitem;               lilitem.texture = textures->agent;
        lilitem.x = x - 50;
        lilitem.y = y - 50;
        lilitem.scale = 0.3f;   lilitem.id = 8;
        world->dropped_Drops.push_back(lilitem);
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_9) {

        Drop lilitem;               lilitem.texture = textures->capsule;
        lilitem.x = x - 50;
        lilitem.y = y + 500;
        lilitem.scale = 0.3f;   lilitem.id = 9;
        world->dropped_Drops.push_back(lilitem);
    }
    return;
}