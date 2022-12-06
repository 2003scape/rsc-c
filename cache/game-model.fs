#version 330 core

#define RAMP_SIZE 256

out vec4 fragment_colour;

in vec3 vertex_colour;
in vec2 vertex_texture_position;
in float vertex_gradient_index;
flat in int is_textured_light;

uniform sampler2D model_texture;

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

    vec4 texture_colour = texture(model_texture, vertex_texture_position);

    if (texture_colour.w <= 0.0f) {
        discard;
    }

    //fragment_colour = vec4(vertex_colour, 0.0) + texture_colour;
    fragment_colour = vec4(vertex_colour, 1.0) * texture_colour;

    /*if (fragment_colour.w <= 0.0f) {
        discard;
    }*/

    if (is_textured_light == 1) {
        lightness = texture_light_gradient[gradient_index];
    } else {
        float reversed = RAMP_SIZE - gradient_index - 1;
        lightness = (reversed * reversed) / 65536.0;
        //lightness = light_gradient[gradient_index];
    }

    fragment_colour =
        vec4(vec3(fragment_colour) * lightness, fragment_colour.w);
}
