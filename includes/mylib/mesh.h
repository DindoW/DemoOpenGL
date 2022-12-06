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
};

struct Texture {
    uint id;
    string type;
    aiString path;  // 我们储存纹理的路径用于与其它纹理进行比较
};

class Mesh {
public:
    // 网格数据
    vector<Vertex> mVertices;
    vector<unsigned int> mIndices;
    vector<Texture> mTextures;

    // 函数
    Mesh(const vector<Vertex>& vertices, const vector<uint>& indices, const vector<Texture>& textures);
    //~Mesh() {
    // 不能在这里调用，因为只要有一个拷贝对象被析构，则指向同一个VAO的Mesh都用不了了
    //    glDeleteVertexArrays(1, &VAO);
    //    glDeleteBuffers(1, &VBO);
    //    glDeleteBuffers(1, &EBO);
    //}
    void Draw(const Shader &shader);
private:
    // 渲染数据
    unsigned int VAO, VBO, EBO;
    // 函数
    void _setupMesh();
};

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
