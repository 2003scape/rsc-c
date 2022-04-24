#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <cglm/cglm.h>

typedef struct Shader {
    int id;
} Shader;

void shader_new(Shader *shader, char *vertex_path, char *fragment_path);
void shader_use(Shader *shader);
void shader_set_int(Shader *shader, char *name, int value);
void shader_set_float(Shader *shader, char *name, float value);
void shader_set_mat4(Shader *shader, char *name, mat4 value);

#endif
