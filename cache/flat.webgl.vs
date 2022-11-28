#version 300 es
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 texture_position;
layout (location = 3) in vec2 base_texture_position;

out vec4 vertex_colour;
out vec2 vertex_texture_position;
out vec2 vertex_base_texture_position;

void main() {
    gl_Position = vec4(position, 1.0);

    vertex_colour = colour;
    vertex_texture_position = texture_position;
    vertex_base_texture_position = base_texture_position;
}
