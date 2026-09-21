#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

#include <cstdint>

struct FramebufferSpec
{
    int width = 0;
    int height = 0;

    bool hasColor = true;
    GLenum colorInternalFormat = GL_RGB; // GL_RGBA16F for HDR

    bool hasDepth = true;
    // false: depth+stencil renderbuffer (not sampleable, cheapest)
    // true : depth texture you can sample from (shadow maps)
    bool depthAsTexture = false;
};

// Owns an FBO and its attachments. Recreates them on Resize().
class Framebuffer
{
public:
    explicit Framebuffer(const FramebufferSpec &spec);
    ~Framebuffer();

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;
    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer &operator=(Framebuffer &&other) noexcept;

    // Binds the FBO and sets the viewport to its size.
    void Bind() const;
    // Binds the default framebuffer. Does not touch the viewport.
    static void Unbind();

    // No-op if the size is unchanged or either dimension is 0.
    void Resize(int width, int height);

    uint32_t ColorTexture() const { return mColorTexture; }
    uint32_t DepthTexture() const { return mDepthTexture; } // 0 unless depthAsTexture
    int Width() const { return mSpec.width; }
    int Height() const { return mSpec.height; }
    const FramebufferSpec &Spec() const { return mSpec; }

private:
    void Create();
    void Destroy();

    FramebufferSpec mSpec;
    uint32_t mFBO = 0;
    uint32_t mColorTexture = 0;
    uint32_t mDepthTexture = 0;
    uint32_t mDepthRenderbuffer = 0;
};

#endif
