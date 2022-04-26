#version 330 core
out vec4 fragment_colour;

in vec3 texture_position;

uniform sampler2DArray textures;

void main() {
    fragment_colour = texture(textures, texture_position);
    //fragment_colour = vec4(1.0, 0.0, 1.0, 1.0);
}
