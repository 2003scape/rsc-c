#version 330 core

#define RAMP_SIZE 256
#define USE_GOURAUD 12345678

out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;
in float vertex_gradient_index;
flat in int foggy;

uniform float vertex_scale;

uniform sampler2DArray textures;

uniform bool interlace;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

uniform bool unlit;
uniform int light_ambience;
uniform vec3 light_direction;
uniform float light_diffuse;
uniform float light_direction_magnitude;

uniform bool cull_front;

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

    if (vertex_texture_position.z > -1) {
        fragment_colour = texture(textures, vertex_texture_position);

        if (vertex_colour.x > -1) {
            fragment_colour *= vertex_colour;
        }

        if (foggy == 1) {
            lightness = light_gradient[gradient_index];
        } else {
            lightness = texture_light_gradient[gradient_index];
        }
    } else {
        fragment_colour = vertex_colour;
        lightness = light_gradient[gradient_index];
    }

    if (fragment_colour.w <= 0.0) {
        discard;
    }

    fragment_colour = vec4(vec3(fragment_colour) * lightness, fragment_colour.w);
}
