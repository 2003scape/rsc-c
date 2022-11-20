#include "vertex-buffer.h"

void vertex_buffer_gl_new(gl_vertex_buffer *vertex_buffer, int vertex_length) {
    vertex_buffer->vertex_length = vertex_length;

    if (vertex_buffer->vao) {
        vertex_buffer->attribute_index = 0;

        glDeleteVertexArrays(1, &vertex_buffer->vao);
        glDeleteBuffers(1, &vertex_buffer->vbo);
        glDeleteBuffers(1, &vertex_buffer->ebo);
    }

    glGenVertexArrays(1, &vertex_buffer->vao);
    glGenBuffers(1, &vertex_buffer->vbo);
    glGenBuffers(1, &vertex_buffer->ebo);

    vertex_buffer_gl_bind(vertex_buffer);
}

void vertex_buffer_gl_bind(gl_vertex_buffer *vertex_buffer) {
    glBindVertexArray(vertex_buffer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer->ebo);
}

void vertex_buffer_gl_add_attribute(gl_vertex_buffer *vertex_buffer,
                                 int *attribute_offset, int attribute_length) {
#ifdef RENDER_GL
    glVertexAttribPointer(vertex_buffer->attribute_index, attribute_length,
                          GL_FLOAT, GL_FALSE, vertex_buffer->vertex_length,
                          (void *)((*attribute_offset) * sizeof(GLfloat)));

    glEnableVertexAttribArray(vertex_buffer->attribute_index);

    *attribute_offset += attribute_length;
    vertex_buffer->attribute_index++;
#endif
}

