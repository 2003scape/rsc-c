#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec3 texture_position;

out vec4 vertex_colour;
out vec3 vertex_texture_position;

out vec3 vertex_position;
out vec3 vertex_normal;

//out vec3 test_normal;
out float visibility;

uniform mat4 model;
uniform mat4 view;
//uniform mat4 projection;

uniform mat4 view_model;
uniform mat4 projection_view_model;

void main() {
    gl_Position = projection_view_model * vec4(position, 1.0);

    vertex_colour = colour;
    vertex_texture_position = texture_position;

    vertex_position = vec3(model * vec4(position, 1.0));
    vertex_normal = vec3(model * vec4(normal, 0.0));

    vec3 test_normal = vec3(view_model * vec4(normal, 0.0));
    vec4 test_position = view_model * vec4(position, 1.0);

    visibility = test_position.x * test_normal.x + test_position.y * test_normal.y + test_position.z * test_normal.z;

    //test_normal = mat3(transpose(inverse(model))) * normal;

    /*if (gl_Position.z > (2500.0f / 1000.0f)) {
        vertex_colour = vec4(0, 0, 0, 1);
    }*/
}

