#include "game-model.h"

#ifdef RENDER_GL
float gl_tri_face_us[] = {0.0f, 1.0f, 0.0f};
float gl_tri_face_vs[] = {1.0f, 1.0f, 0.0f};
#endif

void game_model_new(GameModel *game_model) {
    memset(game_model, 0, sizeof(GameModel));

    game_model->transform_state = 1;
    game_model->visible = 1;
    game_model->key = -1;
    game_model->light_diffuse = 512;
    game_model->light_ambience = 32;
    game_model->diameter = COLOUR_TRANSPARENT;
    game_model->light_direction_x = 180;
    game_model->light_direction_y = 155;
    game_model->light_direction_z = 95;
    game_model->light_direction_magnitude = 256;

#ifdef RENDER_GL
    game_model->ebo_offset = -1;
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
    }

    for (int i = 0; i < num_faces; i++) {
        int is_intense = data[offset++] & 0xff;
        game_model->face_intensity[i] =
            is_intense == 0 ? 0 : COLOUR_TRANSPARENT;
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
    /* TODO why is this happening? */
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
    /* TODO why is this happening? */
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

    game_model->transform_state = 1;
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
    game_model->transform_state = 1;

    return game_model->num_faces++;
}

void game_model_split(GameModel *game_model, GameModel **pieces, int piece_dx,
                      int piece_dz, int rows, int count, int piece_max_vertices,
                      int pickable) {
    game_model_commit(game_model);

    int *piece_nv = calloc(count, sizeof(int));
    int *piece_nf = calloc(count, sizeof(int));

    for (int f = 0; f < game_model->num_faces; f++) {
        int sum_x = 0;
        int sum_z = 0;
        int n = game_model->face_num_vertices[f];
        int *vertices = game_model->face_vertices[f];

        for (int i = 0; i < n; i++) {
            sum_x += game_model->vertex_x[vertices[i]];
            sum_z += game_model->vertex_z[vertices[i]];
        }

        int p = ((int)(sum_x / (n * piece_dx))) +
                ((int)(sum_z / (n * piece_dz))) * rows;

        piece_nv[p] += n;
        piece_nf[p]++;
    }

    for (int i = 0; i < count; i++) {
        if (piece_nv[i] > piece_max_vertices) {
            piece_nv[i] = piece_max_vertices;
        }

        pieces[i] = malloc(sizeof(GameModel));

        game_model_from7(pieces[i], piece_nv[i], piece_nf[i], 1, 1, 1, pickable,
                         1);

        pieces[i]->light_diffuse = game_model->light_diffuse;
        pieces[i]->light_ambience = game_model->light_ambience;
    }

    free(piece_nv);
    free(piece_nf);

    for (int f = 0; f < game_model->num_faces; f++) {
        int sum_x = 0;
        int sum_z = 0;
        int n = game_model->face_num_vertices[f];
        int *vertices = game_model->face_vertices[f];

        for (int i = 0; i < n; i++) {
            sum_x += game_model->vertex_x[vertices[i]];
            sum_z += game_model->vertex_z[vertices[i]];
        }

        int p = ((int)(sum_x / (n * piece_dx))) +
                ((int)(sum_z / (n * piece_dz))) * rows;

        game_model_copy_lighting(game_model, pieces[p], vertices, n, f);
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

    for (int i = 0; i < game_model->num_faces; i++) {
        game_model->face_intensity[i] = gouraud ? COLOUR_TRANSPARENT : 0;
    }

    game_model_set_light_from5(game_model, ambience, diffuse, x, y, z);
}

void game_model_set_vertex_ambience(GameModel *game_model, int vertex_index,
                                    int ambience) {
    game_model->vertex_ambience[vertex_index] = ambience & 0xff;
}

void game_model_orient(GameModel *game_model, int yaw, int pitch, int roll) {
    game_model->orientation_yaw = yaw & 0xff;
    game_model->orientation_pitch = pitch & 0xff;
    game_model->orientation_roll = roll & 0xff;

    game_model_determine_transform_kind(game_model);

    game_model->transform_state = 1;
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
    game_model->transform_state = 1;
}

void game_model_translate(GameModel *game_model, int x, int y, int z) {
    game_model_place(game_model, game_model->base_x + x, game_model->base_y + y,
                     game_model->base_z + z);
}

void game_model_determine_transform_kind(GameModel *game_model) {
    if (game_model->orientation_yaw != 0 ||
               game_model->orientation_pitch != 0 ||
               game_model->orientation_roll != 0) {
        game_model->transform_kind = 2;
    } else if (game_model->base_x != 0 || game_model->base_y != 0 ||
               game_model->base_z != 0) {
        game_model->transform_kind = 1;
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

void game_model_apply_scale(GameModel *game_model, int fx, int fy, int fz) {
    for (int i = 0; i < game_model->num_vertices; i++) {
        game_model->vertex_transformed_x[i] =
            (game_model->vertex_transformed_x[i] * fx) >> 8;

        game_model->vertex_transformed_y[i] =
            (game_model->vertex_transformed_y[i] * fy) >> 8;

        game_model->vertex_transformed_z[i] =
            (game_model->vertex_transformed_z[i] * fz) >> 8;
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

    for (int face = 0; face < game_model->num_faces; face++) {
        int *vs = game_model->face_vertices[face];
        int v = vs[0];
        int n = game_model->face_num_vertices[face];
        int x1 = 0;
        int x2 = (x1 = game_model->vertex_transformed_x[v]);
        int y1 = 0;
        int y2 = (y1 = game_model->vertex_transformed_y[v]);
        int z1 = 0;
        int z2 = (z1 = game_model->vertex_transformed_z[v]);

        for (int i = 0; i < n; i++) {
            v = vs[i];

            if (game_model->vertex_transformed_x[v] < x1) {
                x1 = game_model->vertex_transformed_x[v];
            } else if (game_model->vertex_transformed_x[v] > x2) {
                x2 = game_model->vertex_transformed_x[v];
            }

            if (game_model->vertex_transformed_y[v] < y1) {
                y1 = game_model->vertex_transformed_y[v];
            } else if (game_model->vertex_transformed_y[v] > y2) {
                y2 = game_model->vertex_transformed_y[v];
            }

            if (game_model->vertex_transformed_z[v] < z1) {
                z1 = game_model->vertex_transformed_z[v];
            } else if (game_model->vertex_transformed_z[v] > z2) {
                z2 = game_model->vertex_transformed_z[v];
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
    if (game_model->unlit) {
        return;
    }

    int divisor =
        (game_model->light_diffuse * game_model->light_direction_magnitude) >>
        8;

    for (int i = 0; i < game_model->num_faces; i++) {
        if (game_model->face_intensity[i] != COLOUR_TRANSPARENT) {
            game_model->face_intensity[i] =
                (int)((game_model->face_normal_x[i] *
                           game_model->light_direction_x +
                       game_model->face_normal_y[i] *
                           game_model->light_direction_y +
                       game_model->face_normal_z[i] *
                           game_model->light_direction_z) /
                      divisor);
        }
    }

    int *normal_x = malloc(game_model->num_vertices * sizeof(int));
    int *normal_y = malloc(game_model->num_vertices * sizeof(int));
    int *normal_z = malloc(game_model->num_vertices * sizeof(int));
    int *normal_magnitude = malloc(game_model->num_vertices * sizeof(int));

    for (int i = 0; i < game_model->num_vertices; i++) {
        normal_x[i] = 0;
        normal_y[i] = 0;
        normal_z[i] = 0;
        normal_magnitude[i] = 0;
    }

    for (int i = 0; i < game_model->num_faces; i++) {
        if (game_model->face_intensity[i] == COLOUR_TRANSPARENT) {
            for (int v = 0; v < game_model->face_num_vertices[i]; v++) {
                int k1 = game_model->face_vertices[i][v];

                normal_x[k1] += game_model->face_normal_x[i];
                normal_y[k1] += game_model->face_normal_y[i];
                normal_z[k1] += game_model->face_normal_z[i];

                normal_magnitude[k1]++;
            }
        }
    }

    for (int i = 0; i < game_model->num_vertices; i++) {
        if (normal_magnitude[i] > 0) {
            game_model->vertex_intensity[i] =
                (int)((normal_x[i] * game_model->light_direction_x +
                       normal_y[i] * game_model->light_direction_y +
                       normal_z[i] * game_model->light_direction_z) /
                      (divisor * normal_magnitude[i]));
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
        int *verts = game_model->face_vertices[i];

        int a_x = game_model->vertex_transformed_x[verts[0]];
        int a_y = game_model->vertex_transformed_y[verts[0]];
        int a_z = game_model->vertex_transformed_z[verts[0]];
        int b_x = game_model->vertex_transformed_x[verts[1]] - a_x;
        int b_y = game_model->vertex_transformed_y[verts[1]] - a_y;
        int b_z = game_model->vertex_transformed_z[verts[1]] - a_z;
        int c_x = game_model->vertex_transformed_x[verts[2]] - a_x;
        int c_y = game_model->vertex_transformed_y[verts[2]] - a_y;
        int c_z = game_model->vertex_transformed_z[verts[2]] - a_z;

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

        game_model->face_normal_x[i] = (int)((norm_x * 0x10000) / norm_mag);
        game_model->face_normal_y[i] = (int)((norm_y * 0x10000) / norm_mag);
        game_model->face_normal_z[i] = (int)((norm_z * 0xffff) / norm_mag);
        game_model->normal_scale[i] = -1;
    }

    game_model_light(game_model);
}

void game_model_apply(GameModel *game_model) {
    if (game_model == test_model) {
        //printf("hello\n");
    }

    if (game_model->transform_state == 2) {
        game_model->transform_state = 0;

        for (int i = 0; i < game_model->num_vertices; i++) {
            game_model->vertex_transformed_x[i] = game_model->vertex_x[i];
            game_model->vertex_transformed_y[i] = game_model->vertex_y[i];
            game_model->vertex_transformed_z[i] = game_model->vertex_z[i];
        }

        game_model->x1 = -9999999;
        game_model->y1 = -9999999;
        game_model->z1 = -9999999;
        game_model->diameter = 9999999;
        game_model->x2 = 9999999;
        game_model->y2 = 9999999;
        game_model->z2 = 9999999;
    } else if (game_model->transform_state == 1) {
        game_model->transform_state = 0;

        for (int i = 0; i < game_model->num_vertices; i++) {
            game_model->vertex_transformed_x[i] = game_model->vertex_x[i];
            game_model->vertex_transformed_y[i] = game_model->vertex_y[i];
            game_model->vertex_transformed_z[i] = game_model->vertex_z[i];
        }

        if (game_model->transform_kind >= 2) {
            game_model_apply_rotation(game_model, game_model->orientation_yaw,
                                      game_model->orientation_pitch,
                                      game_model->orientation_roll);
        }

        if (game_model->transform_kind >= 1) {
            game_model_apply_translate(game_model, game_model->base_x,
                                       game_model->base_y, game_model->base_z);
        }

        game_model_compute_bounds(game_model);
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

    int yaw_sin = 0;
    int yaw_cos = 0;
    int pitch_sin = 0;
    int pitch_cos = 0;
    int roll_sin = 0;
    int roll_cos = 0;

    // there is usually no yaw
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

void game_model_copy_position(GameModel *game_model, GameModel *model) {
    game_model->orientation_yaw = model->orientation_yaw;
    game_model->orientation_pitch = model->orientation_pitch;
    game_model->orientation_roll = model->orientation_roll;
    game_model->base_x = model->base_x;
    game_model->base_y = model->base_y;
    game_model->base_z = model->base_z;
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = 1;
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
        glm_vec3_sub(vertices[i], vertices[0], vertex);

        GLfloat x = glm_vec3_dot(vertex, location_x);
        GLfloat y = glm_vec3_dot(vertex, location_y);

        if (i == 0 || x > max_x) {
            max_x = x;
        }

        if (i == 0 || x < min_x) {
            min_x = x;
        }

        if (i == 0 || x > max_y) {
            max_y = y;
        }

        if (i == 0 || x < min_y) {
            min_y = y;
        }

        us[i] = x;
        vs[i] = y;
    }

    for (int i = 0; i < face_num_vertices; i++) {
        GLfloat x = us[i];
        GLfloat y = vs[i];

        us[i] = (x - min_x) / (max_x - min_x);
        vs[i] = 1.0f - ((y - min_y) / (max_y - min_y));
    }
}

void game_model_gl_buffer_arrays(GameModel *game_model, int *vertex_offset,
                                 int *ebo_offset) {
    for (int i = 0; i < game_model->num_faces; i++) {
        int fill_front = game_model->face_fill_front[i];
        int fill_back = game_model->face_fill_back[i];

        GLfloat r = -1.0f;
        GLfloat g = -1.0f;
        GLfloat b = -1.0f;
        GLfloat a = 1.0f;

        GLfloat texture_index = -1.0f;

        if (fill_front != COLOUR_TRANSPARENT) {
            if (fill_front < 0) {
                fill_front = -(fill_front + 1);
                r = ((fill_front >> 10) & 31) / 31.0f;
                g = ((fill_front >> 5) & 31) / 31.0f;
                b = (fill_front & 31) / 31.0f;
            } else if (fill_front >= 0) {
                texture_index = (float)fill_front;
            }
        } else if (fill_back != COLOUR_TRANSPARENT) {
            if (fill_back < 0) {
                fill_back = -(fill_back + 1);
                r = ((fill_back >> 10) & 31) / 31.0f;
                g = ((fill_back >> 5) & 31) / 31.0f;
                b = (fill_back & 31) / 31.0f;
            } else if (fill_back >= 0) {
                texture_index = (float)fill_back;
            }
        }

        int face_num_vertices = game_model->face_num_vertices[i];
        int *face_vertices = game_model->face_vertices[i];

        GLfloat *face_us = NULL;
        GLfloat *face_vs = NULL;

        if (texture_index > -1.0f) {
            if (face_num_vertices == 3) {
                face_us = gl_tri_face_us;
                face_vs = gl_tri_face_vs;
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

            GLfloat texture_x = -1.0f;
            GLfloat texture_y = -1.0f;

            if (face_us != NULL && face_vs != NULL) {
                texture_x = face_us[j];
                texture_y = 1.0f - face_vs[j];
            }

            GLfloat vertices[] = {
                /* vertex */
                vertex_x, vertex_y, vertex_z, //

                /* colour */
                r, g, b, a, //

                /* texture */
                texture_x, texture_y, texture_index //
            };

            // TODO we'll need two texture indices - and an animation position

            glBufferSubData(GL_ARRAY_BUFFER,
                            ((*vertex_offset) + j) * 10 * sizeof(GLfloat),
                            10 * sizeof(GLfloat), vertices);
        }

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
}
#endif
