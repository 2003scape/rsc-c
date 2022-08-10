#version 300 es

#define RAMP_SIZE 256
#define USE_GOURAUD 12345678

precision mediump float;

out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;
in float vertex_gradient_index;
flat in int foggy;

uniform mediump sampler2DArray textures;

uniform bool interlace;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

void main() {
    if (interlace && int(gl_FragCoord.y) % 2 == 0) {
        discard;
    }

    float lightness = 1.0f;
    int gradient_index = int(round(vertex_gradient_index * float(RAMP_SIZE)));

    if (gradient_index > (RAMP_SIZE - 1)) {
        gradient_index = (RAMP_SIZE - 1);
    } else if (gradient_index < 0) {
        gradient_index = 0;
    }

    if (vertex_texture_position.z > -1.0f) {
        fragment_colour = texture(textures, vertex_texture_position);

        if (vertex_colour.x > -1.0f) {
            fragment_colour *= vertex_colour;
        }

        lightness = foggy == 1 ? light_gradient[gradient_index]
                               : texture_light_gradient[gradient_index];
    } else {
        fragment_colour = vertex_colour;
        lightness = light_gradient[gradient_index];
    }

    if (fragment_colour.w <= 0.0f) {
        discard;
    }

    fragment_colour =
        vec4(vec3(fragment_colour) * lightness, fragment_colour.w);
}
