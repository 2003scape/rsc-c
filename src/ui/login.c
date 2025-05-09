#include "login.h"

void mudclient_create_login_panels(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int is_touch = mudclient_is_touch(mud);

    int login_background_height = is_compact ? 125 : 200;

    mud->panel_login_welcome = malloc(sizeof(Panel));
    panel_new(mud->panel_login_welcome, mud->surface, 50);

    int x = (is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH) / 2;
    int y = 40;

    int offset_y = is_compact ? 17 : 40;

    // TODO divorce registration from p2p
    int show_registration =
        !mud->options->members || mud->options->registration;

    if (show_registration) {
        int offset_x = is_compact ? 16 : 0;

        panel_add_text_centre(
            mud->panel_login_welcome, x,
            login_background_height + y - (is_compact ? 20 : 0),
            is_touch ? "Tap an option" : "Click on an option", FONT_BOLD_16, 1);

        panel_add_button_background(
            mud->panel_login_welcome, x - 100 + offset_x,
            login_background_height + offset_y + y, 120, 35);

        panel_add_text_centre(mud->panel_login_welcome, x - 100 + offset_x,
                              login_background_height + offset_y + y,
                              "New User", FONT_BOLD_16, 0);

        mud->control_welcome_new_user =
            panel_add_button(mud->panel_login_welcome, x - 100 + offset_x,
                             login_background_height + offset_y + y, 120, 35);

        panel_add_button_background(
            mud->panel_login_welcome, x + 100 - offset_x,
            login_background_height + offset_y + y, 120, 35);

        panel_add_text_centre(mud->panel_login_welcome, x + 100 - offset_x,
                              login_background_height + offset_y + y,
                              "Existing User", FONT_BOLD_16, 0);

        mud->control_welcome_existing_user =
            panel_add_button(mud->panel_login_welcome, x + 100 - offset_x,
                             login_background_height + offset_y + y, 120, 35);
    } else {
        FontStyle font_style = FONT_BOLD_14;
        int button_width = 200;

        if (is_compact) {
            y -= 18;
            font_style = FONT_BOLD_13;
            button_width = 180;
        }

        panel_add_text_centre(mud->panel_login_welcome, x,
                              login_background_height + y,
                              "Welcome to RuneScape", font_style, 1);

        panel_add_text_centre(
            mud->panel_login_welcome, x, login_background_height + 15 + y,
            "You need a members account to use this server", font_style, 1);

        panel_add_button_background(mud->panel_login_welcome, x,
                                    login_background_height + 50 + y,
                                    button_width, 35);

        panel_add_text_centre(
            mud->panel_login_welcome, x, login_background_height + 50 + y,
            is_touch ? "Tap here to login" : "Click here to login",
            FONT_BOLD_16, 0);

        mud->control_welcome_existing_user = panel_add_button(
            mud->panel_login_welcome, x, login_background_height + 50 + y,
            button_width, 35);
    }

    if (mud->options->show_additional_options) {
        int button_x = (is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH) - 36;

        int button_y =
            is_compact ? MUD_MIN_HEIGHT - 24 : MUD_VANILLA_HEIGHT - 32;

        panel_add_button_background(mud->panel_login_welcome, button_x,
                                    button_y, 60, 20);

        panel_add_text_centre(mud->panel_login_welcome, button_x, button_y,
                              "Options", FONT_BOLD_12, 0);

        mud->control_welcome_options = panel_add_button(
            mud->panel_login_welcome, button_x, button_y, 60, 20);

        button_x -= 63;

        panel_add_button_background(mud->panel_login_welcome, button_x,
                                    button_y, 60, 20);

        panel_add_text_centre(mud->panel_login_welcome, button_x, button_y,
                              "Worlds", FONT_BOLD_12, 0);

        mud->control_welcome_worlds = panel_add_button(
            mud->panel_login_welcome, button_x, button_y, 60, 20);
    }

    mud->panel_login_new_user = malloc(sizeof(Panel));
    panel_new(mud->panel_login_new_user, mud->surface, 50);

    if (!mud->options->account_management) {
        FontStyle font;

        if (!is_compact) {
            font = FONT_BOLD_14;
            y = 230;
        } else {
            font = FONT_REGULAR_12;
            y = 140;
        }

        if (mud->refer_id == 0 && !is_compact) {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "To create an account please go back to the",
                                  font, 1);

            y += 20;

            panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "www.runescape.com front page, and choose 'create account'",
                font, 1);
        } else if (mud->refer_id == 1) {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "To create an account please click on the",
                                  font, 1);

            y += 20;

            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "'create account' link below the game window",
                                  font, 1);
        } else {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "To create an account please go back to the",
                                  font, 1);

            y += 20;

            panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "runescape front webpage and choose 'create account'", font, 1);
        }

        y += 30;

        panel_add_button_background(mud->panel_login_new_user, x, y + 17, 150,
                                    34);

        panel_add_text_centre(mud->panel_login_new_user, x, y + 17, "Ok",
                              FONT_BOLD_16, 0);

        mud->control_login_new_ok =
            panel_add_button(mud->panel_login_new_user, x, y + 17, 150, 34);
    } else {
        if (is_compact) {
            y = 12;

            mud->control_register_status = panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "To create an account please enter", FONT_BOLD_13, 1);

            y += 13;

            mud->control_register_status_bottom = panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "all the requested details", FONT_BOLD_13, 1);
        } else {
            y = 70;

            mud->control_register_status = panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "To create an account please enter all the requested details",
                FONT_BOLD_14, 1);
        }

        int offset_y = y + 25;

        panel_add_button_background(mud->panel_login_new_user, x, offset_y + 17,
                                    250, 34);

        panel_add_text_centre(mud->panel_login_new_user, x, offset_y + 8,
                              "Choose a username", FONT_BOLD_14, 0);

        mud->control_register_user =
            panel_add_text_input(mud->panel_login_new_user, x, offset_y + 25,
                                 200, 40, FONT_BOLD_14, 12, 0, 0);

        offset_y += 40;

        if (is_compact) {
            panel_add_button_background(mud->panel_login_new_user, x,
                                        offset_y + 17, 220, 34);

            panel_add_text_centre(mud->panel_login_new_user, x, offset_y + 8,
                                  "Choose a Password", FONT_BOLD_14, 0);

            mud->control_register_password = panel_add_text_input(
                mud->panel_login_new_user, x, offset_y + 25, 220, 40,
                FONT_BOLD_14, 20, 1, 0);

            offset_y += 40;

            panel_add_button_background(mud->panel_login_new_user, x,
                                        offset_y + 17, 220, 34);

            panel_add_text_centre(mud->panel_login_new_user, x, offset_y + 8,
                                  "Confirm Password", FONT_BOLD_14, 0);

            mud->control_register_confirm_password = panel_add_text_input(
                mud->panel_login_new_user, x, offset_y + 25, 220, 40,
                FONT_BOLD_14, 20, 1, 0);
            offset_y += 34;
        } else {
            panel_add_button_background(mud->panel_login_new_user, x - 115,
                                        offset_y + 17, 220, 34);

            panel_add_text_centre(mud->panel_login_new_user, x - 115,
                                  offset_y + 8, "Choose a Password",
                                  FONT_BOLD_14, 0);

            mud->control_register_password = panel_add_text_input(
                mud->panel_login_new_user, x - 115, offset_y + 25, 220, 40,
                FONT_BOLD_14, 20, 1, 0);

            panel_add_button_background(mud->panel_login_new_user, x + 115,
                                        offset_y + 17, 220, 34);

            panel_add_text_centre(mud->panel_login_new_user, x + 115,
                                  offset_y + 8, "Confirm Password",
                                  FONT_BOLD_14, 0);

            mud->control_register_confirm_password = panel_add_text_input(
                mud->panel_login_new_user, x + 115, offset_y + 25, 220, 40,
                FONT_BOLD_14, 20, 1, 0);
            offset_y += 60;

            mud->control_register_checkbox = panel_add_checkbox(
                mud->panel_login_new_user, x - 196 - 7, offset_y - 7, 14, 14);

            panel_add_text(mud->panel_login_new_user, x - 181, offset_y,
                           "I have read and agreed to the terms and conditions",
                           FONT_BOLD_14, 1);

            offset_y += 15;

            panel_add_text_centre(mud->panel_login_new_user, x, offset_y,
                                  "(to view these click the relevant link "
                                  "below this game window)",
                                  FONT_BOLD_14, 1);
        }

        offset_y += 20;

        int offset_x = is_compact ? 20 : 0;

        panel_add_button_background(mud->panel_login_new_user,
                                    x - 100 + offset_x, offset_y + 17, 150, 34);

        panel_add_text_centre(mud->panel_login_new_user, x - 100 + offset_x,
                              offset_y + 17, "Submit", FONT_BOLD_16, 0);

        mud->control_register_submit =
            panel_add_button(mud->panel_login_new_user, x - 100 + offset_x,
                             offset_y + 17, 150, 34);

        panel_add_button_background(mud->panel_login_new_user,
                                    x + 100 - offset_x, offset_y + 17, 150, 34);

        panel_add_text_centre(mud->panel_login_new_user, x + 100 - offset_x,
                              offset_y + 17, "Cancel", FONT_BOLD_16, 0);

        mud->control_register_cancel =
            panel_add_button(mud->panel_login_new_user, x + 100 - offset_x,
                             offset_y + 17, 150, 34);
    }

    mud->panel_login_existing_user = malloc(sizeof(Panel));
    panel_new(mud->panel_login_existing_user, mud->surface, 50);

    y = login_background_height + 30;

    if (is_compact) {
        y -= 34;

        mud->control_login_status = panel_add_text_centre(
            mud->panel_login_existing_user, x, y - 10, "", FONT_BOLD_13, 1);

        y += 13;

        mud->control_login_status_bottom = panel_add_text_centre(
            mud->panel_login_existing_user, x, y - 10,
            "Please enter your username and password", FONT_BOLD_13, 1);

        y += 24;

        panel_add_button_background(mud->panel_login_existing_user, x - 56, y,
                                    200, 40);

        panel_add_text_centre(mud->panel_login_existing_user, x - 56, y - 10,
                              "Username:", FONT_BOLD_14, 0);

        mud->control_login_username =
            panel_add_text_input(mud->panel_login_existing_user, x - 56, y + 10,
                                 200, 40, FONT_BOLD_14, 12, 0, 0);

        y += 47;

        panel_add_button_background(mud->panel_login_existing_user, x - 32, y,
                                    200, 40);

        panel_add_text_centre(mud->panel_login_existing_user, x - 32, y - 10,
                              "Password:", FONT_BOLD_14, 0);

        mud->control_login_password =
            panel_add_text_input(mud->panel_login_existing_user, x - 32, y + 10,
                                 200, 40, FONT_BOLD_14, 20, 1, 0);

        y -= 55;

        panel_add_button_background(mud->panel_login_existing_user,
                                    x + 154 - 38, y, 80, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154 - 38, y,
                              "Ok", FONT_BOLD_14, 0);

        mud->control_login_ok = panel_add_button(mud->panel_login_existing_user,
                                                 x + 154 - 38, y, 80, 25);

        y += 30;

        panel_add_button_background(mud->panel_login_existing_user,
                                    x + 154 - 38, y, 80, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154 - 38, y,
                              "Cancel", FONT_BOLD_14, 0);

        mud->control_login_cancel = panel_add_button(
            mud->panel_login_existing_user, x + 154 - 38, y, 80, 25);

    } else {
        mud->control_login_status = panel_add_text_centre(
            mud->panel_login_existing_user, x, y - 10,
            "Please enter your username and password", FONT_BOLD_14, 1);

        y += 28;

        panel_add_button_background(mud->panel_login_existing_user, x - 116, y,
                                    200, 40);

        panel_add_text_centre(mud->panel_login_existing_user, x - 116, y - 10,
                              "Username:", FONT_BOLD_14, 0);

        mud->control_login_username =
            panel_add_text_input(mud->panel_login_existing_user, x - 116,
                                 y + 10, 200, 40, FONT_BOLD_14, 12, 0, 0);

        y += 47;

        panel_add_button_background(mud->panel_login_existing_user, x - 66, y,
                                    200, 40);

        panel_add_text_centre(mud->panel_login_existing_user, x - 66, y - 10,
                              "Password:", FONT_BOLD_14, 0);

        mud->control_login_password =
            panel_add_text_input(mud->panel_login_existing_user, x - 66, y + 10,
                                 200, 40, FONT_BOLD_14, 20, 1, 0);

        y -= 55;

        panel_add_button_background(mud->panel_login_existing_user, x + 154, y,
                                    120, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154, y, "Ok",
                              FONT_BOLD_14, 0);

        mud->control_login_ok = panel_add_button(mud->panel_login_existing_user,
                                                 x + 154, y, 120, 25);

        y += 30;

        panel_add_button_background(mud->panel_login_existing_user, x + 154, y,
                                    120, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154, y,
                              "Cancel", FONT_BOLD_14, 0);

        mud->control_login_cancel = panel_add_button(
            mud->panel_login_existing_user, x + 154, y, 120, 25);
    }

    if (mud->options->account_management) {
        y += 30;

        /*panel_add_button_background(mud->panel_login_existing_user, x + 154,
        y, 160, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154, y,
                              "I've lost my password", FONT_BOLD_14, 0);

        mud->control_login_recover = panel_add_button(
            mud->panel_login_existing_user, x + 154, y, 160, 25);*/
    }
}

void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    if (mud->login_screen == LOGIN_STAGE_NEW) {
        sprintf(login_screen_status, "%s %s", s, s1);

        if (is_compact) {
            panel_update_text(mud->panel_login_new_user,
                              mud->control_register_status, s);

            panel_update_text(mud->panel_login_new_user,
                              mud->control_register_status_bottom, s1);
        } else {
            panel_update_text(mud->panel_login_new_user,
                              mud->control_register_status,
                              login_screen_status);
        }

    } else if (mud->login_screen == LOGIN_STAGE_EXISTING) {
        if (is_compact) {
            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_status, s);

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_status_bottom, s1);
        } else {
            sprintf(login_screen_status, "%s %s", s, s1);

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_status, login_screen_status);
        }
    }

    mudclient_draw_login_screens(mud);
    mudclient_reset_timings(mud);
}

void mudclient_reset_login_screen(mudclient *mud) {
    mud->logged_in = 0;
    mud->login_screen = LOGIN_STAGE_WELCOME;

    memset(mud->login_username, '\0', USERNAME_LENGTH + 1);
    memset(mud->login_pass, '\0', PASSWORD_LENGTH + 1);

    mud->login_prompt = "Please enter a username:";

    sprintf(mud->login_username_display, "*%s*", mud->login_username);

    mud->player_count = 0;
    mud->npc_count = 0;
}

void mudclient_render_login_scene_sprites(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int login_background_height = is_compact ? 125 : 200;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    int old_fov = mud->options->field_of_view;
    mud->options->field_of_view = 360;
    mudclient_update_fov(mud);
#endif

    int plane = 0;
    int region_x = 50; // 49;
    int region_y = 50; // 47;

    world_load_section(mud->world, region_x * 48 + 23, region_y * 48 + 23,
                       plane);

    world_add_models(mud->world, mud->game_models);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    world_gl_buffer_world_models(mud->world);
#endif

#ifdef RENDER_3DS_GL
    int model_index = 0;

    for (int i = 0; i < mud->scene->model_count; i++) {
        if (mud->scene->models[i]->gl_ebo_offset == -1) {
            model_index++;
        }
    }

    GameModel *world_models[model_index];

    model_index = 0;

    for (int i = 0; i < mud->scene->model_count; i++) {
        GameModel *game_model = mud->scene->models[i];

        if (game_model->gl_ebo_offset == -1) {
            world_models[model_index++] = game_model;
        }
    }

    game_model_gl_buffer_models(&mud->scene->gl_game_model_buffers,
                                &mud->scene->gl_game_model_buffer_length,
                                world_models, model_index);
#endif

    int x = 9728;
    int y = 6400;
    int zoom = 1100;
    int rotation = 888;

    mud->scene->clip_far_3d = 4100;
    mud->scene->clip_far_2d = 4100;
    mud->scene->fog_z_distance = 4000;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    /* clear the previous buffer */
    surface_gl_reset_context(mud->surface);
#endif

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_offscreen_frame_start(mud);
#endif

    scene_render(mud->scene);

    surface_apply_login_filter(mud->surface, login_background_height);

    int logo_width = MUD_WIDTH - 29;
    int logo_height = (int)((logo_width / 483.0f) * 146.0f);
    int offset_x = (mud->surface->width / 2) - logo_width / 2;
    int offset_y = 15;

    surface_draw_sprite_transform_mask(mud->surface, offset_x, offset_y,
                                       logo_width, logo_height,
                                       mud->sprite_media + 10, 0, 0, 0, 0);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_draw(mud->surface);
#endif

#ifdef RENDER_3DS_GL
    C3D_FrameEnd(0);

    /* stupid hack but it works */
    // TODO try with smaller value on real hardware
    delay_ticks(LOGIN_RENDER_DELAY);
#endif

    surface_screen_raster_to_sprite(mud->surface, mud->sprite_logo, 0, 0,
                                    mud->surface->width,
                                    login_background_height);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_apply_login_filter(mud->surface, mud->sprite_logo);
#endif

#ifdef RENDER_SW
    surface_screen_raster_to_palette_sprite(mud->surface, mud->sprite_logo);

    /* for surface_draw_sprite_scale_alpha */
    surface_load_sprite(mud->surface, mud->sprite_logo);
#endif

    x = 9216;
    y = 9216;
    zoom = 1100;
    rotation = 888;

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_offscreen_frame_start(mud);
#endif

    scene_render(mud->scene);

    surface_apply_login_filter(mud->surface, login_background_height);

    surface_draw_sprite_transform_mask(mud->surface, offset_x, offset_y,
                                       logo_width, logo_height,
                                       mud->sprite_media + 10, 0, 0, 0, 0);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_draw(mud->surface);
#endif

#ifdef RENDER_3DS_GL
    C3D_FrameEnd(0);

    delay_ticks(LOGIN_RENDER_DELAY);
#endif

    surface_screen_raster_to_sprite(mud->surface, mud->sprite_logo + 1, 0, 0,
                                    mud->surface->width,
                                    login_background_height);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_apply_login_filter(mud->surface, mud->sprite_logo + 1);
#endif

#ifdef RENDER_SW
    surface_screen_raster_to_palette_sprite(mud->surface, mud->sprite_logo + 1);
    surface_load_sprite(mud->surface, mud->sprite_logo + 1);
#endif

    for (int i = 0; i < TERRAIN_COUNT; i++) {
        scene_remove_model(mud->scene, mud->world->roof_models[0][i]);
        scene_remove_model(mud->scene, mud->world->wall_models[1][i]);
        scene_remove_model(mud->scene, mud->world->roof_models[1][i]);
        scene_remove_model(mud->scene, mud->world->wall_models[2][i]);
        scene_remove_model(mud->scene, mud->world->roof_models[2][i]);

        game_model_destroy(mud->world->roof_models[0][i]);
        game_model_destroy(mud->world->wall_models[1][i]);
        game_model_destroy(mud->world->roof_models[1][i]);
        game_model_destroy(mud->world->wall_models[2][i]);
        game_model_destroy(mud->world->roof_models[2][i]);

        free(mud->world->roof_models[0][i]);
        free(mud->world->wall_models[1][i]);
        free(mud->world->roof_models[1][i]);
        free(mud->world->wall_models[2][i]);
        free(mud->world->roof_models[2][i]);

        mud->world->roof_models[0][i] = NULL;
        mud->world->wall_models[1][i] = NULL;
        mud->world->roof_models[1][i] = NULL;
        mud->world->wall_models[2][i] = NULL;
        mud->world->roof_models[2][i] = NULL;
    }

    x = 11136;
    y = 10368;
    zoom = 500;
    rotation = 376;

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_offscreen_frame_start(mud);
#endif

    scene_render(mud->scene);

    surface_apply_login_filter(mud->surface, login_background_height);

    surface_draw_sprite_transform_mask(mud->surface, offset_x, offset_y,
                                       logo_width, logo_height,
                                       mud->sprite_media + 10, 0, 0, 0, 0);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_draw(mud->surface);
#endif

#ifdef RENDER_3DS_GL
    C3D_FrameEnd(0);

    /* stupid hack but it works */
    // TODO try with smaller value on real hardware
    delay_ticks(LOGIN_RENDER_DELAY);
#endif

    surface_screen_raster_to_sprite(mud->surface, mud->sprite_logo + 2, 0, 0,
                                    mud->surface->width,
                                    login_background_height);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_apply_login_filter(mud->surface, mud->sprite_logo + 2);
#endif

#ifdef RENDER_SW
    surface_screen_raster_to_palette_sprite(mud->surface, mud->sprite_logo + 2);
    surface_load_sprite(mud->surface, mud->sprite_logo + 2);
#endif

    world_reset(mud->world, 0);

    for (int i = 0; i < mud->scene->model_count; i++) {
        game_model_destroy(mud->scene->models[i]);
        free(mud->scene->models[i]);
        mud->scene->models[i] = NULL;
    }

    mud->scene->model_count = 0;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    mud->options->field_of_view = old_fov;
    mudclient_update_fov(mud);
#endif
}

void mudclient_draw_login_screens(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int login_background_width = is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH;
    int login_background_height = is_compact ? 125 : 200;

#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_frame_start(mud, 1);
#endif

    mud->welcome_screen_already_shown = 0;
    mud->surface->interlace = 0;

    surface_black_screen(mud->surface);

    int show_background = mud->login_screen != LOGIN_STAGE_RECOVER &&
                          mud->login_screen != LOGIN_STAGE_NEW;

    if (mud->login_screen == LOGIN_STAGE_NEW &&
        !mud->options->account_management) {
        show_background = 1;
    }

    if (mud->options->lowmem) {
        show_background = 0;
    }

    if (show_background) {
        int width = is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH;
        int height = is_compact ? MUD_MIN_HEIGHT : MUD_VANILLA_HEIGHT;

        int offset_x = (mud->surface->width / 2) - (width / 2);
        int offset_y = (mud->surface->height / 2) - (height / 2);

        int cycle = (mud->login_timer * 2) % 3072;

        int scale_login = login_background_height !=
                          mud->surface->sprite_height[mud->sprite_logo];

        int sprite_offset = cycle / 1024;

        cycle %= 1024;

        if (scale_login) {
            surface_draw_sprite_scale(
                mud->surface, offset_x, offset_y + 10, login_background_width,
                login_background_height, mud->sprite_logo + sprite_offset, 0);
        } else {
            surface_draw_sprite(mud->surface, offset_x, offset_y + 10,
                                mud->sprite_logo + sprite_offset);
        }

        if (cycle > 768) {
            int next_sprite =
                mud->sprite_logo + (sprite_offset == 2 ? 0 : sprite_offset + 1);

            int alpha = cycle - 768;

            if (scale_login) {
                surface_draw_sprite_scale_alpha(
                    mud->surface, offset_x, offset_y + 10,
                    login_background_width, login_background_height,
                    next_sprite, alpha);
            } else {
                surface_draw_sprite_alpha(mud->surface, offset_x, offset_y + 10,
                                          next_sprite, alpha);
            }
        }

        /* fade the left/right of the login scene if the width exceeds 512 */
        if (offset_x > 0) {
            for (int i = 0; i < 3; i++) {
                int alpha = 192 - (i * 64);

                /* left */
                surface_draw_box_alpha(mud->surface, (i * 2) + offset_x,
                                       offset_y + 10, 2,
                                       login_background_height, BLACK, alpha);

                /* right */
                surface_draw_box_alpha(
                    mud->surface, login_background_width - (i * 2) + offset_x,
                    10 + offset_y, 2, login_background_height, BLACK, alpha);
            }
        }
    }

    switch (mud->login_screen) {
    case LOGIN_STAGE_WELCOME:
        panel_draw_panel(mud->panel_login_welcome);
        break;
    case LOGIN_STAGE_NEW:
        panel_draw_panel(mud->panel_login_new_user);
        break;
    case LOGIN_STAGE_EXISTING: {
        if (is_compact) {
            int box_height = 12;

            if (strlen(panel_get_text(mud->panel_login_existing_user,
                                      mud->control_login_status)) != 0) {
                box_height *= 2;
            }

            surface_draw_box_alpha(
                mud->surface, 0,
                login_background_height - box_height +
                    ((mud->surface->height / 2) - (MUD_MIN_HEIGHT / 2)),
                mud->surface->width, box_height + 4, 0, 128);
        }

        panel_draw_panel(mud->panel_login_existing_user);
        break;
    }
    case LOGIN_STAGE_RECOVER:
        // TODO
        break;
    case LOGIN_STAGE_WORLD:
        panel_draw_panel(mud->panel_login_worldlist);
        break;
    }

    if (!mud->options->lowmem) {
        mudclient_draw_blue_bar(mud);
    }

    if (mud->show_additional_options) {
        mudclient_draw_additional_options(mud);

        if (mud->show_dialog_confirm) {
            mudclient_draw_confirm(mud);
        }
    }

    surface_draw(mud->surface);

#ifdef RENDER_GL
#ifdef SDL12
    SDL_GL_SwapBuffers();
#else
    SDL_GL_SwapWindow(mud->gl_window);
#endif
#elif defined(RENDER_3DS_GL)
    mudclient_3ds_gl_frame_end();
#endif
}

void mudclient_handle_login_screen_input(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    if (mud->show_dialog_confirm) {
        mudclient_handle_confirm_input(mud);
        return;
    } else if (mud->show_additional_options) {
        mudclient_handle_additional_options_input(mud);
        return;
    }

    if (mud->world_full_timeout > 0) {
        mud->world_full_timeout--;
    }

    switch (mud->login_screen) {
    case LOGIN_STAGE_WELCOME:
        panel_handle_mouse(mud->panel_login_welcome, mud->mouse_x, mud->mouse_y,
                           mud->last_mouse_button_down, mud->mouse_button_down,
                           mud->mouse_scroll_delta);

        if (panel_is_clicked(mud->panel_login_welcome,
                             mud->control_welcome_new_user)) {
            mud->login_screen = LOGIN_STAGE_NEW;

            if (mud->options->account_management) {
                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_user, "");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_password, "");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_confirm_password, "");

                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_user);

                if (is_compact) {
                    panel_update_text(mud->panel_login_new_user,
                                      mud->control_register_status,
                                      "To create an account please enter");

                    panel_update_text(mud->panel_login_new_user,
                                      mud->control_register_status_bottom,
                                      "all the requested details");

                    panel_toggle_checkbox(mud->panel_login_new_user,
                                          mud->control_register_checkbox, 1);
                } else {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "To create an account please enter all the requested "
                        "details");

                    panel_toggle_checkbox(mud->panel_login_new_user,
                                          mud->control_register_checkbox, 0);
                }
            }
        } else if (panel_is_clicked(mud->panel_login_welcome,
                                    mud->control_welcome_existing_user)) {
            mud->login_screen = LOGIN_STAGE_EXISTING;

            if (is_compact) {
                panel_update_text(mud->panel_login_existing_user,
                                  mud->control_login_status, "");

                panel_update_text(mud->panel_login_existing_user,
                                  mud->control_login_status_bottom,
                                  "Please enter your username and password");
            } else {
                panel_update_text(mud->panel_login_existing_user,
                                  mud->control_login_status,
                                  "Please enter your username and password");
            }

            char *username =
                mud->options->remember_username ? mud->options->username : "";

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_username, username);

            char *password =
                mud->options->remember_password ? mud->options->password : "";

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_password, password);

            panel_set_focus(mud->panel_login_existing_user,
                            mud->control_login_username);
        } else if (panel_is_clicked(mud->panel_login_welcome,
                                    mud->control_welcome_options)) {
            mud->show_additional_options = 1;
        } else if (panel_is_clicked(mud->panel_login_welcome,
                                    mud->control_welcome_worlds)) {
            mud->login_screen = LOGIN_STAGE_WORLD;
        }
        return;
    case LOGIN_STAGE_NEW:
        panel_handle_mouse(mud->panel_login_new_user, mud->mouse_x,
                           mud->mouse_y, mud->last_mouse_button_down,
                           mud->mouse_button_down, mud->mouse_scroll_delta);

        if (mud->options->account_management) {
            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_cancel)) {
                mud->login_screen = 0;
                return;
            }

            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_user)) {
                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_password);
                return;
            }

            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_password)) {
                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_confirm_password);

                return;
            }

            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_confirm_password) ||
                panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_submit)) {
                char *username = panel_get_text(mud->panel_login_new_user,
                                                mud->control_register_user);

                char *password = panel_get_text(mud->panel_login_new_user,
                                                mud->control_register_password);

                char *confirm_password =
                    panel_get_text(mud->panel_login_new_user,
                                   mud->control_register_confirm_password);

                size_t password_length = strlen(password);

                if (strlen(username) == 0 || password_length == 0 ||
                    strlen(confirm_password) == 0) {
                    if (is_compact) {
                        panel_update_text(mud->panel_login_new_user,
                                          mud->control_register_status,
                                          "@yel@Please fill in ALL requested");
                        panel_update_text(mud->panel_login_new_user,
                                          mud->control_register_status_bottom,
                                          "@yel@information to continue!");
                    } else {
                        panel_update_text(
                            mud->panel_login_new_user,
                            mud->control_register_status,
                            "@yel@Please fill in ALL requested information to "
                            "continue!");
                    }

                    return;
                }

                if (strcmp(password, confirm_password) != 0) {
                    if (is_compact) {
                        panel_update_text(mud->panel_login_new_user,
                                          mud->control_register_status,
                                          "@yel@The two passwords entered");
                        panel_update_text(mud->panel_login_new_user,
                                          mud->control_register_status_bottom,
                                          "@yel@not the same as each other!");
                    } else {
                        panel_update_text(mud->panel_login_new_user,
                                          mud->control_register_status,
                                          "@yel@The two passwords entered are "
                                          "not the same as "
                                          "each other!");
                    }
                    return;
                }

                if (password_length < 5) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@Your password must be at least 5 letters long");

                    panel_update_text(mud->panel_login_new_user,
                                      mud->control_register_status_bottom, "");

                    return;
                }

                if (!panel_is_activated(mud->panel_login_new_user,
                                        mud->control_register_checkbox)) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@You must agree to the terms+conditions to "
                        "continue");

                    return;
                }

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_status,
                                  "Please wait... creating new account");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_status_bottom, "");

                mudclient_draw_login_screens(mud);
                mudclient_reset_timings(mud);

                mudclient_register(mud, username, password);
            }
        } else {
            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_login_new_ok)) {
                mud->login_screen = 0;
            }
        }
        return;
    case LOGIN_STAGE_EXISTING:
        panel_handle_mouse(mud->panel_login_existing_user, mud->mouse_x,
                           mud->mouse_y, mud->last_mouse_button_down,
                           mud->mouse_button_down, mud->mouse_scroll_delta);

        if (panel_is_clicked(mud->panel_login_existing_user,
                             mud->control_login_cancel)) {
            mud->login_screen = 0;
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_username)) {
            panel_set_focus(mud->panel_login_existing_user,
                            mud->control_login_password);
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_password) ||
                   panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_ok)) {

            strcpy(mud->login_username,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_username));
            strcpy(mud->login_pass,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_password));

            mudclient_login(mud, mud->login_username, mud->login_pass, 0);
        } /*else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_recover)) {
            strcpy(mud->login_username,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_username));

            if (strlen(mud->login_username) == 0) {
                mudclient_show_login_screen_status(
                    mud,
                    "You must enter your username to recover your password",
                    "");

                return;
            }

            // mudclient_recover_attempt(mud, mud->login_username);
        }*/
        return;
    case LOGIN_STAGE_RECOVER:
        // TODO
        return;
    case LOGIN_STAGE_WORLD:
        worldlist_handle_mouse(mud);
        return;
    }
}
