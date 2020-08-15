#pragma once
#include "Danger.h"
#include "util/FileManip.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>

namespace DGREngine{
    typedef uint32_t uint;
    struct WindowAttributes
    {
        uint width, height;
        const char *title;
        
    };
    struct Handles{
        std::vector<uint> vbo_list;
        uint shader_program;
        uint vao;
    };
    
    //create vertex and vertex attribute structs and stuff later
    class Core
    {
    public:
            float temp_verts[9] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f    };

        GLFWwindow *ctx_window;
        WindowAttributes window_attribs;
        uint vbo_list;
        uint shader_program;
        uint vao;

        void init();
        void initGLFW();
        void genWindow(WindowAttributes attribs);
        void initGLAD();
        bool shouldClose();
        void preLoopProcesses();
        void draw();
        void postLoopProcesses();
        void terminate();
        void setBlankCol(float, float, float, float);
        void newVAO();
        uint genVBO(float*);
        void setVertexAttributes();//the way im doing this means i need to populate some local list of VAPs ahead of time when i start abstracting more. for now numbers are hard coded bc im lazy
        void loadShaders(const char [], const char []);

    };
    void setFramebufferSize(GLFWwindow *window, int width, int height);
    void checkInput(GLFWwindow *window);
}