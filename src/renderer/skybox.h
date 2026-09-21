#ifndef SKYBOX_H
#define SKYBOX_H

#include <cstdint>
#include <string>
#include <vector>

#include "core/shader.h"
#include "helper/matrix.h"
#include "shapes/cube.h"

// Cubemap skybox. Owns the cubemap texture and its shader; reuses Cube for geometry.
// Must be constructed after the GL context exists.
class Skybox
{
public:
    // faces: +X, -X, +Y, -Y, +Z, -Z
    explicit Skybox(const std::vector<std::string> &faces);
    ~Skybox();

    // Owns GL resources; Cube member is not move-safe yet, so disallow both copy and move.
    Skybox(const Skybox &) = delete;
    Skybox &operator=(const Skybox &) = delete;
    Skybox(Skybox &&) = delete;
    Skybox &operator=(Skybox &&) = delete;

    // Draw after all opaque geometry. Strips translation from view internally
    // and switches depth func to GL_LEQUAL so the skybox lands at depth 1.0.
    void Draw(const Mat4 &view, const Mat4 &proj);

private:
    uint32_t mCubemap = 0;
    Shader mShader;
    Cube mCube;
};

#endif
