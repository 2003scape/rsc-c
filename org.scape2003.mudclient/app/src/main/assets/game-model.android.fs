precision mediump float;

#define RAMP_SIZE 256

varying vec3 vertex_colour;
varying vec2 vertex_texture_position;
varying float vertex_gradient_index;

uniform sampler2D model_texture;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

uniform float opacity;

void main() {
    float lightness = 1.0;
    int gradient_index = int(vertex_gradient_index);

    vec4 texture_colour = texture2D(model_texture, vertex_texture_position);

    if (texture_colour.w <= 0.0) {
        discard;
    }

    bool is_textured_light =
        !(texture_colour.r == 1.0 && texture_colour.g == 1.0 &&
          texture_colour.b == 1.0);

    lightness = is_textured_light ? texture_light_gradient[gradient_index]
                                  : light_gradient[gradient_index];

    gl_FragColor = vec4(vertex_colour, opacity) * texture_colour;

    gl_FragColor =
        vec4(vec3(gl_FragColor) * lightness, gl_FragColor.a);
}
