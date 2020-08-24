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
    using namespace util::debug;
    using namespace structs;

    void Core::init(){
        if(!glfwInit()){
            DGR_LOG_FATAL("GLFW REFUSES TO INITIALIZE");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glEnable(GL_DEBUG_OUTPUT);
        //glDebugMessageCallback(MessageCallback, 0);
        window = glfwCreateWindow(w, h, name, NULL, NULL);
        if(!window){
            DGR_LOG_FATAL("WINDOW REFUSES TO EXIST");
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);//vsync sync
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            DGR_LOG_FATAL("GLAD REFUSES TO MAKE ME HAPPY");
        }

        float positions[28] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
        };
        uint indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        ctx.shader.CreateShaderFromFiles("src/shaders/vShader.vert", "src/shaders/fShader.frag");
        ctx.CreateVAO();
        ctx.GenerateBuffer(positions, 28*sizeof(float), BufferType::VERT);
        AttributeInfo info1 = {AttributeInfo::DataType::FLOAT, 2, 0};
        AttributeInfo info2 = {AttributeInfo::DataType::FLOAT, 3, 2*sizeof(float)};
        AttributeInfo info3 = {AttributeInfo::DataType::FLOAT, 2, 5 * sizeof(float)};
        ctx.current_vao.attribute_count = 3;
        ctx.current_vao.attribs.push_back(info1);
        ctx.current_vao.attribs.push_back(info2);
        ctx.current_vao.attribs.push_back(info3);
        ctx.current_vao.vertex_stride = 7*sizeof(float);
        ctx.SetAttributes();
        ctx.GenerateBuffer(indices, 6 * sizeof(uint), BufferType::IND);

        float ucol[4] = {0.0f,0.0f,1.0f,1.0f};
        ctx.shader.SetUniformFloatVec("uCol", ucol, 4);
        ctx.shader.LoadTexture("src/assets/textures/wall.jpg");
        ctx.shader.SetUniformSamplerSlot("uTex", ctx.shader.current_texture.sampler_slot);

        ctx.UnbindAll();

    }

    void Core::run(){

        glm::vec4 col = {0.25f, 0.0f, 0.75f, 1.0f};
        glm::vec4 delta_col = {0.056f, 0.0f, -0.05f, 0.0f};

        while(!glfwWindowShouldClose(window)){
            glClear(GL_COLOR_BUFFER_BIT);
            if(col.x <= 0.0f)
                delta_col.x = 0.005f;
            if (col.x >= 1.0f)
                delta_col.x = -0.005f;
            if (col.z <= 0.0f)
                delta_col.z = 0.005f;
            if (col.z >= 1.0f)
                delta_col.z = -0.005f;

            col += delta_col;

            ctx.BindAll();
            ctx.shader.SetUniformFloatVec("uCol", &col[0], 4 );
            ctx.shader.SetUniformSamplerSlot("uTex", ctx.shader.current_texture.sampler_slot);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glDeleteProgram(shader);
        glfwTerminate();
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
        
    }

    //static uint createShaderProgram(const char vertexFile[], const char fragmentFile[])
    //{
    //    std::string vstring = util::io::getFileText(vertexFile);
    //    std::string fstring = util::io::getFileText(fragmentFile);
    //    const char *src_vert = vstring.c_str();
    //    const char *src_frag = fstring.c_str();
    //    //DGR_LOG_INFO("FragmentShader:\n", frag);
    //    //DGR_LOG_INFO("VertexShader\n", vert);
    //    uint h_vert = glCreateShader(GL_VERTEX_SHADER);
    //    uint h_frag = glCreateShader(GL_FRAGMENT_SHADER);
    //    uint h_program;
    //    
    //    auto query = [=](auto func, auto handle, auto queryType, auto logfunc) {  
    //        int success=0; char debugInfo[512];
    //        func(handle, queryType, &success);
    //        if(!success) { 
    //            logfunc(handle, 512, NULL, debugInfo);
    //            DGR_LOG_ERROR("failed to load shaders:\n >>>> ", debugInfo, " <<<<");
    //        }
    //        else{
    //            const char *output = (queryType == GL_COMPILE_STATUS ? "shader link" : (handle == h_vert ? "vertex shader compilation" : "fragment shader compilation"));
    //            DGR_LOG_INFO("successful", output,"!");
    //        }
    //    };
    //    
    //    glShaderSource(h_vert, 1, &(src_vert), NULL);
    //    glCompileShader(h_vert);
    //    glShaderSource(h_frag, 1, &(src_frag), NULL);
    //    glCompileShader(h_frag);
    //    
    //    query(glGetShaderiv, h_vert, GL_COMPILE_STATUS, glGetShaderInfoLog);
    //    query(glGetShaderiv, h_frag, GL_COMPILE_STATUS, glGetShaderInfoLog);
    //    h_program = glCreateProgram();
    //    glAttachShader(h_program, h_vert);
    //    glAttachShader(h_program, h_frag);
    //    glLinkProgram(h_program);
    //    glValidateProgram(h_program);
//
    //    query(glGetProgramiv, h_program, GL_LINK_STATUS, glGetProgramInfoLog);
    //    glDeleteShader(h_vert);
    //    glDeleteShader(h_frag);
    //    return h_program;
    //    //delete[] vert, frag; //these are pointers so im p sure they don't get freed when they go outta scope here... 
    //                        //debugInfo should tho since it's stack allocated. it's 512 bytes, but i think default c++ stack size is 1MB
    //}

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
