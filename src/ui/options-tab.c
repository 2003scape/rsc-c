#include "options-tab.h"

void mudclient_send_privacy_settings(mudclient *mud, int chat, int private_chat,
                                     int trade, int duel) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_SETTINGS_PRIVACY);
    packet_stream_put_byte(mud->packet_stream, chat);
    packet_stream_put_byte(mud->packet_stream, private_chat);
    packet_stream_put_byte(mud->packet_stream, trade);
    packet_stream_put_byte(mud->packet_stream, duel);
    packet_stream_send_packet(mud->packet_stream);
}

void mudclient_draw_change_password(mudclient *mud) {
    int dialog_x = (mud->surface->width / 2) - (CHANGE_PASSWORD_WIDTH / 2);

    int dialog_y =
        (mud->surface->height / 2) - (CHANGE_PASSWORD_HEIGHT / 2) + 7;

    if (mud->mouse_button_click != 0) {
        mud->mouse_button_click = 0;

        if (mud->mouse_x < dialog_x || mud->mouse_y < dialog_y ||
            mud->mouse_x > CHANGE_PASSWORD_WIDTH + dialog_x ||
            mud->mouse_y > CHANGE_PASSWORD_HEIGHT + dialog_y) {
            mud->show_change_password_step = 0;
            return;
        }
    }

    surface_draw_box(mud->surface, dialog_x, dialog_y, CHANGE_PASSWORD_WIDTH,
                     CHANGE_PASSWORD_HEIGHT, BLACK);

    surface_draw_border(mud->surface, dialog_x, dialog_y, CHANGE_PASSWORD_WIDTH,
                        CHANGE_PASSWORD_HEIGHT, WHITE);

    int x = mud->surface->width / 2;
    int y = dialog_y + 22;
    char password_input[PASSWORD_LENGTH + 2] = {0};

    if (mud->show_change_password_step == 6) {
        surface_draw_string_centre(
            mud->surface, "Please enter your current password", x, y, 4, WHITE);

        y += 25;

        int current_length = strlen(mud->input_text_current);

        for (int i = 0; i < current_length; i++) {
            password_input[i] = 'X';
        }

        password_input[current_length] = '*';

        surface_draw_string_centre(mud->surface, password_input, x, y, 4,
                                   WHITE);

        if (strlen(mud->input_text_final) > 0) {
            strcpy(mud->change_password_old, mud->input_text_final);
            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);
            mud->show_change_password_step = 1;
        }
    } else if (mud->show_change_password_step == 1) {
        surface_draw_string_centre(
            mud->surface, "Please enter your new password", x, y, 4, WHITE);

        y += 25;

        int current_length = strlen(mud->input_text_current);

        for (int i = 0; i < current_length; i++) {
            password_input[i] = 'X';
        }

        password_input[current_length] = '*';

        surface_draw_string_centre(mud->surface, password_input, x, y, 4,
                                   WHITE);

        if (strlen(mud->input_text_final) > 0) {
            strcpy(mud->change_password_new, mud->input_text_final);
            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

            if (strlen(mud->change_password_new) >= 5) {
                mud->show_change_password_step = 2;
            } else {
                mud->show_change_password_step = 5;
            }
        }
    } else if (mud->show_change_password_step == 2) {
        surface_draw_string_centre(
            mud->surface, "Enter password again to confirm", x, y, 4, WHITE);

        y += 25;

        int current_length = strlen(mud->input_text_current);

        for (int i = 0; i < current_length; i++) {
            password_input[i] = 'X';
        }

        password_input[current_length] = '*';

        surface_draw_string_centre(mud->surface, password_input, x, y, 4,
                                   WHITE);

        if (strlen(mud->input_text_final) > 0) {
            if (strcasecmp(mud->input_text_final, mud->change_password_new) ==
                0) {
                mud->show_change_password_step = 4;

                mudclient_change_password(mud, mud->change_password_old,
                                          mud->change_password_new);
            } else {
                mud->show_change_password_step = 3;
            }
        }
    } else {
        if (mud->show_change_password_step == 3) {
            surface_draw_string_centre(mud->surface, "Passwords do not match!",
                                       x, y, 4, WHITE);

            y += 25;

            surface_draw_string_centre(mud->surface, "Press any key to close",
                                       x, y, 4, WHITE);
        } else if (mud->show_change_password_step == 4) {
            surface_draw_string_centre(
                mud->surface, "Ok, your request has been sent", x, y, 4, WHITE);

            y += 25;

            surface_draw_string_centre(mud->surface, "Press any key to close",
                                       x, y, 4, WHITE);
        } else if (mud->show_change_password_step == 5) {
            surface_draw_string_centre(mud->surface, "Password must be at", x,
                                       y, 4, WHITE);

            y += 25;

            surface_draw_string_centre(mud->surface, "least 5 letters long", x,
                                       y, 4, WHITE);
        }
    }
}

void mudclient_draw_ui_tab_options(mudclient *mud, int no_menus) {
    int ui_x = mud->surface->width - OPTIONS_WIDTH - 3;
    int ui_y = 36;

    int height = 265;

    int is_touch = mudclient_is_touch(mud);

    int is_compact = is_touch || mud->surface->height <
                                     (height + (OPTIONS_LINE_BREAK * 2)) + ui_y;

    int player_x = mud->region_x + mud->local_region_x;
    int player_y = mud->region_y + mud->local_region_y;

    int show_skip_tutorial =
        mud->options->skip_tutorial && (player_x >= 190 && player_x <= 240 &&
                                        player_y >= 720 && player_y <= 770);

    if (is_compact) {
        height = 152;
    }

    if (mud->options->show_additional_options) {
        height += OPTIONS_LINE_BREAK;
    }

    if (show_skip_tutorial) {
        height += OPTIONS_LINE_BREAK;
    }

    if (is_touch) {
        height = 198;
        ui_x = UI_TABS_TOUCH_X - OPTIONS_WIDTH - 1;
        ui_y = (UI_TABS_TOUCH_Y + UI_TABS_TOUCH_HEIGHT) - height - 2;
    }

#if (VERSION_MEDIA >= 59)
    mudclient_draw_ui_tab_label(mud, OPTIONS_TAB, OPTIONS_WIDTH + !is_touch,
                                ui_x - !is_touch, ui_y - 10);
#endif

    mud->ui_tab_min_x = ui_x;
    mud->ui_tab_max_x = mud->surface->width;
    mud->ui_tab_min_y = 0;
    mud->ui_tab_max_y = height + 46;

    int controls_box_height = is_compact ? 48 : 65;

    if (mud->options->show_additional_options) {
        controls_box_height += OPTIONS_LINE_BREAK;
    }

    surface_draw_box_alpha(mud->surface, ui_x, ui_y, OPTIONS_WIDTH,
                           controls_box_height, GREY_B5, 160);

    int security_box_height = is_compact ? 15 : 65;

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + controls_box_height,
                           OPTIONS_WIDTH, security_box_height, GREY_C9, 160);

    int privacy_box_height = is_compact ? 60 : 95;

    surface_draw_box_alpha(mud->surface, ui_x,
                           ui_y + security_box_height + controls_box_height,
                           OPTIONS_WIDTH, privacy_box_height, GREY_B5, 160);

    int logout_box_height = is_compact ? 29 : 40;

    if (show_skip_tutorial) {
        logout_box_height += OPTIONS_LINE_BREAK;
    }

    surface_draw_box_alpha(mud->surface, ui_x,
                           ui_y + privacy_box_height + security_box_height +
                               controls_box_height,
                           OPTIONS_WIDTH, logout_box_height, GREY_C9, 160);

    int x = ui_x + 3;
    int y = ui_y + OPTIONS_LINE_BREAK;

    if (!is_compact) {
        surface_draw_string(mud->surface, "Game options - click to toggle", x,
                            y, 1, BLACK);

        y += OPTIONS_LINE_BREAK;
    }

    char settings_string[64] = {0};

    sprintf(settings_string, "Camera angle mode - %s",
            mud->settings_camera_auto ? "@gre@Auto" : "@red@Manual");

    surface_draw_string(mud->surface, settings_string, x, y, 1, WHITE);

    y += OPTIONS_LINE_BREAK;

    sprintf(settings_string, "Mouse buttons - %s",
            (mud->settings_mouse_button_one ? "@red@One" : "@gre@Two"));

    surface_draw_string(mud->surface, settings_string, x, y, 1, WHITE);

    y += OPTIONS_LINE_BREAK;

    if (mud->options->members) {
        sprintf(settings_string, "Sound effects - %s",
                (mud->settings_sound_disabled ? "@red@off" : "@gre@on"));

        surface_draw_string(mud->surface, settings_string, x, y, 1, WHITE);
    }

    y += OPTIONS_LINE_BREAK;

    if (mud->options->show_additional_options) {
        int text_colour = WHITE;

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "Additional options...", x, y, 1,
                            text_colour);

        y += OPTIONS_LINE_BREAK;
    }

    if (mud->options->account_management) {
        if (!is_compact) {
            y += 5;

            surface_draw_string(mud->surface, "Security settings", x, y, 1, 0);

            y += OPTIONS_LINE_BREAK;
        }

        int text_colour = WHITE;

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "Change password", x, y, 1,
                            text_colour);

        y += OPTIONS_LINE_BREAK;

        /*text_colour = WHITE;

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "Change recovery questions", x, y, 1,
                            text_colour);*/

        if (!is_compact) {
            y += OPTIONS_LINE_BREAK * 2;
        }
    } else {
        surface_draw_string(mud->surface, "To change your contact details,", x,
                            y, FONT_REGULAR_11, WHITE);

        y += OPTIONS_LINE_BREAK;

        surface_draw_string(mud->surface, "password, recovery questions, etc..",
                            x, y, FONT_REGULAR_11, WHITE);

        y += OPTIONS_LINE_BREAK;

        surface_draw_string(mud->surface, "Please select 'account management'",
                            x, y, FONT_REGULAR_11, WHITE);

        y += OPTIONS_LINE_BREAK;

        char *location = "";

        if (mud->refer_id == 0) {
            location = "from the runescape.com front page";
        } else if (mud->refer_id == 1) {
            location = "from the link below the gamewindow";
        } else {
            location = "from the runescape front webpage";
        }

        surface_draw_string(mud->surface, location, x, y, FONT_REGULAR_11,
                            WHITE);

        y += OPTIONS_LINE_BREAK + 5;
    }

    if (!is_compact) {
        surface_draw_string(mud->surface,
                            "Privacy settings. Will be applied to", ui_x + 3, y,
                            1, BLACK);

        y += OPTIONS_LINE_BREAK;

        surface_draw_string(mud->surface, "all people not on your friends list",
                            ui_x + 3, y, 1, BLACK);

        y += OPTIONS_LINE_BREAK;
    }

    sprintf(settings_string, "Block chat messages: %s",
            (!mud->settings_block_chat ? "@red@<off>" : "@gre@<on>"));

    surface_draw_string(mud->surface, settings_string, ui_x + 3, y, 1, WHITE);

    y += OPTIONS_LINE_BREAK;

    sprintf(settings_string, "Block private messages: %s",
            (!mud->settings_block_private ? "@red@<off>" : "@gre@<on>"));

    surface_draw_string(mud->surface, settings_string, ui_x + 3, y, 1, WHITE);

    y += OPTIONS_LINE_BREAK;

    sprintf(settings_string, "Block trade requests: %s",
            (!mud->settings_block_trade ? "@red@<off>" : "@gre@<on>"));

    surface_draw_string(mud->surface, settings_string, ui_x + 3, y, 1, WHITE);

    y += OPTIONS_LINE_BREAK;

    if (mud->options->members) {
        sprintf(settings_string, "Block duel requests: %s",
                (!mud->settings_block_duel ? "@red@<off>" : "@gre@<on>"));

        surface_draw_string(mud->surface, settings_string, ui_x + 3, y, 1,
                            WHITE);
    }

    y += OPTIONS_LINE_BREAK + (is_compact ? 0 : 5);

    int text_colour = WHITE;

    if (show_skip_tutorial) {
        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "Skip the tutorial", x, y, 1,
                            text_colour);

        if (is_compact) {
            y += OPTIONS_LINE_BREAK - 1;
        } else {
            y += OPTIONS_LINE_BREAK + 5;
        }
    }

    surface_draw_string(mud->surface, "Always logout when you finish", x, y, 1,
                        BLACK);

    y += OPTIONS_LINE_BREAK - (is_compact ? 2 : 0);

    text_colour = WHITE;

    if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
        mud->mouse_y > y - 12 && mud->mouse_y < y + 4) {
        text_colour = YELLOW;
    }

    surface_draw_string(mud->surface, "Click here to logout", ui_x + 3, y, 1,
                        text_colour);

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < 196 && mouse_y < height) {
        int x = ui_x + 3;

        int y =
            ui_y + OPTIONS_LINE_BREAK + (is_compact ? 0 : OPTIONS_LINE_BREAK);

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
            mud->mouse_button_click == 1) {
            mud->settings_camera_auto = !mud->settings_camera_auto;
            packet_stream_new_packet(mud->packet_stream, CLIENT_SETTINGS_GAME);
            packet_stream_put_byte(mud->packet_stream, 0);

            packet_stream_put_byte(mud->packet_stream,
                                   mud->settings_camera_auto ? 1 : 0);

            packet_stream_send_packet(mud->packet_stream);
        }

        y += OPTIONS_LINE_BREAK;

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
            mud->mouse_button_click == 1) {
            mud->settings_mouse_button_one = !mud->settings_mouse_button_one;
            packet_stream_new_packet(mud->packet_stream, CLIENT_SETTINGS_GAME);
            packet_stream_put_byte(mud->packet_stream, 2);

            packet_stream_put_byte(mud->packet_stream,
                                   mud->settings_mouse_button_one ? 1 : 0);

            packet_stream_send_packet(mud->packet_stream);
        }

        y += OPTIONS_LINE_BREAK;

        if (mud->options->members && mud->mouse_x > x &&
            mud->mouse_x < x + OPTIONS_WIDTH && mud->mouse_y > y - 12 &&
            mud->mouse_y < y + 4 && mud->mouse_button_click == 1) {
            mud->settings_sound_disabled = !mud->settings_sound_disabled;
            packet_stream_new_packet(mud->packet_stream, CLIENT_SETTINGS_GAME);
            packet_stream_put_byte(mud->packet_stream, 3);

            packet_stream_put_byte(mud->packet_stream,
                                   mud->settings_sound_disabled ? 1 : 0);

            packet_stream_send_packet(mud->packet_stream);
        }

        y += OPTIONS_LINE_BREAK;

        if (mud->options->show_additional_options) {
            if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
                mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
                mud->mouse_button_click == 1) {
                mud->show_additional_options = 1;
            }

            y += OPTIONS_LINE_BREAK;
        }

        if (mud->options->account_management) {
            if (!is_compact) {
                y += OPTIONS_LINE_BREAK + 5;
            }

            if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
                mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
                mud->mouse_button_click == 1) {
                mud->show_change_password_step = 6;
                mud->input_text_current[0] = '\0';
                mud->input_text_final[0] = '\0';
            }

            /*if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
                mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
                mud->mouse_button_click == 1) {
                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_RECOVER_SET_REQUEST);

                packet_stream_send_packet(mud->packet_stream);
            }*/

            if (!is_compact) {
                y += OPTIONS_LINE_BREAK * 3;
            }
        } else {
            if (!is_compact) {
                y += OPTIONS_LINE_BREAK * 4;
            }
        }

        int has_changed_setting = 0;

        if (is_compact) {
            y += OPTIONS_LINE_BREAK;
        } else {
            y += OPTIONS_LINE_BREAK * 2;
        }

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
            mud->mouse_button_click == 1) {
            mud->settings_block_chat = 1 - mud->settings_block_chat;
            has_changed_setting = 1;
        }

        y += OPTIONS_LINE_BREAK;

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
            mud->mouse_button_click == 1) {
            mud->settings_block_private = 1 - mud->settings_block_private;
            has_changed_setting = 1;
        }

        y += OPTIONS_LINE_BREAK;

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
            mud->mouse_button_click == 1) {
            mud->settings_block_trade = 1 - mud->settings_block_trade;
            has_changed_setting = 1;
        }

        y += OPTIONS_LINE_BREAK;

        if (mud->options->members && mud->mouse_x > x &&
            mud->mouse_x < x + OPTIONS_WIDTH && mud->mouse_y > y - 12 &&
            mud->mouse_y < y + 4 && mud->mouse_button_click == 1) {
            mud->settings_block_duel = 1 - mud->settings_block_duel;
            has_changed_setting = 1;
        }

        y += OPTIONS_LINE_BREAK;

        if (has_changed_setting) {
            mudclient_send_privacy_settings(
                mud, mud->settings_block_chat, mud->settings_block_private,
                mud->settings_block_trade, mud->settings_block_duel);
        }

        if (show_skip_tutorial) {
            if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
                mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
                mud->mouse_button_click == 1) {
                mud->confirm_text_top =
                    "Are you sure you wish to skip the tutorial";

                mud->confirm_text_bottom = "and teleport to Lumbridge?";

                mud->confirm_type = CONFIRM_TUTORIAL;
                mud->show_dialog_confirm = 1;
            }

            if (is_compact) {
                y += OPTIONS_LINE_BREAK - 1;
            } else {
                y += OPTIONS_LINE_BREAK + 5;
            }
        }

        y += OPTIONS_LINE_BREAK - (is_compact ? 2 : 0);

        if (mud->mouse_x > x && mud->mouse_x < x + OPTIONS_WIDTH &&
            mud->mouse_y > y - 12 && mud->mouse_y < y + 4 &&
            mud->mouse_button_click == 1) {
            mudclient_send_logout(mud);
        }

        mud->mouse_button_click = 0;
    }
}
