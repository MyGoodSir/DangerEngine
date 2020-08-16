#pragma once
#include "Danger.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace DGREngine{
    typedef unsigned int uint;
    struct WindowAttributes
    {
        uint width, height;
        const char *title;
        
    };
    struct RenderableObject{
        float* verts;
        uint8_t numverts;
        uint8_t stride;
        glm::mat4 translation;
        glm::mat4 model_view_projection;
        uint matrixID;
    };
    //create vertex and vertex attribute structs and stuff later
    class Core
    {
    public:
        
        GLFWwindow *ctx_window;
        WindowAttributes window_attribs;
        uint vbo_list;
        int shader_program;
        uint vao;
        glm::mat4 projection_matrix;
        glm::mat4 view_matrix;
        RenderableObject temp;

        Core(){};
        void init();
        void setup();
        bool shouldClose();
        void preLoopProcesses();
        void draw();
        void postLoopProcesses();
        void terminate();
        void setBlankCol(float, float, float, float);
        void loadShaders(const char [], const char []);

    };
    void setFramebufferSize(GLFWwindow *window, int width, int height);
    void checkInput(GLFWwindow *window);
    void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
}

