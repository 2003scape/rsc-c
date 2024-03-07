attribute vec3 position;
attribute vec4 colour;
attribute vec2 texture_position;
attribute vec2 base_texture_position;

varying vec4 vertex_colour;
varying vec2 vertex_texture_position;
varying vec2 vertex_base_texture_position;

void main() {
    gl_Position = vec4(position, 1.0);

    vertex_colour = colour;
    vertex_texture_position = texture_position;
    vertex_base_texture_position = base_texture_position;
}
