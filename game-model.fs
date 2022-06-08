#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

in vec3 vertex_position;
in vec3 vertex_normal;

uniform sampler2DArray textures;

uniform float ambient;
uniform vec3 light_position;

void main() {
    if (vertex_texture_position.z > -1) {
        fragment_colour = texture(textures, vertex_texture_position);
        //fragment_colour = vec4(vec3(fragment_colour) * (ambient), fragment_colour.w);
    } else {
        fragment_colour = vertex_colour;

        /*fragment_colour.x = floor((fragment_colour.x * ambient) * 32) / 32.0f;
        fragment_colour.y = floor((fragment_colour.y * ambient) * 32) / 32.0f;
        fragment_colour.z = floor((fragment_colour.z * ambient) * 32) / 32.0f;*/

        fragment_colour = vec4(vec3(fragment_colour) * (ambient), fragment_colour.w);
    }

    if (fragment_colour.w <= 0.0) {
        discard;
    }

    //fragment_colour = vec4(vec3(fragment_colour) * (ambient), fragment_colour.w);

    /*vec3 light_direction = normalize(light_position - vertex_position);
    float diffuse = max(dot(vertex_normal, light_direction), 0.0);
    fragment_colour = vec4(vec3(fragment_colour) * (ambient + diffuse), fragment_colour.w);*/
}
