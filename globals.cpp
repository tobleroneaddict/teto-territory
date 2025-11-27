#include "globals.h"


float RUN_SPEED = 0.5f;
float CAR_SPEED = 1.5f;

int* gFrameBuffer = nullptr;
SDL_Window* sdl_window = nullptr;
SDL_Renderer* sdl_renderer = nullptr;
SDL_Texture* sdl_texture = nullptr;
Game_Textures* textures = nullptr;
int WINDOW_WIDTH = 1300;
int WINDOW_HEIGHT = 900;
int MAX_WORLD_X = 3000;
int MAX_WORLD_Y = 3000;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
float get_distance(float x1, float y1, float x2, float y2) {
    float dx = (x1 - x2);
    float dy = (y1 - y2);

    float ddddd = sqrtf(dx*dx+dy*dy);
    return ddddd;
}

                //p.x      //p.y  passed in
void Car::render(int xoff, int yoff) {
    rect.x = x - xoff + WINDOW_WIDTH/2 - rect.w/2 ;
    rect.y = y - yoff + WINDOW_HEIGHT/2 - rect.h/2 ;
    
    //rect.x = x - xoff + rect.w/2 + 100; //draw in center
    //rect.y = y - yoff + rect.h/2 + 150;
    if (flip) {
        SDL_RenderTextureRotated(sdl_renderer,textures->car,nullptr,&rect,0,nullptr,SDL_FLIP_HORIZONTAL);
    } else {
    SDL_RenderTexture(sdl_renderer,textures->car,nullptr,&rect); //Draw car regardless
    }
}

void World_C::renderLayer(float player_x, float player_y,int layer,TMX* tiles) {
    int scale = 64;
    tiler.h = scale;
    tiler.w = scale;
    SDL_FRect srcrect;

    //TEXTURE BLEED FIX PART A
    srcrect.h = scale - 1.0f;
    srcrect.w = scale - 1.0f;

    //cout << teto.x << endl;
    for (int y = 0; y < WINDOW_HEIGHT+scale; y += scale) {
            
        for (int x = 0; x < WINDOW_WIDTH+scale; x += scale ) {
            
            
            //Coarse scroll
            float coarsex = floor(player_x/scale) - 10;
            float coarsey = floor(player_y/scale) - 7;
            
            //Fine scroll
            float finex = fmod(player_x,scale);
            float finey = fmod(player_y,scale);

            //helps fix weird jitter, but 0,0 still shows at player x -580 or so.
            //actually 10 tiles up and left
            //so lets try too..... move coarsex??
            //seems to have worked.

            if (finex < 0) finex+= scale;
            if (finey < 0) finey+= scale;
            
            //write this tile's position on screen
            tiler.x = x - finex;
            tiler.y = y - finey;
            

            //Get the tile atlas cutout for this tile
            int atlasx,atlasy;
            int tileID = tiles->get(
                abs(floor(coarsex+x / scale)),
                abs(floor(coarsey+y / scale)),
            layer);
            tiles->coordinate(tileID,atlasx,atlasy);
            //Ro
            //TEXTURE BLEED FIX PART B
            srcrect.x = atlasx + 0.5f;
            srcrect.y = atlasy + 0.5f;
            
            SDL_RenderTexture(sdl_renderer,textures->tile_atlas,&srcrect,&tiler);

        }
    }
}