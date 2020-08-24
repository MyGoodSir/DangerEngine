#pragma once
#include "structs.h"
#include "Structures/Shader.h"
namespace DGREngine::structs
{
    struct Context
    {
        std::vector<VAO> vao_list;
        std::vector<Buffer> index_buffer_list;
        VAO current_vao;
        Buffer bound_index_buffer;
        Shader shader{};

        Buffer GenerateBuffer(void *data, uint size, BufferType type);
        void BindBuffer(Buffer buf);
        void BindAll();
        void UnbindAll();
        void UnbindBuffer(Buffer buf);
        VAO CreateVAO();
        void SetVAO(VAO vao);
        void UnbindVAO();
        void SetAttributes();
    };
}