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
glm::vec3 modelPositions[] = {
    glm::vec3(0.0f,  0.0f,  -5.0f)
};

// ���Դλ��
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
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
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO); // ֻ��Ҫ��VBO�����ٴ�����VBO�����ݣ���Ϊ���ӵ�VBO�������Ѿ���������ȷ�������嶥������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);  // ���õ�������Ķ�������
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ��Ⱦ�����õ�shader
    Shader objectShader(FileSystem::getPath("shaders/shader_2_obj.vs").c_str(), FileSystem::getPath("shaders/shader_2_obj.fs").c_str());
    // �ƹ�shader
    Shader lightingShader(FileSystem::getPath("shaders/shader_2_light.vs").c_str(), FileSystem::getPath("shaders/shader_2_light.fs").c_str());

    objectShader.use();
    objectShader.setInt("material.diffuse", 0);
    objectShader.setInt("material.specular", 1);
    objectShader.setFloat("material.shininess", 32.0f);
    // directLight
    objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    objectShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    objectShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // pointLight
    objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    objectShader.setFloat("pointLights[0].constant", 1.0f);
    objectShader.setFloat("pointLights[0].linear", 0.09f);
    objectShader.setFloat("pointLights[0].quadratic", 0.032f);
    objectShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    objectShader.setFloat("pointLights[1].constant", 1.0f);
    objectShader.setFloat("pointLights[1].linear", 0.09f);
    objectShader.setFloat("pointLights[1].quadratic", 0.032f);
    objectShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    objectShader.setFloat("pointLights[2].constant", 1.0f);
    objectShader.setFloat("pointLights[2].linear", 0.09f);
    objectShader.setFloat("pointLights[2].quadratic", 0.032f);
    objectShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    objectShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    objectShader.setFloat("pointLights[3].constant", 1.0f);
    objectShader.setFloat("pointLights[3].linear", 0.09f);
    objectShader.setFloat("pointLights[3].quadratic", 0.032f);
    objectShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    objectShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    objectShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    // spotLight
    objectShader.setVec3("spotLight.position", ourCamera.GetPos());
    objectShader.setVec3("spotLight.direction", ourCamera.GetDir());
    objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.09f);
    objectShader.setFloat("spotLight.quadratic", 0.032f);
    objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

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

        // �ӽǾ���ͶӰ����ģ�;���
        const glm::mat4& view = ourCamera.GetViewMatrix();
        const glm::mat4& proj = ourCamera.GetProjectMatrix();
        const glm::mat4 objModel(1.0f);

        // ��������
        objectShader.use();
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", proj);
        objectShader.setVec3("spotLight.position", ourCamera.GetPos());
        objectShader.setVec3("spotLight.direction", ourCamera.GetDir());
        objectShader.setVec3("viewPos", ourCamera.GetPos());

        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 model = glm::translate(objModel, modelPositions[i]);
            objectShader.setMat4("model", model);
            myModel.Draw(objectShader);
        }

        // ���Ƶ�
        lightingShader.use();
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", ourCamera.GetProjectMatrix());

        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 lightModel = glm::translate(objModel, pointLightPositions[i]);
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