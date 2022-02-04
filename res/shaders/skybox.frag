#version 330 core

in vec3 vTexCoord;
out vec4 fFragColor;

uniform samplerCube uCubeMap;

void main() {
    fFragColor = texture(uCubeMap, vTexCoord);
}
