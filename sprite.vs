#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 vertex_texture_position;

out vec3 texture_position;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    texture_position = vertex_texture_position;
    //vertex_colour = colour;
}
