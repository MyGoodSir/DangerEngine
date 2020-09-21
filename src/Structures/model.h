#pragma once
#include "Danger.h"
#include "util/Logger.h"
#include "shader.h"
#include "mesh.h"
#include "util/FileManip.h"

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
#include <vector>

//model data
struct Model{
    std::vector<Texture> textures;
    std::vector<Mesh> meshes;
    std::string path;
    bool gamma_correction;
};

class ModelManager{
    //load the textures for the given material
    static std::vector<Texture> getMatTextures(Model &m, aiMaterial *mat, aiTextureType type, std::string typeName){
        std::string dir = m.path.substr(0, m.path.find_last_of('/'));//this can be done in a better place...
        std::vector<Texture> ts;
        for(uint i = 0; i < mat->GetTextureCount(type); i++){
            aiString s;
            mat->GetTexture(type, i, &s);
            bool sk = false;
            for(uint j=0; j < m.textures.size(); j++){
                if(std::strcmp(m.textures[j].filepath.data(), s.C_Str()) == 0){
                    ts.push_back(m.textures[j]);
                    sk = true;
                    break;
                }
            }
            if(!sk){
                Texture t{};
                t.filepath = s.C_Str();
                t.type = typeName;
                std::string fname = dir + "/" + std::string(t.filepath);
                TextureData d = FileIOManager::loadTexture(fname.c_str());
                glGenTextures(1, &t.handle);
                if(d.data){
                    GLenum fmt;
                    if(d.component_count == 1){
                        fmt = GL_RED;
                    }
                    else if(d.component_count == 3){
                        fmt = GL_RGB;
                    }
                    else if(d.component_count == 4){
                        fmt = GL_RGBA;
                    }
                    glBindTexture(GL_TEXTURE_2D, t.handle);
                    glTexImage2D(GL_TEXTURE_2D, 0, fmt, d.width, d.height, 0, fmt, GL_UNSIGNED_BYTE, d.data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    MeshManager::RegisterTextureType(typeName);
                    FileIOManager::freeTexture(d);
                }
                else{
                    DGR_LOG_FATAL("TEXTURE NO LOAD :(");
                    FileIOManager::freeTexture(d);
                }

                ts.push_back(t);
                m.textures.push_back(t);
            }
        }
        return ts;
    }

    //parse assimp mesh data into our mesh object
    static Mesh processMesh(Model &model, aiMesh *m, const aiScene *s)
    {
        std::vector<Vertex> vt;
        std::vector<unsigned int> id;
        std::vector<Texture> tx;
        for(uint i = 0; i<m->mNumVertices; i++){
            Vertex v{};
            v.position = {m->mVertices[i].x,
                          m->mVertices[i].y,
                          m->mVertices[i].z};

            if(m->HasNormals()){
                v.normal = {m->mNormals[i].x,
                            m->mNormals[i].y,
                            m->mNormals[i].z};
            }
            if(m->mTextureCoords[0]){
                v.texture_coordinates = {m->mTextureCoords[0][i].x, m->mTextureCoords[0][i].y};

                v.tangent = {m->mTangents[i].x,
                             m->mTangents[i].y,
                             m->mTangents[i].z};

                v.bitangent = {m->mBitangents[i].x,
                               m->mBitangents[i].y,
                               m->mBitangents[i].z};
            }
            else{
                v.texture_coordinates = {0,0};
            }
            vt.push_back(v);
        }
        for (unsigned int i = 0; i < m->mNumFaces; i++)
        {
            aiFace f = m->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < f.mNumIndices; j++)
                id.push_back(f.mIndices[j]);
        }

        aiMaterial *mat = s->mMaterials[m->mMaterialIndex];

        std::vector<Texture> diffuse = getMatTextures(model, mat, aiTextureType_DIFFUSE, "t_diffuse");
        tx.insert(tx.end(), diffuse.begin(), diffuse.end());
        std::vector<Texture> specular = getMatTextures(model, mat, aiTextureType_SPECULAR, "t_specular");
        tx.insert(tx.end(), specular.begin(), specular.end());
        std::vector<Texture> normal = getMatTextures(model, mat, aiTextureType_HEIGHT, "t_normal");
        tx.insert(tx.end(), normal.begin(), normal.end());
        std::vector<Texture> height = getMatTextures(model, mat, aiTextureType_AMBIENT, "t_height");
        tx.insert(tx.end(), height.begin(), height.end());

        return MeshManager::Generate(vt, id, tx);
    }

    //recursively process all of the model data
    static void processNodes(Model &model, aiNode* n, const aiScene* s){
        for(uint i = 0; i < n->mNumMeshes; i++){
            aiMesh* m = s->mMeshes[n->mMeshes[i]];
            model.meshes.push_back(processMesh(model, m, s));
        }

        for(uint i = 0; i<n->mNumChildren; i++){
            processNodes(model, n->mChildren[i], s);
        }
    }

    public:
    static void Init(){}
    //create a new model
    static Model Create(std::string path, bool gamma = false){
        Model m = {{},{},path,gamma};
        Assimp::Importer imp;
        const aiScene* scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            DGR_LOG_FATAL("ASSET IMPORT ERROR: ", imp.GetErrorString());
            return {{},{},"",false};
        }

        processNodes(m, scene->mRootNode, scene);
        return m;
    }
    //draw model
    static void Draw(Model model, Shader &shader){
        for(Mesh& m : model.meshes){
            MeshManager::draw(m, shader);
        }
    }
};