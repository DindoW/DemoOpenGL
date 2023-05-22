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

    glm::vec3 model1Position(0.0f, 0.01f, -5.0f); // ģ��1λ�� ̧��һ���ֹ��ȳ�ͻ
    glm::vec3 grassPositions[] = {
        {-5.5f,  0.0f, -5.48f},
        {5.5f,  0.0f,  -6.51f},
        {6.0f,  0.0f,  -7.7f},
        {-6.3f,  0.0f, -4.3f},
        {7.5f,  0.0f, -8.6f}
    }; // ��λ��
    glm::vec3 windowPositions[] = {
        {0.0f,  0.0f, -3.49f},
        {4.5f,  0.0f,  -6.5f},
        {7.0f,  0.0f,  -7.69f},
        {-3.3f,  0.0f, -2.3f},
    }; // ����λ��
    glm::vec3 planePosition(0.0f, -1.5f, 0.0f); // �ذ�λ��
    glm::vec3 lightPosition(4.0f, 5.0f, -3.0f);  // ���Դλ��

    // ��Ⱦ�����õ�shader
    Shader objectShader(FileSystem::getPath("shaders/shader_3_obj.vs").c_str(), FileSystem::getPath("shaders/shader_2_obj.fs").c_str());
    Model cubeModel1(Mesh::CreateCube(3.0f, FileSystem::getPath("resources/marble.jpg").c_str()));

    // ������Ⱦobj���ù��ղ���
    LightParameters::MaterialParam lightMaterial(0, 1, 32.0f);
    LightParameters::DirectLight directLight(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.05f), glm::vec3(3.5f), glm::vec3(0.5f));
    LightParameters::SpotLight spotLight(glm::vec3(0), glm::vec3(0), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f, 0.5f, 0.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
    std::vector<LightParameters::PointLight> pointLights;
    pointLights.emplace_back(lightPosition);
    LightParameters allLightParams(lightMaterial, directLight, pointLights, spotLight);
    cubeModel1.SetLightParameters(objectShader, allLightParams);

    // �ƹ�shader
    Shader lightingShader(FileSystem::getPath("shaders/shader_2_light.vs").c_str(), FileSystem::getPath("shaders/shader_2_light.fs").c_str());
    Model lightCube(Mesh::CreateCube(0.5f, ""));

    // �ذ�shader
    Model plane(Mesh::CreatePlane(100.0f, glm::vec3(0, 1, 0), FileSystem::getPath("resources/metal.png").c_str()));

    // ��shader
    Shader grassShader(FileSystem::getPath("shaders/shader_3_obj.vs").c_str(), FileSystem::getPath("shaders/shader_3_obj_2.fs").c_str());
    Model grassModel(Mesh::CreatePlane(5.0f, glm::vec3(0, 0, 1), FileSystem::getPath("resources/grass.png").c_str()));

    // ��͸������shader
    Shader windowShader(FileSystem::getPath("shaders/shader_3_obj.vs").c_str(), FileSystem::getPath("shaders/shader_3_obj_3.fs").c_str());
    Model windowModel(Mesh::CreatePlane(3.0f, glm::vec3(0, 0, 1), FileSystem::getPath("resources/blending_transparent_window.png").c_str()));

    double deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
    double lastFrame = glfwGetTime(); // ��һ֡��ʱ��
    double currentFrame = glfwGetTime(); // ��ǰ֡��ʱ��

    // �����꣬����׽���λ��
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // ������
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // �������޳������޳������棬���趨��ʱ��Ϊ������
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // ����֡����
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // ����������
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


    // ������Ⱦ����
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // ���֡�����Ƿ����
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Create frame buffers error!" << std::endl;
        return 0;
    }

    // ���֡����
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ��Ⱦ֡��������Ҫ����Ƭ
    Shader frameBufferShader(FileSystem::getPath("shaders/shader_4_buffer_1.vs").c_str(), FileSystem::getPath("shaders/shader_4_buffer_1.fs").c_str());
    unsigned int bufferVAO, bufferVBO;
    glGenVertexArrays(1, &bufferVAO);
    glGenBuffers(1, &bufferVBO);
    glBindVertexArray(bufferVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bufferVBO);
    float bufferVertices[] = {
        //     ---- λ�� ----     - �������� -
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f,   // ����
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,   // ����
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,   // ����
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,   // ����
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f,   // ����
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,   // ����
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(bufferVertices), bufferVertices, GL_STATIC_DRAW);
    // ����λ��
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // ������������
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);


    // ��Ⱦѭ�������ݴ������ٶȵ��õ�Ƶ�ʻ��в�ͬ��������Ҫ����֡��
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        // �󶨵�֡�����Ͻ�����Ⱦ
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glStencilMask(0xFF);// �������ڲ�ʹ��ģ�建��
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        ModelRenderParam modelRenderParam(ourCamera);

        // ���Ƶذ�
        modelRenderParam.SetModelPosition(planePosition);
        plane.Draw(objectShader, modelRenderParam);

        // ����ģ��1
        modelRenderParam.SetModelPosition(model1Position);
        cubeModel1.UpdateLightParam(objectShader, modelRenderParam);
        cubeModel1.Draw(objectShader, modelRenderParam);

        // ���Ʋ�
        for (auto&& pos : grassPositions) {
            modelRenderParam.SetModelPosition(pos);
            grassModel.Draw(grassShader, modelRenderParam);
        }

        // ���Ƶ�
        modelRenderParam.SetModelPosition(lightPosition);
        lightCube.Draw(lightingShader, modelRenderParam);

        // ���ƴ���������������Ⱦ
        std::map<float, glm::vec3> sortedPos;
        for (auto&& pos : windowPositions) {
            float distance = glm::length(pos - ourCamera.GetPos());
            sortedPos[distance] = pos;
        }
        for (std::map<float, glm::vec3>::reverse_iterator it = sortedPos.rbegin(); it != sortedPos.rend(); it++) {
            modelRenderParam.SetModelPosition(it->second);
            windowModel.Draw(windowShader, modelRenderParam);
        }

        // �ڰ󶨵�Ĭ�ϵ�֡��������Ⱦ
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        frameBufferShader.use();

        glBindVertexArray(bufferVAO);
        glDisable(GL_DEPTH_TEST); // �����ı��αȽϼ򵥣����Բ�����Ȳ���
        glBindTexture(GL_TEXTURE_2D, texture);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // ʹ���߿�ģʽ�鿴
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        // ����Ƿ��д����¼����������롢��꣩�����´���״̬
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}