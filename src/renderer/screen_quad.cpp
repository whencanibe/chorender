#include <GL/glew.h>

#include "renderer/screen_quad.h"

#include <utility>

ScreenQuad::ScreenQuad()
{
    // positions (NDC) + texCoords
    static const float vertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
}

ScreenQuad::~ScreenQuad()
{
    if (mVAO)
        glDeleteVertexArrays(1, &mVAO);
    if (mVBO)
        glDeleteBuffers(1, &mVBO);
}

ScreenQuad::ScreenQuad(ScreenQuad &&other) noexcept
    : mVAO(std::exchange(other.mVAO, 0)),
      mVBO(std::exchange(other.mVBO, 0))
{
}

ScreenQuad &ScreenQuad::operator=(ScreenQuad &&other) noexcept
{
    if (this != &other)
    {
        if (mVAO)
            glDeleteVertexArrays(1, &mVAO);
        if (mVBO)
            glDeleteBuffers(1, &mVBO);
        mVAO = std::exchange(other.mVAO, 0);
        mVBO = std::exchange(other.mVBO, 0);
    }
    return *this;
}

void ScreenQuad::Draw() const
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
