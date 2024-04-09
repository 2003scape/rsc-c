precision highp float;

#define RAMP_SIZE 256

varying vec3 vertex_colour;
//varying vec2 vertex_texture_position;
//varying float vertex_gradient_index;

//uniform sampler2D model_texture;

//uniform float light_gradient[RAMP_SIZE];
//uniform float texture_light_gradient[RAMP_SIZE];

//uniform float opacity;

void main() {
    gl_FragColor = vec4(vertex_colour, 1.0);
}
