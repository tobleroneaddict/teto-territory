#include "phone.h"

void Phone_C::update_phone() {
    phone_rect.x = WINDOW_WIDTH - phone_rect.w;

    if (active) {
        target_y = WINDOW_HEIGHT - phone_rect.h;
    } else {
        target_y = WINDOW_HEIGHT - 100;
    }

    phone_rect.y = lerp(phone_rect.y, target_y, 0.1f);
    //cout << phone_rect.y << endl;
}
void Phone_C::play_rainbet() {
    SDL_FRect srcRect;
    srcRect.y = 0;
    srcRect.w = 55;
    srcRect.h = 76;

    card_rect.h = 77;
    card_rect.w = 55;

    card_rect.y = 50;

    if (blackjack->bet_locked_in) {
        //dealer cards
        for (int i = blackjack->dealer.cards.size() - 1; i >= 0; i--) {
            bj_card* card = &blackjack->dealer.cards[i];

            srcRect.x = card->get_atlas_x();
            card_rect.y = phone_rect.y + 280 + (i*2);
            //card_rect.x = WINDOW_WIDTH - 355 + (i*60); //offset for dealer card start
            card_rect.x = WINDOW_WIDTH - 355 + (i*40); //offset for dealer card start, with overlay
            if (!card->facing_up) srcRect.x = 55*13 - 3;
            SDL_RenderTexture(sdl_renderer,textures->card_atlas,&srcRect,&card_rect);


            //Debug
            // std::stringstream stream;
            // stream << "" << card->value;
            // std::string text2 = stream.str();
            // SDL_RenderDebugText(sdl_renderer, card_rect.x, phone.phone_rect.y -100, text2.c_str());

        }
        //player cards
        for (int i = blackjack->player.cards.size() -1; i >= 0; i--) {
            bj_card* card = &blackjack->player.cards[i];

            srcRect.x = card->get_atlas_x();
            card_rect.y = phone_rect.y + 400 + (i*2);
            card_rect.x = WINDOW_WIDTH - 355 + (i*40); //offset for dealer card start

            SDL_RenderTexture(sdl_renderer,textures->card_atlas,&srcRect,&card_rect);
        }
    }

    //Render text

    //render buttons
    rainbet_hs.y = phone_rect.y + 500;
    rainbet_hs.x = WINDOW_WIDTH - 365;
    
    if (blackjack->bet_locked_in) { //round done! new game
        if (blackjack->won || blackjack->bust || blackjack->dealer_won) {
            SDL_RenderTexture(sdl_renderer,textures->newgame,nullptr,&rainbet_hs);
        } else { //ingame
            SDL_RenderTexture(sdl_renderer,textures->hitstand,nullptr,&rainbet_hs);
        }
    } else { //betting sreen
        SDL_RenderTexture(sdl_renderer,textures->playbutton,nullptr,&rainbet_hs);

    }

    //SDL_SetRenderScale(sdl_renderer, 4.0f, 4.0f);
    SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    std::stringstream ss;
    ss << "Bal: $" << blackjack->balance;
    std::string text = ss.str();
    SDL_RenderDebugText(sdl_renderer, WINDOW_WIDTH - 290, phone_rect.y + 175, text.c_str());

    std::stringstream ss2;

    ss2 << "Bet: (Your x pos) $" << blackjack->bet;
    std::string text2 = ss2.str();
    SDL_RenderDebugText(sdl_renderer, WINDOW_WIDTH - 290, phone_rect.y + 550, text2.c_str());

    //Print status

    if (blackjack->won || blackjack->bust || blackjack->dealer_won || blackjack->tie) {
        //Ideally only one of these should happen!
        std::stringstream ss2; //reinit
        if (blackjack->won) ss2 << "Won\n";
        if (blackjack->bust) ss2 << "Bust\n";
        if (blackjack->dealer_won) ss2 << "Dealer won\n";
        if (blackjack->tie) ss2 << "Tie\n";

        text2 = ss2.str();
        SDL_RenderDebugText(sdl_renderer, WINDOW_WIDTH - 290, phone_rect.y + 580, text2.c_str());
    }

}
