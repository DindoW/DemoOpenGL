#pragma once

#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mylib/shader_s.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>


using namespace std;
using uint = unsigned int;
using uint8 = unsigned char;


struct SimpleVertex {
    glm::vec3 Position;

    SimpleVertex() = default;
    SimpleVertex(float posX, float posY, float posZ)
        : Position(posX, posY, posZ) {

    }
};

struct Vertex : SimpleVertex {
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    Vertex() = default;
    Vertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float texX, float texY) :
        SimpleVertex(posX, posY, posZ),
        Normal(normX, normY, normZ),
        TexCoords(texX, texY) {

    }
};


struct Texture {
    uint id;
    string type;
    aiString path;  // 我们储存纹理的路径用于与其它纹理进行比较

    Texture() = default;
    Texture(uint tID, string tType, string tPath) :
        id(tID), type(tType), path(tPath) {
    }
};


uint TextureFromFile(const char* fileName, const char* filePath = nullptr)
{
    uint texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    string resourceLocation;
    if (filePath) {
        resourceLocation = string(filePath) + '/' + string(fileName);
    }
    else {
        resourceLocation = string(fileName);
    }
     
    uint8* data = stbi_load(resourceLocation.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = GL_ZERO;
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
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);
    return texture;
}

class SkyBoxMesh {
public:
    SkyBoxMesh(const vector<SimpleVertex>& vertices, const vector<uint>& indices, const unsigned int& texture) :
        mVertices(vertices),
        mIndices(indices),
        mTextureID(texture)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(SimpleVertex), &mVertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
        // 顶点位置
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)0);

        glBindVertexArray(0);
    }

    void Draw(Shader& shader, ModelRenderParam& modelRenderParam) {
        glDepthMask(GL_FALSE);
        shader.use();
        // 设置观察和投影矩阵
        shader.setMat4("view", glm::mat4(glm::mat3(modelRenderParam.mViewMat)));
        shader.setMat4("projection", modelRenderParam.mProjMat);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
    }

private:
    // 网格数据
    vector<SimpleVertex> mVertices;
    vector<unsigned int> mIndices;
    unsigned int mTextureID;

    // 渲染数据
    unsigned int VAO, VBO, EBO;
};

class Mesh {
public:

    static SkyBoxMesh CreateSkyBox(const string& textureFolderPath);
    static Mesh CreateCube(float lengthOfSide, const string& texturePath);
    static Mesh CreatePlane(float lengthOfSide, glm::vec3& norm, const string& texturePath);

    // 网格数据
    vector<Vertex> mVertices;
    vector<unsigned int> mIndices;
    vector<Texture> mTextures;

    // 函数
    Mesh() = default;
    Mesh(const vector<Vertex>& vertices, const vector<uint>& indices, const vector<Texture>& textures);
    void Draw(const Shader &shader);
private:
    // 渲染数据
    unsigned int VAO, VBO, EBO;
    // 函数
    void _setupMesh();
};

SkyBoxMesh Mesh::CreateSkyBox(const string& textureFolderPath) {
    float length = 1.0f;
    vector<SimpleVertex> vertices = {
        // -Z
        SimpleVertex(-length, -length, -length),
        SimpleVertex(length, -length, -length),
        SimpleVertex(length,  length, -length),
        SimpleVertex(-length,  length, -length),

        // +Z
        SimpleVertex(-length, -length,  length),
        SimpleVertex(length, -length,  length),
        SimpleVertex(length,  length,  length),
        SimpleVertex(-length,  length,  length),
    };

    vector<uint> indices = {
        0,  1,  2,  2,  3,  0,  // -Z
        4,  7,  6,  6,  5,  4,  // +Z

        4,  0,  3,  3,  7,  4,  // -X
        1,  5,  6,  6,  2,  1, // +X

        0,  4,  5,  5,  1,  0, // -Y
        3,  2,  6,  6,  7,  3, // +Y
    };

    assert(textureFolderPath.length() > 0, "sky box need a texture!");

    vector<std::string> faces;
    faces.emplace_back(textureFolderPath + "/right.jpg");
    faces.emplace_back(textureFolderPath + "/left.jpg");
    faces.emplace_back(textureFolderPath + "/top.jpg");
    faces.emplace_back(textureFolderPath + "/bottom.jpg");
    faces.emplace_back(textureFolderPath + "/front.jpg");
    faces.emplace_back(textureFolderPath + "/back.jpg");

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return SkyBoxMesh(vertices, indices, textureID);
}

Mesh Mesh::CreateCube(float lengthOfSide, const string& texturePath) {
    if (lengthOfSide <= 0.0f) {
        return Mesh();
    }
    float length = lengthOfSide * 0.5f;
    vector<Vertex> vertices = {
        // -Z
        Vertex(-length, -length, -length,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f),
        Vertex(length, -length, -length,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f),
        Vertex(length,  length, -length,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f),
        Vertex(-length,  length, -length,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f),

        // +Z
        Vertex(-length, -length,  length,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f),
        Vertex(length, -length,  length,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f),
        Vertex(length,  length,  length,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f),
        Vertex(-length,  length,  length,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f),

        // -X
        Vertex(-length,  length,  length, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f),
        Vertex(-length,  length, -length, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f),
        Vertex(-length, -length, -length, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f),
        Vertex(-length, -length,  length, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f),

        // +X
        Vertex(length,  length,  length,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f),
        Vertex(length,  length, -length,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f),
        Vertex(length, -length, -length,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f),
        Vertex(length, -length,  length,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f),

        // -Y
        Vertex(-length, -length, -length,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f),
        Vertex(length, -length, -length,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f),
        Vertex(length, -length,  length,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f),
        Vertex(-length, -length,  length,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f),

        // +Y
        Vertex(-length,  length, -length,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f),
        Vertex(length,  length, -length,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f),
        Vertex(length,  length,  length,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f),
        Vertex(-length,  length,  length,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f),
    };

    vector<uint> indices = {
        0,  3,  2,  2,  1,  0,  // -Z
        4,  5,  6,  6,  7,  4,  // +Z

        8,  9,  10, 10, 11, 8,  // -X
        12, 15, 14, 14, 13, 12, // +X

        16, 17, 18, 18, 19, 16, // -Y
        20, 23, 22, 22, 21, 20, // +Y
    };

    vector<Texture> textures;

    if (texturePath.length() > 0) {
        textures.emplace_back(TextureFromFile(texturePath.c_str()), "texture_diffuse", texturePath.c_str());
    }

    return Mesh(vertices, indices, textures);
}

Mesh Mesh::CreatePlane(float lengthOfSide, glm::vec3& norm, const string& texturePath) {
    if (lengthOfSide <= 0.0f) {
        return Mesh();
    }
    glm::vec3 normal = glm::normalize(norm);
    float length = lengthOfSide * 0.5f;
    vector<Vertex> vertices = {
        // Z
        Vertex(-length, -length, 0,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f),
        Vertex(length, -length, 0,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f),
        Vertex(length,  length, 0,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f),
        Vertex(-length,  length, 0,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f),
    };

    glm::vec3 zNorm(0.0f, 0.0f, 1.0f);
    glm::vec3 axis = -glm::cross(zNorm, normal);
    float cosValue = glm::dot(zNorm, normal);

    if (axis != glm::vec3(0) && cosValue != 1.0f) {
        auto mat = glm::rotate(glm::mat4(1.0f), glm::acos(cosValue), axis);
        for (auto&& vertex : vertices) {
            vertex.Position = glm::vec4(vertex.Position, 1.0f) * mat;
            vertex.Normal = normal;
        }
    }

    vector<uint> indices = {
        0,  1,  2,  2,  3,  0,  // Z
    };

    vector<Texture> textures;

    if (texturePath.length() > 0) {
        textures.emplace_back(TextureFromFile(texturePath.c_str()), "texture_diffuse", texturePath.c_str());
    }

    return Mesh(vertices, indices, textures);
}

Mesh::Mesh(const vector<Vertex> &vertices, const vector<uint> &indices, const vector<Texture> &textures):
    mVertices(vertices),
    mIndices(indices),
    mTextures(textures) {
    _setupMesh();
}

void Mesh::Draw(const Shader &shader)
{
    uint diffuseNr = 1;
    uint specularNr = 1;
    for (uint i = 0; i < mTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        string number;
        string name = mTextures[i].type;
        if (name == "texture_diffuse")
        {
            number = to_string(diffuseNr++);
        }
        else if (name == "texture_specular")
        {
            number = to_string(specularNr++);
        }

        shader.setInt(("material." + name + number).c_str(), i); // 告诉OpenGL每个采样器属于哪个纹理单元
        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);  // 绑定这个纹理到当前激活的纹理单元
    }
    glActiveTexture(GL_TEXTURE0);

    // 绘制网格
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::_setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
