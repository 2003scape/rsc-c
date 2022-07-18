#version 330 core

#define PHONG_ENABLED 1

#define RAMP_SIZE 256

out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

in float test_index;
flat in int foggy;

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

    float lightness = 1;

    if (vertex_texture_position.z > -1.0f) {
        if (PHONG_ENABLED == 1) {
            int index = int(round(test_index * RAMP_SIZE));
            if (index > (RAMP_SIZE - 1)) {
                index = (RAMP_SIZE - 1);
            } else if (index < 0) {
                index = 0;
            }
            lightness = foggy == 1 ? light_gradient[index] : texture_light_gradient[index];
        }

        fragment_colour = texture(textures, vertex_texture_position);

        if (vertex_colour.x > -1.0f) {
            fragment_colour *= vertex_colour;
        }
    } else {
        if (PHONG_ENABLED == 1) {
            int index = int(round(test_index * RAMP_SIZE));
            if (index > (RAMP_SIZE - 1)) {
                index = (RAMP_SIZE - 1);
            } else if (index < 0) {
                index = 0;
            }
            lightness = light_gradient[index];
        }

        fragment_colour = vertex_colour;
    }

    if (fragment_colour.w <= 0.0f) {
        discard;
    }

    fragment_colour = vec4(vec3(fragment_colour) * lightness, fragment_colour.w);

    // TODO we can toggle this for the login screen
    /*fragment_colour.x = round(fragment_colour.x * 32) / 32.0f;
    fragment_colour.y = round(fragment_colour.y * 32) / 32.0f;
    fragment_colour.z = round(fragment_colour.z * 32) / 32.0f;*/
}
