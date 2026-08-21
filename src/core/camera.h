#ifndef CAMERA_H
#define CAMERA_H

#include "helper/vec.h"
#include "helper/matrix.h"

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

namespace CameraDefaults
{
    // inline : to solve One Definition Rule problem
    // constexpr : compile time constant
    // Default camera values
    inline constexpr float YAW = -90.0f;
    inline constexpr float PITCH = 0.0f;
    inline constexpr float SPEED = 2.5f;
    inline constexpr float SENSITIVITY = 0.1f;
    inline constexpr float ZOOM = 45.0f;
}

class Camera
{
public:
    // camera Attributes
    Vec3 Position{0.0f};
    Vec3 Front{0.0f, 0.0f, -1.0f};
    Vec3 Up{};
    Vec3 Right{};
    Vec3 WorldUp{0.0f, 1.0f, 0.0f};

    // euler Angles
    float Yaw = CameraDefaults::YAW;
    float Pitch = CameraDefaults::PITCH;
    // camera options
    float MovementSpeed = CameraDefaults::SPEED;
    float MouseSensitivity = CameraDefaults::SENSITIVITY;
    float Zoom = CameraDefaults::ZOOM;

    explicit Camera(Vec3 pos = Vec3(0.0f),
                    Vec3 worldUp = Vec3(0.0f, 1.0f, 0.0f),
                    float yaw = CameraDefaults::YAW, float pitch = CameraDefaults::PITCH)
        : Position(pos), WorldUp(worldUp), Yaw(yaw), Pitch(pitch)
    {
        updateCameraVectors();
    }
    // undefined reference 링크 에러 방지
    ~Camera() = default;

    Mat4 GetViewMatrix() const
    {
        return LookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        if (direction == CameraMovement::FORWARD)
            Position += Front * velocity;
        if (direction == CameraMovement::BACKWARD)
            Position -= Front * velocity;
        if (direction == CameraMovement::LEFT)
            Position -= Right * velocity;
        if (direction == CameraMovement::RIGHT)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        Vec3 front;
        front.X = cos(toRadian(Yaw)) * cos(toRadian(Pitch));
        front.Y = sin(toRadian(Pitch));
        front.Z = sin(toRadian(Yaw)) * cos(toRadian(Pitch));
        Front = Normalise(front);

        Right = Normalise(Cross(Front, WorldUp));
        Up = Normalise(Cross(Right, Front));
    }
};

#endif