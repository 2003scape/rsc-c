#version 330 core

#define RAMP_SIZE 256

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 lighting;
layout(location = 3) in vec4 colour;
layout(location = 4) in vec3 texture_position;

out vec4 vertex_colour;
out vec3 vertex_texture_position;

uniform float light_gradient[RAMP_SIZE];
uniform float texture_light_gradient[RAMP_SIZE];

uniform mat4 model;
uniform mat4 view_model;
uniform mat4 projection_view_model;

uniform bool unlit;

uniform int light_ambience;
uniform vec3 light_direction;
uniform int light_diffuse;
uniform int light_direction_magnitude;

uniform bool cull_front;

void main() {
    int intensity = int(lighting.x);
    int normal_magnitude = int(lighting.y);

    if (!unlit) {
        vec3 model_normal = vec3(model * vec4(vec3(normal), 0.0));

        int divisor = (light_diffuse * light_direction_magnitude) / RAMP_SIZE;

        intensity = int(dot(model_normal * 1000, light_direction * 1000) /
                        (divisor * normal_magnitude));
    }

    int gradient_index = light_ambience;

    if (cull_front) {
        gradient_index += intensity;
    } else {
        gradient_index -= intensity;
    }

    if (gradient_index > (RAMP_SIZE - 1)) {
        gradient_index = (RAMP_SIZE - 1);
    } else if (gradient_index < 0) {
        gradient_index = 0;
    }

    float lightness = 1;

    if (texture_position.z > -1) {
        lightness = texture_light_gradient[gradient_index];
    } else {
        lightness = light_gradient[gradient_index];
    }

    gl_Position = projection_view_model * vec4(position, 1.0);

    vertex_colour = vec4(vec3(colour) * lightness, colour.w);
    vertex_texture_position = texture_position;

    /*if (gl_Position.z > (2500.0f / 1000.0f)) {
        vertex_colour = vec4(0, 0, 0, 1);
    }*/
}
