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

    #ifdef OPENGL15
    GLhandleARB vertex = glCreateShaderObjectARB(GL_VERTEX_SHADER);
    glShaderSourceARB(vertex, 1, &vertex_shader_code, NULL);
    glCompileShaderARB(vertex);

    int success = 0;

    glGetObjectParameterfvARB(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetInfoLogARB(vertex, 512, NULL, info_log);
        fprintf(stderr, "vertex shader error: %s\n", info_log);
        exit(1);
    }

    GLhandleARB fragment = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
    glShaderSourceARB(fragment, 1, &fragment_shader_code, NULL);
    glCompileShaderARB(fragment);

    glGetObjectParameterfvARB(fragment, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetInfoLogARB(fragment, 512, NULL, info_log);
        fprintf(stderr, "fragment shader error: %s\n", info_log);
        exit(1);
    }

    int id = glCreateProgramObjectARB();
    glAttachObjectARB(id, vertex);
    glAttachObjectARB(id, fragment);

    //flats
    glBindAttribLocationARB(id, 0, "position");
    glBindAttribLocationARB(id, 1, "colour");
    glBindAttribLocationARB(id, 2, "texture_position");
    glBindAttribLocationARB(id, 3, "base_texture_position");
    //game_models
    glBindAttribLocationARB(id, 1, "normal");
    glBindAttribLocationARB(id, 2, "lighting");
    glBindAttribLocationARB(id, 3, "front_colour");
    glBindAttribLocationARB(id, 4, "front_texture_position");
    glBindAttribLocationARB(id, 5, "back_colour");
    glBindAttribLocationARB(id, 6, "back_texture_position");

    glLinkProgramARB(id);

    glGetObjectParameterfvARB(id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetInfoLogARB(id, 512, NULL, info_log);
        fprintf(stderr, "shader link error: %s\n", info_log);
        exit(1);
    }

    shader->id = id;

    glDeleteObjectARB(vertex);
    glDeleteObjectARB(fragment);

    #else //OpenGL2+
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

    #ifdef OPENGL20
    //flats
    glBindAttribLocation(id, 0, "position");
    glBindAttribLocation(id, 1, "colour");
    glBindAttribLocation(id, 2, "texture_position");
    glBindAttribLocation(id, 3, "base_texture_position");
    //game_models
    glBindAttribLocation(id, 1, "normal");
    glBindAttribLocation(id, 2, "lighting");
    glBindAttribLocation(id, 3, "front_colour");
    glBindAttribLocation(id, 4, "front_texture_position");
    glBindAttribLocation(id, 5, "back_colour");
    glBindAttribLocation(id, 6, "back_texture_position");
    #endif

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
    #endif

    free((char *)vertex_shader_code);
    free((char *)fragment_shader_code);
}

void shader_use(Shader *shader) { 
    #ifdef OPENGL15
    glUseProgramObjectARB(shader->id);
    #else
    glUseProgram(shader->id);
    #endif
    }

void shader_set_int(Shader *shader, char *name, int value) {
    #ifdef OPENGL15
    glUniform1iARB(glGetUniformLocationARB(shader->id, name), value);
    #else
    glUniform1i(glGetUniformLocation(shader->id, name), value);
    #endif
}

void shader_set_float(Shader *shader, char *name, float value) {
    #ifdef OPENGL15
    glUniform1fARB(glGetUniformLocationARB(shader->id, name), value);
    #else
    glUniform1f(glGetUniformLocation(shader->id, name), value);
    #endif
}

void shader_set_float_array(Shader *shader, char *name, float *values,
                            int length) {
    #ifdef OPENGL15
    glUniform1fvARB(glGetUniformLocationARB(shader->id, name), length,
                 (float *)values);
    #else
    glUniform1fv(glGetUniformLocation(shader->id, name), length,
                 (float *)values);
    #endif
}

void shader_set_mat4(Shader *shader, char *name, mat4 value) {
    #ifdef OPENGL15
    glUniformMatrix4fvARB(glGetUniformLocationARB(shader->id, name), 1, GL_FALSE,
                       (float *)value);
    #else
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE,
                       (float *)value);
    #endif
}

void shader_set_vec3(Shader *shader, char *name, vec3 value) {
    #ifdef OPENGL15
    glUniform3fvARB(glGetUniformLocationARB(shader->id, name), 1, (float *)value);
    #else
    glUniform3fv(glGetUniformLocation(shader->id, name), 1, (float *)value);
    #endif
}

void shader_set_vec3_array(Shader *shader, char *name, vec3 *values,
                           int length) {
    #ifdef OPENGL15
    glUniform3fvARB(glGetUniformLocationARB(shader->id, name), length,
                 (float *)values);
    #else
    glUniform3fv(glGetUniformLocation(shader->id, name), length,
                 (float *)values);
    #endif
}
#endif
