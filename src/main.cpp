#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "core/shader.h"
#include "helper/stb_image.h"
#include "helper/matrix.h"
#include "core/model.h"

#include "core/camera.h"
#include "shapes/cube.h"

constexpr int kWindowWidth = 1200;
constexpr int kWindowHeight = 800;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// 각 방향키가 마지막으로 "눌린" 시각
double wPressTime = -1.0, sPressTime = -1.0;
double aPressTime = -1.0, dPressTime = -1.0;

Camera camera(Vec3(0.0f, 0.0f, 3.0f));

float lastX = kWindowWidth / 2.0f;
float lastY = kWindowHeight / 2.0f;
bool firstMouse = true;

void OnMouseMoveCallback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 반전

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void OnFramebufferSizeChange(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void OnKeyEventCallback(GLFWwindow *window,
                        int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (action != GLFW_PRESS)
        return; // 최초로 눌린 시간만 기록

    double now = glfwGetTime();
    if (key == GLFW_KEY_W)
        wPressTime = now;
    if (key == GLFW_KEY_S)
        sPressTime = now;
    if (key == GLFW_KEY_A)
        aPressTime = now;
    if (key == GLFW_KEY_D)
        dPressTime = now;
}

void ProcessInput(GLFWwindow *window)
{
    bool wHeld = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool sHeld = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool aHeld = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool dHeld = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    // 전/후 축: 둘 다 눌려있으면 더 나중에 눌린 쪽 우선
    if (wHeld && sHeld)
    {
        if (wPressTime > sPressTime)
            camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
        else
            camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    }
    else if (wHeld)
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);

    else if (sHeld)
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);

    // 좌/우 축도 동일하게
    if (aHeld && dHeld)
    {
        if (aPressTime > dPressTime)
            camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
        else
            camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
    }
    else if (aHeld)
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    else if (dHeld)
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

int main(int argc, char const *argv[])
{
    if (!glfwInit())
    {
        const char *description = nullptr;
        glfwGetError(&description);
        std::cerr << "Failed to initialize GLFW" << description << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE); // mac retina display

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(kWindowWidth, kWindowHeight, "window",
                                          nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;

        glfwTerminate();
        return -1;
    }
    // 창을 만들고 거기에 생성된 컨텍스트를 사용하겠다는 코드
    glfwMakeContextCurrent(window);

    // hides cursor and makes it move infinitely
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;

    // Get actual framebuffer size (handles Retina displays)
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glEnable(GL_DEPTH_TEST);

    stbi_set_flip_vertically_on_load(true);

    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEventCallback);
    glfwSetCursorPosCallback(window, OnMouseMoveCallback);

    Shader shader = Shader("../src/shaders/basic.vert", "../src/shaders/basic.frag");
    Cube cube = Cube();

    glfwPollEvents(); // reset the delta value for mouse while loading a model
    firstMouse = true;
    lastFrame = static_cast<float>(glfwGetTime());
    
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        shader.use();

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        const float aspect = (fbH > 0) ? static_cast<float>(fbW) / static_cast<float>(fbH)
                                       : 1.0f;

        Mat4 model = Mat4();
        Mat4 view = camera.GetViewMatrix();
        Mat4 proj = Perspective(toRadian(camera.Zoom), aspect, 0.1f, 100.0f);

        shader.setMat4("uModel", model);
        shader.setMat4("uView", view);
        shader.setMat4("uProj", proj);

        cube.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents(); // 이벤트 자동으로 수집
    }

    glDeleteProgram(shader.ID);
    glfwTerminate();
    return 0;
}
