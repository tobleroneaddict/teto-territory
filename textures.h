#pragma once
#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "include/PerlinNoise/PerlinNoise.hpp"
#include "include/SDL_image.h"
#include "vector"
#include <SDL3/SDL_time.h>
#include <sstream>


//All the textures in the game
class Game_Textures {
    public:
        SDL_Texture* teto_texture_alt = nullptr;
        SDL_Texture* teto_texture_r = nullptr;
        SDL_Texture* teto_texture_fold = nullptr;
        SDL_Texture* enemy_texture = nullptr;
        SDL_Texture* car = nullptr;
    
        SDL_Texture* tiling_texture = nullptr;
        SDL_Texture* sand_texture = nullptr;
    
        SDL_Texture* gun_texture = nullptr;
        SDL_Texture* ch = nullptr;
        SDL_Texture* bullet = nullptr;
    
        //Objects
        SDL_Texture* flag = nullptr;
        SDL_Texture* bomb = nullptr;
        SDL_Texture* expedient = nullptr;
        SDL_Texture* big_steppah = nullptr;
        SDL_Texture* apple = nullptr;
        SDL_Texture* bricks = nullptr;
        SDL_Texture* agent = nullptr;
        SDL_Texture* capsule = nullptr;
    
        SDL_Texture* phone = nullptr;
        SDL_Texture* card_atlas = nullptr;
        SDL_Texture* hitstand = nullptr;
        SDL_Texture* playbutton = nullptr;
        
        SDL_Texture* rocket = nullptr;
    
        bool load(SDL_Renderer* renderer) {
            teto_texture_alt = IMG_LoadTexture(renderer, "Assets/alt-player.png");
            teto_texture_r = IMG_LoadTexture(renderer, "Assets/teto-right.png");
            teto_texture_fold = IMG_LoadTexture(renderer, "Assets/teto-lean.png");
            enemy_texture = IMG_LoadTexture(renderer, "Assets/teto-plushie.png");
            car = IMG_LoadTexture(renderer, "Assets/car.png");
    
            tiling_texture = IMG_LoadTexture(renderer, "Assets/tile.png");
            sand_texture = IMG_LoadTexture(renderer, "Assets/sand.png");
            gun_texture = IMG_LoadTexture(renderer, "Assets/ak-47.png");
            ch = IMG_LoadTexture(renderer, "Assets/crosshair.png");
            bullet = IMG_LoadTexture(renderer, "Assets/teto-left.png");
    
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
            playbutton = IMG_LoadTexture(renderer, "Assets/UI/playbutton.png");
    

            return true;
        }
    };