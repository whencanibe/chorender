#include <GL/glew.h>

#include "shapes/cube.h"

#include "helper/matrix.h"
#include "core/shader.h"

Cube::Cube()
{
    setupCube();
}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVbo);
}

void Cube::setupCube()
{
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Cube::Draw(Shader &shader)
{
    glBindVertexArray(mVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
