#include "scene.h"

int scene_frustum_max_x = 0;
int scene_frustum_min_x = 0;
int scene_frustum_max_y = 0;
int scene_frustum_min_y = 0;
int scene_frustum_far_z = 0;
int scene_frustum_near_z = 0;
int64_t scene_texture_count_loaded = 0;

int scene_polygon_depth_compare(const void *a, const void *b) {
    GamePolygon *polygon_a = (*(GamePolygon **)a);
    GamePolygon *polygon_b = (*(GamePolygon **)b);

    if (polygon_a->depth == 0) {
        return -1;
    }

    if (polygon_a->depth == polygon_b->depth) {
        return 0;
    }

    return polygon_a->depth < polygon_b->depth ? 1 : -1;
}

void scene_new(Scene *scene, Surface *surface, int model_count,
               int polygon_count, int max_sprite_count) {
    memset(scene, 0, sizeof(Scene));

    scene->surface = surface;
    scene->max_model_count = model_count;
    scene->max_sprite_count = max_sprite_count;

    memset(scene->gradient_base, 0, RAMP_COUNT * sizeof(int));
    memset(scene->gradient_ramps, 0, RAMP_COUNT * 256 * sizeof(int));

    scene->model_count = 0;
    scene->clip_near = 5;
    scene->clip_far_3d = 1000;
    scene->clip_far_2d = 1000;
    scene->fog_z_falloff = 20;
    scene->fog_z_distance = 10;
    scene->mouse_picking_active = 0;

    scene->interlace = 0;
    scene->width = 512;
    scene->clip_x = 256;
    scene->clip_y = 192;
    scene->base_x = 256;
    scene->base_y = 256;
    scene->view_distance = 8;
    scene->normal_magnitude = 4;

    scene->raster = surface->pixels;

    scene->models = calloc(model_count, sizeof(GameModel *));

    scene->visible_polygons_count = 0;
    scene->visible_polygons = malloc(polygon_count * sizeof(GamePolygon *));

    for (int i = 0; i < polygon_count; i++) {
        scene->visible_polygons[i] = malloc(sizeof(GamePolygon));
        polygon_new(scene->visible_polygons[i]);
    }

    GameModel *view = malloc(sizeof(GameModel));
    game_model_from2(view, scene->max_sprite_count * 2,
                     scene->max_sprite_count);
    scene->view = view;

    scene->sprite_count = 0;
    scene->sprite_id = calloc(max_sprite_count, sizeof(int));
    scene->sprite_width = calloc(max_sprite_count, sizeof(int));
    scene->sprite_height = calloc(max_sprite_count, sizeof(int));
    scene->sprite_x = calloc(max_sprite_count, sizeof(int));
    scene->sprite_z = calloc(max_sprite_count, sizeof(int));
    scene->sprite_y = calloc(max_sprite_count, sizeof(int));
    scene->sprite_translate_x = calloc(max_sprite_count, sizeof(int));
}

void scene_texture_scanline(int32_t *raster, int32_t *texture_pixels, int k,
                            int l, int i1, int j1, int k1, int l1, int i2,
                            int j2, int k2, int l2) {
    if (i2 <= 0) {
        return;
    }

    int i = 0;
    int j = 0;
    int i3 = 0;
    int j3 = 0;
    int i4 = 0;

    if (i1 != 0) {
        i = (k / i1) << 7;
        j = (l / i1) << 7;
    }

    if (i < 0) {
        i = 0;
    } else if (i > 16256) {
        i = 16256;
    }

    k += j1;
    l += k1;
    i1 += l1;

    if (i1 != 0) {
        i3 = (k / i1) << 7;
        j3 = (l / i1) << 7;
    }

    if (i3 < 0) {
        i3 = 0;
    } else if (i3 > 16256) {
        i3 = 16256;
    }

    int k3 = (i3 - i) >> 4;
    int l3 = (j3 - j) >> 4;

    for (int j4 = i2 >> 4; j4 > 0; j4--) {
        i += k2 & 0x600000;
        i4 = k2 >> 23;
        k2 += l2;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i = i3;
        j = j3;
        k += j1;
        l += k1;
        i1 += l1;

        if (i1 != 0) {
            i3 = (k / i1) << 7;
            j3 = (l / i1) << 7;
        }

        if (i3 < 0) {
            i3 = 0;
        } else if (i3 > 16256) {
            i3 = 16256;
        }

        k3 = (i3 - i) >> 4;
        l3 = (j3 - j) >> 4;
    }

    for (int k4 = 0; k4 < (i2 & 0xf); k4++) {
        if ((k4 & 3) == 0) {
            i = (i & 0x3fff) + (k2 & 0x600000);
            i4 = k2 >> 23;
            k2 += l2;
        }

        raster[j2++] = texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
    }
}

void scene_texture_translucent_scanline(int32_t *raster,
                                        int32_t *texture_pixels, int k, int l,
                                        int i1, int j1, int k1, int l1, int i2,
                                        int j2, int k2, int l2) {
    if (i2 <= 0) {
        return;
    }

    int i = 0;
    int j = 0;
    int i3 = 0;
    int j3 = 0;
    int i4 = 0;

    if (i1 != 0) {
        i = (k / i1) << 7;
        j = (l / i1) << 7;
    }

    if (i < 0) {
        i = 0;
    } else if (i > 16256) {
        i = 16256;
    }

    k += j1;
    l += k1;
    i1 += l1;

    if (i1 != 0) {
        i3 = (k / i1) << 7;
        j3 = (l / i1) << 7;
    }

    if (i3 < 0) {
        i3 = 0;
    } else if (i3 > 16256) {
        i3 = 16256;
    }

    int k3 = (i3 - i) >> 4;
    int l3 = (j3 - j) >> 4;

    for (int j4 = i2 >> 4; j4 > 0; j4--) {
        i += k2 & 0x600000;
        i4 = k2 >> 23;
        k2 += l2;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i = i3;
        j = j3;
        k += j1;
        l += k1;
        i1 += l1;

        if (i1 != 0) {
            i3 = (k / i1) << 7;
            j3 = (l / i1) << 7;
        }

        if (i3 < 0) {
            i3 = 0;
        } else if (i3 > 16256) {
            i3 = 16256;
        }

        k3 = (i3 - i) >> 4;
        l3 = (j3 - j) >> 4;
    }

    for (int k4 = 0; k4 < (i2 & 0xf); k4++) {
        if ((k4 & 3) == 0) {
            i = (i & 0x3fff) + (k2 & 0x600000);
            i4 = k2 >> 23;
            k2 += l2;
        }

        raster[j2++] = (texture_pixels[(j & 0x3f80) + (i >> 7)] >> i4) +
                       ((raster[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
    }
}

void scene_texture_back_translucent_scanline(int32_t *raster, int32_t *ai1,
                                             int l, int i1, int j1, int k1,
                                             int l1, int i2, int j2, int k2,
                                             int l2, int i3) {
    if (j2 <= 0) {
        return;
    }

    int i = 0;
    int j = 0;
    int k = 0;
    int j3 = 0;
    int k3 = 0;
    i3 <<= 2;

    if (j1 != 0) {
        j3 = (l / j1) << 7;
        k3 = (i1 / j1) << 7;
    }

    if (j3 < 0) {
        j3 = 0;
    } else if (j3 > 16256) {
        j3 = 16256;
    }

    for (int j4 = j2; j4 > 0; j4 -= 16) {
        l += k1;
        i1 += l1;
        j1 += i2;
        j = j3;
        k = k3;

        if (j1 != 0) {
            j3 = (l / j1) << 7;
            k3 = (i1 / j1) << 7;
        }

        if (j3 < 0) {
            j3 = 0;
        } else if (j3 > 16256) {
            j3 = 16256;
        }

        int l3 = (j3 - j) >> 4;
        int i4 = (k3 - k) >> 4;
        int k4 = l2 >> 23;

        j += l2 & 0x600000;
        l2 += i3;

        if (j4 < 16) {
            for (int l4 = 0; l4 < j4; l4++) {
                if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                    raster[k2] = i;
                }

                k2++;
                j += l3;
                k += i4;

                if ((l4 & 3) == 3) {
                    j = (j & 0x3fff) + (l2 & 0x600000);
                    k4 = l2 >> 23;
                    l2 += i3;
                }
            }
        } else {
            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0x3fff) + (l2 & 0x600000);
            k4 = l2 >> 23;
            l2 += i3;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0x3fff) + (l2 & 0x600000);
            k4 = l2 >> 23;
            l2 += i3;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0x3fff) + (l2 & 0x600000);
            k4 = l2 >> 23;
            l2 += i3;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
        }
    }
}

void scene_texture_scanline2(int32_t *raster, int32_t *texture_pixels, int k,
                             int l, int i1, int j1, int k1, int l1, int i2,
                             int j2, int k2, int l2) {
    if (i2 <= 0) {
        return;
    }

    int i = 0;
    int j = 0;
    int i3 = 0;
    int j3 = 0;
    l2 <<= 2;

    if (i1 != 0) {
        i3 = (k / i1) << 6;
        j3 = (l / i1) << 6;
    }

    if (i3 < 0) {
        i3 = 0;
    } else if (i3 > 4032) {
        i3 = 4032;
    }

    for (int i4 = i2; i4 > 0; i4 -= 16) {
        k += j1;
        l += k1;
        i1 += l1;
        i = i3;
        j = j3;

        if (i1 != 0) {
            i3 = (k / i1) << 6;
            j3 = (l / i1) << 6;
        }

        if (i3 < 0) {
            i3 = 0;
        } else if (i3 > 4032) {
            i3 = 4032;
        }

        int k3 = (i3 - i) >> 4;
        int l3 = (j3 - j) >> 4;
        int32_t j4 = k2 >> 20;
        i += k2 & 0xc0000;
        k2 += l2;

        if (i4 < 16) {
            for (int k4 = 0; k4 < i4; k4++) {
                raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
                i += k3;
                j += l3;

                if ((k4 & 3) == 3) {
                    i = (i & 0xfff) + (k2 & 0xc0000);
                    j4 = k2 >> 20;
                    k2 += l2;
                }
            }
        } else {
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            raster[j2++] = texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4;
        }
    }
}

void scene_texture_translucent_scanline2(int32_t *raster,
                                         int32_t *texture_pixels, int k, int l,
                                         int i1, int j1, int k1, int l1, int i2,
                                         int j2, int k2, int l2) {
    if (i2 <= 0) {
        return;
    }

    int i = 0;
    int j = 0;
    int i3 = 0;
    int j3 = 0;
    l2 <<= 2;

    if (i1 != 0) {
        i3 = (k / i1) << 6;
        j3 = (l / i1) << 6;
    }

    if (i3 < 0) {
        i3 = 0;
    } else if (i3 > 4032) {
        i3 = 4032;
    }

    for (int i4 = i2; i4 > 0; i4 -= 16) {
        k += j1;
        l += k1;
        i1 += l1;
        i = i3;
        j = j3;

        if (i1 != 0) {
            i3 = (k / i1) << 6;
            j3 = (l / i1) << 6;
        }

        if (i3 < 0) {
            i3 = 0;
        } else if (i3 > 4032) {
            i3 = 4032;
        }

        int k3 = (i3 - i) >> 4;
        int l3 = (j3 - j) >> 4;
        int j4 = k2 >> 20;
        i += k2 & 0xc0000;
        k2 += l2;

        if (i4 < 16) {
            for (int k4 = 0; k4 < i4; k4++) {
                raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                               ((raster[j2] >> 1) & 0x7f7f7f);

                i += k3;
                j += l3;

                if ((k4 & 3) == 3) {
                    i = (i & 0xfff) + (k2 & 0xc0000);
                    j4 = k2 >> 20;
                    k2 += l2;
                }
            }
        } else {
            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            raster[j2++] = (texture_pixels[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((raster[j2] >> 1) & 0x7f7f7f);
        }
    }
}

void scene_texture_back_translucent_scanline2(int32_t *raster,
                                              int32_t *texture_pixels, int l,
                                              int i1, int j1, int k1, int l1,
                                              int i2, int j2, int k2, int l2,
                                              int i3) {
    if (j2 <= 0) {
        return;
    }

    int i = 0;
    int j = 0;
    int k = 0;
    int j3 = 0;
    int k3 = 0;
    i3 <<= 2;

    if (j1 != 0) {
        j3 = (l / j1) << 6;
        k3 = (i1 / j1) << 6;
    }

    if (j3 < 0) {
        j3 = 0;
    } else if (j3 > 4032) {
        j3 = 4032;
    }

    for (int j4 = j2; j4 > 0; j4 -= 16) {
        l += k1;
        i1 += l1;
        j1 += i2;
        j = j3;
        k = k3;

        if (j1 != 0) {
            j3 = (l / j1) << 6;
            k3 = (i1 / j1) << 6;
        }

        if (j3 < 0) {
            j3 = 0;
        } else if (j3 > 4032) {
            j3 = 4032;
        }

        int l3 = (j3 - j) >> 4;
        int i4 = (k3 - k) >> 4;
        int k4 = l2 >> 20;
        j += l2 & 0xc0000;
        l2 += i3;

        if (j4 < 16) {
            for (int l4 = 0; l4 < j4; l4++) {
                if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                    raster[k2] = i;
                }

                k2++;
                j += l3;
                k += i4;

                if ((l4 & 3) == 3) {
                    j = (j & 0xfff) + (l2 & 0xc0000);
                    k4 = l2 >> 20;
                    l2 += i3;
                }
            }
        } else {
            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0xfff) + (l2 & 0xc0000);
            k4 = l2 >> 20;
            l2 += i3;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0xfff) + (l2 & 0xc0000);
            k4 = l2 >> 20;
            l2 += i3;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0xfff) + (l2 & 0xc0000);
            k4 = l2 >> 20;
            l2 += i3;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = texture_pixels[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                raster[k2] = i;
            }

            k2++;
        }
    }
}

void scene_gradient_translucent_scanline(int32_t *raster, int i, int raster_idx,
                                         int32_t *ramp, int ramp_index,
                                         int ramp_inc) {
    if (i >= 0) {
        return;
    }

    ramp_inc *= 4;
    int colour = ramp[(ramp_index / RAMP_SIZE) & 0xff];
    ramp_index += ramp_inc;
    int length = i / 16;

    for (int i1 = length; i1 < 0; i1++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                raster[raster_idx++] =
                    colour + ((raster[raster_idx] >> 1) & 0x7f7f7f);
            }

            colour = ramp[(ramp_index / RAMP_SIZE) & 0xff];
            ramp_index += ramp_inc;
        }
    }

    length = -(i % 16);

    for (int i1 = 0; i1 < length; i1++) {
        raster[raster_idx++] = colour + ((raster[raster_idx] >> 1) & 0x7f7f7f);

        if ((i1 & 3) == 3) {
            colour = ramp[(ramp_index / RAMP_SIZE) & 0xff];
            ramp_index += ramp_inc * 2;
        }
    }
}

void scene_gradient_scanline(int32_t *raster, int i, int raster_idx,
                             int32_t *ramp, int ramp_index, int ramp_inc) {
    if (i >= 0) {
        return;
    }

    ramp_inc *= (RAMP_WIDE ? 2 : 4);

    int colour = ramp[(ramp_index / RAMP_SIZE) & 0xff];
    ramp_index += ramp_inc;
    int length = i / (RAMP_WIDE ? 8 : 16);

    for (int i1 = length; i1 < 0; i1++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < (RAMP_WIDE ? 2 : 4); k++) {
                raster[raster_idx++] = colour;
            }

            colour = ramp[(ramp_index / RAMP_SIZE) & 0xff];
            ramp_index += ramp_inc;
        }
    }

    length = -(i % (RAMP_WIDE ? 8 : 16));
    int ramp_flag = (RAMP_WIDE ? 1 : 3);

    for (int i1 = 0; i1 < length; i1++) {
        raster[raster_idx++] = colour;

        if ((i1 & ramp_flag) == ramp_flag) {
            colour = ramp[(ramp_index >> 8) & 0xff];
            ramp_index += ramp_inc;
        }
    }
}

int rgb(int i, int j, int k) {
    return -1 - (i / 8) * 1024 - (j / 8) * 32 - (k / 8);
}

void scene_add_model(Scene *scene, GameModel *model) {
    if (model == NULL) {
        fprintf(stderr, "Warning tried to add null object!\n");
    }

    if (scene->model_count < scene->max_model_count) {
        scene->models[scene->model_count++] = model;
    }
}

void scene_remove_model(Scene *scene, GameModel *model) {
    for (int i = 0; i < scene->model_count; i++) {
        if (scene->models[i] == model) {
            // game_model_destroy(model);
            // free(model);

            scene->models[i] = NULL;
            scene->model_count--;

            for (int j = i; j < scene->model_count; j++) {
                scene->models[j] = scene->models[j + 1];
            }
        }
    }
}

void scene_null_model(Scene *scene, GameModel *model) {
    for (int i = 0; i < scene->model_count; i++) {
        if (scene->models[i] == model) {
            scene->models[i] = NULL;
        }
    }
}

void scene_dispose(Scene *scene) {
    for (int i = 0; i < scene->model_count; i++) {
        /* these need to be free'd elsewhere */
        scene->models[i] = NULL;
    }

    scene_clear(scene);

    scene->model_count = 0;
}

void scene_clear(Scene *scene) {
    scene->sprite_count = 0;

    game_model_destroy(scene->view);

    game_model_from2(scene->view, scene->max_sprite_count * 2,
                     scene->max_sprite_count);
}

void scene_reduce_sprites(Scene *scene, int i) {
    scene->sprite_count -= i;
    game_model_reduce(scene->view, i, i * 2);

    if (scene->sprite_count < 0) {
        scene->sprite_count = 0;
    }
}

int scene_add_sprite(Scene *scene, int n, int x, int z, int y, int w, int h,
                     int tag) {
    scene->sprite_id[scene->sprite_count] = n;
    scene->sprite_x[scene->sprite_count] = x;
    scene->sprite_z[scene->sprite_count] = z;
    scene->sprite_y[scene->sprite_count] = y;
    scene->sprite_width[scene->sprite_count] = w;
    scene->sprite_height[scene->sprite_count] = h;
    scene->sprite_translate_x[scene->sprite_count] = 0;

    int *vertices = malloc(2 * sizeof(int));

    vertices[0] = game_model_create_vertex(scene->view, x, z, y);
    vertices[1] = game_model_create_vertex(scene->view, x, z - h, y);

    game_model_create_face(scene->view, 2, vertices, 0, 0);

    scene->view->face_tag[scene->sprite_count] = tag;
    scene->view->is_local_player[scene->sprite_count++] = 0;

    return scene->sprite_count - 1;
}

void scene_set_local_player(Scene *scene, int i) {
    scene->view->is_local_player[i] = 1;
}

void scene_set_sprite_translate_x(Scene *scene, int i, int n) {
    scene->sprite_translate_x[i] = n;
}

void scene_set_mouse_loc(Scene *scene, int x, int y) {
    scene->mouse_x = x - scene->base_x;
    scene->mouse_y = y;
    scene->mouse_picked_count = 0;
    scene->mouse_picking_active = 1;
}

void scene_set_bounds(Scene *scene, int base_x, int base_y, int clip_x,
                      int clip_y, int width, int view_distance) {
    scene->clip_x = clip_x;
    scene->clip_y = clip_y;
    scene->base_x = base_x;
    scene->base_y = base_y;
    scene->width = width;
    scene->view_distance = view_distance;

    int scanlines_length = clip_y + base_y;

    scene->scanlines = malloc(scanlines_length * sizeof(Scanline *));

    for (int i = 0; i < scanlines_length; i++) {
        Scanline *scanline = calloc(1, sizeof(Scanline));
        scene->scanlines[i] = scanline;
    }
}

void scene_polygons_intersect_sort(Scene *scene, int step,
                                   GamePolygon **polygons, int count) {
    for (int i = 0; i <= count; i++) {
        polygons[i]->skip_something = 0;
        polygons[i]->index = i;
        polygons[i]->index2 = -1;
    }

    int l = 0;

    do {
        while (polygons[l]->skip_something) {
            l++;
        }

        if (l == count) {
            return;
        }

        GamePolygon *polygon = polygons[l];
        polygon->skip_something = 1;

        int start = l;
        int end = l + step;

        if (end >= count) {
            end = count - 1;
        }

        for (int i = end; i >= start + 1; i--) {
            GamePolygon *other = polygons[i];

            if (polygon->min_plane_x < other->max_plane_x &&
                other->min_plane_x < polygon->max_plane_x &&
                polygon->min_plane_y < other->max_plane_y &&
                other->min_plane_y < polygon->max_plane_y &&
                polygon->index != other->index2 &&
                !scene_separate_polygon(polygon, other) &&
                scene_heuristic_polygon(other, polygon)) {
                scene_polygons_order(scene, polygons, start, i);

                if (polygons[i] != other) {
                    i++;
                }

                start = scene->new_start;
                other->index2 = polygon->index;
            }
        }
    } while (1);
}

int scene_polygons_order(Scene *scene, GamePolygon **polygons, int start,
                         int end) {
    do {
        GamePolygon *polygon = polygons[start];

        for (int i = start + 1; i <= end; i++) {
            GamePolygon *polygon_1 = polygons[i];

            if (!scene_separate_polygon(polygon_1, polygon)) {
                break;
            }

            polygons[start] = polygon_1;
            polygons[i] = polygon;
            start = i;

            if (start == end) {
                scene->new_start = start;
                scene->new_end = start - 1;
                return 1;
            }
        }

        GamePolygon *polygon_2 = polygons[end];

        for (int i = end - 1; i >= start; i--) {
            GamePolygon *polygon_3 = polygons[i];

            if (!scene_separate_polygon(polygon_2, polygon_3)) {
                break;
            }

            polygons[end] = polygon_3;
            polygons[i] = polygon_2;
            end = i;

            if (start == end) {
                scene->new_start = end + 1;
                scene->new_end = end;
                return 1;
            }
        }

        if (start + 1 >= end) {
            scene->new_start = start;
            scene->new_end = end;
            return 0;
        }

        if (!scene_polygons_order(scene, polygons, start + 1, end)) {
            scene->new_start = start;
            return 0;
        }

        end = scene->new_end;
    } while (1);
}

void scene_set_frustum(Scene *scene, int x, int y, int z) {
    int yaw = (-scene->camera_yaw + 1024) & 1023;
    int pitch = (-scene->camera_pitch + 1024) & 1023;
    int roll = (-scene->camera_roll + 1024) & 1023;

    if (roll != 0) {
        int sine = sin_cos_2048[roll];
        int cosine = sin_cos_2048[roll + 1024];
        int i3 = (y * sine + x * cosine) / 32768;
        y = (y * cosine - x * sine) / 32768;
        x = i3;
    }

    if (yaw != 0) {
        int sine = sin_cos_2048[yaw];
        int cosine = sin_cos_2048[yaw + 1024];
        int j3 = (y * cosine - z * sine) / 32768;
        z = (y * sine + z * cosine) / 32768;
        y = j3;
    }

    if (pitch != 0) {
        int sine = sin_cos_2048[pitch];
        int cosine = sin_cos_2048[pitch + 1024];
        int k3 = (z * sine + x * cosine) / 32768;
        z = (z * cosine - x * sine) / 32768;
        x = k3;
    }

    if (x < scene_frustum_max_x) {
        scene_frustum_max_x = x;
    }

    if (x > scene_frustum_min_x) {
        scene_frustum_min_x = x;
    }

    if (y < scene_frustum_max_y) {
        scene_frustum_max_y = y;
    }

    if (y > scene_frustum_min_y) {
        scene_frustum_min_y = y;
    }

    if (z < scene_frustum_far_z) {
        scene_frustum_far_z = z;
    }

    if (z > scene_frustum_near_z) {
        scene_frustum_near_z = z;
    }
}

void scene_render(Scene *scene) {
    scene->interlace = scene->surface->interlace;

    int i3 = (scene->clip_x * scene->clip_far_3d) >> scene->view_distance;
    int j3 = (scene->clip_y * scene->clip_far_3d) >> scene->view_distance;

    scene_frustum_max_x = 0;
    scene_frustum_min_x = 0;
    scene_frustum_max_y = 0;
    scene_frustum_min_y = 0;
    scene_frustum_far_z = 0;
    scene_frustum_near_z = 0;

    scene_set_frustum(scene, -i3, -j3, scene->clip_far_3d);
    scene_set_frustum(scene, -i3, j3, scene->clip_far_3d);
    scene_set_frustum(scene, i3, -j3, scene->clip_far_3d);
    scene_set_frustum(scene, i3, j3, scene->clip_far_3d);
    scene_set_frustum(scene, -scene->clip_x, -scene->clip_y, 0);
    scene_set_frustum(scene, -scene->clip_x, scene->clip_y, 0);
    scene_set_frustum(scene, scene->clip_x, -scene->clip_y, 0);
    scene_set_frustum(scene, scene->clip_x, scene->clip_y, 0);

    scene_frustum_max_x += scene->camera_x;
    scene_frustum_min_x += scene->camera_x;
    scene_frustum_max_y += scene->camera_y;
    scene_frustum_min_y += scene->camera_y;
    scene_frustum_far_z += scene->camera_z;
    scene_frustum_near_z += scene->camera_z;

    scene->models[scene->model_count] = scene->view;
    scene->view->transform_state = 2;

    for (int i = 0; i <= scene->model_count; i++) {
        game_model_project(scene->models[i], scene->camera_x, scene->camera_y,
                           scene->camera_z, scene->camera_yaw,
                           scene->camera_pitch, scene->camera_roll,
                           scene->view_distance, scene->clip_near);
    }

    scene->visible_polygons_count = 0;

    for (int i = 0; i < scene->model_count; i++) {
        GameModel *game_model = scene->models[i];

        if (game_model->visible) {
            for (int face = 0; face < game_model->num_faces; face++) {
                int num_vertices = game_model->face_num_vertices[face];
                int *face_vertices = game_model->face_vertices[face];
                int visible = 0;

                for (int vertex = 0; vertex < num_vertices; vertex++) {
                    int z = game_model->project_vertex_z[face_vertices[vertex]];

                    if (z <= scene->clip_near || z >= scene->clip_far_3d) {
                        continue;
                    }

                    visible = 1;
                    break;
                }

                if (visible) {
                    int view_x_count = 0;

                    for (int vertex = 0; vertex < num_vertices; vertex++) {
                        int x =
                            game_model->vertex_view_x[face_vertices[vertex]];

                        if (x > -scene->clip_x) {
                            view_x_count |= 1;
                        }

                        if (x < scene->clip_x) {
                            view_x_count |= 2;
                        }

                        if (view_x_count == 3) {
                            break;
                        }
                    }

                    if (view_x_count == 3) {
                        int view_y_count = 0;

                        for (int vertex = 0; vertex < num_vertices; vertex++) {
                            int k1 = game_model
                                         ->vertex_view_y[face_vertices[vertex]];

                            if (k1 > -scene->clip_y) {
                                view_y_count |= 1;
                            }

                            if (k1 < scene->clip_y) {
                                view_y_count |= 2;
                            }

                            if (view_y_count == 3) {
                                break;
                            }
                        }

                        if (view_y_count == 3) {
                            GamePolygon *polygon_1 =
                                scene->visible_polygons
                                    [scene->visible_polygons_count];

                            polygon_1->model = game_model;
                            polygon_1->face = face;

                            scene_initialise_polygon_3d(
                                scene, scene->visible_polygons_count);

                            int face_fill = 0;

                            if (polygon_1->visibility < 0) {
                                face_fill = game_model->face_fill_front[face];
                            } else {
                                face_fill = game_model->face_fill_back[face];
                            }

                            if (face_fill != COLOUR_TRANSPARENT) {
                                int h = 0;

                                for (int vertex = 0; vertex < num_vertices;
                                     vertex++) {
                                    h += game_model->project_vertex_z
                                             [face_vertices[vertex]];
                                }

                                polygon_1->depth =
                                    (h / num_vertices) + game_model->depth;

                                polygon_1->facefill = face_fill;

                                scene->visible_polygons_count++;
                            }
                        }
                    }
                }
            }
        }
    }

    GameModel *model_2d = scene->view;

    if (model_2d->visible) {
        for (int face = 0; face < model_2d->num_faces; face++) {
            int *face_vertices = model_2d->face_vertices[face];
            int face_0 = face_vertices[0];
            int vx = model_2d->vertex_view_x[face_0];
            int vy = model_2d->vertex_view_y[face_0];
            int vz = model_2d->project_vertex_z[face_0];

            if (vz > scene->clip_near && vz < scene->clip_far_2d) {
                int vw =
                    (scene->sprite_width[face] << scene->view_distance) / vz;

                int vh =
                    (scene->sprite_height[face] << scene->view_distance) / vz;

                if (vx - (vw / 2) <= scene->clip_x &&
                    vx + (vw / 2) >= -scene->clip_x &&
                    vy - vh <= scene->clip_y && vy >= -scene->clip_y) {

                    GamePolygon *polygon_2 =
                        scene->visible_polygons[scene->visible_polygons_count];

                    polygon_2->model = model_2d;
                    polygon_2->face = face;

                    scene_initialise_polygon_2d(scene,
                                                scene->visible_polygons_count);

                    polygon_2->depth =
                        (vz + model_2d->project_vertex_z[face_vertices[1]]) / 2;

                    scene->visible_polygons_count++;
                }
            }
        }
    }

    if (scene->visible_polygons_count == 0) {
        return;
    }

    scene->last_visible_polygons_count = scene->visible_polygons_count;

    qsort(scene->visible_polygons, scene->visible_polygons_count,
          sizeof(GamePolygon *), scene_polygon_depth_compare);

    scene_polygons_intersect_sort(scene, 100, scene->visible_polygons,
                                  scene->visible_polygons_count);

    for (int i = 0; i < scene->visible_polygons_count; i++) {
        GamePolygon *polygon = scene->visible_polygons[i];
        GameModel *game_model = polygon->model;
        int face = polygon->face;

        if (game_model == scene->view) {
            int *face_vertices = game_model->face_vertices[face];
            int face_0 = face_vertices[0];
            int vx = game_model->vertex_view_x[face_0];
            int vy = game_model->vertex_view_y[face_0];
            int vz = game_model->project_vertex_z[face_0];
            int w = (scene->sprite_width[face] << scene->view_distance) / vz;
            int h = (scene->sprite_height[face] << scene->view_distance) / vz;
            int tx = game_model->vertex_view_x[face_vertices[1]] - vx;
            int x = vx - (w / 2);
            int y = scene->base_y + vy - h;

            surface_sprite_clipping_from7(scene->surface, x + scene->base_x, y,
                                          w, h, scene->sprite_id[face], tx,
                                          (256 << scene->view_distance) / vz);

            if (scene->mouse_picking_active &&
                scene->mouse_picked_count < MOUSE_PICKED_MAX) {

                x += (scene->sprite_translate_x[face] << scene->view_distance) /
                     vz;

                if (scene->mouse_y >= y && scene->mouse_y <= y + h &&
                    scene->mouse_x >= x && scene->mouse_x <= x + w &&
                    !game_model->unpickable &&
                    game_model->is_local_player[face] == 0) {
                    scene->mouse_picked_models[scene->mouse_picked_count] =
                        game_model;
                    scene->mouse_picked_faces[scene->mouse_picked_count] = face;
                    scene->mouse_picked_count++;
                }
            }
        } else {
            int k8 = 0;
            int j10 = 0;
            int face_num_vertices = game_model->face_num_vertices[face];
            int *face_vertices = game_model->face_vertices[face];

            if (game_model->face_intensity[face] != COLOUR_TRANSPARENT) {
                if (polygon->visibility < 0) {
                    j10 = game_model->light_ambience -
                          game_model->face_intensity[face];
                } else {
                    j10 = game_model->light_ambience +
                          game_model->face_intensity[face];
                }
            }

            for (int k11 = 0; k11 < face_num_vertices; k11++) {
                int k2 = face_vertices[k11];

                scene->vertex_x[k11] = game_model->project_vertex_x[k2];
                scene->vertex_y[k11] = game_model->project_vertex_y[k2];
                scene->vertex_z[k11] = game_model->project_vertex_z[k2];

                if (game_model->face_intensity[face] == COLOUR_TRANSPARENT) {
                    if (polygon->visibility < 0) {
                        j10 = game_model->light_ambience -
                              game_model->vertex_intensity[k2] +
                              game_model->vertex_ambience[k2];
                    } else {
                        j10 = game_model->light_ambience +
                              game_model->vertex_intensity[k2] +
                              game_model->vertex_ambience[k2];
                    }
                }

                if (game_model->project_vertex_z[k2] >= scene->clip_near) {
                    scene->plane_x[k8] = game_model->vertex_view_x[k2];
                    scene->plane_y[k8] = game_model->vertex_view_y[k2];
                    scene->vertex_shade[k8] = j10;

                    if (game_model->project_vertex_z[k2] >
                        scene->fog_z_distance) {
                        scene->vertex_shade[k8] +=
                            (game_model->project_vertex_z[k2] -
                             scene->fog_z_distance) /
                            scene->fog_z_falloff;
                    }

                    k8++;
                } else {
                    int k9 = 0;

                    if (k11 == 0) {
                        k9 = face_vertices[face_num_vertices - 1];
                    } else {
                        k9 = face_vertices[k11 - 1];
                    }

                    if (game_model->project_vertex_z[k9] >= scene->clip_near) {
                        int k7 = game_model->project_vertex_z[k2] -
                                 game_model->project_vertex_z[k9];

                        int i5 = game_model->project_vertex_x[k2] -
                                 (((game_model->project_vertex_x[k2] -
                                    game_model->project_vertex_x[k9]) *
                                   (game_model->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  k7);

                        int j6 = game_model->project_vertex_y[k2] -
                                 (((game_model->project_vertex_y[k2] -
                                    game_model->project_vertex_y[k9]) *
                                   (game_model->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  k7);

                        scene->plane_x[k8] =
                            (i5 << scene->view_distance) / scene->clip_near;

                        scene->plane_y[k8] =
                            (j6 << scene->view_distance) / scene->clip_near;

                        scene->vertex_shade[k8] = j10;
                        k8++;
                    }

                    if (k11 == face_num_vertices - 1) {
                        k9 = face_vertices[0];
                    } else {
                        k9 = face_vertices[k11 + 1];
                    }

                    if (game_model->project_vertex_z[k9] >= scene->clip_near) {
                        int l7 = game_model->project_vertex_z[k2] -
                                 game_model->project_vertex_z[k9];

                        int j5 = game_model->project_vertex_x[k2] -
                                 (((game_model->project_vertex_x[k2] -
                                    game_model->project_vertex_x[k9]) *
                                   (game_model->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  l7);

                        int k6 = game_model->project_vertex_y[k2] -
                                 (((game_model->project_vertex_y[k2] -
                                    game_model->project_vertex_y[k9]) *
                                   (game_model->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  l7);

                        scene->plane_x[k8] =
                            (j5 << scene->view_distance) / scene->clip_near;

                        scene->plane_y[k8] =
                            (k6 << scene->view_distance) / scene->clip_near;

                        scene->vertex_shade[k8] = j10;
                        k8++;
                    }
                }
            }

            for (int i12 = 0; i12 < face_num_vertices; i12++) {
                if (scene->vertex_shade[i12] < 0) {
                    scene->vertex_shade[i12] = 0;
                } else if (scene->vertex_shade[i12] > 255) {
                    scene->vertex_shade[i12] = 255;
                }

                if (polygon->facefill >= 0) {
                    if (scene->texture_dimension[polygon->facefill] == 1) {
                        scene->vertex_shade[i12] <<= 9;
                    } else {
                        scene->vertex_shade[i12] <<= 6;
                    }
                }
            }

            scene_generate_scanlines(scene, k8, scene->plane_x, scene->plane_y,
                                     scene->vertex_shade, game_model, face);

            if (scene->max_y > scene->min_y) {
                scene_rasterize(scene, face_num_vertices, scene->vertex_x,
                                scene->vertex_y, scene->vertex_z,
                                polygon->facefill, game_model);
            }
        }
    }

    scene->mouse_picking_active = 0;
}

void scene_generate_scanlines(Scene *scene, int i1, int32_t *plane_x,
                              int32_t *plane_y, int32_t *vertex_shade,
                              GameModel *game_model, int face) {
    int start_x = 0;
    int end_x = 0;
    int start_s = 0;

    if (i1 == 3) {
        int k1 = plane_y[0] + scene->base_y;
        int k2 = plane_y[1] + scene->base_y;
        int k3 = plane_y[2] + scene->base_y;
        int k4 = plane_x[0];
        int l5 = plane_x[1];
        int j7 = plane_x[2];
        int l8 = vertex_shade[0];
        int j10 = vertex_shade[1];
        int j11 = vertex_shade[2];
        int j12 = scene->base_y + scene->clip_y - 1;

        int l12 = 0;
        int j13 = 0;
        int l13 = 0;
        int j14 = 0;
        int l14 = COLOUR_TRANSPARENT;
        int j15 = -COLOUR_TRANSPARENT;

        if (k3 != k1) {
            j13 = ((j7 - k4) << 8) / (k3 - k1);
            j14 = ((j11 - l8) << 8) / (k3 - k1);

            if (k1 < k3) {
                l12 = k4 << 8;
                l13 = l8 << 8;
                l14 = k1;
                j15 = k3;
            } else {
                l12 = j7 << 8;
                l13 = j11 << 8;
                l14 = k3;
                j15 = k1;
            }

            if (l14 < 0) {
                l12 -= j13 * l14;
                l13 -= j14 * l14;
                l14 = 0;
            }

            if (j15 > j12) {
                j15 = j12;
            }
        }

        int l15 = 0;
        int j16 = 0;
        int l16 = 0;
        int j17 = 0;
        int l17 = COLOUR_TRANSPARENT;
        int j18 = -COLOUR_TRANSPARENT;

        if (k2 != k1) {
            j16 = ((l5 - k4) << 8) / (k2 - k1);
            j17 = ((j10 - l8) << 8) / (k2 - k1);

            if (k1 < k2) {
                l15 = k4 << 8;
                l16 = l8 << 8;
                l17 = k1;
                j18 = k2;
            } else {
                l15 = l5 << 8;
                l16 = j10 << 8;
                l17 = k2;
                j18 = k1;
            }

            if (l17 < 0) {
                l15 -= j16 * l17;
                l16 -= j17 * l17;
                l17 = 0;
            }

            if (j18 > j12) {
                j18 = j12;
            }
        }

        int l18 = 0;
        int j19 = 0;
        int l19 = 0;
        int j20 = 0;
        int l20 = COLOUR_TRANSPARENT;
        int j21 = -COLOUR_TRANSPARENT;

        if (k3 != k2) {
            j19 = ((j7 - l5) << 8) / (k3 - k2);
            j20 = ((j11 - j10) << 8) / (k3 - k2);

            if (k2 < k3) {
                l18 = l5 << 8;
                l19 = j10 << 8;
                l20 = k2;
                j21 = k3;
            } else {
                l18 = j7 << 8;
                l19 = j11 << 8;
                l20 = k3;
                j21 = k2;
            }

            if (l20 < 0) {
                l18 -= j19 * l20;
                l19 -= j20 * l20;
                l20 = 0;
            }

            if (j21 > j12) {
                j21 = j12;
            }
        }

        scene->min_y = l14;

        if (l17 < scene->min_y) {
            scene->min_y = l17;
        }

        if (l20 < scene->min_y) {
            scene->min_y = l20;
        }

        scene->max_y = j15;

        if (j18 > scene->max_y) {
            scene->max_y = j18;
        }

        if (j21 > scene->max_y) {
            scene->max_y = j21;
        }

        int end_s = 0;

        for (int y = scene->min_y; y < scene->max_y; y++) {
            if (y >= l14 && y < j15) {
                start_x = end_x = l12;
                start_s = end_s = l13;
                l12 += j13;
                l13 += j14;
            } else {
                start_x = 655360;
                end_x = -655360;
            }

            if (y >= l17 && y < j18) {
                if (l15 < start_x) {
                    start_x = l15;
                    start_s = l16;
                }

                if (l15 > end_x) {
                    end_x = l15;
                    end_s = l16;
                }

                l15 += j16;
                l16 += j17;
            }

            if (y >= l20 && y < j21) {
                if (l18 < start_x) {
                    start_x = l18;
                    start_s = l19;
                }

                if (l18 > end_x) {
                    end_x = l18;
                    end_s = l19;
                }

                l18 += j19;
                l19 += j20;
            }

            Scanline *scanline_6 = scene->scanlines[y];
            scanline_6->start_x = start_x;
            scanline_6->end_x = end_x;
            scanline_6->start_s = start_s;
            scanline_6->end_s = end_s;
        }

        if (scene->min_y < scene->base_y - scene->clip_y) {
            scene->min_y = scene->base_y - scene->clip_y;
        }
    } else if (i1 == 4) {
        int l1 = plane_y[0] + scene->base_y;
        int l2 = plane_y[1] + scene->base_y;
        int l3 = plane_y[2] + scene->base_y;
        int l4 = plane_y[3] + scene->base_y;
        int i6 = plane_x[0];
        int k7 = plane_x[1];
        int i9 = plane_x[2];
        int k10 = plane_x[3];
        int k11 = vertex_shade[0];
        int k12 = vertex_shade[1];
        int i13 = vertex_shade[2];
        int k13 = vertex_shade[3];
        int i14 = scene->base_y + scene->clip_y - 1;

        int k14 = 0;
        int i15 = 0;
        int k15 = 0;
        int i16 = 0;
        int k16 = COLOUR_TRANSPARENT;
        int i17 = -COLOUR_TRANSPARENT;

        if (l4 != l1) {
            i15 = ((k10 - i6) << 8) / (l4 - l1);
            i16 = ((k13 - k11) << 8) / (l4 - l1);

            if (l1 < l4) {
                k14 = i6 << 8;
                k15 = k11 << 8;
                k16 = l1;
                i17 = l4;
            } else {
                k14 = k10 << 8;
                k15 = k13 << 8;
                k16 = l4;
                i17 = l1;
            }

            if (k16 < 0) {
                k14 -= i15 * k16;
                k15 -= i16 * k16;
                k16 = 0;
            }

            if (i17 > i14) {
                i17 = i14;
            }
        }

        int k17 = 0;
        int i18 = 0;
        int k18 = 0;
        int i19 = 0;
        int k19 = COLOUR_TRANSPARENT;
        int i20 = -COLOUR_TRANSPARENT;

        if (l2 != l1) {
            i18 = ((k7 - i6) << 8) / (l2 - l1);
            i19 = ((k12 - k11) << 8) / (l2 - l1);

            if (l1 < l2) {
                k17 = i6 << 8;
                k18 = k11 << 8;
                k19 = l1;
                i20 = l2;
            } else {
                k17 = k7 << 8;
                k18 = k12 << 8;
                k19 = l2;
                i20 = l1;
            }

            if (k19 < 0) {
                k17 -= i18 * k19;
                k18 -= i19 * k19;
                k19 = 0;
            }

            if (i20 > i14) {
                i20 = i14;
            }
        }

        int k20 = 0;
        int i21 = 0;
        int k21 = 0;
        int i22 = 0;
        int j22 = COLOUR_TRANSPARENT;
        int k22 = -COLOUR_TRANSPARENT;

        if (l3 != l2) {
            i21 = ((i9 - k7) << 8) / (l3 - l2);
            i22 = ((i13 - k12) << 8) / (l3 - l2);

            if (l2 < l3) {
                k20 = k7 << 8;
                k21 = k12 << 8;
                j22 = l2;
                k22 = l3;
            } else {
                k20 = i9 << 8;
                k21 = i13 << 8;
                j22 = l3;
                k22 = l2;
            }

            if (j22 < 0) {
                k20 -= i21 * j22;
                k21 -= i22 * j22;
                j22 = 0;
            }

            if (k22 > i14) {
                k22 = i14;
            }
        }

        int l22 = 0;
        int i23 = 0;
        int j23 = 0;
        int k23 = 0;
        int l23 = COLOUR_TRANSPARENT;
        int i24 = -COLOUR_TRANSPARENT;

        if (l4 != l3) {
            i23 = ((k10 - i9) << 8) / (l4 - l3);
            k23 = ((k13 - i13) << 8) / (l4 - l3);

            if (l3 < l4) {
                l22 = i9 << 8;
                j23 = i13 << 8;
                l23 = l3;
                i24 = l4;
            } else {
                l22 = k10 << 8;
                j23 = k13 << 8;
                l23 = l4;
                i24 = l3;
            }

            if (l23 < 0) {
                l22 -= i23 * l23;
                j23 -= k23 * l23;
                l23 = 0;
            }

            if (i24 > i14) {
                i24 = i14;
            }
        }

        scene->min_y = k16;

        if (k19 < scene->min_y) {
            scene->min_y = k19;
        }

        if (j22 < scene->min_y) {
            scene->min_y = j22;
        }

        if (l23 < scene->min_y) {
            scene->min_y = l23;
        }

        scene->max_y = i17;

        if (i20 > scene->max_y) {
            scene->max_y = i20;
        }

        if (k22 > scene->max_y) {
            scene->max_y = k22;
        }

        if (i24 > scene->max_y) {
            scene->max_y = i24;
        }

        int end_s = 0;

        for (int y = scene->min_y; y < scene->max_y; y++) {
            if (y >= k16 && y < i17) {
                start_x = end_x = k14;
                start_s = end_s = k15;
                k14 += i15;
                k15 += i16;
            } else {
                start_x = 655360;
                end_x = -655360;
            }

            if (y >= k19 && y < i20) {
                if (k17 < start_x) {
                    start_x = k17;
                    start_s = k18;
                }

                if (k17 > end_x) {
                    end_x = k17;
                    end_s = k18;
                }

                k17 += i18;
                k18 += i19;
            }

            if (y >= j22 && y < k22) {
                if (k20 < start_x) {
                    start_x = k20;
                    start_s = k21;
                }

                if (k20 > end_x) {
                    end_x = k20;
                    end_s = k21;
                }

                k20 += i21;
                k21 += i22;
            }

            if (y >= l23 && y < i24) {
                if (l22 < start_x) {
                    start_x = l22;
                    start_s = j23;
                }

                if (l22 > end_x) {
                    end_x = l22;
                    end_s = j23;
                }

                l22 += i23;
                j23 += k23;
            }

            Scanline *scanline_7 = scene->scanlines[y];
            scanline_7->start_x = start_x;
            scanline_7->end_x = end_x;
            scanline_7->start_s = start_s;
            scanline_7->end_s = end_s;
        }

        if (scene->min_y < scene->base_y - scene->clip_y) {
            scene->min_y = scene->base_y - scene->clip_y;
        }
    } else {
        scene->max_y = scene->min_y = plane_y[0] += scene->base_y;

        for (int y = 1; y < i1; y++) {
            int i2 = 0;

            if ((i2 = plane_y[y] += scene->base_y) < scene->min_y) {
                scene->min_y = i2;
            } else if (i2 > scene->max_y) {
                scene->max_y = i2;
            }
        }

        if (scene->min_y < scene->base_y - scene->clip_y) {
            scene->min_y = scene->base_y - scene->clip_y;
        }

        if (scene->max_y >= scene->base_y + scene->clip_y) {
            scene->max_y = scene->base_y + scene->clip_y - 1;
        }

        if (scene->min_y >= scene->max_y) {
            return;
        }

        for (int y = scene->min_y; y < scene->max_y; y++) {
            Scanline *scanline = scene->scanlines[y];
            scanline->start_x = 655360;
            scanline->end_x = -655360;
        }

        int j2 = i1 - 1;
        int i3 = plane_y[0];
        int i4 = plane_y[j2];

        if (i3 < i4) {
            int i5 = plane_x[0] << 8;
            int j6 = ((plane_x[j2] - plane_x[0]) << 8) / (i4 - i3);
            int l7 = vertex_shade[0] << 8;
            int j9 = ((vertex_shade[j2] - vertex_shade[0]) << 8) / (i4 - i3);

            if (i3 < 0) {
                i5 -= j6 * i3;
                l7 -= j9 * i3;
                i3 = 0;
            }

            if (i4 > scene->max_y) {
                i4 = scene->max_y;
            }

            for (int y = i3; y <= i4; y++) {
                Scanline *scanline_2 = scene->scanlines[y];
                scanline_2->start_x = scanline_2->end_x = i5;
                scanline_2->start_s = scanline_2->end_s = l7;
                i5 += j6;
                l7 += j9;
            }
        } else if (i3 > i4) {
            int j5 = plane_x[j2] << 8;
            int k6 = ((plane_x[0] - plane_x[j2]) << 8) / (i3 - i4);
            int i8 = vertex_shade[j2] << 8;
            int k9 = ((vertex_shade[0] - vertex_shade[j2]) << 8) / (i3 - i4);

            if (i4 < 0) {
                j5 -= k6 * i4;
                i8 -= k9 * i4;
                i4 = 0;
            }

            if (i3 > scene->max_y) {
                i3 = scene->max_y;
            }

            for (int y = i4; y <= i3; y++) {
                Scanline *scanline_3 = scene->scanlines[y];
                scanline_3->start_x = j5;
                scanline_3->end_x = j5;
                scanline_3->start_s = i8;
                scanline_3->end_s = i8;
                j5 += k6;
                i8 += k9;
            }
        }

        for (int y = 0; y < j2; y++) {
            int k5 = y + 1;
            int j3 = plane_y[y];
            int j4 = plane_y[k5];

            if (j3 < j4) {
                int l6 = plane_x[y] << 8;
                int j8 = ((plane_x[k5] - plane_x[y]) << 8) / (j4 - j3);
                int l9 = vertex_shade[y] << 8;
                int l10 =
                    ((vertex_shade[k5] - vertex_shade[y]) << 8) / (j4 - j3);

                if (j3 < 0) {
                    l6 -= j8 * j3;
                    l9 -= l10 * j3;
                    j3 = 0;
                }

                if (j4 > scene->max_y) {
                    j4 = scene->max_y;
                }

                for (int l11 = j3; l11 <= j4; l11++) {
                    Scanline *scanline_4 = scene->scanlines[l11];

                    if (l6 < scanline_4->start_x) {
                        scanline_4->start_x = l6;
                        scanline_4->start_s = l9;
                    }

                    if (l6 > scanline_4->end_x) {
                        scanline_4->end_x = l6;
                        scanline_4->end_s = l9;
                    }

                    l6 += j8;
                    l9 += l10;
                }
            } else if (j3 > j4) {
                int i7 = plane_x[k5] << 8;
                int k8 = ((plane_x[y] - plane_x[k5]) << 8) / (j3 - j4);
                int i10 = vertex_shade[k5] << 8;
                int i11 =
                    ((vertex_shade[y] - vertex_shade[k5]) << 8) / (j3 - j4);

                if (j4 < 0) {
                    i7 -= k8 * j4;
                    i10 -= i11 * j4;
                    j4 = 0;
                }

                if (j3 > scene->max_y) {
                    j3 = scene->max_y;
                }

                for (int i12 = j4; i12 <= j3; i12++) {
                    Scanline *scanline_5 = scene->scanlines[i12];

                    if (i7 < scanline_5->start_x) {
                        scanline_5->start_x = i7;
                        scanline_5->start_s = i10;
                    }

                    if (i7 > scanline_5->end_x) {
                        scanline_5->end_x = i7;
                        scanline_5->end_s = i10;
                    }

                    i7 += k8;
                    i10 += i11;
                }
            }
        }

        if (scene->min_y < scene->base_y - scene->clip_y) {
            scene->min_y = scene->base_y - scene->clip_y;
        }
    }

    if (scene->mouse_picking_active &&
        scene->mouse_picked_count < MOUSE_PICKED_MAX &&
        scene->mouse_y >= scene->min_y && scene->mouse_y < scene->max_y) {
        Scanline *scanline_1 = scene->scanlines[scene->mouse_y];

        if (scene->mouse_x >= scanline_1->start_x >> 8 &&
            scene->mouse_x <= scanline_1->end_x >> 8 &&
            scanline_1->start_x <= scanline_1->end_x &&
            !game_model->unpickable && game_model->is_local_player[face] == 0) {
            scene->mouse_picked_models[scene->mouse_picked_count] = game_model;
            scene->mouse_picked_faces[scene->mouse_picked_count] = face;
            scene->mouse_picked_count++;
        }
    }
}

void scene_rasterize(Scene *scene, int num_vertices, int32_t *vertices_x,
                     int32_t *vertices_y, int32_t *vertices_z, int face_fill,
                     GameModel *game_model) {
    if (face_fill == -2) {
        return;
    }

    // face_fill's > 0 are textures, < 0 map to RGB
    if (face_fill >= 0) {
        if (face_fill >= scene->texture_count) {
            face_fill = 0;
        }

        scene_prepare_texture(scene, face_fill);

        int vertex_x = vertices_x[0];
        int vertex_y = vertices_y[0];
        int vertex_z = vertices_z[0];
        int i3 = vertex_x - vertices_x[1];
        int k3 = vertex_y - vertices_y[1];
        int i4 = vertex_z - vertices_z[1];

        num_vertices--;

        int i6 = vertices_x[num_vertices] - vertex_x;
        int j7 = vertices_y[num_vertices] - vertex_y;
        int k8 = vertices_z[num_vertices] - vertex_z;

        if (scene->texture_dimension[face_fill] == 1) {
            int l9 = (i6 * vertex_y - j7 * vertex_x) << 12;

            int k10 = (j7 * vertex_z - k8 * vertex_y)
                      << (5 - scene->view_distance + 7 + 4);

            int i11 = (k8 * vertex_x - i6 * vertex_z)
                      << (5 - scene->view_distance + 7);

            int k11 = (i3 * vertex_y - k3 * vertex_x) << 12;

            int i12 = (k3 * vertex_z - i4 * vertex_y)
                      << (5 - scene->view_distance + 7 + 4);

            int k12 = (i4 * vertex_x - i3 * vertex_z)
                      << (5 - scene->view_distance + 7);

            int i13 = (k3 * i6 - i3 * j7) << 5;
            int k13 = (i4 * j7 - k3 * k8) << (5 - scene->view_distance + 4);
            int i14 = (i3 * k8 - i4 * i6) >> (scene->view_distance - 5);
            int k14 = k10 >> 4;
            int i15 = i12 >> 4;
            int k15 = k13 >> 4;
            int i16 = scene->min_y - scene->base_y;
            int k16 = scene->width;
            int i17 = scene->base_x + scene->min_y * k16;
            int8_t scanline_inc = 1;
            l9 += i11 * i16;
            k11 += k12 * i16;
            i13 += i14 * i16;

            if (scene->interlace) {
                if ((scene->min_y & 1) == 1) {
                    scene->min_y++;
                    l9 += i11;
                    k11 += k12;
                    i13 += i14;
                    i17 += k16;
                }

                i11 <<= 1;
                k12 <<= 1;
                i14 <<= 1;
                k16 <<= 1;
                scanline_inc = 2;
            }

            if (game_model->texture_translucent) {
                for (int i = scene->min_y; i < scene->max_y;
                     i += scanline_inc) {
                    Scanline *scanline_3 = scene->scanlines[i];
                    int j = scanline_3->start_x >> 8;
                    int k17 = scanline_3->end_x >> 8;
                    int k20 = k17 - j;

                    if (k20 <= 0) {
                        l9 += i11;
                        k11 += k12;
                        i13 += i14;
                        i17 += k16;
                    } else {
                        int start_s = scanline_3->start_s;
                        int k23 = (scanline_3->end_s - start_s) / k20;

                        if (j < -scene->clip_x) {
                            start_s += (-scene->clip_x - j) * k23;
                            j = -scene->clip_x;
                            k20 = k17 - j;
                        }

                        if (k17 > scene->clip_x) {
                            int l17 = scene->clip_x;
                            k20 = l17 - j;
                        }

                        scene_texture_translucent_scanline(
                            scene->raster, scene->texture_pixels[face_fill],
                            l9 + k14 * j, k11 + i15 * j, i13 + k15 * j, k10,
                            i12, k13, k20, i17 + j, start_s, k23 << 2);

                        l9 += i11;
                        k11 += k12;
                        i13 += i14;
                        i17 += k16;
                    }
                }

                return;
            }

            if (!scene->texture_back_transparent[face_fill]) {
                for (int i = scene->min_y; i < scene->max_y;
                     i += scanline_inc) {
                    Scanline *scanline_4 = scene->scanlines[i];
                    int j = scanline_4->start_x >> 8;
                    int i18 = scanline_4->end_x >> 8;
                    int l20 = i18 - j;

                    if (l20 <= 0) {
                        l9 += i11;
                        k11 += k12;
                        i13 += i14;
                        i17 += k16;
                    } else {
                        int j22 = scanline_4->start_s;
                        int l23 = (scanline_4->end_s - j22) / l20;

                        if (j < -scene->clip_x) {
                            j22 += (-scene->clip_x - j) * l23;
                            j = -scene->clip_x;
                            l20 = i18 - j;
                        }

                        if (i18 > scene->clip_x) {
                            int j18 = scene->clip_x;
                            l20 = j18 - j;
                        }

                        scene_texture_scanline(
                            scene->raster, scene->texture_pixels[face_fill],
                            l9 + k14 * j, k11 + i15 * j, i13 + k15 * j, k10,
                            i12, k13, l20, i17 + j, j22, l23 << 2);

                        l9 += i11;
                        k11 += k12;
                        i13 += i14;
                        i17 += k16;
                    }
                }

                return;
            }

            for (int i = scene->min_y; i < scene->max_y; i += scanline_inc) {
                Scanline *scanline_5 = scene->scanlines[i];
                int j = scanline_5->start_x >> 8;
                int k18 = scanline_5->end_x >> 8;
                int i21 = k18 - j;

                if (i21 <= 0) {
                    l9 += i11;
                    k11 += k12;
                    i13 += i14;
                    i17 += k16;
                } else {
                    int k22 = scanline_5->start_s;
                    int i24 = (scanline_5->end_s - k22) / i21;

                    if (j < -scene->clip_x) {
                        k22 += (-scene->clip_x - j) * i24;
                        j = -scene->clip_x;
                        i21 = k18 - j;
                    }

                    if (k18 > scene->clip_x) {
                        int l18 = scene->clip_x;
                        i21 = l18 - j;
                    }

                    scene_texture_back_translucent_scanline(
                        scene->raster, scene->texture_pixels[face_fill],
                        l9 + k14 * j, k11 + i15 * j, i13 + k15 * j, k10, i12,
                        k13, i21, i17 + j, k22, i24);

                    l9 += i11;
                    k11 += k12;
                    i13 += i14;
                    i17 += k16;
                }
            }

            return;
        }

        int i10 = (i6 * vertex_y - j7 * vertex_x) << 11;

        int l10 = (j7 * vertex_z - k8 * vertex_y)
                  << (5 - scene->view_distance + 6 + 4);

        int j11 = (k8 * vertex_x - i6 * vertex_z)
                  << (5 - scene->view_distance + 6);

        int l11 = (i3 * vertex_y - k3 * vertex_x) << 11;

        int j12 = (k3 * vertex_z - i4 * vertex_y)
                  << (5 - scene->view_distance + 6 + 4);

        int l12 = (i4 * vertex_x - i3 * vertex_z)
                  << (5 - scene->view_distance + 6);

        int j13 = (k3 * i6 - i3 * j7) << 5;
        int l13 = (i4 * j7 - k3 * k8) << (5 - scene->view_distance + 4);
        int j14 = (i3 * k8 - i4 * i6) >> (scene->view_distance - 5);
        int l14 = l10 >> 4;
        int j15 = j12 >> 4;
        int l15 = l13 >> 4;
        int j16 = scene->min_y - scene->base_y;
        int l16 = scene->width;
        int j17 = scene->base_x + scene->min_y * l16;
        int8_t scanline_inc = 1;

        i10 += j11 * j16;
        l11 += l12 * j16;
        j13 += j14 * j16;

        if (scene->interlace) {
            if ((scene->min_y & 1) == 1) {
                scene->min_y++;
                i10 += j11;
                l11 += l12;
                j13 += j14;
                j17 += l16;
            }

            j11 <<= 1;
            l12 <<= 1;
            j14 <<= 1;
            l16 <<= 1;
            scanline_inc = 2;
        }

        if (game_model->texture_translucent) {
            for (int i = scene->min_y; i < scene->max_y; i += scanline_inc) {
                Scanline *scanline_6 = scene->scanlines[i];
                int j = scanline_6->start_x >> 8;
                int i19 = scanline_6->end_x >> 8;
                int j21 = i19 - j;

                if (j21 <= 0) {
                    i10 += j11;
                    l11 += l12;
                    j13 += j14;
                    j17 += l16;
                } else {
                    int l22 = scanline_6->start_s;
                    int j24 = (scanline_6->end_s - l22) / j21;

                    if (j < -scene->clip_x) {
                        l22 += (-scene->clip_x - j) * j24;
                        j = -scene->clip_x;
                        j21 = i19 - j;
                    }

                    if (i19 > scene->clip_x) {
                        int j19 = scene->clip_x;
                        j21 = j19 - j;
                    }

                    scene_texture_translucent_scanline2(
                        scene->raster, scene->texture_pixels[face_fill],
                        i10 + l14 * j, l11 + j15 * j, j13 + l15 * j, l10, j12,
                        l13, j21, j17 + j, l22, j24);

                    i10 += j11;
                    l11 += l12;
                    j13 += j14;
                    j17 += l16;
                }
            }

            return;
        }

        if (!scene->texture_back_transparent[face_fill]) {
            for (int i = scene->min_y; i < scene->max_y; i += scanline_inc) {
                Scanline *scanline_7 = scene->scanlines[i];
                int j = scanline_7->start_x >> 8;
                int k19 = scanline_7->end_x >> 8;
                int k21 = k19 - j;

                if (k21 <= 0) {
                    i10 += j11;
                    l11 += l12;
                    j13 += j14;
                    j17 += l16;
                } else {
                    int i23 = scanline_7->start_s;
                    int k24 = (scanline_7->end_s - i23) / k21;

                    if (j < -scene->clip_x) {
                        i23 += (-scene->clip_x - j) * k24;
                        j = -scene->clip_x;
                        k21 = k19 - j;
                    }
                    if (k19 > scene->clip_x) {
                        int l19 = scene->clip_x;
                        k21 = l19 - j;
                    }

                    scene_texture_scanline2(
                        scene->raster, scene->texture_pixels[face_fill],
                        i10 + l14 * j, l11 + j15 * j, j13 + l15 * j, l10, j12,
                        l13, k21, j17 + j, i23, k24);

                    i10 += j11;
                    l11 += l12;
                    j13 += j14;
                    j17 += l16;
                }
            }

            return;
        }

        for (int i = scene->min_y; i < scene->max_y; i += scanline_inc) {
            Scanline *scanline = scene->scanlines[i];
            int j = scanline->start_x >> 8;
            int i20 = scanline->end_x >> 8;
            int l21 = i20 - j;

            if (l21 <= 0) {
                i10 += j11;
                l11 += l12;
                j13 += j14;
                j17 += l16;
            } else {
                int j23 = scanline->start_s;
                int l24 = (scanline->end_s - j23) / l21;

                if (j < -scene->clip_x) {
                    j23 += (-scene->clip_x - j) * l24;
                    j = -scene->clip_x;
                    l21 = i20 - j;
                }

                if (i20 > scene->clip_x) {
                    int j20 = scene->clip_x;
                    l21 = j20 - j;
                }

                scene_texture_back_translucent_scanline2(
                    scene->raster, scene->texture_pixels[face_fill],
                    i10 + l14 * j, l11 + j15 * j, j13 + l15 * j, l10, j12, l13,
                    l21, j17 + j, j23, l24);

                i10 += j11;
                l11 += l12;
                j13 += j14;
                j17 += l16;
            }
        }

        return;
    }

    for (int i = 0; i < RAMP_COUNT; i++) {
        if (scene->gradient_base[i] == face_fill) {
            scene->gradient_ramp = scene->gradient_ramps[i];
            break;
        }

        if (i == RAMP_COUNT - 1) {
            int l1 = ((float)rand() / (float)RAND_MAX) * RAMP_COUNT;

            scene->gradient_base[l1] = face_fill;
            face_fill = -1 - face_fill;

            int r = ((face_fill >> 10) & 0x1f) * 8;
            int g = ((face_fill >> 5) & 0x1f) * 8;
            int b = (face_fill & 0x1f) * 8;

            for (int j4 = 0; j4 < 256; j4++) {
                int j6 = j4 * j4;
                int k7 = (r * j6) / 0x10000;
                int l8 = (g * j6) / 0x10000;
                int i0 = (b * j6) / 0x10000;

                scene->gradient_ramps[l1][255 - j4] =
                    (k7 << 16) + (l8 << 8) + i0;
            }

            scene->gradient_ramp = scene->gradient_ramps[l1];
        }
    }

    int i2 = scene->width;
    int l2 = scene->base_x + scene->min_y * i2;
    int8_t scanline_inc = 1;

    if (scene->interlace) {
        if ((scene->min_y & 1) == 1) {
            scene->min_y++;
            l2 += i2;
        }

        i2 <<= 1;
        scanline_inc = 2;
    }

    if (game_model->transparent) {
        for (int i = scene->min_y; i < scene->max_y; i += scanline_inc) {
            Scanline *scanline = scene->scanlines[i];
            int j = scanline->start_x >> 8;
            int k4 = scanline->end_x >> 8;
            int k6 = k4 - j;

            if (k6 <= 0) {
                l2 += i2;
            } else {
                int l7 = scanline->start_s;
                int i9 = (scanline->end_s - l7) / k6;

                if (j < -scene->clip_x) {
                    l7 += (-scene->clip_x - j) * i9;
                    j = -scene->clip_x;
                    k6 = k4 - j;
                }

                if (k4 > scene->clip_x) {
                    int l4 = scene->clip_x;
                    k6 = l4 - j;
                }

                scene_gradient_translucent_scanline(
                    scene->raster, -k6, l2 + j, scene->gradient_ramp, l7, i9);

                l2 += i2;
            }
        }

        return;
    }

    for (int i = scene->min_y; i < scene->max_y; i += scanline_inc) {
        Scanline *scanline_2 = scene->scanlines[i];
        int j = scanline_2->start_x >> 8;
        int k5 = scanline_2->end_x >> 8;
        int i7 = k5 - j;

        if (i7 <= 0) {
            l2 += i2;
        } else {
            int j8 = scanline_2->start_s;
            int k9 = (scanline_2->end_s - j8) / i7;

            if (j < -scene->clip_x) {
                j8 += (-scene->clip_x - j) * k9;
                j = -scene->clip_x;
                i7 = k5 - j;
            }

            if (k5 > scene->clip_x) {
                int l5 = scene->clip_x;
                i7 = l5 - j;
            }

            scene_gradient_scanline(scene->raster, -i7, l2 + j,
                                    scene->gradient_ramp, j8, k9);

            l2 += i2;
        }
    }
}

void scene_set_camera(Scene *scene, int x, int z, int y, int pitch, int yaw,
                      int roll, int distance) {
    pitch &= 1023;
    yaw &= 1023;
    roll &= 1023;

    scene->camera_yaw = (1024 - pitch) & 1023;
    scene->camera_pitch = (1024 - yaw) & 1023;
    scene->camera_roll = (1024 - roll) & 1023;

    // printf("%d %d %d\n", scene->camera_yaw, scene->camera_pitch,
    // scene->camera_roll);

    int l1 = 0;
    int i2 = 0;
    int j2 = distance;

    if (pitch != 0) {
        int k2 = sin_cos_2048[pitch];
        int j3 = sin_cos_2048[pitch + 1024];
        int i4 = (i2 * j3 - j2 * k2) >> 15;
        j2 = (i2 * k2 + j2 * j3) >> 15;
        i2 = i4;
    }

    if (yaw != 0) {
        int l2 = sin_cos_2048[yaw];
        int k3 = sin_cos_2048[yaw + 1024];
        int j4 = (j2 * l2 + l1 * k3) >> 15;
        j2 = (j2 * k3 - l1 * l2) >> 15;
        l1 = j4;
    }

    if (roll != 0) {
        int i3 = sin_cos_2048[roll];
        int l3 = sin_cos_2048[roll + 1024];
        int k4 = (i2 * i3 + l1 * l3) >> 15;
        i2 = (i2 * l3 - l1 * i3) >> 15;
        l1 = k4;
    }

    scene->camera_x = x - l1;
    scene->camera_y = z - i2;
    scene->camera_z = y - j2;
}

void scene_initialise_polygon_3d(Scene *scene, int i) {
    GamePolygon *polygon = scene->visible_polygons[i];
    GameModel *game_model = polygon->model;
    int face = polygon->face;
    int *face_vertices = game_model->face_vertices[face];
    int face_num_vertices = game_model->face_num_vertices[face];
    int face_camera_normal_scale = game_model->normal_scale[face];
    int vcx = game_model->project_vertex_x[face_vertices[0]];
    int vcy = game_model->project_vertex_y[face_vertices[0]];
    int vcz = game_model->project_vertex_z[face_vertices[0]];
    int vcx1 = game_model->project_vertex_x[face_vertices[1]] - vcx;
    int vcy1 = game_model->project_vertex_y[face_vertices[1]] - vcy;
    int vcz1 = game_model->project_vertex_z[face_vertices[1]] - vcz;
    int vcx2 = game_model->project_vertex_x[face_vertices[2]] - vcx;
    int vcy2 = game_model->project_vertex_y[face_vertices[2]] - vcy;
    int vcz2 = game_model->project_vertex_z[face_vertices[2]] - vcz;
    int t1 = vcy1 * vcz2 - vcy2 * vcz1;
    int t2 = vcz1 * vcx2 - vcz2 * vcx1;
    int t3 = vcx1 * vcy2 - vcx2 * vcy1;

    if (face_camera_normal_scale == -1) {
        face_camera_normal_scale = 0;

        for (; t1 > 25000 || t2 > 25000 || t3 > 25000 || t1 < -25000 ||
               t2 < -25000 || t3 < -25000;
             t3 >>= 1) {
            face_camera_normal_scale++;
            t1 >>= 1;
            t2 >>= 1;
        }

        game_model->normal_scale[face] = face_camera_normal_scale;

        game_model->normal_magnitude[face] =
            (int)(scene->normal_magnitude * sqrt(t1 * t1 + t2 * t2 + t3 * t3));
    } else {
        t1 >>= face_camera_normal_scale;
        t2 >>= face_camera_normal_scale;
        t3 >>= face_camera_normal_scale;
    }

    polygon->visibility = vcx * t1 + vcy * t2 + vcz * t3;
    polygon->normal_x = t1;
    polygon->normal_y = t2;
    polygon->normal_z = t3;

    int j4 = game_model->project_vertex_z[face_vertices[0]];
    int k4 = j4;
    int l4 = game_model->vertex_view_x[face_vertices[0]];
    int i5 = l4;
    int j5 = game_model->vertex_view_y[face_vertices[0]];
    int k5 = j5;

    for (int l5 = 1; l5 < face_num_vertices; l5++) {
        int i1 = game_model->project_vertex_z[face_vertices[l5]];

        if (i1 > k4) {
            k4 = i1;
        } else if (i1 < j4) {
            j4 = i1;
        }

        i1 = game_model->vertex_view_x[face_vertices[l5]];

        if (i1 > i5) {
            i5 = i1;
        } else if (i1 < l4) {
            l4 = i1;
        }

        i1 = game_model->vertex_view_y[face_vertices[l5]];

        if (i1 > k5) {
            k5 = i1;
        } else if (i1 < j5) {
            j5 = i1;
        }
    }

    polygon->min_z = j4;
    polygon->max_z = k4;
    polygon->min_plane_x = l4;
    polygon->max_plane_x = i5;
    polygon->min_plane_y = j5;
    polygon->max_plane_y = k5;
}

void scene_initialise_polygon_2d(Scene *scene, int i) {
    GamePolygon *polygon = scene->visible_polygons[i];
    GameModel *game_model = polygon->model;
    int j = polygon->face;
    int *ai = game_model->face_vertices[j];
    int l = 0;
    int i1 = 0;
    int j1 = 1;
    int k1 = game_model->project_vertex_x[ai[0]];
    int l1 = game_model->project_vertex_y[ai[0]];
    int i2 = game_model->project_vertex_z[ai[0]];

    game_model->normal_magnitude[j] = 1;
    game_model->normal_scale[j] = 0;
    polygon->visibility = k1 * l + l1 * i1 + i2 * j1;
    polygon->normal_x = l;
    polygon->normal_y = i1;
    polygon->normal_z = j1;

    int j2 = game_model->project_vertex_z[ai[0]];
    int k2 = j2;
    int l2 = game_model->vertex_view_x[ai[0]];
    int i3 = l2;

    if (game_model->vertex_view_x[ai[1]] < l2) {
        l2 = game_model->vertex_view_x[ai[1]];
    } else {
        i3 = game_model->vertex_view_x[ai[1]];
    }

    int j3 = game_model->vertex_view_y[ai[1]];
    int k3 = game_model->vertex_view_y[ai[0]];
    int k = game_model->project_vertex_z[ai[1]];

    if (k > k2) {
        k2 = k;
    } else if (k < j2) {
        j2 = k;
    }

    k = game_model->vertex_view_x[ai[1]];

    if (k > i3) {
        i3 = k;
    } else if (k < l2) {
        l2 = k;
    }

    k = game_model->vertex_view_y[ai[1]];

    if (k > k3) {
        k3 = k;
    } else if (k < j3) {
        j3 = k;
    }

    polygon->min_z = j2;
    polygon->max_z = k2;
    polygon->min_plane_x = l2 - 20;
    polygon->max_plane_x = i3 + 20;
    polygon->min_plane_y = j3;
    polygon->max_plane_y = k3;
}

int scene_separate_polygon(GamePolygon *polygon_a, GamePolygon *polygon_b) {
    if (polygon_a->min_plane_x >= polygon_b->max_plane_x) {
        return 1;
    }

    if (polygon_b->min_plane_x >= polygon_a->max_plane_x) {
        return 1;
    }

    if (polygon_a->min_plane_y >= polygon_b->max_plane_y) {
        return 1;
    }

    if (polygon_b->min_plane_y >= polygon_a->max_plane_y) {
        return 1;
    }

    if (polygon_a->min_z >= polygon_b->max_z) {
        return 1;
    }

    if (polygon_b->min_z > polygon_a->max_z) {
        return 0;
    }

    GameModel *game_model_a = polygon_a->model;
    GameModel *game_model_b = polygon_b->model;
    int face_a = polygon_a->face;
    int face_b = polygon_b->face;
    int *face_vertices_a = game_model_a->face_vertices[face_a];
    int *face_vertices_b = game_model_b->face_vertices[face_b];
    int face_num_vertices_a = game_model_a->face_num_vertices[face_a];
    int face_num_vertices_b = game_model_b->face_num_vertices[face_b];

    int first_project_x = game_model_b->project_vertex_x[face_vertices_b[0]];
    int first_project_y = game_model_b->project_vertex_y[face_vertices_b[0]];
    int first_project_z = game_model_b->project_vertex_z[face_vertices_b[0]];
    int normal_x = polygon_b->normal_x;
    int normal_y = polygon_b->normal_y;
    int normal_z = polygon_b->normal_z;
    int normal_magnitude = game_model_b->normal_magnitude[face_b];
    int visibility = polygon_b->visibility;
    int flag = 0;

    for (int i = 0; i < face_num_vertices_a; i++) {
        int vertex_index = face_vertices_a[i];

        int magnitude =
            (first_project_x - game_model_a->project_vertex_x[vertex_index]) *
                normal_x +
            (first_project_y - game_model_a->project_vertex_y[vertex_index]) *
                normal_y +
            (first_project_z - game_model_a->project_vertex_z[vertex_index]) *
                normal_z;

        if ((magnitude >= -normal_magnitude || visibility >= 0) &&
            (magnitude <= normal_magnitude || visibility <= 0)) {
            continue;
        }

        flag = 1;
        break;
    }

    if (!flag) {
        return 1;
    }

    first_project_x = game_model_a->project_vertex_x[face_vertices_a[0]];
    first_project_y = game_model_a->project_vertex_y[face_vertices_a[0]];
    first_project_z = game_model_a->project_vertex_z[face_vertices_a[0]];
    normal_x = polygon_a->normal_x;
    normal_y = polygon_a->normal_y;
    normal_z = polygon_a->normal_z;
    normal_magnitude = game_model_a->normal_magnitude[face_a];
    visibility = polygon_a->visibility;
    flag = 0;

    for (int i = 0; i < face_num_vertices_b; i++) {
        int vertex_index = face_vertices_b[i];

        int magnitude =
            (first_project_x - game_model_b->project_vertex_x[vertex_index]) *
                normal_x +
            (first_project_y - game_model_b->project_vertex_y[vertex_index]) *
                normal_y +
            (first_project_z - game_model_b->project_vertex_z[vertex_index]) *
                normal_z;

        if ((magnitude >= -normal_magnitude || visibility <= 0) &&
            (magnitude <= normal_magnitude || visibility >= 0)) {
            continue;
        }

        flag = 1;
        break;
    }

    if (!flag) {
        return 1;
    }

    int *vertex_view_x_a = NULL;
    int *vertex_view_y_a = NULL;
    int length_a = 0;

    if (face_num_vertices_a == 2) {
        length_a = 4;
        vertex_view_x_a = alloca(length_a * sizeof(int));
        vertex_view_y_a = alloca(length_a * sizeof(int));
        int first_vertex_index = face_vertices_a[0];
        int second_vertex_index = face_vertices_a[1];
        vertex_view_x_a[0] =
            game_model_a->vertex_view_x[first_vertex_index] - 20;
        vertex_view_x_a[1] =
            game_model_a->vertex_view_x[second_vertex_index] - 20;
        vertex_view_x_a[2] =
            game_model_a->vertex_view_x[second_vertex_index] + 20;
        vertex_view_x_a[3] =
            game_model_a->vertex_view_x[first_vertex_index] + 20;
        vertex_view_y_a[0] = vertex_view_y_a[3] =
            game_model_a->vertex_view_y[first_vertex_index];
        vertex_view_y_a[1] = vertex_view_y_a[2] =
            game_model_a->vertex_view_y[second_vertex_index];
    } else {
        vertex_view_x_a = alloca(face_num_vertices_a * sizeof(int));
        vertex_view_y_a = alloca(face_num_vertices_a * sizeof(int));

        for (int i = 0; i < face_num_vertices_a; i++) {
            int vertex_index = face_vertices_a[i];
            vertex_view_x_a[i] = game_model_a->vertex_view_x[vertex_index];
            vertex_view_y_a[i] = game_model_a->vertex_view_y[vertex_index];
        }

        length_a = face_num_vertices_a;
    }

    int *vertex_view_x_b = NULL;
    int *vertex_view_y_b = NULL;
    int length_b = 0;

    if (face_num_vertices_b == 2) {
        length_b = 4;
        vertex_view_x_b = alloca(length_b * sizeof(int));
        vertex_view_y_b = alloca(length_b * sizeof(int));
        int k5 = face_vertices_b[0];
        int l1 = face_vertices_b[1];
        vertex_view_x_b[0] = game_model_b->vertex_view_x[k5] - 20;
        vertex_view_x_b[1] = game_model_b->vertex_view_x[l1] - 20;
        vertex_view_x_b[2] = game_model_b->vertex_view_x[l1] + 20;
        vertex_view_x_b[3] = game_model_b->vertex_view_x[k5] + 20;
        vertex_view_y_b[0] = vertex_view_y_b[3] =
            game_model_b->vertex_view_y[k5];
        vertex_view_y_b[1] = vertex_view_y_b[2] =
            game_model_b->vertex_view_y[l1];
    } else {
        vertex_view_x_b = alloca(face_num_vertices_b * sizeof(int));
        vertex_view_y_b = alloca(face_num_vertices_b * sizeof(int));

        for (int i = 0; i < face_num_vertices_b; i++) {
            int j6 = face_vertices_b[i];
            vertex_view_x_b[i] = game_model_b->vertex_view_x[j6];
            vertex_view_y_b[i] = game_model_b->vertex_view_y[j6];
        }

        length_b = face_num_vertices_b;
    }

    return !scene_intersect(vertex_view_x_a, vertex_view_y_a, vertex_view_x_b,
                            vertex_view_y_b, length_a, length_b);
}

int scene_heuristic_polygon(GamePolygon *polygon, GamePolygon *polygon_1) {
    GameModel *game_model = polygon->model;
    GameModel *game_model_1 = polygon_1->model;
    int i = polygon->face;
    int j = polygon_1->face;
    int *ai = game_model->face_vertices[i];
    int *ai1 = game_model_1->face_vertices[j];
    int k = game_model->face_num_vertices[i];
    int l = game_model_1->face_num_vertices[j];
    int i2 = game_model_1->project_vertex_x[ai1[0]];
    int j2 = game_model_1->project_vertex_y[ai1[0]];
    int k2 = game_model_1->project_vertex_z[ai1[0]];
    int l2 = polygon_1->normal_x;
    int i3 = polygon_1->normal_y;
    int j3 = polygon_1->normal_z;
    int k3 = game_model_1->normal_magnitude[j];
    int l3 = polygon_1->visibility;
    int flag = 0;

    for (int i4 = 0; i4 < k; i4++) {
        int i1 = ai[i4];

        int k1 = (i2 - game_model->project_vertex_x[i1]) * l2 +
                 (j2 - game_model->project_vertex_y[i1]) * i3 +
                 (k2 - game_model->project_vertex_z[i1]) * j3;

        if ((k1 >= -k3 || l3 >= 0) && (k1 <= k3 || l3 <= 0)) {
            continue;
        }
        flag = 1;

        break;
    }

    if (!flag) {
        return 1;
    }

    i2 = game_model->project_vertex_x[ai[0]];
    j2 = game_model->project_vertex_y[ai[0]];
    k2 = game_model->project_vertex_z[ai[0]];
    l2 = polygon->normal_x;
    i3 = polygon->normal_y;
    j3 = polygon->normal_z;
    k3 = game_model->normal_magnitude[i];
    l3 = polygon->visibility;
    flag = 0;

    for (int j4 = 0; j4 < l; j4++) {
        int j1 = ai1[j4];

        int l1 = (i2 - game_model_1->project_vertex_x[j1]) * l2 +
                 (j2 - game_model_1->project_vertex_y[j1]) * i3 +
                 (k2 - game_model_1->project_vertex_z[j1]) * j3;

        if ((l1 >= -k3 || l3 <= 0) && (l1 <= k3 || l3 >= 0)) {
            continue;
        }

        flag = 1;
        break;
    }

    return !flag;
}

void scene_allocate_textures(Scene *scene, int count, int length_64,
                             int length_128) {
    scene->texture_count = count;
    scene->texture_colours_used = calloc(count, sizeof(int8_t *));
    scene->texture_colour_list = calloc(count, sizeof(int32_t *));
    scene->texture_loaded_number = calloc(count, sizeof(int64_t));
    scene->texture_dimension = calloc(count, sizeof(int8_t));
    scene->texture_back_transparent = calloc(count, sizeof(int8_t));
    scene->texture_pixels = calloc(count, sizeof(int32_t *));

    scene_texture_count_loaded = 0;

    for (int i = 0; i < count; i++) {
        scene->texture_loaded_number[i] = 0;
    }

    /* 64x64 rgba */
    scene->texture_colours_64 = calloc(length_64, sizeof(int32_t *));
    scene->length_64 = length_64;

    /* 128x128 rgba */
    scene->texture_colours_128 = calloc(length_128, sizeof(int32_t *));
    scene->length_128 = length_128;
}

void scene_define_texture(Scene *scene, int id, int8_t *colour_idx,
                          int32_t *colours, int wide128) {
    scene->texture_colours_used[id] = colour_idx;
    scene->texture_colour_list[id] = colours;

    /* is 1 if the scene->texture is 128+ pixels wide, 0 if < 128 */
    scene->texture_dimension[id] = wide128;

    scene->texture_loaded_number[id] = 0;
    scene->texture_back_transparent[id] = 0;
    scene->texture_pixels[id] = NULL;

    scene_prepare_texture(scene, id);
}

void scene_prepare_texture(Scene *scene, int id) {
    if (id < 0) {
        return;
    }

    scene_texture_count_loaded++;
    scene->texture_loaded_number[id] = scene_texture_count_loaded;

    if (scene->texture_pixels[id] != NULL) {
        return;
    }

    if (scene->texture_dimension[id] == 0) {
        // is 64 pixels wide
        for (int i = 0; i < scene->length_64; i++) {
            if (scene->texture_colours_64[i] == NULL) {
                scene->texture_colours_64[i] =
                    calloc(128 * 128, sizeof(int32_t));

                scene->texture_pixels[id] = scene->texture_colours_64[i];
                scene_set_texture_pixels(scene, id);
                return;
            }
        }

        int max_id = 1 << 30;
        int old_id = 0;

        for (int i = 0; i < scene->texture_count; i++) {
            if (i != id && scene->texture_dimension[i] == 0 &&
                scene->texture_pixels[i] != NULL &&
                scene->texture_loaded_number[i] < max_id) {
                max_id = scene->texture_loaded_number[i];
                old_id = i;
            }
        }

        free(scene->texture_pixels[id]);
        scene->texture_pixels[id] = scene->texture_pixels[old_id];
        scene->texture_pixels[old_id] = NULL;

        scene_set_texture_pixels(scene, id);
        return;
    }

    // is 128 wide
    for (int i = 0; i < scene->length_128; i++) {
        if (scene->texture_colours_128[i] == NULL) {
            scene->texture_colours_128[i] = calloc(256 * 256, sizeof(int32_t));

            scene->texture_pixels[id] = scene->texture_colours_128[i];
            scene_set_texture_pixels(scene, id);
            return;
        }
    }

    int max_id = 1 << 30;
    int old_id = 0;

    for (int i = 0; i < scene->texture_count; i++) {
        if (i != id && scene->texture_dimension[i] == 1 &&
            scene->texture_pixels[i] != NULL &&
            scene->texture_loaded_number[i] < max_id) {
            max_id = scene->texture_loaded_number[i];
            old_id = i;
        }
    }

    free(scene->texture_pixels[id]);
    scene->texture_pixels[id] = scene->texture_pixels[old_id];
    scene->texture_pixels[old_id] = NULL;
    scene_set_texture_pixels(scene, id);
}

void scene_set_texture_pixels(Scene *scene, int id) {
    int texture_width = 0;

    if (scene->texture_dimension[id] == 0) {
        texture_width = 64;
    } else {
        texture_width = 128;
    }

    int32_t *colours = scene->texture_pixels[id];
    int colour_count = 0;

    for (int x = 0; x < texture_width; x++) {
        for (int y = 0; y < texture_width; y++) {
            int colour =
                scene->texture_colour_list[id][scene->texture_colours_used
                                                   [id][y + x * texture_width] &
                                               0xff];

            colour &= 0xf8f8ff;

            if (colour == 0) {
                colour = 1;
            } else if (colour == 0xf800ff) {
                colour = 0;
                scene->texture_back_transparent[id] = 1;
            }

            colours[colour_count++] = colour;
        }
    }

    for (int i = 0; i < colour_count; i++) {
        int32_t colour = colours[i];

        colours[colour_count + i] = (colour - (colour >> 3)) & 0xf8f8ff;
        colours[colour_count * 2 + i] = (colour - (colour >> 2)) & 0xf8f8ff;

        colours[colour_count * 3 + i] =
            (colour - (colour >> 2) - (colour >> 3)) & 0xf8f8ff;
    }
}

void scene_scroll_texture(Scene *scene, int id) {
    if (scene->texture_pixels[id] == NULL) {
        return;
    }

    int32_t *colours = scene->texture_pixels[id];

    for (int i = 0; i < SCROLL_TEXTURE_SIZE; i++) {
        int pixel_index = i + SCROLL_TEXTURE_AREA - SCROLL_TEXTURE_SIZE;
        int colour = colours[pixel_index];

        for (int j = 0; j < SCROLL_TEXTURE_SIZE - 1; j++) {
            colours[pixel_index] = colours[pixel_index - SCROLL_TEXTURE_SIZE];
            pixel_index -= SCROLL_TEXTURE_SIZE;
        }

        scene->texture_pixels[id][pixel_index] = colour;
    }

    for (int i = 0; i < SCROLL_TEXTURE_AREA; i++) {
        int colour = colours[i];

        colours[SCROLL_TEXTURE_AREA + i] = (colour - (colour >> 3)) & 0xf8f8ff;

        colours[SCROLL_TEXTURE_AREA * 2 + i] =
            (colour - (colour >> 2)) & 0xf8f8ff;

        colours[SCROLL_TEXTURE_AREA * 3 + i] =
            (colour - (colour >> 2) - (colour >> 3)) & 0xf8f8ff;
    }
}

/* used to convert face_fill values (textures or colours) to minimap colours */
int scene_get_fill_colour(Scene *scene, int face_fill) {
    if (face_fill == COLOUR_TRANSPARENT) {
        return 0;
    }

    scene_prepare_texture(scene, face_fill);

    if (face_fill >= 0) {
        return scene->texture_pixels[face_fill][0];
    }

    if (face_fill < 0) {
        face_fill = -(face_fill + 1);

        int r = (face_fill >> 10) & 0x1f;
        int g = (face_fill >> 5) & 0x1f;
        int b = face_fill & 0x1f;

        return (r << 19) + (g << 11) + (b << 3);
    }

    return 0;
}

void scene_set_light_from3(Scene *scene, int x, int y, int z) {
    if (x == 0 && y == 0 && z == 0) {
        x = 32;
    }

    for (int i = 0; i < scene->model_count; i++) {
        game_model_set_light_from3(scene->models[i], x, y, z);
    }
}

void scene_set_light_from5(Scene *scene, int ambience, int diffuse, int x,
                           int y, int z) {
    if (x == 0 && y == 0 && z == 0) {
        x = 32;
    }

    for (int i = 0; i < scene->model_count; i++) {
        game_model_set_light_from5(scene->models[i], ambience, diffuse, x, y,
                                   z);
    }
}

int scene_method306(int i, int j, int k, int l, int i1) {
    if (l == j) {
        return i;
    }

    return i + (((k - i) * (i1 - j)) / (l - j));
}

int scene_method307(int i, int j, int k, int l, int flag) {
    if ((flag && i <= k) || i < k) {
        if (i > l) {
            return 1;
        }

        if (j > k) {
            return 1;
        }

        if (j > l) {
            return 1;
        }

        return !flag;
    }

    if (i < l) {
        return 1;
    }

    if (j < k) {
        return 1;
    }

    if (j < l) {
        return 1;
    }

    return flag;
}

int scene_method308(int i, int j, int k, int flag) {
    if ((flag && i <= k) || i < k) {
        if (j > k) {
            return 1;
        }

        return !flag;
    }

    if (j < k) {
        return 1;
    }

    return flag;
}

int scene_intersect(int *vertex_view_x_a, int *vertex_view_y_a,
                    int *vertex_view_x_b, int *vertex_view_y_b, int length_a,
                    int length_b) {
    int8_t byte0 = 0;
    int view_y_a = vertex_view_y_a[0];
    int k20 = view_y_a;
    int k = 0;
    int view_y_b = vertex_view_y_b[0];
    int l20 = view_y_b;
    int i1 = 0;

    for (int i = 1; i < length_a; i++) {
        if (vertex_view_y_a[i] < view_y_a) {
            view_y_a = vertex_view_y_a[i];
            k = i;
        } else if (vertex_view_y_a[i] > k20) {
            k20 = vertex_view_y_a[i];
        }
    }

    for (int i = 1; i < length_b; i++) {
        if (vertex_view_y_b[i] < view_y_b) {
            view_y_b = vertex_view_y_b[i];
            i1 = i;
        } else if (vertex_view_y_b[i] > l20) {
            l20 = vertex_view_y_b[i];
        }
    }

    if (view_y_b >= k20) {
        return 0;
    }

    if (view_y_a >= l20) {
        return 0;
    }

    int l = 0;
    int j1 = 0;
    int flag = 0;

    if (vertex_view_y_a[k] < vertex_view_y_b[i1]) {
        for (l = k; vertex_view_y_a[l] < vertex_view_y_b[i1];
             l = (l + 1) % length_a)
            ;

        for (; vertex_view_y_a[k] < vertex_view_y_b[i1];
             k = (k - 1 + length_a) % length_a)
            ;

        int k1 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                 vertex_view_y_a[(k + 1) % length_a],
                                 vertex_view_x_a[k], vertex_view_y_a[k],
                                 vertex_view_y_b[i1]);

        int k6 = scene_method306(vertex_view_x_a[(l - 1 + length_a) % length_a],
                                 vertex_view_y_a[(l - 1 + length_a) % length_a],
                                 vertex_view_x_a[l], vertex_view_y_a[l],
                                 vertex_view_y_b[i1]);

        int l10 = vertex_view_x_b[i1];
        flag = (k1 < l10) | (k6 < l10);

        if (scene_method308(k1, k6, l10, flag)) {
            return 1;
        }

        j1 = (i1 + 1) % length_b;
        i1 = (i1 - 1 + length_b) % length_b;

        if (k == l) {
            byte0 = 1;
        }
    } else {
        for (j1 = i1; vertex_view_y_b[j1] < vertex_view_y_a[k];
             j1 = (j1 + 1) % length_b)
            ;

        for (; vertex_view_y_b[i1] < vertex_view_y_a[k];
             i1 = (i1 - 1 + length_b) % length_b)
            ;

        int l1 = vertex_view_x_a[k];

        int i11 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                  vertex_view_y_b[(i1 + 1) % length_b],
                                  vertex_view_x_b[i1], vertex_view_y_b[i1],
                                  vertex_view_y_a[k]);

        int l15 = scene_method306(
            vertex_view_x_b[(j1 - 1 + length_b) % length_b],
            vertex_view_y_b[(j1 - 1 + length_b) % length_b],
            vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_a[k]);

        flag = (l1 < i11) | (l1 < l15);

        if (scene_method308(i11, l15, l1, !flag)) {
            return 1;
        }

        l = (k + 1) % length_a;
        k = (k - 1 + length_a) % length_a;

        if (i1 == j1) {
            byte0 = 2;
        }
    }

    while (byte0 == 0) {
        if (vertex_view_y_a[k] < vertex_view_y_a[l]) {
            if (vertex_view_y_a[k] < vertex_view_y_b[i1]) {
                if (vertex_view_y_a[k] < vertex_view_y_b[j1]) {
                    int i2 = vertex_view_x_a[k];

                    int l6 = scene_method306(
                        vertex_view_x_a[(l - 1 + length_a) % length_a],
                        vertex_view_y_a[(l - 1 + length_a) % length_a],
                        vertex_view_x_a[l], vertex_view_y_a[l],
                        vertex_view_y_a[k]);

                    int j11 = scene_method306(
                        vertex_view_x_b[(i1 + 1) % length_b],
                        vertex_view_y_b[(i1 + 1) % length_b],
                        vertex_view_x_b[i1], vertex_view_y_b[i1],
                        vertex_view_y_a[k]);

                    int i16 = scene_method306(
                        vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                        vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                        vertex_view_x_b[j1], vertex_view_y_b[j1],
                        vertex_view_y_a[k]);

                    if (scene_method307(i2, l6, j11, i16, flag)) {
                        return 1;
                    }

                    k = (k - 1 + length_a) % length_a;

                    if (k == l) {
                        byte0 = 1;
                    }
                } else {
                    int j2 = scene_method306(
                        vertex_view_x_a[(k + 1) % length_a],
                        vertex_view_y_a[(k + 1) % length_a], vertex_view_x_a[k],
                        vertex_view_y_a[k], vertex_view_y_b[j1]);

                    int i7 = scene_method306(
                        vertex_view_x_a[(l - 1 + length_a) % length_a],
                        vertex_view_y_a[(l - 1 + length_a) % length_a],
                        vertex_view_x_a[l], vertex_view_y_a[l],
                        vertex_view_y_b[j1]);

                    int k11 = scene_method306(
                        vertex_view_x_b[(i1 + 1) % length_b],
                        vertex_view_y_b[(i1 + 1) % length_b],
                        vertex_view_x_b[i1], vertex_view_y_b[i1],
                        vertex_view_y_b[j1]);

                    int j16 = vertex_view_x_b[j1];

                    if (scene_method307(j2, i7, k11, j16, flag)) {
                        return 1;
                    }

                    j1 = (j1 + 1) % length_b;

                    if (i1 == j1) {
                        byte0 = 2;
                    }
                }
            } else if (vertex_view_y_b[i1] < vertex_view_y_b[j1]) {
                int k2 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                         vertex_view_y_a[(k + 1) % length_a],
                                         vertex_view_x_a[k], vertex_view_y_a[k],
                                         vertex_view_y_b[i1]);

                int j7 = scene_method306(
                    vertex_view_x_a[(l - 1 + length_a) % length_a],
                    vertex_view_y_a[(l - 1 + length_a) % length_a],
                    vertex_view_x_a[l], vertex_view_y_a[l],
                    vertex_view_y_b[i1]);

                int l11 = vertex_view_x_b[i1];

                int k16 = scene_method306(
                    vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                    vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                    vertex_view_x_b[j1], vertex_view_y_b[j1],
                    vertex_view_y_b[i1]);

                if (scene_method307(k2, j7, l11, k16, flag)) {
                    return 1;
                }

                i1 = (i1 - 1 + length_b) % length_b;

                if (i1 == j1) {
                    byte0 = 2;
                }
            } else {
                int l2 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                         vertex_view_y_a[(k + 1) % length_a],
                                         vertex_view_x_a[k], vertex_view_y_a[k],
                                         vertex_view_y_b[j1]);

                int k7 = scene_method306(
                    vertex_view_x_a[(l - 1 + length_a) % length_a],
                    vertex_view_y_a[(l - 1 + length_a) % length_a],
                    vertex_view_x_a[l], vertex_view_y_a[l],
                    vertex_view_y_b[j1]);

                int i12 = scene_method306(
                    vertex_view_x_b[(i1 + 1) % length_b],
                    vertex_view_y_b[(i1 + 1) % length_b], vertex_view_x_b[i1],
                    vertex_view_y_b[i1], vertex_view_y_b[j1]);

                int l16 = vertex_view_x_b[j1];

                if (scene_method307(l2, k7, i12, l16, flag)) {
                    return 1;
                }

                j1 = (j1 + 1) % length_b;

                if (i1 == j1) {
                    byte0 = 2;
                }
            }
        } else if (vertex_view_y_a[l] < vertex_view_y_b[i1]) {
            if (vertex_view_y_a[l] < vertex_view_y_b[j1]) {
                int i3 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                         vertex_view_y_a[(k + 1) % length_a],
                                         vertex_view_x_a[k], vertex_view_y_a[k],
                                         vertex_view_y_a[l]);

                int l7 = vertex_view_x_a[l];

                int j12 = scene_method306(
                    vertex_view_x_b[(i1 + 1) % length_b],
                    vertex_view_y_b[(i1 + 1) % length_b], vertex_view_x_b[i1],
                    vertex_view_y_b[i1], vertex_view_y_a[l]);

                int i17 = scene_method306(
                    vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                    vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                    vertex_view_x_b[j1], vertex_view_y_b[j1],
                    vertex_view_y_a[l]);

                if (scene_method307(i3, l7, j12, i17, flag)) {
                    return 1;
                }

                l = (l + 1) % length_a;

                if (k == l) {
                    byte0 = 1;
                }
            } else {
                int j3 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                         vertex_view_y_a[(k + 1) % length_a],
                                         vertex_view_x_a[k], vertex_view_y_a[k],
                                         vertex_view_y_b[j1]);

                int i8 = scene_method306(
                    vertex_view_x_a[(l - 1 + length_a) % length_a],
                    vertex_view_y_a[(l - 1 + length_a) % length_a],
                    vertex_view_x_a[l], vertex_view_y_a[l],
                    vertex_view_y_b[j1]);

                int k12 = scene_method306(
                    vertex_view_x_b[(i1 + 1) % length_b],
                    vertex_view_y_b[(i1 + 1) % length_b], vertex_view_x_b[i1],
                    vertex_view_y_b[i1], vertex_view_y_b[j1]);

                int j17 = vertex_view_x_b[j1];

                if (scene_method307(j3, i8, k12, j17, flag)) {
                    return 1;
                }

                j1 = (j1 + 1) % length_b;

                if (i1 == j1) {
                    byte0 = 2;
                }
            }
        } else if (vertex_view_y_b[i1] < vertex_view_y_b[j1]) {
            int k3 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_b[i1]);

            int j8 = scene_method306(
                vertex_view_x_a[(l - 1 + length_a) % length_a],
                vertex_view_y_a[(l - 1 + length_a) % length_a],
                vertex_view_x_a[l], vertex_view_y_a[l], vertex_view_y_b[i1]);

            int l12 = vertex_view_x_b[i1];

            int k17 = scene_method306(
                vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_b[i1]);

            if (scene_method307(k3, j8, l12, k17, flag)) {
                return 1;
            }

            i1 = (i1 - 1 + length_b) % length_b;

            if (i1 == j1) {
                byte0 = 2;
            }
        } else {
            int l3 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_b[j1]);

            int k8 = scene_method306(
                vertex_view_x_a[(l - 1 + length_a) % length_a],
                vertex_view_y_a[(l - 1 + length_a) % length_a],
                vertex_view_x_a[l], vertex_view_y_a[l], vertex_view_y_b[j1]);

            int i13 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                      vertex_view_y_b[(i1 + 1) % length_b],
                                      vertex_view_x_b[i1], vertex_view_y_b[i1],
                                      vertex_view_y_b[j1]);

            int l17 = vertex_view_x_b[j1];

            if (scene_method307(l3, k8, i13, l17, flag)) {
                return 1;
            }

            j1 = (j1 + 1) % length_b;

            if (i1 == j1) {
                byte0 = 2;
            }
        }
    }

    while (byte0 == 1) {
        if (vertex_view_y_a[k] < vertex_view_y_b[i1]) {
            if (vertex_view_y_a[k] < vertex_view_y_b[j1]) {
                int i4 = vertex_view_x_a[k];

                int j13 = scene_method306(
                    vertex_view_x_b[(i1 + 1) % length_b],
                    vertex_view_y_b[(i1 + 1) % length_b], vertex_view_x_b[i1],
                    vertex_view_y_b[i1], vertex_view_y_a[k]);

                int i18 = scene_method306(
                    vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                    vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                    vertex_view_x_b[j1], vertex_view_y_b[j1],
                    vertex_view_y_a[k]);

                return scene_method308(j13, i18, i4, !flag);
            }

            int j4 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_b[j1]);

            int l8 = scene_method306(
                vertex_view_x_a[(l - 1 + length_a) % length_a],
                vertex_view_y_a[(l - 1 + length_a) % length_a],
                vertex_view_x_a[l], vertex_view_y_a[l], vertex_view_y_b[j1]);

            int k13 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                      vertex_view_y_b[(i1 + 1) % length_b],
                                      vertex_view_x_b[i1], vertex_view_y_b[i1],
                                      vertex_view_y_b[j1]);

            int j18 = vertex_view_x_b[j1];

            if (scene_method307(j4, l8, k13, j18, flag)) {
                return 1;
            }

            j1 = (j1 + 1) % length_b;

            if (i1 == j1) {
                byte0 = 0;
            }
        } else if (vertex_view_y_b[i1] < vertex_view_y_b[j1]) {
            int k4 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_b[i1]);

            int i9 = scene_method306(
                vertex_view_x_a[(l - 1 + length_a) % length_a],
                vertex_view_y_a[(l - 1 + length_a) % length_a],
                vertex_view_x_a[l], vertex_view_y_a[l], vertex_view_y_b[i1]);

            int l13 = vertex_view_x_b[i1];

            int k18 = scene_method306(
                vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_b[i1]);

            if (scene_method307(k4, i9, l13, k18, flag)) {
                return 1;
            }

            i1 = (i1 - 1 + length_b) % length_b;

            if (i1 == j1) {
                byte0 = 0;
            }
        } else {
            int l4 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_b[j1]);

            int j9 = scene_method306(
                vertex_view_x_a[(l - 1 + length_a) % length_a],
                vertex_view_y_a[(l - 1 + length_a) % length_a],
                vertex_view_x_a[l], vertex_view_y_a[l], vertex_view_y_b[j1]);

            int i14 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                      vertex_view_y_b[(i1 + 1) % length_b],
                                      vertex_view_x_b[i1], vertex_view_y_b[i1],
                                      vertex_view_y_b[j1]);

            int l18 = vertex_view_x_b[j1];

            if (scene_method307(l4, j9, i14, l18, flag)) {
                return 1;
            }

            j1 = (j1 + 1) % length_b;

            if (i1 == j1) {
                byte0 = 0;
            }
        }
    }

    while (byte0 == 2) {
        if (vertex_view_y_b[i1] < vertex_view_y_a[k]) {
            if (vertex_view_y_b[i1] < vertex_view_y_a[l]) {
                int i5 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                         vertex_view_y_a[(k + 1) % length_a],
                                         vertex_view_x_a[k], vertex_view_y_a[k],
                                         vertex_view_y_b[i1]);

                int k9 = scene_method306(
                    vertex_view_x_a[(l - 1 + length_a) % length_a],
                    vertex_view_y_a[(l - 1 + length_a) % length_a],
                    vertex_view_x_a[l], vertex_view_y_a[l],
                    vertex_view_y_b[i1]);

                int j14 = vertex_view_x_b[i1];

                return scene_method308(i5, k9, j14, flag);
            }

            int j5 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_a[l]);

            int l9 = vertex_view_x_a[l];

            int k14 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                      vertex_view_y_b[(i1 + 1) % length_b],
                                      vertex_view_x_b[i1], vertex_view_y_b[i1],
                                      vertex_view_y_a[l]);

            int i19 = scene_method306(
                vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_a[l]);

            if (scene_method307(j5, l9, k14, i19, flag)) {
                return 1;
            }

            l = (l + 1) % length_a;

            if (k == l) {
                byte0 = 0;
            }
        } else if (vertex_view_y_a[k] < vertex_view_y_a[l]) {
            int k5 = vertex_view_x_a[k];

            int i10 = scene_method306(
                vertex_view_x_a[(l - 1 + length_a) % length_a],
                vertex_view_y_a[(l - 1 + length_a) % length_a],
                vertex_view_x_a[l], vertex_view_y_a[l], vertex_view_y_a[k]);

            int l14 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                      vertex_view_y_b[(i1 + 1) % length_b],
                                      vertex_view_x_b[i1], vertex_view_y_b[i1],
                                      vertex_view_y_a[k]);

            int j19 = scene_method306(
                vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_a[k]);

            if (scene_method307(k5, i10, l14, j19, flag)) {
                return 1;
            }

            k = (k - 1 + length_a) % length_a;

            if (k == l) {
                byte0 = 0;
            }
        } else {
            int l5 = scene_method306(vertex_view_x_a[(k + 1) % length_a],
                                     vertex_view_y_a[(k + 1) % length_a],
                                     vertex_view_x_a[k], vertex_view_y_a[k],
                                     vertex_view_y_a[l]);

            int j10 = vertex_view_x_a[l];

            int i15 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                      vertex_view_y_b[(i1 + 1) % length_b],
                                      vertex_view_x_b[i1], vertex_view_y_b[i1],
                                      vertex_view_y_a[l]);

            int k19 = scene_method306(
                vertex_view_x_b[(j1 - 1 + length_b) % length_b],
                vertex_view_y_b[(j1 - 1 + length_b) % length_b],
                vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_a[l]);

            if (scene_method307(l5, j10, i15, k19, flag)) {
                return 1;
            }

            l = (l + 1) % length_a;

            if (k == l) {
                byte0 = 0;
            }
        }
    }

    if (vertex_view_y_a[k] < vertex_view_y_b[i1]) {
        int i6 = vertex_view_x_a[k];

        int j15 = scene_method306(vertex_view_x_b[(i1 + 1) % length_b],
                                  vertex_view_y_b[(i1 + 1) % length_b],
                                  vertex_view_x_b[i1], vertex_view_y_b[i1],
                                  vertex_view_y_a[k]);

        int l19 = scene_method306(
            vertex_view_x_b[(j1 - 1 + length_b) % length_b],
            vertex_view_y_b[(j1 - 1 + length_b) % length_b],
            vertex_view_x_b[j1], vertex_view_y_b[j1], vertex_view_y_a[k]);

        return scene_method308(j15, l19, i6, !flag);
    }

    int j6 =
        scene_method306(vertex_view_x_a[(k + 1) % length_a],
                        vertex_view_y_a[(k + 1) % length_a], vertex_view_x_a[k],
                        vertex_view_y_a[k], vertex_view_y_b[i1]);

    int k10 = scene_method306(vertex_view_x_a[(l - 1 + length_a) % length_a],
                              vertex_view_y_a[(l - 1 + length_a) % length_a],
                              vertex_view_x_a[l], vertex_view_y_a[l],
                              vertex_view_y_b[i1]);

    int k15 = vertex_view_x_b[i1];

    return scene_method308(j6, k10, k15, flag);
}
