#pragma once
#include <string>
#include "util/typealias.h"

//data about a texture being loaded
struct TextureData{
    int width, height, component_count;
    byte* data;
};

//data about a texture that is in use by the program
struct Texture
{
    uint handle;
    std::string filepath;
    std::string type;
};