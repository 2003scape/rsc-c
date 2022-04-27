#version 330 core
out vec4 fragment_colour;

in vec4 vertex_colour;
in vec3 vertex_texture_position;

uniform sampler2DArray textures;

void main() {
    if (vertex_texture_position.z > -1) {
        vec4 texture_colour = texture(textures, vertex_texture_position);

        if (vertex_colour.w == 1.0 && texture_colour.w == 1.0 &&
            texture_colour.x == texture_colour.y &&
            texture_colour.x == texture_colour.z) {
            texture_colour *= vertex_colour;
        }

        fragment_colour = texture_colour;
    } else {
        fragment_colour = vec4(vertex_colour);
    }
}
