#ifndef _H_VERTEX_BUFFER
#define _H_VERTEX_BUFFER

#ifdef RENDER_GL
#include <GL/glew.h>
#include <GL/glu.h>
#elif defined(RENDER_3DS_GL)
#include <citro3d.h>
#include <tex3ds.h>
#endif

typedef struct gl_vertex_buffer {
    int vertex_length;
    int attribute_index;

#ifdef RENDER_GL
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
#elif defined(RENDER_3DS_GL)
    C3D_AttrInfo attr_info;
    C3D_BufInfo buf_info;
    void *vbo;
    void *ebo;
#endif
} gl_vertex_buffer;

void vertex_buffer_gl_new(gl_vertex_buffer *vertex_buffer, int vertex_length,
                          int vbo_length, int ebo_length);
void vertex_buffer_gl_bind(gl_vertex_buffer *vertex_buffer);
void vertex_buffer_gl_add_attribute(gl_vertex_buffer *vertex_buffer,
                                 int *attribute_offset, int attribute_length);
#endif
