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
    world->world_initialised = 1;
    world->player_alive = 0;
    world->base_media_sprite = 750;

    //memset(world->terrain_models, 0, TERRAIN_COUNT * sizeof(GameModel *));

    memset(world->local_x, 0, LOCAL_COUNT * sizeof(int));
    memset(world->local_y, 0, LOCAL_COUNT * sizeof(int));
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

void world_set_terrain_ambience(World *world, int x, int y, int x2, int y2,
                                int ambience) {
    GameModel *game_model = world->terrain_models[x + y * 8];

    for (int i = 0; i < game_model->num_vertices; i++) {
        if (game_model->vertex_x[i] == x2 * AN_INT_585 &&
            game_model->vertex_z[i] == y2 * AN_INT_585) {
            game_model_set_vertex_ambience(game_model, i, ambience);
            return;
        }
    }
}

int world_get_wall_roof(World *world, int x, int y) {
    return get_byte_plane_coord(world->walls_roof, x, y);
}

int world_get_elevation(World *world, int x, int y) {
    int s_x = x >> 7;
    int s_y = y >> 7;
    int a_x = x & 0x7f;
    int a_y = y & 0x7f;

    if (s_x < 0 || s_y < 0 || s_x >= 95 || s_y >= 95) {
        return 0;
    }

    int h = 0;
    int hx = 0;
    int hy = 0;

    if (a_x <= AN_INT_585 - a_y) {
        h = world_get_terrain_height(world, s_x, s_y);
        hx = world_get_terrain_height(world, s_x + 1, s_y) - h;
        hy = world_get_terrain_height(world, s_x, s_y + 1) - h;
    } else {
        h = world_get_terrain_height(world, s_x + 1, s_y + 1);
        hx = world_get_terrain_height(world, s_x, s_y + 1) - h;
        hy = world_get_terrain_height(world, s_x + 1, s_y) - h;
        a_x = AN_INT_585 - a_x;
        a_y = AN_INT_585 - a_y;
    }

    int elevation = h + ((hx * a_x) / AN_INT_585) + ((hy * a_y) / AN_INT_585);

    return elevation;
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

void world_remove_object2(World *world, int x, int y, int id) {
    if (x < 0 || y < 0 || x >= 95 || y >= 95) {
        return;
    }

    if (game_data_object_type[id] == 1 || game_data_object_type[id] == 2) {
        int tile_dir = world_get_tile_direction(world, x, y);
        int model_width;
        int model_height;

        if (tile_dir == 0 || tile_dir == 4) {
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
                } else if (tile_dir == 0) {
                    world->object_adjacency[mx][my] |= 2;

                    if (mx > 0) {
                        world_set_object_adjacency_from3(world, mx - 1, my, 8);
                    }
                } else if (tile_dir == 2) {
                    world->object_adjacency[mx][my] |= 4;

                    if (my < 95) {
                        world_set_object_adjacency_from3(world, mx, my + 1, 1);
                    }
                } else if (tile_dir == 4) {
                    world->object_adjacency[mx][my] |= 8;

                    if (mx < 95) {
                        world_set_object_adjacency_from3(world, mx + 1, my, 2);
                    }
                } else if (tile_dir == 6) {
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

void world_method402(World *world, int i, int j, int k, int l,
                     int texture_id_2) {
    int line_x = i * 3;
    int line_y = j * 3;
    int l1 = scene_method302(world->scene, l);
    int i2 = scene_method302(world->scene, texture_id_2);
    l1 = l1 >> 1 & 0x7f7f7f;
    i2 = i2 >> 1 & 0x7f7f7f;

    if (k == 0) {
        surface_draw_line_horiz(world->surface, line_x, line_y, 3, l1);
        surface_draw_line_horiz(world->surface, line_x, line_y + 1, 2, l1);
        surface_draw_line_horiz(world->surface, line_x, line_y + 2, 1, l1);
        surface_draw_line_horiz(world->surface, line_x + 2, line_y + 1, 1, i2);
        surface_draw_line_horiz(world->surface, line_x + 1, line_y + 2, 2, i2);
        return;
    }

    if (k == 1) {
        surface_draw_line_horiz(world->surface, line_x, line_y, 3, i2);
        surface_draw_line_horiz(world->surface, line_x + 1, line_y + 1, 2, i2);
        surface_draw_line_horiz(world->surface, line_x + 2, line_y + 2, 1, i2);
        surface_draw_line_horiz(world->surface, line_x, line_y + 1, 1, l1);
        surface_draw_line_horiz(world->surface, line_x, line_y + 2, 2, l1);
    }
}

void world_load_section_from4i(World *world, int x, int y, int plane,
                               int chunk) {
    if (world->landscape_pack == NULL) {
        return;
    }

    char map_name[16]; // 2 digits for %d (10), m (1), file ext (4) and null
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
                last_val = world->terrain_height[chunk][tile_x * 48 + tile_y] +
                               last_val &
                           0x7f;

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
                last_val = world->terrain_colour[chunk][tile_x * 48 + tile_y] +
                               last_val &
                           0x7f;

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
        }
    } else {
        for (int tile = 0; tile < TILE_COUNT; tile++) {
            world->terrain_height[chunk][tile] = 0;
            world->terrain_colour[chunk][tile] = 0;
            world->walls_north_south[chunk][tile] = 0;
            world->walls_east_west[chunk][tile] = 0;
            world->walls_diagonal[chunk][tile] = 0;
            world->walls_roof[chunk][tile] = 0;
            world->tile_decoration[chunk][tile] = 0;

            if (plane == 0) {
                world->tile_decoration[chunk][tile] = -6;
            }

            if (plane == 3) {
                world->tile_decoration[chunk][tile] = 8;
            }

            world->tile_direction[chunk][tile] = 0;
        }
    }
}

void world_method404(World *world, int x, int y, int k, int l) {
    if (x < 1 || y < 1 || x + k >= REGION_WIDTH || y + l >= REGION_HEIGHT) {
        return;
    }

    for (int xx = x; xx <= x + k; xx++) {
        for (int yy = y; yy <= y + l; yy++) {
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

void world_reset(World *world) {
    if (world->world_initialised) {
        scene_dispose(world->scene);
    }

    for (int i = 0; i < TERRAIN_COUNT; i++) {
        free(world->terrain_models[i]);

        for (int j = 0; j < PLANE_COUNT; j++) {
            free(world->wall_models[j][i]);
        }

        for (int j = 0; j < PLANE_COUNT; j++) {
            free(world->roof_models[j][i]);
        }
    }
}

void world_set_tiles(World *world) {
    for (int x = 0; x < REGION_WIDTH; x++) {
        for (int y = 0; y < REGION_HEIGHT; y++) {
            if (world_get_tile_decoration(world, x, y) != 250) {
                break;
            }

            if (x == 47 && world_get_tile_decoration(world, x + 1, y) != 250 &&
                world_get_tile_decoration(world, x + 1, y) != 2) {
                world_set_tile_decoration(world, x, y, 9);
            } else if (y == 47 &&
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

int world_get_tile_decoration_from4(World *world, int x, int y, int unused,
                                    int def) {
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
                int end_x2, int end_y2, int *route_x, int *route_y, int objects,
                int size) {
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

    /*int size = route_x.length; TODO remember to pass this in! */
    int reached = 0;

    while (read_ptr != write_ptr) {
        x = route_x[read_ptr];
        y = route_y[read_ptr];
        read_ptr = (read_ptr + 1) % size;

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

            if (x < 95 && x + 1 >= end_x1 && x + 1 <= end_x2 && y >= end_y1 &&
                y <= end_y2 && (world->object_adjacency[x + 1][y] & 2) == 0) {
                reached = 1;
                break;
            }
            if (y > 0 && x >= end_x1 && x <= end_x2 && y - 1 >= end_y1 &&
                y - 1 <= end_y2 &&
                (world->object_adjacency[x][y - 1] & 4) == 0) {
                reached = 1;
                break;
            }
            if (y < 95 && x >= end_x1 && x <= end_x2 && y + 1 >= end_y1 &&
                y + 1 <= end_y2 &&
                (world->object_adjacency[x][y + 1] & 1) == 0) {
                reached = 1;
                break;
            }
        }

        if (x > 0 && world->route_via[x - 1][y] == 0 &&
            (world->object_adjacency[x - 1][y] & 0x78) == 0) {
            route_x[write_ptr] = x - 1;
            route_y[write_ptr] = y;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x - 1][y] = 2;
        }

        if (x < 95 && world->route_via[x + 1][y] == 0 &&
            (world->object_adjacency[x + 1][y] & 0x72) == 0) {
            route_x[write_ptr] = x + 1;
            route_y[write_ptr] = y;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x + 1][y] = 8;
        }

        if (y > 0 && world->route_via[x][y - 1] == 0 &&
            (world->object_adjacency[x][y - 1] & 0x74) == 0) {
            route_x[write_ptr] = x;
            route_y[write_ptr] = y - 1;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x][y - 1] = 1;
        }

        if (y < 95 && world->route_via[x][y + 1] == 0 &&
            (world->object_adjacency[x][y + 1] & 0x71) == 0) {
            route_x[write_ptr] = x;
            route_y[write_ptr] = y + 1;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x][y + 1] = 4;
        }

        if (x > 0 && y > 0 && (world->object_adjacency[x][y - 1] & 0x74) == 0 &&
            (world->object_adjacency[x - 1][y] & 0x78) == 0 &&
            (world->object_adjacency[x - 1][y - 1] & 0x7c) == 0 &&
            world->route_via[x - 1][y - 1] == 0) {
            route_x[write_ptr] = x - 1;
            route_y[write_ptr] = y - 1;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x - 1][y - 1] = 3;
        }

        if (x < 95 && y > 0 &&
            (world->object_adjacency[x][y - 1] & 0x74) == 0 &&
            (world->object_adjacency[x + 1][y] & 0x72) == 0 &&
            (world->object_adjacency[x + 1][y - 1] & 0x76) == 0 &&
            world->route_via[x + 1][y - 1] == 0) {
            route_x[write_ptr] = x + 1;
            route_y[write_ptr] = y - 1;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x + 1][y - 1] = 9;
        }

        if (x > 0 && y < 95 &&
            (world->object_adjacency[x][y + 1] & 0x71) == 0 &&
            (world->object_adjacency[x - 1][y] & 0x78) == 0 &&
            (world->object_adjacency[x - 1][y + 1] & 0x79) == 0 &&
            world->route_via[x - 1][y + 1] == 0) {
            route_x[write_ptr] = x - 1;
            route_y[write_ptr] = y + 1;
            write_ptr = (write_ptr + 1) % size;
            world->route_via[x - 1][y + 1] = 6;
        }

        if (x < 95 && y < 95 &&
            (world->object_adjacency[x][y + 1] & 0x71) == 0 &&
            (world->object_adjacency[x + 1][y] & 0x72) == 0 &&
            (world->object_adjacency[x + 1][y + 1] & 0x73) == 0 &&
            world->route_via[x + 1][y + 1] == 0) {
            route_x[write_ptr] = x + 1;
            route_y[write_ptr] = y + 1;
            write_ptr = (write_ptr + 1) % size;
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

void world_load_section_from4(World *world, int x, int y, int plane, int flag) {
    int l = (x + 24) / 48;
    int i1 = (y + 24) / 48;

    world_load_section_from4i(world, l - 1, i1 - 1, plane, 0);
    world_load_section_from4i(world, l, i1 - 1, plane, 1);
    world_load_section_from4i(world, l - 1, i1, plane, 2);
    world_load_section_from4i(world, l, i1, plane, 3);
    world_set_tiles(world);

    if (world->parent_model == NULL) {
        world->parent_model = malloc(sizeof(GameModel));
        game_model_from7(world->parent_model, 18688, 18688, 1, 1, 0, 0, 1);
    }

    if (flag) {
        surface_black_screen(world->surface);

        for (int j1 = 0; j1 < REGION_WIDTH; j1++) {
            for (int l1 = 0; l1 < REGION_HEIGHT; l1++) {
                world->object_adjacency[j1][l1] = 0;
            }
        }

        GameModel *game_model = world->parent_model;
        game_model_clear(game_model);

        for (int j2 = 0; j2 < REGION_WIDTH; j2++) {
            for (int i3 = 0; i3 < REGION_HEIGHT; i3++) {
                int i4 = -world_get_terrain_height(world, j2, i3);

                if (world_get_tile_decoration(world, j2, i3) > 0 &&
                    game_data_tile_type[world_get_tile_decoration(world, j2,
                                                                  i3) -
                                        1] == 4) {
                    i4 = 0;
                }

                if (world_get_tile_decoration(world, j2 - 1, i3) > 0 &&
                    game_data_tile_type[world_get_tile_decoration(world, j2 - 1,
                                                                  i3) -
                                        1] == 4) {
                    i4 = 0;
                }

                if (world_get_tile_decoration(world, j2, i3 - 1) > 0 &&
                    game_data_tile_type[world_get_tile_decoration(world, j2,
                                                                  i3 - 1) -
                                        1] == 4) {
                    i4 = 0;
                }

                if (world_get_tile_decoration(world, j2 - 1, i3 - 1) > 0 &&
                    game_data_tile_type[world_get_tile_decoration(world, j2 - 1,
                                                                  i3 - 1) -
                                        1] == 4) {
                    i4 = 0;
                }

                int j5 = game_model_vertex_at(game_model, j2 * AN_INT_585, i4,
                                              i3 * AN_INT_585);

                float r = (float)rand() / (float)RAND_MAX;
                int j7 = (int)(r * 10) - 5;
                game_model_set_vertex_ambience(game_model, j5, j7);
            }
        }

        for (int lx = 0; lx < 95; lx++) {
            for (int ly = 0; ly < 95; ly++) {
                int colour_index = world_get_terrain_colour(world, lx, ly);
                int colour = terrain_colours[colour_index];
                int colour_1 = colour;
                int colour_2 = colour;
                int l14 = 0;

                if (plane == 1 || plane == 2) {
                    colour = COLOUR_TRANSPARENT;
                    colour_1 = COLOUR_TRANSPARENT;
                    colour_2 = COLOUR_TRANSPARENT;
                }

                if (world_get_tile_decoration(world, lx, ly) > 0) {
                    int decoration_type =
                        world_get_tile_decoration(world, lx, ly);

                    int decoration_tile_type =
                        game_data_tile_type[decoration_type - 1];

                    int tile_type = world_get_tile_type(world, lx, ly, plane);

                    colour = game_data_tile_decoration[decoration_type - 1];
                    colour_1 = game_data_tile_decoration[decoration_type - 1];

                    if (decoration_tile_type == 4) {
                        colour = 1;
                        colour_1 = 1;

                        if (decoration_type == 12) {
                            colour = 31;
                            colour_1 = 31;
                        }
                    }

                    if (decoration_tile_type == 5) {
                        if (world_get_wall_diagonal(world, lx, ly) > 0 &&
                            world_get_wall_diagonal(world, lx, ly) < 24000) {
                            if (world_get_tile_decoration_from4(
                                    world, lx - 1, ly, plane, colour_2) !=
                                    COLOUR_TRANSPARENT &&
                                world_get_tile_decoration_from4(
                                    world, lx, ly - 1, plane, colour_2) !=
                                    COLOUR_TRANSPARENT) {
                                colour = world_get_tile_decoration_from4(
                                    world, lx - 1, ly, plane, colour_2);
                                l14 = 0;
                            } else if (world_get_tile_decoration_from4(
                                           world, lx + 1, ly, plane,
                                           colour_2) != COLOUR_TRANSPARENT &&
                                       world_get_tile_decoration_from4(
                                           world, lx, ly + 1, plane,
                                           colour_2) != COLOUR_TRANSPARENT) {
                                colour_1 = world_get_tile_decoration_from4(
                                    world, lx + 1, ly, plane, colour_2);

                                l14 = 0;
                            } else if (world_get_tile_decoration_from4(
                                           world, lx + 1, ly, plane,
                                           colour_2) != COLOUR_TRANSPARENT &&
                                       world_get_tile_decoration_from4(
                                           world, lx, ly - 1, plane,
                                           colour_2) != COLOUR_TRANSPARENT) {
                                colour_1 = world_get_tile_decoration_from4(
                                    world, lx + 1, ly, plane, colour_2);

                                l14 = 1;
                            } else if (world_get_tile_decoration_from4(
                                           world, lx - 1, ly, plane,
                                           colour_2) != COLOUR_TRANSPARENT &&
                                       world_get_tile_decoration_from4(
                                           world, lx, ly + 1, plane,
                                           colour_2) != COLOUR_TRANSPARENT) {
                                colour = world_get_tile_decoration_from4(
                                    world, lx - 1, ly, plane, colour_2);

                                l14 = 1;
                            }
                        }
                    } else if (decoration_tile_type != 2 ||
                               (world_get_wall_diagonal(world, lx, ly) > 0 &&
                                world_get_wall_diagonal(world, lx, ly) <
                                    24000)) {
                        if (world_get_tile_type(world, lx - 1, ly, plane) !=
                                tile_type &&
                            world_get_tile_type(world, lx, ly - 1, plane) !=
                                tile_type) {
                            colour = colour_2;
                            l14 = 0;
                        } else if (world_get_tile_type(world, lx + 1, ly,
                                                       plane) != tile_type &&
                                   world_get_tile_type(world, lx, ly + 1,
                                                       plane) != tile_type) {
                            colour_1 = colour_2;
                            l14 = 0;
                        } else if (world_get_tile_type(world, lx + 1, ly,
                                                       plane) != tile_type &&
                                   world_get_tile_type(world, lx, ly - 1,
                                                       plane) != tile_type) {
                            colour_1 = colour_2;
                            l14 = 1;
                        } else if (world_get_tile_type(world, lx - 1, ly,
                                                       plane) != tile_type &&
                                   world_get_tile_type(world, lx, ly + 1,
                                                       plane) != tile_type) {
                            colour = colour_2;
                            l14 = 1;
                        }
                    }

                    if (game_data_tile_adjacent[decoration_type - 1] != 0) {
                        world->object_adjacency[lx][ly] |= 0x40;
                    }

                    if (game_data_tile_type[decoration_type - 1] == 2) {
                        world->object_adjacency[lx][ly] |= 0x80;
                    }
                }

                world_method402(world, lx, ly, l14, colour, colour_1);

                int i17 = world_get_terrain_height(world, lx + 1, ly + 1) -
                          world_get_terrain_height(world, lx + 1, ly) +
                          world_get_terrain_height(world, lx, ly + 1) -
                          world_get_terrain_height(world, lx, ly);

                if (colour != colour_1 || i17 != 0) {
                    int *ai = malloc(3 * sizeof(int));
                    int *ai7 = malloc(3 * sizeof(int));

                    if (l14 == 0) {
                        if (colour != COLOUR_TRANSPARENT) {
                            ai[0] = ly + lx * 96 + 96;
                            ai[1] = ly + lx * 96;
                            ai[2] = ly + lx * 96 + 1;

                            int l21 = game_model_create_face(
                                game_model, 3, ai, COLOUR_TRANSPARENT, colour);

                            world->local_x[l21] = lx;
                            world->local_y[l21] = ly;

                            game_model->face_tag[l21] = 0x30d40 + l21;
                        }

                        if (colour_1 != COLOUR_TRANSPARENT) {
                            ai7[0] = ly + lx * 96 + 1;
                            ai7[1] = ly + lx * 96 + 96 + 1;
                            ai7[2] = ly + lx * 96 + 96;

                            int i22 = game_model_create_face(game_model, 3, ai7,
                                                             COLOUR_TRANSPARENT,
                                                             colour_1);

                            world->local_x[i22] = lx;
                            world->local_y[i22] = ly;

                            game_model->face_tag[i22] = 0x30d40 + i22;
                        }
                    } else {
                        if (colour != COLOUR_TRANSPARENT) {
                            ai[0] = ly + lx * 96 + 1;
                            ai[1] = ly + lx * 96 + 96 + 1;
                            ai[2] = ly + lx * 96;

                            int j22 = game_model_create_face(
                                game_model, 3, ai, COLOUR_TRANSPARENT, colour);

                            world->local_x[j22] = lx;
                            world->local_y[j22] = ly;

                            game_model->face_tag[j22] = 0x30d40 + j22;
                        }

                        if (colour_1 != COLOUR_TRANSPARENT) {
                            ai7[0] = ly + lx * 96 + 96;
                            ai7[1] = ly + lx * 96;
                            ai7[2] = ly + lx * 96 + 96 + 1;

                            int k22 = game_model_create_face(game_model, 3, ai7,
                                                             COLOUR_TRANSPARENT,
                                                             colour_1);

                            world->local_x[k22] = lx;
                            world->local_y[k22] = ly;

                            game_model->face_tag[k22] = 0x30d40 + k22;
                        }
                    }
                } else if (colour != COLOUR_TRANSPARENT) {
                    int *vertices = malloc(4 * sizeof(int));
                    vertices[0] = ly + lx * 96 + 96;
                    vertices[1] = ly + lx * 96;
                    vertices[2] = ly + lx * 96 + 1;
                    vertices[3] = ly + lx * 96 + 96 + 1;

                    int l19 = game_model_create_face(
                        game_model, 4, vertices, COLOUR_TRANSPARENT, colour);

                    world->local_x[l19] = lx;
                    world->local_y[l19] = ly;

                    game_model->face_tag[l19] = 0x30d40 + l19;
                }
            }
        }

        for (int k4 = 1; k4 < 95; k4++) {
            for (int i6 = 1; i6 < 95; i6++) {
                if (world_get_tile_decoration(world, k4, i6) > 0 &&
                    game_data_tile_type[world_get_tile_decoration(world, k4,
                                                                  i6) -
                                        1] == 4) {
                    int l7 = game_data_tile_decoration
                        [world_get_tile_decoration(world, k4, i6) - 1];

                    int j10 = game_model_vertex_at(
                        game_model, k4 * AN_INT_585,
                        -world_get_terrain_height(world, k4, i6),
                        i6 * AN_INT_585);

                    int l12 = game_model_vertex_at(
                        game_model, (k4 + 1) * AN_INT_585,
                        -world_get_terrain_height(world, k4 + 1, i6),
                        i6 * AN_INT_585);

                    int i15 = game_model_vertex_at(
                        game_model, (k4 + 1) * AN_INT_585,
                        -world_get_terrain_height(world, k4 + 1, i6 + 1),
                        (i6 + 1) * AN_INT_585);

                    int j17 = game_model_vertex_at(
                        game_model, k4 * AN_INT_585,
                        -world_get_terrain_height(world, k4, i6 + 1),
                        (i6 + 1) * AN_INT_585);

                    int *vertices = malloc(4 * sizeof(int));
                    vertices[0] = j10;
                    vertices[1] = l12;
                    vertices[2] = i15;
                    vertices[3] = j17;

                    int i20 = game_model_create_face(game_model, 4, vertices,
                                                     l7, COLOUR_TRANSPARENT);

                    world->local_x[i20] = k4;
                    world->local_y[i20] = i6;
                    game_model->face_tag[i20] = 0x30d40 + i20;

                    world_method402(world, k4, i6, 0, l7, l7);
                } else if (world_get_tile_decoration(world, k4, i6) == 0 ||
                           game_data_tile_type[world_get_tile_decoration(
                                                   world, k4, i6) -
                                               1] != 3) {
                    if (world_get_tile_decoration(world, k4, i6 + 1) > 0 &&
                        game_data_tile_type[world_get_tile_decoration(world, k4,
                                                                      i6 + 1) -
                                            1] == 4) {
                        int i8 = game_data_tile_decoration
                            [world_get_tile_decoration(world, k4, i6 + 1) - 1];

                        int k10 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6),
                            i6 * AN_INT_585);

                        int i13 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6),
                            i6 * AN_INT_585);

                        int j15 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int k17 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int *vertices = malloc(4 * sizeof(int));
                        vertices[0] = k10;
                        vertices[1] = i13;
                        vertices[2] = j15;
                        vertices[3] = k17;

                        int j20 = game_model_create_face(
                            game_model, 4, vertices, i8, COLOUR_TRANSPARENT);

                        world->local_x[j20] = k4;
                        world->local_y[j20] = i6;
                        game_model->face_tag[j20] = 0x30d40 + j20;

                        world_method402(world, k4, i6, 0, i8, i8);
                    }

                    if (world_get_tile_decoration(world, k4, i6 - 1) > 0 &&
                        game_data_tile_type[world_get_tile_decoration(world, k4,
                                                                      i6 - 1) -
                                            1] == 4) {
                        int j8 = game_data_tile_decoration
                            [world_get_tile_decoration(world, k4, i6 - 1) - 1];

                        int l10 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6),
                            i6 * AN_INT_585);

                        int j13 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6),
                            i6 * AN_INT_585);

                        int k15 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int l17 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int *vertices = malloc(4 * sizeof(int));
                        vertices[0] = l10;
                        vertices[1] = j13;
                        vertices[2] = k15;
                        vertices[3] = l17;

                        int k20 = game_model_create_face(
                            game_model, 4, vertices, j8, COLOUR_TRANSPARENT);

                        world->local_x[k20] = k4;
                        world->local_y[k20] = i6;
                        game_model->face_tag[k20] = 0x30d40 + k20;

                        world_method402(world, k4, i6, 0, j8, j8);
                    }

                    if (world_get_tile_decoration(world, k4 + 1, i6) > 0 &&
                        game_data_tile_type[world_get_tile_decoration(
                                                world, k4 + 1, i6) -
                                            1] == 4) {
                        int k8 = game_data_tile_decoration
                            [world_get_tile_decoration(world, k4 + 1, i6) - 1];

                        int i11 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6),
                            i6 * AN_INT_585);

                        int k13 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6),
                            i6 * AN_INT_585);

                        int l15 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int i18 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int *vertices = malloc(4 * sizeof(int));
                        vertices[0] = i11;
                        vertices[1] = k13;
                        vertices[2] = l15;
                        vertices[3] = i18;

                        int l20 = game_model_create_face(
                            game_model, 4, vertices, k8, COLOUR_TRANSPARENT);

                        world->local_x[l20] = k4;
                        world->local_y[l20] = i6;

                        game_model->face_tag[l20] = 0x30d40 + l20;

                        world_method402(world, k4, i6, 0, k8, k8);
                    }

                    if (world_get_tile_decoration(world, k4 - 1, i6) > 0 &&
                        game_data_tile_type[world_get_tile_decoration(
                                                world, k4 - 1, i6) -
                                            1] == 4) {
                        int l8 = game_data_tile_decoration
                            [world_get_tile_decoration(world, k4 - 1, i6) - 1];

                        int j11 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6),
                            i6 * AN_INT_585);

                        int l13 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6),
                            i6 * AN_INT_585);

                        int i16 = game_model_vertex_at(
                            game_model, (k4 + 1) * AN_INT_585,
                            -world_get_terrain_height(world, k4 + 1, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int j18 = game_model_vertex_at(
                            game_model, k4 * AN_INT_585,
                            -world_get_terrain_height(world, k4, i6 + 1),
                            (i6 + 1) * AN_INT_585);

                        int *vertices = malloc(4 * sizeof(int));
                        vertices[0] = j11;
                        vertices[1] = l13;
                        vertices[2] = i16;
                        vertices[3] = j18;

                        int i21 = game_model_create_face(
                            game_model, 4, vertices, l8, COLOUR_TRANSPARENT);

                        world->local_x[i21] = k4;
                        world->local_y[i21] = i6;
                        game_model->face_tag[i21] = 0x30d40 + i21;
                        world_method402(world, k4, i6, 0, l8, l8);
                    }
                }
            }
        }

        game_model_set_light_from6(game_model, 1, 40, 48, -50, -10, -50);

        // world->terrain_models = calloc(64, sizeof(GameModel *));
        // memset(world->terrain_models, 0, 64 * sizeof(GameModel *));
        game_model_split(world->parent_model, world->terrain_models, 0, 0, 1536,
                         1536, 8, 64, 233, 0);

        for (int j6 = 0; j6 < TERRAIN_COUNT; j6++) {
            scene_add_model(world->scene, world->terrain_models[j6]);
        }

        for (int X = 0; X < REGION_WIDTH; X++) {
            for (int Y = 0; Y < REGION_HEIGHT; Y++) {
                world->terrain_height_local[X][Y] =
                    world_get_terrain_height(world, X, Y);
            }
        }
    }

    game_model_clear(world->parent_model);

    int k1 = 0x606060;

    for (int i2 = 0; i2 < 95; i2++) {
        for (int k2 = 0; k2 < 95; k2++) {
            int k3 = world_get_wall_east_west(world, i2, k2);

            if (k3 > 0 && game_data_wall_object_invisible[k3 - 1] == 0) {
                world_method422(world, world->parent_model, k3 - 1, i2, k2,
                                i2 + 1, k2);

                if (flag && game_data_wall_object_adjacent[k3 - 1] != 0) {
                    world->object_adjacency[i2][k2] |= 1;

                    if (k2 > 0) {
                        world_set_object_adjacency_from3(world, i2, k2 - 1, 4);
                    }
                }

                if (flag) {
                    surface_draw_line_horiz(world->surface, i2 * 3, k2 * 3, 3,
                                            k1);
                }
            }

            k3 = world_get_wall_north_south(world, i2, k2);

            if (k3 > 0 && game_data_wall_object_invisible[k3 - 1] == 0) {
                world_method422(world, world->parent_model, k3 - 1, i2, k2, i2,
                                k2 + 1);

                if (flag && game_data_wall_object_adjacent[k3 - 1] != 0) {
                    world->object_adjacency[i2][k2] |= 2;

                    if (i2 > 0) {
                        world_set_object_adjacency_from3(world, i2 - 1, k2, 8);
                    }
                }

                if (flag) {
                    surface_draw_line_vert(world->surface, i2 * 3, k2 * 3, 3,
                                           k1);
                }
            }

            k3 = world_get_wall_diagonal(world, i2, k2);

            if (k3 > 0 && k3 < 12000 &&
                game_data_wall_object_invisible[k3 - 1] == 0) {
                world_method422(world, world->parent_model, k3 - 1, i2, k2,
                                i2 + 1, k2 + 1);

                if (flag && game_data_wall_object_adjacent[k3 - 1] != 0) {
                    world->object_adjacency[i2][k2] |= 0x20;
                }

                if (flag) {
                    surface_set_pixel(world->surface, i2 * 3, k2 * 3, k1);
                    surface_set_pixel(world->surface, i2 * 3 + 1, k2 * 3 + 1,
                                      k1);
                    surface_set_pixel(world->surface, i2 * 3 + 2, k2 * 3 + 2,
                                      k1);
                }
            }

            if (k3 > 12000 && k3 < 24000 &&
                game_data_wall_object_invisible[k3 - 12001] == 0) {
                world_method422(world, world->parent_model, k3 - 12001, i2 + 1,
                                k2, i2, k2 + 1);

                if (flag && game_data_wall_object_adjacent[k3 - 12001] != 0) {
                    world->object_adjacency[i2][k2] |= 0x10;
                }

                if (flag) {
                    surface_set_pixel(world->surface, i2 * 3 + 2, k2 * 3, k1);
                    surface_set_pixel(world->surface, i2 * 3 + 1, k2 * 3 + 1,
                                      k1);
                    surface_set_pixel(world->surface, i2 * 3, k2 * 3 + 2, k1);
                }
            }
        }
    }

    if (flag) {
        surface_draw_sprite_from5(world->surface, world->base_media_sprite - 1,
                                  0, 0, 285, 285);
    }

    game_model_set_light_from6(world->parent_model, 0, 60, 24, -50, -10, -50);

    game_model_split(world->parent_model, world->wall_models[plane], 0, 0, 1536,
                     1536, 8, 64, 338, 1);

    for (int l2 = 0; l2 < TERRAIN_COUNT; l2++) {
        scene_add_model(world->scene, world->wall_models[plane][l2]);
    }

    for (int l3 = 0; l3 < 95; l3++) {
        for (int l4 = 0; l4 < 95; l4++) {
            int k6 = world_get_wall_east_west(world, l3, l4);

            if (k6 > 0) {
                world_method428(world, k6 - 1, l3, l4, l3 + 1, l4);
            }

            k6 = world_get_wall_north_south(world, l3, l4);

            if (k6 > 0) {
                world_method428(world, k6 - 1, l3, l4, l3, l4 + 1);
            }

            k6 = world_get_wall_diagonal(world, l3, l4);

            if (k6 > 0 && k6 < 12000) {
                world_method428(world, k6 - 1, l3, l4, l3 + 1, l4 + 1);
            }

            if (k6 > 12000 && k6 < 24000) {
                world_method428(world, k6 - 12001, l3 + 1, l4, l3, l4 + 1);
            }
        }
    }

    for (int i5 = 1; i5 < 95; i5++) {
        for (int l6 = 1; l6 < 95; l6++) {
            int j9 = world_get_wall_roof(world, i5, l6);

            if (j9 > 0) {
                int l11 = i5;
                int i14 = l6;
                int j16 = i5 + 1;
                int k18 = l6;
                int j19 = i5 + 1;
                int j21 = l6 + 1;
                int l22 = i5;
                int j23 = l6 + 1;
                int l23 = 0;
                int j24 = world->terrain_height_local[l11][i14];
                int l24 = world->terrain_height_local[j16][k18];
                int j25 = world->terrain_height_local[j19][j21];
                int l25 = world->terrain_height_local[l22][j23];

                if (j24 > 0x13880) {
                    j24 -= 0x13880;
                }

                if (l24 > 0x13880) {
                    l24 -= 0x13880;
                }

                if (j25 > 0x13880) {
                    j25 -= 0x13880;
                }

                if (l25 > 0x13880) {
                    l25 -= 0x13880;
                }

                if (j24 > l23) {
                    l23 = j24;
                }

                if (l24 > l23) {
                    l23 = l24;
                }

                if (j25 > l23) {
                    l23 = j25;
                }

                if (l25 > l23) {
                    l23 = l25;
                }

                if (l23 >= 0x13880) {
                    l23 -= 0x13880;
                }

                if (j24 < 0x13880) {
                    world->terrain_height_local[l11][i14] = l23;
                } else {
                    world->terrain_height_local[l11][i14] -= 0x13880;
                }

                if (l24 < 0x13880) {
                    world->terrain_height_local[j16][k18] = l23;
                } else {
                    world->terrain_height_local[j16][k18] -= 0x13880;
                }

                if (j25 < 0x13880) {
                    world->terrain_height_local[j19][j21] = l23;
                } else {
                    world->terrain_height_local[j19][j21] -= 0x13880;
                }

                if (l25 < 0x13880) {
                    world->terrain_height_local[l22][j23] = l23;
                } else {
                    world->terrain_height_local[l22][j23] -= 0x13880;
                }
            }
        }
    }

    game_model_clear(world->parent_model);

    for (int i7 = 1; i7 < 95; i7++) {
        for (int k9 = 1; k9 < 95; k9++) {
            int roof_nvs = world_get_wall_roof(world, i7, k9);

            if (roof_nvs > 0) {
                int j14 = i7;
                int k16 = k9;
                int l18 = i7 + 1;
                int k19 = k9;
                int k21 = i7 + 1;
                int i23 = k9 + 1;
                int k23 = i7;
                int i24 = k9 + 1;
                int k24 = i7 * AN_INT_585;
                int i25 = k9 * AN_INT_585;
                int k25 = k24 + AN_INT_585;
                int i26 = i25 + AN_INT_585;
                int j26 = k24;
                int k26 = i25;
                int l26 = k25;
                int i27 = i26;
                int j27 = world->terrain_height_local[j14][k16];
                int k27 = world->terrain_height_local[l18][k19];
                int l27 = world->terrain_height_local[k21][i23];
                int i28 = world->terrain_height_local[k23][i24];
                int unknown = game_data_roof_height[roof_nvs - 1];

                if (world_has_roof(world, j14, k16) && j27 < 0x13880) {
                    j27 += unknown + 0x13880;
                    world->terrain_height_local[j14][k16] = j27;
                }

                if (world_has_roof(world, l18, k19) && k27 < 0x13880) {
                    k27 += unknown + 0x13880;
                    world->terrain_height_local[l18][k19] = k27;
                }

                if (world_has_roof(world, k21, i23) && l27 < 0x13880) {
                    l27 += unknown + 0x13880;
                    world->terrain_height_local[k21][i23] = l27;
                }

                if (world_has_roof(world, k23, i24) && i28 < 0x13880) {
                    i28 += unknown + 0x13880;
                    world->terrain_height_local[k23][i24] = i28;
                }

                if (j27 >= 0x13880) {
                    j27 -= 0x13880;
                }

                if (k27 >= 0x13880) {
                    k27 -= 0x13880;
                }

                if (l27 >= 0x13880) {
                    l27 -= 0x13880;
                }

                if (i28 >= 0x13880) {
                    i28 -= 0x13880;
                }

                int8_t byte0 = 16;

                if (!world_method427(world, j14 - 1, k16)) {
                    k24 -= byte0;
                }

                if (!world_method427(world, j14 + 1, k16)) {
                    k24 += byte0;
                }

                if (!world_method427(world, j14, k16 - 1)) {
                    i25 -= byte0;
                }

                if (!world_method427(world, j14, k16 + 1)) {
                    i25 += byte0;
                }

                if (!world_method427(world, l18 - 1, k19)) {
                    k25 -= byte0;
                }

                if (!world_method427(world, l18 + 1, k19)) {
                    k25 += byte0;
                }

                if (!world_method427(world, l18, k19 - 1)) {
                    k26 -= byte0;
                }

                if (!world_method427(world, l18, k19 + 1)) {
                    k26 += byte0;
                }

                if (!world_method427(world, k21 - 1, i23)) {
                    l26 -= byte0;
                }

                if (!world_method427(world, k21 + 1, i23)) {
                    l26 += byte0;
                }

                if (!world_method427(world, k21, i23 - 1)) {
                    i26 -= byte0;
                }

                if (!world_method427(world, k21, i23 + 1)) {
                    i26 += byte0;
                }

                if (!world_method427(world, k23 - 1, i24)) {
                    j26 -= byte0;
                }

                if (!world_method427(world, k23 + 1, i24)) {
                    j26 += byte0;
                }

                if (!world_method427(world, k23, i24 - 1)) {
                    i27 -= byte0;
                }

                if (!world_method427(world, k23, i24 + 1)) {
                    i27 += byte0;
                }

                roof_nvs = game_data_roof_num_vertices[roof_nvs - 1];

                j27 = -j27;
                k27 = -k27;
                l27 = -l27;
                i28 = -i28;

                if (world_get_wall_diagonal(world, i7, k9) > 12000 &&
                    world_get_wall_diagonal(world, i7, k9) < 24000 &&
                    world_get_wall_roof(world, i7 - 1, k9 - 1) == 0) {
                    int *ai8 = malloc(3 * sizeof(int));
                    ai8[0] = game_model_vertex_at(world->parent_model, l26, l27,
                                                  i26);
                    ai8[1] = game_model_vertex_at(world->parent_model, j26, i28,
                                                  i27);
                    ai8[2] = game_model_vertex_at(world->parent_model, k25, k27,
                                                  k26);
                    game_model_create_face(world->parent_model, 3, ai8,
                                           roof_nvs, COLOUR_TRANSPARENT);
                } else if (world_get_wall_diagonal(world, i7, k9) > 12000 &&
                           world_get_wall_diagonal(world, i7, k9) < 24000 &&
                           world_get_wall_roof(world, i7 + 1, k9 + 1) == 0) {
                    int *ai9 = malloc(3 * sizeof(int));
                    ai9[0] = game_model_vertex_at(world->parent_model, k24, j27,
                                                  i25);
                    ai9[1] = game_model_vertex_at(world->parent_model, k25, k27,
                                                  k26);
                    ai9[2] = game_model_vertex_at(world->parent_model, j26, i28,
                                                  i27);
                    game_model_create_face(world->parent_model, 3, ai9,
                                           roof_nvs, COLOUR_TRANSPARENT);
                } else if (world_get_wall_diagonal(world, i7, k9) > 0 &&
                           world_get_wall_diagonal(world, i7, k9) < 12000 &&
                           world_get_wall_roof(world, i7 + 1, k9 - 1) == 0) {
                    int *ai10 = malloc(3 * sizeof(int));
                    ai10[0] = game_model_vertex_at(world->parent_model, j26,
                                                   i28, i27);
                    ai10[1] = game_model_vertex_at(world->parent_model, k24,
                                                   j27, i25);
                    ai10[2] = game_model_vertex_at(world->parent_model, l26,
                                                   l27, i26);
                    game_model_create_face(world->parent_model, 3, ai10,
                                           roof_nvs, COLOUR_TRANSPARENT);
                } else if (world_get_wall_diagonal(world, i7, k9) > 0 &&
                           world_get_wall_diagonal(world, i7, k9) < 12000 &&
                           world_get_wall_roof(world, i7 - 1, k9 + 1) == 0) {
                    int *ai11 = malloc(3 * sizeof(int));
                    ai11[0] = game_model_vertex_at(world->parent_model, k25,
                                                   k27, k26);
                    ai11[1] = game_model_vertex_at(world->parent_model, l26,
                                                   l27, i26);
                    ai11[2] = game_model_vertex_at(world->parent_model, k24,
                                                   j27, i25);
                    game_model_create_face(world->parent_model, 3, ai11,
                                           roof_nvs, COLOUR_TRANSPARENT);
                } else if (j27 == k27 && l27 == i28) {
                    int *ai12 = malloc(4 * sizeof(int));
                    ai12[0] = game_model_vertex_at(world->parent_model, k24,
                                                   j27, i25);
                    ai12[1] = game_model_vertex_at(world->parent_model, k25,
                                                   k27, k26);
                    ai12[2] = game_model_vertex_at(world->parent_model, l26,
                                                   l27, i26);
                    ai12[3] = game_model_vertex_at(world->parent_model, j26,
                                                   i28, i27);
                    game_model_create_face(world->parent_model, 4, ai12,
                                           roof_nvs, COLOUR_TRANSPARENT);
                } else if (j27 == i28 && k27 == l27) {
                    int *ai13 = malloc(4 * sizeof(int));
                    ai13[0] = game_model_vertex_at(world->parent_model, j26,
                                                   i28, i27);
                    ai13[1] = game_model_vertex_at(world->parent_model, k24,
                                                   j27, i25);
                    ai13[2] = game_model_vertex_at(world->parent_model, k25,
                                                   k27, k26);
                    ai13[3] = game_model_vertex_at(world->parent_model, l26,
                                                   l27, i26);
                    game_model_create_face(world->parent_model, 4, ai13,
                                           roof_nvs, COLOUR_TRANSPARENT);
                } else {
                    int flag1 = 1;

                    if (world_get_wall_roof(world, i7 - 1, k9 - 1) > 0) {
                        flag1 = 0;
                    }

                    if (world_get_wall_roof(world, i7 + 1, k9 + 1) > 0) {
                        flag1 = 0;
                    }

                    if (!flag1) {
                        int *ai14 = malloc(3 * sizeof(int));
                        ai14[0] = game_model_vertex_at(world->parent_model, k25,
                                                       k27, k26);
                        ai14[1] = game_model_vertex_at(world->parent_model, l26,
                                                       l27, i26);
                        ai14[2] = game_model_vertex_at(world->parent_model, k24,
                                                       j27, i25);
                        game_model_create_face(world->parent_model, 3, ai14,
                                               roof_nvs, COLOUR_TRANSPARENT);
                        int *ai16 = malloc(3 * sizeof(int));
                        ai16[0] = game_model_vertex_at(world->parent_model, j26,
                                                       i28, i27);
                        ai16[1] = game_model_vertex_at(world->parent_model, k24,
                                                       j27, i25);
                        ai16[2] = game_model_vertex_at(world->parent_model, l26,
                                                       l27, i26);
                        game_model_create_face(world->parent_model, 3, ai16,
                                               roof_nvs, COLOUR_TRANSPARENT);
                    } else {
                        int *ai15 = malloc(3 * sizeof(int));
                        ai15[0] = game_model_vertex_at(world->parent_model, k24,
                                                       j27, i25);
                        ai15[1] = game_model_vertex_at(world->parent_model, k25,
                                                       k27, k26);
                        ai15[2] = game_model_vertex_at(world->parent_model, j26,
                                                       i28, i27);
                        game_model_create_face(world->parent_model, 3, ai15,
                                               roof_nvs, COLOUR_TRANSPARENT);
                        int *ai17 = malloc(3 * sizeof(int));
                        ai17[0] = game_model_vertex_at(world->parent_model, l26,
                                                       l27, i26);
                        ai17[1] = game_model_vertex_at(world->parent_model, j26,
                                                       i28, i27);
                        ai17[2] = game_model_vertex_at(world->parent_model, k25,
                                                       k27, k26);
                        game_model_create_face(world->parent_model, 3, ai17,
                                               roof_nvs, COLOUR_TRANSPARENT);
                    }
                }
            }
        }
    }

    game_model_set_light_from6(world->parent_model, 1, 50, 50, -50, -10, -50);

    game_model_split(world->parent_model, world->roof_models[plane], 0, 0, 1536,
                     1536, 8, 64, 169, 1);

    for (int l9 = 0; l9 < TERRAIN_COUNT; l9++) {
        scene_add_model(world->scene, world->roof_models[plane][l9]);
    }

    for (int j12 = 0; j12 < REGION_WIDTH; j12++) {
        for (int k14 = 0; k14 < REGION_HEIGHT; k14++) {
            if (world->terrain_height_local[j12][k14] >= 0x13880) {
                world->terrain_height_local[j12][k14] -= 0x13880;
            }
        }
    }
}

void world_set_object_adjacency_from3(World *world, int i, int j, int k) {
    world->object_adjacency[i][j] |= k;
}

int world_get_tile_type(World *world, int i, int j, int k) {
    int l = world_get_tile_decoration(world, i, j);

    if (l == 0) {
        return -1;
    }

    int i1 = game_data_tile_type[l - 1];

    return i1 != 2 ? 0 : 1;
}

void world_add_models(World *world, GameModel **models) {
    for (int i = 0; i < 94; i++) {
        for (int j = 0; j < 94; j++) {
            if (world_get_wall_diagonal(world, i, j) > 48000 &&
                world_get_wall_diagonal(world, i, j) < 60000) {
                int k = world_get_wall_diagonal(world, i, j) - 48001;
                int l = world_get_tile_direction(world, i, j);
                int i1 = 0;
                int j1 = 0;

                if (l == 0 || l == 4) {
                    i1 = game_data_object_width[k];
                    j1 = game_data_object_height[k];
                } else {
                    j1 = game_data_object_width[k];
                    i1 = game_data_object_height[k];
                }

                world_remove_object2(world, i, j, k);

                GameModel *game_model = game_model_copy_from4(
                    models[game_data_object_model_index[k]], 0, 1, 0, 0);

                int k1 = ((i + i + i1) * AN_INT_585) / 2;
                int i2 = ((j + j + j1) * AN_INT_585) / 2;

                game_model_translate(game_model, k1,
                                     -world_get_elevation(world, k1, i2), i2);

                game_model_orient(game_model, 0,
                                  world_get_tile_direction(world, i, j) * 32,
                                  0);

                scene_add_model(world->scene, game_model);
                game_model_set_light_from5(game_model, 48, 48, -50, -10, -50);

                if (i1 > 1 || j1 > 1) {
                    for (int k2 = i; k2 < i + i1; k2++) {
                        for (int l2 = j; l2 < j + j1; l2++) {
                            if ((k2 > i || l2 > j) &&
                                world_get_wall_diagonal(world, k2, l2) -
                                        48001 ==
                                    k) {
                                int l1 = k2;
                                int j2 = l2;
                                int8_t height = 0;

                                if (l1 >= 48 && j2 < 48) {
                                    height = 1;
                                    l1 -= 48;
                                } else if (l1 < 48 && j2 >= 48) {
                                    height = 2;
                                    j2 -= 48;
                                } else if (l1 >= 48 && j2 >= 48) {
                                    height = 3;
                                    l1 -= 48;
                                    j2 -= 48;
                                }

                                world->walls_diagonal[height][l1 * 48 + j2] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

void world_method422(World *world, GameModel *game_model, int i, int j, int k,
                     int l, int i1) {
    world_method425(world, j, k, 40);
    world_method425(world, l, i1, 40);

    int h = game_data_wall_object_height[i];
    int front = game_data_wall_object_texture_front[i];
    int back = game_data_wall_object_texture_back[i];
    int i2 = j * AN_INT_585;
    int j2 = k * AN_INT_585;
    int k2 = l * AN_INT_585;
    int l2 = i1 * AN_INT_585;

    int i3 = game_model_vertex_at(game_model, i2,
                                  -world->terrain_height_local[j][k], j2);

    int j3 = game_model_vertex_at(game_model, i2,
                                  -world->terrain_height_local[j][k] - h, j2);

    int k3 = game_model_vertex_at(game_model, k2,
                                  -world->terrain_height_local[l][i1] - h, l2);

    int l3 = game_model_vertex_at(game_model, k2,
                                  -world->terrain_height_local[l][i1], l2);

    int *vertices = malloc(4 * sizeof(int));
    vertices[0] = i3;
    vertices[1] = j3;
    vertices[2] = k3;
    vertices[3] = l3;

    int i4 = game_model_create_face(game_model, 4, vertices, front, back);

    if (game_data_wall_object_invisible[i] == 5) {
        game_model->face_tag[i4] = 30000 + i;
        return;
    } else {
        game_model->face_tag[i4] = 0;
        return;
    }
}

int world_get_terrain_height(World *world, int x, int y) {
    return (get_byte_plane_coord(world->terrain_height, x, y) & 0xff) * 3;
}

void world_load_section_from3(World *world, int x, int y, int plane) {
    world_reset(world);

    int l = (x + 24) / 48;
    int i1 = (y + 24) / 48;

    world_load_section_from4(world, x, y, plane, 1);

    if (plane == 0) {
        world_load_section_from4(world, x, y, 1, 0);
        world_load_section_from4(world, x, y, 2, 0);
        world_load_section_from4i(world, l - 1, i1 - 1, plane, 0);
        world_load_section_from4i(world, l, i1 - 1, plane, 1);
        world_load_section_from4i(world, l - 1, i1, plane, 2);
        world_load_section_from4i(world, l, i1, plane, 3);
        world_set_tiles(world);
    }
}

void world_method425(World *world, int i, int j, int k) {
    int l = i / 12;
    int i1 = j / 12;
    int j1 = (i - 1) / 12;
    int k1 = (j - 1) / 12;

    world_set_terrain_ambience(world, l, i1, i, j, k);

    if (l != j1) {
        world_set_terrain_ambience(world, j1, i1, i, j, k);
    }

    if (i1 != k1) {
        world_set_terrain_ambience(world, l, k1, i, j, k);
    }

    if (l != j1 && i1 != k1) {
        world_set_terrain_ambience(world, j1, k1, i, j, k);
    }
}

void world_remove_object(World *world, int x, int y, int id) {
    if (x < 0 || y < 0 || x >= 95 || y >= 95) {
        return;
    }

    if (game_data_object_type[id] == 1 || game_data_object_type[id] == 2) {
        int l = world_get_tile_direction(world, x, y);
        int i1;
        int j1;

        if (l == 0 || l == 4) {
            i1 = game_data_object_width[id];
            j1 = game_data_object_height[id];
        } else {
            j1 = game_data_object_width[id];
            i1 = game_data_object_height[id];
        }

        for (int k1 = x; k1 < x + i1; k1++) {
            for (int l1 = y; l1 < y + j1; l1++) {
                if (game_data_object_type[id] == 1) {
                    world->object_adjacency[k1][l1] &= 0xffbf;
                } else if (l == 0) {
                    world->object_adjacency[k1][l1] &= 0xfffd;

                    if (k1 > 0) {
                        world_method407(world, k1 - 1, l1, 8);
                    }
                } else if (l == 2) {
                    world->object_adjacency[k1][l1] &= 0xfffb;

                    if (l1 < 95) {
                        world_method407(world, k1, l1 + 1, 1);
                    }
                } else if (l == 4) {
                    world->object_adjacency[k1][l1] &= 0xfff7;

                    if (k1 < 95) {
                        world_method407(world, k1 + 1, l1, 2);
                    }
                } else if (l == 6) {
                    world->object_adjacency[k1][l1] &= 0xfffe;

                    if (l1 > 0) {
                        world_method407(world, k1, l1 - 1, 4);
                    }
                }
            }
        }

        world_method404(world, x, y, i1, j1);
    }
}

int world_method427(World *world, int i, int j) {
    return (world_get_wall_roof(world, i, j) > 0 ||
            world_get_wall_roof(world, i - 1, j) > 0 ||
            world_get_wall_roof(world, i - 1, j - 1) > 0 ||
            world_get_wall_roof(world, i, j - 1) > 0);
}

void world_method428(World *world, int i, int j, int k, int l, int i1) {
    int j1 = game_data_wall_object_height[i];

    if (world->terrain_height_local[j][k] < 0x13880) {
        world->terrain_height_local[j][k] += 0x13880 + j1;
    }

    if (world->terrain_height_local[l][i1] < 0x13880) {
        world->terrain_height_local[l][i1] += 0x13880 + j1;
    }
}
