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

        // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
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

    void SetLightParameters(Shader& objectShader, LightParameters& lightParams);
    void Draw(Shader &shader, ModelRenderParam& modelRenderParam);

private:
    map<string, Texture> mStoredTextures;  // ������м��ع�������
    vector<Mesh> mMeshes;
    string mDirectory;  // ���ģ���ļ����ڵ�·��

    void _loadModel(const string &path);
    void _processNode(aiNode* node, const aiScene* scene);
    Mesh _processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> _loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

void Model::SetLightParameters(Shader& objectShader, LightParameters& lightParams) {
    objectShader.use();
    objectShader.setInt("material.diffuse", lightParams.mMaterial.mDiffuse);
    objectShader.setInt("material.specular", lightParams.mMaterial.mSpecular);
    objectShader.setFloat("material.shininess", lightParams.mMaterial.mShininess);
    
    objectShader.setVec3("dirLight.direction", lightParams.mDirectLight.mDirection);
    objectShader.setVec3("dirLight.ambient", lightParams.mDirectLight.mAmbient);
    objectShader.setVec3("dirLight.diffuse", lightParams.mDirectLight.mDiffuse);
    objectShader.setVec3("dirLight.specular", lightParams.mDirectLight.mSpecular);
    
    objectShader.setVec3("spotLight.position", lightParams.mSpotLight.mPosition);
    objectShader.setVec3("spotLight.direction", lightParams.mSpotLight.mDirection);
    objectShader.setVec3("spotLight.ambient", lightParams.mSpotLight.mAmbient);
    objectShader.setVec3("spotLight.diffuse", lightParams.mSpotLight.mDiffuse);
    objectShader.setVec3("spotLight.specular", lightParams.mSpotLight.mSpecular);
    objectShader.setFloat("spotLight.constant", lightParams.mSpotLight.mConstant);
    objectShader.setFloat("spotLight.linear", lightParams.mSpotLight.mLinear);
    objectShader.setFloat("spotLight.quadratic", lightParams.mSpotLight.mQuadratic);
    objectShader.setFloat("spotLight.cutOff", lightParams.mSpotLight.mCutOff);
    objectShader.setFloat("spotLight.outerCutOff", lightParams.mSpotLight.mOuterCutOff);

    for (int i = 0; i < lightParams.mPointLights.size(); i++) {
        auto&& pointLight = lightParams.mPointLights[i];
        std::string pointName = "pointLights[" + to_string(i) + "]";
        objectShader.setVec3(pointName + ".position", pointLight.mPosition);
        objectShader.setFloat(pointName + ".constant", pointLight.mConstant);
        objectShader.setFloat(pointName + ".linear", pointLight.mLinear);
        objectShader.setFloat(pointName + ".quadratic", pointLight.mQuadratic);
        objectShader.setVec3(pointName + ".ambient", pointLight.mAmbient);
        objectShader.setVec3(pointName + ".diffuse", pointLight.mDiffuse);
        objectShader.setVec3(pointName + ".specular", pointLight.mSpecular);
    }
    objectShader.setInt("pointLightCount", lightParams.mPointLights.size());
}

void Model::Draw(Shader &shader, ModelRenderParam& modelRenderParam)
{
    // ��������
    shader.use();
    shader.setMat4("view", modelRenderParam.mViewMat);
    shader.setMat4("projection", modelRenderParam.mProjMat);
    shader.setVec3("viewPos", modelRenderParam.mCameraPos);
    shader.setVec3("spotLight.position", modelRenderParam.mCameraPos);
    shader.setVec3("spotLight.direction", modelRenderParam.mCameraDir);
    shader.setMat4("model", modelRenderParam.mModelTransMat);

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

    // ������λ�á����ߺ���������
    for (uint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // ��������
    for (uint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // �������
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
