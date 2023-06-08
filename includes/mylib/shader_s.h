#pragma once

// 包含glad来获取所有的必须OpenGL头文件
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <mylib/camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct LightParameters
{
    struct MaterialParam
    {
    public:
        int mDiffuse;
        int mSpecular;
        float mShininess;
        MaterialParam() = delete;
        MaterialParam(int diffuse, int specular, float shininess)
            : mDiffuse(diffuse), mSpecular(specular), mShininess(shininess) {

        }
    };

    struct DirectLight
    {
    public:
        glm::vec3 mDirection;
        glm::vec3 mAmbient;
        glm::vec3 mDiffuse;
        glm::vec3 mSpecular;

        DirectLight() = delete;
        DirectLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
            :mDirection(direction), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular) {
        }
    };

    struct PointLight
    {
    public:
        glm::vec3 mPosition;
        float mConstant;
        float mLinear;
        float mQuadratic;
        glm::vec3 mAmbient;
        glm::vec3 mDiffuse;
        glm::vec3 mSpecular;

        PointLight() = delete;
        PointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
            : mPosition(position),
            mConstant(constant),
            mLinear(linear),
            mQuadratic(quadratic),
            mAmbient(ambient),
            mDiffuse(diffuse),
            mSpecular(specular) {
        }
        PointLight(glm::vec3 position)
            : PointLight(position, 1.0f, 0.09f, 0.032f, glm::vec3(0.05f), glm::vec3(1.8f, 0.0f, 0.0f), glm::vec3(1.0f)) {
        }
    };

    struct SpotLight
    {
    public:
        glm::vec3 mPosition;
        glm::vec3 mDirection;
        glm::vec3 mAmbient;
        glm::vec3 mDiffuse;
        glm::vec3 mSpecular;
        float mConstant;
        float mLinear;
        float mQuadratic;
        float mCutOff;
        float mOuterCutOff;

        SpotLight() = delete;
        SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
            float constant, float linear, float quadratic, float cutOff, float outerCutOff)
            : mPosition(position), mDirection(direction), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular),
            mConstant(constant), mLinear(linear), mQuadratic(quadratic), mCutOff(cutOff), mOuterCutOff(outerCutOff) {
        }
    };

    MaterialParam mMaterial;
    DirectLight mDirectLight;
    std::vector<PointLight> mPointLights;
    SpotLight mSpotLight;

    LightParameters(MaterialParam& material, DirectLight& directLight, std::vector<PointLight>& pointLights, SpotLight& spotLight)
        : mMaterial(material), mDirectLight(directLight), mSpotLight(spotLight) {
        int pointCount = std::min(pointLights.size(), (size_t)4);
        if (pointCount == 0) {
            std::cout << "Must contain a point param at least" << std::endl;
            return;
        }
        for (int i = 0; i < pointCount; i++) {
            mPointLights.emplace_back(pointLights[i]);
        }
    }
};


struct ModelRenderParam
{
public:
    glm::mat4 mViewMat;
    glm::mat4 mProjMat;
    glm::mat4 mModelTransMat;

    glm::vec3 mCameraPos;
    glm::vec3 mCameraDir;

    ModelRenderParam(Camera& camera) {
        mViewMat = camera.GetViewMatrix();
        mProjMat = camera.GetProjectMatrix();
        mModelTransMat = glm::mat4(1.0f);
        mCameraPos = camera.GetPos();
        mCameraDir = camera.GetDir();
    }

    void SetModelPosition(glm::vec3& modelPos) {
        mModelTransMat = glm::translate(glm::mat4(1.0f), modelPos);
    }

    void SetModelScale(float scale) {
        mModelTransMat = glm::scale(mModelTransMat, glm::vec3(scale));
    }
};


class Shader
{
public:
    // shader程序ID
    unsigned int mID;

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
    // utility function for checking shader compilation/linking errors.
    void checkCompileErrors(unsigned int shader, std::string type);
};


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. 编译着色器
    unsigned int vertex, fragment;

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 打印编译错误（如果有的话）
    checkCompileErrors(vertex, "VERTEX");

    // 片段着色器也类似
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 着色器程序
    mID = glCreateProgram();
    glAttachShader(mID, vertex);
    glAttachShader(mID, fragment);
    glLinkProgram(mID);
    // 打印连接错误（如果有的话）
    checkCompileErrors(mID, "PROGRAM");

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(mID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
