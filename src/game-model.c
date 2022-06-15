#include "game-model.h"

#ifdef RENDER_GL
float gl_tri_face_us[] = {0.0f, 1.0f, 0.0f};
float gl_tri_face_vs[] = {1.0f, 1.0f, 0.0f};

float gl_quad_face_us[] = {0.0f, 1.0f, 1.0f, 0.0f};
float gl_quad_face_vs[] = {1.0f, 1.0f, 0.0f, 0.0f};
#endif

void game_model_new(GameModel *game_model) {
    memset(game_model, 0, sizeof(GameModel));

    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;
    game_model->diameter = COLOUR_TRANSPARENT;
    game_model->visible = 1;
    game_model->key = -1;
    //game_model->light_ambience = 32; /* 256 is the maximum? */
    game_model->light_diffuse = 512;
    game_model->light_direction_x = 180;
    game_model->light_direction_y = 155;
    game_model->light_direction_z = 95;
    game_model->light_direction_magnitude = 256;

#ifdef RENDER_GL
    game_model->ebo_offset = -1;
    glm_mat4_identity(game_model->transform);
#endif
}

void game_model_from2(GameModel *game_model, int num_vertices, int num_faces) {
    game_model_new(game_model);
    game_model_allocate(game_model, num_vertices, num_faces);
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
    //game_model->unlit = 0;
    game_model->unpickable = unpickable;

    game_model_merge(game_model, pieces, count);
}

void game_model_from7(GameModel *game_model, int num_vertices, int num_faces,
                      int autocommit, int isolated, int unlit, int unpickable,
                      int projected) {
    game_model_new(game_model);

    game_model->autocommit = autocommit;
    game_model->isolated = isolated;
    game_model->unlit = unlit;
    //game_model->unlit = 0;
    game_model->unpickable = unpickable;
    game_model->projected = projected;

    game_model_allocate(game_model, num_vertices, num_faces);
}

void game_model_from_bytes(GameModel *game_model, int8_t *data, int offset) {
    game_model_new(game_model);

    int num_vertices = get_unsigned_short(data, offset);
    offset += 2;

    int num_faces = get_unsigned_short(data, offset);
    offset += 2;

    game_model_allocate(game_model, num_vertices, num_faces);

    int max = 0;

    for (int i = 0; i < num_vertices; i++) {
        game_model->vertex_x[i] = get_signed_short(data, offset);

        if (abs(game_model->vertex_x[i]) > max) {
            max = abs(game_model->vertex_x[i]);
        }

        offset += 2;
    }

    for (int i = 0; i < num_vertices; i++) {
        game_model->vertex_y[i] = get_signed_short(data, offset);
        offset += 2;
    }

    for (int i = 0; i < num_vertices; i++) {
        game_model->vertex_z[i] = get_signed_short(data, offset);
        offset += 2;
    }

    game_model->num_vertices = num_vertices;

    for (int i = 0; i < num_faces; i++) {
        game_model->face_num_vertices[i] = data[offset++] & 0xff;
    }

    for (int i = 0; i < num_faces; i++) {
        game_model->face_fill_front[i] = get_signed_short(data, offset);
        offset += 2;

        if (game_model->face_fill_front[i] == 32767) {
            game_model->face_fill_front[i] = COLOUR_TRANSPARENT;
        }
    }

    for (int i = 0; i < num_faces; i++) {
        game_model->face_fill_back[i] = get_signed_short(data, offset);
        offset += 2;

        if (game_model->face_fill_back[i] == 32767) {
            game_model->face_fill_back[i] = COLOUR_TRANSPARENT;
        }

        // TODO remove
        game_model->face_fill_front[i] = -32768;
        game_model->face_fill_back[i] = -32768;
    }

    for (int i = 0; i < num_faces; i++) {
        int is_intense = data[offset++] & 0xff;

        // TODO RENAME to is_gouraud!
        game_model->face_intensity[i] =
            is_intense == 0 ? 0 : GAME_MODEL_USE_GOURAUD;
    }

    for (int i = 0; i < num_faces; i++) {
        game_model->face_vertices[i] =
            malloc(game_model->face_num_vertices[i] * sizeof(int));

        for (int j = 0; j < game_model->face_num_vertices[i]; j++) {
            if (num_vertices < 256) {
                game_model->face_vertices[i][j] = data[offset++] & 0xff;
            } else {
                game_model->face_vertices[i][j] =
                    get_unsigned_short(data, offset);

                offset += 2;
            }
        }
    }

    game_model->num_faces = num_faces;
}

void game_model_reset(GameModel *game_model) {
    game_model->base_x = 0;
    game_model->base_y = 0;
    game_model->base_z = 0;
    game_model->orientation_yaw = 0;
    game_model->orientation_pitch = 0;
    game_model->orientation_roll = 0;
    game_model->transform_kind = 0;
}

void game_model_allocate(GameModel *game_model, int num_vertices,
                         int num_faces) {
    /* each terrain, wall and roof location gets a model, even if empty */
    if (num_vertices == 0) {
        return;
    }

    game_model->vertex_x = calloc(num_vertices, sizeof(int));
    game_model->vertex_y = calloc(num_vertices, sizeof(int));
    game_model->vertex_z = calloc(num_vertices, sizeof(int));
    game_model->vertex_intensity = calloc(num_vertices, sizeof(int));
    game_model->vertex_ambience = calloc(num_vertices, sizeof(int8_t));
    game_model->face_num_vertices = calloc(num_faces, sizeof(int));
    game_model->face_vertices = calloc(num_faces, sizeof(int *));
    game_model->face_fill_front = calloc(num_faces, sizeof(int));
    game_model->face_fill_back = calloc(num_faces, sizeof(int));
    game_model->face_intensity = calloc(num_faces, sizeof(int));
    game_model->normal_scale = calloc(num_faces, sizeof(int));
    game_model->normal_magnitude = calloc(num_faces, sizeof(int));

    if (!game_model->projected) {
        game_model->project_vertex_x = calloc(num_vertices, sizeof(int));
        game_model->project_vertex_y = calloc(num_vertices, sizeof(int));
        game_model->project_vertex_z = calloc(num_vertices, sizeof(int));
        game_model->vertex_view_x = calloc(num_vertices, sizeof(int));
        game_model->vertex_view_y = calloc(num_vertices, sizeof(int));
    }

    if (!game_model->unpickable) {
        game_model->is_local_player = calloc(num_faces, sizeof(int8_t));
        game_model->face_tag = calloc(num_faces, sizeof(int));
    }

    if (game_model->autocommit) {
        game_model->vertex_transformed_x = game_model->vertex_x;
        game_model->vertex_transformed_y = game_model->vertex_y;
        game_model->vertex_transformed_z = game_model->vertex_z;
    } else {
        game_model->vertex_transformed_x = calloc(num_vertices, sizeof(int));
        game_model->vertex_transformed_y = calloc(num_vertices, sizeof(int));
        game_model->vertex_transformed_z = calloc(num_vertices, sizeof(int));
    }

    if (!game_model->unlit || !game_model->isolated) {
        game_model->face_normal_x = calloc(num_faces, sizeof(int));
        game_model->face_normal_y = calloc(num_faces, sizeof(int));
        game_model->face_normal_z = calloc(num_faces, sizeof(int));
    }

    game_model->num_faces = 0;
    game_model->num_vertices = 0;
    game_model->max_verts = num_vertices;
    game_model->max_faces = num_faces;

    game_model_reset(game_model);
}

void game_model_projection_prepare(GameModel *game_model) {
    if (game_model->num_vertices == 0) {
        return;
    }

    game_model->project_vertex_x =
        calloc(game_model->num_vertices, sizeof(int));

    game_model->project_vertex_y =
        calloc(game_model->num_vertices, sizeof(int));

    game_model->project_vertex_z =
        calloc(game_model->num_vertices, sizeof(int));

    game_model->vertex_view_x = calloc(game_model->num_vertices, sizeof(int));
    game_model->vertex_view_y = calloc(game_model->num_vertices, sizeof(int));
}

void game_model_clear(GameModel *game_model) {
    game_model->num_faces = 0;
    game_model->num_vertices = 0;
}

void game_model_reduce(GameModel *game_model, int delta_faces,
                       int delta_vertices) {
    if (game_model->num_faces - delta_faces < 0) {
        delta_faces = game_model->num_faces;
    }

    /* TODO could re-use instead of free here */
    for (int i = 1; i <= delta_faces; i++) {
        free(game_model->face_vertices[game_model->num_faces - i]);
    }

    game_model->num_faces -= delta_faces;

    if (game_model->num_vertices - delta_vertices < 0) {
        delta_vertices = game_model->num_vertices;
    }

    game_model->num_vertices -= delta_vertices;
}

void game_model_merge(GameModel *game_model, GameModel **pieces, int count) {
    int num_faces = 0;
    int num_vertices = 0;

    for (int i = 0; i < count; i++) {
        num_faces += pieces[i]->num_faces;
        num_vertices += pieces[i]->num_vertices;
    }

    game_model_allocate(game_model, num_vertices, num_faces);

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

        for (int src_f = 0; src_f < source->num_faces; src_f++) {
            int *dst_vs =
                malloc(source->face_num_vertices[src_f] * sizeof(int));

            int *src_vs = source->face_vertices[src_f];

            for (int v = 0; v < source->face_num_vertices[src_f]; v++) {
                dst_vs[v] = game_model_vertex_at(
                    game_model, source->vertex_x[src_vs[v]],
                    source->vertex_y[src_vs[v]], source->vertex_z[src_vs[v]]);
            }

            int dst_f = game_model_create_face(
                game_model, source->face_num_vertices[src_f], dst_vs,
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
    for (int i = 0; i < game_model->num_vertices; i++) {
        if (game_model->vertex_x[i] == x && game_model->vertex_y[i] == y &&
            game_model->vertex_z[i] == z) {
            return i;
        }
    }

    return game_model_create_vertex(game_model, x, y, z);
}

int game_model_create_vertex(GameModel *game_model, int x, int y, int z) {
    if (game_model->num_vertices >= game_model->max_verts) {
        return -1;
    }

    game_model->vertex_x[game_model->num_vertices] = x;
    game_model->vertex_y[game_model->num_vertices] = y;
    game_model->vertex_z[game_model->num_vertices] = z;

    return game_model->num_vertices++;
}

int game_model_create_face(GameModel *game_model, int number, int *vertices,
                           int fill_front, int fill_back) {
    if (game_model->num_faces >= game_model->max_faces) {
        return -1;
    }

    game_model->face_num_vertices[game_model->num_faces] = number;
    game_model->face_vertices[game_model->num_faces] = vertices;
    game_model->face_fill_front[game_model->num_faces] = fill_front;
    game_model->face_fill_back[game_model->num_faces] = fill_back;
    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;

    // TODO remove
    game_model->face_fill_front[game_model->num_faces] = -32768;
    game_model->face_fill_back[game_model->num_faces] = -32768;

    return game_model->num_faces++;
}

void game_model_split(GameModel *game_model, GameModel **pieces, int piece_dx,
                      int piece_dz, int rows, int count, int piece_max_vertices,
                      int pickable) {
    game_model_commit(game_model);

    int *piece_num_vertices = calloc(count, sizeof(int));
    int *piece_num_faces = calloc(count, sizeof(int));

    for (int i = 0; i < game_model->num_faces; i++) {
        int sum_x = 0;
        int sum_z = 0;
        int face_num_vertices = game_model->face_num_vertices[i];
        int *vertices = game_model->face_vertices[i];

        for (int i = 0; i < face_num_vertices; i++) {
            sum_x += game_model->vertex_x[vertices[i]];
            sum_z += game_model->vertex_z[vertices[i]];
        }

        int piece_index = ((int)(sum_x / (face_num_vertices * piece_dx))) +
                ((int)(sum_z / (face_num_vertices * piece_dz))) * rows;

        piece_num_vertices[piece_index] += face_num_vertices;
        piece_num_faces[piece_index]++;
    }

    for (int i = 0; i < count; i++) {
        if (piece_num_vertices[i] > piece_max_vertices) {
            piece_num_vertices[i] = piece_max_vertices;
        }

        pieces[i] = malloc(sizeof(GameModel));

        game_model_from7(pieces[i], piece_num_vertices[i], piece_num_faces[i], 1, 1, 1, pickable,
                         1);

        pieces[i]->light_diffuse = game_model->light_diffuse;
        pieces[i]->light_ambience = game_model->light_ambience;
    }

    free(piece_num_vertices);
    free(piece_num_faces);

    for (int i = 0; i < game_model->num_faces; i++) {
        int sum_x = 0;
        int sum_z = 0;
        int face_num_vertices = game_model->face_num_vertices[i];
        int *vertices = game_model->face_vertices[i];

        for (int i = 0; i < face_num_vertices; i++) {
            sum_x += game_model->vertex_x[vertices[i]];
            sum_z += game_model->vertex_z[vertices[i]];
        }

        int piece_index = ((int)(sum_x / (face_num_vertices * piece_dx))) +
                ((int)(sum_z / (face_num_vertices * piece_dz))) * rows;

        game_model_copy_lighting(game_model, pieces[piece_index], vertices, face_num_vertices, i);
    }

    for (int p = 0; p < count; p++) {
        game_model_projection_prepare(pieces[p]);
    }
}

void game_model_copy_lighting(GameModel *game_model, GameModel *model,
                              int *src_vertices, int num_vertices,
                              int in_face) {
    int *dest_vertices = malloc(num_vertices * sizeof(int));

    for (int in_v = 0; in_v < num_vertices; in_v++) {
        int out_v = game_model_vertex_at(
            model, game_model->vertex_x[src_vertices[in_v]],
            game_model->vertex_y[src_vertices[in_v]],
            game_model->vertex_z[src_vertices[in_v]]);

        dest_vertices[in_v] = out_v;

        model->vertex_intensity[out_v] =
            game_model->vertex_intensity[src_vertices[in_v]];

        model->vertex_ambience[out_v] =
            game_model->vertex_ambience[src_vertices[in_v]];
    }

    int out_face = game_model_create_face(model, num_vertices, dest_vertices,
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
    //return;
    gouraud = 1;

    if (game_model->unlit) {
        return;
    }

    for (int i = 0; i < game_model->num_faces; i++) {
        game_model->face_intensity[i] = gouraud ? GAME_MODEL_USE_GOURAUD : 0;
    }

    game_model_set_light_from5(game_model, ambience, diffuse, x, y, z);
}

void game_model_set_vertex_ambience(GameModel *game_model, int vertex_index,
                                    int ambience) {
    return;
    game_model->vertex_ambience[vertex_index] = ambience & 0xff;
}

void game_model_orient(GameModel *game_model, int yaw, int pitch, int roll) {
    game_model->orientation_yaw = yaw & 255;
    game_model->orientation_pitch = pitch & 255;
    game_model->orientation_roll = roll & 255;
    game_model_determine_transform_kind(game_model);
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
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;
}

void game_model_translate(GameModel *game_model, int x, int y, int z) {
    game_model_place(game_model, game_model->base_x + x, game_model->base_y + y,
                     game_model->base_z + z);
}

void game_model_determine_transform_kind(GameModel *game_model) {
    if (game_model->orientation_yaw != 0 ||
        game_model->orientation_pitch != 0 ||
        game_model->orientation_roll != 0) {
        game_model->transform_kind = GAME_MODEL_TRANSFORM_ROTATE;
    } else if (game_model->base_x != 0 || game_model->base_y != 0 ||
               game_model->base_z != 0) {
        game_model->transform_kind = GAME_MODEL_TRANSFORM_TRANSLATE;
    } else {
        game_model->transform_kind = 0;
    }
}

void game_model_apply_translate(GameModel *game_model, int dx, int dy, int dz) {
    for (int i = 0; i < game_model->num_vertices; i++) {
        game_model->vertex_transformed_x[i] += dx;
        game_model->vertex_transformed_y[i] += dy;
        game_model->vertex_transformed_z[i] += dz;
    }
}

void game_model_apply_rotation(GameModel *game_model, int yaw, int roll,
                               int pitch) {
    for (int i = 0; i < game_model->num_vertices; i++) {
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
    game_model->x1 = 999999;
    game_model->y1 = 999999;
    game_model->z1 = 999999;

    game_model->diameter = -999999;
    game_model->x2 = -999999;
    game_model->y2 = -999999;
    game_model->z2 = -999999;

    for (int i = 0; i < game_model->num_faces; i++) {
        int *face_vertices = game_model->face_vertices[i];
        int vertex_index = face_vertices[0];
        int face_num_vertices = game_model->face_num_vertices[i];

#if defined(RENDER_GL) && !defined(RENDER_SW)
        vec3 vertex = {game_model->vertex_x[vertex_index] / 1000.0f,
                       game_model->vertex_y[vertex_index] / 1000.0f,
                       game_model->vertex_z[vertex_index] / 1000.0f};

        vec3 transformed_vertex = {0};
        glm_mat4_mulv3(game_model->transform, vertex, 1, transformed_vertex);

        int x1 = transformed_vertex[0] * 1000;
        int x2 = x2;
        int y1 = transformed_vertex[1] * 1000;
        int y2 = y1;
        int z1 = transformed_vertex[2] * 1000;
        int z2 = z1;
#else
        int x1 = game_model->vertex_transformed_x[vertex_index];
        int x2 = x2;
        int y1 = game_model->vertex_transformed_y[vertex_index];
        int y2 = y1;
        int z1 = game_model->vertex_transformed_z[vertex_index];
        int z2 = z1;
#endif

        for (int j = 0; j < face_num_vertices; j++) {
            vertex_index = face_vertices[j];

#if !defined(RENDER_SW) && defined(RENDER_GL)
            vertex[0] = game_model->vertex_x[vertex_index] / 1000.0f;
            vertex[1] = game_model->vertex_y[vertex_index] / 1000.0f;
            vertex[2] = game_model->vertex_z[vertex_index] / 1000.0f;

            glm_mat4_mulv3(game_model->transform, vertex, 1,
                           transformed_vertex);

            int vertex_transformed_x = transformed_vertex[0] * 1000;
            int vertex_transformed_y = transformed_vertex[1] * 1000;
            int vertex_transformed_z = transformed_vertex[2] * 1000;
#else
            int vertex_transformed_x =
                game_model->vertex_transformed_x[vertex_index];

            int vertex_transformed_y =
                game_model->vertex_transformed_y[vertex_index];

            int vertex_transformed_z =
                game_model->vertex_transformed_z[vertex_index];
#endif

            if (vertex_transformed_x < x1) {
                x1 = vertex_transformed_x;
            } else if (vertex_transformed_x > x2) {
                x2 = vertex_transformed_x;
            }

            if (vertex_transformed_y < y1) {
                y1 = vertex_transformed_y;
            } else if (vertex_transformed_y > y2) {
                y2 = vertex_transformed_y;
            }

            if (vertex_transformed_z < z1) {
                z1 = vertex_transformed_z;
            } else if (vertex_transformed_z > z2) {
                z2 = vertex_transformed_z;
            }
        }

        if (x2 - x1 > game_model->diameter) {
            game_model->diameter = x2 - x1;
        }

        if (y2 - y1 > game_model->diameter) {
            game_model->diameter = y2 - y1;
        }

        if (z2 - z1 > game_model->diameter) {
            game_model->diameter = z2 - z1;
        }

        if (x1 < game_model->x1) {
            game_model->x1 = x1;
        }

        if (x2 > game_model->x2) {
            game_model->x2 = x2;
        }

        if (y1 < game_model->y1) {
            game_model->y1 = y1;
        }

        if (y2 > game_model->y2) {
            game_model->y2 = y2;
        }

        if (z1 < game_model->z1) {
            game_model->z1 = z1;
        }

        if (z2 > game_model->z2) {
            game_model->z2 = z2;
        }
    }
}

void game_model_light(GameModel *game_model) {
    //return;

    if (game_model->unlit) {
        return;
    }

    int divisor =
        (game_model->light_diffuse * game_model->light_direction_magnitude) >>
        8; // >> 8 is / 256

    for (int i = 0; i < game_model->num_faces; i++) {
        if (game_model->face_intensity[i] != GAME_MODEL_USE_GOURAUD) {
            /*DOT_PRODUCT(
                game_model->face_normal_x[i],
                game_model->light_direction_x,
                game_model->face_normal_y[i],
                game_model->light_direction_y,
                game_model->face_normal_z[i],
                game_model->light_direction_z
            ) / divisor;*/

            game_model->face_intensity[i] =
                (game_model->face_normal_x[i] *
                           game_model->light_direction_x +
                       game_model->face_normal_y[i] *
                           game_model->light_direction_y +
                       game_model->face_normal_z[i] *
                           game_model->light_direction_z) /
                      divisor;
        }
    }

    int *normal_x = calloc(game_model->num_vertices, sizeof(int));
    int *normal_y = calloc(game_model->num_vertices, sizeof(int));
    int *normal_z = calloc(game_model->num_vertices, sizeof(int));
    int *normal_magnitude = calloc(game_model->num_vertices, sizeof(int));

    for (int i = 0; i < game_model->num_faces; i++) {
        if (game_model->face_intensity[i] == GAME_MODEL_USE_GOURAUD) {
            for (int j = 0; j < game_model->face_num_vertices[i]; j++) {
                int vertex_index = game_model->face_vertices[i][j];

                normal_x[vertex_index] += game_model->face_normal_x[i];
                normal_y[vertex_index] += game_model->face_normal_y[i];
                normal_z[vertex_index] += game_model->face_normal_z[i];

                normal_magnitude[vertex_index]++;
            }
        }
    }

    for (int i = 0; i < game_model->num_vertices; i++) {
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

    for (int i = 0; i < game_model->num_faces; i++) {
        int *face_vertices = game_model->face_vertices[i];

        int a_x = game_model->vertex_transformed_x[face_vertices[0]];
        int a_y = game_model->vertex_transformed_y[face_vertices[0]];
        int a_z = game_model->vertex_transformed_z[face_vertices[0]];
        int b_x = game_model->vertex_transformed_x[face_vertices[1]] - a_x;
        int b_y = game_model->vertex_transformed_y[face_vertices[1]] - a_y;
        int b_z = game_model->vertex_transformed_z[face_vertices[1]] - a_z;
        int c_x = game_model->vertex_transformed_x[face_vertices[2]] - a_x;
        int c_y = game_model->vertex_transformed_y[face_vertices[2]] - a_y;
        int c_z = game_model->vertex_transformed_z[face_vertices[2]] - a_z;

        int norm_x = b_y * c_z - c_y * b_z;
        int norm_y = b_z * c_x - c_z * b_x;
        int norm_z;

        for (norm_z = b_x * c_y - c_x * b_y;
             norm_x > 8192 || norm_y > 8192 || norm_z > 8192 ||
             norm_x < -8192 || norm_y < -8192 || norm_z < -8192;
             norm_z /= 2) {
            norm_x /= 2;
            norm_y /= 2;
        }

        int norm_mag =
            256 * sqrt(norm_x * norm_x + norm_y * norm_y + norm_z * norm_z);

        if (norm_mag <= 0) {
            norm_mag = 1;
        }

        game_model->face_normal_x[i] = (int)((norm_x * 65536) / norm_mag);
        game_model->face_normal_y[i] = (int)((norm_y * 65536) / norm_mag);
        game_model->face_normal_z[i] = (int)((norm_z * 65535) / norm_mag);

        game_model->normal_scale[i] = -1;
    }

    game_model_light(game_model);
}

void game_model_reset_transform(GameModel *game_model) {
    game_model->transform_state = 0;

#ifdef RENDER_SW
    for (int i = 0; i < game_model->num_vertices; i++) {
        game_model->vertex_transformed_x[i] = game_model->vertex_x[i];
        game_model->vertex_transformed_y[i] = game_model->vertex_y[i];
        game_model->vertex_transformed_z[i] = game_model->vertex_z[i];
    }
#endif

#ifdef RENDER_GL
    glm_mat4_identity(game_model->transform);
#endif
}

void game_model_apply(GameModel *game_model) {
    if (game_model->transform_state == GAME_MODEL_TRANSFORM_RESET) {
        game_model_reset_transform(game_model);

        game_model->x1 = -9999999;
        game_model->y1 = -9999999;
        game_model->z1 = -9999999;
        game_model->diameter = 9999999;
        game_model->x2 = 9999999;
        game_model->y2 = 9999999;
        game_model->z2 = 9999999;
    } else if (game_model->transform_state == GAME_MODEL_TRANSFORM_BEGIN) {
        game_model_reset_transform(game_model);

#ifdef RENDER_GL
        if (game_model->transform_kind >= GAME_MODEL_TRANSFORM_TRANSLATE) {
            glm_translate(game_model->transform,
                          (vec3){game_model->base_x / 1000.0f,
                                 game_model->base_y / 1000.0f,
                                 game_model->base_z / 1000.0f});
        }

        if (game_model->transform_kind >= GAME_MODEL_TRANSFORM_ROTATE) {
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
#endif

#if 1
        if (game_model->transform_kind >= GAME_MODEL_TRANSFORM_ROTATE) {
            game_model_apply_rotation(game_model, game_model->orientation_yaw,
                                      game_model->orientation_pitch,
                                      game_model->orientation_roll);
        }

        if (game_model->transform_kind >= GAME_MODEL_TRANSFORM_TRANSLATE) {
            game_model_apply_translate(game_model, game_model->base_x,
                                       game_model->base_y, game_model->base_z);
        }

        game_model_compute_bounds(game_model);
#endif

        game_model_relight(game_model);
    }
}

void game_model_project(GameModel *game_model, int camera_x, int camera_y,
                        int camera_z, int camera_pitch, int camera_roll,
                        int camera_yaw, int view_distance, int clip_near) {
    game_model_apply(game_model);

    if (game_model->z1 > scene_frustum_near_z ||
        game_model->z2 < scene_frustum_far_z ||
        game_model->x1 > scene_frustum_min_x ||
        game_model->x2 < scene_frustum_max_x ||
        game_model->y1 > scene_frustum_min_y ||
        game_model->y2 < scene_frustum_max_y) {
        game_model->visible = 0;
        return;
    }

    game_model->visible = 1;

#ifdef RENDER_SW
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

    for (int i = 0; i < game_model->num_vertices; i++) {
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
            game_model->vertex_view_x[i] = (int)((x << view_distance) / z);
            game_model->vertex_view_y[i] = (int)((y << view_distance) / z);
        } else {
            game_model->vertex_view_x[i] = x << view_distance;
            game_model->vertex_view_y[i] = y << view_distance;
        }

        game_model->project_vertex_x[i] = x;
        game_model->project_vertex_y[i] = y;
        game_model->project_vertex_z[i] = z;
    }
#endif
}

void game_model_commit(GameModel *game_model) {
    game_model_apply(game_model);

    for (int i = 0; i < game_model->num_vertices; i++) {
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
    copy->vao = game_model->vao;
    copy->ebo_offset = game_model->ebo_offset;
    copy->ebo_length = game_model->ebo_length;
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
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = GAME_MODEL_TRANSFORM_BEGIN;

#ifdef RENDER_GL
    glm_mat4_copy(source->transform, game_model->transform);
#endif
}

void game_model_destroy(GameModel *game_model) {
    if (game_model == NULL) {
        return;
    }

    game_model->num_vertices = 0;

    for (int i = 0; i < game_model->num_faces; i++) {
        free(game_model->face_vertices[i]);
        game_model->face_vertices[i] = NULL;
    }

    game_model->num_faces = 0;

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

    free(game_model->face_num_vertices);
    game_model->face_num_vertices = NULL;

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

void game_model_dump(GameModel *game_model, int i) {
    /*if (game_model->num_vertices < 1000) {
        return;
    }*/

    char name[255];

    sprintf(name, "./world-%d.obj", i);

    FILE *obj_file = fopen(name, "w");

    for (int i = 0; i < game_model->num_vertices; i++) {
        float vertex_x = (((float)game_model->vertex_x[i]) / 100);
        float vertex_y = ((float)game_model->vertex_y[i]) / 100;
        float vertex_z = (((float)game_model->vertex_z[i]) / 100);

        fprintf(obj_file, "v %f %f %f\n", vertex_x, -vertex_y, vertex_z);
    }

    for (int i = 0; i < game_model->num_faces; i++) {
        fprintf(obj_file, "f ");

        for (int j = 0; j < game_model->face_num_vertices[i]; j++) {
            fprintf(obj_file, "%d ", game_model->face_vertices[i][j] + 1);
        }

        fprintf(obj_file, "\n");
    }

    fclose(obj_file);
}

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

    glBufferData(GL_ARRAY_BUFFER, vbo_length * sizeof(GLfloat) * 15, NULL,
                 GL_STATIC_DRAW);

    /* vertex { x, y, z } */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat),
                          (void *)0);

    glEnableVertexAttribArray(0);

    /* normal { x, y, z } */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(1);

    /* lighting { intensity, magnitude } */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat),
                          (void *)(6 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);

    /* colour { r, g, b, a } */
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat),
                          (void *)(8 * sizeof(GLfloat)));

    glEnableVertexAttribArray(3);

    /* texture { s, t, index } */
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat),
                          (void *)(12 * sizeof(GLfloat)));

    glEnableVertexAttribArray(4);

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_length * sizeof(GLuint), NULL,
                 GL_STATIC_DRAW);
}

void game_model_gl_unwrap_uvs(GameModel *game_model, int *face_vertices,
                              int face_num_vertices, GLfloat *us, GLfloat *vs) {
    vec3 vertices[face_num_vertices];

    for (int i = 0; i < face_num_vertices; i++) {
        int vertex_index = face_vertices[i];

        vertices[i][0] = game_model->vertex_x[vertex_index] / 1000.0f;
        vertices[i][1] = game_model->vertex_y[vertex_index] / 1000.0f;
        vertices[i][2] = game_model->vertex_z[vertex_index] / 1000.0f;
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

    GLfloat max_x = 0;
    GLfloat min_x = 0;

    GLfloat max_y = 0;
    GLfloat min_y = 0;

    for (int i = 0; i < face_num_vertices; i++) {
        vec3 vertex = {0};

        /*vertex[0] = vertices[i][0];
        vertex[1] = vertices[i][1];
        vertex[2] = vertices[i][2];*/

        glm_vec3_sub(vertices[i], vertices[0], vertex);

        GLfloat x = glm_vec3_dot(vertex, location_x);
        GLfloat y = glm_vec3_dot(vertex, location_y);

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

    for (int i = 0; i < face_num_vertices; i++) {
        GLfloat x = us[i];
        GLfloat y = vs[i];

        us[i] = (x - min_x) / (max_x - min_x);
        vs[i] = 1.0f - (y - min_y) / (max_y - min_y);
    }
}

void game_model_gl_decode_face_fill(int face_fill, float *r, float *g, float *b) {
    face_fill = -1 - face_fill;
    *r = (((face_fill >> 10) & 31) * 8) / 255.0f;
    *g = (((face_fill >> 5) & 31) * 8) / 255.0f;
    *b = ((face_fill & 31) * 8) / 255.0f;
}

int game_model_gl_get_face_area(int *face_vertices, int face_num_vertices, int *vertices_x, int *vertices_y) {
    float left_sum = 0;
    float right_sum = 0;

    for (int i = 0; i < face_num_vertices; i++) {
        int vertex_index = face_vertices[i];
        int next_vertex_index = face_vertices[(i + 1) % face_num_vertices];

        left_sum += vertices_x[vertex_index] * vertices_y[next_vertex_index];
        right_sum += vertices_x[next_vertex_index] * vertices_y[vertex_index];
    }

    return (left_sum - right_sum) / 2;
}

void game_model_gl_buffer_arrays(GameModel *game_model, int *vertex_offset,
                                 int *ebo_offset) {
    int *face_normal_x = calloc(game_model->num_faces, sizeof(int));
    int *face_normal_y = calloc(game_model->num_faces, sizeof(int));
    int *face_normal_z = calloc(game_model->num_faces, sizeof(int));

    for (int i = 0; i < game_model->num_faces; i++) {
        int *face_vertices = game_model->face_vertices[i];

        int a_x = game_model->vertex_x[face_vertices[0]];
        int a_y = game_model->vertex_y[face_vertices[0]];
        int a_z = game_model->vertex_z[face_vertices[0]];
        int b_x = game_model->vertex_x[face_vertices[1]] - a_x;
        int b_y = game_model->vertex_y[face_vertices[1]] - a_y;
        int b_z = game_model->vertex_z[face_vertices[1]] - a_z;
        int c_x = game_model->vertex_x[face_vertices[2]] - a_x;
        int c_y = game_model->vertex_y[face_vertices[2]] - a_y;
        int c_z = game_model->vertex_z[face_vertices[2]] - a_z;

        int norm_x = b_y * c_z - c_y * b_z;
        int norm_y = b_z * c_x - c_z * b_x;
        int norm_z;

        for (norm_z = b_x * c_y - c_x * b_y;
             norm_x > 8192 || norm_y > 8192 || norm_z > 8192 ||
             norm_x < -8192 || norm_y < -8192 || norm_z < -8192;
             norm_z /= 2) {
            norm_x /= 2;
            norm_y /= 2;
        }

        int norm_mag =
            256 * sqrt(norm_x * norm_x + norm_y * norm_y + norm_z * norm_z);

        if (norm_mag <= 0) {
            norm_mag = 1;
        }

        face_normal_x[i] = (int)((norm_x * 65536) / norm_mag);
        face_normal_y[i] = (int)((norm_y * 65536) / norm_mag);
        face_normal_z[i] = (int)((norm_z * 65535) / norm_mag);
    }

    int *normal_x = calloc(game_model->num_vertices, sizeof(int));
    int *normal_y = calloc(game_model->num_vertices, sizeof(int));
    int *normal_z = calloc(game_model->num_vertices, sizeof(int));
    int *normal_magnitude = calloc(game_model->num_vertices, sizeof(int));

    for (int i = 0; i < game_model->num_faces; i++) {
        for (int j = 0; j < game_model->face_num_vertices[i]; j++) {
            int vertex_index = game_model->face_vertices[i][j];

            normal_x[vertex_index] += face_normal_x[i];
            normal_y[vertex_index] += face_normal_y[i];
            normal_z[vertex_index] += face_normal_z[i];

            normal_magnitude[vertex_index]++;
        }
    }

    for (int i = 0; i < game_model->num_faces; i++) {
        int *face_vertices = game_model->face_vertices[i];
        int face_num_vertices = game_model->face_num_vertices[i];

        vec3 vertices[3];

        for (int i = 0; i < 3; i++) {
            int vertex_index = face_vertices[i];

            vertices[i][0] = game_model->vertex_x[vertex_index] / 1000.0f;
            vertices[i][1] = game_model->vertex_y[vertex_index] / 1000.0f;
            vertices[i][2] = game_model->vertex_z[vertex_index] / 1000.0f;
        }

        vec3 location_x = {0};
        glm_vec3_sub(vertices[1], vertices[0], location_x);

        vec3 diff = {0};
        glm_vec3_sub(vertices[2], vertices[0], diff);

        vec3 test_normal = {0};
        glm_vec3_cross(location_x, diff, test_normal);

        glm_vec3_normalize(test_normal);

        vec3 a_c_sub = {0};
        glm_vec3_sub(vertices[0], vertices[2], a_c_sub);

        vec3 b_c_sub = {0};
        glm_vec3_sub(vertices[1], vertices[2], b_c_sub);

        vec3 a_c_b_c_cross = {0};
        glm_vec3_cross(a_c_sub, b_c_sub, a_c_b_c_cross);

        float dot_product = glm_vec3_dot(
            test_normal,
            a_c_b_c_cross
        );

        /*int x_y_area = game_model_gl_get_face_area(face_vertices, face_num_vertices, game_model->vertex_x, game_model->vertex_y);

        int x_z_area = game_model_gl_get_face_area(face_vertices, face_num_vertices, game_model->vertex_x, game_model->vertex_z);

        int y_z_area = game_model_gl_get_face_area(face_vertices, face_num_vertices, game_model->vertex_y, game_model->vertex_z);

        printf("%d, %d, %d\n", x_y_area, x_z_area, y_z_area);*/

        //printf("%f\n", dot_product);

        /*int a_x = game_model->vertex_x[face_vertices[0]];
        int a_y = game_model->vertex_y[face_vertices[0]];
        int a_z = game_model->vertex_z[face_vertices[0]];
        int b_x = game_model->vertex_x[face_vertices[1]] - a_x;
        int b_y = game_model->vertex_y[face_vertices[1]] - a_y;
        int b_z = game_model->vertex_z[face_vertices[1]] - a_z;
        int c_x = game_model->vertex_x[face_vertices[2]] - a_x;
        int c_y = game_model->vertex_y[face_vertices[2]] - a_y;
        int c_z = game_model->vertex_z[face_vertices[2]] - a_z;

        int norm_x = b_y * c_z - c_y * b_z;
        int norm_y = b_z * c_x - c_z * b_x;
        int norm_z;

        for (norm_z = b_x * c_y - c_x * b_y;
             norm_x > 8192 || norm_y > 8192 || norm_z > 8192 ||
             norm_x < -8192 || norm_y < -8192 || norm_z < -8192;
             norm_z >>= 1) {
            norm_x >>= 1;
            norm_y >>= 1;
        }

        int norm_mag =
            256 * sqrt(norm_x * norm_x + norm_y * norm_y + norm_z * norm_z);

        if (norm_mag <= 0) {
            norm_mag = 1;
        }

        int normal_x = (int)((norm_x * 65536) / norm_mag);
        int normal_y = (int)((norm_y * 65536) / norm_mag);
        int normal_z = (int)((norm_z * 65535) / norm_mag);

        vec3 normal = {
            normal_x / 1000.0f,
            normal_y / 1000.0f,
            normal_z / 1000.0f,
        };*/

        vec3 normal = {
            face_normal_x[i] / 1000.0f,
            face_normal_y[i] / 1000.0f,
            face_normal_z[i] / 1000.0f,
        };

        int fill_front = game_model->face_fill_front[i];
        int fill_back = game_model->face_fill_back[i];

        int face_intensity = game_model->face_intensity[i];

        if (face_intensity != GAME_MODEL_USE_GOURAUD) {
            if (face_intensity > (RAMP_SIZE - 1)) {
                face_intensity = RAMP_SIZE - 1;
            }
        } else {
            face_intensity = -256;
        }

        GLfloat r = 1.0f;
        GLfloat g = 1.0f;
        GLfloat b = 1.0f;
        GLfloat a = 1.0f;

        GLfloat texture_index = -1.0f;

        if (fill_front != COLOUR_TRANSPARENT) {
            if (fill_front < 0) {
                game_model_gl_decode_face_fill(fill_front, &r, &g, &b);
            } else if (fill_front >= 0) {
                texture_index = (float)fill_front;
            }
        } else if (fill_back != COLOUR_TRANSPARENT) {
            if (fill_back < 0) {
                game_model_gl_decode_face_fill(fill_back, &r, &g, &b);
            } else if (fill_back >= 0) {
                texture_index = (float)fill_back;
            }
        } else {
            a = 0;
        }

        GLfloat *face_us = NULL;
        GLfloat *face_vs = NULL;

        if (texture_index > -1.0f) {
            if (face_num_vertices == 3) {
                face_us = gl_tri_face_us;
                face_vs = gl_tri_face_vs;
            } else if (face_num_vertices == 4) {
                face_us = gl_quad_face_us;
                face_vs = gl_quad_face_vs;
            } else {
                face_us = alloca(face_num_vertices * sizeof(GLfloat));
                face_vs = alloca(face_num_vertices * sizeof(GLfloat));

                game_model_gl_unwrap_uvs(game_model, face_vertices,
                                         face_num_vertices, face_us, face_vs);
            }
        }

        for (int j = 0; j < face_num_vertices; j++) {
            int vertex_index = face_vertices[j];

            GLfloat vertex_x = game_model->vertex_x[vertex_index] / 1000.0f;
            GLfloat vertex_y = game_model->vertex_y[vertex_index] / 1000.0f;
            GLfloat vertex_z = game_model->vertex_z[vertex_index] / 1000.0f;

            normal[0] = normal_x[vertex_index] / 1000.0f;
            normal[1] = normal_y[vertex_index] / 1000.0f;
            normal[2] = normal_z[vertex_index] / 1000.0f;

            //glm_vec3_normalize(normal);

            face_intensity = -256;

            GLfloat texture_x = -1.0f;
            GLfloat texture_y = -1.0f;

            if (face_us != NULL && face_vs != NULL) {
                texture_x = face_us[j];
                texture_y = 1.0f - face_vs[j];
            }

            GLfloat vertex[] = {
                /* vertex */
                vertex_x, vertex_y, vertex_z, //

                /* normal */
                normal[0], normal[1], normal[2], //

                /* lighting */
                (float)(face_intensity), (float)(normal_magnitude[vertex_index]), //

                /* colour */
                r, g, b, a, //

                /* texture */
                texture_x, texture_y, texture_index //
            };

            glBufferSubData(GL_ARRAY_BUFFER,
                            ((*vertex_offset) + j) * 15 * sizeof(GLfloat),
                            15 * sizeof(GLfloat), vertex);
        }

        // TODO preserve winding order for GL_CULL_FACE
        for (int j = 0; j < face_num_vertices - 2; j++) {
            GLuint indices[] = {(*vertex_offset), (*vertex_offset) + j + 1,
                                (*vertex_offset) + j + 2};

            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                            (*ebo_offset) * sizeof(GLuint), sizeof(indices),
                            indices);

            (*ebo_offset) += 3;
        }

        (*vertex_offset) += face_num_vertices;
    }

    /*free(normal_x);
    free(normal_y);
    free(normal_z);
    free(normal_magnitude);*/
}

void game_model_gl_buffer_models(GLuint *vao, GLuint *vbo, GLuint *ebo,
                                 GameModel **game_models, int length) {
    int vertex_offset = 0;
    int ebo_offset = 0;

    for (int i = 0; i < length; i++) {
        GameModel *game_model = game_models[i];

        game_model->ebo_offset = ebo_offset;

        for (int j = 0; j < game_model->num_faces; j++) {
            int face_num_vertices = game_model->face_num_vertices[j];
            vertex_offset += face_num_vertices;
            game_model->ebo_length += (face_num_vertices - 2) * 3;
        }

        ebo_offset += game_model->ebo_length;
    }

    game_model_gl_create_vao(vao, vbo, ebo, vertex_offset, ebo_offset);

    vertex_offset = 0;
    ebo_offset = 0;

    for (int i = 0; i < length; i++) {
        GameModel *game_model = game_models[i];

        game_model->vao = *vao;

        game_model_gl_buffer_arrays(game_model, &vertex_offset, &ebo_offset);
    }
}
#endif
