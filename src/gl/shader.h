#ifndef SHADER_H
#define SHADER_H
#ifdef RENDER_GL

#include <stdio.h>
#include <stdlib.h>

#ifdef GLAD
#include <glad/glad.h>
#else
#include <GL/glew.h>
#include <GL/glu.h>
#endif

#include <cglm/cglm.h>

#include "../utility.h"

#if defined(OPENGL15) && !defined(GLAD)
#define glCreateShader glCreateShaderObjectARB
#define glShaderSource glShaderSourceARB
#define glCompileShader glCompileShaderARB
#define glGetShaderiv glGetObjectParameterfvARB
#define glGetShaderInfoLog glGetInfoLogARB
#define glCreateProgram glCreateProgramObjectARB
#define glAttachShader glAttachObjectARB
#define glBindAttribLocation glBindAttribLocationARB
#define glLinkProgram glLinkProgramARB
#define glDeleteShader glDeleteObjectARB
#define glUseProgram glUseProgramObjectARB
#define glGetUniformLocation glGetUniformLocationARB
#define glUniform1i glUniform1iARB
#define glUniform1f glUniform1fARB
#define glUniform1fv glUniform1fvARB
#define glUniformMatrix4fv glUniformMatrix4fvARB
#define glUniform3fv glUniform3fvARB
#endif

typedef struct Shader {
    int id;
} Shader;

void shader_new(Shader *shader, char *vertex_path, char *fragment_path);
void shader_use(Shader *shader);
void shader_set_int(Shader *shader, char *name, int value);
void shader_set_float(Shader *shader, char *name, float value);
void shader_set_float_array(Shader *shader, char *name, float *values,
                            int length);
void shader_set_mat4(Shader *shader, char *name, mat4 value);
void shader_set_vec3(Shader *shader, char *name, vec3 value);
void shader_set_vec3_array(Shader *shader, char *name, vec3 *values,
                           int length);
#endif
#endif
