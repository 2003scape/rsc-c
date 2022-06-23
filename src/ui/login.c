#include "login.h"

void mudclient_create_login_panels(mudclient *mud) {
    mud->panel_login_welcome = malloc(sizeof(Panel));
    panel_new(mud->panel_login_welcome, mud->surface, 50);

    int x = mud->game_width / 2;
    int y = 40;

    if (!mud->members) {
        panel_add_text_centre(mud->panel_login_welcome, x, 200 + y,
                              "Click on an option", 5, 1);

        panel_add_button_background(mud->panel_login_welcome, x - 100, 240 + y,
                                    120, 35);

        panel_add_text_centre(mud->panel_login_welcome, x - 100, 240 + y,
                              "New User", 5, 0);

        mud->control_welcome_new_user = panel_add_button(
            mud->panel_login_welcome, x - 100, 240 + y, 120, 35);

        panel_add_button_background(mud->panel_login_welcome, x + 100, 240 + y,
                                    120, 35);

        panel_add_text_centre(mud->panel_login_welcome, x + 100, 240 + y,
                              "Existing User", 5, 0);

        mud->control_welcome_existing_user = panel_add_button(
            mud->panel_login_welcome, x + 100, 240 + y, 120, 35);
    } else {
        panel_add_text_centre(mud->panel_login_welcome, x, 200 + y,
                              "Welcome to RuneScape", 4, 1);

        panel_add_text_centre(mud->panel_login_welcome, x, 215 + y,
                              "You need a members account to use this server",
                              4, 1);

        panel_add_button_background(mud->panel_login_welcome, x, 250 + y, 200,
                                    35);

        panel_add_text_centre(mud->panel_login_welcome, x, 250 + y,
                              "Click here to login", 5, 0);

        mud->control_welcome_existing_user =
            panel_add_button(mud->panel_login_welcome, x, 250 + y, 200, 35);
    }

    mud->panel_login_new_user = malloc(sizeof(Panel));
    panel_new(mud->panel_login_new_user, mud->surface, 50);

    if (!mud->options->account_management) {
        y = 230;

        if (mud->refer_id == 0) {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "to create an account please go back to the",
                                  4, 1);

            y += 20;

            panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "www.runescape.com front page, and choose 'create account'", 4,
                1);
        } else if (mud->refer_id == 1) {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "to create an account please click on the", 4,
                                  1);

            y += 20;

            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "'create account' link below the game window",
                                  4, 1);
        } else {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "To create an account please go back to the",
                                  4, 1);

            y += 20;

            panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "runescape front webpage and choose 'create account'", 4, 1);
        }

        y += 30;

        panel_add_button_background(mud->panel_login_new_user, x, y + 17, 150,
                                    34);

        panel_add_text_centre(mud->panel_login_new_user, x, y + 17, "Ok", 5, 0);

        mud->control_login_new_ok =
            panel_add_button(mud->panel_login_new_user, x, y + 17, 150, 34);
    } else {
        y = 70;

        mud->control_register_status = panel_add_text_centre(
            mud->panel_login_new_user, x, y + 8,
            "to create an account please enter all the requested details", 4,
            1);

        int offset_y = y + 25;

        panel_add_button_background(mud->panel_login_new_user, x, offset_y + 17,
                                    250, 34);

        panel_add_text_centre(mud->panel_login_new_user, x, offset_y + 8,
                              "Choose a username", 4, 0);

        mud->control_register_user = panel_add_text_input(
            mud->panel_login_new_user, x, offset_y + 25, 200, 40, 4, 12, 0, 0);

        offset_y += 40;

        panel_add_button_background(mud->panel_login_new_user, x - 115,
                                    offset_y + 17, 220, 34);

        panel_add_text_centre(mud->panel_login_new_user, x - 115, offset_y + 8,
                              "Choose a Password", 4, 0);

        mud->control_register_password =
            panel_add_text_input(mud->panel_login_new_user, x - 115,
                                 offset_y + 25, 220, 40, 4, 20, 1, 0);

        panel_add_button_background(mud->panel_login_new_user, x + 115,
                                    offset_y + 17, 220, 34);

        panel_add_text_centre(mud->panel_login_new_user, x + 115, offset_y + 8,
                              "Confirm Password", 4, 0);

        mud->control_register_confirm_password =
            panel_add_text_input(mud->panel_login_new_user, x + 115,
                                 offset_y + 25, 220, 40, 4, 20, 1, 0);

        offset_y += 60;

        mud->control_register_checkbox = panel_add_checkbox(
            mud->panel_login_new_user, x - 196 - 7, offset_y - 7, 14, 14);

        panel_add_text(mud->panel_login_new_user, x - 181, offset_y,
                       "I have read and agreed to the terms and conditions", 4,
                       1);

        offset_y += 15;

        panel_add_text_centre(
            mud->panel_login_new_user, x, offset_y,
            "(to view these click the relevant link below this game window)", 4,
            1);

        offset_y += 20;

        panel_add_button_background(mud->panel_login_new_user, x - 100,
                                    offset_y + 17, 150, 34);

        panel_add_text_centre(mud->panel_login_new_user, x - 100, offset_y + 17,
                              "Submit", 5, 0);

        mud->control_register_submit = panel_add_button(
            mud->panel_login_new_user, x - 100, offset_y + 17, 150, 34);

        panel_add_button_background(mud->panel_login_new_user, x + 100,
                                    offset_y + 17, 150, 34);

        panel_add_text_centre(mud->panel_login_new_user, x + 100, offset_y + 17,
                              "Cancel", 5, 0);

        mud->control_register_cancel = panel_add_button(
            mud->panel_login_new_user, x + 100, offset_y + 17, 150, 34);
    }

    mud->panel_login_existing_user = malloc(sizeof(Panel));
    panel_new(mud->panel_login_existing_user, mud->surface, 50);

    y = 230;

    mud->control_login_status =
        panel_add_text_centre(mud->panel_login_existing_user, x, y - 10,
                              "Please enter your username and password", 4, 1);

    y += 28;

    panel_add_button_background(mud->panel_login_existing_user, x - 116, y, 200,
                                40);

    panel_add_text_centre(mud->panel_login_existing_user, x - 116, y - 10,
                          "Username:", 4, 0);

    mud->control_login_user = panel_add_text_input(
        mud->panel_login_existing_user, x - 116, y + 10, 200, 40, 4, 12, 0, 0);

    y += 47;

    panel_add_button_background(mud->panel_login_existing_user, x - 66, y, 200,
                                40);

    panel_add_text_centre(mud->panel_login_existing_user, x - 66, y - 10,
                          "Password:", 4, 0);

    mud->control_login_password = panel_add_text_input(
        mud->panel_login_existing_user, x - 66, y + 10, 200, 40, 4, 20, 1, 0);

    y -= 55;

    panel_add_button_background(mud->panel_login_existing_user, x + 154, y, 120,
                                25);
    panel_add_text_centre(mud->panel_login_existing_user, x + 154, y, "Ok", 4,
                          0);

    mud->control_login_ok =
        panel_add_button(mud->panel_login_existing_user, x + 154, y, 120, 25);

    y += 30;

    panel_add_button_background(mud->panel_login_existing_user, x + 154, y, 120,
                                25);
    panel_add_text_centre(mud->panel_login_existing_user, x + 154, y, "Cancel",
                          4, 0);

    mud->control_login_cancel =
        panel_add_button(mud->panel_login_existing_user, x + 154, y, 120, 25);

    if (mud->options->account_management) {
        y += 30;

        /*panel_add_button_background(mud->panel_login_existing_user, x + 154, y,
                                    160, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154, y,
                              "I've lost my password", 4, 0);

        mud->control_login_recover = panel_add_button(
            mud->panel_login_existing_user, x + 154, y, 160, 25);*/
    }
}

void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1) {

    if (mud->login_screen == 1) {
        sprintf(login_screen_status, "%s %s", s, s1);

        panel_update_text(mud->panel_login_new_user,
                          mud->control_register_status, login_screen_status);
    } else if (mud->login_screen == 2) {
        sprintf(login_screen_status, "%s %s", s, s1);

        panel_update_text(mud->panel_login_existing_user,
                          mud->control_login_status, login_screen_status);
    }

    mudclient_draw_login_screens(mud);
    mudclient_reset_timings(mud);
}

void mudclient_reset_login_screen(mudclient *mud) {
    mud->logged_in = 0;
    mud->login_screen = 0;

    memset(mud->login_user, '\0', USERNAME_LENGTH + 1);
    memset(mud->login_pass, '\0', PASSWORD_LENGTH + 1);

    mud->login_prompt = "Please enter a username:";
    sprintf(mud->login_user_disp, "*%s*", mud->login_user);
    mud->player_count = 0;
    mud->npc_count = 0;
}

void mudclient_render_login_screen_viewports(mudclient *mud) {
    int plane = 0;
    int region_x = 50; // 49;
    int region_y = 50; // 47;

    world_load_section_from3(mud->world, region_x * 48 + 23, region_y * 48 + 23,
                             plane);

    world_add_models(mud->world, mud->game_models);

    int x = 9728;
    int y = 6400;
    int zoom = 1100;
    int rotation = 888;

    mud->scene->clip_far_3d = 4100;
    mud->scene->clip_far_2d = 4100;
    mud->scene->fog_z_falloff = 1;
    mud->scene->fog_z_distance = 4000;

#ifdef RENDER_GL
    /* clear the previous buffer */
    surface_gl_reset_context(mud->surface);
#endif

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

    scene_render(mud->scene);

    //surface_draw_box(mud->surface, 0, 0, mud->game_width, 250, WHITE);

    surface_apply_login_filter(mud->surface);

    surface_draw_sprite_from3(
        mud->surface,
        (mud->game_width / 2) -
            (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
        15, mud->sprite_media + 10);

    /* TODO: resizable logo
    surface_sprite_clipping_from9(
           mud->surface,
           (mud->game_width / 2) -
               (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
           15, 400, 120, mud->sprite_media + 10, 0, 0, 0, 0);*/

    surface_draw_sprite_from5(mud->surface, mud->sprite_logo, 0, 0,
                              mud->game_width, 200);

#ifdef RENDER_GL
    surface_gl_draw(mud->surface);
#endif

    surface_screen_raster_to_sprite(mud->surface, mud->sprite_logo);

    x = 9216;
    y = 9216;
    zoom = 1100;
    rotation = 888;

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

    scene_render(mud->scene);

    surface_apply_login_filter(mud->surface);

    surface_draw_sprite_from3(
        mud->surface,
        (mud->game_width / 2) -
            (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
        15, mud->sprite_media + 10);

    surface_draw_sprite_from5(mud->surface, mud->sprite_logo + 1, 0, 0,
                              mud->game_width, 200);

#ifdef RENDER_GL
    surface_gl_draw(mud->surface);
#endif

    surface_screen_raster_to_sprite(mud->surface, mud->sprite_logo + 1);

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

    scene_render(mud->scene);

    surface_apply_login_filter(mud->surface);

    surface_draw_sprite_from3(
        mud->surface,
        (mud->game_width / 2) -
            (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
        15, mud->sprite_media + 10);

    surface_draw_sprite_from5(mud->surface, mud->sprite_media + 10, 0, 0,
                              mud->game_width, 200);

#ifdef RENDER_GL
    surface_gl_draw(mud->surface);
#endif

    surface_screen_raster_to_sprite(mud->surface, mud->sprite_media + 10);

    world_reset(mud->world, 0);

    for (int i = 0; i < mud->scene->model_count; i++) {
        game_model_destroy(mud->scene->models[i]);
        free(mud->scene->models[i]);
        mud->scene->models[i] = NULL;
    }

    mud->scene->model_count = 0;
}

void mudclient_draw_login_screens(mudclient *mud) {
    mud->welcome_screen_already_shown = 0;
    mud->surface->interlace = 0;

    surface_black_screen(mud->surface);

    int show_background = 0;

    if (mud->options->account_management) {
        show_background = mud->login_screen == 0 || mud->login_screen == 2;
    } else {
        show_background = mud->login_screen >= 0 && mud->login_screen <= 3;
    }

    if (show_background) {
        int cycle = (mud->login_timer * 2) % 3072;

        if (cycle < 1024) {
            surface_draw_sprite_from3(mud->surface, 0, 10, mud->sprite_logo);

            if (cycle > 768) {
                surface_draw_sprite_alpha_from4(
                    mud->surface, 0, 10, mud->sprite_logo + 1, cycle - 768);
            }
        } else if (cycle < 2048) {
            surface_draw_sprite_from3(mud->surface, 0, 10,
                                      mud->sprite_logo + 1);

            if (cycle > 1792) {
                surface_draw_sprite_alpha_from4(
                    mud->surface, 0, 10, mud->sprite_media + 10, cycle - 1792);
            }
        } else {
            surface_draw_sprite_from3(mud->surface, 0, 10,
                                      mud->sprite_media + 10);

            if (cycle > 2816) {
                surface_draw_sprite_alpha_from4(mud->surface, 0, 10,
                                                mud->sprite_logo, cycle - 2816);
            }
        }
    }

    if (mud->login_screen == 0) {
        panel_draw_panel(mud->panel_login_welcome);
    } else if (mud->login_screen == 1) {
        panel_draw_panel(mud->panel_login_new_user);
    } else if (mud->login_screen == 2) {
        panel_draw_panel(mud->panel_login_existing_user);
    }

    /* blue bar */
    surface_draw_sprite_from3(mud->surface, 0, mud->game_height - 4,
                              mud->sprite_media + 22);

    surface_draw(mud->surface);
}

void mudclient_handle_login_screen_input(mudclient *mud) {
    if (mud->world_full_timeout > 0) {
        mud->world_full_timeout--;
    }

    if (mud->login_screen == 0) {
        panel_handle_mouse(mud->panel_login_welcome, mud->mouse_x, mud->mouse_y,
                           mud->last_mouse_button_down, mud->mouse_button_down,
                           mud->mouse_scroll_delta);

        if (panel_is_clicked(mud->panel_login_welcome,
                             mud->control_welcome_new_user)) {
            mud->login_screen = 1;

            if (mud->options->account_management) {
                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_user, "");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_password, "");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_confirm_password, "");

                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_user);

                panel_toggle_checkbox(mud->panel_login_new_user,
                                      mud->control_register_checkbox, 0);

                panel_update_text(
                    mud->panel_login_new_user, mud->control_register_status,
                    "To create an account please enter all the requested "
                    "details");
            }
        } else if (panel_is_clicked(mud->panel_login_welcome,
                                    mud->control_welcome_existing_user)) {
            mud->login_screen = 2;

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_status,
                              "Please enter your username and password");

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_user, "");

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_password, "");

            panel_set_focus(mud->panel_login_existing_user,
                            mud->control_login_user);

            return;
        }
    } else if (mud->login_screen == 1) {
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

                int password_length = strlen(password);

                if (strlen(username) == 0 || password_length == 0 ||
                    strlen(confirm_password) == 0) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@Please fill in ALL requested information to "
                        "continue!");

                    return;
                }

                if (strcmp(password, confirm_password) != 0) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@The two passwords entered are not the same as "
                        "each other!");

                    return;
                }

                if (password_length < 5) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@Your password must be at least 5 letters long");

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
    } else if (mud->login_screen == 2) {
        panel_handle_mouse(mud->panel_login_existing_user, mud->mouse_x,
                           mud->mouse_y, mud->last_mouse_button_down,
                           mud->mouse_button_down, mud->mouse_scroll_delta);

        if (panel_is_clicked(mud->panel_login_existing_user,
                             mud->control_login_cancel)) {
            mud->login_screen = 0;
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_user)) {
            panel_set_focus(mud->panel_login_existing_user,
                            mud->control_login_password);
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_password) ||
                   panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_ok)) {

            strcpy(mud->login_user,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_user));
            strcpy(mud->login_pass,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_password));

            mudclient_login(mud, mud->login_user, mud->login_pass, 0);
        } /*else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_recover)) {
            strcpy(mud->login_user,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_user));

            if (strlen(mud->login_user) == 0) {
                mudclient_show_login_screen_status(
                    mud,
                    "You must enter your username to recover your password",
                    "");

                return;
            }

            // mudclient_recover_attempt(mud, mud->login_user);
        }*/
    }
}
