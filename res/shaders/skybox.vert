#version 330 core

layout (location = 0) in vec4 aPos;

out vec3 vTexCoord;

uniform mat4 uViewProj;

void main() {
    vTexCoord = aPos.xyz;
    gl_Position = uViewProj * aPos;
}
