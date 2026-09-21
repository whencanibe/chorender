#ifndef LIGHT_H
#define LIGHT_H

#include "core/shader.h"
#include "helper/vec.h"

// CPU-side mirrors of the light structs in lightShader.frag.
// Field names match the GLSL struct members one-to-one.

struct DirLight
{
    Vec3 direction{0.0f, -1.0f, 0.0f}; // direction the light travels, not toward the light
    Vec3 ambient{0.0f};
    Vec3 diffuse{1.0f};
    Vec3 specular{1.0f};
};

struct PointLight
{
    Vec3 position{0.0f};
    Vec3 ambient{0.05f};
    Vec3 diffuse{0.5f};
    Vec3 specular{1.0f};
    float constant = 1.0f;
    float linear = 0.14f;
    float quadratic = 0.07f;
};

struct SpotLight
{
    Vec3 position{0.0f};
    Vec3 direction{0.0f, 0.0f, -1.0f};
    Vec3 ambient{0.0f};
    Vec3 diffuse{1.0f};
    Vec3 specular{1.0f};
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float cutOffDegrees = 12.5f;      // inner cone half-angle; uploaded as cos()
    float outerCutOffDegrees = 15.0f; // outer cone half-angle; uploaded as cos()
};

// The shader program must be current (shader.use()) before calling these.
// Uniforms persist per program, so static lights only need uploading once.
void UploadLight(const Shader &shader, const DirLight &light, const char *name = "dirLight");
void UploadLight(const Shader &shader, const PointLight &light, int index, const char *arrayName = "pointLights");
void UploadLight(const Shader &shader, const SpotLight &light, const char *name = "spotLight");

#endif
