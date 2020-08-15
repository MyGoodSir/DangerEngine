#include "DGRCore.h"
#include <iostream>
#include <stdexcept>

namespace DGREngine
{

    /*                  TODO
     * -put together a respectable logging system
     * -create an event manager and messaging system
     * -put key input listening into event system
     * -make a custom assert instead of throwing arbitrary stl errors
     * -refactor class into POD structs and free functions
     * -hide lesser used functions from intellisense
     */

    void Core::init(){
        printf("Initializing GLFW:\n");
        initGLFW();
        printf("Initializing Window:\n");
        genWindow( {800, 600, "Danger Engine"} );
        printf("Initializing GLAD:\n");
        glfwSetFramebufferSizeCallback(ctx_window, setFramebufferSize);
        initGLAD();
        printf("Initializing FramebufferSize:\n");
        setFramebufferSize(ctx_window, window_attribs.width, window_attribs.height);
        printf("Initializing FramebufferSizeCallback:\n");
        loadShaders("src/shaders/vShader.glsl", "src/shaders/fShader.glsl");
        newVAO();
        genVBO(temp_verts);
        setVertexAttributes();
        
    }
    void Core::initGLFW(){
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
    }

    void Core::genWindow(WindowAttributes attribs)
    {
        window_attribs = attribs;
        ctx_window = glfwCreateWindow(window_attribs.width, window_attribs.height, window_attribs.title, NULL, NULL);
        if (ctx_window == NULL)
        {
            glfwTerminate();
            throw std::runtime_error("window broke D:\n");
        }
        glfwMakeContextCurrent(ctx_window);
    }

    void Core::initGLAD(){
        if( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
        {
            throw std::runtime_error("GLAD broke D:\n");
        }
    }
    void setFramebufferSize(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    bool Core::shouldClose(){
        return glfwWindowShouldClose(ctx_window);
    }

    void Core::preLoopProcesses(){
        checkInput(ctx_window);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    void Core::draw(){
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    void Core::postLoopProcesses(){


        glfwPollEvents();
        glfwSwapBuffers(ctx_window);
    }

    void Core::terminate(){
        glfwTerminate();
    }

    void Core::setBlankCol(float r, float g, float b, float a){
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Core::newVAO(){
        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);
    }

    uint Core::genVBO(float *data)
    {
        glGenBuffers(1, &vbo_list);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_list);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW); //Dynamic draw finna be used a lot, stream draw maybe too. remember em
        return vbo_list;
    }

    void Core::setVertexAttributes(){
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void checkInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

   


    void Core::loadShaders(const char vertexFile[], const char fragmentFile[]){
        const char* vert = util::io::getFileText(vertexFile);
        const char* frag = util::io::getFileText(fragmentFile);

        std::cout<<vert<<"\n \n"<<frag;
        uint h_vert, h_frag;

        auto query = [](auto func, auto handle, auto queryType, auto logfunc) {  
            int success; char debugInfo[512];
            func(handle, queryType, &success);
            if(!success) { 
                logfunc(handle, 512, NULL, debugInfo);
                printf("failed to load shaders:\n >>>> %s <<<<", debugInfo); 
            }
            else{
                printf("success!");
            }
        };

        h_vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(h_vert, 1, &vert, NULL);
        glCompileShader(h_vert);
        h_frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(h_frag, 1, &frag, NULL);
        glCompileShader(h_frag);
        
        query(glGetShaderiv, h_vert, GL_COMPILE_STATUS, glGetShaderInfoLog);
        query(glGetShaderiv, h_frag, GL_COMPILE_STATUS, glGetShaderInfoLog);

        shader_program = glCreateProgram();

        glAttachShader(shader_program, h_vert);
        glAttachShader(shader_program, h_frag);
        glLinkProgram(shader_program);

        query(glGetProgramiv, shader_program, GL_LINK_STATUS, glGetProgramInfoLog);
        //delete[] vert, frag; //these are pointers so im p sure they don't get freed when they go outta scope here... 
                            //debugInfo should tho since it's stack allocated. it's 512 bytes, but i think default c++ stack size is 1MB


    }



}