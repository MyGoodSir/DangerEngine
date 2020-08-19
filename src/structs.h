#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <tuple>

namespace DGREngine::structs{
    typedef unsigned int uint;
    struct WindowAttributes
    {
        GLFWwindow* handle;
        uint width, height;
        const char *title;
    };
    struct TextureData{
        unsigned char* data;
        int width, height, numChannels;
        uint handle;
    };
    struct RenderableObject
    {
        float *verts;
        uint8_t numverts;
        uint8_t stride, num_attribs;
        std::vector<uint8_t> attrib_offset;

        glm::mat4 translation;
        glm::mat4 model_view_projection;
        uint matrixID;
        uint size;
        uint handle;
    };
    struct Shader{
        uint h_program;
        uint h_vert, h_frag;
        const char *path_vert, *path_frag, *src_vert, *src_frag;

    };
    
}