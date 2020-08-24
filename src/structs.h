/*\
 *
 * Temp file for POD structures. 
 * TODO: break this into multiple files that hold the structure (and associated functions?)
 * 
\*/
#pragma once
#include "Danger.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <tuple>
#include <bitset>
namespace DGREngine::structs{
    typedef unsigned int uint;
    typedef unsigned char byte;
    enum BufferType{
        VERT, IND, DATA, CMD, ARRAY, ATTRIB, EMPTY
    };
    struct Buffer{
        uint handle;
        BufferType type;
    };
    struct AttributeInfo{
        enum DataType{
            UINT = GL_UNSIGNED_INT, FLOAT = GL_FLOAT
        }type;
        uint count, offset;
    };
    struct VAO{
        uint handle, attribute_count, vertex_stride;
        std::vector<AttributeInfo> attribs;
        Buffer vertex_buffer, index_buffer;
    };
    struct TextureData{
        uint handle;
        int width, height, channels;
        int sampler_slot;
        byte* data;
    };
    

    ///////////////////////
    /*
     *
     * Events are functors
     *  -window event
     *  -keypress event
     *  -mouse event
     *  -game events (collision event, creation event, impulse event)
     *  -render event?
     * 
     * API heirarchy
     *  -Window
     *   -Rendering Device
     *    -Context (system state)
     *      -bound buffers
     *      -attribute arrays
     *      -uniforms
     *      -shaders
     *      -stencil
     *      -depth buffer
     *      -AA settings
     *      -rendering type (forward or deffered, raytraced or rasterized)
     *    -Objects (data)
     *      -camera (projection matrix, view matrix, camera position and orientation, ortho/perspective)
     *      -buffers
     * 
     */
    ///////////////////////
}