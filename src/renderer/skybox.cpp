#include <GL/glew.h>

#include "renderer/skybox.h"

#include "core/texture.h"

Skybox::Skybox(const std::vector<std::string> &faces)
    : mCubemap(LoadCubemap(faces, /*srgb=*/true)), // photos are sRGB; the post pass re-encodes
      mShader("../src/shaders/skybox.vert", "../src/shaders/skybox.frag")
{
    mShader.use();
    mShader.setInt("skybox", 0);
}

Skybox::~Skybox()
{
    glDeleteTextures(1, &mCubemap);
}

void Skybox::Draw(const Mat4 &view, const Mat4 &proj)
{
    // Pass when depth equals the cleared value (1.0), which is where skybox.vert places every fragment.
    glDepthFunc(GL_LEQUAL);

    mShader.use();
    mShader.setMat4("view", Mat4(Mat3(view))); // remove translation so the box follows the camera
    mShader.setMat4("projection", proj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemap);
    mCube.Draw(mShader);

    glDepthFunc(GL_LESS);
}
