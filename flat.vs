#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec4 colour;

// TODO make this length 1 and use a GLSL array
layout (location = 2) in vec3 skin_colour;

layout (location = 3) in vec3 texture_position;

out vec4 vertex_colour;
out vec4 vertex_skin_colour;
out vec3 vertex_texture_position;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);

    vertex_colour = colour;
    vertex_skin_colour = vec4(skin_colour, 1.0);
    vertex_texture_position = texture_position;
}
