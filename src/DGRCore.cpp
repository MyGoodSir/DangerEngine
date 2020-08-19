#include "DGRCore.h"
#include "util/Logger.h"
#include "util/FileManip.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>

namespace DGREngine
{

    /*                  TODO
     * -put together a respectable logging system                               COMPLETE
     * -create an event manager and messaging system                            
     * -put key input listening into event system
     * -make a custom assert instead of throwing arbitrary stl errors
     * -refactor class into POD structs and free functions
     * -hide lesser used functions from intellisense
     */
    using namespace util::debug;
    using namespace structs;

    void Core::init(){

        DGR_LOG_INFO("Initializing GLFW");
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        DGR_LOG_INFO("Initializing Window:\n");
        GLFWwindow* ctx_window = glfwCreateWindow(800, 600, "Danger Engine", NULL, NULL);
        window_attribs = {ctx_window, 800, 600, "Danger Engine"};
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

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        projection_matrix = glm::perspective(glm::radians(45.0f), (float)window_attribs.width / (float)window_attribs.height, 0.1f, 100.0f);
        view_matrix = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


        makeObjects();
        setup();
    }

    void Core::setup(){
        glfwSetFramebufferSizeCallback(window_attribs.handle, setFramebufferSize);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        loadShaders("src/shaders/vShader.vert", "src/shaders/fShader.frag");

        temp.matrixID = glGetUniformLocation(shader.h_program, "MVP");

        glGenBuffers(1, &(temp.handle));
        glBindBuffer(GL_ARRAY_BUFFER, temp.handle);
        glBufferData(GL_ARRAY_BUFFER, temp.size, temp.verts, GL_STATIC_DRAW); //Dynamic draw finna be used a lot, stream draw maybe too. remember em

    }

    
    void setFramebufferSize(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    bool Core::shouldClose(){
        return glfwWindowShouldClose(window_attribs.handle);
    }

    void Core::preLoopProcesses(){
        checkInput(window_attribs.handle);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void Core::draw(){
        glUseProgram(shader.h_program);

        glBindVertexArray(vao);
        
        glUniformMatrix4fv(temp.matrixID, 1, GL_FALSE, &(temp.model_view_projection[0][0]));

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, temp.handle);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, temp.stride, (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, temp.stride, (void *)(3 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, 12*3);

    }
    void Core::postLoopProcesses(){


        glfwPollEvents();
        glfwSwapBuffers(window_attribs.handle);
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

    void Core::registerRenderable(RenderableObject &obj){
        objects.push_back(obj);
    }

    void Core::makeObjects(){
        float* temp_verts = new float[216]{
                                          -1.0f, -1.0f, -1.0f,      0.583f, 0.771f, 0.014f,
                                          -1.0f, -1.0f, 1.0f,       0.609f, 0.115f, 0.436f,
                                          -1.0f, 1.0f, 1.0f,        0.327f, 0.483f, 0.844f,
                                          1.0f, 1.0f, -1.0f,        0.822f, 0.569f, 0.201f,
                                          -1.0f, -1.0f, -1.0f,      0.435f, 0.602f, 0.223f,
                                          -1.0f, 1.0f, -1.0f,       0.310f, 0.747f, 0.185f,
                                          1.0f, -1.0f, 1.0f,        0.597f, 0.770f, 0.761f,
                                          -1.0f, -1.0f, -1.0f,      0.559f, 0.436f, 0.730f,
                                          1.0f, -1.0f, -1.0f,       0.359f, 0.583f, 0.152f,
                                          1.0f, 1.0f, -1.0f,        0.483f, 0.596f, 0.789f,
                                          1.0f, -1.0f, -1.0f,       0.559f, 0.861f, 0.639f,
                                          -1.0f, -1.0f, -1.0f,      0.195f, 0.548f, 0.859f,
                                          -1.0f, -1.0f, -1.0f,      0.014f, 0.184f, 0.576f,
                                          -1.0f, 1.0f, 1.0f,        0.771f, 0.328f, 0.970f,
                                          -1.0f, 1.0f, -1.0f,       0.406f, 0.615f, 0.116f,
                                          1.0f, -1.0f, 1.0f,        0.676f, 0.977f, 0.133f,
                                          -1.0f, -1.0f, 1.0f,       0.971f, 0.572f, 0.833f,
                                          -1.0f, -1.0f, -1.0f,      0.140f, 0.616f, 0.489f,
                                          -1.0f, 1.0f, 1.0f,        0.997f, 0.513f, 0.064f,
                                          -1.0f, -1.0f, 1.0f,       0.945f, 0.719f, 0.592f,
                                          1.0f, -1.0f, 1.0f,        0.543f, 0.021f, 0.978f,
                                          1.0f, 1.0f, 1.0f,         0.279f, 0.317f, 0.505f,
                                          1.0f, -1.0f, -1.0f,       0.167f, 0.620f, 0.077f,
                                          1.0f, 1.0f, -1.0f,        0.347f, 0.857f, 0.137f,
                                          1.0f, -1.0f, -1.0f,       0.055f, 0.953f, 0.042f,
                                          1.0f, 1.0f, 1.0f,         0.714f, 0.505f, 0.345f,
                                          1.0f, -1.0f, 1.0f,        0.783f, 0.290f, 0.734f,
                                          1.0f, 1.0f, 1.0f,         0.722f, 0.645f, 0.174f,
                                          1.0f, 1.0f, -1.0f,        0.302f, 0.455f, 0.848f,
                                          -1.0f, 1.0f, -1.0f,       0.225f, 0.587f, 0.040f,
                                          1.0f, 1.0f, 1.0f,         0.517f, 0.713f, 0.338f,
                                          -1.0f, 1.0f, -1.0f,       0.053f, 0.959f, 0.120f,
                                          -1.0f, 1.0f, 1.0f,        0.393f, 0.621f, 0.362f,
                                          1.0f, 1.0f, 1.0f,         0.673f, 0.211f, 0.457f,
                                          -1.0f, 1.0f, 1.0f,        0.820f, 0.883f, 0.371f,
                                          1.0f, -1.0f, 1.0f,        0.982f, 0.099f, 0.879f
                                          };
        temp.verts = temp_verts;
        temp.numverts = 36;
        temp.stride = 6 * sizeof(float);
        temp.translation = glm::mat4(1.0f);
        temp.model_view_projection = projection_matrix * view_matrix * temp.translation;
        temp.size = temp.numverts * temp.stride;
        temp.num_attribs = 2;
        temp.attrib_offset.push_back(3);
        temp.attrib_offset.push_back(3);
    }


    void Core::loadShaders(const char vertexFile[], const char fragmentFile[]){
        shader.path_vert = vertexFile;
        shader.path_frag = fragmentFile;
        std::string vstring = util::io::getFileText(vertexFile);
        std::string fstring = util::io::getFileText(fragmentFile);
        shader.src_vert = vstring.c_str();
        shader.src_frag = fstring.c_str();
        //DGR_LOG_INFO("FragmentShader:\n", frag);
        //DGR_LOG_INFO("VertexShader\n", vert);
        
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
        
        shader.h_vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shader.h_vert, 1, &(shader.src_vert), NULL);
        glCompileShader(shader.h_vert);
        shader.h_frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shader.h_frag, 1, &(shader.src_frag), NULL);
        glCompileShader(shader.h_frag);
        
        query(glGetShaderiv, shader.h_vert, GL_COMPILE_STATUS, glGetShaderInfoLog);
        query(glGetShaderiv, shader.h_frag, GL_COMPILE_STATUS, glGetShaderInfoLog);
        
        shader.h_program = glCreateProgram();
        glAttachShader(shader.h_program, shader.h_vert);
        glAttachShader(shader.h_program, shader.h_frag);
        glLinkProgram(shader.h_program);

        query(glGetProgramiv, shader.h_program, GL_LINK_STATUS, glGetProgramInfoLog);
        glDeleteShader(shader.h_vert);
        glDeleteShader(shader.h_frag);
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
