#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h> // GLFW_KEY_* constants are the key identifiers used throughout

#include <array>

#include "core/camera.h"
#include "platform/window.h"

// Keyboard/mouse state for an FPS-style camera. Hooks itself into Window's callbacks.
//
// Per frame: window.PollEvents() -> input.Update(camera, dt) -> query WasKeyPressed()/IsKeyDown()
class Input
{
public:
    explicit Input(Window &window);

    // Applies accumulated mouse motion and held movement keys to the camera,
    // then rolls the edge-trigger state over for this frame.
    void Update(Camera &camera, float deltaTime);

    bool IsKeyDown(int glfwKey) const { return mWindow.IsKeyDown(glfwKey); }
    // True only on the frame the key went down (after the Update() that followed the press).
    bool WasKeyPressed(int glfwKey) const { return mPressedThisFrame[glfwKey]; }

    // Ignore the next cursor event's delta. Call after anything that stalls the loop (e.g. model loading)
    // so the accumulated cursor jump does not spin the camera.
    void ResetMouse() { mFirstMouse = true; }

    // Camera mode: cursor captured, mouse drives the camera.
    // UI mode: cursor visible, mouse ignored by the camera (so it can operate ImGui widgets).
    void SetCameraMode(bool enabled);
    bool IsCameraMode() const { return mCameraMode; }

private:
    void OnKey(int key, int action);
    void OnCursorMove(float x, float y);

    Window &mWindow;

    // Time each key was last pressed; used to resolve opposite keys held together (later press wins).
    std::array<double, GLFW_KEY_LAST + 1> mPressTime{};
    std::array<bool, GLFW_KEY_LAST + 1> mPressedSincePoll{}; // set by callbacks
    std::array<bool, GLFW_KEY_LAST + 1> mPressedThisFrame{}; // snapshot exposed via WasKeyPressed

    bool mCameraMode = true;
    bool mFirstMouse = true;
    float mLastX = 0.0f;
    float mLastY = 0.0f;
    float mMouseDeltaX = 0.0f;
    float mMouseDeltaY = 0.0f;
};

#endif
