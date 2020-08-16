#include "DGRCore.h"
#include "util/Logger.h"
#include "util/FileManip.h"
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
    using namespace util::debug;

    void Core::init(){

        DGR_LOG_INFO("Initializing GLFW");
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        DGR_LOG_INFO("Initializing Window:\n");
        window_attribs = {800, 600, "Danger Engine"};
        ctx_window = glfwCreateWindow(window_attribs.width, window_attribs.height, window_attribs.title, NULL, NULL);
        if (ctx_window == NULL)
        {
            glfwTerminate();
            DGR_LOG_FATAL("GLFW WINDOW NOT CREATED")
        }
        glfwMakeContextCurrent(ctx_window);

        DGR_LOG_INFO("Initializing GLAD:\n");
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            DGR_LOG_FATAL("GLAD broke D:");
        }

        projection_matrix = glm::perspective(glm::radians(45.0f), (float)window_attribs.width / (float)window_attribs.height, 0.1f, 100.0f);
        view_matrix = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        float temp_verts[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
        temp.verts = temp_verts;
        temp.numverts = 9;
        temp.stride = 3 * sizeof(float);
        temp.translation = glm::mat4(1.0f);
        temp.model_view_projection = projection_matrix * view_matrix * temp.translation;

        setup();
        
    }

    void Core::setup(){
        glfwSetFramebufferSizeCallback(ctx_window, setFramebufferSize);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);

        glGenVertexArrays(1, &vao);
        DGR_LOG_INFO("generated vert array");
        glBindVertexArray(vao);
        DGR_LOG_INFO("bound vert array");

        loadShaders("src/shaders/vShader.glsl", "src/shaders/fShader.glsl");

        float temp_verts[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
        glGenBuffers(1, &vbo_list);
        DGR_LOG_INFO("generated vbo");
        glBindBuffer(GL_ARRAY_BUFFER, vbo_list);
        DGR_LOG_INFO("bound vbo");
        glBufferData(GL_ARRAY_BUFFER, sizeof(temp_verts), temp_verts, GL_STATIC_DRAW); //Dynamic draw finna be used a lot, stream draw maybe too. remember em
        DGR_LOG_INFO("bound buffer data { ", sizeof(temp_verts), " }");

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        temp.matrixID = glGetUniformLocation(shader_program, "MVP");
    }

    
    void setFramebufferSize(GLFWwindow* window, int width, int height)
    {
        DGR_LOG_INFO("Framebuffer size set to ", width, ", ", height);
        glViewport(0, 0, width, height);
    }

    bool Core::shouldClose(){
        DGR_LOG_INFO("shouldClose: ", glfwWindowShouldClose(ctx_window));
        return glfwWindowShouldClose(ctx_window);
    }

    void Core::preLoopProcesses(){
        checkInput(ctx_window);
        DGR_LOG_INFO("checked input")
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DGR_LOG_INFO("cleared color buffer")
    }
    
    void Core::draw(){
        glUseProgram(shader_program);
        DGR_LOG_INFO("used shader")
        DGR_LOG_FATAL("current: ", shader_program);
        glBindVertexArray(vao);
        DGR_LOG_INFO("bound vao")
        glUniformMatrix4fv(temp.matrixID, 1, GL_FALSE, &(temp.model_view_projection[0][0]));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        DGR_LOG_INFO("drew triangles")
    }
    void Core::postLoopProcesses(){


        glfwPollEvents();
        DGR_LOG_INFO("polled events")
        glfwSwapBuffers(ctx_window);
        DGR_LOG_INFO("swapped buffers")
        GLenum err = glGetError();
        while ((err = glGetError()) != GL_NO_ERROR){
            DGR_LOG_ERROR("OPENGL ERROR: ", err);
        }
    }

    void Core::terminate(){
        glfwTerminate();
    }

    void Core::setBlankCol(float r, float g, float b, float a){
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT);
        
    }

    void checkInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

   


    void Core::loadShaders(const char vertexFile[], const char fragmentFile[]){
        std::string vstring = util::io::getFileText(vertexFile);
        std::string fstring = util::io::getFileText(fragmentFile);
        const char *vert = vstring.c_str();
        const char* frag = fstring.c_str();
        DGR_LOG_INFO("%s", frag);
        DGR_LOG_INFO("%s", vert);
        uint h_vert, h_frag;
        
        auto query = [](auto func, auto handle, auto queryType, auto logfunc) {  
            int success=0; char debugInfo[512];
            func(handle, queryType, &success);
            if(!success) { 
                logfunc(handle, 512, NULL, debugInfo);
                DGR_LOG_ERROR("failed to load shaders:\n >>>> ", debugInfo, " <<<<");
            }
            else{
                DGR_LOG_INFO("success!");
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
        DGR_LOG_FATAL("initial: ",shader_program);
        glAttachShader(shader_program, h_vert);
        glAttachShader(shader_program, h_frag);
        glLinkProgram(shader_program);
        
        query(glGetProgramiv, shader_program, GL_LINK_STATUS, glGetProgramInfoLog);
        glDeleteShader(h_vert);
        glDeleteShader(h_frag);
        //delete[] vert, frag; //these are pointers so im p sure they don't get freed when they go outta scope here... 
                            //debugInfo should tho since it's stack allocated. it's 512 bytes, but i think default c++ stack size is 1MB


    }

    void GLAPIENTRY
    MessageCallback(GLenum source,
                    GLenum type,
                    GLuint id,
                    GLenum severity,
                    GLsizei length,
                    const GLchar *message,
                    const void *userParam)
    {
        if(type == GL_DEBUG_TYPE_ERROR){
            DGR_LOG_ERROR("GL CALLBACK: type = ", type, ", severity = ", severity, ", message = ", message, "\n");
        }
        else{
            DGR_LOG_INFO("GL CALLBACK: type = ", type, ", severity = ", severity, ", message = ", message, "\n");
        }
    }

}