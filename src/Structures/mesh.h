#pragma once
#include <Danger.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <util/Logger.h>
#include "texture.h"
#include "shader.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

typedef unsigned int uint;

struct Vertex{
    glm::vec3 position, normal;
    glm::vec2 texture_coordinates;
    glm::vec3 tangent, bitangent;
};

struct Mesh{
    std::vector<Vertex> verts;
    std::vector<uint> inds;
    std::vector<Texture> texs;
    uint vao, vbo, eabo;
};

class MeshManager{
    public:
    static std::vector<std::string> texture_types;
    private:
    static int indexOfTextureType(const std::string type_name){
        for(int i = 0; i < texture_types.size(); i++){
            if (strcmp(type_name.c_str(), texture_types[i].c_str()) == 0){
                return i;
            }
        }
        DGR_LOG_FATAL("TEXTURE TYPE NOT FOUND: ", type_name);
        return -1;
    }
    static void setupMesh(Mesh &mesh)
    {
        // create buffers/arrays
        glGenVertexArrays(1, &(mesh.vao));
        glGenBuffers(1, &(mesh.vbo));
        glGenBuffers(1, &(mesh.eabo));

        glBindVertexArray(mesh.vao);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, mesh.verts.size() * sizeof(Vertex), &mesh.verts[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eabo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.inds.size() * sizeof(unsigned int), &mesh.inds[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texture_coordinates));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

        glBindVertexArray(0);
    }

    public:
    static void Init(){
        
    }
    static void RegisterTextureType(std::string type){
        texture_types.push_back(type);
    }
    static Mesh Generate(std::vector<Vertex> vertices, std::vector<uint> indices, std::vector<Texture> textures) {
        Mesh m = {
            vertices,
            indices,
            textures,
            0,0,0
        };
        setupMesh(m);
        return m;
    }
    static void draw(Mesh &mesh, Shader &shader)
    {
        static std::vector<uint> type_counters;
        for (std::string n : texture_types)
        {
            type_counters.push_back(1);
        }
        //loop through all textures
        for (uint i = 0; i < mesh.texs.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number = std::to_string(type_counters[indexOfTextureType(mesh.texs[i].type)]++); //sorry this is so hard to read.
                                                                                                         //the vector "type_counters" functions as a
                                                                                                         //paralell array to "texture_types" and it just holds
                                                                                                         //a count of how many times we have bound a uniform of that texture type.
                                                                                                         //this is in place of a hard coded switch statement.
                                                                                                         //im doing it this way so that i dont have to worry about adding a new
                                                                                                         //texture type to this class every time i want to use a texture for a different purpose.
                                                                                                         //instead i just regester the types as they are named in the shader at initialization and
                                                                                                         //use those as identifiers in my texture objects
            glUniform1i(glGetUniformLocation(shader, (mesh.texs[i].type + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, mesh.texs[i].handle);
        }

        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.inds.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }
};