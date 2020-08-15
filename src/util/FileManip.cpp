#include "FileManip.h"

namespace DGREngine::util::io{

    extern const char* getFileText(const char path[]){
        std::ifstream f(path);
        std::vector<char> o( (std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>() );
        return &o[0];
    }

}