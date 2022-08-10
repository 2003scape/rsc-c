#version 330 core

#define PHONG_ENABLED 1

#define FOUNTAIN_ID 17
#define RAMP_SIZE 256
#define USE_GOURAUD 12345678

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 lighting;
layout(location = 3) in vec4 front_colour;
layout(location = 4) in vec3 front_texture_position;
layout(location = 5) in vec4 back_colour;
layout(location = 6) in vec3 back_texture_position;

out vec4 vertex_colour;
out vec3 vertex_texture_position;

flat out int foggy;
out vec2 test_lighting;
out float test_normal_magnitude;
out vec3 test_normal;
flat out mat4 test_model;

uniform float vertex_scale;

uniform float scroll_texture;

uniform int fog_distance;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

uniform mat4 model;
uniform mat4 projection_view_model;

uniform bool unlit;
uniform int light_ambience;
uniform vec3 light_direction;
uniform float light_diffuse;
uniform float light_direction_magnitude;

uniform bool cull_front;

void main() {
    gl_Position = projection_view_model * vec4(position, 1.0);

    test_lighting = vec2(lighting);
    test_normal_magnitude = normal.w;
    test_normal = vec3(normal);
    test_model = model;

    if (cull_front) {
        vertex_colour = back_colour;
        vertex_texture_position = back_texture_position;
    } else {
        vertex_colour = front_colour;
        vertex_texture_position = front_texture_position;
    }

    if (gl_Position.z > (fog_distance / float(vertex_scale))) {
        foggy = 1;
    } else {
        foggy = 0;
    }

    if (vertex_texture_position.z > -1.0f && vertex_texture_position.z == FOUNTAIN_ID) {
        vertex_texture_position.y -= scroll_texture;
    }

    vertex_colour = vec4(vec3(vertex_colour), vertex_colour.w);
}
