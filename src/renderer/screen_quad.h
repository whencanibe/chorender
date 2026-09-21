#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H

#include <cstdint>

// Two triangles covering the full screen in NDC. Attribute 0 = vec2 position, 1 = vec2 uv.
// Used for post-processing passes; the caller binds the shader and source texture.
class ScreenQuad
{
public:
    ScreenQuad();
    ~ScreenQuad();

    ScreenQuad(const ScreenQuad &) = delete;
    ScreenQuad &operator=(const ScreenQuad &) = delete;
    ScreenQuad(ScreenQuad &&other) noexcept;
    ScreenQuad &operator=(ScreenQuad &&other) noexcept;

    void Draw() const;

private:
    uint32_t mVAO = 0;
    uint32_t mVBO = 0;
};

#endif
