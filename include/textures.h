#pragma once
#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL_image.h"
#include "vector"

//All the textures in the game
class Game_Textures {
    public:
        //Tiler
        SDL_Texture* tile_atlas = nullptr;

        //Player, world
        SDL_Texture* teto_texture_alt = nullptr;
        SDL_Texture* teto_texture_r = nullptr;
        SDL_Texture* teto_texture_fold = nullptr;
        SDL_Texture* enemy_texture = nullptr;
        SDL_Texture* car = nullptr;
        SDL_Texture* horse = nullptr;
        SDL_Texture* tiling_texture = nullptr;
        SDL_Texture* sand_texture = nullptr;
        SDL_Texture* gun_texture = nullptr;
        SDL_Texture* ch = nullptr;
        SDL_Texture* bullet = nullptr;
        SDL_Texture* rocket = nullptr;
        SDL_Texture* capsule = nullptr; //Beer

        //Legacy Objects
        SDL_Texture* flag = nullptr;
        SDL_Texture* bomb = nullptr;
        SDL_Texture* expedient = nullptr;
        SDL_Texture* big_steppah = nullptr;
        SDL_Texture* apple = nullptr;
        SDL_Texture* bricks = nullptr;
        SDL_Texture* agent = nullptr;
       
        //UI Elements
        SDL_Texture* phone = nullptr;
        SDL_Texture* card_atlas = nullptr;
        SDL_Texture* hitstand = nullptr;
        SDL_Texture* newgame = nullptr;
        SDL_Texture* playbutton = nullptr;
        SDL_Texture* block_highlight = nullptr;
        SDL_Texture* block_highlight_arrow = nullptr;
        SDL_Texture* BG = nullptr;
        SDL_Texture* FG = nullptr;
        SDL_Texture* vingette = nullptr;
        SDL_Texture* hintbg = nullptr;

        //Cooking
        SDL_Texture* cooking_barrel = nullptr;

        //Machines
        SDL_Texture* machine_basic = nullptr;


        

        bool load(SDL_Renderer* renderer) {
            tile_atlas = IMG_LoadTexture(renderer, "Assets/atlas.png");
            teto_texture_alt = IMG_LoadTexture(renderer, "Assets/alt-player.png");
            teto_texture_r = IMG_LoadTexture(renderer, "Assets/teto-right.png");
            teto_texture_fold = IMG_LoadTexture(renderer, "Assets/teto-lean.png");
            enemy_texture = IMG_LoadTexture(renderer, "Assets/teto-plushie.png");
            car = IMG_LoadTexture(renderer, "Assets/car.png");
            horse = IMG_LoadTexture(renderer, "Assets/horse.png");

            tiling_texture = IMG_LoadTexture(renderer, "Assets/tile.png");
            sand_texture = IMG_LoadTexture(renderer, "Assets/sand.png");
            gun_texture = IMG_LoadTexture(renderer, "Assets/ak-47.png");
            ch = IMG_LoadTexture(renderer, "Assets/crosshair.png");
            bullet = IMG_LoadTexture(renderer, "Assets/capsule.png");

            flag = IMG_LoadTexture(renderer, "Assets/flag.png");
            bomb = IMG_LoadTexture(renderer, "Assets/bomb.png");
            expedient = IMG_LoadTexture(renderer, "Assets/firearms.png");
            big_steppah = IMG_LoadTexture(renderer, "Assets/big-steppah.png");
            apple = IMG_LoadTexture(renderer, "Assets/apple.png");
            bricks = IMG_LoadTexture(renderer, "Assets/bricks.png");
            agent = IMG_LoadTexture(renderer, "Assets/muffin-clipart-xl.png");
            rocket = IMG_LoadTexture(renderer, "Assets/rocket.png");


            capsule = IMG_LoadTexture(renderer, "Assets/capsule.png");

            phone = IMG_LoadTexture(renderer, "Assets/UI/android.png");

            card_atlas = IMG_LoadTexture(renderer, "Assets/UI/cards.png");
            hitstand = IMG_LoadTexture(renderer, "Assets/UI/hitstand.png");
            newgame = IMG_LoadTexture(renderer, "Assets/UI/newgame.png");
            playbutton = IMG_LoadTexture(renderer, "Assets/UI/playbutton.png");
            block_highlight = IMG_LoadTexture(renderer, "Assets/UI/highlight.png");
            block_highlight_arrow = IMG_LoadTexture(renderer, "Assets/UI/selector-arrow.png");

            BG = IMG_LoadTexture(renderer, "Assets/UI/BG.png");
            FG = IMG_LoadTexture(renderer, "Assets/UI/FG.png");
            vingette = IMG_LoadTexture(renderer, "Assets/UI/vingette.png");
            hintbg = IMG_LoadTexture(renderer, "Assets/UI/hintbg.png");

            cooking_barrel = IMG_LoadTexture(renderer, "Assets/Cooking/barrel.png");

            machine_basic = IMG_LoadTexture(renderer, "Assets/Cooking/machine_basic.png");

            return true;
        }
    };
