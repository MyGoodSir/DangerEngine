#pragma once
#include "structures/texture.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

typedef unsigned int uint;
typedef unsigned char byte;

class FileIOManager{
    public:
    static std::string getFileText(const char path[]);
    static TextureData loadTexture(const char path[]);
    static void freeTexture(TextureData &td);
};
