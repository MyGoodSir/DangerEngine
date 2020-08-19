#pragma once
#include "structs.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

namespace DGREngine::util::io{
    using namespace structs;
    extern std::string getFileText(const char path[]);
    extern TextureData* loadImage(const char path[]);
}