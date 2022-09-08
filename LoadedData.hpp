#pragma once

#include "PPU466.hpp"
#include <glm/glm.hpp>


struct LoadedData {

    struct TileData {
        PPU466::Palette palette;
        PPU466::Tile tile;
    };

    TileData *load_sprite(std::string filename);

};