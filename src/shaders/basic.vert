#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main(){
    gl_Position = uProj * uView * uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}