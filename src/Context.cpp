#include "Context.h"

namespace DGREngine::structs{
    Buffer Context::GenerateBuffer(void *data, uint size, BufferType type)
    {
        Buffer buf;
        buf.type = type;
        glGenBuffers(1, &(buf.handle));
        if (type == VERT)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buf.handle);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            current_vao.vertex_buffer = buf;
        }
        else if (type == IND)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.handle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            current_vao.index_buffer = buf;
        }
        return buf;
    }
    void Context::BindBuffer(Buffer buf)
    {
        if (buf.type == VERT)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buf.handle);
        }
        else if (buf.type == IND)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.handle);
        }
    }
    void Context::BindAll()
    {
        glUseProgram(shader.handle);
        shader.BindTexture(shader.current_texture.handle, shader.current_texture.sampler_slot);
        glBindVertexArray(current_vao.handle);
        glBindBuffer(GL_ARRAY_BUFFER, current_vao.vertex_buffer.handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vao.index_buffer.handle);
    }
    void Context::UnbindAll()
    {
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void Context::UnbindBuffer(Buffer buf)
    {
        if (buf.type == VERT)
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (buf.type == IND)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    VAO Context::CreateVAO()
    {
        VAO vao;
        vao.attribute_count = 0;
        glGenVertexArrays(1, &(vao.handle));
        glBindVertexArray(vao.handle);
        current_vao = vao;
        vao_list.push_back(vao);
        return vao;
    }
    void Context::SetVAO(VAO vao)
    {
        current_vao = vao;
        glBindVertexArray(current_vao.handle);
        BindBuffer(vao.vertex_buffer);
        BindBuffer(vao.index_buffer);
    }
    void Context::UnbindVAO()
    {
        glBindVertexArray(0);
        //current_vao = {0,0};
    }
    void Context::SetAttributes()
    {
        for (int i = 0; i < current_vao.attribute_count; i++)
        {
            AttributeInfo info = current_vao.attribs[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, info.count, info.type, GL_FALSE, current_vao.vertex_stride, (const void *)(1 * info.offset));
        }
    }



}
