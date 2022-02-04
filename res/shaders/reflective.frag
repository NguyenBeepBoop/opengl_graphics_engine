#version 330 core

in vec3 v_tex_coord;

uniform samplerCube u_texture;

out vec4 f_color;

void main() {
    f_color = texture(u_texture, v_tex_coord);
}
