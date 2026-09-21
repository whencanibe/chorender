#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include <cstdint>

#include "core/framebuffer.h"
#include "core/shader.h"
#include "helper/matrix.h"
#include "helper/vec.h"

// Depth-only render target for a single directional light, plus the shader that fills it.
//
// Per frame:
//   Mat4 ls = ShadowMap::ComputeLightSpace(sunDir, sceneCenter, sceneRadius);
//   shadowMap.Begin(ls);
//   shadowMap.GetShader().setMat4("uModel", model);  model.Draw(shadowMap.GetShader());
//   shadowMap.End();
//   ... then bind DepthTexture() for the lighting pass and pass it `ls` as uLightSpace.
class ShadowMap
{
public:
    explicit ShadowMap(int size = 2048);

    // Orthographic light-space matrix that encloses a bounding sphere.
    // lightDir is the direction the light travels (same convention as DirLight).
    static Mat4 ComputeLightSpace(const Vec3 &lightDir, const Vec3 &sceneCenter, float sceneRadius);

    // Binds the depth FBO (viewport = map size), clears it, and activates the depth shader.
    void Begin(const Mat4 &lightSpace);
    // Unbinds the FBO. The caller's next Framebuffer::Bind() restores its own viewport.
    void End();

    Shader &GetShader() { return mShader; }
    uint32_t DepthTexture() const { return mFbo.DepthTexture(); }
    int Size() const { return mFbo.Width(); }

private:
    Framebuffer mFbo;
    Shader mShader;
};

#endif
