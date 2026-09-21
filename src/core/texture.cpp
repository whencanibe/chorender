#include <GL/glew.h>

#include "core/texture.h"

#include "helper/stb_image.h"

#include <iostream>

// Maps stb_image channel count to a GL pixel format. Returns 0 for unsupported counts.
static GLenum FormatFromChannels(int nrComponents)
{
    switch (nrComponents)
    {
    case 1:
        return GL_RED;
    case 2:
        return GL_RG;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    default:
        return 0;
    }
}

// Picks the internal (GPU-side) format. With srgb, the GPU converts sRGB -> linear on every sample,
// so lighting math runs in linear space. Only 3/4-channel color images have an sRGB variant;
// 1/2-channel data (masks, heights) is assumed linear.
static GLenum InternalFormatFor(GLenum format, bool srgb)
{
    if (srgb)
    {
        if (format == GL_RGB)
            return GL_SRGB8;
        if (format == GL_RGBA)
            return GL_SRGB8_ALPHA8;
    }
    return format;
}

uint32_t LoadTexture2D(const std::string &path, bool srgb)
{
    int width, height, nrComponents;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        return 0;
    }

    GLenum format = FormatFromChannels(nrComponents);
    if (format == 0)
    {
        std::cerr << "Unsupported channel count (" << nrComponents << ") at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormatFor(format, srgb), width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

uint32_t LoadCubemap(const std::vector<std::string> &faces, bool srgb)
{
    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (uint32_t i = 0; i < faces.size(); i++)
    {
        uint8_t *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (!data)
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            continue;
        }

        GLenum format = FormatFromChannels(nrComponents);
        if (format == 0)
        {
            std::cerr << "Unsupported channel count (" << nrComponents << ") at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            continue;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, InternalFormatFor(format, srgb), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
