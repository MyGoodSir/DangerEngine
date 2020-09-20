#include "FileManip.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string FileIOManager::getFileText(const char path[])
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

TextureData FileIOManager::loadTexture(const char path[]){
    TextureData td{};
    td.data = stbi_load(path, &td.width, &td.height, &td.component_count, 0);
    return td;
}
void FileIOManager::freeTexture(TextureData &td){
    stbi_image_free(td.data);

}