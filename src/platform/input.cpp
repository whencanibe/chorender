#include "platform/input.h"

Input::Input(Window &window) : mWindow(window)
{
    mPressTime.fill(-1.0);

    mWindow.SetCursorCaptured(true);
    mWindow.OnKey = [this](int key, int action) { OnKey(key, action); };
    mWindow.OnCursorMove = [this](float x, float y) { OnCursorMove(x, y); };
}

void Input::OnKey(int key, int action)
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        mWindow.SetShouldClose(true);

    if (action != GLFW_PRESS)
        return; // only the initial press matters; GLFW_REPEAT is ignored

    mPressTime[key] = mWindow.GetTime();
    mPressedSincePoll[key] = true;
}

void Input::SetCameraMode(bool enabled)
{
    if (mCameraMode == enabled)
        return;
    mCameraMode = enabled;
    mWindow.SetCursorCaptured(enabled);
    // The cursor jumps when capture toggles; do not feed that jump to the camera.
    mFirstMouse = true;
    mMouseDeltaX = mMouseDeltaY = 0.0f;
}

void Input::OnCursorMove(float x, float y)
{
    if (!mCameraMode)
        return;

    if (mFirstMouse)
    {
        mLastX = x;
        mLastY = y;
        mFirstMouse = false;
    }

    mMouseDeltaX += x - mLastX;
    mMouseDeltaY += mLastY - y; // screen y grows downward; camera pitch grows upward

    mLastX = x;
    mLastY = y;
}

void Input::Update(Camera &camera, float deltaTime)
{
    // Mouse look
    if (mMouseDeltaX != 0.0f || mMouseDeltaY != 0.0f)
    {
        camera.ProcessMouseMovement(mMouseDeltaX, mMouseDeltaY);
        mMouseDeltaX = mMouseDeltaY = 0.0f;
    }

    // Movement. When opposite keys are both held, the one pressed more recently wins.
    const bool w = IsKeyDown(GLFW_KEY_W);
    const bool s = IsKeyDown(GLFW_KEY_S);
    const bool a = IsKeyDown(GLFW_KEY_A);
    const bool d = IsKeyDown(GLFW_KEY_D);

    if (w && s)
        camera.ProcessKeyboard(mPressTime[GLFW_KEY_W] > mPressTime[GLFW_KEY_S] ? CameraMovement::FORWARD : CameraMovement::BACKWARD, deltaTime);
    else if (w)
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    else if (s)
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);

    if (a && d)
        camera.ProcessKeyboard(mPressTime[GLFW_KEY_A] > mPressTime[GLFW_KEY_D] ? CameraMovement::LEFT : CameraMovement::RIGHT, deltaTime);
    else if (a)
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    else if (d)
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);

    // Roll edge-trigger state: presses collected during the last PollEvents become visible for this frame.
    mPressedThisFrame = mPressedSincePoll;
    mPressedSincePoll.fill(false);
}
