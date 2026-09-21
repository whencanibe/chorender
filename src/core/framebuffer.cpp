#include "core/framebuffer.h"

#include <iostream>
#include <utility>

// glTexImage2D needs a (format, type) pair that is compatible with the internal format,
// even when no data is uploaded.
static void PixelFormatFor(GLenum internalFormat, GLenum &format, GLenum &type)
{
    switch (internalFormat)
    {
    case GL_RGB:
    case GL_RGB8:
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        break;
    case GL_RGBA:
    case GL_RGBA8:
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    case GL_RGB16F:
    case GL_RGB32F:
        format = GL_RGB;
        type = GL_FLOAT;
        break;
    case GL_RGBA16F:
    case GL_RGBA32F:
        format = GL_RGBA;
        type = GL_FLOAT;
        break;
    default:
        std::cerr << "Framebuffer: unhandled color internal format 0x" << std::hex << internalFormat << std::dec << std::endl;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    }
}

Framebuffer::Framebuffer(const FramebufferSpec &spec) : mSpec(spec)
{
    Create();
}

Framebuffer::~Framebuffer()
{
    Destroy();
}

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : mSpec(other.mSpec),
      mFBO(std::exchange(other.mFBO, 0)),
      mColorTexture(std::exchange(other.mColorTexture, 0)),
      mDepthTexture(std::exchange(other.mDepthTexture, 0)),
      mDepthRenderbuffer(std::exchange(other.mDepthRenderbuffer, 0))
{
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept
{
    if (this != &other)
    {
        Destroy();
        mSpec = other.mSpec;
        mFBO = std::exchange(other.mFBO, 0);
        mColorTexture = std::exchange(other.mColorTexture, 0);
        mDepthTexture = std::exchange(other.mDepthTexture, 0);
        mDepthRenderbuffer = std::exchange(other.mDepthRenderbuffer, 0);
    }
    return *this;
}

void Framebuffer::Create()
{
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    if (mSpec.hasColor)
    {
        GLenum format, type;
        PixelFormatFor(mSpec.colorInternalFormat, format, type);

        glGenTextures(1, &mColorTexture);
        glBindTexture(GL_TEXTURE_2D, mColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, mSpec.colorInternalFormat, mSpec.width, mSpec.height, 0, format, type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
    }
    else
    {
        // Depth-only target (e.g. shadow map): tell GL there is no color to write or read.
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (mSpec.hasDepth)
    {
        if (mSpec.depthAsTexture)
        {
            glGenTextures(1, &mDepthTexture);
            glBindTexture(GL_TEXTURE_2D, mDepthTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, mSpec.width, mSpec.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // Samples outside the map read depth 1.0 (= not in shadow).
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            const float border[] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
            // Depth samples as (d, 0, 0, 1); broadcast d to GB so debug views show grayscale.
            // Shaders reading .r are unaffected.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
        }
        else
        {
            glGenRenderbuffers(1, &mDepthRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mSpec.width, mSpec.height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
        }
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: not complete, status 0x" << std::hex << status << std::dec << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Destroy()
{
    if (mColorTexture)
        glDeleteTextures(1, &mColorTexture);
    if (mDepthTexture)
        glDeleteTextures(1, &mDepthTexture);
    if (mDepthRenderbuffer)
        glDeleteRenderbuffers(1, &mDepthRenderbuffer);
    if (mFBO)
        glDeleteFramebuffers(1, &mFBO);

    mColorTexture = mDepthTexture = mDepthRenderbuffer = mFBO = 0;
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glViewport(0, 0, mSpec.width, mSpec.height);
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;
    if (width == mSpec.width && height == mSpec.height)
        return;

    mSpec.width = width;
    mSpec.height = height;
    Destroy();
    Create();
}
