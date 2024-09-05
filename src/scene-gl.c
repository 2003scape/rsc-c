#include "scene.h"

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
int scene_gl_model_time_compare(const void *a, const void *b) {
    GlModelTime model_time_a = *(GlModelTime *)a;
    GlModelTime model_time_b = *(GlModelTime *)b;

    if (model_time_a.time == model_time_b.time) {
        return 0;
    }

    return model_time_a.time < model_time_b.time ? -1 : 1;
}

void scene_gl_update_camera(Scene *scene) {
    vec3 camera_position = {VERTEX_TO_FLOAT(scene->camera_x),
                            VERTEX_TO_FLOAT(scene->camera_y),
                            VERTEX_TO_FLOAT(scene->camera_z)};

    vec3 camera_front = {0.0, 0.0, -1.0};
    vec3 camera_up = {0.0, -1.0, 0.0};

    // ??
    float yaw = 1.571051f + TABLE_TO_RADIANS(scene->camera_pitch, 2048);
    float pitch = 1.338493f - TABLE_TO_RADIANS(scene->camera_yaw, 2048);

    vec3 front = {cos(yaw) * cos(pitch), pitch, sin(yaw) * cos(pitch)};

    glm_normalize_to(front, camera_front);

    vec3 camera_centre = {0};
    glm_vec3_add(camera_position, camera_front, camera_centre);

    glm_lookat(camera_position, camera_centre, camera_up, scene->gl_view);

    glm_mat4_inv(scene->gl_view, scene->gl_inverse_view);

    float clip_far =
        VERTEX_TO_FLOAT(scene->clip_far_3d + scene->fog_z_distance);

#ifdef RENDER_GL
    glm_perspective(scene->gl_fov,
                    (float)(scene->width) / (float)(scene->gl_height - 1),
                    VERTEX_TO_FLOAT(scene->clip_near),
                    VERTEX_TO_FLOAT(clip_far), scene->gl_projection);

    glm_mat4_inv(scene->gl_projection, scene->gl_inverse_projection);
#elif defined(RENDER_3DS_GL)
    _3ds_gl_perspective(scene->gl_fov,
                        (float)(scene->width) / (float)(scene->gl_height - 1),
                        VERTEX_TO_FLOAT(scene->clip_near),
                        VERTEX_TO_FLOAT(clip_far), scene->gl_projection);

    glm_perspective(scene->gl_fov,
                    (float)(scene->width) / (float)(scene->gl_height - 1),
                    VERTEX_TO_FLOAT(scene->clip_near),
                    VERTEX_TO_FLOAT(clip_far), scene->gl_original_projection);

    glm_mat4_inv(scene->gl_original_projection, scene->gl_inverse_projection);
#endif

    glm_mat4_mul(scene->gl_projection, scene->gl_view,
                 scene->gl_projection_view);
}
#endif /* end shared between 3DS and normal GL */

/* normal GL only */
#ifdef RENDER_GL
void scene_gl_draw_game_model(Scene *scene, GameModel *game_model) {
    if (game_model->gl_ebo_offset == -1 || !game_model->visible) {
        return;
    }

    vertex_buffer_gl_bind(game_model->gl_buffer);

    shader_set_mat4(&scene->game_model_shader, "model", game_model->transform);

    mat4 view_model = {0};
    glm_mat4_mul(scene->gl_view, game_model->transform, view_model);

    mat4 projection_view_model = {0};
    glm_mat4_mul(scene->gl_projection, view_model, projection_view_model);

    shader_set_mat4(&scene->game_model_shader, "projection_view_model",
                    projection_view_model);

    vec3 light_direction = {
        VERTEX_TO_FLOAT(game_model->light_direction_x) * VERTEX_SCALE,
        VERTEX_TO_FLOAT(game_model->light_direction_y) * VERTEX_SCALE,
        VERTEX_TO_FLOAT(game_model->light_direction_z) * VERTEX_SCALE};

    shader_set_float(&scene->game_model_shader, "light_ambience",
                     game_model->light_ambience);

    shader_set_int(&scene->game_model_shader, "unlit", game_model->unlit);

    if (!game_model->unlit) {
        shader_set_vec3(&scene->game_model_shader, "light_direction",
                        light_direction);

        shader_set_float(&scene->game_model_shader, "light_diffuse",
                         ((float)game_model->light_diffuse *
                          (float)game_model->light_direction_magnitude) /
                             256.0f);
    }

    shader_set_float(&scene->game_model_shader, "opacity",
                     game_model->transparent ? TRANSLUCENT_MODEL_OPACITY
                                             : 1.0f);

    glCullFace(GL_BACK);
    shader_set_int(&scene->game_model_shader, "cull_front", 0);

    /* GL_LINES for polygons */
    glDrawElements(GL_TRIANGLES, game_model->gl_ebo_length, GL_UNSIGNED_INT,
                   (void *)(game_model->gl_ebo_offset * sizeof(GLuint)));

    glCullFace(GL_FRONT);
    shader_set_int(&scene->game_model_shader, "cull_front", 1);

    glDrawElements(GL_TRIANGLES, game_model->gl_ebo_length, GL_UNSIGNED_INT,
                   (void *)(game_model->gl_ebo_offset * sizeof(GLuint)));
}

void scene_gl_render(Scene *scene) {
    int scene_height = scene->gl_height - 1;

    int old_width = scene->surface->width;
    int old_height = scene->surface->height;

    scene->surface->width = scene->width;

    scene->surface->height =
        scene_height + 12 - mudclient_is_ui_scaled(scene->surface->mud);

    surface_reset_bounds(scene->surface);

    game_model_project_view(scene->view, scene->camera_x, scene->camera_y,
                            scene->camera_z, scene->camera_yaw,
                            scene->camera_pitch, scene->camera_roll,
                            scene->view_distance, scene->clip_near);

    scene->visible_polygons_count = 0;

    scene_initialise_polygons_2d(scene);

    for (int i = 0; i < scene->visible_polygons_count; i++) {
        GamePolygon *polygon = scene->visible_polygons[i];
        scene_render_polygon_2d_face(scene, polygon->face);
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 13, scene->width, scene_height);

    shader_use(&scene->game_model_shader);

    shader_set_int(&scene->game_model_shader, "fog_distance",
                   scene->fog_z_distance);

    shader_set_float(&scene->game_model_shader, "scroll_texture",
                     scene->gl_scroll_texture_position / GL_TEXTURE_SIZE);

    if (scene->gl_scroll_texture_position <= 0) {
        scene->gl_scroll_texture_position = SCROLL_TEXTURE_SIZE;
    }

    scene->gl_scroll_texture_position--;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene->gl_model_texture);

    vec3 ray_start = {VERTEX_TO_FLOAT(scene->camera_x),
                      VERTEX_TO_FLOAT(scene->camera_y),
                      VERTEX_TO_FLOAT(scene->camera_z)};

    vec3 ray_end = {0};

    glm_vec3_add(ray_start, scene->gl_mouse_ray, ray_end);

#ifdef EMSCRIPTEN
    /* webgl does not support depth buffer reading :( */
    if (scene->gl_terrain_pick_step == GL_PICK_STEP_SAMPLE) {
        GameModel *terrain_picked[4] = {0};
        int terrain_picked_length = 0;

        for (int i = 0; i < scene->model_count; i++) {
            GameModel *game_model = scene->models[i];

            if (!game_model->autocommit || game_model->unpickable) {
                continue;
            }

            float time = game_model_gl_intersects(game_model,
                                                  scene->gl_mouse_ray, ray_end);

            if (time >= 0 && terrain_picked_length < 4) {
                terrain_picked[terrain_picked_length++] = game_model;
            }
        }

        glDisable(GL_CULL_FACE);

        shader_use(&scene->game_model_pick_shader);

        game_model_gl_buffer_pick_models(&scene->gl_pick_buffer, terrain_picked,
                                         terrain_picked_length);

        for (int i = 0; i < terrain_picked_length; i++) {
            GameModel *game_model = terrain_picked[i];

            mat4 projection_view_model = {0};

            glm_mat4_mul(scene->gl_view, game_model->transform,
                         projection_view_model);

            glm_mat4_mul(scene->gl_projection, projection_view_model,
                         projection_view_model);

            shader_set_mat4(&scene->game_model_pick_shader,
                            "projection_view_model", projection_view_model);

            glDrawElements(
                GL_TRIANGLES, game_model->gl_ebo_length, GL_UNSIGNED_INT,
                (void *)(game_model->gl_pick_ebo_offset * sizeof(GLuint)));
        }

        int mouse_x = scene->mouse_x + (scene->surface->width / 2);
        int mouse_y = scene->surface->height - scene->mouse_y;
        uint8_t pick_colour[4] = {0};

        glReadPixels(mouse_x, mouse_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                     pick_colour);

        glClear(GL_DEPTH_BUFFER_BIT);

        scene->gl_terrain_pick_step = GL_PICK_STEP_FINISHED;
        scene->gl_pick_face_tag = (pick_colour[1] << 8) + pick_colour[0];

        shader_use(&scene->game_model_shader);

        glEnable(GL_CULL_FACE);
    }
#else
    /* draw the terrain first for potential mouse picking, since we can click
     * through the unpickable models */
    for (int i = 0; i < scene->model_count; i++) {
        GameModel *game_model = scene->models[i];

        if (game_model->autocommit && !game_model->unpickable) {
            scene_gl_draw_game_model(scene, game_model);
            game_model->gl_invisible = 1;
        }
    }

    if (scene->gl_terrain_pick_step == GL_PICK_STEP_SAMPLE) {
        int mouse_x = scene->mouse_x + (scene->surface->width / 2);
        int mouse_y = scene->surface->height - scene->mouse_y;

        float mouse_z = 0;

        glReadPixels(mouse_x, mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT,
                     &mouse_z);

        vec3 position = {(float)mouse_x, (float)mouse_y, mouse_z};
        vec4 bounds = {0, 0, scene->surface->width, scene->surface->height};

        glm_unproject(position, scene->gl_projection_view, bounds,
                      scene->gl_mouse_world);

        scene->gl_terrain_pick_step = GL_PICK_STEP_FINISHED;

        scene->gl_terrain_pick_x =
            FLOAT_TO_VERTEX(scene->gl_mouse_world[0]) / MAGIC_LOC;

        scene->gl_terrain_pick_y =
            FLOAT_TO_VERTEX(scene->gl_mouse_world[2]) / MAGIC_LOC;
    }
#endif

    for (int i = 0; i < scene->model_count; i++) {
        GameModel *game_model = scene->models[i];

        if (scene->mouse_picking_active && !game_model->unpickable) {
            float time = game_model_gl_intersects(game_model,
                                                  scene->gl_mouse_ray, ray_end);

            if (time >= 0) {
                if (game_model->autocommit) {
                    scene->gl_terrain_walkable = 1;
                } else {
                    GlModelTime model_time = {game_model, time};

                    if (scene->gl_mouse_picked_size <
                        (scene->gl_mouse_picked_count / 2)) {
                        size_t new_size = scene->gl_mouse_picked_count * 2;
                        void *new_ptr = NULL;

                        new_ptr = realloc(scene->gl_mouse_picked_time,
                                          new_size * sizeof(GlModelTime *));
                        if (new_ptr == NULL) {
                            return;
                        }
                        scene->gl_mouse_picked_time = new_ptr;
                        scene->gl_mouse_picked_size = new_size;
                    }

                    scene->gl_mouse_picked_time[scene->gl_mouse_picked_count] =
                        model_time;

                    scene->gl_mouse_picked_count++;
                }
            }
        }

        if (!game_model->gl_invisible && !game_model->transparent) {
            scene_gl_draw_game_model(scene, game_model);
        }

        game_model->gl_invisible = 0;
    }

    qsort(scene->gl_mouse_picked_time, scene->gl_mouse_picked_count,
          sizeof(GlModelTime), scene_gl_model_time_compare);

    for (int i = 0; i < scene->gl_mouse_picked_count; i++) {
        scene_mouse_pick(scene, scene->gl_mouse_picked_time[i].game_model, -1);
    }

    scene->surface->width = old_width;
    scene->surface->height = old_height;

    surface_reset_bounds(scene->surface);

    glViewport(0, 0, scene->surface->mud->game_width,
               scene->surface->mud->game_height);
}

/* draw translucent models (giant crystal) */
void scene_gl_render_transparent_models(Scene *scene) {
    int scene_height = scene->gl_height - 1;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 13, scene->width, scene_height);

    shader_use(&scene->game_model_shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene->gl_model_texture);

    for (int i = 0; i < scene->model_count; i++) {
        GameModel *game_model = scene->models[i];

        if (game_model->transparent) {
            scene_gl_draw_game_model(scene, game_model);
        }
    }

    glViewport(0, 0, scene->surface->mud->game_width,
               scene->surface->mud->game_height);
}

#endif
