#version 330 core

#define RAMP_SIZE 256
#define USE_GOURAUD 12345678

out vec4 fragment_colour;

in vec3 vertex_colour;
in vec2 vertex_texture_position;
in float vertex_gradient_index;
flat in int foggy;

uniform sampler2D textures;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

void main() {
    float lightness = 1.0f;
    int gradient_index = int(round(vertex_gradient_index * float(RAMP_SIZE)));

    if (gradient_index > (RAMP_SIZE - 1)) {
        gradient_index = (RAMP_SIZE - 1);
    } else if (gradient_index < 0) {
        gradient_index = 0;
    }

    /*if (vertex_texture_position.z >= 0.0f) {
        lightness = foggy == 1 ? light_gradient[gradient_index]
                               : texture_light_gradient[gradient_index];
    } else {
        lightness = light_gradient[gradient_index];
    }*/

    vec4 texture_colour = texture2D(textures, vertex_texture_position);

    // check if texture_colour is transparent, then use light_gradient
    // otherwise use texture_gradient

    fragment_colour = vec4(vertex_colour, 0.0) + texture_colour;

    if (fragment_colour.w <= 0.0f) {
        discard;
    }

    lightness = light_gradient[gradient_index];

    fragment_colour =
        vec4(vec3(fragment_colour) * lightness, fragment_colour.w);
}
