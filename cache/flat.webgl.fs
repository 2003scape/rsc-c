#version 300 es

precision mediump float;

out vec4 fragment_colour;

in vec4 vertex_colour;
in vec2 vertex_texture_position;
in vec2 vertex_base_texture_position;

uniform sampler2D sprite_texture;
uniform sampler2D sprite_base_texture;

void main() {
    vec4 texture_colour = texture(sprite_texture, vertex_texture_position.xy);

    vec4 base_texture_colour =
        texture(sprite_base_texture, vertex_base_texture_position);

    fragment_colour = (texture_colour * vertex_colour) + base_texture_colour;

    /*if (fragment_colour.w <= 0.0) {
        discard;
    }*/
}
