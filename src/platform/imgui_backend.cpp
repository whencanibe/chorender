#include "platform/imgui_backend.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

ImGuiBackend::ImGuiBackend(Window &window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // install_callbacks=true: ImGui hooks the GLFW callbacks and forwards to the ones Window installed,
    // so Input keeps working.
    ImGui_ImplGlfw_InitForOpenGL(window.GetNativeHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

ImGuiBackend::~ImGuiBackend()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiBackend::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiBackend::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiBackend::WantsMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiBackend::WantsKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}
