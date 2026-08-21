#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "helper/matrix.h"

class Shader{

public:
    uint32_t ID;

    Shader() = default;
    Shader(const char* vertexPath, const char* fragmentPath);

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const Vec2 &value) const;
    void setVec3(const std::string &name, const Vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const Vec4 &value) const;
    void setMat3(const std::string &name, const Mat3 &mat) const;
    void setMat4(const std::string &name, const Mat4 &mat) const;
};


#endif