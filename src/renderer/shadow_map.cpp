#include "renderer/shadow_map.h"

#include <cmath>

static FramebufferSpec DepthOnlySpec(int size)
{
    FramebufferSpec spec;
    spec.width = size;
    spec.height = size;
    spec.hasColor = false;
    spec.hasDepth = true;
    spec.depthAsTexture = true;
    return spec;
}

ShadowMap::ShadowMap(int size)
    : mFbo(DepthOnlySpec(size)),
      mShader("../src/shaders/shadow.vert", "../src/shaders/shadow.frag")
{
}

Mat4 ShadowMap::ComputeLightSpace(const Vec3 &lightDir, const Vec3 &sceneCenter, float sceneRadius)
{
    const Vec3 dir = Normalise(lightDir);

    // Place the "eye" 2r behind the sphere center along the light; the sphere then spans [r, 3r]
    // in front of it. Near/far get half a radius of margin on each side.
    const Vec3 eye = sceneCenter - dir * (sceneRadius * 2.0f);
    const float nearPlane = sceneRadius * 0.5f;
    const float farPlane = sceneRadius * 3.5f;

    // LookAt degenerates when the view direction is parallel to up; pick a different up for a vertical sun.
    const Vec3 up = (std::fabs(dir.Y) > 0.99f) ? Vec3(0.0f, 0.0f, 1.0f) : Vec3(0.0f, 1.0f, 0.0f);

    const Mat4 view = LookAt(eye, sceneCenter, up);
    const Mat4 proj = Ortho(-sceneRadius, sceneRadius, -sceneRadius, sceneRadius, nearPlane, farPlane);
    return proj * view;
}

void ShadowMap::Begin(const Mat4 &lightSpace)
{
    mFbo.Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mShader.use();
    mShader.setMat4("uLightSpace", lightSpace);
}

void ShadowMap::End()
{
    Framebuffer::Unbind();
}
