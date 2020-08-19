#include "FileManip.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace DGREngine::util::io{
using namespace structs;
    extern std::string getFileText(const char path[])
    {
        std::string text;
        std::ifstream file_s(path, std::ios::in);
        if (file_s.is_open())
        {
            std::stringstream sstr;
            sstr << file_s.rdbuf();
            file_s.close();
            return sstr.str();
        }
        else
        {
            printf("cant read file");
            return "";
        }
    }

    extern TextureData *loadImage(const char path[]){
        TextureData* t = new TextureData;
        t->data = stbi_load(path, &(t->width), &(t->height), &(t->numChannels), 0);
        return t;
    }
}