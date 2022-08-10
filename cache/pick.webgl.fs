#version 300 es

precision mediump float;

out vec4 fragment_colour;

in vec4 vertex_colour;

uniform bool interlace;

void main() {
    if (interlace && int(gl_FragCoord.y) % 2 == 0) {
        discard;
    }

    fragment_colour = vertex_colour;
}
