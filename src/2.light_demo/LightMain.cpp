#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mylib/shader_s.h"
#include "mylib/filesystem.h"
#include "mylib/camera.h"

// VAO & VBO
float vertices[] = {
    //     ---- λ�� ----
         0.5f,  0.5f, 0.0f,   // ����
         0.5f, -0.5f, 0.0f,   // ����
        -0.5f, -0.5f, 0.0f,   // ����
        -0.5f,  0.5f, 0.0f,   // ����
};

// EBO��ע��������0��ʼ 
unsigned int indices[] = {
    0, 1, 2,  // ��һ��������
    2, 3, 0,  // �ڶ���������
};

// ���ڴ�С
int windowWidth = 800;
int windowHeight = 600;

// ���ڻص�
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

// �����������
void processInput(GLFWwindow* window, const double deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // ��ʼ�����汾��
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // �߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // shader
    Shader ourShader(FileSystem::getPath("shaders/shader_2.vs").c_str(), FileSystem::getPath("shaders/shader_2.fs").c_str());

    double deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    double lastFrame = glfwGetTime(); // ��һ֡��ʱ��
    double currentFrame = glfwGetTime(); // ��ǰ֡��ʱ��

    // ��Ⱦѭ�������ݴ������ٶȵ��õ�Ƶ�ʻ��в�ͬ��������Ҫ����֡��
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        // ����Ƿ��д����¼����������롢��꣩�����´���״̬
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}