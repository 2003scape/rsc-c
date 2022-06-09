#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

in vec3 vertex_position;
in vec3 vertex_normal;

uniform sampler2DArray textures;

uniform float vertex_ambience;
uniform float texture_ambience;
uniform vec3 light_position;

void main() {
    float ambience = 1;

    if (vertex_texture_position.z > -1) {
        fragment_colour = texture(textures, vertex_texture_position);
        ambience = texture_ambience;
    } else {
        fragment_colour = vertex_colour;
        ambience = vertex_ambience;
    }

    if (fragment_colour.w <= 0.0) {
        discard;
    }

    fragment_colour = vec4(vec3(fragment_colour) * (ambience), fragment_colour.w);

    /*vec3 light_direction = normalize(light_position - vertex_position);
    float diffuse = max(dot(vertex_normal, light_direction), 0.0);
    fragment_colour = vec4(vec3(fragment_colour) * (ambience + diffuse), fragment_colour.w);*/
}
