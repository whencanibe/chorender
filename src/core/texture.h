#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>
#include <string>
#include <vector>

// Loads a 2D texture from file and returns the GL texture ID.
// Returns 0 on failure so the caller can decide on a fallback.
// srgb: true for color images authored in sRGB (diffuse/albedo, skybox photos). The GPU then
// converts to linear on sample. Leave false for data textures (normals, heights, masks).
uint32_t LoadTexture2D(const std::string &path, bool srgb = false);

// Loads six cubemap faces in order (+X, -X, +Y, -Y, +Z, -Z) and returns the GL texture ID.
uint32_t LoadCubemap(const std::vector<std::string> &faces, bool srgb = false);

#endif
