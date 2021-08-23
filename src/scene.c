#include "scene.h"

int scene_frustum_max_x;
int scene_frustum_min_x;
int scene_frustum_max_y;
int scene_frustum_min_y;
int scene_frustum_far_z;
int scene_frustum_near_z;

void scene_new(Scene *scene, Surface *surface, int model_count,
               int polygon_count, int sprite_count) {
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
