#version 330 core

#define RAMP_SIZE 256

out vec4 fragment_colour;

in vec3 vertex_colour;
in vec2 vertex_texture_position;
in float vertex_gradient_index;

uniform sampler2D model_texture;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

uniform float opacity;

/* the maximum number of uniforms we can have in webgl2 is 224 */

/*
float get_light_gradient(int gradient_index) {
    gradient_index = (RAMP_SIZE - 1) - gradient_index;

    return (gradient_index * gradient_index) / float(RAMP_SIZE * RAMP_SIZE);
}

float get_texture_gradient(int gradient_index) {
    gradient_index = ((RAMP_SIZE - 1) - gradient_index) / 16;

    int x = gradient_index / 4;
    int y = gradient_index % 4;

    return ((19 * pow(2, x)) + (4 * pow(2, x) * y)) / 255.0f;
}
*/

void main() {
    float lightness = 1.0f;
    int gradient_index = int(vertex_gradient_index);

    vec4 texture_colour = texture(model_texture, vertex_texture_position);

    if (texture_colour.w <= 0.0f) {
        discard;
    }

    bool is_textured_light =
        !(texture_colour.r == 1.0 && texture_colour.g == 1.0 &&
          texture_colour.b == 1.0);

    lightness = is_textured_light ? texture_light_gradient[gradient_index]
                                  : light_gradient[gradient_index];

    fragment_colour = vec4(vertex_colour, opacity) * texture_colour;

    fragment_colour =
        vec4(vec3(fragment_colour) * lightness, fragment_colour.w);
}
