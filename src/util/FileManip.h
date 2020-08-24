/*\
 *
 * helper functions that handle file IO
 * TODO: IO for object files
 * 
\*/
#pragma once
#include "structs.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

namespace DGREngine::util::io{
    using namespace structs;
    std::string getFileText(const char path[]);
    TextureData loadImage(const char path[]);
    void FreeTextureData(byte *data);
}