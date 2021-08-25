#include "world.h"

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
    world->scene = scene;
    world->surface = surface;
    world->world_initialised = 1;
    world->base_media_sprite = 750;
}

static int get_byte_wall_coord(int8_t **wall_array, int x, int y) {
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

    return wall_array[height][x * REGION_SIZE + y] & 0xff;
}

int world_get_wall_east_west(World *world, int x, int y) {
    return get_byte_wall_coord(world->walls_east_west, x, y);
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
    return get_byte_wall_coord(world->walls_roof, x, y);
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
    COORD_RANGE_CHECK(x, y);

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
                        world_set_object_adjacency(world, mx - 1, my, 8);
                    }
                } else if (tile_dir == 2) {
                    world->object_adjacency[mx][my] |= 4;

                    if (my < 95) {
                        world_set_object_adjacency(world, mx, my + 1, 1);
                    }
                } else if (tile_dir == 4) {
                    world->object_adjacency[mx][my] |= 8;

                    if (mx < 95) {
                        world_set_object_adjacency(world, mx + 1, my, 2);
                    }
                } else if (tile_dir == 6) {
                    world->object_adjacency[mx][my] |= 1;

                    if (my > 0) {
                        world_set_object_adjacency(world, mx, my - 1, 4);
                    }
                }
            }
        }

        world_method404(world, x, y, model_width, model_height);
    }
}

void world_remove_wall_object(World *world, int x, int y, int k, int id) {
    COORD_RANGE_CHECK(x, y);

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

void world_method402(World *world, int i, int j, int k, int l, int i1) {
    int line_x = i * 3;
    int line_y = j * 3;
    int l1 = scene_method302(world->scene, l);
    int i2 = scene_method302(world->scene, i1);
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

    if (map_data != NULL && sizeof(map_data) > 0) {
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

    if (map_data != NULL && sizeof(map_data) > 0) {
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
                for (int i = 0; i < val - 128; i++)
                    world->walls_roof[chunk][tile++] = 0;
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

        if (map_data != NULL && sizeof(map_data) > 0) {
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
