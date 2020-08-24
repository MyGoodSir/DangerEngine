/*\
 *
 * Will be API's outward interface 
 * TODO: figure out exactly how this is gonna be structured
 * 
\*/
#pragma once
#include "Danger.h"
#include "structs.h"
#include "Context.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace DGREngine{
    using namespace structs;
    //create vertex and vertex attribute structs and stuff later
    class Core
    {
        const char* name;
        uint w, h;
        GLFWwindow* window;
        uint shader, u_loc, vbuf, ibuf, varr;
        Context ctx{};
    public:
        Core(const char *app_name = "default title", uint window_width = 800, uint window_height = 600) : name(app_name), w(window_width), h(window_height){}
        void init();
        void run();
        void makeObjects();
    };
    static uint createShaderProgram(const char vertexFile[], const char fragmentFile[]);
    void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
}

