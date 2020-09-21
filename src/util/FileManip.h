#pragma once
#include "structures/texture.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "typealias.h"

class FileIOManager{
    public:
    static std::string getFileText(const char path[]);
    static TextureData loadTexture(const char path[]);
    static void freeTexture(TextureData &td);
};
