#version 330 core

#define RAMP_SIZE 256

out vec4 fragment_colour;

in vec3 vertex_colour;
in vec2 vertex_texture_position;
in float vertex_gradient_index;

uniform sampler2D model_texture;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

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

    if (is_textured_light) {
        lightness = texture_light_gradient[gradient_index];
    } else {
        //float reversed = RAMP_SIZE - gradient_index - 1;
        //lightness = (reversed * reversed) / 65536.0;
        lightness = light_gradient[gradient_index];
    }

    // TODO add uniform for merlin's crystal
    fragment_colour = vec4(vertex_colour, 1.0) * texture_colour;

    fragment_colour =
        vec4(vec3(fragment_colour) * lightness, fragment_colour.w);
}
