#version 300 es

precision mediump float;

out vec4 fragment_colour;

in vec4 vertex_colour;

void main() {
    fragment_colour = vertex_colour;
}
