#include "scene.h"

int scene_frustum_max_x;
int scene_frustum_min_x;
int scene_frustum_max_y;
int scene_frustum_min_y;
int scene_frustum_far_z;
int scene_frustum_near_z;

void scene_new(Scene *scene, Surface *surface, int max_model_count,
               int polygon_count, int sprite_count) {
    scene->clip_near = 5;
    scene->clip_far_3d = 1000;
    scene->clip_far_2d = 1000;
    scene->fog_z_falloff = 20;
    scene->fog_z_distance = 10;
    scene->ramp_count = 50;
    scene->mouse_picked_max = 100;
    scene->width = 512;
    scene->clip_x = 256;
    scene->clip_y = 192;
    scene->base_x = 256;
    scene->base_y = 256;
    scene->view_distance = 8;
    scene->normal_magnitude = 4;
}
