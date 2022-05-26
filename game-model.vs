#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec3 texture_position;

out vec4 vertex_colour;
out vec3 vertex_texture_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);

    vertex_colour = colour;
    vertex_texture_position = texture_position;
}

