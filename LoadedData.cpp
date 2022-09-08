#include "LoadedData.hpp"

#include "load_save_png.hpp"

#include <vector>


LoadedData::TileData *LoadedData::load_sprite(std::string filename) {
    glm::uvec2 size;
    std::vector< glm::u8vec4 > data;
    
    load_png(filename, &size, &data, LowerLeftOrigin);

    TileData *tileData = new TileData();

    glm::u8vec4 color;   
    uint8_t num_colors = 0;
    bool color_found = false;

    for (unsigned int i = 0; i < size.x; i++) {
        for (unsigned int j = 0; j < size.y; j++) {
            color = data.at((j * size.y) + i);

            color_found = false;
            for (uint8_t c = 0; c < num_colors; c++) {
                if (tileData->palette.at(c) == color) {
                    color_found = true;
                    tileData->tile.bit1[j] = (tileData->tile.bit1[j] << 1) | ((c & 0b10) >> 1);
                    tileData->tile.bit0[j] = (tileData->tile.bit0[j] << 1) | (c & 1);
                }
            }
            if (color_found == false && num_colors < 4) {
                tileData->palette[num_colors] = color;
                tileData->tile.bit1[j] = (tileData->tile.bit1[j] << 1) | ((num_colors & 0b10) >> 1);
                tileData->tile.bit0[j] = (tileData->tile.bit0[j] << 1) | (num_colors & 1);
                num_colors++;
            }
            
        }
    }

    return tileData;
}