#ifndef _H_VERTEX_BUFFER
#define _H_VERTEX_BUFFER

#include <GL/glew.h>
#include <GL/glu.h>

typedef struct gl_vertex_buffer {
    int vertex_length;
    int attribute_index;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} gl_vertex_buffer;

void vertex_buffer_gl_new(gl_vertex_buffer *vertex_buffer, int vertex_length);
void vertex_buffer_gl_bind(gl_vertex_buffer *vertex_buffer);
void vertex_buffer_gl_add_attribute(gl_vertex_buffer *vertex_buffer,
                                 int *attribute_offset, int attribute_length);

#endif
