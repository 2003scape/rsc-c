#version 330 core

#define FOUNTAIN_ID 17
#define RAMP_SIZE 256

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 lighting;
layout(location = 3) in vec4 front_colour;
layout(location = 4) in vec3 front_texture_position;
layout(location = 5) in vec4 back_colour;
layout(location = 6) in vec3 back_texture_position;

out vec4 vertex_colour;
out vec3 vertex_texture_position;

uniform float scroll_texture;

uniform int fog_distance;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

uniform mat4 model;
uniform mat4 projection_view_model;

uniform bool unlit;

uniform int light_ambience;
uniform vec3 light_direction;
uniform int light_diffuse;
uniform int light_direction_magnitude;

uniform bool cull_front;

void main() {
    gl_Position = projection_view_model * vec4(position, 1.0);

    int face_intensity = int(lighting.x);
    int vertex_intensity = int(lighting.y);
    int normal_magnitude = int(normal.w);
    int intensity = 0;

    if (unlit) {
        if (face_intensity == 12345678) {
            intensity = vertex_intensity;
        } else {
            intensity = face_intensity;
        }
    } else {
        vec3 model_normal = vec3(model * vec4(vec3(normal), 0.0));

        int divisor = (light_diffuse * light_direction_magnitude) / RAMP_SIZE;

        intensity = int(dot(model_normal * 1000, light_direction * 1000) /
                        (divisor * normal_magnitude));
    }

    int gradient_index = light_ambience;

    if (cull_front) {
        gradient_index += intensity;

        vertex_colour = back_colour;
        vertex_texture_position = back_texture_position;
    } else {
        gradient_index -= intensity;

        vertex_colour = front_colour;
        vertex_texture_position = front_texture_position;
    }

    if (gl_Position.z > (fog_distance / 1000.0f)) {
        gradient_index += int(gl_Position.z  * 1000) - fog_distance;
    }

    if (gradient_index > (RAMP_SIZE - 1)) {
        gradient_index = (RAMP_SIZE - 1);
    } else if (gradient_index < 0) {
        gradient_index = 0;
    }

    float lightness = 1;

    if (vertex_texture_position.z > -1) {
        lightness = texture_light_gradient[gradient_index];

        if (vertex_texture_position.z == FOUNTAIN_ID) {
            vertex_texture_position.y -= scroll_texture;
        }
    } else {
        lightness = light_gradient[gradient_index];
    }

    vertex_colour = vec4(vec3(vertex_colour) * lightness, vertex_colour.w);
}
