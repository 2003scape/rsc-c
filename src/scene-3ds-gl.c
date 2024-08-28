#include "scene.h"

#ifdef RENDER_3DS_GL
void _3ds_gl_perspective(float fov, float aspect, float near, float far,
                         mat4 projection) {
    float fovx_tan = tanf(fov / 2.0f);

    glm_mat4_zero(projection);

    projection[1][0] = 1.0f / fovx_tan;
    projection[0][1] = -1.0f / (fovx_tan * aspect);
    projection[3][2] = far * near / (near - far);
    projection[2][3] = -1.0f;
    projection[2][2] = -projection[2][3] * near / (near - far);
}

/* transpose convert row/column and vertex order */
void _3ds_gl_mat4_to_pica(mat4 mtx) {
    /* change column/row */
    glm_mat4_transpose(mtx);

    /* reverse vertices */
    for (int i = 0; i < 4; i++) {
        float temp = mtx[i][0];

        mtx[i][0] = mtx[i][3];
        mtx[i][3] = temp;

        temp = mtx[i][1];

        mtx[i][1] = mtx[i][2];
        mtx[i][2] = temp;
    }
}

void scene_3ds_gl_draw_game_model(Scene *scene, GameModel *game_model) {
    if (game_model->gl_ebo_offset == -1 || !game_model->visible) {
        return;
    }

    // TODO should bind outside of here
    C3D_SetAttrInfo(&game_model->gl_buffer->attr_info);
    C3D_SetBufInfo(&game_model->gl_buffer->buf_info);

    mat4 pica_model = {0};
    glm_mat4_copy(game_model->transform, pica_model);

    _3ds_gl_mat4_to_pica(pica_model);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, scene->_3ds_gl_model_uniform,
                     (C3D_Mtx *)pica_model);

    mat4 view_model = {0};
    glm_mat4_mul(scene->gl_view, game_model->transform, view_model);

    mat4 projection_view_model = {0};
    glm_mat4_mul(scene->gl_projection, view_model, projection_view_model);

    _3ds_gl_mat4_to_pica(projection_view_model);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER,
                     scene->_3ds_gl_projection_view_model_uniform,
                     (C3D_Mtx *)projection_view_model);

    vec3 light_direction = {
        VERTEX_TO_FLOAT(game_model->light_direction_x) * VERTEX_SCALE,
        VERTEX_TO_FLOAT(game_model->light_direction_y) * VERTEX_SCALE,
        VERTEX_TO_FLOAT(game_model->light_direction_z) * VERTEX_SCALE};

    float light_diffuse = ((float)game_model->light_diffuse *
                           (float)game_model->light_direction_magnitude) /
                          256.0f;

    float fog_z_distance = (float)scene->fog_z_distance / -1000000.0f;

    C3D_FVUnifSet(
        GPU_VERTEX_SHADER, scene->_3ds_gl_light_ambience_diffuse_fog_uniform,
        (float)game_model->light_ambience, light_diffuse, fog_z_distance, 0);

    C3D_BoolUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_unlit_uniform,
                    game_model->unlit);

    C3D_FVUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_light_direction_uniform,
                  light_direction[0], light_direction[1], light_direction[2],
                  0);

    C3D_FVUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_opacity_uniform, 1.0f, 1.0f,
                  1.0f,
                  game_model->transparent ? TRANSLUCENT_MODEL_OPACITY : 1.0f);

    C3D_BoolUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_cull_front_uniform, 0);

    C3D_CullFace(GPU_CULL_BACK_CCW);

    C3D_DrawElements(GPU_TRIANGLES, game_model->gl_ebo_length,
                     C3D_UNSIGNED_SHORT,
                     game_model->gl_buffer->ebo +
                         (game_model->gl_ebo_offset * sizeof(uint16_t)));

    C3D_FVUnifSet(
        GPU_VERTEX_SHADER, scene->_3ds_gl_light_ambience_diffuse_fog_uniform,
        (float)game_model->light_ambience, light_diffuse, fog_z_distance, 0);

    C3D_BoolUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_unlit_uniform,
                    game_model->unlit);

    C3D_FVUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_light_direction_uniform,
                  light_direction[0], light_direction[1], light_direction[2],
                  0);

    C3D_BoolUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_cull_front_uniform, 1);

    C3D_CullFace(GPU_CULL_FRONT_CCW);

    C3D_DrawElements(GPU_TRIANGLES, game_model->gl_ebo_length,
                     C3D_UNSIGNED_SHORT,
                     game_model->gl_buffer->ebo +
                         (game_model->gl_ebo_offset * sizeof(uint16_t)));
}

void scene_3ds_gl_render(Scene *scene) {
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

    C3D_BindProgram(&scene->_3ds_gl_model_shader);

    C3D_AlphaTest(true, GPU_GREATER, 0);
    C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);

    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_TEXTURE0, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

    /* clear the second texenv */
    C3D_TexEnvInit(C3D_GetTexEnv(1));

    C3D_TexBind(0, &scene->gl_model_texture);

    C3D_FVUnifSet(GPU_VERTEX_SHADER, scene->_3ds_gl_scroll_texture_uniform,
                  0.0f, scene->gl_scroll_texture_position / GL_TEXTURE_SIZE,
                  0.0f, 0.0f);

    scene->gl_scroll_texture_position++;

    if (scene->gl_scroll_texture_position >= SCROLL_TEXTURE_SIZE) {
        scene->gl_scroll_texture_position = 0;
    }

    vec3 ray_start = {VERTEX_TO_FLOAT(scene->camera_x),
                      VERTEX_TO_FLOAT(scene->camera_y),
                      VERTEX_TO_FLOAT(scene->camera_z)};

    vec3 ray_end = {0};
    glm_vec3_add(ray_start, scene->gl_mouse_ray, ray_end);

    if (scene->gl_terrain_pick_step == GL_PICK_STEP_SAMPLE) {
        C3D_FrameDrawOn(scene->surface->mud->_3ds_gl_offscreen_render_target);

        /* draw the terrain first for potential mouse picking, since we can
         * click through the unpickable models */
        for (int i = 0; i < scene->model_count; i++) {
            GameModel *game_model = scene->models[i];

            if (game_model->autocommit && !game_model->unpickable) {
                scene_3ds_gl_draw_game_model(scene, game_model);
            }
        }

        int mouse_x = scene->mouse_x + (scene->surface->width / 2);
        int mouse_y = scene->surface->height - scene->mouse_y;

        float mouse_z = 0;

        uint16_t *depth_buf =
            (uint16_t *)(scene->surface->mud->_3ds_gl_offscreen_render_target
                             ->frameBuf.depthBuf);

        int fb_index = _3ds_gl_translate_framebuffer_index(
            (scene->mouse_y * 320) + mouse_x);

        mouse_z = 1.0f - (depth_buf[fb_index] / 65535.0f);

        vec3 position = {(float)mouse_x, (float)mouse_y, mouse_z};
        vec4 bounds = {0, 0, scene->surface->width, scene->surface->height};

        mat4 projection_view = {0};

        glm_mat4_mul(scene->gl_original_projection, scene->gl_view,
                     projection_view);

        glm_unproject(position, projection_view, bounds, scene->gl_mouse_world);

        scene->gl_terrain_pick_step = GL_PICK_STEP_FINISHED;

        scene->gl_terrain_pick_x =
            FLOAT_TO_VERTEX(scene->gl_mouse_world[0]) / MAGIC_LOC;

        scene->gl_terrain_pick_y =
            FLOAT_TO_VERTEX(scene->gl_mouse_world[2]) / MAGIC_LOC;

        C3D_RenderTargetClear(
            scene->surface->mud->_3ds_gl_offscreen_render_target, C3D_CLEAR_ALL,
            BLACK, 0);

        C3D_FrameDrawOn(scene->surface->mud->_3ds_gl_render_target);
    }

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

                    scene->gl_mouse_picked_time[scene->gl_mouse_picked_count] =
                        model_time;

                    scene->gl_mouse_picked_count++;
                }
            }
        }

        if (!game_model->transparent) {
            scene_3ds_gl_draw_game_model(scene, game_model);
        }
    }

    qsort(scene->gl_mouse_picked_time, scene->gl_mouse_picked_count,
          sizeof(GlModelTime), scene_gl_model_time_compare);

    for (int i = 0; i < scene->gl_mouse_picked_count; i++) {
        scene_mouse_pick(scene, scene->gl_mouse_picked_time[i].game_model, -1);
    }
}

void scene_3ds_gl_render_transparent_models(Scene *scene) {
    C3D_BindProgram(&scene->_3ds_gl_model_shader);

    C3D_AlphaTest(true, GPU_GREATER, 0);
    C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);

    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_TEXTURE0, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

    /* clear the second texenv */
    C3D_TexEnvInit(C3D_GetTexEnv(1));

    C3D_TexBind(0, &scene->gl_model_texture);

    for (int i = 0; i < scene->model_count; i++) {
        GameModel *game_model = scene->models[i];

        if (game_model->transparent) {
            scene_3ds_gl_draw_game_model(scene, game_model);
        }
    }
}
#endif
