#ifndef MATRIX_H
#define MATRIX_H

#include "helper/vec.h"

#include <cmath>

struct Mat4;

struct Mat3
{
    Vec3 cols[3];

    Mat3()
    {
        cols[0] = Vec3(1.0f, 0.0f, 0.0f);
        cols[1] = Vec3(0.0f, 1.0f, 0.0f);
        cols[2] = Vec3(0.0f, 0.0f, 1.0f);
    }

    Mat3(float s)
    {
        cols[0] = Vec3(s, 0.0f, 0.0f);
        cols[1] = Vec3(0.0f, s, 0.0f);
        cols[2] = Vec3(0.0f, 0.0f, s);
    }

    Mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
    {
        cols[0] = Vec3(a, b, c);
        cols[1] = Vec3(d, e, f);
        cols[2] = Vec3(g, h, i);
    }

    Mat3(const Vec3 &va, const Vec3 &vb, const Vec3 &vc)
    {
        cols[0] = va;
        cols[1] = vb;
        cols[2] = vc;
    }

    Mat3(const Mat4 &m);

    Vec3 &operator[](int index)
    {
        if (index == 0)
        {
            return cols[0];
        }

        if (index == 1)
        {
            return cols[1];
        }

        return cols[2];
    }

    const Vec3 &operator[](int index) const
    {
        if (index == 0)
        {
            return cols[0];
        }

        if (index == 1)
        {
            return cols[1];
        }

        return cols[2];
    }

    Mat3 operator*(const float s) const
    {
        return Mat3(cols[0] * s, cols[1] * s, cols[2] * s);
    }

    Vec3 operator*(const Vec3 &v) const
    {
        return Vec3(
            cols[0].X * v.X + cols[1].X * v.Y + cols[2].X * v.Z,
            cols[0].Y * v.X + cols[1].Y * v.Y + cols[2].Y * v.Z,
            cols[0].Z * v.X + cols[1].Z * v.Y + cols[2].Z * v.Z);
    }

    Mat3 operator*(const Mat3 &m) const
    {
        return Mat3(
            cols[0].X * m[0].X + cols[1].X * m[0].Y + cols[2].X * m[0].Z,
            cols[0].Y * m[0].X + cols[1].Y * m[0].Y + cols[2].Y * m[0].Z,
            cols[0].Z * m[0].X + cols[1].Z * m[0].Y + cols[2].Z * m[0].Z,

            cols[0].X * m[1].X + cols[1].X * m[1].Y + cols[2].X * m[1].Z,
            cols[0].Y * m[1].X + cols[1].Y * m[1].Y + cols[2].Y * m[1].Z,
            cols[0].Z * m[1].X + cols[1].Z * m[1].Y + cols[2].Z * m[1].Z,

            cols[0].X * m[2].X + cols[1].X * m[2].Y + cols[2].X * m[2].Z,
            cols[0].Y * m[2].X + cols[1].Y * m[2].Y + cols[2].Y * m[2].Z,
            cols[0].Z * m[2].X + cols[1].Z * m[2].Y + cols[2].Z * m[2].Z);
    }
};

struct Mat4
{
    Vec4 cols[4];

    Mat4()
    {
        cols[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
        cols[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
        cols[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
        cols[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Mat4(float s)
    {
        cols[0] = Vec4(s, 0.0f, 0.0f, 0.0f);
        cols[1] = Vec4(0.0f, s, 0.0f, 0.0f);
        cols[2] = Vec4(0.0f, 0.0f, s, 0.0f);
        cols[3] = Vec4(0.0f, 0.0f, 0.0f, s);
    }

    Mat4(float a, float b, float c, float d,
         float e, float f, float g, float h,
         float i, float j, float k, float l,
         float m, float n, float o, float p)
    {
        cols[0] = Vec4(a, b, c, d);
        cols[1] = Vec4(e, f, g, h);
        cols[2] = Vec4(i, j, k, l);
        cols[3] = Vec4(m, n, o, p);
    }

    Mat4(const Vec4 &va, const Vec4 &vb, const Vec4 &vc, const Vec4 &vd)
    {
        cols[0] = va;
        cols[1] = vb;
        cols[2] = vc;
        cols[3] = vd;
    }

    Vec4 &operator[](int index)
    {
        if (index == 0)
        {
            return cols[0];
        }

        if (index == 1)
        {
            return cols[1];
        }

        if (index == 2)
        {
            return cols[2];
        }

        return cols[3];
    }

    const Vec4 &operator[](int index) const
    {
        if (index == 0)
        {
            return cols[0];
        }

        if (index == 1)
        {
            return cols[1];
        }

        if (index == 2)
        {
            return cols[2];
        }

        return cols[3];
    }

    Mat4 operator*(const float s) const
    {
        return Mat4(cols[0] * s, cols[1] * s, cols[2] * s, cols[3] * s);
    }

    Vec4 operator*(const Vec4 &v) const
    {
        return Vec4(
            cols[0].X * v.X + cols[1].X * v.Y + cols[2].X * v.Z + cols[3].X * v.W,
            cols[0].Y * v.X + cols[1].Y * v.Y + cols[2].Y * v.Z + cols[3].Y * v.W,
            cols[0].Z * v.X + cols[1].Z * v.Y + cols[2].Z * v.Z + cols[3].Z * v.W,
            cols[0].W * v.X + cols[1].W * v.Y + cols[2].W * v.Z + cols[3].W * v.W);
    }

    Mat4 operator*(const Mat4 &m) const
    {
        return Mat4(
            cols[0].X * m[0].X + cols[1].X * m[0].Y + cols[2].X * m[0].Z + cols[3].X * m[0].W,
            cols[0].Y * m[0].X + cols[1].Y * m[0].Y + cols[2].Y * m[0].Z + cols[3].Y * m[0].W,
            cols[0].Z * m[0].X + cols[1].Z * m[0].Y + cols[2].Z * m[0].Z + cols[3].Z * m[0].W,
            cols[0].W * m[0].X + cols[1].W * m[0].Y + cols[2].W * m[0].Z + cols[3].W * m[0].W,

            cols[0].X * m[1].X + cols[1].X * m[1].Y + cols[2].X * m[1].Z + cols[3].X * m[1].W,
            cols[0].Y * m[1].X + cols[1].Y * m[1].Y + cols[2].Y * m[1].Z + cols[3].Y * m[1].W,
            cols[0].Z * m[1].X + cols[1].Z * m[1].Y + cols[2].Z * m[1].Z + cols[3].Z * m[1].W,
            cols[0].W * m[1].X + cols[1].W * m[1].Y + cols[2].W * m[1].Z + cols[3].W * m[1].W,

            cols[0].X * m[2].X + cols[1].X * m[2].Y + cols[2].X * m[2].Z + cols[3].X * m[2].W,
            cols[0].Y * m[2].X + cols[1].Y * m[2].Y + cols[2].Y * m[2].Z + cols[3].Y * m[2].W,
            cols[0].Z * m[2].X + cols[1].Z * m[2].Y + cols[2].Z * m[2].Z + cols[3].Z * m[2].W,
            cols[0].W * m[2].X + cols[1].W * m[2].Y + cols[2].W * m[2].Z + cols[3].W * m[2].W,

            cols[0].X * m[3].X + cols[1].X * m[3].Y + cols[2].X * m[3].Z + cols[3].X * m[3].W,
            cols[0].Y * m[3].X + cols[1].Y * m[3].Y + cols[2].Y * m[3].Z + cols[3].Y * m[3].W,
            cols[0].Z * m[3].X + cols[1].Z * m[3].Y + cols[2].Z * m[3].Z + cols[3].Z * m[3].W,
            cols[0].W * m[3].X + cols[1].W * m[3].Y + cols[2].W * m[3].Z + cols[3].W * m[3].W

        );
    }
};

inline Mat4 Translate(const Mat4 &transformMat, const Vec3 &v)
{
    Mat4 tm;
    tm[3] = Vec4(v, 1.0f);

    return transformMat * tm;
}

inline Mat4 Scale(const Mat4 &m, const Vec3 &v)
{
    Mat4 sm;

    sm.cols[0] = m.cols[0] * v.X;
    sm.cols[1] = m.cols[1] * v.Y;
    sm.cols[2] = m.cols[2] * v.Z;
    sm.cols[3] = m.cols[3];

    return sm;
}

inline Mat4 Rotate(const Mat4 &transformMat, const float angleRadian, const Vec3 &axisVec)
{
    Mat4 rotationMat = transformMat;
    Vec3 axis = Normalise(axisVec);
    float c = cos(angleRadian);
    float s = sin(angleRadian);
    float t = 1 - c;

    Vec4 c0 = Vec4(axis.X * axis.X * t + c,
                   axis.Y * axis.X * t + axis.Z * s,
                   axis.Z * axis.X * t - axis.Y * s,
                   0.0f);

    Vec4 c1 = Vec4(axis.X * axis.Y * t - axis.Z * s,
                   axis.Y * axis.Y * t + c,
                   axis.Z * axis.Y * t + axis.X * s,
                   0.0f);

    Vec4 c2 = Vec4(axis.X * axis.Z * t + axis.Y * s,
                   axis.Y * axis.Z * t - axis.X * s,
                   axis.Z * axis.Z * t + c,
                   0.0f);

    Vec4 c3 = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

    return rotationMat * Mat4(c0, c1, c2, c3);
}

inline Mat4 LookAt(const Vec3 &eye, const Vec3 &at, const Vec3 &up)
{
    if (eye == at)
    {
        // exception
        return Mat4();
    }
    // Camera is towards -Z direction in OpenGL so flipped the direction.
    Vec3 forward = Normalise(eye - at);
    Vec3 right = Normalise(Cross(up, forward));
    Vec3 upCam = Normalise(Cross(forward, right));

    Vec4 c0 = Vec4(right.X, upCam.X, forward.X, 0.0f);
    Vec4 c1 = Vec4(right.Y, upCam.Y, forward.Y, 0.0f);
    Vec4 c2 = Vec4(right.Z, upCam.Z, forward.Z, 0.0f);
    Vec4 c3 = Vec4(-Dot(right, eye), -Dot(upCam, eye), -Dot(forward, eye), 1.0f);

    return Mat4(c0, c1, c2, c3);
}

inline Mat4 Perspective(float fovRadian, float aspect, float near, float far)
{
    if (aspect == 0.0f || near == far)
    {
        return Mat4();
    }

    float tanHalfFov = std::tan(fovRadian / 2.0f);

    Mat4 Result;
    Result[0] = Vec4(1.0f / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f);

    Result[1] = Vec4(0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f);

    Result[2] = Vec4(0.0f, 0.0f, -(far + near) / (far - near), -1.0f);

    Result[3] = Vec4(0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f);

    return Result;
}

inline Mat4 Ortho(float left, float right, float bottom, float top, float near, float far)
{
    if (left == right || bottom == top || near == far)
    {
        return Mat4();
    }

    Mat4 Result = Mat4();

    // 대각선 성분 (Scale: NDC 크기인 2에 맞춰 배율 조정)
    Result[0].X = 2.0f / (right - left);
    Result[1].Y = 2.0f / (top - bottom);
    Result[2].Z = -2.0f / (far - near); // 오른손 좌표계 -Z 방향 반영

    // 4번째 열 (Translate: 공간의 정중앙을 원점으로 이동)
    Result[3].X = -(right + left) / (right - left);
    Result[3].Y = -(top + bottom) / (top - bottom);
    Result[3].Z = -(far + near) / (far - near);
    Result[3].W = 1.0f;

    return Result;
}

inline Mat4 Ortho(float left, float right, float bottom, float top)
{
    // 깊이(Z)를 기본값인 -1.0f ~ 1.0f로 설정하여 6개짜리 함수 호출
    return Ortho(left, right, bottom, top, -1.0f, 1.0f);
}

inline float toRadian(float degrees)
{
    return degrees * (3.14159265359 / 180.0f);
}

inline Mat3 transpose(const Mat3 &m)
{
    return Mat3{
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]};
}

inline Mat3 inverse(const Mat3 &m)
{
    float det =
        m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

    float invDet = 1.0f / det;

    Mat3 result;
    result[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
    result[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * invDet;
    result[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
    result[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * invDet;
    result[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * invDet;
    result[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * invDet;
    result[0][2] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * invDet;
    result[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * invDet;
    result[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;
    return result;
}

inline Mat3 inverseTranspose(const Mat3 &m)
{
    float det =
        m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

    float invDet = 1.0f / det;

    Mat3 result;
    result[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
    result[0][1] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * invDet;
    result[0][2] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
    result[1][0] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * invDet;
    result[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * invDet;
    result[1][2] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * invDet;
    result[2][0] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * invDet;
    result[2][1] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * invDet;
    result[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;
    return result;
}

inline Mat3::Mat3(const Mat4 &m)
{
    cols[0] = Vec3(m[0][0], m[0][1], m[0][2]);
    cols[1] = Vec3(m[1][0], m[1][1], m[1][2]);
    cols[2] = Vec3(m[2][0], m[2][1], m[2][2]);
}
#endif