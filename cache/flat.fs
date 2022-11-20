#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec2 vertex_texture_position;
in vec2 vertex_base_texture_position;

uniform sampler2D texture;
uniform sampler2D base_texture;

uniform bool ui_scale;

void main() {
    float frag_x = gl_FragCoord.x;
    float frag_y = gl_FragCoord.y;

    if (ui_scale) {
        frag_x /= 2.0f;
        frag_y /= 2.0f;
    }

    vec4 texture_colour = texture2D(texture, vertex_texture_position);
    //vec4 base_texture_colour = texture2D(texture, vertex_base_texture_position);

    texture_colour *= vertex_colour;
    //fragment_colour = texture_colour + base_texture_colour;

    fragment_colour = texture_colour;

//    if (vertex_texture_position.z > -1) {
//        vec4 texture_colour = texture(textures, vertex_texture_position);
//
//        /* sprite pixel is not transparent */
//        if (texture_colour.w == 1.0) {
//            /* mask colour is set (-1.0 is no-mask) and texture pixel is grey */
//            if (vertex_colour.x >= 0.0 &&
//                texture_colour.x == texture_colour.y &&
//                texture_colour.x == texture_colour.z) {
//                texture_colour *= vertex_colour;
//            }
//
//            if (vertex_skin_colour.x >= 0.0 && texture_colour.x == 1.0 &&
//                texture_colour.y == texture_colour.z) {
//                texture_colour *= vec4(vertex_skin_colour, 1.0);
//            }
//
//            /* set alpha with or without mask colour */
//            texture_colour.w = vertex_colour.w;
//        }
//
//        fragment_colour = texture_colour;
//    } else {
//        fragment_colour = vertex_colour;
//    }

    if (fragment_colour.w <= 0.0) {
        discard;
    }
}
