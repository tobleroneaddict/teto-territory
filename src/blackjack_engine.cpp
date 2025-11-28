#include <math.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../include/SDL_image.h"
#include "vector"

#include "../include/blackjack_engine.h"

void bj_player::hit() {

}
void bj_player::stand() {

}

int bj_player::calculate() {
    int accumulator = 0; //hehe is cpu
    int num_aces = 0; //theres how many aces in this room?

    for (int i =0; i < (int)cards.size(); i++) {
        bj_card* card = &cards[i];

        accumulator += card->value;

        if (card->value == bj_card::ACE) num_aces++;
    }

    //Pop out aces until possible bust
    while (num_aces > 0 && accumulator + 10 <= 21) {
        accumulator += 10;
        num_aces--;
    }

    return accumulator;
}


void blackjack_engine::init() {
    std::cout << "bj_engine_start\n";

}

bj_card blackjack_engine::random_card(bool facing_up) {
    bj_card card;
    card.facing_up = facing_up;
    card.value = static_cast<bj_card::Rank>(std::rand() % 13);
    if (card.value == 0) card.value = 4; //rigging :P
    //card.value = std::rand() % 11;
    return card;
}

//for texture atlas
int bj_card::get_atlas_x() {
    //55x74 card
    //thsi is bugged but idc, all values of 10 are just 10
    return (value-1) * 55;

}

//called in init().
void blackjack_engine::initial_deal() {
    //do not move this to init it somehow calls SIGABRT
    dealer.cards.clear();
    player.cards.clear();
    won = tie = bust = dealer_won = false;

    //std::cout << bet << std::endl;
    balance -= bet;
    bet_locked_in = true;
    //std::cout << balance << std::endl;

    dealer.cards.emplace_back(random_card(true));
    dealer.cards.emplace_back(random_card(false));

    player.cards.emplace_back(random_card(true));
    player.cards.emplace_back(random_card(true));

}

void blackjack_engine::hit() {
    if (won || bust || dealer_won || tie) return; //cant turn if htis happens
    if (!bet_locked_in) return;
    player.cards.emplace_back(random_card(true));

    int val = player.calculate();
    if (val == 21) won = true;
    if (val > 21) bust = true;

    //Pay earnings

    if (won) {
        balance += bet*2;
        //bet_locked_in = false;
    } else if (bust) {
        //bet_locked_in = false;
    }
}


void blackjack_engine::stand() {
    if (won || bust || dealer_won || tie) return; //cant turn if htis happens
    if (!bet_locked_in) return;
    dealer.cards.back().facing_up = true; //reveal hole card

    //Soft 17
    while (dealer.calculate() < 17) {
        dealer.cards.emplace_back(random_card(true));
    }
    int dealer_val = dealer.calculate();
    if (dealer_val > 21) won = true;    //dealer bust === player won
    else if (dealer_val > player.calculate()) {
        dealer_won = true;
    } else if (dealer_val < player.calculate()) {
        won = true; //player won!
    } else {
        tie = true;
    }

    //Pay earnings

    if (won) {
        balance += bet*2;
        //bet_locked_in = false;
    } else if (tie) {
        balance += bet;
        //bet_locked_in = false;
    } else if (bust) {
        //bet_locked_in = false;
    } else if (dealer_won) {
                    //bet_locked_in = false;
    }


}

void blackjack_engine::start_game() {
    bet_locked_in = true;
    initial_deal();
}
