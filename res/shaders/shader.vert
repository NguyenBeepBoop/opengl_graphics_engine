#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vPosition;
out vec3 vView;
noperspective out vec2 vScreenCoord;

uniform mat4 uViewProj;
uniform mat4 uModel;
uniform vec3 uCameraPos;
uniform vec4 uClipPlane;
uniform bool uIsWaterSurface;
uniform bool uIsWater;
uniform float uNow;
uniform sampler2D uHeightMap;

out float gl_ClipDistance[1];

void main() {
    vTexCoord = aTexCoord;
    vNormal = normalize(mat3(uModel) * aNormal);
    vec4 pos = uModel * aPos;

    pos.y += texture(uHeightMap, vTexCoord).r;

    if (uIsWater || uIsWaterSurface) { // to be deleted
        pos.y += sin(uNow) * 0.0001;// to be deleted
    }// to be deleted


    vPosition = pos.xyz;
    vView = normalize(uCameraPos - vPosition);
    gl_Position = uViewProj * pos;

    vScreenCoord = (gl_Position.xy / gl_Position.w) * 0.5 + vec2(0.5);

    // set gl_ClipDistance[0] to be the distance between vPosition and the clipping plane
    gl_ClipDistance[0] = dot(pos, uClipPlane);
}
