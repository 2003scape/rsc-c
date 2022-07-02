#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

uniform sampler2DArray textures;

uniform bool interlace;

void main() {
    /* why not :) */
    if (interlace && int(gl_FragCoord.y) % 2 == 0) {
        discard;
    }

    if (vertex_texture_position.z > -1) {
        fragment_colour = texture(textures, vertex_texture_position);

        if (vertex_colour.x > -1) {
            fragment_colour *= vertex_colour;
        }
    } else {
        fragment_colour = vertex_colour;
    }

    if (fragment_colour.w <= 0.0) {
        discard;
    }

    // TODO we can toggle this for the login screen
    /*fragment_colour.x = round(fragment_colour.x * 32) / 32.0f;
    fragment_colour.y = round(fragment_colour.y * 32) / 32.0f;
    fragment_colour.z = round(fragment_colour.z * 32) / 32.0f;*/

    /*float diffuse =
        clamp(dot(normalize(vertex_normal), normalize(light_direction)), 0.0f, 1.0f);*/
}
