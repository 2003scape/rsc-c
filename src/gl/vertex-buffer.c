#include "vertex-buffer.h"

// TODO vertex_length = vertex_size
void vertex_buffer_gl_new(gl_vertex_buffer *vertex_buffer, int vertex_length,
                          int vbo_length, int ebo_length) {
    vertex_buffer->vertex_length = vertex_length;

    if (vertex_buffer->attribute_index != 0) {
        vertex_buffer->attribute_index = 0;

        vertex_buffer_gl_destroy(vertex_buffer);
    }

    printf("creating new vertex buffer %d %d: %d\n", vbo_length, vertex_length, vbo_length * vertex_length);

    printf("creating new ebo %d: %d\n", ebo_length, ebo_length * sizeof(uint16_t));

#ifdef RENDER_GL
    glGenVertexArrays(1, &vertex_buffer->vao);
    glGenBuffers(1, &vertex_buffer->vbo);
    glGenBuffers(1, &vertex_buffer->ebo);
#elif defined(RENDER_3DS_GL)
    vertex_buffer->vbo = linearAlloc(vbo_length * vertex_length);
    vertex_buffer->ebo = linearAlloc(ebo_length * sizeof(uint16_t));

    if (!vertex_buffer->vbo) {
        fprintf(stderr, "oh no\n");

        while (1) {
            delay_ticks(100);
        }
    }

    AttrInfo_Init(&vertex_buffer->attr_info);
    BufInfo_Init(&vertex_buffer->buf_info);
#endif

#ifdef RENDER_GL
    vertex_buffer_gl_bind(vertex_buffer);

    glBufferData(GL_ARRAY_BUFFER, vbo_length * vertex_length, NULL,
                 GL_DYNAMIC_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_length * sizeof(GLuint), NULL,
                 GL_DYNAMIC_DRAW);
#endif
}

void vertex_buffer_gl_bind(gl_vertex_buffer *vertex_buffer) {
#ifdef RENDER_GL
    glBindVertexArray(vertex_buffer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer->ebo);
#elif defined(RENDER_3DS_GL)
    C3D_SetAttrInfo(&vertex_buffer->attr_info);
    C3D_SetBufInfo(&vertex_buffer->buf_info);
#endif
}

void vertex_buffer_gl_add_attribute(gl_vertex_buffer *vertex_buffer,
                                    int *attribute_offset,
                                    int attribute_length) {
#ifdef RENDER_GL
    glVertexAttribPointer(vertex_buffer->attribute_index, attribute_length,
                          GL_FLOAT, GL_FALSE, vertex_buffer->vertex_length,
                          (void *)((*attribute_offset) * sizeof(GLfloat)));

    glEnableVertexAttribArray(vertex_buffer->attribute_index);
#elif defined(RENDER_3DS_GL)
    AttrInfo_AddLoader(&vertex_buffer->attr_info,
                       vertex_buffer->attribute_index, GPU_FLOAT,
                       attribute_length);
#endif

    *attribute_offset += attribute_length;
    vertex_buffer->attribute_index++;
}

void vertex_buffer_gl_destroy(gl_vertex_buffer *vertex_buffer) {
    if (vertex_buffer == NULL) {
        return;
    }

#ifdef RENDER_GL
        glDeleteVertexArrays(1, &vertex_buffer->vao);
        glDeleteBuffers(1, &vertex_buffer->vbo);
        glDeleteBuffers(1, &vertex_buffer->ebo);
#elif defined(RENDER_3DS_GL)
        linearFree(vertex_buffer->vbo);
        linearFree(vertex_buffer->ebo);
#endif
}
