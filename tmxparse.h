#pragma once
#include "globals.h"
#include <tinyxml2.h>

//Got mad that two of my tmx parsers DIDNT WORK
//even tinier tmx parser

struct TMX_TILE { 
    int id;
}; //we only need an ID

struct TMX_LAYER {
    int id;
    std::vector<int> data;
};

struct TMX_MAP {
    int width;
    int height;
    int tilewidth;
    int tileheight;
    std::vector<TMX_LAYER> layers;
};

class TMX {
public:
    TMX_MAP* map;
    bool load(const std::string &filename);
    
    //get id at x,y, layer ID
    int get(int x, int y, int layer = 1);
};