#include "scene.h"

int scene_frustum_max_x;
int scene_frustum_min_x;
int scene_frustum_max_y;
int scene_frustum_min_y;
int scene_frustum_far_z;
int scene_frustum_near_z;

void scene_new(Scene *scene, Surface *surface, int model_count,
               int polygon_count, int sprite_count) {
    scene->surface = surface;
    scene->max_model_count = model_count;

    scene->clip_near = 5;
    scene->clip_far_3d = 1000;
    scene->clip_far_2d = 1000;
    scene->fog_z_falloff = 20;
    scene->fog_z_distance = 10;
    scene->width = 512;
    scene->clip_x = 256;
    scene->clip_y = 192;
    scene->base_x = 256;
    scene->base_y = 256;
    scene->view_distance = 8;
    scene->normal_magnitude = 4;

    scene->raster = surface->pixels;

    scene->models = malloc(model_count * sizeof(GameModel));
    scene->visible_polygons = malloc(polygon_count * sizeof(Polygon));

    for (int i = 0; i < polygon_count; i++) {
        polygon_new(scene->visible_polygons[i]);
    }

    GameModel *view = malloc(sizeof(GameModel));
    game_model_from2(view, sprite_count * 2, sprite_count);
    scene->view = view;

    scene->sprite_id = malloc(sprite_count * sizeof(int));
    scene->sprite_width = malloc(sprite_count * sizeof(int));
    scene->sprite_height = malloc(sprite_count * sizeof(int));
    scene->sprite_x = malloc(sprite_count * sizeof(int));
    scene->sprite_z = malloc(sprite_count * sizeof(int));
    scene->sprite_y = malloc(sprite_count * sizeof(int));
    scene->sprite_translate_x = malloc(sprite_count * sizeof(int));
}

void scene_texture_scanline(uint32_t *ai, uint32_t *ai1, int i, int j, int k,
                            int l, int i1, int j1, int k1, int l1, int i2,
                            int j2, int k2, int l2) {
    if (i2 <= 0) {
        return;
    }

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
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
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

        ai[j2++] = ai1[(j & 0x3f80) + (i >> 7)] >> i4;
        i += k3;
        j += l3;
    }
}

void scene_texture_translucent_scanline(uint32_t *ai, uint32_t *ai1, int i,
                                        int j, int k, int l, int i1, int j1,
                                        int k1, int l1, int i2, int j2, int k2,
                                        int l2) {
    if (i2 <= 0) {
        return;
    }

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

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
        i = (i & 0x3fff) + (k2 & 0x600000);
        i4 = k2 >> 23;
        k2 += l2;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

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

        ai[j2++] =
            (ai1[(j & 0x3f80) + (i >> 7)] >> i4) + ((ai[j2] >> 1) & 0x7f7f7f);

        i += k3;
        j += l3;
    }
}

void scene_texture_back_translucent_scanline(uint32_t *ai, int i, int j, int k,
                                             uint32_t *ai1, int l, int i1,
                                             int j1, int k1, int l1, int i2,
                                             int j2, int k2, int l2, int i3) {
    if (j2 <= 0) {
        return;
    }

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
                    ai[k2] = i;
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
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0x3fff) + (l2 & 0x600000);
            k4 = l2 >> 23;
            l2 += i3;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0x3fff) + (l2 & 0x600000);
            k4 = l2 >> 23;
            l2 += i3;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0x3fff) + (l2 & 0x600000);
            k4 = l2 >> 23;
            l2 += i3;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0x3f80) + (j >> 7)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
        }
    }
}

void scene_texture_scanline2(uint32_t *ai, uint32_t *ai1, int i, int j, int k,
                             int l, int i1, int j1, int k1, int l1, int i2,
                             int j2, int k2, int l2) {
    if (i2 <= 0) {
        return;
    }

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
                ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
                i += k3;
                j += l3;

                if ((k4 & 3) == 3) {
                    i = (i & 0xfff) + (k2 & 0xc0000);
                    j4 = k2 >> 20;
                    k2 += l2;
                }
            }
        } else {
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
            i += k3;
            j += l3;
            ai[j2++] = ai1[(j & 0xfc0) + (i >> 6)] >> j4;
        }
    }
}

void scene_texture_translucent_scanline2(uint32_t *ai, uint32_t *ai1, int i,
                                         int j, int k, int l, int i1, int j1,
                                         int k1, int l1, int i2, int j2, int k2,
                                         int l2) {
    if (i2 <= 0) {
        return;
    }

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
                ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                           ((ai[j2] >> 1) & 0x7f7f7f);

                i += k3;
                j += l3;

                if ((k4 & 3) == 3) {
                    i = (i & 0xfff) + (k2 & 0xc0000);
                    j4 = k2 >> 20;
                    k2 += l2;
                }
            }
        } else {
            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;
            i = (i & 0xfff) + (k2 & 0xc0000);
            j4 = k2 >> 20;
            k2 += l2;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);

            i += k3;
            j += l3;

            ai[j2++] = (ai1[(j & 0xfc0) + (i >> 6)] >> j4) +
                       ((ai[j2] >> 1) & 0x7f7f7f);
        }
    }
}

void scene_texture_back_translucent_scanline2(uint32_t *ai, int i, int j, int k,
                                              uint32_t *ai1, int l, int i1,
                                              int j1, int k1, int l1, int i2,
                                              int j2, int k2, int l2, int i3) {
    if (j2 <= 0) {
        return;
    }

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
                if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                    ai[k2] = i;
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
            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0xfff) + (l2 & 0xc0000);
            k4 = l2 >> 20;
            l2 += i3;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0xfff) + (l2 & 0xc0000);
            k4 = l2 >> 20;
            l2 += i3;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;
            j = (j & 0xfff) + (l2 & 0xc0000);
            k4 = l2 >> 20;
            l2 += i3;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
            j += l3;
            k += i4;

            if ((i = ai1[(k & 0xfc0) + (j >> 6)] >> k4) != 0) {
                ai[k2] = i;
            }

            k2++;
        }
    }
}

void scene_gradient_scanline(uint32_t *ai, int i, int j, int k, uint32_t *ai1,
                             int l, int i1) {
    if (i >= 0) {
        return;
    }

    i1 <<= 1;
    k = ai1[(l >> 8) & 0xff];
    l += i1;
    int j1 = i / 8;

    for (int k1 = j1; k1 < 0; k1++) {
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
    }

    j1 = -(i % 8);

    for (int l1 = 0; l1 < j1; l1++) {
        ai[j++] = k;

        if ((l1 & 1) == 1) {
            k = ai1[(l >> 8) & 0xff];
            l += i1;
        }
    }
}

void scene_texture_gradient_scanline(uint32_t *ai, int i, int j, int k,
                                     uint32_t *ai1, int l, int i1) {
    if (i >= 0) {
        return;
    }

    i1 <<= 2;
    k = ai1[(l >> 8) & 0xff];
    l += i1;
    int j1 = i / 16;

    for (int k1 = j1; k1 < 0; k1++) {
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);
        k = ai1[(l >> 8) & 0xff];
        l += i1;
    }

    j1 = -(i % 16);

    for (int l1 = 0; l1 < j1; l1++) {
        ai[j++] = k + ((ai[j] >> 1) & 0x7f7f7f);

        if ((l1 & 3) == 3) {
            k = ai1[(l >> 8) & 0xff];
            l += i1;
            l += i1;
        }
    }
}

void scene_gradient_scanline2(uint32_t *ai, int i, int j, int k, uint32_t *ai1,
                              int l, int i1) {
    if (i >= 0) {
        return;
    }

    i1 <<= 2;
    k = ai1[(l >> 8) & 0xff];
    l += i1;
    int j1 = i / 16;

    for (int k1 = j1; k1 < 0; k1++) {
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        ai[j++] = k;
        k = ai1[(l >> 8) & 0xff];
        l += i1;
    }

    j1 = -(i % 16);

    for (int l1 = 0; l1 < j1; l1++) {
        ai[j++] = k;

        if ((l1 & 3) == 3) {
            k = ai1[(l >> 8) & 0xff];
            l += i1;
        }
    }
}

int rgb(int i, int j, int k) {
    return -1 - (i / 8) * 1024 - (j / 8) * 32 - (k / 8);
}

void scene_add_model(Scene *scene, GameModel *model) {
    if (model == NULL) {
        fprintf(stderr, "Warning tried to add null object!");
    }

    if (scene->model_count < scene->max_model_count) {
        scene->models[scene->model_count++] = model;
    }
}

void scene_remove_model(Scene *scene, GameModel *model) {
    for (int i = 0; i < scene->model_count; i++) {
        if (scene->models[i] == model) {
            scene->model_count--;

            for (int j = i; j < scene->model_count; j++) {
                scene->models[j] = scene->models[j + 1];
            }
        }
    }
}

void scene_dispose(Scene *scene) {
    scene_clear(scene);

    for (int i = 0; i < scene->model_count; i++) {
        free(scene->models[i]);
    }

    scene->model_count = 0;
}

void scene_clear(Scene *scene) {
    scene->sprite_count = 0;
    game_model_clear(scene->view);
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

    int bottom_vert = game_model_create_vertex(scene->view, x, z, y);
    int top_vert = game_model_create_vertex(scene->view, x, z - h, y);
    int *vertexes = malloc(2 * sizeof(int));
    vertexes[0] = bottom_vert;
    vertexes[1] = top_vert;

    game_model_create_face(scene->view, 2, vertexes, 0, 0);
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
        Scanline *scanline = malloc(sizeof(Scanline));
        scene->scanlines[i] = scanline;
    }
}

/* TODO replace with qsort */
void scene_polygons_q_sort(Scene *scene, Polygon **polygons, int low,
                           int high) {
    if (low < high) {
        int min = low - 1;
        int max = high + 1;
        int mid = (low + high) / 2;
        Polygon *polygon = polygons[mid];
        polygons[mid] = polygons[low];
        polygons[low] = polygon;
        int depth = polygon->depth;

        while (min < max) {
            do {
                max--;
            } while (polygons[max]->depth < depth);

            do {
                min++;
            } while (polygons[min]->depth > depth);

            if (min < max) {
                Polygon *polygon_1 = polygons[min];
                polygons[min] = polygons[max];
                polygons[max] = polygon_1;
            }
        }

        scene_polygons_q_sort(scene, polygons, low, max);
        scene_polygons_q_sort(scene, polygons, max + 1, high);
    }
}

void scene_polygons_intersect_sort(Scene *scene, int step, Polygon **polygons,
                                   int count) {
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

        Polygon *polygon = polygons[l];
        polygon->skip_something = 1;

        int i1 = l;
        int j1 = l + step;

        if (j1 >= count) {
            j1 = count - 1;
        }

        for (int k1 = j1; k1 >= i1 + 1; k1--) {
            Polygon *other = polygons[k1];

            if (polygon->min_plane_x < other->max_plane_x &&
                other->min_plane_x < polygon->max_plane_x &&
                polygon->min_plane_y < other->max_plane_y &&
                other->min_plane_y < polygon->max_plane_y &&
                polygon->index != other->index2 &&
                !scene_separate_polygon(polygon, other) &&
                scene_heuristic_polygon(other, polygon)) {
                scene_polygons_order(scene, polygons, i1, k1);

                if (polygons[k1] != other) {
                    k1++;
                }

                i1 = scene->new_start;
                other->index2 = polygon->index;
            }
        }
    } while (1);
}

int scene_polygons_order(Scene *scene, Polygon **polygons, int start, int end) {
    do {
        Polygon *polygon = polygons[start];

        for (int k = start + 1; k <= end; k++) {
            Polygon *polygon_1 = polygons[k];

            if (!scene_separate_polygon(polygon_1, polygon)) {
                break;
            }

            polygons[start] = polygon_1;
            polygons[k] = polygon;
            start = k;

            if (start == end) {
                scene->new_start = start;
                scene->new_end = start - 1;
                return 1;
            }
        }

        Polygon *polygon_2 = polygons[end];

        for (int l = end - 1; l >= start; l--) {
            Polygon *polygon_3 = polygons[l];

            if (!scene_separate_polygon(polygon_2, polygon_3)) {
                break;
            }

            polygons[end] = polygon_3;
            polygons[l] = polygon_2;
            end = l;

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

void scene_set_frustum(Scene *scene, int i, int j, int k) {
    int l = (-scene->camera_yaw + 1024) & 0x3ff;
    int i1 = (-scene->camera_pitch + 1024) & 0x3ff;
    int j1 = (-scene->camera_roll + 1024) & 0x3ff;

    if (j1 != 0) {
        int k1 = sin_cos_2048[j1];
        int j2 = sin_cos_2048[j1 + 1024];
        int i3 = (j * k1 + i * j2) >> 15;
        j = (j * j2 - i * k1) >> 15;
        i = i3;
    }

    if (l != 0) {
        int l1 = sin_cos_2048[l];
        int k2 = sin_cos_2048[l + 1024];
        int j3 = (j * k2 - k * l1) >> 15;
        k = (j * l1 + k * k2) >> 15;
        j = j3;
    }

    if (i1 != 0) {
        int i2 = sin_cos_2048[i1];
        int l2 = sin_cos_2048[i1 + 1024];
        int k3 = (k * i2 + i * l2) >> 15;
        k = (k * l2 - i * i2) >> 15;
        i = k3;
    }

    if (i < scene_frustum_max_x) {
        scene_frustum_max_x = i;
    }

    if (i > scene_frustum_min_x) {
        scene_frustum_min_x = i;
    }

    if (j < scene_frustum_max_y) {
        scene_frustum_max_y = j;
    }

    if (j > scene_frustum_min_y) {
        scene_frustum_min_y = j;
    }

    if (k < scene_frustum_far_z) {
        scene_frustum_far_z = k;
    }

    if (k > scene_frustum_near_z) {
        scene_frustum_near_z = k;
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

    /* TODO make sure this is working */
    for (int i = 0; i <= scene->model_count; i++) {
        game_model_project(scene->models[i], scene->camera_x, scene->camera_y,
                           scene->camera_z, scene->camera_yaw,
                           scene->camera_pitch, scene->camera_roll,
                           scene->view_distance, scene->clip_near);
    }

    scene->visible_polygons_count = 0;

    for (int count = 0; count < scene->model_count; count++) {
        GameModel *game_model = scene->models[count];

        if (game_model->visible) {
            for (int face = 0; face < game_model->num_faces; face++) {
                int num_vertices = game_model->face_num_vertices[face];
                int *vertices = game_model->face_vertices[face];
                int visible = 0;

                for (int vertex = 0; vertex < num_vertices; vertex++) {
                    int z = game_model->project_vertex_z[vertices[vertex]];

                    if (z <= scene->clip_near || z >= scene->clip_far_3d) {
                        continue;
                    }

                    visible = 1;
                    break;
                }

                if (visible) {
                    int view_x_count = 0;

                    for (int vertex = 0; vertex < num_vertices; vertex++) {
                        int x = game_model->vertex_view_x[vertices[vertex]];

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
                            int k1 =
                                game_model->vertex_view_y[vertices[vertex]];

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
                            Polygon *polygon_1 =
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
                                             [vertices[vertex]];
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
            int vertex0 = face_vertices[0];
            int vx = model_2d->vertex_view_x[vertex0];
            int vy = model_2d->vertex_view_y[vertex0];
            int vz = model_2d->project_vertex_z[vertex0];

            if (vz > scene->clip_near && vz < scene->clip_far_2d) {
                int vw =
                    (scene->sprite_width[face] << scene->view_distance) / vz;

                int vh =
                    (scene->sprite_height[face] << scene->view_distance) / vz;

                if (vx - (vw / 2) <= scene->clip_x &&
                    vx + (vw / 2) >= -scene->clip_x &&
                    vy - vh <= scene->clip_y && vy >= -scene->clip_y) {

                    Polygon *polygon_2 =
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

    scene_polygons_q_sort(scene, scene->visible_polygons, 0,
                          scene->visible_polygons_count - 1);

    scene_polygons_intersect_sort(scene, 100, scene->visible_polygons,
                                  scene->visible_polygons_count);

    for (int i = 0; i < scene->visible_polygons_count; i++) {
        Polygon *polygon = scene->visible_polygons[i];
        GameModel *game_model_2 = polygon->model;
        int l = polygon->face;

        if (game_model_2 == scene->view) {
            int *faceverts = game_model_2->face_vertices[l];
            int face_0 = faceverts[0];
            int vx = game_model_2->vertex_view_x[face_0];
            int vy = game_model_2->vertex_view_y[face_0];
            int vz = game_model_2->project_vertex_z[face_0];
            int w = (scene->sprite_width[l] << scene->view_distance) / vz;
            int h = (scene->sprite_height[l] << scene->view_distance) / vz;
            int tx = game_model_2->vertex_view_x[faceverts[1]] - vx;
            int x = vx - (w / 2);
            int y = scene->base_y + vy - h;

            surface_sprite_clipping_from7(scene->surface, x + scene->base_x, y,
                                          w, h, scene->sprite_id[l], tx,
                                          (256 << scene->view_distance) / vz);

            if (scene->mouse_picking_active &&
                scene->mouse_picked_count < MOUSE_PICKED_MAX) {
                x +=
                    (scene->sprite_translate_x[l] << scene->view_distance) / vz;

                if (scene->mouse_y >= y && scene->mouse_y <= y + h &&
                    scene->mouse_x >= x && scene->mouse_x <= x + w &&
                    !game_model_2->unpickable &&
                    game_model_2->is_local_player[l] == 0) {
                    scene->mouse_picked_models[scene->mouse_picked_count] =
                        game_model_2;
                    scene->mouse_picked_faces[scene->mouse_picked_count] = l;
                    scene->mouse_picked_count++;
                }
            }
        } else {
            int k8 = 0;
            int j10 = 0;
            int l10 = game_model_2->face_num_vertices[l];
            int *ai3 = game_model_2->face_vertices[l];

            if (game_model_2->face_intensity[l] != COLOUR_TRANSPARENT) {
                if (polygon->visibility < 0) {
                    j10 = game_model_2->light_ambience -
                          game_model_2->face_intensity[l];
                } else {
                    j10 = game_model_2->light_ambience +
                          game_model_2->face_intensity[l];
                }
            }

            for (int k11 = 0; k11 < l10; k11++) {
                int k2 = ai3[k11];

                scene->vertex_x[k11] = game_model_2->project_vertex_x[k2];
                scene->vertex_y[k11] = game_model_2->project_vertex_y[k2];
                scene->vertex_z[k11] = game_model_2->project_vertex_z[k2];

                if (game_model_2->face_intensity[l] == COLOUR_TRANSPARENT) {
                    if (polygon->visibility < 0) {
                        j10 = game_model_2->light_ambience -
                              game_model_2->vertex_intensity[k2] +
                              game_model_2->vertex_ambience[k2];
                    } else {
                        j10 = game_model_2->light_ambience +
                              game_model_2->vertex_intensity[k2] +
                              game_model_2->vertex_ambience[k2];
                    }
                }

                if (game_model_2->project_vertex_z[k2] >= scene->clip_near) {
                    scene->plane_x[k8] = game_model_2->vertex_view_x[k2];
                    scene->plane_y[k8] = game_model_2->vertex_view_y[k2];
                    scene->vertex_shade[k8] = j10;

                    if (game_model_2->project_vertex_z[k2] >
                        scene->fog_z_distance) {
                        scene->vertex_shade[k8] +=
                            (game_model_2->project_vertex_z[k2] -
                             scene->fog_z_distance) /
                            scene->fog_z_falloff;
                    }

                    k8++;
                } else {
                    int k9 = 0;

                    if (k11 == 0) {
                        k9 = ai3[l10 - 1];
                    } else {
                        k9 = ai3[k11 - 1];
                    }

                    if (game_model_2->project_vertex_z[k9] >=
                        scene->clip_near) {
                        int k7 = game_model_2->project_vertex_z[k2] -
                                 game_model_2->project_vertex_z[k9];

                        int i5 = game_model_2->project_vertex_x[k2] -
                                 (((game_model_2->project_vertex_x[k2] -
                                    game_model_2->project_vertex_x[k9]) *
                                   (game_model_2->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  k7);

                        int j6 = game_model_2->project_vertex_y[k2] -
                                 (((game_model_2->project_vertex_y[k2] -
                                    game_model_2->project_vertex_y[k9]) *
                                   (game_model_2->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  k7);

                        scene->plane_x[k8] =
                            (i5 << scene->view_distance) / scene->clip_near;

                        scene->plane_y[k8] =
                            (j6 << scene->view_distance) / scene->clip_near;

                        scene->vertex_shade[k8] = j10;
                        k8++;
                    }

                    if (k11 == l10 - 1) {
                        k9 = ai3[0];
                    } else {
                        k9 = ai3[k11 + 1];
                    }

                    if (game_model_2->project_vertex_z[k9] >=
                        scene->clip_near) {
                        int l7 = game_model_2->project_vertex_z[k2] -
                                 game_model_2->project_vertex_z[k9];

                        int j5 = game_model_2->project_vertex_x[k2] -
                                 (((game_model_2->project_vertex_x[k2] -
                                    game_model_2->project_vertex_x[k9]) *
                                   (game_model_2->project_vertex_z[k2] -
                                    scene->clip_near)) /
                                  l7);

                        int k6 = game_model_2->project_vertex_y[k2] -
                                 (((game_model_2->project_vertex_y[k2] -
                                    game_model_2->project_vertex_y[k9]) *
                                   (game_model_2->project_vertex_z[k2] -
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

            for (int i12 = 0; i12 < l10; i12++) {
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

            scene_generate_scanlines(scene, 0, 0, 0, 0, k8, scene->plane_x,
                                     scene->plane_y, scene->vertex_shade,
                                     game_model_2, l);

            if (scene->max_y > scene->min_y) {
                scene_rasterize(scene, 0, 0, l10, scene->vertex_x,
                                scene->vertex_y, scene->vertex_z,
                                polygon->facefill, game_model_2);
            }
        }
    }

    scene->mouse_picking_active = 0;
}

void scene_generate_scanlines(Scene *scene, int i, int j, int k, int l, int i1,
                              uint32_t *ai, uint32_t *ai1, uint32_t *ai2,
                              GameModel *game_model, int pid) {
    if (i1 == 3) {
        int k1 = ai1[0] + scene->base_y;
        int k2 = ai1[1] + scene->base_y;
        int k3 = ai1[2] + scene->base_y;
        int k4 = ai[0];
        int l5 = ai[1];
        int j7 = ai[2];
        int l8 = ai2[0];
        int j10 = ai2[1];
        int j11 = ai2[2];
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

        int l21 = 0;

        for (k = scene->min_y; k < scene->max_y; k++) {
            if (k >= l14 && k < j15) {
                i = j = l12;
                l = l21 = l13;
                l12 += j13;
                l13 += j14;
            } else {
                i = 655360;
                j = -655360;
            }

            if (k >= l17 && k < j18) {
                if (l15 < i) {
                    i = l15;
                    l = l16;
                }

                if (l15 > j) {
                    j = l15;
                    l21 = l16;
                }

                l15 += j16;
                l16 += j17;
            }

            if (k >= l20 && k < j21) {
                if (l18 < i) {
                    i = l18;
                    l = l19;
                }

                if (l18 > j) {
                    j = l18;
                    l21 = l19;
                }

                l18 += j19;
                l19 += j20;
            }

            Scanline *scanline_6 = scene->scanlines[k];
            scanline_6->start_x = i;
            scanline_6->end_x = j;
            scanline_6->start_s = l;
            scanline_6->end_s = l21;
        }

        if (scene->min_y < scene->base_y - scene->clip_y) {
            scene->min_y = scene->base_y - scene->clip_y;
        }
    } else if (i1 == 4) {
        int l1 = ai1[0] + scene->base_y;
        int l2 = ai1[1] + scene->base_y;
        int l3 = ai1[2] + scene->base_y;
        int l4 = ai1[3] + scene->base_y;
        int i6 = ai[0];
        int k7 = ai[1];
        int i9 = ai[2];
        int k10 = ai[3];
        int k11 = ai2[0];
        int k12 = ai2[1];
        int i13 = ai2[2];
        int k13 = ai2[3];
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

        int j24 = 0;

        for (k = scene->min_y; k < scene->max_y; k++) {
            if (k >= k16 && k < i17) {
                i = j = k14;
                l = j24 = k15;
                k14 += i15;
                k15 += i16;
            } else {
                i = 655360;
                j = -655360;
            }

            if (k >= k19 && k < i20) {
                if (k17 < i) {
                    i = k17;
                    l = k18;
                }

                if (k17 > j) {
                    j = k17;
                    j24 = k18;
                }

                k17 += i18;
                k18 += i19;
            }

            if (k >= j22 && k < k22) {
                if (k20 < i) {
                    i = k20;
                    l = k21;
                }

                if (k20 > j) {
                    j = k20;
                    j24 = k21;
                }

                k20 += i21;
                k21 += i22;
            }

            if (k >= l23 && k < i24) {
                if (l22 < i) {
                    i = l22;
                    l = j23;
                }

                if (l22 > j) {
                    j = l22;
                    j24 = j23;
                }

                l22 += i23;
                j23 += k23;
            }

            Scanline *scanline_7 = scene->scanlines[k];
            scanline_7->start_x = i;
            scanline_7->end_x = j;
            scanline_7->start_s = l;
            scanline_7->end_s = j24;
        }

        if (scene->min_y < scene->base_y - scene->clip_y) {
            scene->min_y = scene->base_y - scene->clip_y;
        }
    } else {
        scene->max_y = scene->min_y = ai1[0] += scene->base_y;

        for (k = 1; k < i1; k++) {
            int i2 = 0;

            if ((i2 = ai1[k] += scene->base_y) < scene->min_y) {
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

        for (k = scene->min_y; k < scene->max_y; k++) {
            Scanline *scanline = scene->scanlines[k];
            scanline->start_x = 655360;
            scanline->end_x = -655360;
        }

        int j2 = i1 - 1;
        int i3 = ai1[0];
        int i4 = ai1[j2];

        if (i3 < i4) {
            int i5 = ai[0] << 8;
            int j6 = ((ai[j2] - ai[0]) << 8) / (i4 - i3);
            int l7 = ai2[0] << 8;
            int j9 = ((ai2[j2] - ai2[0]) << 8) / (i4 - i3);

            if (i3 < 0) {
                i5 -= j6 * i3;
                l7 -= j9 * i3;
                i3 = 0;
            }

            if (i4 > scene->max_y) {
                i4 = scene->max_y;
            }

            for (k = i3; k <= i4; k++) {
                Scanline *scanline_2 = scene->scanlines[k];
                scanline_2->start_x = scanline_2->end_x = i5;
                scanline_2->start_s = scanline_2->end_s = l7;
                i5 += j6;
                l7 += j9;
            }
        } else if (i3 > i4) {
            int j5 = ai[j2] << 8;
            int k6 = ((ai[0] - ai[j2]) << 8) / (i3 - i4);
            int i8 = ai2[j2] << 8;
            int k9 = ((ai2[0] - ai2[j2]) << 8) / (i3 - i4);

            if (i4 < 0) {
                j5 -= k6 * i4;
                i8 -= k9 * i4;
                i4 = 0;
            }

            if (i3 > scene->max_y) {
                i3 = scene->max_y;
            }

            for (k = i4; k <= i3; k++) {
                Scanline *scanline_3 = scene->scanlines[k];
                scanline_3->start_x = scanline_3->end_x = j5;
                scanline_3->start_s = scanline_3->end_s = i8;
                j5 += k6;
                i8 += k9;
            }
        }

        for (k = 0; k < j2; k++) {
            int k5 = k + 1;
            int j3 = ai1[k];
            int j4 = ai1[k5];

            if (j3 < j4) {
                int l6 = ai[k] << 8;
                int j8 = ((ai[k5] - ai[k]) << 8) / (j4 - j3);
                int l9 = ai2[k] << 8;
                int l10 = ((ai2[k5] - ai2[k]) << 8) / (j4 - j3);

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
                int i7 = ai[k5] << 8;
                int k8 = ((ai[k] - ai[k5]) << 8) / (j3 - j4);
                int i10 = ai2[k5] << 8;
                int i11 = ((ai2[k] - ai2[k5]) << 8) / (j3 - j4);

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
            !game_model->unpickable && game_model->is_local_player[pid] == 0) {
            scene->mouse_picked_models[scene->mouse_picked_count] = game_model;
            scene->mouse_picked_faces[scene->mouse_picked_count] = pid;
            scene->mouse_picked_count++;
        }
    }
}

void scene_rasterize(Scene *scene, int i, int j, int k, uint32_t *ai,
                     uint32_t *ai1, uint32_t *ai2, int l,
                     GameModel *game_model) {
    if (l == -2) {
        return;
    }

    if (l >= 0) {
        if (l >= scene->texture_count) {
            l = 0;
        }

        scene_prepare_texture(scene, l);

        int i1 = ai[0];
        int k1 = ai1[0];
        int j2 = ai2[0];
        int i3 = i1 - ai[1];
        int k3 = k1 - ai1[1];
        int i4 = j2 - ai2[1];
        k--;
        int i6 = ai[k] - i1;
        int j7 = ai1[k] - k1;
        int k8 = ai2[k] - j2;

        if (scene->texture_dimension[l] == 1) {
            int l9 = (i6 * k1 - j7 * i1) << 12;
            int k10 = (j7 * j2 - k8 * k1) << (5 - scene->view_distance + 7 + 4);
            int i11 = (k8 * i1 - i6 * j2) << (5 - scene->view_distance + 7);
            int k11 = (i3 * k1 - k3 * i1) << 12;
            int i12 = (k3 * j2 - i4 * k1) << (5 - scene->view_distance + 7 + 4);
            int k12 = (i4 * i1 - i3 * j2) << (5 - scene->view_distance + 7);
            int i13 = (k3 * i6 - i3 * j7) << 5;
            int k13 = (i4 * j7 - k3 * k8) << (5 - scene->view_distance + 4);
            int i14 = (i3 * k8 - i4 * i6) >> (scene->view_distance - 5);
            int k14 = k10 >> 4;
            int i15 = i12 >> 4;
            int k15 = k13 >> 4;
            int i16 = scene->min_y - scene->base_y;
            int k16 = scene->width;
            int i17 = scene->base_x + scene->min_y * k16;
            int8_t byte1 = 1;
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
                byte1 = 2;
            }

            if (game_model->texture_translucent) {
                for (i = scene->min_y; i < scene->max_y; i += byte1) {
                    Scanline *scanline_3 = scene->scanlines[i];
                    j = scanline_3->start_x >> 8;
                    int k17 = scanline_3->end_x >> 8;
                    int k20 = k17 - j;

                    if (k20 <= 0) {
                        l9 += i11;
                        k11 += k12;
                        i13 += i14;
                        i17 += k16;
                    } else {
                        int i22 = scanline_3->start_s;
                        int k23 = (scanline_3->end_s - i22) / k20;

                        if (j < -scene->clip_x) {
                            i22 += (-scene->clip_x - j) * k23;
                            j = -scene->clip_x;
                            k20 = k17 - j;
                        }

                        if (k17 > scene->clip_x) {
                            int l17 = scene->clip_x;
                            k20 = l17 - j;
                        }

                        scene_texture_translucent_scanline(
                            scene->raster, scene->texture_pixels[l], 0, 0,
                            l9 + k14 * j, k11 + i15 * j, i13 + k15 * j, k10,
                            i12, k13, k20, i17 + j, i22, k23 << 2);

                        l9 += i11;
                        k11 += k12;
                        i13 += i14;
                        i17 += k16;
                    }
                }

                return;
            }

            if (!scene->texture_back_transparent[l]) {
                for (i = scene->min_y; i < scene->max_y; i += byte1) {
                    Scanline *scanline_4 = scene->scanlines[i];
                    j = scanline_4->start_x >> 8;
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
                            scene->raster, scene->texture_pixels[l], 0, 0,
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

            for (i = scene->min_y; i < scene->max_y; i += byte1) {
                Scanline *scanline_5 = scene->scanlines[i];
                j = scanline_5->start_x >> 8;
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
                        scene->raster, 0, 0, 0, scene->texture_pixels[l],
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

        int i10 = (i6 * k1 - j7 * i1) << 11;
        int l10 = (j7 * j2 - k8 * k1) << (5 - scene->view_distance + 6 + 4);
        int j11 = (k8 * i1 - i6 * j2) << (5 - scene->view_distance + 6);
        int l11 = (i3 * k1 - k3 * i1) << 11;
        int j12 = (k3 * j2 - i4 * k1) << (5 - scene->view_distance + 6 + 4);
        int l12 = (i4 * i1 - i3 * j2) << (5 - scene->view_distance + 6);
        int j13 = (k3 * i6 - i3 * j7) << 5;
        int l13 = (i4 * j7 - k3 * k8) << (5 - scene->view_distance + 4);
        int j14 = (i3 * k8 - i4 * i6) >> (scene->view_distance - 5);
        int l14 = l10 >> 4;
        int j15 = j12 >> 4;
        int l15 = l13 >> 4;
        int j16 = scene->min_y - scene->base_y;
        int l16 = scene->width;
        int j17 = scene->base_x + scene->min_y * l16;
        int8_t byte2 = 1;

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
            byte2 = 2;
        }

        if (game_model->texture_translucent) {
            for (i = scene->min_y; i < scene->max_y; i += byte2) {
                Scanline *scanline_6 = scene->scanlines[i];
                j = scanline_6->start_x >> 8;
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
                        scene->raster, scene->texture_pixels[l], 0, 0,
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

        if (!scene->texture_back_transparent[l]) {
            for (i = scene->min_y; i < scene->max_y; i += byte2) {
                Scanline *scanline_7 = scene->scanlines[i];
                j = scanline_7->start_x >> 8;
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
                        scene->raster, scene->texture_pixels[l], 0, 0,
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

        for (i = scene->min_y; i < scene->max_y; i += byte2) {
            Scanline *scanline = scene->scanlines[i];
            j = scanline->start_x >> 8;
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
                    scene->raster, 0, 0, 0, scene->texture_pixels[l],
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

    for (int j1 = 0; j1 < RAMP_COUNT; j1++) {
        if (scene->gradient_base[j1] == l) {
            scene->an_int_array_377 = scene->gradient_ramps[j1];
            break;
        }

        if (j1 == RAMP_COUNT - 1) {
            float r = (float)rand() / (float)RAND_MAX;
            int l1 = r * RAMP_COUNT;
            scene->gradient_base[l1] = l;

            l = -1 - l;
            int k2 = ((l >> 10) & 0x1f) * 8;
            int j3 = ((l >> 5) & 0x1f) * 8;
            int l3 = (l & 0x1f) * 8;

            for (int j4 = 0; j4 < 256; j4++) {
                int j6 = j4 * j4;
                int k7 = (k2 * j6) / 0x10000;
                int l8 = (j3 * j6) / 0x10000;
                int j10 = (l3 * j6) / 0x10000;

                scene->gradient_ramps[l1][255 - j4] =
                    (k7 << 16) + (l8 << 8) + j10;
            }

            scene->an_int_array_377 = scene->gradient_ramps[l1];
        }
    }

    int i2 = scene->width;
    int l2 = scene->base_x + scene->min_y * i2;
    int8_t byte0 = 1;

    if (scene->interlace) {
        if ((scene->min_y & 1) == 1) {
            scene->min_y++;
            l2 += i2;
        }

        i2 <<= 1;
        byte0 = 2;
    }

    if (game_model->transparent) {
        for (i = scene->min_y; i < scene->max_y; i += byte0) {
            Scanline *scanline = scene->scanlines[i];
            j = scanline->start_x >> 8;
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

                scene_texture_gradient_scanline(scene->raster, -k6, l2 + j, 0,
                                                scene->an_int_array_377, l7,
                                                i9);

                l2 += i2;
            }
        }

        return;
    }

    if (scene->wide_band) {
        for (i = scene->min_y; i < scene->max_y; i += byte0) {
            Scanline *scanline_1 = scene->scanlines[i];
            j = scanline_1->start_x >> 8;
            int i5 = scanline_1->end_x >> 8;
            int l6 = i5 - j;

            if (l6 <= 0) {
                l2 += i2;
            } else {
                int i8 = scanline_1->start_s;
                int j9 = (scanline_1->end_s - i8) / l6;

                if (j < -scene->clip_x) {
                    i8 += (-scene->clip_x - j) * j9;
                    j = -scene->clip_x;
                    l6 = i5 - j;
                }

                if (i5 > scene->clip_x) {
                    int j5 = scene->clip_x;
                    l6 = j5 - j;
                }

                scene_gradient_scanline(scene->raster, -l6, l2 + j, 0,
                                        scene->an_int_array_377, i8, j9);

                l2 += i2;
            }
        }

        return;
    }

    for (i = scene->min_y; i < scene->max_y; i += byte0) {
        Scanline *scanline_2 = scene->scanlines[i];
        j = scanline_2->start_x >> 8;
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

            scene_gradient_scanline2(scene->raster, -i7, l2 + j, 0,
                                     scene->an_int_array_377, j8, k9);

            l2 += i2;
        }
    }
}

void scene_set_camera(Scene *scene, int x, int z, int y, int pitch, int yaw,
                      int roll, int distance) {
    pitch &= 0x3ff;
    yaw &= 0x3ff;
    roll &= 0x3ff;
    scene->camera_yaw = (1024 - pitch) & 0x3ff;
    scene->camera_pitch = (1024 - yaw) & 0x3ff;
    scene->camera_roll = (1024 - roll) & 0x3ff;

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
    Polygon *polygon = scene->visible_polygons[i];
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
    Polygon *polygon = scene->visible_polygons[i];
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

int scene_separate_polygon(Polygon *polygon, Polygon *polygon_1) {
    if (polygon->min_plane_x >= polygon_1->max_plane_x) {
        return 1;
    }

    if (polygon_1->min_plane_x >= polygon->max_plane_x) {
        return 1;
    }

    if (polygon->min_plane_y >= polygon_1->max_plane_y) {
        return 1;
    }

    if (polygon_1->min_plane_y >= polygon->max_plane_y) {
        return 1;
    }

    if (polygon->min_z >= polygon_1->max_z) {
        return 1;
    }

    if (polygon_1->min_z > polygon->max_z) {
        return 0;
    }

    GameModel *game_model = polygon->model;
    GameModel *game_model_1 = polygon_1->model;
    int i = polygon->face;
    int j = polygon_1->face;
    int *ai = game_model->face_vertices[i];
    int *ai1 = game_model_1->face_vertices[j];
    int k = game_model->face_num_vertices[i];
    int l = game_model_1->face_num_vertices[j];
    int k2 = game_model_1->project_vertex_x[ai1[0]];
    int l2 = game_model_1->project_vertex_y[ai1[0]];
    int i3 = game_model_1->project_vertex_z[ai1[0]];
    int j3 = polygon_1->normal_x;
    int k3 = polygon_1->normal_y;
    int l3 = polygon_1->normal_z;
    int i4 = game_model_1->normal_magnitude[j];
    int j4 = polygon_1->visibility;
    int flag = 0;

    for (int k4 = 0; k4 < k; k4++) {
        int i1 = ai[k4];

        int i2 = (k2 - game_model->project_vertex_x[i1]) * j3 +
                 (l2 - game_model->project_vertex_y[i1]) * k3 +
                 (i3 - game_model->project_vertex_z[i1]) * l3;

        if ((i2 >= -i4 || j4 >= 0) && (i2 <= i4 || j4 <= 0)) {
            continue;
        }

        flag = 1;
        break;
    }

    if (!flag) {
        return 1;
    }

    k2 = game_model->project_vertex_x[ai[0]];
    l2 = game_model->project_vertex_y[ai[0]];
    i3 = game_model->project_vertex_z[ai[0]];
    j3 = polygon->normal_x;
    k3 = polygon->normal_y;
    l3 = polygon->normal_z;
    i4 = game_model->normal_magnitude[i];
    j4 = polygon->visibility;
    flag = 0;

    for (int l4 = 0; l4 < l; l4++) {
        int j1 = ai1[l4];
        int j2 = (k2 - game_model_1->project_vertex_x[j1]) * j3 +
                 (l2 - game_model_1->project_vertex_y[j1]) * k3 +
                 (i3 - game_model_1->project_vertex_z[j1]) * l3;

        if ((j2 >= -i4 || j4 <= 0) && (j2 <= i4 || j4 >= 0)) {
            continue;
        }

        flag = 1;
        break;
    }

    if (!flag) {
        return 1;
    }

    int *ai2;
    int *ai3;
    int len1;

    if (k == 2) {
        ai2 = alloca(4 * sizeof(int));
        ai3 = alloca(4 * sizeof(int));
        int i5 = ai[0];
        int k1 = ai[1];
        ai2[0] = game_model->vertex_view_x[i5] - 20;
        ai2[1] = game_model->vertex_view_x[k1] - 20;
        ai2[2] = game_model->vertex_view_x[k1] + 20;
        ai2[3] = game_model->vertex_view_x[i5] + 20;
        ai3[0] = ai3[3] = game_model->vertex_view_y[i5];
        ai3[1] = ai3[2] = game_model->vertex_view_y[k1];
        len1 = 4;
    } else {
        ai2 = alloca(k * sizeof(int));
        ai3 = alloca(k * sizeof(int));

        for (int j5 = 0; j5 < k; j5++) {
            int i6 = ai[j5];
            ai2[j5] = game_model->vertex_view_x[i6];
            ai3[j5] = game_model->vertex_view_y[i6];
        }

        len1 = k;
    }

    int *ai4;
    int *ai5;
    int len2;

    if (l == 2) {
        ai4 = alloca(4 * sizeof(int));
        ai5 = alloca(4 * sizeof(int));
        int k5 = ai1[0];
        int l1 = ai1[1];
        ai4[0] = game_model_1->vertex_view_x[k5] - 20;
        ai4[1] = game_model_1->vertex_view_x[l1] - 20;
        ai4[2] = game_model_1->vertex_view_x[l1] + 20;
        ai4[3] = game_model_1->vertex_view_x[k5] + 20;
        ai5[0] = ai5[3] = game_model_1->vertex_view_y[k5];
        ai5[1] = ai5[2] = game_model_1->vertex_view_y[l1];
        len2 = 4;
    } else {
        /* TODO check this size */

        ai4 = alloca(l * sizeof(int));
        ai5 = alloca(l * sizeof(int));

        for (int l5 = 0; l5 < l; l5++) {
            int j6 = ai1[l5];
            ai4[l5] = game_model_1->vertex_view_x[j6];
            ai5[l5] = game_model_1->vertex_view_y[j6];
        }

        len2 = l;
    }

    return !scene_intersect(ai2, ai3, ai4, ai5, len1, len2);
}

int scene_heuristic_polygon(Polygon *polygon, Polygon *polygon_1) {
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
    scene->texture_colours_used = malloc(count * sizeof(int8_t *));
    scene->texture_colour_list = malloc(count * sizeof(uint32_t *));
    scene->texture_dimension = new int32Array(count);
    scene->texture_loaded_number = [];
    scene->texture_loaded_number.length = count;
    scene->texture_loaded_number.fill(null);
    scene->texture_back_transparent = malloc(count);
    scene->texture_pixels = malloc(count * sizeof(uint32_t *));
    scene_texture_count_loaded = 0;

    for (int i = 0; i < count; i++) {
        scene->texture_loaded_number[i] = 0;
    }

    // 64x64 rgba
    scene->texture_colours64 = [];
    scene->texture_colours64.length = something7;
    scene->texture_colours64.fill(null);

    // 128x128 rgba
    scene->texture_colours128 = [];
    scene->texture_colours128.length = something11;
    scene->texture_colours128.fill(null);
}
