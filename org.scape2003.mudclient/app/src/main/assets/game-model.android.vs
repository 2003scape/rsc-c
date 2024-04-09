precision highp float;

#define VERTEX_SCALE 100.0

#define RAMP_SIZE 256.0
#define USE_GOURAUD 32767.0

attribute vec3 position;
attribute vec4 normal;
attribute vec2 lighting;
attribute vec3 front_colour;
attribute vec2 front_texture_position;
attribute vec3 back_colour;
attribute vec2 back_texture_position;

varying vec3 vertex_colour;
//varying vec2 vertex_texture_position;
//varying float vertex_gradient_index;

uniform float scroll_texture;

uniform int fog_distance;

uniform mat4 model;
uniform mat4 projection_view_model;

uniform bool unlit;
uniform float light_ambience;
uniform vec3 light_direction;
uniform float light_diffuse;

uniform bool cull_front;

void main() {
    gl_Position = projection_view_model * vec4(position, 1.0);
    vertex_colour = vec3(back_colour.r, back_colour.g, back_colour.b);
}
