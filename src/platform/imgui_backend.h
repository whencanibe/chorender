#ifndef IMGUI_BACKEND_H
#define IMGUI_BACKEND_H

#include "platform/window.h"

// Owns the Dear ImGui context and its GLFW/OpenGL3 backends.
// Construct after Window (it needs the GL context and chains onto Window's GLFW callbacks).
//
// Per frame: BeginFrame() -> ImGui::* calls -> EndFrame(), after the scene is drawn to the default framebuffer.
class ImGuiBackend
{
public:
    explicit ImGuiBackend(Window &window);
    ~ImGuiBackend();

    ImGuiBackend(const ImGuiBackend &) = delete;
    ImGuiBackend &operator=(const ImGuiBackend &) = delete;
    ImGuiBackend(ImGuiBackend &&) = delete;
    ImGuiBackend &operator=(ImGuiBackend &&) = delete;

    void BeginFrame();
    void EndFrame();

    // True while a widget is using the mouse/keyboard; skip game input on those frames.
    bool WantsMouse() const;
    bool WantsKeyboard() const;
};

#endif
