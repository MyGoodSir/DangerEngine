#pragma once
#include "structs.h"
namespace DGREngine::structs
{
    struct Shader{
        uint handle, sampler_slots;
        TextureData current_texture{};
        void SetUniformFloatVec( const char* name, float* data, int count);
        void SetUniformIntVec( const char *name, int *data, int count);
        void SetUniformFloatMat( const char *name, float *data, int count);
        void SetUniformSamplerSlot(const char* name, int slot);
        void CreateShaderFromFiles(const char vertexFile[], const char fragmentFile[]);
        TextureData LoadTexture(const char path[]);
        void BindTexture(uint tex_handle, uint slot);
        void FreeTextureRAM(TextureData tex);
        void FreeTextureVRAM(TextureData tex);
    };
}