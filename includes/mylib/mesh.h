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
    aiString path;  // ���Ǵ��������·������������������бȽ�
};

class Mesh {
public:
    // ��������
    vector<Vertex> mVertices;
    vector<unsigned int> mIndices;
    vector<Texture> mTextures;

    // ����
    Mesh(const vector<Vertex>& vertices, const vector<uint>& indices, const vector<Texture>& textures);
    //~Mesh() {
    // ������������ã���ΪֻҪ��һ������������������ָ��ͬһ��VAO��Mesh���ò�����
    //    glDeleteVertexArrays(1, &VAO);
    //    glDeleteBuffers(1, &VBO);
    //    glDeleteBuffers(1, &EBO);
    //}
    void Draw(const Shader &shader);
private:
    // ��Ⱦ����
    unsigned int VAO, VBO, EBO;
    // ����
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
        glActiveTexture(GL_TEXTURE0 + i); // �ڰ�֮ǰ������Ӧ������Ԫ
        // ��ȡ������ţ�diffuse_textureN �е� N��
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

        shader.setInt(("material." + name + number).c_str(), i); // ����OpenGLÿ�������������ĸ�����Ԫ
        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);  // �����������ǰ���������Ԫ
    }
    glActiveTexture(GL_TEXTURE0);

    // ��������
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
    // ����λ��
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // ���㷨��
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // ������������
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
