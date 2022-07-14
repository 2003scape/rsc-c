#version 300 es

precision mediump float;

out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_skin_colour;
in vec3 vertex_texture_position;

uniform mediump sampler2DArray textures;

uniform bool interlace;

uniform float bounds_min_x;
uniform float bounds_max_x;
uniform float bounds_min_y;
uniform float bounds_max_y;

void main() {
    if (interlace && int(gl_FragCoord.y) % 2 == 0) {
        discard;
    }

    if (gl_FragCoord.x < bounds_min_x || gl_FragCoord.x > bounds_max_x ||
        gl_FragCoord.y > bounds_min_y || gl_FragCoord.y < bounds_max_y) {
        discard;
    }

    if (vertex_texture_position.z > -1.0f) {
        vec4 texture_colour = texture(textures, vertex_texture_position);

        /* sprite pixel is not transparent */
        if (texture_colour.w == 1.0) {
            /* mask colour is set (-1.0 is no-mask) and texture pixel is grey */
            if (vertex_colour.x >= 0.0 &&
                texture_colour.x == texture_colour.y &&
                texture_colour.x == texture_colour.z) {
                texture_colour *= vertex_colour;
            }

            if (vertex_skin_colour.x >= 0.0 && texture_colour.x == 1.0 &&
                texture_colour.y == texture_colour.z) {
                texture_colour *= vec4(vertex_skin_colour, 1.0);
            }

            /* set alpha with or without mask colour */
            texture_colour.w = vertex_colour.w;
        }

        fragment_colour = texture_colour;
    } else {
        fragment_colour = vertex_colour;
    }
}
