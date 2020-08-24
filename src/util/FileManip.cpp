#include "FileManip.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace DGREngine::util::io{
using namespace structs;
    std::string getFileText(const char path[])
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

    TextureData loadImage(const char path[]){
        TextureData t{};
        stbi_set_flip_vertically_on_load(1);
        t.data = stbi_load(path, &(t.width), &(t.height), &(t.channels), 4);
        return t;
    }

    void FreeTextureData(byte* data){
        stbi_image_free(data);
    }
}