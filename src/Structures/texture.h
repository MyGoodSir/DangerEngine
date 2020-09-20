#pragma once
#include <string>

typedef unsigned char byte;
typedef unsigned int uint;

struct TextureData{
    int width, height, component_count;
    byte* data;
};

struct Texture
{
    uint handle;
    std::string filepath;
    std::string type;
};