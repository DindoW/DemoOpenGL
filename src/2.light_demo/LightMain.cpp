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

// VAO & VBO ����
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
};

// EBO��ע��������0��ʼ 
unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,  // -Z
    4, 5, 6, 6, 7, 4,  // +z

    5, 1, 2, 2, 6, 5,  // +x
    0, 4, 7, 7, 3, 0,  // -x

    7, 6, 2, 2, 3, 7,  // +y
    0, 1, 5, 5, 4, 0,  // -y
};

// ���ڴ�С
int windowWidth = 800;
int windowHeight = 600;

// ��һ֡���λ��
double lastX = static_cast<double>(windowWidth / 2);
double lastY = static_cast<double>(windowHeight / 2);
bool firstMouse = true;

// ȫ�����
Camera ourCamera;

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

    Camera_Movement move = Camera_Movement::none;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        move = Camera_Movement::front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        move = Camera_Movement::back;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        move = Camera_Movement::left;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        move = Camera_Movement::right;

    ourCamera.SetPos(move, deltaTime);
}

// ��������ƶ�
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // ע���������෴�ģ���Ϊy�����Ǵӵײ����������������
    ourCamera.SetPitchYaw(xoffset, yoffset);

    lastX = xpos;
    lastY = ypos;
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
    glfwSetCursorPosCallback(window, mouse_callback);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ����Ϊ��Դ����������һ��VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // ֻ��Ҫ��VBO�����ٴ�����VBO�����ݣ���Ϊ���ӵ�VBO�������Ѿ���������ȷ�������嶥������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // ���õ�������Ķ�������
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ��Ⱦ�����õ�shader
    Shader ourShader(FileSystem::getPath("shaders/shader_2.vs").c_str(), FileSystem::getPath("shaders/shader_2.fs").c_str());

    // �ƹ�shader
    Shader lightingShader(FileSystem::getPath("shaders/shader_2_light.vs").c_str(), FileSystem::getPath("shaders/shader_2_light.fs").c_str());

    double deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    double lastFrame = glfwGetTime(); // ��һ֡��ʱ��
    double currentFrame = glfwGetTime(); // ��ǰ֡��ʱ��

    // �����꣬����׽���λ��
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ����ģ�;����ӽǾ���ͶӰ����
    glm::mat4 model, view, projection;

    // �Ƶ�ģ�;���
    glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, -1.0f));
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));

    // ��Ⱦѭ�������ݴ������ٶȵ��õ�Ƶ�ʻ��в�ͬ��������Ҫ����֡��
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ��������
        ourShader.use();
        view = ourCamera.GetViewMatrix();
        model = glm::mat4(1.0f);
        projection = ourCamera.GetProjectMatrix();

        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        ourShader.setMat4("projection", projection);
        ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.3f);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ���Ƶ�
        lightingShader.use();
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", lightModel);
        lightingShader.setMat4("projection", projection);
        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        // ����Ƿ��д����¼����������롢��꣩�����´���״̬
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}