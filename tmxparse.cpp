#include "tmxparse.h"
#include <iostream>

bool TMX::load(const std::string &filename) {
    tinyxml2::XMLDocument doc;
    map = new TMX_MAP();

    bool error = doc.LoadFile(filename.c_str());
    if (error != tinyxml2::XML_SUCCESS) {return false;} //no workie

    tinyxml2::XMLElement* zemap = doc.FirstChildElement("map");
    if (!zemap) return false;
    
    //Put attrib
    zemap->QueryIntAttribute("width",&map->width);
    zemap->QueryIntAttribute("height",&map->height);
    zemap->QueryIntAttribute("tilewidth",&map->tilewidth);
    zemap->QueryIntAttribute("tileheight",&map->tileheight);

    //put layers in and data, looping thru layers of type layer
    for(tinyxml2::XMLElement* layerElem = zemap->FirstChildElement("layer"); layerElem; layerElem = layerElem->NextSiblingElement("layer")) 
    {
        TMX_LAYER layer;
        layerElem->QueryIntAttribute("id",&layer.id); //set ID to whats made in Tiler

        //get data
        tinyxml2::XMLElement* dataElem = layerElem->FirstChildElement("data");
        if (!dataElem) continue; //skip missing data

        const char* csv = dataElem->GetText();
        if (!csv) continue; //skip missing data

        //fat rip into data vector bongwater
        std::stringstream ss(csv);
        std::string toke;
        while (std::getline(ss, toke, ',')) {
            if (!toke.empty()) {
                layer.data.push_back(std::stoi(toke));
            }
        }

        //put layer in our shit
        map->layers.emplace_back(layer);
    }
    return true;
}

//get tile id from xy [0]
int TMX::get(int x, int y,int layer) {
    TMX_LAYER& curr = map->layers[layer];
    int off = y*map->width + x;
    return curr.data[off];

}

//get atlas xy from id
void TMX::coordinate(int tileID, int &x, int &y) {
    int tx = tileID % 10; //tile x
    int ty = floor(tileID / 10);
    x = tx * map->tilewidth;
    y = ty * map->tileheight;
    
    return;
}