#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec3 texture_position;

out vec4 vertex_colour;
out vec3 vertex_texture_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 test;

uniform vec3 translate_model;
uniform vec3 camera_pos;
uniform vec3 camera_dir;

void main() {
    // gl_Position = projection * view * model * (vec4(position, 1.0) - vec4(test, 0.0));
    // gl_Position = projection * view * vec4(position, 1.0);
    // gl_Position = vec4(position, 1.0);
    // gl_Position = vec4(64 * position, 1.0);

    /*float x = (position.x + translate_model.x) - camera_pos.x;
    float y = (position.y + translate_model.y) - camera_pos.y;
    float z = (position.z + translate_model.z) - camera_pos.z;*/

    //float Y = (y * cos(camera_dir.x) - z * sin(camera_dir.x));
    //z = (y * sin(camera_dir.x) + z * cos(camera_dir.x));
    //y = Y;
    //gl_Position = projection * view * vec4(x, y, z, 1.0);
    //gl_Position = projection * view * model * vec4(position, 1.0);

    gl_Position = projection * view * model * vec4(position, 1.0);

    vertex_colour = colour;
    vertex_texture_position = texture_position;
}

