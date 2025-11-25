#pragma once
#include "globals.h"
#include "blackjack_engine.h"

class Phone_C {
    public:
        SDL_FRect phone_rect;
        SDL_FRect card_rect;
        SDL_FRect rainbet_hs;
        float target_y = WINDOW_HEIGHT;
        bool clickable = false;
        bool active = false; //is up in face
        //Blackjack engine
        blackjack_engine* blackjack;
        void update_phone();
        void play_rainbet();
};