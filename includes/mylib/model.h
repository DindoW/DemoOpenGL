#pragma once

#include <iostream>
#include <mylib/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h> 
#include <stb_image.h>
#include <map>


uint TextureFromFile(const char* fileName, const char* filePath)
{
    uint texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    string resourceLocation = string(filePath) + '/' + string(fileName);
    uint8* data = stbi_load(resourceLocation.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format;
        switch (nrChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            cout << "Error on channels: " << nrChannels << endl;
            break;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);
    return texture;
}

class Model
{
public:
    Model(const char* path) { _loadModel(path); }
    void Draw(const Shader &shader);

private:
    map<string, Texture> mStoredTextures;  // 存放所有加载过的纹理
    vector<Mesh> mMeshes;
    string mDirectory;  // 存放模型文件所在的路径

    void _loadModel(const string &path);
    void _processNode(aiNode* node, const aiScene* scene);
    Mesh _processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> _loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

void Model::Draw(const Shader &shader)
{
    for (Mesh mesh : mMeshes)
    {
        mesh.Draw(shader);
    }
}

void Model::_loadModel(const string &path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    mDirectory = path.substr(0, path.find_last_of('/'));

    _processNode(scene->mRootNode, scene);
}

void Model::_processNode(aiNode* node, const aiScene* scene)
{
    for (uint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        mMeshes.push_back(_processMesh(mesh, scene));
    }

    for (uint i = 0; i < node->mNumChildren; i++)
    {
        _processNode(node->mChildren[i], scene);
    }
}

Mesh Model::_processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<uint> indices;
    vector<Texture> textures;

    // 处理顶点位置、法线和纹理坐标
    for (uint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // 处理索引
    for (uint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // 处理材质
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = _loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = _loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::_loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (uint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        string location(str.C_Str());
        auto res = mStoredTextures.find(location);
        if (res == mStoredTextures.end())
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), mDirectory.c_str());
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
            mStoredTextures[location] = texture;
        }
        else
        {
            textures.push_back(res->second);
        }
    }
    return textures;
}
