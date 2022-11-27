#ifndef _H_GAME_MODEL
#define _H_GAME_MODEL

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

extern float gl_tri_face_us[];
extern float gl_tri_face_vs[];

extern float gl_quad_face_us[];
extern float gl_quad_face_vs[];

typedef struct gl_face_fill {
    float r, g, b;
    int texture_index;
} gl_face_fill;
#endif

#ifdef RENDER_GL
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>

#include "gl/vertex-buffer.h"

typedef struct gl_model_vertex {
    float x, y, z;
    float normal_x, normal_y, normal_z, normal_magnitude;
    float face_intensity, vertex_intensity;
    float front_r, front_g, front_b;
    float front_texture_u, front_texture_v;
    float back_r, back_g, back_b;
    float back_texture_u, back_texture_v;
} gl_model_vertex;
#endif

#ifdef RENDER_3DS_GL
#include <citro3d.h>

typedef struct _3ds_gl_model_vertex {
    float x, y, z;
    float normal_x, normal_y, normal_z, normal_magnitude;
    float face_intensity, vertex_intensity;
    float front_r, front_g, front_b, front_a;
    float back_r, back_g, back_b, back_a;
    float front_texture_u, front_texture_v, back_texture_u, back_texture_v;
} _3ds_gl_model_vertex;

// TODO could also use in surface i guess
// essentially a vao
typedef struct _3ds_gl_vertex_buffer {
    C3D_AttrInfo attr_info;
    C3D_BufInfo buf_info;
    void *vbo;
    void *ebo;
} _3ds_gl_vertex_buffer;
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

#ifdef RENDER_GL
#include "gl/textures/model_textures.h"
#endif

#ifdef RENDER_3DS_GL
#include "textures/model-textures.h"
#endif

typedef struct GameModel {
    uint16_t vertex_count;
    int *project_vertex_x;
    int *project_vertex_y;
    int *project_vertex_z;
    int *vertex_view_x;
    int *vertex_view_y;
    int *vertex_intensity;
    int8_t *vertex_ambience;
    uint16_t face_count;
    int *face_vertex_count;
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

    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int min_z;
    int max_z;

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
    uint16_t max_vertices;
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

    uint16_t max_faces;

    int base_x;
    int base_y;
    int base_z;
    int orientation_yaw;
    int orientation_pitch;
    int orientation_roll;
    int transform_type;
    int transform_state;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    int gl_vbo_offset;
    int gl_ebo_offset;
    int gl_ebo_length;

    int gl_invisible;

    mat4 transform;
#endif
#ifdef RENDER_GL
    gl_vertex_buffer *gl_buffer;

#ifdef EMSCRIPTEN
    int gl_pick_vbo_offset;
    int gl_pick_ebo_offset;
#endif
#elif defined(RENDER_3DS_GL)
    _3ds_gl_vertex_buffer *_3ds_gl_buffer;
#endif
} GameModel;

void game_model_new(GameModel *game_model);
void game_model_from2(GameModel *game_model, int vertex_count, int face_count);
void game_model_from2a(GameModel *game_model, GameModel **pieces, int count);
void game_model_from6(GameModel *game_model, GameModel **pieces, int count,
                      int autocommit, int isolated, int unlit, int unpickable);
void game_model_from7(GameModel *game_model, int vertex_count, int face_count,
                      int autocommit, int isolated, int unlit, int unpickable,
                      int projected);
void game_model_from_bytes(GameModel *game_model, int8_t *data, int offset);
void game_model_reset(GameModel *game_model);
void game_model_allocate(GameModel *game_model, int vertex_count,
                         int face_count);
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
                              int *src_vertices, int vertex_count, int in_face);
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
void game_model_determine_transform_type(GameModel *game_model);
void game_model_apply_translate(GameModel *game_model, int dx, int dy, int dz);
void game_model_apply_rotation(GameModel *game_model, int yaw, int roll,
                               int pitch);
void game_model_compute_bounds(GameModel *game_model);
void game_model_get_face_normals(GameModel *game_model, int *vertex_x,
                                 int *vertex_y, int *vertex_z,
                                 int *face_normal_x, int *face_normal_y,
                                 int *face_normal_z, int reset_scale);
void game_model_get_vertex_normals(GameModel *game_model, int *face_normal_x,
                                   int *face_normal_y, int *face_normal_z,
                                   int *normal_x, int *normal_y, int *normal_z,
                                   int *normal_magnitude);
void game_model_light(GameModel *game_model);
void game_model_relight(GameModel *game_model);
void game_model_reset_transform(GameModel *game_model);
void game_model_apply(GameModel *game_model);
void game_model_project_view(GameModel *game_model, int camera_x, int camera_y,
                             int camera_z, int camera_pitch, int camera_roll,
                             int camera_yaw, int view_distance, int clip_near);
void game_model_project(GameModel *game_model, int camera_x, int camera_y,
                        int camera_z, int camera_pitch, int camera_roll,
                        int camera_yaw, int view_distance, int clip_near);
void game_model_commit(GameModel *game_model);
GameModel *game_model_copy(GameModel *game_model);
GameModel *game_model_copy_from4(GameModel *game_model, int autocommit,
                                 int isolated, int unlit, int pickable);
void game_model_copy_position(GameModel *game_model, GameModel *source);
void game_model_destroy(GameModel *game_model);
void game_model_dump(GameModel *game_model, char *file_name);
void game_model_mask_faces(GameModel *game_model, int *face_fill,
                           int mask_colour);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
void game_model_gl_decode_face_fill(int face_fill, gl_face_fill *vbo_face_fill);
void game_model_gl_unwrap_uvs(GameModel *game_model, int *face_vertices,
                              int face_vertex_count, float *us, float *vs);
void gl_offset_texture_uvs_atlas(gl_atlas_position texture_position,
                                 float *texture_x, float *texture_y);
void game_model_gl_buffer_arrays(GameModel *game_model, int *vertex_offset,
                                 int *ebo_offset);
void game_model_get_vertex_ebo_lengths(GameModel **game_models, int length,
                                       int *vertex_count, int *ebo_length);
float game_model_gl_intersects(GameModel *game_model, vec3 ray_start,
                               vec3 ray_end);
#endif
#ifdef RENDER_GL
void game_model_gl_create_buffer(gl_vertex_buffer *vertex_buffer,
                                 int vbo_length, int ebo_length);
void game_model_gl_buffer_models(gl_vertex_buffer *vertex_buffer,
                                 GameModel **game_models, int length);
#ifdef EMSCRIPTEN
void game_model_gl_create_pick_vao(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                   int vbo_length, int ebo_length);
void game_model_gl_buffer_pick_arrays(GameModel *game_model, int *vertex_offset,
                                      int *ebo_offset);
void game_model_gl_buffer_pick_models(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                      GameModel **game_models, int length);
#endif
#elif defined(RENDER_3DS_GL)
void game_model_3ds_gl_create_buffers(_3ds_gl_vertex_buffer *buffer,
                                      int vbo_length, int ebo_length);
void game_model_3ds_gl_buffer_models(_3ds_gl_vertex_buffer *buffer,
                                     GameModel **game_models, int length);
void _3ds_gl_offset_texture_uvs_atlas(_3ds_gl_atlas_position texture_position, float *texture_x, float *texture_y);
#endif
#endif
