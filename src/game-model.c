#include "game-model.h"

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
float gl_tri_face_us[] = {0.0f, 1.0f, 0.0f};
float gl_tri_face_vs[] = {1.0f, 1.0f, 0.0f};

float gl_quad_face_us[] = {0.0f, 1.0f, 1.0f, 0.0f};
float gl_quad_face_vs[] = {1.0f, 1.0f, 0.0f, 0.0f};
#endif

void game_model_new(GameModel *game_model) {
    memset(game_model, 0, sizeof(GameModel));

    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;
    game_model->visible = 1;
    game_model->key = -1;
    game_model->light_ambience = 32; /* 256 is the maximum */
    game_model->light_diffuse = 512;
    game_model->light_direction_x = 180;
    game_model->light_direction_y = 155;
    game_model->light_direction_z = 95;
    game_model->light_direction_magnitude = 256;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    game_model->gl_ebo_offset = -1;
    glm_mat4_identity(game_model->transform);
#endif
}

void game_model_from2(GameModel *game_model, int vertex_count, int face_count) {
    game_model_new(game_model);
    game_model_allocate(game_model, vertex_count, face_count);
}

void game_model_from2a(GameModel *game_model, GameModel **pieces, int count) {
    game_model_new(game_model);
    game_model_merge(game_model, pieces, count);
}

void game_model_from6(GameModel *game_model, GameModel **pieces, int count,
                      int autocommit, int isolated, int unlit, int unpickable) {
    game_model_new(game_model);

    game_model->autocommit = autocommit;
    game_model->isolated = isolated;
    game_model->unlit = unlit;
    game_model->unpickable = unpickable;

    game_model_merge(game_model, pieces, count);
}

void game_model_from7(GameModel *game_model, int vertex_count, int face_count,
                      int autocommit, int isolated, int unlit, int unpickable,
                      int projected) {
    game_model_new(game_model);

    game_model->autocommit = autocommit;
    game_model->isolated = isolated;
    game_model->unlit = unlit;
    game_model->unpickable = unpickable;
    game_model->projected = projected;

    game_model_allocate(game_model, vertex_count, face_count);
}

void game_model_from_bytes(GameModel *game_model, int8_t *data, int offset) {
    game_model_new(game_model);

    int vertex_count = get_unsigned_short(data, offset);
    offset += 2;

    int face_count = get_unsigned_short(data, offset);
    offset += 2;

    game_model_allocate(game_model, vertex_count, face_count);

    for (int i = 0; i < vertex_count; i++) {
        game_model->vertex_x[i] = get_signed_short(data, offset);
        offset += 2;
    }

    for (int i = 0; i < vertex_count; i++) {
        game_model->vertex_y[i] = get_signed_short(data, offset);
        offset += 2;
    }

    for (int i = 0; i < vertex_count; i++) {
        game_model->vertex_z[i] = get_signed_short(data, offset);
        offset += 2;
    }

    game_model->vertex_count = vertex_count;

    for (int i = 0; i < face_count; i++) {
        game_model->face_vertex_count[i] = data[offset++] & 0xff;
    }

    for (int i = 0; i < face_count; i++) {
        game_model->face_fill_front[i] = get_signed_short(data, offset);
        offset += 2;

        if (game_model->face_fill_front[i] == 32767) {
            game_model->face_fill_front[i] = COLOUR_TRANSPARENT;
        }
    }

    for (int i = 0; i < face_count; i++) {
        game_model->face_fill_back[i] = get_signed_short(data, offset);
        offset += 2;

        if (game_model->face_fill_back[i] == 32767) {
            game_model->face_fill_back[i] = COLOUR_TRANSPARENT;
        }
    }

    for (int i = 0; i < face_count; i++) {
        int is_gouraud = data[offset++] & 0xff;
        game_model->face_intensity[i] = is_gouraud ? GAME_MODEL_USE_GOURAUD : 0;
    }

    for (int i = 0; i < face_count; i++) {
        game_model->face_vertices[i] =
            malloc(game_model->face_vertex_count[i] * sizeof(int));

        for (int j = 0; j < game_model->face_vertex_count[i]; j++) {
            if (vertex_count < 256) {
                game_model->face_vertices[i][j] = data[offset++] & 0xff;
            } else {
                game_model->face_vertices[i][j] =
                    get_unsigned_short(data, offset);

                offset += 2;
            }
        }
    }

    game_model->face_count = face_count;
}

void game_model_reset(GameModel *game_model) {
    game_model->base_x = 0;
    game_model->base_y = 0;
    game_model->base_z = 0;
    game_model->orientation_yaw = 0;
    game_model->orientation_pitch = 0;
    game_model->orientation_roll = 0;
    game_model->transform_type = 0;
}

void game_model_allocate(GameModel *game_model, int vertex_count,
                         int face_count) {
    /* each terrain, wall and roof location gets a model, even if empty */
    if (vertex_count == 0) {
        return;
    }

    game_model->vertex_x = calloc(vertex_count, sizeof(int));
    game_model->vertex_y = calloc(vertex_count, sizeof(int));
    game_model->vertex_z = calloc(vertex_count, sizeof(int));
    game_model->vertex_intensity = calloc(vertex_count, sizeof(int));
    game_model->vertex_ambience = calloc(vertex_count, sizeof(int8_t));
    game_model->face_vertex_count = calloc(face_count, sizeof(int));
    game_model->face_vertices = calloc(face_count, sizeof(int *));
    game_model->face_fill_front = calloc(face_count, sizeof(int));
    game_model->face_fill_back = calloc(face_count, sizeof(int));
    game_model->face_intensity = calloc(face_count, sizeof(int));
    game_model->normal_scale = calloc(face_count, sizeof(int));
    game_model->normal_magnitude = calloc(face_count, sizeof(int));

    if (!game_model->projected) {
        game_model->project_vertex_x = calloc(vertex_count, sizeof(int));
        game_model->project_vertex_y = calloc(vertex_count, sizeof(int));
        game_model->project_vertex_z = calloc(vertex_count, sizeof(int));
        game_model->vertex_view_x = calloc(vertex_count, sizeof(int));
        game_model->vertex_view_y = calloc(vertex_count, sizeof(int));
    }

    if (!game_model->unpickable) {
        game_model->is_local_player = calloc(face_count, sizeof(int8_t));
        game_model->face_tag = calloc(face_count, sizeof(int));
    }

    if (game_model->autocommit) {
        game_model->vertex_transformed_x = game_model->vertex_x;
        game_model->vertex_transformed_y = game_model->vertex_y;
        game_model->vertex_transformed_z = game_model->vertex_z;
    } else {
        game_model->vertex_transformed_x = calloc(vertex_count, sizeof(int));
        game_model->vertex_transformed_y = calloc(vertex_count, sizeof(int));
        game_model->vertex_transformed_z = calloc(vertex_count, sizeof(int));
    }

    if (!game_model->unlit || !game_model->isolated) {
        game_model->face_normal_x = calloc(face_count, sizeof(int));
        game_model->face_normal_y = calloc(face_count, sizeof(int));
        game_model->face_normal_z = calloc(face_count, sizeof(int));
    }

    game_model->face_count = 0;
    game_model->vertex_count = 0;
    game_model->max_vertices = vertex_count;
    game_model->max_faces = face_count;

    game_model_reset(game_model);
}

void game_model_projection_prepare(GameModel *game_model) {
    if (game_model->vertex_count == 0) {
        return;
    }

    game_model->project_vertex_x =
        calloc(game_model->vertex_count, sizeof(int));

    game_model->project_vertex_y =
        calloc(game_model->vertex_count, sizeof(int));

    game_model->project_vertex_z =
        calloc(game_model->vertex_count, sizeof(int));

    game_model->vertex_view_x = calloc(game_model->vertex_count, sizeof(int));
    game_model->vertex_view_y = calloc(game_model->vertex_count, sizeof(int));
}

void game_model_clear(GameModel *game_model) {
    game_model->face_count = 0;
    game_model->vertex_count = 0;
}

void game_model_reduce(GameModel *game_model, int delta_faces,
                       int delta_vertices) {
    if (game_model->face_count - delta_faces < 0) {
        delta_faces = game_model->face_count;
    }

    for (int i = 1; i <= delta_faces; i++) {
        free(game_model->face_vertices[game_model->face_count - i]);
    }

    game_model->face_count -= delta_faces;

    if (game_model->vertex_count - delta_vertices < 0) {
        delta_vertices = game_model->vertex_count;
    }

    game_model->vertex_count -= delta_vertices;
}

void game_model_merge(GameModel *game_model, GameModel **pieces, int count) {
    int face_count = 0;
    int vertex_count = 0;

    for (int i = 0; i < count; i++) {
        face_count += pieces[i]->face_count;
        vertex_count += pieces[i]->vertex_count;
    }

    game_model_allocate(game_model, vertex_count, face_count);

    for (int i = 0; i < count; i++) {
        GameModel *source = pieces[i];
        game_model_commit(source);

        game_model->light_ambience = source->light_ambience;
        game_model->light_diffuse = source->light_diffuse;
        game_model->light_direction_x = source->light_direction_x;
        game_model->light_direction_y = source->light_direction_y;
        game_model->light_direction_z = source->light_direction_z;

        game_model->light_direction_magnitude =
            source->light_direction_magnitude;

        for (int src_f = 0; src_f < source->face_count; src_f++) {
            int *dst_vs =
                malloc(source->face_vertex_count[src_f] * sizeof(int));

            int *src_vs = source->face_vertices[src_f];

            for (int v = 0; v < source->face_vertex_count[src_f]; v++) {
                dst_vs[v] = game_model_vertex_at(
                    game_model, source->vertex_x[src_vs[v]],
                    source->vertex_y[src_vs[v]], source->vertex_z[src_vs[v]]);
            }

            int dst_f = game_model_create_face(
                game_model, source->face_vertex_count[src_f], dst_vs,
                source->face_fill_front[src_f], source->face_fill_back[src_f]);

            game_model->face_intensity[dst_f] = source->face_intensity[src_f];
            game_model->normal_scale[dst_f] = source->normal_scale[src_f];

            game_model->normal_magnitude[dst_f] =
                source->normal_magnitude[src_f];
        }
    }

    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;
}

int game_model_vertex_at(GameModel *game_model, int x, int y, int z) {
    for (int i = 0; i < game_model->vertex_count; i++) {
        if (game_model->vertex_x[i] == x && game_model->vertex_y[i] == y &&
            game_model->vertex_z[i] == z) {
            return i;
        }
    }

    return game_model_create_vertex(game_model, x, y, z);
}

int game_model_create_vertex(GameModel *game_model, int x, int y, int z) {
    if (game_model->vertex_count >= game_model->max_vertices) {
        return -1;
    }

    game_model->vertex_x[game_model->vertex_count] = x;
    game_model->vertex_y[game_model->vertex_count] = y;
    game_model->vertex_z[game_model->vertex_count] = z;

    return game_model->vertex_count++;
}

int game_model_create_face(GameModel *game_model, int number, int *vertices,
                           int fill_front, int fill_back) {
    if (game_model->face_count >= game_model->max_faces) {
        return -1;
    }

    game_model->face_vertex_count[game_model->face_count] = number;
    game_model->face_vertices[game_model->face_count] = vertices;
    game_model->face_fill_front[game_model->face_count] = fill_front;
    game_model->face_fill_back[game_model->face_count] = fill_back;
    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;

    return game_model->face_count++;
}

void game_model_split(GameModel *game_model, GameModel **pieces, int piece_dx,
                      int piece_dz, int rows, int count, int piece_max_vertices,
                      int pickable) {
    game_model_commit(game_model);

    int *piece_vertex_count = calloc(count, sizeof(int));
    int *piece_face_count = calloc(count, sizeof(int));

    for (int i = 0; i < game_model->face_count; i++) {
        int sum_x = 0;
        int sum_z = 0;
        int face_vertex_count = game_model->face_vertex_count[i];
        int *vertices = game_model->face_vertices[i];

        for (int i = 0; i < face_vertex_count; i++) {
            sum_x += game_model->vertex_x[vertices[i]];
            sum_z += game_model->vertex_z[vertices[i]];
        }

        int piece_index =
            ((int)(sum_x / (face_vertex_count * piece_dx))) +
            ((int)(sum_z / (face_vertex_count * piece_dz))) * rows;

        piece_vertex_count[piece_index] += face_vertex_count;
        piece_face_count[piece_index]++;
    }

    for (int i = 0; i < count; i++) {
        if (piece_vertex_count[i] > piece_max_vertices) {
            piece_vertex_count[i] = piece_max_vertices;
        }

        pieces[i] = malloc(sizeof(GameModel));

        game_model_from7(pieces[i], piece_vertex_count[i], piece_face_count[i],
                         1, 1, 1, pickable, 1);

        pieces[i]->light_diffuse = game_model->light_diffuse;
        pieces[i]->light_ambience = game_model->light_ambience;
    }

    free(piece_vertex_count);
    free(piece_face_count);

    for (int i = 0; i < game_model->face_count; i++) {
        int sum_x = 0;
        int sum_z = 0;
        int face_vertex_count = game_model->face_vertex_count[i];
        int *vertices = game_model->face_vertices[i];

        for (int i = 0; i < face_vertex_count; i++) {
            sum_x += game_model->vertex_x[vertices[i]];
            sum_z += game_model->vertex_z[vertices[i]];
        }

        int piece_index =
            ((int)(sum_x / (face_vertex_count * piece_dx))) +
            ((int)(sum_z / (face_vertex_count * piece_dz))) * rows;

        game_model_copy_lighting(game_model, pieces[piece_index], vertices,
                                 face_vertex_count, i);
    }

    for (int i = 0; i < count; i++) {
        game_model_projection_prepare(pieces[i]);
    }
}

void game_model_copy_lighting(GameModel *game_model, GameModel *model,
                              int *src_vertices, int vertex_count,
                              int in_face) {
    int *dest_vertices = malloc(vertex_count * sizeof(int));

    for (int i = 0; i < vertex_count; i++) {
        int vertex =
            game_model_vertex_at(model, game_model->vertex_x[src_vertices[i]],
                                 game_model->vertex_y[src_vertices[i]],
                                 game_model->vertex_z[src_vertices[i]]);

        dest_vertices[i] = vertex;

        model->vertex_intensity[vertex] =
            game_model->vertex_intensity[src_vertices[i]];

        model->vertex_ambience[vertex] =
            game_model->vertex_ambience[src_vertices[i]];
    }

    int out_face = game_model_create_face(model, vertex_count, dest_vertices,
                                          game_model->face_fill_front[in_face],
                                          game_model->face_fill_back[in_face]);

    if (!model->unpickable && !game_model->unpickable) {
        model->face_tag[out_face] = game_model->face_tag[in_face];
    }

    model->face_intensity[out_face] = game_model->face_intensity[in_face];
    model->normal_scale[out_face] = game_model->normal_scale[in_face];
    model->normal_magnitude[out_face] = game_model->normal_magnitude[in_face];
}

void game_model_set_light_from3(GameModel *game_model, int x, int y, int z) {
    if (game_model->unlit) {
        return;
    }

    game_model->light_direction_x = x;
    game_model->light_direction_y = y;
    game_model->light_direction_z = z;
    game_model->light_direction_magnitude = (int)sqrt(x * x + y * y + z * z);

    game_model_light(game_model);
}

void game_model_set_light_from5(GameModel *game_model, int ambience,
                                int diffuse, int x, int y, int z) {
    if (game_model->unlit) {
        return;
    }

    game_model->light_ambience = 256 - ambience * 4;
    game_model->light_diffuse = (64 - diffuse) * 16 + 128;

    game_model_set_light_from3(game_model, x, y, z);
}

void game_model_set_light_from6(GameModel *game_model, int gouraud,
                                int ambience, int diffuse, int x, int y,
                                int z) {
    if (game_model->unlit) {
        return;
    }

    for (int i = 0; i < game_model->face_count; i++) {
        game_model->face_intensity[i] = gouraud ? GAME_MODEL_USE_GOURAUD : 0;
    }

    game_model_set_light_from5(game_model, ambience, diffuse, x, y, z);
}

void game_model_set_vertex_ambience(GameModel *game_model, int vertex_index,
                                    int ambience) {
    game_model->vertex_ambience[vertex_index] = ambience & 0xff;
}

void game_model_orient(GameModel *game_model, int yaw, int pitch, int roll) {
    game_model->orientation_yaw = yaw & 255;
    game_model->orientation_pitch = pitch & 255;
    game_model->orientation_roll = roll & 255;
    game_model_determine_transform_type(game_model);
    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;
}

void game_model_rotate(GameModel *game_model, int yaw, int pitch, int roll) {
    game_model_orient(game_model, game_model->orientation_yaw + yaw,
                      game_model->orientation_pitch + pitch,
                      game_model->orientation_roll + roll);
}

void game_model_place(GameModel *game_model, int x, int y, int z) {
    game_model->base_x = x;
    game_model->base_y = y;
    game_model->base_z = z;
    game_model_determine_transform_type(game_model);
    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;
}

void game_model_translate(GameModel *game_model, int x, int y, int z) {
    game_model_place(game_model, game_model->base_x + x, game_model->base_y + y,
                     game_model->base_z + z);
}

void game_model_determine_transform_type(GameModel *game_model) {
    if (game_model->orientation_yaw != 0 ||
        game_model->orientation_pitch != 0 ||
        game_model->orientation_roll != 0) {
        game_model->transform_type = GAME_MODEL_TRANSFORM_ROTATE;
    } else if (game_model->base_x != 0 || game_model->base_y != 0 ||
               game_model->base_z != 0) {
        game_model->transform_type = GAME_MODEL_TRANSFORM_TRANSLATE;
    } else {
        game_model->transform_type = 0;
    }
}

void game_model_apply_translate(GameModel *game_model, int dx, int dy, int dz) {
    for (int i = 0; i < game_model->vertex_count; i++) {
        game_model->vertex_transformed_x[i] += dx;
        game_model->vertex_transformed_y[i] += dy;
        game_model->vertex_transformed_z[i] += dz;
    }
}

void game_model_apply_rotation(GameModel *game_model, int yaw, int roll,
                               int pitch) {
    for (int i = 0; i < game_model->vertex_count; i++) {
        if (pitch != 0) {
            int sin = sin_cos_512[pitch];
            int cos = sin_cos_512[pitch + 256];

            int x = (game_model->vertex_transformed_y[i] * sin +
                     game_model->vertex_transformed_x[i] * cos) >>
                    15;

            game_model->vertex_transformed_y[i] =
                (game_model->vertex_transformed_y[i] * cos -
                 game_model->vertex_transformed_x[i] * sin) >>
                15;

            game_model->vertex_transformed_x[i] = x;
        }

        if (yaw != 0) {
            int sin = sin_cos_512[yaw];
            int cos = sin_cos_512[yaw + 256];

            int y = (game_model->vertex_transformed_y[i] * cos -
                     game_model->vertex_transformed_z[i] * sin) >>
                    15;

            game_model->vertex_transformed_z[i] =
                (game_model->vertex_transformed_y[i] * sin +
                 game_model->vertex_transformed_z[i] * cos) >>
                15;

            game_model->vertex_transformed_y[i] = y;
        }

        if (roll != 0) {
            int sin = sin_cos_512[roll];
            int cos = sin_cos_512[roll + 256];

            int x = (game_model->vertex_transformed_z[i] * sin +
                     game_model->vertex_transformed_x[i] * cos) >>
                    15;

            game_model->vertex_transformed_z[i] =
                (game_model->vertex_transformed_z[i] * cos -
                 game_model->vertex_transformed_x[i] * sin) >>
                15;

            game_model->vertex_transformed_x[i] = x;
        }
    }
}

void game_model_compute_bounds(GameModel *game_model) {
    game_model->min_x = 999999;
    game_model->min_y = 999999;
    game_model->min_z = 999999;
    game_model->max_x = -999999;
    game_model->max_y = -999999;
    game_model->max_z = -999999;

    for (int i = 0; i < game_model->face_count; i++) {
        int *face_vertices = game_model->face_vertices[i];
        int vertex_index = face_vertices[0];
        int face_vertex_count = game_model->face_vertex_count[i];

#if !defined(RENDER_SW) && defined(RENDER_GL)
        vec3 vertex = {VERTEX_TO_FLOAT(game_model->vertex_x[vertex_index]),
                       VERTEX_TO_FLOAT(game_model->vertex_y[vertex_index]),
                       VERTEX_TO_FLOAT(game_model->vertex_z[vertex_index])};

        vec3 transformed_vertex = {0};
        glm_mat4_mulv3(game_model->transform, vertex, 1, transformed_vertex);

        int min_x = FLOAT_TO_VERTEX(transformed_vertex[0]);
        int min_y = FLOAT_TO_VERTEX(transformed_vertex[1]);
        int min_z = FLOAT_TO_VERTEX(transformed_vertex[2]);
#else
        int min_x = game_model->vertex_transformed_x[vertex_index];
        int min_y = game_model->vertex_transformed_y[vertex_index];
        int min_z = game_model->vertex_transformed_z[vertex_index];
#endif
        int max_x = min_x;
        int max_y = min_y;
        int max_z = min_z;

        for (int j = 0; j < face_vertex_count; j++) {
            vertex_index = face_vertices[j];

#if !defined(RENDER_SW) && defined(RENDER_GL)
            vec3 vertex = {0};
            vertex[0] = VERTEX_TO_FLOAT(game_model->vertex_x[vertex_index]);
            vertex[1] = VERTEX_TO_FLOAT(game_model->vertex_y[vertex_index]);
            vertex[2] = VERTEX_TO_FLOAT(game_model->vertex_z[vertex_index]);

            vec3 transformed_vertex = {0};
            glm_mat4_mulv3(game_model->transform, vertex, 1,
                           transformed_vertex);

            int vertex_transformed_x = FLOAT_TO_VERTEX(transformed_vertex[0]);
            int vertex_transformed_y = FLOAT_TO_VERTEX(transformed_vertex[1]);
            int vertex_transformed_z = FLOAT_TO_VERTEX(transformed_vertex[2]);
#else
            int vertex_transformed_x =
                game_model->vertex_transformed_x[vertex_index];

            int vertex_transformed_y =
                game_model->vertex_transformed_y[vertex_index];

            int vertex_transformed_z =
                game_model->vertex_transformed_z[vertex_index];
#endif

            if (vertex_transformed_x < min_x) {
                min_x = vertex_transformed_x;
            } else if (vertex_transformed_x > max_x) {
                max_x = vertex_transformed_x;
            }

            if (vertex_transformed_y < min_y) {
                min_y = vertex_transformed_y;
            } else if (vertex_transformed_y > max_y) {
                max_y = vertex_transformed_y;
            }

            if (vertex_transformed_z < min_z) {
                min_z = vertex_transformed_z;
            } else if (vertex_transformed_z > max_z) {
                max_z = vertex_transformed_z;
            }
        }

        if (min_x < game_model->min_x) {
            game_model->min_x = min_x;
        }

        if (max_x > game_model->max_x) {
            game_model->max_x = max_x;
        }

        if (min_y < game_model->min_y) {
            game_model->min_y = min_y;
        }

        if (max_y > game_model->max_y) {
            game_model->max_y = max_y;
        }

        if (min_z < game_model->min_z) {
            game_model->min_z = min_z;
        }

        if (max_z > game_model->max_z) {
            game_model->max_z = max_z;
        }
    }
}

void game_model_get_face_normals(GameModel *game_model, int *vertex_x,
                                 int *vertex_y, int *vertex_z,
                                 int *face_normal_x, int *face_normal_y,
                                 int *face_normal_z, int reset_scale) {
    for (int i = 0; i < game_model->face_count; i++) {
        int *face_vertices = game_model->face_vertices[i];

        int vertex_x_a = vertex_x[face_vertices[0]];
        int vertex_y_a = vertex_y[face_vertices[0]];
        int vertex_z_a = vertex_z[face_vertices[0]];
        int vertex_x_difference_ba = vertex_x[face_vertices[1]] - vertex_x_a;
        int vertex_y_difference_ba = vertex_y[face_vertices[1]] - vertex_y_a;
        int vertex_z_difference_ba = vertex_z[face_vertices[1]] - vertex_z_a;
        int vertex_x_difference_ca = vertex_x[face_vertices[2]] - vertex_x_a;
        int vertex_y_difference_ca = vertex_y[face_vertices[2]] - vertex_y_a;
        int vertex_z_difference_ca = vertex_z[face_vertices[2]] - vertex_z_a;

        int normal_x = vertex_y_difference_ba * vertex_z_difference_ca -
                       vertex_y_difference_ca * vertex_z_difference_ba;

        int normal_y = vertex_z_difference_ba * vertex_x_difference_ca -
                       vertex_z_difference_ca * vertex_x_difference_ba;

        int normal_z = 0;

        for (normal_z = vertex_x_difference_ba * vertex_y_difference_ca -
                        vertex_x_difference_ca * vertex_y_difference_ba;
             normal_x > 8192 || normal_y > 8192 || normal_z > 8192 ||
             normal_x < -8192 || normal_y < -8192 || normal_z < -8192;
             normal_z /= 2) {
            normal_x /= 2;
            normal_y /= 2;
        }

        int normal_magnitude =
            256 * sqrt(normal_x * normal_x + normal_y * normal_y +
                       normal_z * normal_z);

        if (normal_magnitude <= 0) {
            normal_magnitude = 1;
        }

        // << 16
        face_normal_x[i] = (normal_x * 65536) / normal_magnitude;
        face_normal_y[i] = (normal_y * 65536) / normal_magnitude;
        face_normal_z[i] = (normal_z * 65535) / normal_magnitude;

        if (reset_scale) {
            game_model->normal_scale[i] = -1;
        }
    }
}

void game_model_get_vertex_normals(GameModel *game_model, int *face_normal_x,
                                   int *face_normal_y, int *face_normal_z,
                                   int *normal_x, int *normal_y, int *normal_z,
                                   int *normal_magnitude) {
    for (int i = 0; i < game_model->face_count; i++) {
        if (game_model->face_intensity[i] == GAME_MODEL_USE_GOURAUD) {
            for (int j = 0; j < game_model->face_vertex_count[i]; j++) {
                int vertex_index = game_model->face_vertices[i][j];

                normal_x[vertex_index] += face_normal_x[i];
                normal_y[vertex_index] += face_normal_y[i];
                normal_z[vertex_index] += face_normal_z[i];

                normal_magnitude[vertex_index]++;
            }
        }
    }
}

void game_model_light(GameModel *game_model) {
    if (game_model->unlit) {
        return;
    }

    int divisor =
        (game_model->light_diffuse * game_model->light_direction_magnitude) >>
        8; // >> 8 is / 256

    for (int i = 0; i < game_model->face_count; i++) {
        if (game_model->face_intensity[i] != GAME_MODEL_USE_GOURAUD) {
            game_model->face_intensity[i] =
                (game_model->face_normal_x[i] * game_model->light_direction_x +
                 game_model->face_normal_y[i] * game_model->light_direction_y +
                 game_model->face_normal_z[i] * game_model->light_direction_z) /
                divisor;
        }
    }

    int *normal_x = calloc(game_model->vertex_count, sizeof(int));
    int *normal_y = calloc(game_model->vertex_count, sizeof(int));
    int *normal_z = calloc(game_model->vertex_count, sizeof(int));
    int *normal_magnitude = calloc(game_model->vertex_count, sizeof(int));

    game_model_get_vertex_normals(game_model, game_model->face_normal_x,
                                  game_model->face_normal_y,
                                  game_model->face_normal_z, normal_x, normal_y,
                                  normal_z, normal_magnitude);

    for (int i = 0; i < game_model->vertex_count; i++) {
        if (normal_magnitude[i] > 0) {
            game_model->vertex_intensity[i] =
                (normal_x[i] * game_model->light_direction_x +
                 normal_y[i] * game_model->light_direction_y +
                 normal_z[i] * game_model->light_direction_z) /
                (divisor * normal_magnitude[i]);
        }
    }

    free(normal_x);
    free(normal_y);
    free(normal_z);
    free(normal_magnitude);
}

void game_model_relight(GameModel *game_model) {
    if (game_model->unlit && game_model->isolated) {
        return;
    }

    game_model_get_face_normals(
        game_model, game_model->vertex_transformed_x,
        game_model->vertex_transformed_y, game_model->vertex_transformed_z,
        game_model->face_normal_x, game_model->face_normal_y,
        game_model->face_normal_z, 1);

    game_model_light(game_model);
}

void game_model_reset_transform(GameModel *game_model) {
    game_model->transform_state = 0;

    for (int i = 0; i < game_model->vertex_count; i++) {
        game_model->vertex_transformed_x[i] = game_model->vertex_x[i];
        game_model->vertex_transformed_y[i] = game_model->vertex_y[i];
        game_model->vertex_transformed_z[i] = game_model->vertex_z[i];
    }

#ifdef RENDER_GL
    glm_mat4_identity(game_model->transform);
#endif
}

void game_model_apply(GameModel *game_model) {
    if (game_model->transform_state == GAME_MODEL_TRANSFORM_RESET) {
        game_model_reset_transform(game_model);

        game_model->min_x = -9999999;
        game_model->min_y = -9999999;
        game_model->min_z = -9999999;
        game_model->max_x = 9999999;
        game_model->max_y = 9999999;
        game_model->max_z = 9999999;
    } else if (game_model->transform_state == GAME_MODEL_TRANSFORM_BEGIN) {
        game_model_reset_transform(game_model);

#ifdef RENDER_GL
        if (game_model->transform_type >= GAME_MODEL_TRANSFORM_TRANSLATE) {
            glm_translate(game_model->transform,
                          (vec3){VERTEX_TO_FLOAT(game_model->base_x),
                                 VERTEX_TO_FLOAT(game_model->base_y),
                                 VERTEX_TO_FLOAT(game_model->base_z)});
        }

        if (game_model->transform_type >= GAME_MODEL_TRANSFORM_ROTATE) {
            glm_rotate(game_model->transform,
                       TABLE_TO_RADIANS(game_model->orientation_pitch, 512),
                       (vec3){0.0f, 1.0f, 0.0f});

            glm_rotate(game_model->transform,
                       TABLE_TO_RADIANS(game_model->orientation_yaw, 512),
                       (vec3){1.0f, 0.0f, 0.0f});

            glm_rotate(game_model->transform,
                       TABLE_TO_RADIANS(game_model->orientation_roll, 512),
                       (vec3){0.0f, 0.0f, -1.0f});
        }

        /* fixes the z-fighting with the walls. check if faces > 1 so it
         * doesn't break wallobjects */
        if (!game_model->autocommit && game_model->face_count > 1) {
            glm_scale_uni(game_model->transform, 0.990f);
        }
#endif

#ifdef RENDER_SW
        if (game_model->transform_type >= GAME_MODEL_TRANSFORM_ROTATE) {
            game_model_apply_rotation(game_model, game_model->orientation_yaw,
                                      game_model->orientation_pitch,
                                      game_model->orientation_roll);
        }

        if (game_model->transform_type >= GAME_MODEL_TRANSFORM_TRANSLATE) {
            game_model_apply_translate(game_model, game_model->base_x,
                                       game_model->base_y, game_model->base_z);
        }
#endif

        game_model_compute_bounds(game_model);

        game_model_relight(game_model);
    }
}

void game_model_project_view(GameModel *game_model, int camera_x, int camera_y,
                             int camera_z, int camera_pitch, int camera_roll,
                             int camera_yaw, int view_distance, int clip_near) {
    int yaw_sin = 0;
    int yaw_cos = 0;
    int pitch_sin = 0;
    int pitch_cos = 0;
    int roll_sin = 0;
    int roll_cos = 0;

    if (camera_yaw != 0) {
        yaw_sin = sin_cos_2048[camera_yaw];
        yaw_cos = sin_cos_2048[camera_yaw + 1024];
    }

    if (camera_roll != 0) {
        roll_sin = sin_cos_2048[camera_roll];
        roll_cos = sin_cos_2048[camera_roll + 1024];
    }

    if (camera_pitch != 0) {
        pitch_sin = sin_cos_2048[camera_pitch];
        pitch_cos = sin_cos_2048[camera_pitch + 1024];
    }

    for (int i = 0; i < game_model->vertex_count; i++) {
        int x = game_model->vertex_transformed_x[i] - camera_x;
        int y = game_model->vertex_transformed_y[i] - camera_y;
        int z = game_model->vertex_transformed_z[i] - camera_z;

        if (camera_yaw != 0) {
            int X = (y * yaw_sin + x * yaw_cos) / 32768;
            y = (y * yaw_cos - x * yaw_sin) / 32768;
            x = X;
        }

        if (camera_roll != 0) {
            int X = (z * roll_sin + x * roll_cos) / 32768;
            z = (z * roll_cos - x * roll_sin) / 32768;
            x = X;
        }

        if (camera_pitch != 0) {
            int Y = (y * pitch_cos - z * pitch_sin) / 32768;
            z = (y * pitch_sin + z * pitch_cos) / 32768;
            y = Y;
        }

        if (z >= clip_near) {
            // game_model->vertex_view_x[i] = (int)((x << view_distance) / z);
            // game_model->vertex_view_y[i] = (int)((y << view_distance) / z);
            game_model->vertex_view_x[i] = (int)((x * view_distance) / z);
            game_model->vertex_view_y[i] = (int)((y * view_distance) / z);
        } else {
            // game_model->vertex_view_x[i] = x << view_distance;
            // game_model->vertex_view_y[i] = y << view_distance;
            game_model->vertex_view_x[i] = x * view_distance;
            game_model->vertex_view_y[i] = y * view_distance;
        }

        game_model->project_vertex_x[i] = x;
        game_model->project_vertex_y[i] = y;
        game_model->project_vertex_z[i] = z;
    }
}

void game_model_project(GameModel *game_model, int camera_x, int camera_y,
                        int camera_z, int camera_pitch, int camera_roll,
                        int camera_yaw, int view_distance, int clip_near) {
    game_model_apply(game_model);

    if (game_model->min_z > scene_frustum_near_z ||
        game_model->max_z < scene_frustum_far_z ||
        game_model->min_x > scene_frustum_min_x ||
        game_model->max_x < scene_frustum_max_x ||
        game_model->min_y > scene_frustum_min_y ||
        game_model->max_y < scene_frustum_max_y) {
        game_model->visible = 0;
        return;
    }

    game_model->visible = 1;

#ifdef RENDER_SW
    game_model_project_view(game_model, camera_x, camera_y, camera_z,
                            camera_pitch, camera_roll, camera_yaw,
                            view_distance, clip_near);
#endif
}

void game_model_commit(GameModel *game_model) {
    game_model_apply(game_model);

    for (int i = 0; i < game_model->vertex_count; i++) {
        game_model->vertex_x[i] = game_model->vertex_transformed_x[i];
        game_model->vertex_y[i] = game_model->vertex_transformed_y[i];
        game_model->vertex_z[i] = game_model->vertex_transformed_z[i];
    }

    game_model_reset(game_model);
}

GameModel *game_model_copy(GameModel *game_model) {
    GameModel **pieces = malloc(sizeof(GameModel *));
    pieces[0] = game_model;

    GameModel *copy = malloc(sizeof(GameModel));

    game_model_from2a(copy, pieces, 1);

    copy->depth = game_model->depth;
    copy->transparent = game_model->transparent;

#ifdef RENDER_GL
    copy->gl_vao = game_model->gl_vao;
    copy->gl_vbo_offset = game_model->gl_vbo_offset;
    copy->gl_ebo_offset = game_model->gl_ebo_offset;
    copy->gl_ebo_length = game_model->gl_ebo_length;
#endif

    free(pieces);

    return copy;
}

GameModel *game_model_copy_from4(GameModel *game_model, int autocommit,
                                 int isolated, int unlit, int pickable) {
    GameModel **pieces = malloc(sizeof(GameModel *));
    pieces[0] = game_model;

    GameModel *copy = malloc(sizeof(GameModel));
    game_model_from6(copy, pieces, 1, autocommit, isolated, unlit, pickable);

    copy->depth = game_model->depth;

#ifdef RENDER_GL
    copy->gl_vao = game_model->gl_vao;
    copy->gl_vbo_offset = game_model->gl_vbo_offset;
    copy->gl_ebo_offset = game_model->gl_ebo_offset;
    copy->gl_ebo_length = game_model->gl_ebo_length;
#endif

    free(pieces);

    return copy;
}

void game_model_copy_position(GameModel *game_model, GameModel *source) {
    game_model->orientation_yaw = source->orientation_yaw;
    game_model->orientation_pitch = source->orientation_pitch;
    game_model->orientation_roll = source->orientation_roll;
    game_model->base_x = source->base_x;
    game_model->base_y = source->base_y;
    game_model->base_z = source->base_z;

    game_model_determine_transform_type(game_model);

    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;

#ifdef RENDER_GL
    glm_mat4_copy(source->transform, game_model->transform);
#endif
}

void game_model_destroy(GameModel *game_model) {
    if (game_model == NULL) {
        return;
    }

    game_model->vertex_count = 0;

    for (int i = 0; i < game_model->face_count; i++) {
        free(game_model->face_vertices[i]);
        game_model->face_vertices[i] = NULL;
    }

    game_model->face_count = 0;

    free(game_model->face_vertices);
    game_model->face_vertices = NULL;

    if (game_model->vertex_x != game_model->vertex_transformed_x) {
        free(game_model->vertex_transformed_x);
    }

    free(game_model->vertex_x);
    game_model->vertex_x = NULL;
    game_model->vertex_transformed_x = NULL;

    if (game_model->vertex_y != game_model->vertex_transformed_y) {
        free(game_model->vertex_transformed_y);
    }

    free(game_model->vertex_y);
    game_model->vertex_y = NULL;
    game_model->vertex_transformed_y = NULL;

    if (game_model->vertex_z != game_model->vertex_transformed_z) {
        free(game_model->vertex_transformed_z);
    }

    free(game_model->vertex_z);
    game_model->vertex_z = NULL;
    game_model->vertex_transformed_z = NULL;

    free(game_model->vertex_intensity);
    game_model->vertex_intensity = NULL;

    free(game_model->vertex_ambience);
    game_model->vertex_ambience = NULL;

    free(game_model->face_vertex_count);
    game_model->face_vertex_count = NULL;

    free(game_model->face_vertices);
    game_model->face_vertices = NULL;

    free(game_model->face_fill_front);
    game_model->face_fill_front = NULL;

    free(game_model->face_fill_back);
    game_model->face_fill_back = NULL;

    free(game_model->face_intensity);
    game_model->face_intensity = NULL;

    free(game_model->normal_scale);
    game_model->normal_scale = NULL;

    free(game_model->normal_magnitude);
    game_model->normal_magnitude = NULL;

    free(game_model->project_vertex_x);
    game_model->project_vertex_x = NULL;

    free(game_model->project_vertex_y);
    game_model->project_vertex_y = NULL;

    free(game_model->project_vertex_z);
    game_model->project_vertex_z = NULL;

    free(game_model->vertex_view_x);
    game_model->vertex_view_x = NULL;

    free(game_model->vertex_view_y);
    game_model->vertex_view_y = NULL;

    free(game_model->is_local_player);
    game_model->is_local_player = NULL;

    free(game_model->face_tag);
    game_model->face_tag = NULL;

    free(game_model->vertex_transformed_y);
    game_model->vertex_transformed_y = NULL;

    free(game_model->vertex_transformed_z);
    game_model->vertex_transformed_z = NULL;

    free(game_model->vertex_transformed_x);
    game_model->vertex_transformed_x = NULL;

    free(game_model->vertex_transformed_y);
    game_model->vertex_transformed_y = NULL;

    free(game_model->vertex_transformed_z);
    game_model->vertex_transformed_z = NULL;

    free(game_model->face_normal_x);
    game_model->face_normal_x = NULL;

    free(game_model->face_normal_y);
    game_model->face_normal_y = NULL;

    free(game_model->face_normal_z);
    game_model->face_normal_z = NULL;

    free(game_model->project_vertex_x);
    game_model->project_vertex_x = NULL;

    free(game_model->project_vertex_y);
    game_model->project_vertex_y = NULL;

    free(game_model->project_vertex_z);
    game_model->project_vertex_z = NULL;

    free(game_model->vertex_view_x);
    game_model->vertex_view_x = NULL;

    free(game_model->vertex_view_y);
    game_model->vertex_view_y = NULL;
}

void game_model_dump(GameModel *game_model, char *file_name) {
    char name[255];

    sprintf(name, "./dump-%s.obj", file_name);

    FILE *obj_file = fopen(name, "w");

    for (int i = 0; i < game_model->vertex_count; i++) {
        float vertex_x = (((float)game_model->vertex_x[i]) / 1000.0f);
        float vertex_y = ((float)game_model->vertex_y[i]) / 1000.0f;
        float vertex_z = (((float)game_model->vertex_z[i]) / 1000.0f);

        fprintf(obj_file, "v %f %f %f\n", vertex_x, -vertex_y, vertex_z);
    }

    for (int i = 0; i < game_model->face_count; i++) {
        fprintf(obj_file, "f ");

        for (int j = 0; j < game_model->face_vertex_count[i]; j++) {
            fprintf(obj_file, "%d ", game_model->face_vertices[i][j] + 1);
        }

        fprintf(obj_file, "\n");
    }

    fclose(obj_file);
}

/* use the sprite masking technique on model faces */
void game_model_mask_faces(GameModel *game_model, int *face_fill,
                           int mask_colour) {
    for (int j = 0; j < game_model->face_count; j++) {
        int fill_colour = -1 - face_fill[j];
        int r = ((fill_colour >> 10) & 31) * 8;
        int g = ((fill_colour >> 5) & 31) * 8;
        int b = (fill_colour & 31) * 8;

        if (r == g && g == b) {
            float mask_r = ((mask_colour >> 16) & 0xff) / 255.0f;
            float mask_g = ((mask_colour >> 8) & 0xff) / 255.0f;
            float mask_b = (mask_colour & 0xff) / 255.0f;

            int new_r = (int)((float)r * mask_r) & 0xff;
            int new_g = (int)((float)g * mask_g) & 0xff;
            int new_b = (int)((float)b * mask_b) & 0xff;

            face_fill[j] =
                -(((new_r / 8) << 10) | ((new_g / 8) << 5) | (new_b / 8)) - 1;
        }
    }
}

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
void game_model_gl_unwrap_uvs(GameModel *game_model, int *face_vertices,
                              int face_vertex_count, float *us, float *vs) {
    if (face_vertex_count <= 4) {
        float *face_us = NULL;
        float *face_vs = NULL;

        if (face_vertex_count == 3) {
            face_us = gl_tri_face_us;
            face_vs = gl_tri_face_vs;
        } else if (face_vertex_count == 4) {
            face_us = gl_quad_face_us;
            face_vs = gl_quad_face_vs;
        }

        for (int i = 0; i < face_vertex_count; i++) {
            us[i] = face_us[i];
            vs[i] = face_vs[i];
        }

        return;
    }

    vec3 vertices[face_vertex_count];

    for (int i = 0; i < face_vertex_count; i++) {
        int vertex_index = face_vertices[i];

        vertices[i][0] = VERTEX_TO_FLOAT(game_model->vertex_x[vertex_index]);
        vertices[i][1] = VERTEX_TO_FLOAT(game_model->vertex_y[vertex_index]);
        vertices[i][2] = VERTEX_TO_FLOAT(game_model->vertex_z[vertex_index]);
    }

    vec3 location_x = {0};
    glm_vec3_sub(vertices[1], vertices[0], location_x);

    vec3 delta = {0};
    glm_vec3_sub(vertices[2], vertices[0], delta);

    // TODO get face normal
    vec3 normal = {0};
    glm_vec3_cross(location_x, delta, normal);

    vec3 location_y = {0};
    glm_vec3_cross(normal, location_x, location_y);

    glm_vec3_normalize(location_x);
    glm_vec3_normalize(location_y);

    float max_x = 0;
    float min_x = 0;

    float max_y = 0;
    float min_y = 0;

    for (int i = 0; i < face_vertex_count; i++) {
        vec3 vertex = {0};

        glm_vec3_sub(vertices[i], vertices[0], vertex);

        float x = glm_vec3_dot(vertex, location_x);
        float y = glm_vec3_dot(vertex, location_y);

        if (i == 0 || x > max_x) {
            max_x = x;
        }

        if (i == 0 || x < min_x) {
            min_x = x;
        }

        if (i == 0 || y > max_y) {
            max_y = y;
        }

        if (i == 0 || y < min_y) {
            min_y = y;
        }

        us[i] = x;
        vs[i] = y;
    }

    for (int i = 0; i < face_vertex_count; i++) {
        float x = us[i];
        float y = vs[i];

        us[i] = (x - min_x) / (max_x - min_x);
        vs[i] = 1.0f - (y - min_y) / (max_y - min_y);
    }
}

void game_model_gl_decode_face_fill(int face_fill,
                                    gl_face_fill *vbo_face_fill) {
    vbo_face_fill->r = 1.0f;
    vbo_face_fill->g = 1.0f;
    vbo_face_fill->b = 1.0f;
    vbo_face_fill->a = 1.0f;
    vbo_face_fill->texture_index = -1.0f;

    if (face_fill != COLOUR_TRANSPARENT) {
        if (face_fill < 0) {
            face_fill = -1 - face_fill;
            vbo_face_fill->r = (((face_fill >> 10) & 31) * 8) / 255.0f;
            vbo_face_fill->g = (((face_fill >> 5) & 31) * 8) / 255.0f;
            vbo_face_fill->b = ((face_fill & 31) * 8) / 255.0f;
        } else if (face_fill >= 0) {
            vbo_face_fill->texture_index = face_fill;
        }
    } else {
        vbo_face_fill->a = 0.0f;
    }
}

/* add a game model to VBO and EBO arrays at the specified offsets, then update
 * those offsets to new ones */
void game_model_gl_buffer_arrays(GameModel *game_model, int *vertex_offset,
                                 int *ebo_offset) {
    int *face_normal_x = calloc(game_model->face_count, sizeof(int));
    int *face_normal_y = calloc(game_model->face_count, sizeof(int));
    int *face_normal_z = calloc(game_model->face_count, sizeof(int));

    game_model_get_face_normals(game_model, game_model->vertex_x,
                                game_model->vertex_y, game_model->vertex_z,
                                face_normal_x, face_normal_y, face_normal_z, 0);

    int *vertex_normal_x = calloc(game_model->vertex_count, sizeof(int));
    int *vertex_normal_y = calloc(game_model->vertex_count, sizeof(int));
    int *vertex_normal_z = calloc(game_model->vertex_count, sizeof(int));

    int *vertex_normal_magnitude =
        calloc(game_model->vertex_count, sizeof(int));

    game_model_get_vertex_normals(game_model, face_normal_x, face_normal_y,
                                  face_normal_z, vertex_normal_x,
                                  vertex_normal_y, vertex_normal_z,
                                  vertex_normal_magnitude);

    for (int i = 0; i < game_model->face_count; i++) {
        int *face_vertices = game_model->face_vertices[i];
        int face_vertex_count = game_model->face_vertex_count[i];
        int face_intensity = game_model->face_intensity[i];
        int fill_front = game_model->face_fill_front[i];
        int fill_back = game_model->face_fill_back[i];

        gl_face_fill face_fill_front;
        game_model_gl_decode_face_fill(fill_front, &face_fill_front);

        gl_face_fill face_fill_back;
        game_model_gl_decode_face_fill(fill_back, &face_fill_back);

        float *front_face_us = NULL;
        float *front_face_vs = NULL;

        if (face_fill_front.texture_index > -1.0f) {
            front_face_us = alloca(face_vertex_count * sizeof(float));
            front_face_vs = alloca(face_vertex_count * sizeof(float));

            game_model_gl_unwrap_uvs(game_model, face_vertices,
                                     face_vertex_count, front_face_us,
                                     front_face_vs);
        }

        float *back_face_us = NULL;
        float *back_face_vs = NULL;

        if (face_fill_back.texture_index > -1.0f) {
            back_face_us = alloca(face_vertex_count * sizeof(float));
            back_face_vs = alloca(face_vertex_count * sizeof(float));

            game_model_gl_unwrap_uvs(game_model, face_vertices,
                                     face_vertex_count, back_face_us,
                                     back_face_vs);
        }

        for (int j = 0; j < face_vertex_count; j++) {
            int vertex_index = face_vertices[j];

            float vertex_x =
                VERTEX_TO_FLOAT(game_model->vertex_x[vertex_index]);

            float vertex_y =
                VERTEX_TO_FLOAT(game_model->vertex_y[vertex_index]);

            float vertex_z =
                VERTEX_TO_FLOAT(game_model->vertex_z[vertex_index]);

            vec3 normal = {0};
            int normal_magnitude = 1;

            if (face_intensity == GAME_MODEL_USE_GOURAUD) {
                normal[0] = VERTEX_TO_FLOAT(vertex_normal_x[vertex_index]);
                normal[1] = VERTEX_TO_FLOAT(vertex_normal_y[vertex_index]);
                normal[2] = VERTEX_TO_FLOAT(vertex_normal_z[vertex_index]);

                normal_magnitude = vertex_normal_magnitude[vertex_index];
            } else {
                normal[0] = VERTEX_TO_FLOAT(face_normal_x[i]);
                normal[1] = VERTEX_TO_FLOAT(face_normal_y[i]);
                normal[2] = VERTEX_TO_FLOAT(face_normal_z[i]);
            }

            int vertex_intensity = game_model->vertex_intensity[vertex_index] +
                                   game_model->vertex_ambience[vertex_index];

            float front_texture_x = -1.0f;
            float front_texture_y = -1.0f;

            if (front_face_us != NULL && front_face_vs != NULL) {
                front_texture_x = front_face_us[j];
                front_texture_y = 1.0f - front_face_vs[j];
            }

            float back_texture_x = -1.0f;
            float back_texture_y = -1.0f;

            if (back_face_us != NULL && back_face_vs != NULL) {
                back_texture_x = back_face_us[j];
                back_texture_y = 1.0f - back_face_vs[j];
            }

#ifdef RENDER_GL
            GLfloat vertex[] = {
                /* vertex */
                vertex_x, vertex_y, vertex_z, //

                /* normal */
                normal[0], normal[1], normal[2], (float)(normal_magnitude), //

                /* lighting */
                (float)(face_intensity), (float)(vertex_intensity), //

                /* front colour */
                face_fill_front.r, face_fill_front.g, face_fill_front.b,
                face_fill_front.a, //

                /* front texture */
                front_texture_x, front_texture_y, (float)face_fill_front.texture_index, //

                /* back colour */
                face_fill_back.r, face_fill_back.g, face_fill_back.b,
                face_fill_back.a, //

                /* back texture */
                back_texture_x, back_texture_y, (float)face_fill_back.texture_index //
            };

            glBufferSubData(GL_ARRAY_BUFFER,
                            ((*vertex_offset) + j) * 23 * sizeof(GLfloat),
                            23 * sizeof(GLfloat), vertex);
#endif
        }

        for (int j = 0; j < face_vertex_count - 2; j++) {
#ifdef RENDER_GL
            GLuint indices[] = {(*vertex_offset), (*vertex_offset) + j + 1,
                                (*vertex_offset) + j + 2};

            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                            (*ebo_offset) * sizeof(GLuint), sizeof(indices),
                            indices);
#endif

            (*ebo_offset) += 3;
        }

        (*vertex_offset) += face_vertex_count;
    }

    free(face_normal_x);
    free(face_normal_y);
    free(face_normal_z);

    free(vertex_normal_x);
    free(vertex_normal_y);
    free(vertex_normal_z);
    free(vertex_normal_magnitude);
}

void game_model_get_vertex_ebo_lengths(GameModel **game_models, int length,
                                       int *vertex_count, int *ebo_length) {
    for (int i = 0; i < length; i++) {
        GameModel *game_model = game_models[i];

        if (game_model == NULL) {
            continue;
        }

        game_model->gl_ebo_length = 0;

        for (int j = 0; j < game_model->face_count; j++) {
            int face_vertex_count = game_model->face_vertex_count[j];
            *vertex_count += face_vertex_count;
            game_model->gl_ebo_length += (face_vertex_count - 2) * 3;
        }

        *ebo_length += game_model->gl_ebo_length;
    }
}

float game_model_gl_intersects(GameModel *game_model, vec3 ray_direction,
                               vec3 ray_position) {
    float t[10] = {0};

    float min_vertex_x = VERTEX_TO_FLOAT(game_model->min_x);
    float max_vertex_x = VERTEX_TO_FLOAT(game_model->max_x);

    float min_vertex_y = VERTEX_TO_FLOAT(game_model->min_y);
    float max_vertex_y = VERTEX_TO_FLOAT(game_model->max_y);

    float min_vertex_z = VERTEX_TO_FLOAT(game_model->min_z);
    float max_vertex_z = VERTEX_TO_FLOAT(game_model->max_z);

    t[1] = (min_vertex_x - ray_position[0]) / ray_direction[0];
    t[2] = (max_vertex_x - ray_position[0]) / ray_direction[0];

    t[3] = (min_vertex_y - ray_position[1]) / ray_direction[1];
    t[4] = (max_vertex_y - ray_position[1]) / ray_direction[1];

    t[5] = (min_vertex_z - ray_position[2]) / ray_direction[2];
    t[6] = (max_vertex_z - ray_position[2]) / ray_direction[2];

    t[7] = fmax(fmax(fmin(t[1], t[2]), fmin(t[3], t[4])), fmin(t[5], t[6]));
    t[8] = fmin(fmin(fmax(t[1], t[2]), fmax(t[3], t[4])), fmax(t[5], t[6]));
    t[9] = (t[8] < 0 || t[7] > t[8]) ? -1 : t[7];

    return t[9];
}
#endif

#ifdef RENDER_GL
void game_model_gl_create_vao(GLuint *vao, GLuint *vbo, GLuint *ebo,
                              int vbo_length, int ebo_length) {
    if (*vao) {
        glDeleteVertexArrays(1, vao);
        glDeleteBuffers(1, vbo);
        glDeleteBuffers(1, ebo);
    }

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    glBufferData(GL_ARRAY_BUFFER, vbo_length * sizeof(GLfloat) * 23, NULL,
                 GL_STATIC_DRAW);

    /* vertex { x, y, z } */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)0);

    glEnableVertexAttribArray(0);

    /* normal { x, y, z, magnitude } */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(1);

    /* lighting { face_intensity, vertex_intensity } */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)(7 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);

    /* front colour { r, g, b, a } */
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)(9 * sizeof(GLfloat)));

    glEnableVertexAttribArray(3);

    /* front texture { s, t, index } */
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)(13 * sizeof(GLfloat)));

    glEnableVertexAttribArray(4);

    /* back colour { r, g, b, a } */
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)(16 * sizeof(GLfloat)));

    glEnableVertexAttribArray(5);

    /* back texture { s, t, index } */
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 23 * sizeof(GLfloat),
                          (void *)(20 * sizeof(GLfloat)));

    glEnableVertexAttribArray(6);

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_length * sizeof(GLuint), NULL,
                 GL_STATIC_DRAW);
}

/* calculate the length of the VBO and EBO arrays for a list of game models,
 * then populate them */
void game_model_gl_buffer_models(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                 GameModel **game_models, int length) {
    int vertex_offset = 0;
    int ebo_offset = 0;

    game_model_get_vertex_ebo_lengths(game_models, length, &vertex_offset,
                                      &ebo_offset);

    game_model_gl_create_vao(vao, vbo, ebo, vertex_offset, ebo_offset);

    vertex_offset = 0;
    ebo_offset = 0;

    for (int i = 0; i < length; i++) {
        GameModel *game_model = game_models[i];

        if (game_model == NULL) {
            continue;
        }

        game_model->gl_vao = *vao;
        game_model->gl_vbo_offset = vertex_offset;
        game_model->gl_ebo_offset = ebo_offset;

        game_model_gl_buffer_arrays(game_model, &vertex_offset, &ebo_offset);
    }
}

#ifdef EMSCRIPTEN
void game_model_gl_create_pick_vao(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                   int vbo_length, int ebo_length) {
    if (*vao) {
        glDeleteVertexArrays(1, vao);
        glDeleteBuffers(1, vbo);
        glDeleteBuffers(1, ebo);
    }

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    glBufferData(GL_ARRAY_BUFFER, vbo_length * sizeof(GLfloat) * 5, NULL,
                 GL_DYNAMIC_DRAW);

    /* vertex { x, y, z } */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (void *)0);

    glEnableVertexAttribArray(0);

    /* colour { r, g } */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(1);

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_length * sizeof(GLuint), NULL,
                 GL_DYNAMIC_DRAW);
}

void game_model_gl_buffer_pick_models(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                      GameModel **game_models, int length) {
    int vertex_offset = 0;
    int ebo_offset = 0;

    game_model_get_vertex_ebo_lengths(game_models, length, &vertex_offset,
                                      &ebo_offset);

    game_model_gl_create_pick_vao(vao, vbo, ebo, vertex_offset, ebo_offset);

    vertex_offset = 0;
    ebo_offset = 0;

    for (int i = 0; i < length; i++) {
        GameModel *game_model = game_models[i];

        game_model->gl_pick_vbo_offset = vertex_offset;
        game_model->gl_pick_ebo_offset = ebo_offset;

        game_model_gl_buffer_pick_arrays(game_model, &vertex_offset,
                                         &ebo_offset);
    }
}

void game_model_gl_buffer_pick_arrays(GameModel *game_model, int *vertex_offset,
                                      int *ebo_offset) {
    for (int i = 0; i < game_model->face_count; i++) {
        int *face_vertices = game_model->face_vertices[i];
        int face_vertex_count = game_model->face_vertex_count[i];

        int face_tag = game_model->face_tag[i] - TILE_FACE_TAG;
        float face_tag_r = (face_tag & 0xff) / 255.0f;
        float face_tag_g = ((face_tag >> 8) & 0xff) / 255.0f;

        for (int j = 0; j < face_vertex_count; j++) {
            int vertex_index = face_vertices[j];

            GLfloat vertex_x =
                VERTEX_TO_FLOAT(game_model->vertex_x[vertex_index]);

            GLfloat vertex_y =
                VERTEX_TO_FLOAT(game_model->vertex_y[vertex_index]);

            GLfloat vertex_z =
                VERTEX_TO_FLOAT(game_model->vertex_z[vertex_index]);

            GLfloat vertex[] = {
                /* vertex */
                vertex_x, vertex_y, vertex_z, //

                /* face tag */
                face_tag_r, face_tag_g //
            };

            glBufferSubData(GL_ARRAY_BUFFER,
                            ((*vertex_offset) + j) * 5 * sizeof(GLfloat),
                            5 * sizeof(GLfloat), vertex);
        }

        for (int j = 0; j < face_vertex_count - 2; j++) {
            GLuint indices[] = {(*vertex_offset), (*vertex_offset) + j + 1,
                                (*vertex_offset) + j + 2};

            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                            (*ebo_offset) * sizeof(GLuint), sizeof(indices),
                            indices);

            (*ebo_offset) += 3;
        }

        (*vertex_offset) += face_vertex_count;
    }
}
#endif
#endif

#ifdef RENDER_3DS_GL
void game_model_3ds_gl_create_buffers(_3ds_gl_vertex_buffer *buffer,
                                      int vbo_length, int ebo_length) {
    AttrInfo_Init(&buffer->attr_info);

    /* vertex { x, y, z } */
    AttrInfo_AddLoader(&buffer->attr_info, 0, GPU_FLOAT, 3);

    /* normal { x, y, z, magnitude */
    AttrInfo_AddLoader(&buffer->attr_info, 1, GPU_FLOAT, 4);

    /* lighting { face, vertex } */
    AttrInfo_AddLoader(&buffer->attr_info, 2, GPU_FLOAT, 2);

    /* front colour { r, g, b } */
    AttrInfo_AddLoader(&buffer->attr_info, 3, GPU_FLOAT, 3);

    /* back colour { r, g, b } */
    AttrInfo_AddLoader(&buffer->attr_info, 4, GPU_FLOAT, 3);

    /* textures { front_u, front_v, back_u, back_ v } */
    AttrInfo_AddLoader(&buffer->attr_info, 5, GPU_FLOAT, 4);

    // TODO make sure this works on null
    linearFree(buffer->vbo);
    linearFree(buffer->ebo);

    buffer->vbo = linearAlloc(vbo_length * sizeof(_3ds_gl_model_vertex));
    buffer->ebo = linearAlloc(ebo_length * sizeof(uint16_t));

    BufInfo_Init(&buffer->buf_info);

    BufInfo_Add(&buffer->buf_info, buffer->vbo, sizeof(_3ds_gl_flat_vertex), 6,
                0x543210);
}

void game_model_3ds_gl_buffer_models(_3ds_gl_vertex_buffer *buffer,
                                     GameModel **game_models, int length) {
    int vbo_offset = 0;
    int ebo_offset = 0;

    game_model_get_vertex_ebo_lengths(game_models, length, &vbo_offset,
                                      &ebo_offset);

    game_model_3ds_gl_create_buffers(buffer, vbo_offset, ebo_offset);

    vbo_offset = 0;
    ebo_offset = 0;

    for (int i = 0; i < length; i++) {
        GameModel *game_model = game_models[i];

        if (game_model == NULL) {
            continue;
        }

        game_model->_3ds_gl_buffer = buffer;
        game_model->gl_vbo_offset = vbo_offset;
        game_model->gl_ebo_offset = ebo_offset;

        game_model_gl_buffer_arrays(game_model, &vbo_offset, &ebo_offset);
    }
}
#endif
