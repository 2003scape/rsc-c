precision mediump float;

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
varying vec2 vertex_texture_position;
varying float vertex_gradient_index;

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

    float face_intensity = lighting.x;
    float vertex_intensity = lighting.y;

    float intensity = lighting.y;
    float gradient_index = light_ambience;

    if (unlit) {
        intensity =
            face_intensity == USE_GOURAUD ? vertex_intensity : face_intensity;
    } else {
        vec3 model_normal = vec3(model * vec4(vec3(normal), 0.0));

        /* normal.w = normal_magnitude */
        intensity =
            dot(light_direction, model_normal) / (light_diffuse * normal.w);
    }

    if (cull_front) {
        gradient_index += intensity;
        vertex_colour = back_colour;
        vertex_texture_position = back_texture_position;
    } else {
        gradient_index -= intensity;
        vertex_colour = front_colour;
        vertex_texture_position = front_texture_position;
    }

    // TODO replace division with multiplication
    if (gl_Position.z > (float(fog_distance) / VERTEX_SCALE)) {
        gradient_index += gl_Position.z * VERTEX_SCALE - float(fog_distance);
    }

    gradient_index = max(0.0, min(gradient_index, RAMP_SIZE - 1.0));

    vertex_gradient_index = gradient_index;

    if (vertex_texture_position.x < 0.0) {
        vertex_texture_position.x *= -1.0;
        vertex_texture_position.y *= -1.0;

        vertex_texture_position.y += scroll_texture;
    }
}
