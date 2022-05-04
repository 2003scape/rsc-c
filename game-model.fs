#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

uniform sampler2DArray textures;

void main() {
    if (vertex_texture_position.z > -1) {
        fragment_colour = texture(textures, vertex_texture_position);
    } else {
        //fragment_colour = vec4(0.0, 0, 1.0, 1.0);
        fragment_colour = vertex_colour;
    }
}
