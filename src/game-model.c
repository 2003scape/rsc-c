#include "game-model.h"

void init_game_model_global() {
    for (int i = 0; i < 256; i++) {
        sine9[i] = (int)(sin((double)i * 0.02454369) * 32768);
        sine9[i + 256] = (int)(cos((double)i * 0.02454369) * 32768);
    }

    for (int i = 0; i < 1024; i++) {
        sine11[i] = (int)(sin((double)i * 0.00613592315) * 32768);
        sine11[i + 1024] = (int)(cos((double)i * 0.00613592315) * 32768);
    }
}

void game_model_new(GameModel *game_model) {
    game_model->transform_state = 1;
    game_model->visible = 1;
    game_model->key = -1;
    game_model->light_diffuse = 512;
    game_model->light_ambience = 32;
    game_model->magic = COLOUR_TRANSPARENT;
    game_model->diameter = COLOUR_TRANSPARENT;
    game_model->light_direction_x = 180;
    game_model->light_direction_y = 155;
    game_model->light_direction_z = 95;
    game_model->light_direction_magnitude = 256;
}

void game_model_from2(GameModel *game_model, int num_vertices, int num_faces) {
    game_model_new(game_model);
    game_model_allocate(game_model, num_vertices, num_faces);
    game_model->face_trans_state_thing = malloc(num_faces * sizeof(int *));

    for (int i = 0; i < num_faces; i++) {
        game_model->face_trans_state_thing[i] = malloc(sizeof(int));
    }
}

void game_model_from2a(GameModel *game_model, GameModel **pieces, int count) {
    game_model_new(game_model);
    game_model_merge(game_model, pieces, count, 1);
}

void game_model_from6(GameModel *game_model, GameModel **pieces, int count,
                      int autocommit, int isolated, int unlit, int unpickable) {
    game_model_new(game_model);

    game_model->autocommit = autocommit;
    game_model->isolated = isolated;
    game_model->unlit = unlit;
    game_model->unpickable = unpickable;

    game_model_merge(game_model, pieces, count, 1);
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
    game_model->face_trans_state_thing = malloc(num_faces * sizeof(int *));

    for (int i = 0; i < num_faces; i++) {
        game_model->face_trans_state_thing[i] = malloc(sizeof(int));
    }

    for (int i = 0; i < num_vertices; i++) {
        game_model->vertex_x[i] = get_signed_short(data, offset);
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
            game_model->face_fill_front[i] = game_model->magic;
        }
    }

    for (int i = 0; i < num_faces; i++) {
        game_model->face_fill_back[i] = get_signed_short(data, offset);
        offset += 2;

        if (game_model->face_fill_back[i] == 32767) {
            game_model->face_fill_back[i] = game_model->magic;
        }
    }

    for (int i = 0; i < num_faces; i++) {
        int is_intense = data[offset++] & 0xff;
        game_model->face_intensity[i] = is_intense == 0 ? 0 : game_model->magic;
    }

    for (int i = 0; i < num_faces; i++) {
        game_model->face_vertices[i] =
            malloc(game_model->face_num_vertices[i] * sizeof(int));

        for (int j = 0; j < game_model->face_num_vertices[i]; j++) {
            if (j < 256) {
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
    game_model->scale_fx = 256;
    game_model->scale_fy = 256;
    game_model->scale_fz = 256;
    game_model->shear_xy = 256;
    game_model->shear_xz = 256;
    game_model->shear_yx = 256;
    game_model->shear_yz = 256;
    game_model->shear_zx = 256;
    game_model->shear_zy = 256;
    game_model->transform_kind = 0;
}

void game_model_allocate(GameModel *game_model, int num_vertices,
                         int num_faces) {
    game_model->vertex_x = malloc(num_vertices * sizeof(int));
    game_model->vertex_y = malloc(num_vertices * sizeof(int));
    game_model->vertex_z = malloc(num_vertices * sizeof(int));
    game_model->vertex_intensity = malloc(num_vertices * sizeof(int));
    game_model->vertex_ambience = malloc(num_vertices);
    game_model->face_num_vertices = malloc(num_faces * sizeof(int));
    game_model->face_vertices = malloc(num_faces * sizeof(int *));
    game_model->face_fill_front = malloc(num_faces * sizeof(int));
    game_model->face_fill_back = malloc(num_faces * sizeof(int));
    game_model->face_intensity = malloc(num_faces * sizeof(int));
    game_model->normal_scale = malloc(num_faces * sizeof(int));
    game_model->normal_magnitude = malloc(num_faces * sizeof(int));

    if (!game_model->projected) {
        game_model->project_vertex_x = malloc(num_vertices * sizeof(int));
        game_model->project_vertex_y = malloc(num_vertices * sizeof(int));
        game_model->project_vertex_z = malloc(num_vertices * sizeof(int));
        game_model->vertex_view_x = malloc(num_vertices * sizeof(int));
        game_model->vertex_view_y = malloc(num_vertices * sizeof(int));
    }

    if (!game_model->unpickable) {
        game_model->is_local_player = malloc(num_faces);
        game_model->face_tag = malloc(num_faces * sizeof(int));
    }

    if (game_model->autocommit) {
        game_model->vertex_transformed_x = game_model->vertex_x;
        game_model->vertex_transformed_y = game_model->vertex_y;
        game_model->vertex_transformed_z = game_model->vertex_z;
    } else {
        game_model->vertex_transformed_x = malloc(num_vertices * sizeof(int));
        game_model->vertex_transformed_y = malloc(num_vertices * sizeof(int));
        game_model->vertex_transformed_z = malloc(num_vertices * sizeof(int));
    }

    if (!game_model->unlit || !game_model->isolated) {
        game_model->face_normal_x = malloc(num_faces * sizeof(int));
        game_model->face_normal_y = malloc(num_faces * sizeof(int));
        game_model->face_normal_z = malloc(num_faces * sizeof(int));
    }

    if (!game_model->isolated) {
        game_model->face_bound_left = malloc(num_faces * sizeof(int));
        game_model->face_bound_right = malloc(num_faces * sizeof(int));
        game_model->face_bound_bottom = malloc(num_faces * sizeof(int));
        game_model->face_bound_top = malloc(num_faces * sizeof(int));
        game_model->face_bound_near = malloc(num_faces * sizeof(int));
        game_model->face_bound_far = malloc(num_faces * sizeof(int));
    }

    game_model->num_faces = 0;
    game_model->num_vertices = 0;
    game_model->max_verts = num_vertices;
    game_model->max_faces = num_faces;

    game_model_reset(game_model);
}

void game_model_projection_prepare(GameModel *game_model) {
    game_model->project_vertex_x =
        malloc(game_model->num_vertices * sizeof(int));

    game_model->project_vertex_y =
        malloc(game_model->num_vertices * sizeof(int));

    game_model->project_vertex_z =
        malloc(game_model->num_vertices * sizeof(int));

    game_model->vertex_view_x = malloc(game_model->num_vertices * sizeof(int));
    game_model->vertex_view_y = malloc(game_model->num_vertices * sizeof(int));
}

void game_model_clear(GameModel *game_model) {
    /* TODO free here as well? */
    game_model->num_faces = 0;
    game_model->num_vertices = 0;
}

void game_model_reduce(GameModel *game_model, int delta_faces,
                       int delta_vertices) {
    game_model->num_faces -= delta_faces;

    if (game_model->num_faces < 0) {
        game_model->num_faces = 0;
    }

    game_model->num_vertices -= delta_vertices;

    if (game_model->num_vertices < 0) {
        game_model->num_vertices = 0;
    }
}

void game_model_merge(GameModel *game_model, GameModel **pieces, int count,
                      int trans_state) {
    int num_faces = 0;
    int num_vertices = 0;

    for (int i = 0; i < count; i++) {
        num_faces += pieces[i]->num_faces;
        num_vertices += pieces[i]->num_vertices;
    }

    game_model_allocate(game_model, num_vertices, num_faces);

    if (trans_state) {
        /* TODO check for existing one and free? */
        game_model->face_trans_state_thing = malloc(num_faces * sizeof(int *));
    }

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

            if (trans_state) {
                if (count > 1) {
                    int source_trans_length =
                        sizeof(source->face_trans_state_thing[src_f]) /
                        sizeof(int *);

                    game_model->face_trans_state_thing[dst_f] =
                        malloc((source_trans_length + 1) * sizeof(int));

                    game_model->face_trans_state_thing[dst_f][0] = i;

                    for (int j = 0; j < source_trans_length; j++) {
                        game_model->face_trans_state_thing[dst_f][j + 1] =
                            source->face_trans_state_thing[src_f][j];
                    }
                } else {
                    int source_trans_length =
                        sizeof(source->face_trans_state_thing[src_f]) /
                        sizeof(int *);

                    game_model->face_trans_state_thing[dst_f] =
                        malloc(source_trans_length * sizeof(int));

                    for (int j = 0; j < source_trans_length; j++) {
                        game_model->face_trans_state_thing[dst_f][j] =
                            source->face_trans_state_thing[src_f][j];
                    }
                }
            }
        }
    }
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
                           int front, int back) {
    if (game_model->num_faces >= game_model->max_faces) {
        return -1;
    }

    game_model->face_num_vertices[game_model->num_faces] = number;
    game_model->face_vertices[game_model->num_faces] = vertices;
    game_model->face_fill_front[game_model->num_faces] = front;
    game_model->face_fill_back[game_model->num_faces] = back;
    game_model->transform_state = 1;

    return game_model->num_faces++;
}

GameModel **split(GameModel *game_model, int unused1, int unused2, int piece_dx,
                  int piece_dz, int rows, int count, int piece_max_vertices,
                  int pickable) {
    game_model_commit(game_model);

    int *piece_nV = malloc(count * sizeof(int));
    int *piece_nF = malloc(count * sizeof(int));

    for (int i = 0; i < count; i++) {
        piece_nV[i] = 0;
        piece_nF[i] = 0;
    }

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

        piece_nV[p] += n;
        piece_nF[p]++;
    }

    /* TODO maybe pieces should be a function argument instead */
    GameModel **pieces = malloc(sizeof(GameModel *) * count);

    for (int i = 0; i < count; i++) {
        if (piece_nV[i] > piece_max_vertices) {
            piece_nV[i] = piece_max_vertices;
        }

        pieces[i] = malloc(sizeof(GameModel));

        game_model_from7(pieces[i], piece_nV[i], piece_nF[i], 1, 1, 1, pickable,
                         1);

        pieces[i]->light_diffuse = game_model->light_diffuse;
        pieces[i]->light_ambience = game_model->light_ambience;
    }

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

    return pieces;
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
        game_model->face_intensity[i] = gouraud ? game_model->magic : 0;
    }

    game_model_set_light_from5(game_model, ambience, diffuse, x, y, z);
}

void game_model_set_vertex_ambience(GameModel *game_model, int v,
                                    int ambience) {
    game_model->vertex_ambience[v] = ambience & 0xff;
}

void game_model_rotate(GameModel *game_model, int yaw, int pitch, int roll) {
    game_model->orientation_yaw = (game_model->orientation_yaw + yaw) & 0xff;

    game_model->orientation_pitch =
        (game_model->orientation_pitch + pitch) & 0xff;

    game_model->orientation_roll = (game_model->orientation_roll + roll) & 0xff;
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = 1;
}

void game_model_orient(GameModel *game_model, int yaw, int pitch, int roll) {
    game_model->orientation_yaw = yaw & 0xff;
    game_model->orientation_pitch = pitch & 0xff;
    game_model->orientation_roll = roll & 0xff;
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = 1;
}

void game_model_translate(GameModel *game_model, int x, int y, int z) {
    game_model->base_x += x;
    game_model->base_y += y;
    game_model->base_z += z;
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = 1;
}

void game_model_place(GameModel *game_model, int x, int y, int z) {
    game_model->base_x = x;
    game_model->base_y = y;
    game_model->base_z = z;
    game_model_determine_transform_kind(game_model);
    game_model->transform_state = 1;
}

void game_model_determine_transform_kind(GameModel *game_model) {
    if (game_model->shear_xy != 256 || game_model->shear_xz != 256 ||
        game_model->shear_yx != 256 || game_model->shear_yz != 256 ||
        game_model->shear_zx != 256 || game_model->shear_zy != 256) {
        game_model->transform_kind = 4;
    } else if (game_model->scale_fx != 256 || game_model->scale_fy != 256 ||
               game_model->scale_fz != 256) {
        game_model->transform_kind = 3;
    } else if (game_model->orientation_yaw != 0 ||
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
            int sin = sine9[pitch];
            int cos = sine9[pitch + 256];

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
            int sin = sine9[yaw];
            int cos = sine9[yaw + 256];

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
            int sin = sine9[roll];
            int cos = sine9[roll + 256];

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

void game_model_apply_shear(GameModel *game_model, int xy, int xz, int yx,
                            int yz, int zx, int zy) {
    for (int i = 0; i < game_model->num_vertices; i++) {
        if (xy != 0) {
            game_model->vertex_transformed_x[i] +=
                (game_model->vertex_transformed_y[i] * xy) >> 8;
        }

        if (xz != 0) {
            game_model->vertex_transformed_z[i] +=
                (game_model->vertex_transformed_y[i] * xz) >> 8;
        }

        if (yx != 0) {
            game_model->vertex_transformed_x[i] +=
                (game_model->vertex_transformed_z[i] * yx) >> 8;
        }

        if (yz != 0) {
            game_model->vertex_transformed_y[i] +=
                (game_model->vertex_transformed_z[i] * yz) >> 8;
        }

        if (zx != 0) {
            game_model->vertex_transformed_z[i] +=
                (game_model->vertex_transformed_x[i] * zx) >> 8;
        }

        if (zy != 0) {
            game_model->vertex_transformed_y[i] +=
                (game_model->vertex_transformed_x[i] * zy) >> 8;
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

        if (!game_model->isolated) {
            game_model->face_bound_left[face] = x1;
            game_model->face_bound_right[face] = x2;
            game_model->face_bound_bottom[face] = y1;
            game_model->face_bound_top[face] = y2;
            game_model->face_bound_near[face] = z1;
            game_model->face_bound_far[face] = z2;
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
        if (game_model->face_intensity[i] != game_model->magic) {
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

    /*int *normal_x = malloc(game_model->num_vertices * sizeof(int));
    int *normal_y = malloc(game_model->num_vertices * sizeof(int));
    int *normal_z = malloc(game_model->num_vertices * sizeof(int));
    int *normal_magnitude = malloc(game_model->num_vertices * sizeof(int));*/
    int normal_x[game_model->num_vertices];
    int normal_y[game_model->num_vertices];
    int normal_z[game_model->num_vertices];
    int normal_magnitude[game_model->num_vertices];

    for (int i = 0; i < game_model->num_vertices; i++) {
        normal_x[i] = 0;
        normal_y[i] = 0;
        normal_z[i] = 0;
        normal_magnitude[i] = 0;
    }

    for (int i = 0; i < game_model->num_faces; i++) {
        if (game_model->face_intensity[i] == game_model->magic) {
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

    /*free(normal_x);
    free(normal_y);
    free(normal_z);
    free(normal_magnitude);*/
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

        return;
    }

    if (game_model->transform_state == 1) {
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

        if (game_model->transform_kind >= 3) {
            game_model_apply_scale(game_model, game_model->scale_fx,
                                   game_model->scale_fy, game_model->scale_fz);
        }

        if (game_model->transform_kind >= 4) {
            game_model_apply_shear(game_model, game_model->shear_xy,
                                   game_model->shear_xz, game_model->shear_yx,
                                   game_model->shear_yz, game_model->shear_zx,
                                   game_model->shear_zy);
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
                        int camera_yaw, int view_dist, int clip_near) {
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

    if (camera_yaw != 0) {
        yaw_sin = sine11[camera_yaw];
        yaw_cos = sine11[camera_yaw + 1024];
    }

    if (camera_roll != 0) {
        roll_sin = sine11[camera_roll];
        roll_cos = sine11[camera_roll + 1024];
    }

    if (camera_pitch != 0) {
        pitch_sin = sine11[camera_pitch];
        pitch_cos = sine11[camera_pitch + 1024];
    }

    for (int i = 0; i < game_model->num_vertices; i++) {
        int x = game_model->vertex_transformed_x[i] - camera_x;
        int y = game_model->vertex_transformed_y[i] - camera_y;
        int z = game_model->vertex_transformed_z[i] - camera_z;

        if (camera_yaw != 0) {
            int X = (y * yaw_sin + x * yaw_cos) >> 15;
            y = (y * yaw_cos - x * yaw_sin) >> 15;
            x = X;
        }

        if (camera_roll != 0) {
            int X = (z * roll_sin + x * roll_cos) >> 15;
            z = (z * roll_cos - x * roll_sin) >> 15;
            x = X;
        }

        if (camera_pitch != 0) {
            int Y = (y * pitch_cos - z * pitch_sin) >> 15;
            z = (y * pitch_sin + z * pitch_cos) >> 15;
            y = Y;
        }

        if (z >= clip_near) {
            game_model->vertex_view_x[i] = (int)((x << view_dist) / z);
        } else {
            game_model->vertex_view_x[i] = x << view_dist;
        }

        if (z >= clip_near) {
            game_model->vertex_view_y[i] = (int)((y << view_dist) / z);
        } else {
            game_model->vertex_view_y[i] = y << view_dist;
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

    return copy;
}

GameModel *game_model_copy_from4(GameModel *game_model, int autocommit,
                                 int isolated, int unlit, int pickable) {
    GameModel **pieces = malloc(sizeof(GameModel *));
    pieces[0] = game_model;

    GameModel *copy = malloc(sizeof(GameModel));
    game_model_from6(copy, pieces, 1, autocommit, isolated, unlit, pickable);
    copy->depth = game_model->depth;

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
