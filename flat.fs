#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;

void main() {
    fragment_colour = vec4(vertex_colour);
}
