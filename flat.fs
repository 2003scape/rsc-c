#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

uniform sampler2DArray textures;

void main() {
    if (vertex_texture_position.z > -1) {
        vec4 texture_colour = texture(textures, vertex_texture_position);

        /* sprite pixel is not transparent */
        if (texture_colour.w == 1.0) {
            /* mask colour is set (-1.0 is no-mask) and texture pixel is grey */
            if (vertex_colour.x >= 0.0 &&
                texture_colour.x == texture_colour.y &&
                texture_colour.x == texture_colour.z) {
                texture_colour *= vertex_colour;
            }

            /* set alpha with or without mask colour */
            texture_colour.w = vertex_colour.w;
        }

        fragment_colour = texture_colour;
    } else {
        fragment_colour = vertex_colour;
    }
}
