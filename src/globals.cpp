#include "../include/globals.h"


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

//Item texture helper
void Item::set_texture(SDL_Texture* texas) {
    this->texture = texas;
    //this->rect.h = texas->h;
    //this->rect.w = texas->w; //this is silly if u have differentresolutions.
    //Items are 128x128 on screen, make sure theyre square in Krita.
    this->rect.h = 128;
    this->rect.w = 128;
    
}


//CAR RENDERING
                //p.x      //p.y  passed in
void Car::render(int xoff, int yoff) {
    rect.x = x - xoff + WINDOW_WIDTH/2 - rect.w/2 ;
    rect.y = y - yoff + WINDOW_HEIGHT/2 - rect.h/2 ;

    //First draw item in the backseat
    if (stored_item != nullptr) {
        if (stored_item->rect.h == 0) {std::cout << "Init item height dummy\n";}
        //Transform to car
        stored_item->rect.x = rect.x + rect.w / 5;
        stored_item->rect.y = rect.y + rect.h / 4;
        //if car flip
        if (flip) stored_item->rect.x = rect.x + (rect.w / 1.7f);
        
        SDL_RenderTexture(sdl_renderer,stored_item->texture,nullptr,&stored_item->rect);

    } //end backseat

    //Now draw car over item
    if (flip) {
        
        SDL_RenderTextureRotated(sdl_renderer,textures->car,nullptr,&rect,0,nullptr,SDL_FLIP_HORIZONTAL);
    } else {
    SDL_RenderTexture(sdl_renderer,textures->car,nullptr,&rect); //Draw car regardless
    }
}


//Render the block selector
void UI_Blocks::render_UI_Block(int center_block_id,bool drawfg_thing) {   //Block selector UI
    int x = 0, y = 0;
    //bg
    thisblock.h = 64+16;
    thisblock.w = 630+16;
    thisblock.x = WINDOW_WIDTH/2 - 315 - 12;
    thisblock.y = 4;
    SDL_RenderTexture(sdl_renderer,textures->sand_texture,nullptr,&thisblock);

    SDL_FRect srcrect;

    //TEXTURE BLEED FIX PART A
    srcrect.h = 64 - 1.0f;
    srcrect.w = 64 - 1.0f;
    thisblock.y = 12;
    thisblock.h = 64;
    thisblock.w = 64;



    //630 wide, 315 center local
    int offx = WINDOW_WIDTH/2 - 315;


    //9 wide
    for (int i = 0; i < 9; i++) {
        int this_id = center_block_id - 4 + i; //center is 4 higher than L - - - M - - - R
        if (this_id < 0 || this_id > 100) continue; //OOB
        tiles->coordinate(this_id,x,y); //get x,y of atlas from ID
        //TEXTURE BLEED FIX PART B
        srcrect.x = x + 0.5f;
        srcrect.y = y + 0.5f;

        thisblock.x = 70 * i + offx;

        SDL_RenderTexture(sdl_renderer,textures->tile_atlas,&srcrect,&thisblock);
    }
    //Done, now draw arrow!
    thisblock.h = textures->block_highlight_arrow->h/2;
    thisblock.w = textures->block_highlight_arrow->w/2;
    thisblock.x = WINDOW_WIDTH/2 - (thisblock.w /2);
    thisblock.y = -60;
    SDL_RenderTexture(sdl_renderer,textures->block_highlight_arrow,nullptr,&thisblock);

    //Waow! now draw the FG/BG... (FG == true)
    thisblock.x = -20;
    thisblock.y = -20;
    thisblock.h = 160;
    thisblock.w = 160;

    if (drawfg_thing) {
        SDL_RenderTexture(sdl_renderer,textures->FG,nullptr,&thisblock);
    } else {
        SDL_RenderTexture(sdl_renderer,textures->BG,nullptr,&thisblock);
    }

}

//2D MC style rendering
//Render layer x of tiles (0 is background, 1 is foreground)
void World_C::renderLayer(float player_x, float player_y,int layer,TMX* tiles) {
    int scale = 64;
    tiler.h = scale;
    tiler.w = scale;
    SDL_FRect srcrect;

    //TEXTURE BLEED FIX PART A
    srcrect.h = scale - 1.0f;
    srcrect.w = scale - 1.0f;

    //anchor screen to 0.5,0.5 and not 0 0


    float screen_anchor_x = player_x - WINDOW_WIDTH  / 2;
    float screen_anchor_y = player_y - WINDOW_HEIGHT / 2;
    
    //cout << teto.x << endl;
    for (int y = 0; y < WINDOW_HEIGHT+scale; y += scale) {

        for (int x = 0; x < WINDOW_WIDTH+scale; x += scale ) {


            //Coarse scroll
            float coarsex = floor(screen_anchor_x/scale);
            float coarsey = floor(screen_anchor_y/scale);

            //Fine scroll
            float finex = fmod(screen_anchor_x,scale);
            float finey = fmod(screen_anchor_y,scale);

            //helps fix weird jitter, but 0,0 still shows at player x -580 or so.
            //actually 10 tiles up and left
            //so lets try too..... move coarsex??
            //seems to have worked.

            if (finex < 0) finex+= scale;
            if (finey < 0) finey+= scale;

            //write this tile's position on screen
            tiler.x = x - finex;
            tiler.y = y - finey;

            //looking at tile that doesnt exist
            if (floor(coarsex+x / scale) < 0) continue;
            if (floor(coarsey+y / scale) < 0) continue;

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

void World_C::renderBlockHighlight(float player_x, float player_y, float mx, float my) {

        float screen_anchor_x = player_x - WINDOW_WIDTH  / 2;
        float screen_anchor_y = player_y - WINDOW_HEIGHT / 2;
        highlight.x = mx - highlight.w;
        highlight.y = my - highlight.h;
        float wx = mx + screen_anchor_x;
        float wy = my + screen_anchor_y;
        wx = floor(wx / 64.0f) * 64;
        wy = floor(wy / 64.0f) * 64;
        highlight.x = wx - screen_anchor_x;
        highlight.y = wy - screen_anchor_y;
        SDL_RenderTexture(sdl_renderer,textures->block_highlight,nullptr,&highlight);
}
