#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <cassert>

struct Vec2
{
    float X, Y;
    static const int N = 2;

    Vec2() : X(0.0f), Y(0.0f) {}
    Vec2(float c) : X(c), Y(c) {}
    Vec2(float inX, float inY) : X(inX), Y(inY) {}

    Vec2 operator+(const Vec2 &o) const { return Vec2(X + o.X, Y + o.Y); }
    Vec2 operator-(const Vec2 &o) const { return Vec2(X - o.X, Y - o.Y); }
    Vec2 operator*(const float s) const { return Vec2(X * s, Y * s); }
    Vec2 operator/(const float s) const
    {
        if (std::abs(s) < 0.0001f)
        {
            return Vec2(X, Y);
        }
        float inv = 1.0f / s;
        return Vec2(X * inv, Y * inv);
    }

    Vec2 &operator+=(const Vec2 &o)
    {
        X += o.X;
        Y += o.Y;

        return *this;
    }

    Vec2 &operator-=(const Vec2 &o)
    {
        X -= o.X;
        Y -= o.Y;

        return *this;
    }

    Vec2 &operator*=(const float s)
    {
        X *= s;
        Y *= s;

        return *this;
    }

    Vec2 &operator/=(const float s)
    {
        if (std::abs(s) < 0.0001f)
        {
            return *this;
        }

        float inv = 1.0f / s;

        X *= inv;
        Y *= inv;

        return *this;
    }

    float &operator[](int index)
    {   
        assert(index >= 0 && index < N);
        if (index == 0)
        {
            return X;
        }

        return Y;
    }

    const float &operator[](int index) const
    {
        assert(index >= 0 && index < N);
        if (index == 0)
        {
            return X;
        }

        return Y;
    }
};

inline Vec2 operator*(const float s, const Vec2 &vec)
{
    return vec * s;
}

struct Vec3
{
    float X, Y, Z;
    static const int N = 3;

    Vec3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    Vec3(float c) : X(c), Y(c), Z(c) {}
    Vec3(float inX, float inY, float inZ) : X(inX), Y(inY), Z(inZ) {}
    Vec3(Vec2 v, float z) : X(v.X), Y(v.Y), Z(z) {}

    Vec3 operator+(const Vec3 &o) const { return Vec3(X + o.X, Y + o.Y, Z + o.Z); }
    Vec3 operator-(const Vec3 &o) const { return Vec3(X - o.X, Y - o.Y, Z - o.Z); }
    Vec3 operator*(const float s) const { return Vec3(X * s, Y * s, Z * s); }
    Vec3 operator*(const Vec3 &o) const { return Vec3(X * o.X, Y * o.Y, Z * o.Z); }
    bool operator==(const Vec3 &o) const
    {
        if (X == o.X && Y == o.Y && Z == o.Z)
        {
            return true;
        }
        return false;
    };
    Vec3 operator/(const float s) const
    {
        if (std::abs(s) < 0.0001f)
        {
            return Vec3(X, Y, Z);
        }

        float inv = 1.0f / s;

        return Vec3(X * inv, Y * inv, Z * inv);
    }

    Vec3 &operator+=(const Vec3 &o)
    {
        X += o.X;
        Y += o.Y;
        Z += o.Z;

        return *this;
    }

    Vec3 &operator-=(const Vec3 &o)
    {
        X -= o.X;
        Y -= o.Y;
        Z -= o.Z;

        return *this;
    }

    Vec3 &operator*=(const float s)
    {
        X *= s;
        Y *= s;
        Z *= s;

        return *this;
    }

    Vec3 &operator/=(const float s)
    {
        if (std::abs(s) < 0.0001f)
        {
            return *this;
        }

        float inv = 1.0f / s;

        X *= inv;
        Y *= inv;
        Z *= inv;

        return *this;
    }

    float &operator[](int index)
    {
        assert(index >= 0 && index < N);
        if (index == 0)
        {
            return X;
        }
        if (index == 1)
        {
            return Y;
        }

        return Z;
    }

    const float &operator[](int index) const
    {
        assert(index >= 0 && index < N);
        if (index == 0)
        {
            return X;
        }
        if (index == 1)
        {
            return Y;
        }

        return Z;
    }
};

inline Vec3 operator*(const float s, const Vec3 &vec)
{
    return vec * s;
}

struct Vec4
{
    float X, Y, Z, W;
    static const int N = 4;

    Vec4() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {}
    Vec4(float c) : X(c), Y(c), Z(c), W(c) {}
    Vec4(float inX, float inY, float inZ, float inW) : X(inX), Y(inY), Z(inZ), W(inW) {}
    Vec4(Vec3 v, float w) : X(v.X), Y(v.Y), Z(v.Z), W(w) {}

    Vec4 operator+(const Vec4 &o) const { return Vec4(X + o.X, Y + o.Y, Z + o.Z, W + o.W); }
    Vec4 operator-(const Vec4 &o) const { return Vec4(X - o.X, Y - o.Y, Z - o.Z, W - o.W); }
    Vec4 operator*(const float s) const { return Vec4(X * s, Y * s, Z * s, W * s); }
    Vec4 operator*(const Vec4 &o) const { return Vec4(X * o.X, Y * o.Y, Z * o.Z, W * o.W); }

    bool operator==(const Vec4 &o) const
    {
        if (X == o.X && Y == o.Y && Z == o.Z && W == o.W)
        {
            return true;
        }
        return false;
    };
    Vec4 operator/(const float s) const
    {
        if (std::abs(s) < 0.0001f)
        {
            return Vec4(X, Y, Z, W);
        }

        float inv = 1.0f / s;

        return Vec4(X * inv, Y * inv, Z * inv, W * inv);
    }

    Vec4 &operator+=(const Vec4 &o)
    {
        X += o.X;
        Y += o.Y;
        Z += o.Z;
        W += o.W;

        return *this;
    }

    Vec4 &operator-=(const Vec4 &o)
    {
        X -= o.X;
        Y -= o.Y;
        Z -= o.Z;
        W -= o.W;

        return *this;
    }

    Vec4 &operator*=(const float s)
    {
        X *= s;
        Y *= s;
        Z *= s;
        W *= s;

        return *this;
    }

    Vec4 &operator/=(const float s)
    {
        if (std::abs(s) < 0.0001f)
        {
            return *this;
        }

        float inv = 1.0f / s;

        X *= inv;
        Y *= inv;
        Z *= inv;
        W *= inv;

        return *this;
    }

    float &operator[](int index)
    {
        assert(index >= 0 && index < N);
        if (index == 0)
        {
            return X;
        }
        if (index == 1)
        {
            return Y;
        }
        if (index == 2)
        {
            return Z;
        }

        return W;
    }

    const float &operator[](int index) const
    {
        assert(index >= 0 && index < N);
        if (index == 0)
        {
            return X;
        }
        if (index == 1)
        {
            return Y;
        }
        if (index == 2)
        {
            return Z;
        }

        return W;
    }
};

inline Vec4 operator*(const float s, const Vec4 &vec)
{
    return vec * s;
}

template <typename V>
float Dot(const V &a, const V &b)
{
    float result = 0.0f;
    for (int i = 0; i < V::N; ++i)
    {
        result += a[i] * b[i];
    }
    return result;
}

template <typename V>
float Length(const V &v)
{
    return std::sqrt(Dot(v, v));
}

template <typename V>
V Normalise(const V &v)
{
    float len = Length(v);

    if (len < 0.00001f)
    {
        return v;
    }

    return v / len;
}

inline Vec3 Cross(const Vec3 &a, const Vec3 &b)
{
    return Vec3(
        a.Y * b.Z - a.Z * b.Y,
        a.Z * b.X - a.X * b.Z,
        a.X * b.Y - a.Y * b.X);
}

#endif