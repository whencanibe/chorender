#ifndef WINDOW_H
#define WINDOW_H

#include <functional>

struct GLFWwindow;

// Owns the GLFW window and GL context. This is the only place GLFW window/context calls live.
// Constructing it initializes GLFW and GLEW; throws std::runtime_error on failure.
class Window
{
public:
    Window(int width, int height, const char *title);
    ~Window();

    // GLFW stores a raw pointer back to this object, so it must not move.
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    bool ShouldClose() const;
    void SetShouldClose(bool value);
    void PollEvents();
    void SwapBuffers();

    void GetFramebufferSize(int &width, int &height) const;
    double GetTime() const;
    bool IsKeyDown(int glfwKey) const;

    // true: hide the cursor and let it move without bounds (FPS-style camera)
    void SetCursorCaptured(bool captured);

    // For libraries that need the raw handle (ImGui backend). Do not use elsewhere.
    GLFWwindow *GetNativeHandle() const { return mWindow; }

    // Event hooks. Set any subset; unset ones are ignored.
    std::function<void(int width, int height)> OnResize;   // framebuffer size, in pixels
    std::function<void(float x, float y)> OnCursorMove;    // absolute cursor position
    std::function<void(int glfwKey, int action)> OnKey;    // action: GLFW_PRESS / GLFW_RELEASE / GLFW_REPEAT

private:
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void CursorCallback(GLFWwindow *window, double x, double y);
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    GLFWwindow *mWindow = nullptr;
};

#endif
