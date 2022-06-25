#ifndef _H_GAME_MODEL
#define _H_GAME_MODEL

#include <math.h>
#include <stdint.h>
#include <stdio.h> // TODO remove
#include <stdlib.h>

#ifdef RENDER_GL
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>
#endif

/* states */
#define GAME_MODEL_TRANSFORM_BEGIN 1
#define GAME_MODEL_TRANSFORM_RESET 2

/* types */
#define GAME_MODEL_TRANSFORM_TRANSLATE 1
#define GAME_MODEL_TRANSFORM_ROTATE 2

#define GAME_MODEL_USE_GOURAUD 12345678
#define COLOUR_TRANSPARENT 12345678

typedef struct GameModel GameModel;

#include "scene.h"
#include "utility.h"

typedef struct GameModel {
    int num_vertices;
    int *project_vertex_x;
    int *project_vertex_y;
    int *project_vertex_z;
    int *vertex_view_x;
    int *vertex_view_y;
    int *vertex_intensity;
    int8_t *vertex_ambience;
    int num_faces;
    int *face_num_vertices;
    int **face_vertices;
    int *face_fill_front;
    int *face_fill_back;
    int *normal_magnitude;
    int *normal_scale;
    int *face_intensity;
    int *face_normal_x;
    int *face_normal_y;
    int *face_normal_z;
    int depth;
    int8_t visible;

    // TODO rename to min/max
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;

    /* used for walls */
    int8_t unpickable;

    /* used to identify the model in mouse picking. stores entity index */
    int key;

    /* used to determine which face is selected for mouse picking. used with
     * world->local_x and world->local_y */
    int *face_tag;

    int8_t transparent;
    int8_t *is_local_player;
    int8_t isolated;
    int8_t projected;
    int max_verts; // TODO max_vertices
    int *vertex_x;
    int *vertex_y;
    int *vertex_z;
    int *vertex_transformed_x;
    int *vertex_transformed_y;
    int *vertex_transformed_z;

    int8_t unlit;
    int light_ambience;
    int light_diffuse;
    int light_direction_x;
    int light_direction_y;
    int light_direction_z;
    int light_direction_magnitude;

    /* treat vertex_ arrays as vertex_transformed_. used for geneated terrain,
     * wall and roof models */
    int8_t autocommit;

    int max_faces;

    int base_x;
    int base_y;
    int base_z;
    int orientation_yaw;
    int orientation_pitch;
    int orientation_roll;
    int transform_kind; // TODO rename to type
    int transform_state;

#ifdef RENDER_GL
    // TODO prefix gl_
    GLuint vao;

    int vbo_offset;
    int ebo_offset;
    int ebo_length;

    mat4 transform;
#endif

    int test;
} GameModel;

void game_model_new(GameModel *game_model);
void game_model_from2(GameModel *game_model, int num_vertices, int num_faces);
void game_model_from2a(GameModel *game_model, GameModel **pieces, int count);
void game_model_from6(GameModel *game_model, GameModel **pieces, int count,
                      int autocommit, int isolated, int unlit, int unpickable);
void game_model_from7(GameModel *game_model, int num_vertices, int num_faces,
                      int autocommit, int isolated, int unlit, int unpickable,
                      int projected);
void game_model_from_bytes(GameModel *game_model, int8_t *data, int offset);
void game_model_reset(GameModel *game_model);
void game_model_allocate(GameModel *game_model, int num_vertices,
                         int num_faces);
void game_model_projection_prepare(GameModel *game_model);
void game_model_clear(GameModel *game_model);
void game_model_reduce(GameModel *game_model, int delta_faces,
                       int delta_vertices);
void game_model_merge(GameModel *game_model, GameModel **pieces, int count);
int game_model_vertex_at(GameModel *game_model, int x, int y, int z);
int game_model_create_vertex(GameModel *game_model, int x, int y, int z);
int game_model_create_face(GameModel *game_model, int number, int *vertices,
                           int fill_front, int fill_back);
void game_model_split(GameModel *game_model, GameModel **pieces, int piece_dx,
                      int piece_dz, int rows, int count, int piece_max_vertices,
                      int pickable);
void game_model_copy_lighting(GameModel *game_model, GameModel *model,
                              int *src_vertices, int num_vertices, int in_face);
void game_model_set_light_from3(GameModel *game_model, int x, int y, int z);
void game_model_set_light_from5(GameModel *game_model, int ambience,
                                int diffuse, int x, int y, int z);
void game_model_set_light_from6(GameModel *game_model, int gouraud,
                                int ambience, int diffuse, int x, int y, int z);
void game_model_set_vertex_ambience(GameModel *game_model, int vertex_index,
                                    int ambience);
void game_model_orient(GameModel *game_model, int yaw, int pitch, int roll);
void game_model_rotate(GameModel *game_model, int yaw, int pitch, int roll);
void game_model_place(GameModel *game_model, int x, int y, int z);
void game_model_translate(GameModel *game_model, int x, int y, int z);
void game_model_determine_transform_kind(GameModel *game_model);
void game_model_apply_translate(GameModel *game_model, int dx, int dy, int dz);
void game_model_apply_rotation(GameModel *game_model, int yaw, int roll,
                               int pitch);
void game_model_compute_bounds(GameModel *game_model);
void game_model_get_face_normals(GameModel *game_model, int *vertex_x,
                                 int *vertex_y, int *vertex_z,
                                 int *face_normal_x, int *face_normal_y,
                                 int *face_normal_z, int reset_scale);
void game_model_get_vertex_normals(GameModel *game_model,
        int *face_normal_x,
        int *face_normal_y,
        int *face_normal_z,
        int *normal_x,
        int *normal_y, int *normal_z,
        int *normal_magnitude);
void game_model_light(GameModel *game_model);
void game_model_relight(GameModel *game_model);
void game_model_reset_transform(GameModel *game_model);
void game_model_apply(GameModel *game_model);
void game_model_project(GameModel *game_model, int camera_x, int camera_y,
                        int camera_z, int camera_pitch, int camera_roll,
                        int camera_yaw, int view_distance, int clip_near);
void game_model_commit(GameModel *game_model);
GameModel *game_model_copy(GameModel *game_model);
GameModel *game_model_copy_from4(GameModel *game_model, int autocommit,
                                 int isolated, int unlit, int pickable);
void game_model_copy_position(GameModel *game_model, GameModel *source);
void game_model_destroy(GameModel *game_model);
void game_model_dump(GameModel *game_model, int i);

#ifdef RENDER_GL
void game_model_gl_create_vao(GLuint *vao, GLuint *vbo, GLuint *ebo,
                              int vbo_length, int ebo_length);
void game_model_gl_unwrap_uvs(GameModel *game_model, int *face_vertices,
                              int face_num_vertices, GLfloat *us, GLfloat *vs);
void game_model_gl_decode_face_fill(int face_fill, float *r, float *g,
                                    float *b, float *a, float *texture_index);
void game_model_gl_buffer_arrays(GameModel *game_model, int *vertex_offset,
                                 int *ebo_offset);
void game_model_gl_buffer_models(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                 GameModel **game_models, int length);
float game_model_gl_intersects(GameModel *game_model, vec3 ray_start, vec3 ray_end);
#endif
#endif
