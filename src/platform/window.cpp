#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "platform/window.h"

#include <iostream>
#include <stdexcept>
#include <string>

Window::Window(int width, int height, const char *title)
{
    if (!glfwInit())
    {
        const char *description = nullptr;
        glfwGetError(&description);
        throw std::runtime_error(std::string("Failed to initialize GLFW: ") + (description ? description : ""));
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Retina framebuffers stay at the default (enabled). Disabling them via GLFW_COCOA_RETINA_FRAMEBUFFER
    // is unreliable on macOS once a window moves between displays: the OS silently switches the backing
    // store to 2x while GLFW keeps reporting 1x, and the viewport ends up covering a quarter of the window.
    // Always size viewports and FBOs from GetFramebufferSize(), never from the window size.
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    mWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!mWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(mWindow);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::string msg = std::string("Failed to initialize GLEW: ") + reinterpret_cast<const char *>(glewGetErrorString(err));
        glfwDestroyWindow(mWindow);
        glfwTerminate();
        throw std::runtime_error(msg);
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;

    // Route C callbacks back to this instance
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, ResizeCallback);
    glfwSetCursorPosCallback(mWindow, CursorCallback);
    glfwSetKeyCallback(mWindow, KeyCallback);
}

Window::~Window()
{
    if (mWindow)
        glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(mWindow);
}

void Window::SetShouldClose(bool value)
{
    glfwSetWindowShouldClose(mWindow, value);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(mWindow);
}

void Window::GetFramebufferSize(int &width, int &height) const
{
    glfwGetFramebufferSize(mWindow, &width, &height);
}

double Window::GetTime() const
{
    return glfwGetTime();
}

bool Window::IsKeyDown(int glfwKey) const
{
    return glfwGetKey(mWindow, glfwKey) == GLFW_PRESS;
}

void Window::SetCursorCaptured(bool captured)
{
    glfwSetInputMode(mWindow, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::ResizeCallback(GLFWwindow *window, int width, int height)
{
    auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (self && self->OnResize)
        self->OnResize(width, height);
}

void Window::CursorCallback(GLFWwindow *window, double x, double y)
{
    auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (self && self->OnCursorMove)
        self->OnCursorMove(static_cast<float>(x), static_cast<float>(y));
}

void Window::KeyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/)
{
    auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (self && self->OnKey)
        self->OnKey(key, action);
}
