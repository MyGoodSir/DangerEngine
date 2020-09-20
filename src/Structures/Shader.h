#pragma once
#include <Danger.h>
#include <glm/glm.hpp>
#include <util/FileManip.h>
#include <util/Logger.h>

#include <string>
#include <iostream>
#include <map>

typedef unsigned int Shader;
typedef unsigned int uint;
//TODO: ADD UBOs

class ShaderManager
{
public:
    static void Init() {}
    static Shader Generate(const char vertexFile[], const char fragmentFile[], const char geometryFile[] = nullptr)
    {
        std::string vstring, fstring, gstring;
        const char* src_vert; const char* src_frag; const char* src_geom;

        vstring = FileIOManager::getFileText(vertexFile);
        fstring = FileIOManager::getFileText(fragmentFile);


        src_vert = vstring.c_str();
        src_frag = fstring.c_str();
        uint h_vert, h_frag, h_geom;
        h_vert = glCreateShader(GL_VERTEX_SHADER);
        h_frag = glCreateShader(GL_FRAGMENT_SHADER);

        if (geometryFile) {
            gstring = FileIOManager::getFileText(fragmentFile);
            src_geom = gstring.c_str(); 
            h_geom = glCreateShader(GL_GEOMETRY_SHADER);
        }

        auto query = [=](auto func, auto handle, auto queryType, auto logfunc) {
            int success = 0;
            char debugInfo[512];
            func(handle, queryType, &success);
            if (!success)
            {
                logfunc(handle, 512, NULL, debugInfo);
                DGR_LOG_ERROR("failed to load shaders:\n >>>> ", debugInfo, " <<<<");
            }
            else
            {
                const char *output = (queryType == GL_COMPILE_STATUS ? "shader link" : (handle == h_vert ? "vertex shader compilation" : (handle == h_frag ? "fragment shader compilation" : "geometry shader compilation")));//this is stupid
                std::cout<<"successful " << output<< "!\n";
            }
        };

        glShaderSource(h_vert, 1, &(src_vert), NULL);
        glCompileShader(h_vert);
        glShaderSource(h_frag, 1, &(src_frag), NULL);
        glCompileShader(h_frag);
        

       // query(glGetShaderiv, h_vert, GL_COMPILE_STATUS, glGetShaderInfoLog);
        //query(glGetShaderiv, h_frag, GL_COMPILE_STATUS, glGetShaderInfoLog);

        if (geometryFile) {
            glShaderSource(h_geom, 1, &(src_geom), NULL);
            glCompileShader(h_geom);
            query(glGetShaderiv, h_geom, GL_COMPILE_STATUS, glGetShaderInfoLog);
        }

        Shader handle = glCreateProgram();
        glAttachShader(handle, h_vert);
        glAttachShader(handle, h_frag);
        if (geometryFile){ glAttachShader(handle, h_geom); }

        glLinkProgram(handle);
        glValidateProgram(handle);

        //query(glGetProgramiv, handle, GL_LINK_STATUS, glGetProgramInfoLog);
        glDeleteShader(h_vert);
        glDeleteShader(h_frag);
        if (geometryFile) { glDeleteShader(h_geom); }
        
        //debugInfo should tho since it's stack allocated. it's 512 bytes, but i think default c++ stack size is 1MB
        return handle;
    }
    
    static void use(Shader s)
    {
        glUseProgram(s);
    }

    static void setUniBool(Shader handle, const std::string &name, bool value)
    {
        glUniform1i(glGetUniformLocation(handle, name.c_str()), (int)value);
    }
    
    static void setUniInt(Shader handle, const std::string &name, int value)
    {
        glUniform1i(glGetUniformLocation(handle, name.c_str()), value);
    }
    
    static void setUniFloat(Shader handle, const std::string &name, float value)
    {
        glUniform1f(glGetUniformLocation(handle, name.c_str()), value);
    }
    
    static void setUniVec2(Shader handle, const std::string &name, const glm::vec2 &value)
    {
        glUniform2fv(glGetUniformLocation(handle, name.c_str()), 1, &value[0]);
    }
    static void setUniVec2(Shader handle, const std::string &name, float x, float y)
    {
        glUniform2f(glGetUniformLocation(handle, name.c_str()), x, y);
    }
    
    static void setUniVec3(Shader handle, const std::string &name, const glm::vec3 &value)
    {
        glUniform3fv(glGetUniformLocation(handle, name.c_str()), 1, &value[0]);
    }
    static void setUniVec3(Shader handle, const std::string &name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(handle, name.c_str()), x, y, z);
    }
    
    static void setUniVec4(Shader handle, const std::string &name, const glm::vec4 &value)
    {
        glUniform4fv(glGetUniformLocation(handle, name.c_str()), 1, &value[0]);
    }
    static void setUniVec4(Shader handle, const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(handle, name.c_str()), x, y, z, w);
    }
    
    static void setUniMat2(Shader handle, const std::string &name, const glm::mat2 &mat)
    {
        glUniformMatrix2fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    static void setUniMat3(Shader handle, const std::string &name, const glm::mat3 &mat)
    {
        glUniformMatrix3fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    static void setUniMat4(Shader handle, const std::string &name, const glm::mat4 &mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    inline static uint getLocation(Shader handle, const std::string& name) {
        
    }
};
