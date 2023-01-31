#pragma once

#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <mylib/shader_s.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using namespace std;
using uint = unsigned int;
using uint8 = unsigned char;


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    Vertex() = default;
    Vertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float texX, float texY)
        : Position(posX, posY, posZ),
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
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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


class Mesh {
public:
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
        0,  1,  2,  2,  3,  0,  // -Z
        4,  5,  6,  6,  7,  4,  // +Z

        8,  9,  10, 10, 11, 8,  // -X
        12, 13, 14, 14, 15, 12, // +X

        16, 17, 18, 18, 19, 16, // -Y
        20, 21, 22, 22, 23, 20, // +Y
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
    float length = lengthOfSide * 0.5f;
    vector<Vertex> vertices = {
        // Z
        Vertex(-length, -length, 0,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f),
        Vertex(length, -length, 0,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f),
        Vertex(length,  length, 0,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f),
        Vertex(-length,  length, 0,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f),
    };

    glm::vec3 zNorm(0.0f, 0.0f, -1.0f);
    glm::mat4 mat = glm::lookAt(norm, glm::vec3(0), glm::cross(zNorm, norm));
    for (auto&& vertex : vertices) {
        vertex.Position = glm::vec4(vertex.Position, 1.0f) * mat;
        vertex.Normal = norm;
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
