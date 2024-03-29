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


// 窗口大小
int windowWidth = 800;
int windowHeight = 600;

// 上一帧鼠标位置
double lastX = static_cast<double>(windowWidth / 2);
double lastY = static_cast<double>(windowHeight / 2);
bool firstMouse = true;

// 全局相机
Camera ourCamera;

// 窗口回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

// 处理键盘输入
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

// 处理鼠标移动
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    ourCamera.SetPitchYaw(xoffset, yoffset);

    lastX = xpos;
    lastY = ypos;
}


int main()
{
    // 初始化，版本号
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

    glm::vec3 model1Position(0.0f, 0.01f, -5.0f); // 模型1位置 抬高一点防止深度冲突
    glm::vec3 model2Position(3.0f, 0.0f, -9.0f); // 模型2位置
    glm::vec3 planePosition(0.0f, -1.5f, 0.0f); // 地板位置
    glm::vec3 lightPosition(4.0f, 5.0f, -3.0f);  // 点光源位置

    // 渲染物体用的shader
    Shader objectShader(FileSystem::getPath("shaders/shader_3_obj.vs").c_str(), FileSystem::getPath("shaders/shader_2_obj.fs").c_str());
    // 渲染物体边框用地shader
    Shader outlineShader(FileSystem::getPath("shaders/shader_3_obj.vs").c_str(), FileSystem::getPath("shaders/shader_3_obj_1.fs").c_str());
    Model cubeModel1(Mesh::CreateCube(3.0f, FileSystem::getPath("resources/marble.jpg").c_str()));
    Model cubeModel2(cubeModel1);

    LightParameters::MaterialParam lightMaterial(0, 1, 32.0f);
    LightParameters::DirectLight directLight(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.05f), glm::vec3(3.5f), glm::vec3(0.5f));
    LightParameters::SpotLight spotLight(glm::vec3(0), glm::vec3(0), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f, 0.5f, 0.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
    std::vector<LightParameters::PointLight> pointLights;
    pointLights.emplace_back(lightPosition);
    LightParameters allLightParams(lightMaterial, directLight, pointLights, spotLight);

    // 给待渲染obj设置光照参数
    cubeModel1.SetLightParameters(objectShader, allLightParams);
    cubeModel2.SetLightParameters(objectShader, allLightParams);

    // 灯光shader
    Shader lightingShader(FileSystem::getPath("shaders/shader_2_light.vs").c_str(), FileSystem::getPath("shaders/shader_2_light.fs").c_str());
    Model lightCube(Mesh::CreateCube(0.5f, ""));

    // 地板shader
    Model plane(Mesh::CreatePlane(100.0f, glm::vec3(0, 1, 0), FileSystem::getPath("resources/metal.png").c_str()));


    double deltaTime = 0.0f; // 当前帧与上一帧的时间差
    double lastFrame = glfwGetTime(); // 上一帧的时间
    double currentFrame = glfwGetTime(); // 当前帧的时间

    // 捕获光标，并捕捉光标位置
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 允许深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 允许模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // 渲染循环，根据处理器速度调用的频率会有不同，所以需要限制帧率
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        ModelRenderParam modelRenderParam(ourCamera);

        // 绘制地板
        glStencilMask(0x00);
        modelRenderParam.SetModelPosition(planePosition);
        plane.Draw(objectShader, modelRenderParam);

        // 绘制模型1
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT); // 需要在开启写入后才能清空
        modelRenderParam.SetModelPosition(model1Position);
        cubeModel1.UpdateLightParam(objectShader, modelRenderParam);
        cubeModel1.Draw(objectShader, modelRenderParam);
        // 绘制模型1边框
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        modelRenderParam.SetModelScale(1.05);
        cubeModel1.Draw(outlineShader, modelRenderParam);

        // 绘制模型2
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);
        modelRenderParam.SetModelPosition(model2Position);
        cubeModel2.UpdateLightParam(objectShader, modelRenderParam);
        cubeModel2.Draw(objectShader, modelRenderParam);
        // 绘制模型2边框
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        modelRenderParam.SetModelScale(1.05);
        cubeModel2.Draw(outlineShader, modelRenderParam);

        // 绘制灯
        modelRenderParam.SetModelPosition(lightPosition);
        lightCube.Draw(lightingShader, modelRenderParam);

        glfwSwapBuffers(window);
        // 检查是否有触发事件（键盘输入、鼠标）、更新窗口状态
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}