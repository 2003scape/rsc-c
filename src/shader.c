#include "shader.h"

#ifdef RENDER_GL
char *buffer_file(char *path) {
    FILE *file = fopen(path, "r");

    if (!file) {
        fprintf(stderr, "unable to open file: %s\n", path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_buffer = calloc(size + 1, sizeof(char));
    unsigned int total_read_size = 0;

    do {
        int read_size = fread(file_buffer + total_read_size, 1024, 1, file);

        if (read_size <= 0) {
            break;
        }

        total_read_size += read_size * 1024;
    } while (total_read_size < size);

    fclose(file);

    return file_buffer;
}

void shader_new(Shader *shader, char *vertex_path, char *fragment_path) {
    char info_log[512] = {0};

    const char *vertex_shader_code = buffer_file(vertex_path);
    const char *fragment_shader_code = buffer_file(fragment_path);

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex);

    int success = 0;

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, info_log);
        fprintf(stderr, "vertex shader error: %s\n", info_log);
        exit(1);
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, info_log);
        fprintf(stderr, "fragment shader error: %s\n", info_log);
        exit(1);
    }

    int id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, info_log);
        fprintf(stderr, "shader link error: %s\n", info_log);
        exit(1);
    }

    shader->id = id;

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    free((char*)vertex_shader_code);
    free((char*)fragment_shader_code);
}

void shader_use(Shader *shader) {
    glUseProgram(shader->id);
}

void shader_set_int(Shader *shader, char *name, int value) {
    glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void shader_set_float(Shader *shader, char *name, float value) {
    glUniform1f(glGetUniformLocation(shader->id, name), value);
}

void shader_set_mat4(Shader *shader, char *name, mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, (float*)value);
}
#endif
