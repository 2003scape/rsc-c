#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;

// uniform sampler2D texture1;

void main() {
    fragment_colour = vertex_colour;
}
