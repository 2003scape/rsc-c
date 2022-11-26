#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec2 vertex_texture_position;
in vec2 vertex_base_texture_position;

uniform sampler2D texture;
uniform sampler2D base_texture;

void main() {
    vec4 texture_colour = texture2D(texture, vertex_texture_position);

    vec4 base_texture_colour =
        texture2D(base_texture, vertex_base_texture_position);

    fragment_colour = (texture_colour * vertex_colour) + base_texture_colour;

    /*if (fragment_colour.w <= 0.0) {
        discard;
    }*/
}
