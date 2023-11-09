#version 110

varying vec4 vertex_colour;
varying vec2 vertex_texture_position;
varying vec2 vertex_base_texture_position;

uniform sampler2D sprite_texture;
uniform sampler2D sprite_base_texture;

void main() {
    vec4 texture_colour = texture2D(sprite_texture, vertex_texture_position);

    vec4 base_texture_colour =
        texture2D(sprite_base_texture, vertex_base_texture_position);

    gl_FragColor = (texture_colour * vertex_colour) + base_texture_colour;

    if (gl_FragColor.a <= 0.0) {
        discard;
    }
}
