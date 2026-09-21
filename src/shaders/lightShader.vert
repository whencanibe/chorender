#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace; // this fragment as seen from the directional light

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMat;
uniform mat4 uLightSpace;

void main()
{
    TexCoords = aTexCoords;
    Normal = uNormalMat * aNormal;
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    FragPosLightSpace = uLightSpace * vec4(FragPos, 1.0);
    gl_Position = uProj * uView * vec4(FragPos, 1.0);
}
