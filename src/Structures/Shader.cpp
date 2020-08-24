
#include "Shader.h"
#include "util/FileManip.h"
#include "util/Logger.h"

namespace DGREngine::structs
{
    using namespace util::debug;
    void Shader::SetUniformFloatVec(const char *name, float *data, int count)
    {
        uint loc = glGetUniformLocation(handle, name);
        switch (count)
        {
        case 1:
            glUniform1fv(loc, 1, data);
            break;
        case 2:
            glUniform2fv(loc, 1, data);
            break;
        case 3:
            glUniform3fv(loc, 1, data);
            break;
        case 4:
            glUniform4fv(loc, 1, data);
            break;
        default:
            break;
        }
    }
    void Shader::SetUniformIntVec(const char *name, int *data, int count)
    {
        uint loc = glGetUniformLocation(handle, name);
        switch (count)
        {
        case 1:
            glUniform1i(loc, data[0]);
            break;
        case 2:
            glUniform2iv(loc, 1, data);
            break;
        case 3:
            glUniform3iv(loc, 1, data);
            break;
        case 4:
            glUniform4iv(loc, 1, data);
            break;
        default:
            break;
        }
    }
    void Shader::SetUniformFloatMat(const char *name, float *data, int count)
    {
        uint loc = glGetUniformLocation(handle, name);
        switch (count)
        {
        case 2:
            glUniformMatrix2fv(loc, 1, GL_FALSE, data);
            break;
        case 3:
            glUniformMatrix3fv(loc, 1, GL_FALSE, data);
            break;
        case 4:
            glUniformMatrix4fv(loc, 1, GL_FALSE, data);
            break;
        default:
            break;
        }
    }
    void Shader::SetUniformSamplerSlot(const char *name, int slot){
        uint loc = glGetUniformLocation(handle, name);
        glUniform1i(loc, slot);

    }
        //! MEMORYLEAK: USE stbi_image_free(tex.data) TO FREE DATA FROM RAM
        //! MEMORYLEAK: USE glDeleteTextures(1, &(tex.handle)) TO FREE DATA FROM VRAM
    TextureData Shader::LoadTexture(const char path[])
    {
        TextureData tex = util::io::loadImage(path);
        tex.sampler_slot = 0;
        glGenTextures(1, &(tex.handle));
        sampler_slots++;
        glBindTexture(GL_TEXTURE_2D, tex.handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
        glActiveTexture(GL_TEXTURE0 + sampler_slots);
        current_texture = tex;
        return tex;
    }
    void Shader::BindTexture(uint tex_handle, uint slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
    }
    void Shader::FreeTextureRAM(TextureData tex)
    {
        util::io::FreeTextureData(tex.data);
    }
    void Shader::FreeTextureVRAM(TextureData tex)
    {
        glDeleteTextures(1, &(tex.handle));
    }
    void Shader::CreateShaderFromFiles(const char vertexFile[], const char fragmentFile[])
    {
        std::string vstring = util::io::getFileText(vertexFile);
        std::string fstring = util::io::getFileText(fragmentFile);
        const char *src_vert = vstring.c_str();
        const char *src_frag = fstring.c_str();
        //DGR_LOG_INFO("FragmentShader:\n", frag);
        //DGR_LOG_INFO("VertexShader\n", vert);
        uint h_vert = glCreateShader(GL_VERTEX_SHADER);
        uint h_frag = glCreateShader(GL_FRAGMENT_SHADER);

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
                const char *output = (queryType == GL_COMPILE_STATUS ? "shader link" : (handle == h_vert ? "vertex shader compilation" : "fragment shader compilation"));
                DGR_LOG_INFO("successful", output, "!");
            }
        };

        glShaderSource(h_vert, 1, &(src_vert), NULL);
        glCompileShader(h_vert);
        glShaderSource(h_frag, 1, &(src_frag), NULL);
        glCompileShader(h_frag);

        query(glGetShaderiv, h_vert, GL_COMPILE_STATUS, glGetShaderInfoLog);
        query(glGetShaderiv, h_frag, GL_COMPILE_STATUS, glGetShaderInfoLog);
        handle = glCreateProgram();
        glAttachShader(handle, h_vert);
        glAttachShader(handle, h_frag);
        glLinkProgram(handle);
        glValidateProgram(handle);

        query(glGetProgramiv, handle, GL_LINK_STATUS, glGetProgramInfoLog);
        glDeleteShader(h_vert);
        glDeleteShader(h_frag);
        //delete[] vert, frag; //these are pointers so im p sure they don't get freed when they go outta scope here...
        //debugInfo should tho since it's stack allocated. it's 512 bytes, but i think default c++ stack size is 1MB
    }

} // namespace DGREngine::structs