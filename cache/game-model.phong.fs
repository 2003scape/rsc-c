#version 330 core

#define RAMP_SIZE 256
#define USE_GOURAUD 12345678

out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

in float test_normal_magnitude;
flat in int foggy;
in vec2 test_lighting;
in vec3 test_normal;
flat in mat4 test_model;

uniform float vertex_scale;

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

    int face_intensity = int(test_lighting.x);
    int vertex_intensity = int(test_lighting.y);
    float normal_magnitude = test_normal_magnitude;
    int intensity = 0;

    if (unlit) {
        if (face_intensity == USE_GOURAUD) {
            intensity = vertex_intensity;
        } else {
            intensity = face_intensity;
        }
    } else {
        vec3 model_normal = vec3(test_model * vec4(vec3(test_normal), 0.0));

        float divisor =
            (light_diffuse * light_direction_magnitude) / float(RAMP_SIZE);

        intensity = int(
            dot(model_normal * vertex_scale, light_direction * vertex_scale) /
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

    float lightness = 1.0f;

    if (vertex_texture_position.z > -1.0f) {
        // TODO light_gradient looks ok here too.
        lightness = foggy == 1 ? light_gradient[gradient_index]
                          : texture_light_gradient[gradient_index];

        // lightness = light_gradient[gradient_index];
        fragment_colour = texture(textures, vertex_texture_position);


        if (vertex_colour.x > -1) {
            fragment_colour *= vertex_colour;
        }
    } else {
        lightness = light_gradient[gradient_index];

        fragment_colour = vertex_colour;
    }

    //fragment_colour = vec4(vec3(vertex_colour) * lightness, vertex_colour.w);
    fragment_colour = vec4(vec3(fragment_colour) * lightness, fragment_colour.w);

    // TODO we can toggle this for the login screen
    /*fragment_colour.x = round(fragment_colour.x * 32) / 32.0f;
    fragment_colour.y = round(fragment_colour.y * 32) / 32.0f;
    fragment_colour.z = round(fragment_colour.z * 32) / 32.0f;*/
}
