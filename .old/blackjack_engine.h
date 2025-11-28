#pragma once
#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "include/SDL_image.h"
#include "vector"

class bj_card {
public:
    enum Rank { TWO=2, THREE=3, FOUR=4, FIVE=5, SIX=6, SEVEN=7, EIGHT=8, NINE=9, TEN=10, JACK=10, QUEEN=10, KING=10, ACE=1};
    int value; //Ace is 10 or 1
    bool facing_up; //false is down, true is up
    int get_atlas_x();
    //numbers = face value, JQK are 10.
    //Ace is 10 or 1.
};

class bj_player {
public:
    std::vector<bj_card> cards;

    bool dealer = false;

    void hit();
    void stand();
    void bet(int amount);
    int calculate();
};

class blackjack_engine {
public:
    bool won = false;
    bool dealer_won = false;
    //dealer bust is player won
    bool bust = false;
    bool tie = false;
    bool bet_locked_in = false;

    int bet = 50;
    int balance = 500;

    bj_player dealer;
    bj_player player;


    void init();

    bj_card random_card(bool facing_up);

    void initial_deal();

    void hit();
    void stand();
    void start_game();
    //ONCE DEALER HAS GONE AROUND TABLE: FLIP UP FD CARD
    //16 or under, have to take another card
    //17 or higher stay with hand

};
