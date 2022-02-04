#version 330 core

uniform mat4 u_model_view;
uniform mat4 u_view;
uniform mat4 u_projection;

layout (location = 0) in vec4 a_pos;
layout (location = 1) in vec2 a_tex_coord;
layout (location = 2) in vec3 a_normal;

out vec3 v_tex_coord;
out float gl_ClipDistance[1];

void main() {
    vec4 view_pos = u_model_view * a_pos;
    v_tex_coord = inverse(mat3(u_view)) * reflect(normalize(view_pos.xyz), mat3(u_model_view) * a_normal);
    gl_Position = u_projection * view_pos;
    gl_ClipDistance[0] = 1;
}
