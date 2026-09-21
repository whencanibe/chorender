#include "renderer/light.h"

#include "helper/matrix.h" // toRadian

#include <cmath>
#include <string>

void UploadLight(const Shader &shader, const DirLight &light, const char *name)
{
    const std::string n = name;
    shader.setVec3(n + ".direction", light.direction);
    shader.setVec3(n + ".ambient", light.ambient);
    shader.setVec3(n + ".diffuse", light.diffuse);
    shader.setVec3(n + ".specular", light.specular);
}

void UploadLight(const Shader &shader, const PointLight &light, int index, const char *arrayName)
{
    const std::string n = std::string(arrayName) + "[" + std::to_string(index) + "]";
    shader.setVec3(n + ".position", light.position);
    shader.setVec3(n + ".ambient", light.ambient);
    shader.setVec3(n + ".diffuse", light.diffuse);
    shader.setVec3(n + ".specular", light.specular);
    shader.setFloat(n + ".constant", light.constant);
    shader.setFloat(n + ".linear", light.linear);
    shader.setFloat(n + ".quadratic", light.quadratic);
}

void UploadLight(const Shader &shader, const SpotLight &light, const char *name)
{
    const std::string n = name;
    shader.setVec3(n + ".position", light.position);
    shader.setVec3(n + ".direction", light.direction);
    shader.setVec3(n + ".ambient", light.ambient);
    shader.setVec3(n + ".diffuse", light.diffuse);
    shader.setVec3(n + ".specular", light.specular);
    shader.setFloat(n + ".constant", light.constant);
    shader.setFloat(n + ".linear", light.linear);
    shader.setFloat(n + ".quadratic", light.quadratic);
    shader.setFloat(n + ".cutOff", std::cos(toRadian(light.cutOffDegrees)));
    shader.setFloat(n + ".outerCutOff", std::cos(toRadian(light.outerCutOffDegrees)));
}
