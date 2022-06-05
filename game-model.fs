#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

uniform sampler2DArray textures;

void main() {
    if (vertex_texture_position.z > -1) {
        fragment_colour = texture(textures, vertex_texture_position);

        if (fragment_colour.a <= 0.0) {
            discard;
        }

        fragment_colour.w = vertex_colour.w;
    } else {
        fragment_colour = vertex_colour;
    }
}
