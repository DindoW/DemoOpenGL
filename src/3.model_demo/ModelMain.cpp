#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <mylib/shader_s.h>
#include <mylib/camera.h>
#include <mylib/filesystem.h>
#include <mylib/mesh.h>
#include <mylib/model.h>


// ���ڴ�С
int windowWidth = 800;
int windowHeight = 600;

// ��һ֡���λ��
double lastX = static_cast<double>(windowWidth / 2);
double lastY = static_cast<double>(windowHeight / 2);
bool firstMouse = true;

// ģ��λ��
glm::vec3 modelPosition(0.0f,  0.0f,  -5.0f);

// ���Դλ��
glm::vec3 pointLightPositions[] = {
    glm::vec3(-4.f,  12.0f,  -5.0f),
    glm::vec3(4.0f, 12.0f, -5.0f),
    glm::vec3(-4.0f,  5.0f, -7.0f),
    glm::vec3(4.0f,  5.0f, -3.0f)
};

float vertices[] = {
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     -1.0f,  1.0f, -1.0f,

    -1.0f,  -1.0f, 1.0f,
    1.0f,  -1.0f, 1.0f,
    1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, 1.0f,
};

unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,  // -Z
    5, 4, 7, 7, 6, 5,  // +z

    1, 5, 6, 6, 2, 1,  // +x
    4, 0, 3, 3, 7, 4,  // -x

    6, 7, 3, 3, 2, 6,  // +y
    0, 4, 5, 5, 1, 0,  // -y
};

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

    std::uint8_t move = static_cast<std::uint8_t>(Camera_Movement::none);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        move |= static_cast<std::uint8_t>(Camera_Movement::front);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        move |= static_cast<std::uint8_t>(Camera_Movement::back);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        move |= static_cast<std::uint8_t>(Camera_Movement::left);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        move |= static_cast<std::uint8_t>(Camera_Movement::right);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        move |= static_cast<std::uint8_t>(Camera_Movement::up);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        move |= static_cast<std::uint8_t>(Camera_Movement::down);

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

    // ����ģ��
    Model myModel(FileSystem::getPath("resources/models/Nanosuit/nanosuit.obj").c_str());

    // ����Ϊ��Դ����������һ��VAO
    unsigned int lightVAO, lightVBO, lightEBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ��Ⱦ�����õ�shader
    Shader objectShader(FileSystem::getPath("shaders/shader_2_obj.vs").c_str(), FileSystem::getPath("shaders/shader_2_obj.fs").c_str());
    // �ƹ�shader
    Shader lightingShader(FileSystem::getPath("shaders/shader_2_light.vs").c_str(), FileSystem::getPath("shaders/shader_2_light.fs").c_str());

    LightParameters::MaterialParam lightMaterial(0, 1, 32.0f);
    LightParameters::DirectLight directLight(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.05f), glm::vec3(3.5f), glm::vec3(0.5f));
    LightParameters::SpotLight spotLight(glm::vec3(0), glm::vec3(0), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f, 0.5f, 0.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
    std::vector<LightParameters::PointLight> pointLights;
    for (auto&& pos : pointLightPositions) {
        pointLights.emplace_back(pos);
    }

    LightParameters allLightParams(lightMaterial, directLight, pointLights, spotLight);
    
    myModel.SetLightParameters(objectShader, allLightParams);

    double deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    double lastFrame = glfwGetTime(); // ��һ֡��ʱ��
    double currentFrame = glfwGetTime(); // ��ǰ֡��ʱ��

    // �����꣬����׽���λ��
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ��Ⱦѭ�������ݴ������ٶȵ��õ�Ƶ�ʻ��в�ͬ��������Ҫ����֡��
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ����ģ��
        ModelRenderParam modelRenderParam(ourCamera, modelPosition);
        myModel.Draw(objectShader, modelRenderParam);

        // ���Ƶ�
        lightingShader.use();
        lightingShader.setMat4("view", ourCamera.GetViewMatrix());
        lightingShader.setMat4("projection", ourCamera.GetProjectMatrix());

        glBindVertexArray(lightVAO);
        for (auto&& pos : pointLightPositions) {
            glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), pos);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            lightingShader.setMat4("model", lightModel);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        // ����Ƿ��д����¼����������롢��꣩�����´���״̬
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteBuffers(1, &lightEBO);
    glfwTerminate();
    return 0;
}