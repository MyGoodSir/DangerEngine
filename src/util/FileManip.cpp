#include "FileManip.h"

namespace DGREngine::util::io{

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
}