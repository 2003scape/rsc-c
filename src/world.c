#include "world.h"

int terrain_colours[TERRAIN_COLOUR_COUNT];

void init_world_global() {
    for (int i = 0; i < 64; i++) {
        terrain_colours[i] =
            rgb(255 - i * 4, 255 - (int)((double)i * 1.75), 255 - i * 4);
    }

    for (int i = 0; i < 64; i++) {
        terrain_colours[i + 64] = rgb(i * 3, 144, 0);
    }

    for (int i = 0; i < 64; i++) {
        terrain_colours[i + 128] =
            rgb(192 - (int)((double)i * 1.5), 144 - (int)((double)i * 1.5), 0);
    }

    for (int i = 0; i < 64; i++) {
        terrain_colours[i + 192] = rgb(96 - (int)((double)i * 1.5),
                                       REGION_SIZE + (int)((double)i * 1.5), 0);
    }
}

void world_new(World *world, Scene *scene, Surface *surface) {
    memset(world, 0, sizeof(World));

    world->scene = scene;
    world->surface = surface;
    world->player_alive = 0;
    world->base_media_sprite = 750;
}

int get_byte_plane_coord(int8_t plane_array[PLANE_COUNT][TILE_COUNT], int x,
                         int y) {
    if (x < 0 || x >= REGION_WIDTH || y < 0 || y >= REGION_HEIGHT) {
        return 0;
    }

    int8_t height = 0;

    if (x >= REGION_SIZE && y < REGION_SIZE) {
        height = 1;
        x -= REGION_SIZE;
    } else if (x < REGION_SIZE && y >= REGION_SIZE) {
        height = 2;
        y -= REGION_SIZE;
    } else if (x >= REGION_SIZE && y >= REGION_SIZE) {
        height = 3;
        x -= REGION_SIZE;
        y -= REGION_SIZE;
    }

    return plane_array[height][x * REGION_SIZE + y] & 0xff;
}

int world_get_wall_east_west(World *world, int x, int y) {
    return get_byte_plane_coord(world->walls_east_west, x, y);
}

void world_set_terrain_ambience(World *world, int terrain_x, int terrain_y,
                                int vertex_x, int vertex_y, int ambience) {
    GameModel *game_model = world->terrain_models[terrain_x + terrain_y * 8];

    for (int i = 0; i < game_model->num_vertices; i++) {
        if (game_model->vertex_x[i] == vertex_x * TILE_SIZE &&
            game_model->vertex_z[i] == vertex_y * TILE_SIZE) {
            game_model_set_vertex_ambience(game_model, i, ambience);
            break;
        }
    }
}

int world_get_wall_roof(World *world, int x, int y) {
    return get_byte_plane_coord(world->walls_roof, x, y);
}

int world_get_elevation(World *world, int x, int y) {
    int s_x = x / TILE_SIZE;
    int s_y = y / TILE_SIZE;
    int a_x = x & (TILE_SIZE - 1);
    int a_y = y & (TILE_SIZE - 1);

    if (s_x < 0 || s_y < 0 || s_x >= 95 || s_y >= 95) {
        return 0;
    }

    int height = 0;
    int height_east = 0;
    int height_south = 0;

    if (a_x <= TILE_SIZE - a_y) {
        height = world_get_terrain_height(world, s_x, s_y);
        height_east = world_get_terrain_height(world, s_x + 1, s_y) - height;
        height_south = world_get_terrain_height(world, s_x, s_y + 1) - height;
    } else {
        height = world_get_terrain_height(world, s_x + 1, s_y + 1);
        height_east = world_get_terrain_height(world, s_x, s_y + 1) - height;
        height_south = world_get_terrain_height(world, s_x + 1, s_y) - height;
        a_x = TILE_SIZE - a_x;
        a_y = TILE_SIZE - a_y;
    }

    return (height + ((height_east * a_x) / TILE_SIZE) +
            ((height_south * a_y) / TILE_SIZE));
}

int world_get_wall_diagonal(World *world, int x, int y) {
    if (x < 0 || x >= REGION_WIDTH || y < 0 || y >= REGION_HEIGHT) {
        return 0;
    }

    int8_t height = 0;

    if (x >= REGION_SIZE && y < REGION_SIZE) {
        height = 1;
        x -= REGION_SIZE;
    } else if (x < REGION_SIZE && y >= REGION_SIZE) {
        height = 2;
        y -= REGION_SIZE;
    } else if (x >= REGION_SIZE && y >= REGION_SIZE) {
        height = 3;
        x -= REGION_SIZE;
        y -= REGION_SIZE;
    }

    return world->walls_diagonal[height][x * REGION_SIZE + y];
}

// TODO i don't think this removes
void world_remove_object2(World *world, int x, int y, int id) {
    if (x < 0 || y < 0 || x >= 95 || y >= 95) {
        return;
    }

    if (game_data_object_type[id] == 1 || game_data_object_type[id] == 2) {
        int tile_direction = world_get_tile_direction(world, x, y);
        int model_width;
        int model_height;

        // TODO make cardinal TILE_DIRs
        if (tile_direction == 0 || tile_direction == 4) {
            model_width = game_data_object_width[id];
            model_height = game_data_object_height[id];
        } else {
            model_height = game_data_object_width[id];
            model_width = game_data_object_height[id];
        }

        for (int mx = x; mx < x + model_width; mx++) {
            for (int my = y; my < y + model_height; my++) {
                if (game_data_object_type[id] == 1) {
                    world->object_adjacency[mx][my] |= 0x40;
                } else if (tile_direction == 0) {
                    world->object_adjacency[mx][my] |= 2;

                    if (mx > 0) {
                        world_set_object_adjacency_from3(world, mx - 1, my, 8);
                    }
                } else if (tile_direction == 2) {
                    world->object_adjacency[mx][my] |= 4;

                    if (my < 95) {
                        world_set_object_adjacency_from3(world, mx, my + 1, 1);
                    }
                } else if (tile_direction == 4) {
                    world->object_adjacency[mx][my] |= 8;

                    if (mx < 95) {
                        world_set_object_adjacency_from3(world, mx + 1, my, 2);
                    }
                } else if (tile_direction == 6) {
                    world->object_adjacency[mx][my] |= 1;

                    if (my > 0) {
                        world_set_object_adjacency_from3(world, mx, my - 1, 4);
                    }
                }
            }
        }

        world_method404(world, x, y, model_width, model_height);
    }
}

void world_remove_wall_object(World *world, int x, int y, int k, int id) {
    if (x < 0 || y < 0 || x >= 95 || y >= 95) {
        return;
    }

    if (game_data_wall_object_adjacent[id] == 1) {
        if (k == 0) {
            world->object_adjacency[x][y] &= 0xfffe;

            if (y > 0) {
                world_method407(world, x, y - 1, 4);
            }
        } else if (k == 1) {
            world->object_adjacency[x][y] &= 0xfffd;

            if (x > 0) {
                world_method407(world, x - 1, y, 8);
            }
        } else if (k == 2) {
            world->object_adjacency[x][y] &= 0xffef;
        } else if (k == 3) {
            world->object_adjacency[x][y] &= 0xffdf;
        }

        world_method404(world, x, y, 1, 1);
    }
}

void world_draw_map_tile(World *world, int x, int y, int direction,
                         int face_fill_1, int face_fill_2) {
    int line_x = x * 3;
    int line_y = y * 3;
    int colour_1 = scene_get_fill_colour(world->scene, face_fill_1);
    int colour_2 = scene_get_fill_colour(world->scene, face_fill_2);

    /* darken colours by half */
    colour_1 = colour_1 >> 1 & 0x7f7f7f;
    colour_2 = colour_2 >> 1 & 0x7f7f7f;

    if (direction == 0) {
        surface_draw_line_horizontal_software(world->surface, line_x, line_y, 3,
                                     colour_1);

        surface_draw_line_horizontal_software(world->surface, line_x, line_y + 1, 2,
                                     colour_1);

        surface_draw_line_horizontal_software(world->surface, line_x, line_y + 2, 1,
                                     colour_1);

        surface_draw_line_horizontal_software(world->surface, line_x + 2, line_y + 1, 1,
                                     colour_2);

        surface_draw_line_horizontal_software(world->surface, line_x + 1, line_y + 2, 2,
                                     colour_2);
    } else if (direction == 1) {
        surface_draw_line_horizontal_software(world->surface, line_x, line_y, 3,
                                     colour_2);

        surface_draw_line_horizontal_software(world->surface, line_x + 1, line_y + 1, 2,
                                     colour_2);

        surface_draw_line_horizontal_software(world->surface, line_x + 2, line_y + 2, 1,
                                     colour_2);

        surface_draw_line_horizontal_software(world->surface, line_x, line_y + 1, 1,
                                     colour_1);

        surface_draw_line_horizontal_software(world->surface, line_x, line_y + 2, 2,
                                     colour_1);
    }
}

/* TODO rename to read map files? */
void world_load_section_from4i(World *world, int x, int y, int plane,
                               int chunk) {
    if (world->landscape_pack == NULL) {
        return;
    }

    char map_name[37]; // 2 digits for %d (10), m (1), file ext (4) and null
    sprintf(map_name, "m%d%d%d%d%d", plane, x / 10, x % 10, y / 10, y % 10);
    int map_name_length = strlen(map_name);

    strcpy(map_name + map_name_length, ".hei");

    int8_t *map_data = load_data(map_name, 0, world->landscape_pack);

    if (map_data == NULL && world->member_landscape_pack != NULL) {
        map_data = load_data(map_name, 0, world->member_landscape_pack);
    }

    if (map_data != NULL) {
        int offset = 0;
        int last_val = 0;

        for (int tile = 0; tile < TILE_COUNT;) {
            int val = map_data[offset++] & 0xff;

            if (val < 128) {
                world->terrain_height[chunk][tile++] = (int8_t)val;
                last_val = val;
            }

            if (val >= 128) {
                for (int i = 0; i < val - 128; i++)
                    world->terrain_height[chunk][tile++] = (int8_t)last_val;
            }
        }

        last_val = 64;

        for (int tile_y = 0; tile_y < 48; tile_y++) {
            for (int tile_x = 0; tile_x < 48; tile_x++) {
                last_val = (world->terrain_height[chunk][tile_x * 48 + tile_y] +
                            last_val) &
                           127;

                world->terrain_height[chunk][tile_x * 48 + tile_y] =
                    (int8_t)(last_val * 2);
            }
        }

        last_val = 0;

        for (int tile = 0; tile < TILE_COUNT;) {
            int val = map_data[offset++] & 0xff;

            if (val < 128) {
                world->terrain_colour[chunk][tile++] = (int8_t)val;
                last_val = val;
            }

            if (val >= 128) {
                for (int i = 0; i < val - 128; i++)
                    world->terrain_colour[chunk][tile++] = (int8_t)last_val;
            }
        }

        last_val = 35;

        for (int tile_y = 0; tile_y < 48; tile_y++) {
            for (int tile_x = 0; tile_x < 48; tile_x++) {
                last_val = (world->terrain_colour[chunk][tile_x * 48 + tile_y] +
                            last_val) &
                           127;

                world->terrain_colour[chunk][tile_x * 48 + tile_y] =
                    (int8_t)(last_val * 2);
            }
        }
    } else {
        for (int tile = 0; tile < TILE_COUNT; tile++) {
            world->terrain_height[chunk][tile] = 0;
            world->terrain_colour[chunk][tile] = 0;
        }
    }

    free(map_data);

    strcpy(map_name + map_name_length, ".dat");

    map_data = load_data(map_name, 0, world->map_pack);

    if (map_data == NULL && world->member_map_pack != NULL) {
        map_data = load_data(map_name, 0, world->member_map_pack);
    }

    if (map_data != NULL) {
        int offset = 0;

        for (int tile = 0; tile < TILE_COUNT; tile++) {
            world->walls_north_south[chunk][tile] = map_data[offset++];
        }

        for (int tile = 0; tile < TILE_COUNT; tile++) {
            world->walls_east_west[chunk][tile] = map_data[offset++];
        }

        for (int tile = 0; tile < TILE_COUNT; tile++) {
            world->walls_diagonal[chunk][tile] = map_data[offset++] & 0xff;
        }

        for (int tile = 0; tile < TILE_COUNT; tile++) {
            int val = map_data[offset++] & 0xff;

            if (val > 0) {
                world->walls_diagonal[chunk][tile] = val + 12000;
            }
        }

        for (int tile = 0; tile < TILE_COUNT;) {
            int val = map_data[offset++] & 0xff;

            if (val < 128) {
                world->walls_roof[chunk][tile++] = (int8_t)val;
            } else {
                for (int i = 0; i < val - 128; i++) {
                    world->walls_roof[chunk][tile++] = 0;
                }
            }
        }

        int last_val = 0;

        for (int tile = 0; tile < TILE_COUNT;) {
            int val = map_data[offset++] & 0xff;

            if (val < 128) {
                world->tile_decoration[chunk][tile++] = (int8_t)val;
                last_val = val;
            } else {
                for (int i = 0; i < val - 128; i++) {
                    world->tile_decoration[chunk][tile++] = (int8_t)last_val;
                }
            }
        }

        for (int tile = 0; tile < TILE_COUNT;) {
            int val = map_data[offset++] & 0xff;

            if (val < 128) {
                world->tile_direction[chunk][tile++] = (int8_t)val;
            } else {
                for (int i = 0; i < val - 128; i++) {
                    world->tile_direction[chunk][tile++] = 0;
                }
            }
        }

        free(map_data);

        strcpy(map_name + map_name_length, ".loc");

        map_data = load_data(map_name, 0, world->map_pack);

        if (map_data != NULL) {
            offset = 0;

            for (int tile = 0; tile < TILE_COUNT;) {
                int val = map_data[offset++] & 0xff;

                if (val < 128) {
                    world->walls_diagonal[chunk][tile++] = val + 48000;
                } else {
                    tile += val - 128;
                }
            }

            free(map_data);
        }
    } else {
        for (int tile = 0; tile < TILE_COUNT; tile++) {
            world->terrain_height[chunk][tile] = 0;
            world->terrain_colour[chunk][tile] = 0;
            world->walls_north_south[chunk][tile] = 0;
            world->walls_east_west[chunk][tile] = 0;
            world->walls_diagonal[chunk][tile] = 0;
            world->walls_roof[chunk][tile] = 0;

            if (plane == 0) {
                world->tile_decoration[chunk][tile] = -6;
            } else if (plane == 3) {
                world->tile_decoration[chunk][tile] = 8;
            } else {
                world->tile_decoration[chunk][tile] = 0;
            }

            world->tile_direction[chunk][tile] = 0;
        }
    }
}

/* TODO add adjacent shadows? */

void world_method404(World *world, int x, int y, int width, int height) {
    if (x < 1 || y < 1 || x + width >= REGION_WIDTH ||
        y + height >= REGION_HEIGHT) {
        return;
    }

    for (int xx = x; xx <= x + width; xx++) {
        for (int yy = y; yy <= y + height; yy++) {
            if ((world_get_object_adjacency(world, xx, yy) & 0x63) != 0 ||
                (world_get_object_adjacency(world, xx - 1, yy) & 0x59) != 0 ||
                (world_get_object_adjacency(world, xx, yy - 1) & 0x56) != 0 ||
                (world_get_object_adjacency(world, xx - 1, yy - 1) & 0x6c) !=
                    0) {
                world_method425(world, xx, yy, 35);
            } else {
                world_method425(world, xx, yy, 0);
            }
        }
    }
}

int world_get_object_adjacency(World *world, int x, int y) {
    if (x < 0 || x >= REGION_WIDTH || y < 0 || y >= REGION_HEIGHT) {
        return 0;
    }

    return world->object_adjacency[x][y];
}

int world_has_roof(World *world, int x, int y) {
    return world_get_wall_roof(world, x, y) > 0 &&
           world_get_wall_roof(world, x - 1, y) > 0 &&
           world_get_wall_roof(world, x - 1, y - 1) > 0 &&
           world_get_wall_roof(world, x, y - 1) > 0;
}

void world_method407(World *world, int i, int j, int k) {
    world->object_adjacency[i][j] &= 0xffff - k;
}

int world_get_terrain_colour(World *world, int x, int y) {
    return get_byte_plane_coord(world->terrain_colour, x, y);
}

void world_reset(World *world, int dispose) {
    for (int i = 0; i < TERRAIN_COUNT; i++) {
        scene_null_model(world->scene, world->terrain_models[i]);
        game_model_destroy(world->terrain_models[i]);
        free(world->terrain_models[i]);

        world->terrain_models[i] = NULL;

        for (int j = 0; j < PLANE_COUNT; j++) {
            scene_null_model(world->scene, world->wall_models[j][i]);
            game_model_destroy(world->wall_models[j][i]);
            free(world->wall_models[j][i]);

            world->wall_models[j][i] = NULL;
        }

        for (int j = 0; j < PLANE_COUNT; j++) {
            scene_null_model(world->scene, world->roof_models[j][i]);
            game_model_destroy(world->roof_models[j][i]);
            free(world->roof_models[j][i]);

            world->roof_models[j][i] = NULL;
        }
    }

    if (dispose) {
        /* disable dispose for the login-screen models so we can free them */
        scene_dispose(world->scene);
    }
}

// TODO adds the water and sloping tiles
void world_set_tiles(World *world) {
    for (int x = 0; x < REGION_WIDTH; x++) {
        for (int y = 0; y < REGION_HEIGHT; y++) {
            if (world_get_tile_decoration(world, x, y) != 250) {
                continue;
            }

            if (x == (REGION_SIZE - 1) &&
                world_get_tile_decoration(world, x + 1, y) != 250 &&
                world_get_tile_decoration(world, x + 1, y) != 2) {
                world_set_tile_decoration(world, x, y, 9);
            } else if (y == (REGION_SIZE - 1) &&
                       world_get_tile_decoration(world, x, y + 1) != 250 &&
                       world_get_tile_decoration(world, x, y + 1) != 2) {
                world_set_tile_decoration(world, x, y, 9);
            } else {
                world_set_tile_decoration(world, x, y, 2);
            }
        }
    }
}

int world_get_wall_north_south(World *world, int x, int y) {
    return get_byte_plane_coord(world->walls_north_south, x, y);
}

int world_get_tile_direction(World *world, int x, int y) {
    return get_byte_plane_coord(world->tile_direction, x, y);
}

int world_get_tile_decoration(World *world, int x, int y) {
    return get_byte_plane_coord(world->tile_decoration, x, y) & 0xff;
}

// TODO rename
int world_get_tile_decoration_from4(World *world, int x, int y) {
    int decoration = world_get_tile_decoration(world, x, y);

    if (decoration == 0) {
        return decoration;
    }

    return game_data_tile_decoration[decoration - 1];
}

void world_set_tile_decoration(World *world, int x, int y, int decoration) {
    if (x < 0 || x >= REGION_WIDTH || y < 0 || y >= REGION_HEIGHT) {
        return;
    }

    int8_t height = 0;

    if (x >= REGION_SIZE && y < REGION_SIZE) {
        height = 1;
        x -= REGION_SIZE;
    } else if (x < REGION_SIZE && y >= REGION_SIZE) {
        height = 2;
        y -= REGION_SIZE;
    } else if (x >= REGION_SIZE && y >= REGION_SIZE) {
        height = 3;
        x -= REGION_SIZE;
        y -= REGION_SIZE;
    }

    world->tile_decoration[height][x * REGION_SIZE + y] = (int8_t)decoration;
}

int world_route(World *world, int start_x, int start_y, int end_x1, int end_y1,
                int end_x2, int end_y2, int *route_x, int *route_y,
                int objects) {
    for (int x = 0; x < REGION_WIDTH; x++) {
        for (int y = 0; y < REGION_HEIGHT; y++) {
            world->route_via[x][y] = 0;
        }
    }

    int write_ptr = 0;
    int read_ptr = 0;
    int x = start_x;
    int y = start_y;

    world->route_via[start_x][start_y] = 99;
    route_x[write_ptr] = start_x;
    route_y[write_ptr++] = start_y;

    int reached = 0;

    while (read_ptr != write_ptr) {
        x = route_x[read_ptr];
        y = route_y[read_ptr];
        read_ptr = (read_ptr + 1) % PATH_STEPS_MAX;

        if (x >= end_x1 && x <= end_x2 && y >= end_y1 && y <= end_y2) {
            reached = 1;
            break;
        }

        if (objects) {
            if (x > 0 && x - 1 >= end_x1 && x - 1 <= end_x2 && y >= end_y1 &&
                y <= end_y2 && (world->object_adjacency[x - 1][y] & 8) == 0) {
                reached = 1;
                break;
            }

            if (x < (REGION_WIDTH - 1) && x + 1 >= end_x1 && x + 1 <= end_x2 &&
                y >= end_y1 && y <= end_y2 &&
                (world->object_adjacency[x + 1][y] & 2) == 0) {
                reached = 1;
                break;
            }

            if (y > 0 && x >= end_x1 && x <= end_x2 && y - 1 >= end_y1 &&
                y - 1 <= end_y2 &&
                (world->object_adjacency[x][y - 1] & 4) == 0) {
                reached = 1;
                break;
            }

            if (y < (REGION_HEIGHT - 1) && x >= end_x1 && x <= end_x2 &&
                y + 1 >= end_y1 && y + 1 <= end_y2 &&
                (world->object_adjacency[x][y + 1] & 1) == 0) {
                reached = 1;
                break;
            }
        }

        if (x > 0 && world->route_via[x - 1][y] == 0 &&
            (world->object_adjacency[x - 1][y] & 0x78) == 0) {
            route_x[write_ptr] = x - 1;
            route_y[write_ptr] = y;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x - 1][y] = 2;
        }

        if (x < (REGION_WIDTH - 1) && world->route_via[x + 1][y] == 0 &&
            (world->object_adjacency[x + 1][y] & 0x72) == 0) {
            route_x[write_ptr] = x + 1;
            route_y[write_ptr] = y;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x + 1][y] = 8;
        }

        if (y > 0 && world->route_via[x][y - 1] == 0 &&
            (world->object_adjacency[x][y - 1] & 0x74) == 0) {
            route_x[write_ptr] = x;
            route_y[write_ptr] = y - 1;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x][y - 1] = 1;
        }

        if (y < (REGION_HEIGHT - 1) && world->route_via[x][y + 1] == 0 &&
            (world->object_adjacency[x][y + 1] & 0x71) == 0) {
            route_x[write_ptr] = x;
            route_y[write_ptr] = y + 1;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x][y + 1] = 4;
        }

        if (x > 0 && y > 0 && (world->object_adjacency[x][y - 1] & 0x74) == 0 &&
            (world->object_adjacency[x - 1][y] & 0x78) == 0 &&
            (world->object_adjacency[x - 1][y - 1] & 0x7c) == 0 &&
            world->route_via[x - 1][y - 1] == 0) {
            route_x[write_ptr] = x - 1;
            route_y[write_ptr] = y - 1;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x - 1][y - 1] = 3;
        }

        if (x < (REGION_WIDTH - 1) && y > 0 &&
            (world->object_adjacency[x][y - 1] & 0x74) == 0 &&
            (world->object_adjacency[x + 1][y] & 0x72) == 0 &&
            (world->object_adjacency[x + 1][y - 1] & 0x76) == 0 &&
            world->route_via[x + 1][y - 1] == 0) {
            route_x[write_ptr] = x + 1;
            route_y[write_ptr] = y - 1;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x + 1][y - 1] = 9;
        }

        if (x > 0 && y < (REGION_HEIGHT - 1) &&
            (world->object_adjacency[x][y + 1] & 0x71) == 0 &&
            (world->object_adjacency[x - 1][y] & 0x78) == 0 &&
            (world->object_adjacency[x - 1][y + 1] & 0x79) == 0 &&
            world->route_via[x - 1][y + 1] == 0) {
            route_x[write_ptr] = x - 1;
            route_y[write_ptr] = y + 1;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x - 1][y + 1] = 6;
        }

        if (x < (REGION_WIDTH - 1) && y < (REGION_HEIGHT - 1) &&
            (world->object_adjacency[x][y + 1] & 0x71) == 0 &&
            (world->object_adjacency[x + 1][y] & 0x72) == 0 &&
            (world->object_adjacency[x + 1][y + 1] & 0x73) == 0 &&
            world->route_via[x + 1][y + 1] == 0) {
            route_x[write_ptr] = x + 1;
            route_y[write_ptr] = y + 1;
            write_ptr = (write_ptr + 1) % PATH_STEPS_MAX;
            world->route_via[x + 1][y + 1] = 12;
        }
    }

    if (!reached) {
        return -1;
    }

    read_ptr = 0;
    route_x[read_ptr] = x;
    route_y[read_ptr++] = y;
    int stride;

    for (int step = stride = world->route_via[x][y];
         x != start_x || y != start_y; step = world->route_via[x][y]) {
        if (step != stride) {
            stride = step;
            route_x[read_ptr] = x;
            route_y[read_ptr++] = y;
        }

        if ((step & 2) != 0) {
            x++;
        } else if ((step & 8) != 0) {
            x--;
        }

        if ((step & 1) != 0) {
            y++;
        } else if ((step & 4) != 0) {
            y--;
        }
    }

    return read_ptr;
}

void world_set_object_adjacency_from4(World *world, int x, int y, int dir,
                                      int id) {
    if (x < 0 || y < 0 || x >= 95 || y >= 95) {
        return;
    }

    if (game_data_wall_object_adjacent[id] == 1) {
        if (dir == 0) {
            world->object_adjacency[x][y] |= 1;

            if (y > 0) {
                world_set_object_adjacency_from3(world, x, y - 1, 4);
            }
        } else if (dir == 1) {
            world->object_adjacency[x][y] |= 2;

            if (x > 0) {
                world_set_object_adjacency_from3(world, x - 1, y, 8);
            }
        } else if (dir == 2) {
            world->object_adjacency[x][y] |= 0x10;
        } else if (dir == 3) {
            world->object_adjacency[x][y] |= 0x20;
        }

        world_method404(world, x, y, 1, 1);
    }
}

/* assemble a 3D model from world section */
void world_load_section_from4(World *world, int x, int y, int plane,
                              int is_current_plane) {
    int section_x = (x + (REGION_SIZE / 2)) / REGION_SIZE;
    int section_y = (y + (REGION_SIZE / 2)) / REGION_SIZE;

    world_load_section_from4i(world, section_x - 1, section_y - 1, plane, 0);
    world_load_section_from4i(world, section_x, section_y - 1, plane, 1);
    world_load_section_from4i(world, section_x - 1, section_y, plane, 2);
    world_load_section_from4i(world, section_x, section_y, plane, 3);

    world_set_tiles(world);

    if (world->parent_model == NULL) {
        world->parent_model = malloc(sizeof(GameModel));
    } else {
        game_model_destroy(world->parent_model);
    }

    game_model_from7(world->parent_model, 18688, 18688, 1, 1, 0, 0, 1);

    /* create terrain */

    if (is_current_plane) {
        surface_black_screen(world->surface);

        for (int r_x = 0; r_x < REGION_WIDTH; r_x++) {
            for (int r_y = 0; r_y < REGION_HEIGHT; r_y++) {
                world->object_adjacency[r_x][r_y] = 0;
            }
        }

        GameModel *game_model = world->parent_model;

        game_model_destroy(game_model);
        game_model_from7(game_model, 18688, 18688, 1, 1, 0, 0, 1);

        for (int r_x = 0; r_x < REGION_WIDTH; r_x++) {
            for (int r_y = 0; r_y < REGION_HEIGHT; r_y++) {
                int height = -world_get_terrain_height(world, r_x, r_y);
                int decoration = world_get_tile_decoration(world, r_x, r_y);

                /* keep water tiles under the bridge */

                if (decoration > 0 &&
                    game_data_tile_type[decoration - 1] == BRIDGE_TILE_TYPE) {
                    height = 0;
                }

                int decoration_east =
                    world_get_tile_decoration(world, r_x - 1, r_y);

                if (decoration_east > 0 &&
                    game_data_tile_type[decoration_east - 1] ==
                        BRIDGE_TILE_TYPE) {
                    height = 0;
                }

                int decoration_north =
                    world_get_tile_decoration(world, r_x, r_y - 1);

                if (decoration_north > 0 &&
                    game_data_tile_type[decoration_north - 1] ==
                        BRIDGE_TILE_TYPE) {
                    height = 0;
                }

                int decoration_north_east =
                    world_get_tile_decoration(world, r_x - 1, r_y - 1);

                if (decoration_north_east > 0 &&
                    game_data_tile_type[decoration_north_east - 1] ==
                        BRIDGE_TILE_TYPE) {
                    height = 0;
                }

                int vertex_index = game_model_vertex_at(
                    game_model, r_x * TILE_SIZE, height, r_y * TILE_SIZE);

                int ambience =
                    (int)(((float)rand() / (float)RAND_MAX) * 10) - 5;

                game_model_set_vertex_ambience(game_model, vertex_index,
                                               ambience);
            }
        }

        /* draw regular tiles */

        for (int r_x = 0; r_x < REGION_WIDTH - 1; r_x++) {
            for (int r_y = 0; r_y < REGION_HEIGHT - 1; r_y++) {
                int colour_index = world_get_terrain_colour(world, r_x, r_y);
                int colour = terrain_colours[colour_index];
                int colour_1 = colour;
                int colour_2 = colour;
                int direction = 0;

                if (plane == 1 || plane == 2) {
                    colour = COLOUR_TRANSPARENT;
                    colour_1 = COLOUR_TRANSPARENT;
                    colour_2 = COLOUR_TRANSPARENT;
                }

                int decoration = world_get_tile_decoration(world, r_x, r_y);

                if (decoration > 0) {
                    int tile_type = game_data_tile_type[decoration - 1];
                    int is_floor = world_get_tile_type(world, r_x, r_y);

                    colour = game_data_tile_decoration[decoration - 1];
                    colour_1 = game_data_tile_decoration[decoration - 1];

                    if (tile_type == BRIDGE_TILE_TYPE) {
                        colour = 1;
                        colour_1 = 1;

                        if (decoration == BRIDGE_TILE_DECORATION) {
                            colour = 31;
                            colour_1 = 31;
                        }
                    } else if (tile_type == HOLE_TILE_TYPE) {
                        int diagonal = world_get_wall_diagonal(world, r_x, r_y);

                        if (diagonal > 0 && diagonal < 24000) {
                            if (world_get_tile_decoration_from4(world, r_x - 1,
                                                                r_y) !=
                                    COLOUR_TRANSPARENT &&
                                world_get_tile_decoration_from4(world, r_x,
                                                                r_y - 1) !=
                                    COLOUR_TRANSPARENT) {
                                colour = world_get_tile_decoration_from4(
                                    world, r_x - 1, r_y);

                                direction = 0;
                            } else if (world_get_tile_decoration_from4(
                                           world, r_x + 1, r_y) !=
                                           COLOUR_TRANSPARENT &&
                                       world_get_tile_decoration_from4(
                                           world, r_x, r_y + 1) !=
                                           COLOUR_TRANSPARENT) {
                                colour_1 = world_get_tile_decoration_from4(
                                    world, r_x + 1, r_y);

                                direction = 0;
                            } else if (world_get_tile_decoration_from4(
                                           world, r_x + 1, r_y) !=
                                           COLOUR_TRANSPARENT &&
                                       world_get_tile_decoration_from4(
                                           world, r_x, r_y - 1) !=
                                           COLOUR_TRANSPARENT) {
                                colour_1 = world_get_tile_decoration_from4(
                                    world, r_x + 1, r_y);

                                direction = 1;
                            } else if (world_get_tile_decoration_from4(
                                           world, r_x - 1, r_y) !=
                                           COLOUR_TRANSPARENT &&
                                       world_get_tile_decoration_from4(
                                           world, r_x, r_y + 1) !=
                                           COLOUR_TRANSPARENT) {
                                colour = world_get_tile_decoration_from4(
                                    world, r_x - 1, r_y);

                                direction = 1;
                            }
                        }
                    } else if (tile_type != FLOOR_TILE_TYPE ||
                               (world_get_wall_diagonal(world, r_x, r_y) > 0 &&
                                world_get_wall_diagonal(world, r_x, r_y) <
                                    24000)) {

                        /* antialiasing on diagonals */

                        if (world_get_tile_type(world, r_x - 1, r_y) !=
                                is_floor &&
                            world_get_tile_type(world, r_x, r_y - 1) !=
                                is_floor) {
                            colour = colour_2;
                            direction = 0;
                        } else if (world_get_tile_type(world, r_x + 1, r_y) !=
                                       is_floor &&
                                   world_get_tile_type(world, r_x, r_y + 1) !=
                                       is_floor) {
                            colour_1 = colour_2;
                            direction = 0;
                        } else if (world_get_tile_type(world, r_x + 1, r_y) !=
                                       is_floor &&
                                   world_get_tile_type(world, r_x, r_y - 1) !=
                                       is_floor) {
                            colour_1 = colour_2;
                            direction = 1;
                        } else if (world_get_tile_type(world, r_x - 1, r_y) !=
                                       is_floor &&
                                   world_get_tile_type(world, r_x, r_y + 1) !=
                                       is_floor) {
                            colour = colour_2;
                            direction = 1;
                        }
                    }

                    if (game_data_tile_adjacent[decoration - 1] != 0) {
                        world->object_adjacency[r_x][r_y] |= 0x40;
                    }

                    if (tile_type == FLOOR_TILE_TYPE) {
                        world->object_adjacency[r_x][r_y] |= 0x80;
                    }
                }

                world_draw_map_tile(world, r_x, r_y, direction, colour,
                                    colour_1);

                int i17 = world_get_terrain_height(world, r_x + 1, r_y + 1) -
                          world_get_terrain_height(world, r_x + 1, r_y) +
                          world_get_terrain_height(world, r_x, r_y + 1) -
                          world_get_terrain_height(world, r_x, r_y);

                if (colour != colour_1 || i17 != 0) {
                    if (direction == 0) {
                        if (colour != COLOUR_TRANSPARENT) {
                            int *colour_vertices = malloc(3 * sizeof(int));

                            colour_vertices[0] = r_y + r_x * 96 + 96;
                            colour_vertices[1] = r_y + r_x * 96;
                            colour_vertices[2] = r_y + r_x * 96 + 1;

                            int tile_face = game_model_create_face(
                                game_model, 3, colour_vertices,
                                COLOUR_TRANSPARENT, colour);

                            world->local_x[tile_face] = r_x;
                            world->local_y[tile_face] = r_y;

                            game_model->face_tag[tile_face] =
                                TILE_FACE_TAG + tile_face;
                        }

                        if (colour_1 != COLOUR_TRANSPARENT) {
                            int *colour_1_vertices = malloc(3 * sizeof(int));

                            colour_1_vertices[0] = r_y + r_x * 96 + 1;
                            colour_1_vertices[1] = r_y + r_x * 96 + 96 + 1;
                            colour_1_vertices[2] = r_y + r_x * 96 + 96;

                            int tile_face = game_model_create_face(
                                game_model, 3, colour_1_vertices,
                                COLOUR_TRANSPARENT, colour_1);

                            world->local_x[tile_face] = r_x;
                            world->local_y[tile_face] = r_y;

                            game_model->face_tag[tile_face] =
                                TILE_FACE_TAG + tile_face;
                        }
                    } else {
                        if (colour != COLOUR_TRANSPARENT) {
                            int *colour_vertices = malloc(3 * sizeof(int));

                            colour_vertices[0] = r_y + r_x * 96 + 1;
                            colour_vertices[1] = r_y + r_x * 96 + 96 + 1;
                            colour_vertices[2] = r_y + r_x * 96;

                            int tile_face = game_model_create_face(
                                game_model, 3, colour_vertices,
                                COLOUR_TRANSPARENT, colour);

                            world->local_x[tile_face] = r_x;
                            world->local_y[tile_face] = r_y;

                            game_model->face_tag[tile_face] =
                                TILE_FACE_TAG + tile_face;
                        }

                        if (colour_1 != COLOUR_TRANSPARENT) {
                            int *colour_1_vertices = malloc(3 * sizeof(int));

                            colour_1_vertices[0] = r_y + r_x * 96 + 96;
                            colour_1_vertices[1] = r_y + r_x * 96;
                            colour_1_vertices[2] = r_y + r_x * 96 + 96 + 1;

                            int tile_face = game_model_create_face(
                                game_model, 3, colour_1_vertices,
                                COLOUR_TRANSPARENT, colour_1);

                            world->local_x[tile_face] = r_x;
                            world->local_y[tile_face] = r_y;

                            game_model->face_tag[tile_face] =
                                TILE_FACE_TAG + tile_face;
                        }
                    }
                } else if (colour != COLOUR_TRANSPARENT) {
                    int *vertices = malloc(4 * sizeof(int));

                    vertices[0] = r_y + r_x * 96 + 96;
                    vertices[1] = r_y + r_x * 96;
                    vertices[2] = r_y + r_x * 96 + 1;
                    vertices[3] = r_y + r_x * 96 + 96 + 1;

                    int tile_face = game_model_create_face(
                        game_model, 4, vertices, COLOUR_TRANSPARENT, colour);

                    world->local_x[tile_face] = r_x;
                    world->local_y[tile_face] = r_y;

                    game_model->face_tag[tile_face] = TILE_FACE_TAG + tile_face;
                }
            }
        }

        for (int r_x = 1; r_x < REGION_WIDTH - 1; r_x++) {
            for (int r_y = 1; r_y < REGION_HEIGHT - 1; r_y++) {
                int decoration = world_get_tile_decoration(world, r_x, r_y);

                /* create bridge floor tiles over water */

                if (decoration > 0 &&
                    game_data_tile_type[decoration - 1] == BRIDGE_TILE_TYPE) {
                    int fill_front = game_data_tile_decoration[decoration - 1];

                    int *vertices = malloc(4 * sizeof(int));

                    vertices[0] = game_model_vertex_at(
                        game_model, r_x * TILE_SIZE,
                        -world_get_terrain_height(world, r_x, r_y),
                        r_y * TILE_SIZE);

                    vertices[1] = game_model_vertex_at(
                        game_model, (r_x + 1) * TILE_SIZE,
                        -world_get_terrain_height(world, r_x + 1, r_y),
                        r_y * TILE_SIZE);

                    vertices[2] = game_model_vertex_at(
                        game_model, (r_x + 1) * TILE_SIZE,
                        -world_get_terrain_height(world, r_x + 1, r_y + 1),
                        (r_y + 1) * TILE_SIZE);

                    vertices[3] = game_model_vertex_at(
                        game_model, r_x * TILE_SIZE,
                        -world_get_terrain_height(world, r_x, r_y + 1),
                        (r_y + 1) * TILE_SIZE);

                    int tile_face =
                        game_model_create_face(game_model, 4, vertices,
                                               fill_front, COLOUR_TRANSPARENT);

                    world->local_x[tile_face] = r_x;
                    world->local_y[tile_face] = r_y;

                    game_model->face_tag[tile_face] = TILE_FACE_TAG + tile_face;

                    world_draw_map_tile(world, r_x, r_y, 0, fill_front,
                                        fill_front);
                } else if (decoration == 0 ||
                           game_data_tile_type[decoration - 1] !=
                               LIQUID_TILE_TYPE) {
                    int decoration_south =
                        world_get_tile_decoration(world, r_x, r_y + 1);

                    if (decoration_south > 0 &&
                        game_data_tile_type[decoration_south - 1] ==
                            BRIDGE_TILE_TYPE) {
                        int fill_front =
                            game_data_tile_decoration[decoration_south - 1];

                        int *vertices = malloc(4 * sizeof(int));

                        vertices[0] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y),
                            r_y * TILE_SIZE);

                        vertices[1] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y),
                            r_y * TILE_SIZE);

                        vertices[2] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        vertices[3] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        int tile_face = game_model_create_face(
                            game_model, 4, vertices, fill_front,
                            COLOUR_TRANSPARENT);

                        world->local_x[tile_face] = r_x;
                        world->local_y[tile_face] = r_y;

                        game_model->face_tag[tile_face] = TILE_FACE_TAG + tile_face;

                        world_draw_map_tile(world, r_x, r_y, 0, fill_front,
                                            fill_front);
                    }

                    int decoration_north =
                        world_get_tile_decoration(world, r_x, r_y - 1);

                    if (decoration_north > 0 &&
                        game_data_tile_type[decoration_north - 1] ==
                            BRIDGE_TILE_TYPE) {
                        int fill_front =
                            game_data_tile_decoration[world_get_tile_decoration(
                                                          world, r_x, r_y - 1) -
                                                      1];

                        int *vertices = malloc(4 * sizeof(int));

                        vertices[0] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y),
                            r_y * TILE_SIZE);

                        vertices[1] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y),
                            r_y * TILE_SIZE);

                        vertices[2] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        vertices[3] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        int tile_face = game_model_create_face(
                            game_model, 4, vertices, fill_front,
                            COLOUR_TRANSPARENT);

                        world->local_x[tile_face] = r_x;
                        world->local_y[tile_face] = r_y;

                        game_model->face_tag[tile_face] = TILE_FACE_TAG + tile_face;

                        world_draw_map_tile(world, r_x, r_y, 0, fill_front,
                                            fill_front);
                    }

                    int decoration_east =
                        world_get_tile_decoration(world, r_x + 1, r_y);

                    if (decoration_east > 0 &&
                        game_data_tile_type[decoration_east - 1] ==
                            BRIDGE_TILE_TYPE) {
                        int fill_front =
                            game_data_tile_decoration[decoration_east - 1];

                        int *vertices = malloc(4 * sizeof(int));

                        vertices[0] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y),
                            r_y * TILE_SIZE);

                        vertices[1] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y),
                            r_y * TILE_SIZE);

                        vertices[2] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        vertices[3] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        int tile_face = game_model_create_face(
                            game_model, 4, vertices, fill_front,
                            COLOUR_TRANSPARENT);

                        world->local_x[tile_face] = r_x;
                        world->local_y[tile_face] = r_y;

                        game_model->face_tag[tile_face] = TILE_FACE_TAG + tile_face;

                        world_draw_map_tile(world, r_x, r_y, 0, fill_front,
                                            fill_front);
                    }

                    int decoration_west =
                        world_get_tile_decoration(world, r_x - 1, r_y);

                    if (decoration_west > 0 &&
                        game_data_tile_type[decoration_west - 1] ==
                            BRIDGE_TILE_TYPE) {
                        int face_fill =
                            game_data_tile_decoration[decoration_west - 1];

                        int *vertices = malloc(4 * sizeof(int));

                        vertices[0] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y),
                            r_y * TILE_SIZE);

                        vertices[1] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y),
                            r_y * TILE_SIZE);

                        vertices[2] = game_model_vertex_at(
                            game_model, (r_x + 1) * TILE_SIZE,
                            -world_get_terrain_height(world, r_x + 1, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        vertices[3] = game_model_vertex_at(
                            game_model, r_x * TILE_SIZE,
                            -world_get_terrain_height(world, r_x, r_y + 1),
                            (r_y + 1) * TILE_SIZE);

                        int tile_face = game_model_create_face(
                            game_model, 4, vertices, face_fill,
                            COLOUR_TRANSPARENT);

                        world->local_x[tile_face] = r_x;
                        world->local_y[tile_face] = r_y;

                        game_model->face_tag[tile_face] = TILE_FACE_TAG + tile_face;

                        world_draw_map_tile(world, r_x, r_y, 0, face_fill,
                                            face_fill);
                    }
                }
            }
        }

        game_model_set_light_from6(game_model, 1, 40, 48, -50, -10, -50);

        // game_model_dump(game_model, 123);

        game_model_split(world->parent_model, world->terrain_models, 1536, 1536,
                         8, 64, 233, 0);

        for (int i = 0; i < TERRAIN_COUNT; i++) {
            scene_add_model(world->scene, world->terrain_models[i]);

#ifdef RENDER_GL
            world->world_models_buffer[world->world_models_offset++] =
                world->terrain_models[i];
#endif
        }

        for (int r_x = 0; r_x < REGION_WIDTH; r_x++) {
            for (int r_y = 0; r_y < REGION_HEIGHT; r_y++) {
                world->terrain_height_local[r_x][r_y] =
                    world_get_terrain_height(world, r_x, r_y);
            }
        }
    }

    game_model_destroy(world->parent_model);
    game_model_from7(world->parent_model, 18688, 18688, 1, 1, 0, 0, 1);

    int colour = 0x606060;

    /* create diagonal walls */

    for (int r_x = 0; r_x < REGION_WIDTH - 1; r_x++) {
        for (int r_y = 0; r_y < REGION_HEIGHT - 1; r_y++) {
            int wall = world_get_wall_east_west(world, r_x, r_y);

            if (wall > 0 && game_data_wall_object_invisible[wall - 1] == 0) {
                world_create_wall(world, world->parent_model, wall - 1, r_x,
                                  r_y, r_x + 1, r_y);

                if (is_current_plane &&
                    game_data_wall_object_adjacent[wall - 1] != 0) {
                    world->object_adjacency[r_x][r_y] |= 1;

                    if (r_y > 0) {
                        world_set_object_adjacency_from3(world, r_x, r_y - 1,
                                                         4);
                    }
                }

                if (is_current_plane) {
                    surface_draw_line_horizontal_software(world->surface, r_x * 3,
                                                 r_y * 3, 3, colour);
                }
            }

            wall = world_get_wall_north_south(world, r_x, r_y);

            if (wall > 0 && game_data_wall_object_invisible[wall - 1] == 0) {
                world_create_wall(world, world->parent_model, wall - 1, r_x,
                                  r_y, r_x, r_y + 1);

                if (is_current_plane &&
                    game_data_wall_object_adjacent[wall - 1] != 0) {
                    world->object_adjacency[r_x][r_y] |= 2;

                    if (r_x > 0) {
                        world_set_object_adjacency_from3(world, r_x - 1, r_y,
                                                         8);
                    }
                }

                if (is_current_plane) {
                    surface_draw_line_vertical_software(world->surface, r_x * 3, r_y * 3,
                                               3, colour);
                }
            }

            wall = world_get_wall_diagonal(world, r_x, r_y);

            if (wall > 0 && wall < 12000 &&
                game_data_wall_object_invisible[wall - 1] == 0) {
                world_create_wall(world, world->parent_model, wall - 1, r_x,
                                  r_y, r_x + 1, r_y + 1);

                if (is_current_plane &&
                    game_data_wall_object_adjacent[wall - 1] != 0) {
                    world->object_adjacency[r_x][r_y] |= 0x20;
                }

                if (is_current_plane) {
                    surface_set_pixel(world->surface, r_x * 3, r_y * 3, colour);

                    surface_set_pixel(world->surface, r_x * 3 + 1, r_y * 3 + 1,
                                      colour);

                    surface_set_pixel(world->surface, r_x * 3 + 2, r_y * 3 + 2,
                                      colour);
                }
            } else if (wall > 12000 && wall < 24000 &&
                       game_data_wall_object_invisible[wall - 12001] == 0) {
                world_create_wall(world, world->parent_model, wall - 12001,
                                  r_x + 1, r_y, r_x, r_y + 1);

                if (is_current_plane &&
                    game_data_wall_object_adjacent[wall - 12001] != 0) {
                    world->object_adjacency[r_x][r_y] |= 0x10;
                }

                if (is_current_plane) {
                    surface_set_pixel(world->surface, r_x * 3 + 2, r_y * 3,
                                      colour);

                    surface_set_pixel(world->surface, r_x * 3 + 1, r_y * 3 + 1,
                                      colour);

                    surface_set_pixel(world->surface, r_x * 3, r_y * 3 + 2,
                                      colour);
                }
            }
        }
    }

    if (is_current_plane) {
        surface_draw_sprite_reversed(
            world->surface, world->base_media_sprite - 1, 0, 0, 285, 285);
    }

    game_model_set_light_from6(world->parent_model, 0, 60, 24, -50, -10, -50);

    game_model_split(world->parent_model, world->wall_models[plane], 1536, 1536,
                     8, 64, 338, 1);

    for (int i = 0; i < TERRAIN_COUNT; i++) {
        scene_add_model(world->scene, world->wall_models[plane][i]);

#ifdef RENDER_GL
        world->world_models_buffer[world->world_models_offset++] =
            world->wall_models[plane][i];
#endif
    }

    /* create walls */

    for (int r_x = 0; r_x < REGION_WIDTH - 1; r_x++) {
        for (int r_y = 0; r_y < REGION_HEIGHT; r_y++) {
            int wall_object_id = world_get_wall_east_west(world, r_x, r_y);

            if (wall_object_id > 0) {
                world_raise_wall_object(world, wall_object_id - 1, r_x, r_y,
                                        r_x + 1, r_y);
            }

            wall_object_id = world_get_wall_north_south(world, r_x, r_y);

            if (wall_object_id > 0) {
                world_raise_wall_object(world, wall_object_id - 1, r_x, r_y,
                                        r_x, r_y + 1);
            }

            wall_object_id = world_get_wall_diagonal(world, r_x, r_y);

            if (wall_object_id > 0 && wall_object_id < 12000) {
                world_raise_wall_object(world, wall_object_id - 1, r_x, r_y,
                                        r_x + 1, r_y + 1);
            }

            if (wall_object_id > 12000 && wall_object_id < 24000) {
                world_raise_wall_object(world, wall_object_id - 12001, r_x + 1,
                                        r_y, r_x, r_y + 1);
            }
        }
    }

    /* create roofs */

    for (int r_x = 1; r_x < REGION_WIDTH - 1; r_x++) {
        for (int r_y = 1; r_y < REGION_HEIGHT - 1; r_y++) {
            int roof_id = world_get_wall_roof(world, r_x, r_y);

            if (roof_id <= 0) {
                continue;
            }

            int east_x = r_x + 1;
            int south_y = r_y + 1;
            int height = 0;
            int terrain_height = world->terrain_height_local[r_x][r_y];
            int terrain_east_height = world->terrain_height_local[east_x][r_y];

            int terrain_south_east_height =
                world->terrain_height_local[east_x][south_y];

            int terrain_south_height =
                world->terrain_height_local[r_x][south_y];

            if (terrain_height > PLANE_HEIGHT) {
                terrain_height -= PLANE_HEIGHT;
            }

            if (terrain_east_height > PLANE_HEIGHT) {
                terrain_east_height -= PLANE_HEIGHT;
            }

            if (terrain_south_east_height > PLANE_HEIGHT) {
                terrain_south_east_height -= PLANE_HEIGHT;
            }

            if (terrain_south_height > PLANE_HEIGHT) {
                terrain_south_height -= PLANE_HEIGHT;
            }

            if (terrain_height > height) {
                height = terrain_height;
            }

            if (terrain_east_height > height) {
                height = terrain_east_height;
            }

            if (terrain_south_east_height > height) {
                height = terrain_south_east_height;
            }

            if (terrain_south_height > height) {
                height = terrain_south_height;
            }

            if (height >= PLANE_HEIGHT) {
                height -= PLANE_HEIGHT;
            }

            if (terrain_height < PLANE_HEIGHT) {
                world->terrain_height_local[r_x][r_y] = height;
            } else {
                world->terrain_height_local[r_x][r_y] -= PLANE_HEIGHT;
            }

            if (terrain_east_height < PLANE_HEIGHT) {
                world->terrain_height_local[east_x][r_y] = height;
            } else {
                world->terrain_height_local[east_x][r_y] -= PLANE_HEIGHT;
            }

            if (terrain_south_east_height < PLANE_HEIGHT) {
                world->terrain_height_local[east_x][south_y] = height;
            } else {
                world->terrain_height_local[east_x][south_y] -= PLANE_HEIGHT;
            }

            if (terrain_south_height < PLANE_HEIGHT) {
                world->terrain_height_local[r_x][south_y] = height;
            } else {
                world->terrain_height_local[r_x][south_y] -= PLANE_HEIGHT;
            }
        }
    }

    game_model_destroy(world->parent_model);
    game_model_from7(world->parent_model, 18688, 18688, 1, 1, 0, 0, 1);

    for (int r_x = 1; r_x < REGION_WIDTH - 1; r_x++) {
        for (int r_y = 1; r_y < REGION_HEIGHT - 1; r_y++) {
            int roof_id = world_get_wall_roof(world, r_x, r_y);

            if (roof_id <= 0) {
                continue;
            }

            int east_x = r_x + 1;
            int south_y = r_y + 1;

            int vertex_1_x = r_x * TILE_SIZE;
            int vertex_1_z = r_y * TILE_SIZE;
            int vertex_2_x = vertex_1_x + TILE_SIZE;
            int vertex_4_z = vertex_1_z + TILE_SIZE;
            int vertex_3_x = vertex_1_x;
            int vertex_2_z = vertex_1_z;
            int vertex_4_x = vertex_2_x;
            int vertex_3_z = vertex_4_z;

            int terrain_height = world->terrain_height_local[r_x][r_y];

            int terrain_east_height = world->terrain_height_local[east_x][r_y];

            int terrain_south_east_height =
                world->terrain_height_local[east_x][south_y];

            int terrain_south_height =
                world->terrain_height_local[r_x][south_y];

            int roof_height = game_data_roof_height[roof_id - 1];

            if (world_has_roof(world, r_x, r_y) &&
                terrain_height < PLANE_HEIGHT) {
                terrain_height += roof_height + PLANE_HEIGHT;
                world->terrain_height_local[r_x][r_y] = terrain_height;
            }

            if (world_has_roof(world, east_x, r_y) &&
                terrain_east_height < PLANE_HEIGHT) {
                terrain_east_height += roof_height + PLANE_HEIGHT;
                world->terrain_height_local[east_x][r_y] = terrain_east_height;
            }

            if (world_has_roof(world, east_x, south_y) &&
                terrain_south_east_height < PLANE_HEIGHT) {
                terrain_south_east_height += roof_height + PLANE_HEIGHT;
                world->terrain_height_local[east_x][south_y] =
                    terrain_south_east_height;
            }

            if (world_has_roof(world, r_x, south_y) &&
                terrain_south_height < PLANE_HEIGHT) {
                terrain_south_height += roof_height + PLANE_HEIGHT;
                world->terrain_height_local[r_x][south_y] =
                    terrain_south_height;
            }

            if (terrain_height >= PLANE_HEIGHT) {
                terrain_height -= PLANE_HEIGHT;
            }

            if (terrain_east_height >= PLANE_HEIGHT) {
                terrain_east_height -= PLANE_HEIGHT;
            }

            if (terrain_south_east_height >= PLANE_HEIGHT) {
                terrain_south_east_height -= PLANE_HEIGHT;
            }

            if (terrain_south_height >= PLANE_HEIGHT) {
                terrain_south_height -= PLANE_HEIGHT;
            }

            if (!world_has_neighbouring_roof(world, r_x - 1, r_y)) {
                vertex_1_x -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x + 1, r_y)) {
                vertex_1_x += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x, r_y - 1)) {
                vertex_1_z -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x, r_y + 1)) {
                vertex_1_z += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x - 1, r_y)) {
                vertex_2_x -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x + 1, r_y)) {
                vertex_2_x += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x, r_y - 1)) {
                vertex_2_z -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x, r_y + 1)) {
                vertex_2_z += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x - 1, south_y)) {
                vertex_4_x -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x + 1, south_y)) {
                vertex_4_x += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x, south_y - 1)) {
                vertex_4_z -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, east_x, south_y + 1)) {
                vertex_4_z += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x - 1, south_y)) {
                vertex_3_x -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x + 1, south_y)) {
                vertex_3_x += ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x, south_y - 1)) {
                vertex_3_z -= ROOF_SLOPE;
            }

            if (!world_has_neighbouring_roof(world, r_x, south_y + 1)) {
                vertex_3_z += ROOF_SLOPE;
            }

            roof_id = game_data_roof_fills[roof_id - 1];

            terrain_height = -terrain_height;
            terrain_east_height = -terrain_east_height;
            terrain_south_east_height = -terrain_south_east_height;
            terrain_south_height = -terrain_south_height;

            int diagonal = world_get_wall_diagonal(world, r_x, r_y);

            if (diagonal > 12000 && diagonal < 24000 &&
                world_get_wall_roof(world, r_x - 1, r_y - 1) == 0) {
                int *vertices = malloc(3 * sizeof(int));

                vertices[0] =
                    game_model_vertex_at(world->parent_model, vertex_4_x,
                                         terrain_south_east_height, vertex_4_z);

                vertices[1] =
                    game_model_vertex_at(world->parent_model, vertex_3_x,
                                         terrain_south_height, vertex_3_z);

                vertices[2] =
                    game_model_vertex_at(world->parent_model, vertex_2_x,
                                         terrain_east_height, vertex_2_z);

                game_model_create_face(world->parent_model, 3, vertices,
                                       roof_id, COLOUR_TRANSPARENT);
            } else if (diagonal > 12000 && diagonal < 24000 &&
                       world_get_wall_roof(world, r_x + 1, r_y + 1) == 0) {
                int *vertices = malloc(3 * sizeof(int));

                vertices[0] =
                    game_model_vertex_at(world->parent_model, vertex_1_x,
                                         terrain_height, vertex_1_z);

                vertices[1] =
                    game_model_vertex_at(world->parent_model, vertex_2_x,
                                         terrain_east_height, vertex_2_z);

                vertices[2] =
                    game_model_vertex_at(world->parent_model, vertex_3_x,
                                         terrain_south_height, vertex_3_z);

                game_model_create_face(world->parent_model, 3, vertices,
                                       roof_id, COLOUR_TRANSPARENT);
            } else if (world_get_wall_diagonal(world, r_x, r_y) > 0 &&
                       world_get_wall_diagonal(world, r_x, r_y) < 12000 &&
                       world_get_wall_roof(world, r_x + 1, r_y - 1) == 0) {
                int *vertices = malloc(3 * sizeof(int));

                vertices[0] =
                    game_model_vertex_at(world->parent_model, vertex_3_x,
                                         terrain_south_height, vertex_3_z);

                vertices[1] =
                    game_model_vertex_at(world->parent_model, vertex_1_x,
                                         terrain_height, vertex_1_z);

                vertices[2] =
                    game_model_vertex_at(world->parent_model, vertex_4_x,
                                         terrain_south_east_height, vertex_4_z);

                game_model_create_face(world->parent_model, 3, vertices,
                                       roof_id, COLOUR_TRANSPARENT);
            } else if (world_get_wall_diagonal(world, r_x, r_y) > 0 &&
                       world_get_wall_diagonal(world, r_x, r_y) < 12000 &&
                       world_get_wall_roof(world, r_x - 1, r_y + 1) == 0) {
                int *vertices = malloc(3 * sizeof(int));

                vertices[0] =
                    game_model_vertex_at(world->parent_model, vertex_2_x,
                                         terrain_east_height, vertex_2_z);

                vertices[1] =
                    game_model_vertex_at(world->parent_model, vertex_4_x,
                                         terrain_south_east_height, vertex_4_z);

                vertices[2] =
                    game_model_vertex_at(world->parent_model, vertex_1_x,
                                         terrain_height, vertex_1_z);

                game_model_create_face(world->parent_model, 3, vertices,
                                       roof_id, COLOUR_TRANSPARENT);
            } else if (terrain_height == terrain_east_height &&
                       terrain_south_east_height == terrain_south_height) {
                int *vertices = malloc(4 * sizeof(int));

                vertices[0] =
                    game_model_vertex_at(world->parent_model, vertex_1_x,
                                         terrain_height, vertex_1_z);

                vertices[1] =
                    game_model_vertex_at(world->parent_model, vertex_2_x,
                                         terrain_east_height, vertex_2_z);

                vertices[2] =
                    game_model_vertex_at(world->parent_model, vertex_4_x,
                                         terrain_south_east_height, vertex_4_z);

                vertices[3] =
                    game_model_vertex_at(world->parent_model, vertex_3_x,
                                         terrain_south_height, vertex_3_z);

                game_model_create_face(world->parent_model, 4, vertices,
                                       roof_id, COLOUR_TRANSPARENT);
            } else if (terrain_height == terrain_south_height &&
                       terrain_east_height == terrain_south_east_height) {
                int *vertices = malloc(4 * sizeof(int));

                vertices[0] =
                    game_model_vertex_at(world->parent_model, vertex_3_x,
                                         terrain_south_height, vertex_3_z);

                vertices[1] =
                    game_model_vertex_at(world->parent_model, vertex_1_x,
                                         terrain_height, vertex_1_z);

                vertices[2] =
                    game_model_vertex_at(world->parent_model, vertex_2_x,
                                         terrain_east_height, vertex_2_z);

                vertices[3] =
                    game_model_vertex_at(world->parent_model, vertex_4_x,
                                         terrain_south_east_height, vertex_4_z);

                game_model_create_face(world->parent_model, 4, vertices,
                                       roof_id, COLOUR_TRANSPARENT);
            } else {
                /* draw roof corners */

                int direction = 1;

                if (world_get_wall_roof(world, r_x - 1, r_y - 1) > 0) {
                    direction = 0;
                }

                if (world_get_wall_roof(world, r_x + 1, r_y + 1) > 0) {
                    direction = 0;
                }

                if (!direction) {
                    int *triangle_1 = malloc(3 * sizeof(int));

                    triangle_1[0] =
                        game_model_vertex_at(world->parent_model, vertex_2_x,
                                             terrain_east_height, vertex_2_z);

                    triangle_1[1] = game_model_vertex_at(
                        world->parent_model, vertex_4_x,
                        terrain_south_east_height, vertex_4_z);

                    triangle_1[2] =
                        game_model_vertex_at(world->parent_model, vertex_1_x,
                                             terrain_height, vertex_1_z);

                    game_model_create_face(world->parent_model, 3, triangle_1,
                                           roof_id, COLOUR_TRANSPARENT);

                    int *triangle_2 = malloc(3 * sizeof(int));

                    triangle_2[0] =
                        game_model_vertex_at(world->parent_model, vertex_3_x,
                                             terrain_south_height, vertex_3_z);

                    triangle_2[1] =
                        game_model_vertex_at(world->parent_model, vertex_1_x,
                                             terrain_height, vertex_1_z);

                    triangle_2[2] = game_model_vertex_at(
                        world->parent_model, vertex_4_x,
                        terrain_south_east_height, vertex_4_z);

                    game_model_create_face(world->parent_model, 3, triangle_2,
                                           roof_id, COLOUR_TRANSPARENT);
                } else {
                    int *triangle_1 = malloc(3 * sizeof(int));

                    triangle_1[0] =
                        game_model_vertex_at(world->parent_model, vertex_1_x,
                                             terrain_height, vertex_1_z);
                    triangle_1[1] =
                        game_model_vertex_at(world->parent_model, vertex_2_x,
                                             terrain_east_height, vertex_2_z);
                    triangle_1[2] =
                        game_model_vertex_at(world->parent_model, vertex_3_x,
                                             terrain_south_height, vertex_3_z);

                    game_model_create_face(world->parent_model, 3, triangle_1,
                                           roof_id, COLOUR_TRANSPARENT);

                    int *triangle_2 = malloc(3 * sizeof(int));

                    triangle_2[0] = game_model_vertex_at(
                        world->parent_model, vertex_4_x,
                        terrain_south_east_height, vertex_4_z);

                    triangle_2[1] =
                        game_model_vertex_at(world->parent_model, vertex_3_x,
                                             terrain_south_height, vertex_3_z);
                    triangle_2[2] =
                        game_model_vertex_at(world->parent_model, vertex_2_x,
                                             terrain_east_height, vertex_2_z);

                    game_model_create_face(world->parent_model, 3, triangle_2,
                                           roof_id, COLOUR_TRANSPARENT);
                }
            }
        }
    }

    game_model_set_light_from6(world->parent_model, 1, 50, 50, -50, -10, -50);

    game_model_split(world->parent_model, world->roof_models[plane], 1536, 1536,
                     8, 64, 169, 1);

    for (int i = 0; i < TERRAIN_COUNT; i++) {
        scene_add_model(world->scene, world->roof_models[plane][i]);

#ifdef RENDER_GL
        world->world_models_buffer[world->world_models_offset++] =
            world->roof_models[plane][i];
#endif
    }

    for (int r_x = 0; r_x < REGION_WIDTH; r_x++) {
        for (int r_y = 0; r_y < REGION_HEIGHT; r_y++) {
            if (world->terrain_height_local[r_x][r_y] >= PLANE_HEIGHT) {
                world->terrain_height_local[r_x][r_y] -= PLANE_HEIGHT;
            }
        }
    }

    game_model_destroy(world->parent_model);
}

void world_set_object_adjacency_from3(World *world, int i, int j, int k) {
    world->object_adjacency[i][j] |= k;
}

int world_get_tile_type(World *world, int i, int j) {
    int decoration = world_get_tile_decoration(world, i, j);

    if (decoration == 0) {
        return -1;
    }

    int type = game_data_tile_type[decoration - 1];

    return type != 2 ? 0 : 1; // TODO == 2
}

/* adds login screen models from local game cache */
void world_add_models(World *world, GameModel **models) {
    for (int x = 0; x < 94; x++) {
        for (int y = 0; y < 94; y++) {
            int diagonal = world_get_wall_diagonal(world, x, y);

            if (diagonal > 48000 && diagonal < 60000) {
                int object_id = diagonal - 48001;
                int tile_direction = world_get_tile_direction(world, x, y);
                int width = 0;
                int height = 0;

                if (tile_direction == 0 || tile_direction == 4) {
                    width = game_data_object_width[object_id];
                    height = game_data_object_height[object_id];
                } else {
                    height = game_data_object_width[object_id];
                    width = game_data_object_height[object_id];
                }

                world_remove_object2(world, x, y, object_id);

                GameModel *game_model = game_model_copy_from4(
                    models[game_data_object_model_index[object_id]], 0, 1, 0,
                    0);

                int translate_x = ((x + x + width) * TILE_SIZE) / 2;
                int translate_y = ((y + y + height) * TILE_SIZE) / 2;

                game_model_translate(
                    game_model, translate_x,
                    -world_get_elevation(world, translate_x, translate_y),
                    translate_y);

                game_model_orient(game_model, 0,
                                  world_get_tile_direction(world, x, y) * 32,
                                  0);

                scene_add_model(world->scene, game_model);

                game_model_set_light_from5(game_model, 48, 48, -50, -10, -50);

                if (width > 1 || height > 1) {
                    for (int xx = x; xx < x + width; xx++) {
                        for (int yy = y; yy < y + height; yy++) {
                            if ((xx > x || yy > y) &&
                                world_get_wall_diagonal(world, xx, yy) -
                                        48001 ==
                                    object_id) {
                                int object_x = xx;
                                int object_y = yy;
                                int8_t plane = 0;

                                if (object_x >= REGION_SIZE &&
                                    object_y < REGION_SIZE) {
                                    plane = 1;
                                    object_x -= REGION_SIZE;
                                } else if (object_x < REGION_SIZE &&
                                           object_y >= REGION_SIZE) {
                                    plane = 2;
                                    object_y -= REGION_SIZE;
                                } else if (object_x >= REGION_SIZE &&
                                           object_y >= REGION_SIZE) {
                                    plane = 3;
                                    object_x -= REGION_SIZE;
                                    object_y -= REGION_SIZE;
                                }

                                world->walls_diagonal[plane]
                                                     [object_x * REGION_SIZE +
                                                      object_y] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* create wall face */
void world_create_wall(World *world, GameModel *game_model, int wall_object_id,
                       int x1, int y1, int x2, int y2) {
    world_method425(world, x1, y1, 40);
    world_method425(world, x2, y2, 40);

    int height = game_data_wall_object_height[wall_object_id];
    int front = game_data_wall_object_texture_front[wall_object_id];
    int back = game_data_wall_object_texture_back[wall_object_id];
    int vertex_x1 = x1 * TILE_SIZE;
    int vertex_y1 = y1 * TILE_SIZE;
    int vertex_x2 = x2 * TILE_SIZE;
    int vertex_y2 = y2 * TILE_SIZE;

    int *vertices = malloc(4 * sizeof(int));

    vertices[0] = game_model_vertex_at(
        game_model, vertex_x1, -world->terrain_height_local[x1][y1], vertex_y1);

    vertices[1] = game_model_vertex_at(
        game_model, vertex_x1, -world->terrain_height_local[x1][y1] - height,
        vertex_y1);

    vertices[2] = game_model_vertex_at(
        game_model, vertex_x2, -world->terrain_height_local[x2][y2] - height,
        vertex_y2);

    vertices[3] = game_model_vertex_at(
        game_model, vertex_x2, -world->terrain_height_local[x2][y2], vertex_y2);

    int wall_face =
        game_model_create_face(game_model, 4, vertices, front, back);

    if (game_data_wall_object_invisible[wall_object_id] == 5) {
        game_model->face_tag[wall_face] = 30000 + wall_object_id;
    } else {
        game_model->face_tag[wall_face] = 0;
    }
}

int world_get_terrain_height(World *world, int x, int y) {
    return (get_byte_plane_coord(world->terrain_height, x, y) & 0xff) * 3;
}

void world_load_section_from3(World *world, int x, int y, int plane) {
    world_reset(world, 1);

    int section_x = (x + (REGION_SIZE / 2)) / REGION_SIZE;
    int section_y = (y + (REGION_SIZE / 2)) / REGION_SIZE;

#ifdef RENDER_GL
    int max_models = (TERRAIN_COUNT * 3);

    if (plane == 0) {
        max_models *= 3;
    }

    world->world_models_buffer = calloc(max_models, sizeof(GameModel *));
    world->world_models_offset = 0;
#endif

    world_load_section_from4(world, x, y, plane, 1);

    if (plane == 0) {
        world_load_section_from4(world, x, y, 1, 0);
        world_load_section_from4(world, x, y, 2, 0);

        world_load_section_from4i(world, section_x - 1, section_y - 1, plane,
                                  0);

        world_load_section_from4i(world, section_x, section_y - 1, plane, 1);
        world_load_section_from4i(world, section_x - 1, section_y, plane, 2);
        world_load_section_from4i(world, section_x, section_y, plane, 3);

        world_set_tiles(world);
    }

#ifdef RENDER_GL
    game_model_gl_buffer_models(
        &world->scene->terrain_vao, &world->scene->terrain_vbo,
        &world->scene->terrain_ebo, world->world_models_buffer,
        world->world_models_offset);

    free(world->world_models_buffer);
    world->world_models_buffer = NULL;
#endif

    game_model_destroy(world->parent_model);
    free(world->parent_model);
    world->parent_model = NULL;
}

/* TODO add terrain shadow */
void world_method425(World *world, int vertex_x, int vertex_z, int ambience) {
    int terrain_x = vertex_x / 12;
    int terrain_y = vertex_z / 12;
    int j1 = (vertex_x - 1) / 12;
    int k1 = (vertex_z - 1) / 12;

    world_set_terrain_ambience(world, terrain_x, terrain_y, vertex_x, vertex_z,
                               ambience);

    if (terrain_x != j1) {
        world_set_terrain_ambience(world, j1, terrain_y, vertex_x, vertex_z,
                                   ambience);
    }

    if (terrain_y != k1) {
        world_set_terrain_ambience(world, terrain_x, k1, vertex_x, vertex_z,
                                   ambience);
    }

    if (terrain_x != j1 && terrain_y != k1) {
        world_set_terrain_ambience(world, j1, k1, vertex_x, vertex_z, ambience);
    }
}

void world_remove_object(World *world, int x, int y, int id) {
    if (x < 0 || y < 0 || x >= (REGION_WIDTH - 1) || y >= (REGION_HEIGHT - 1)) {
        return;
    }

    if (game_data_object_type[id] == 1 || game_data_object_type[id] == 2) {
        int tile_direction = world_get_tile_direction(world, x, y);
        int width;
        int height;

        if (tile_direction == 0 || tile_direction == 4) {
            width = game_data_object_width[id];
            height = game_data_object_height[id];
        } else {
            height = game_data_object_width[id];
            width = game_data_object_height[id];
        }

        for (int xx = x; xx < x + width; xx++) {
            for (int yy = y; yy < y + height; yy++) {
                if (game_data_object_type[id] == 1) {
                    world->object_adjacency[xx][yy] &= 0xffbf;
                } else if (tile_direction == 0) {
                    world->object_adjacency[xx][yy] &= 0xfffd;

                    if (xx > 0) {
                        world_method407(world, xx - 1, yy, 8);
                    }
                } else if (tile_direction == 2) {
                    world->object_adjacency[xx][yy] &= 0xfffb;

                    if (yy < (REGION_HEIGHT - 1)) {
                        world_method407(world, xx, yy + 1, 1);
                    }
                } else if (tile_direction == 4) {
                    world->object_adjacency[xx][yy] &= 0xfff7;

                    if (xx < (REGION_WIDTH - 1)) {
                        world_method407(world, xx + 1, yy, 2);
                    }
                } else if (tile_direction == 6) {
                    world->object_adjacency[xx][yy] &= 0xfffe;

                    if (yy > 0) {
                        world_method407(world, xx, yy - 1, 4);
                    }
                }
            }
        }

        world_method404(world, x, y, width, height);
    }
}

int world_has_neighbouring_roof(World *world, int x, int y) {
    return (world_get_wall_roof(world, x, y) > 0 ||
            world_get_wall_roof(world, x - 1, y) > 0 ||
            world_get_wall_roof(world, x - 1, y - 1) > 0 ||
            world_get_wall_roof(world, x, y - 1) > 0);
}

/* move wall (objects) to the upper-plane */
void world_raise_wall_object(World *world, int wall_object_id, int x1, int y1,
                             int x2, int y2) {
    int height = game_data_wall_object_height[wall_object_id];

    if (world->terrain_height_local[x1][y1] < PLANE_HEIGHT) {
        world->terrain_height_local[x1][y1] += PLANE_HEIGHT + height;
    }

    if (world->terrain_height_local[x2][y2] < PLANE_HEIGHT) {
        world->terrain_height_local[x2][y2] += PLANE_HEIGHT + height;
    }
}

#ifdef RENDER_GL
/* update the terrain buffers after ambience changes */
void world_gl_buffer_terrain(World *world) {
    for (int i = 0; i < TERRAIN_COUNT; i++) {
        GameModel *game_model = world->terrain_models[i];

        glBindVertexArray(game_model->vao);

        glBindBuffer(GL_ARRAY_BUFFER, world->scene->terrain_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, world->scene->terrain_ebo);

        int vertex_offset = game_model->vbo_offset;
        int ebo_offset = game_model->ebo_offset;

        game_model_gl_buffer_arrays(game_model, &vertex_offset, &ebo_offset);
    }
}
#endif
