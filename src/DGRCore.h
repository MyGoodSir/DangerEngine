#pragma once
#include "Danger.h"
#include "structs.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace DGREngine{
    using namespace structs;
    //create vertex and vertex attribute structs and stuff later
    class Core
    {
    public:
        
        
        WindowAttributes window_attribs{};
        uint vao;
        glm::mat4 projection_matrix;
        glm::mat4 view_matrix;
        RenderableObject temp{};
        TextureData* tex;
        std::vector<RenderableObject> objects;
        Shader shader{};

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
        void registerRenderable(RenderableObject &obj);
        void makeObjects();
    };
    void setFramebufferSize(GLFWwindow *window, int width, int height);
    void checkInput(GLFWwindow *window);
    void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
}

