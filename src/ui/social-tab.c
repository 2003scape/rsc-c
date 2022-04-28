#include "social-tab.h"

char *social_tabs[] = {"Friends", "Ignore"};

void mudclient_sort_friends(mudclient *mud) {
    int flag = 1;

    while (flag) {
        flag = 0;

        for (int i = 0; i < mud->friend_list_count - 1; i++) {
            if ((mud->friend_list_online[i] != FRIEND_ONLINE &&
                 mud->friend_list_online[i + 1] == FRIEND_ONLINE) ||
                (mud->friend_list_online[i] == 0 &&
                 mud->friend_list_online[i + 1] != 0)) {
                int online_status = mud->friend_list_online[i];
                mud->friend_list_online[i] = mud->friend_list_online[i + 1];
                mud->friend_list_online[i + 1] = online_status;

                int64_t encoded_username = mud->friend_list_hashes[i];
                mud->friend_list_hashes[i] = mud->friend_list_hashes[i + 1];
                mud->friend_list_hashes[i + 1] = encoded_username;

                flag = 1;
            }
        }
    }
}

void mudclient_add_friend(mudclient *mud, char *username) {
    int64_t encoded_username = encode_username(username);

    packet_stream_new_packet(mud->packet_stream, CLIENT_FRIEND_ADD);
    packet_stream_put_long(mud->packet_stream, encoded_username);
    packet_stream_send_packet(mud->packet_stream);

    for (int i = 0; i < mud->friend_list_count; i++) {
        if (mud->friend_list_hashes[i] == encoded_username) {
            return;
        }
    }

    if (mud->friend_list_count >= SOCIAL_LIST_MAX) {
        return;
    }

    mud->friend_list_hashes[mud->friend_list_count] = encoded_username;
    mud->friend_list_online[mud->friend_list_count] = 0;
    mud->friend_list_count++;
}

void mudclient_remove_friend(mudclient *mud, int64_t encoded_username) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_FRIEND_REMOVE);
    packet_stream_put_long(mud->packet_stream, encoded_username);
    packet_stream_send_packet(mud->packet_stream);

    for (int i = 0; i < mud->friend_list_count; i++) {
        if (mud->friend_list_hashes[i] != encoded_username) {
            continue;
        }

        mud->friend_list_count--;

        for (int j = i; j < mud->friend_list_count; j++) {
            mud->friend_list_hashes[j] = mud->friend_list_hashes[j + 1];
            mud->friend_list_online[j] = mud->friend_list_online[j + 1];
        }

        break;
    }

    char username[USERNAME_LENGTH] = {0};
    decode_username(encoded_username, username);

    char formatted[USERNAME_LENGTH + 46] = {0};

    sprintf(formatted, "@pri@%s has been removed from your friends list",
            username);

    mudclient_show_server_message(mud, formatted);
}

void mudclient_add_ignore(mudclient *mud, char *username) {
    int64_t encoded_username = encode_username(username);

    packet_stream_new_packet(mud->packet_stream, CLIENT_IGNORE_ADD);
    packet_stream_put_long(mud->packet_stream, encoded_username);
    packet_stream_send_packet(mud->packet_stream);

    for (int i = 0; i < mud->ignore_list_count; i++) {
        if (mud->ignore_list[i] == encoded_username) {
            return;
        }
    }

    if (mud->ignore_list_count >= SOCIAL_LIST_MAX) {
        return;
    }

    mud->ignore_list[mud->ignore_list_count++] = encoded_username;
}

void mudclient_remove_ignore(mudclient *mud, int64_t encoded_username) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_IGNORE_REMOVE);
    packet_stream_put_long(mud->packet_stream, encoded_username);
    packet_stream_send_packet(mud->packet_stream);

    for (int i = 0; i < mud->ignore_list_count; i++) {
        if (mud->ignore_list[i] != encoded_username) {
            continue;
        }

        mud->ignore_list_count--;

        for (int j = i; j < mud->ignore_list_count; j++) {
            mud->ignore_list[j] = mud->ignore_list[j + 1];
        }

        return;
    }
}

void mudclient_send_private_message(mudclient *mud, int64_t username,
                                    int8_t *message, int length) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_PM);
    packet_stream_put_long(mud->packet_stream, username);
    packet_stream_put_bytes(mud->packet_stream, message, 0, length);
    packet_stream_send_packet(mud->packet_stream);
}

void mudclient_draw_ui_tab_social(mudclient *mud, int no_menus) {
    int ui_x = mud->game_width - SOCIAL_WIDTH - 3;
    int ui_y = 36;

    surface_draw_sprite_from3(mud->surface, mud->game_width - UI_TABS_WIDTH - 3,
                              3, mud->sprite_media + SOCIAL_TAB_SPRITE_OFFSET);

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + SOCIAL_TAB_HEIGHT,
                           SOCIAL_WIDTH, SOCIAL_HEIGHT - SOCIAL_TAB_HEIGHT,
                           GREY_DC, 128);

    surface_draw_line_horizontal(mud->surface, ui_x, ui_y + SOCIAL_HEIGHT - 16,
                                 SOCIAL_WIDTH, BLACK);

    surface_draw_tabs(mud->surface, ui_x, ui_y, SOCIAL_WIDTH, SOCIAL_TAB_HEIGHT,
                      social_tabs, 2, mud->ui_tab_social_sub_tab);

    panel_clear_list(mud->panel_social_list, mud->control_list_social);

    if (mud->ui_tab_social_sub_tab == 0) {
        for (int i = 0; i < mud->friend_list_count; i++) {
            char colour[6] = "@red@";

            if (mud->friend_list_online[i] == FRIEND_ONLINE) {
                strcpy(colour, "@gre@");
            } else if (mud->friend_list_online[i] > 0) {
                strcpy(colour, "@yel@");
            }

            char username[USERNAME_LENGTH + 1] = {0};
            decode_username(mud->friend_list_hashes[i], username);

            char formatted_username[USERNAME_LENGTH + 30] = {0};

            sprintf(formatted_username, "%s%s~%d~@whi@Remove", colour, username,
                    ui_x + 126);

            panel_add_list_entry(mud->panel_social_list,
                                 mud->control_list_social, i,
                                 formatted_username);
        }
    } else if (mud->ui_tab_social_sub_tab == 1) {
        for (int i = 0; i < mud->ignore_list_count; i++) {
            char username[USERNAME_LENGTH + 1] = {0};
            decode_username(mud->ignore_list[i], username);

            char formatted_username[USERNAME_LENGTH + 30] = {0};

            sprintf(formatted_username, "@yel@%s~%d~@whi@Remove", username,
                    ui_x + 126);

            panel_add_list_entry(mud->panel_social_list,
                                 mud->control_list_social, i,
                                 formatted_username);
        }
    }

    panel_draw_panel(mud->panel_social_list);

    if (mud->ui_tab_social_sub_tab == 0) {
        int friend_index = panel_get_list_entry_index(mud->panel_social_list,
                                                      mud->control_list_social);

        char formatted[48] = {0};
        strcpy(formatted, "Click a name to send a message");

        if (friend_index >= 0 && mud->mouse_x < ui_x + 176) {
            char username[USERNAME_LENGTH + 1] = {0};
            decode_username(mud->friend_list_hashes[friend_index], username);

            if (mud->mouse_x > ui_x + 116) {
                sprintf(formatted, "Click to remove %s", username);
            } else if (mud->friend_list_online[friend_index] == FRIEND_ONLINE) {
                sprintf(formatted, "Click to message %s", username);
            } else if (mud->friend_list_online[friend_index] > 0) {
#ifdef REVISION_177
                sprintf(formatted, "%s is on world %d", username,
                        mud->friend_list_online[friend_index]);
#else
                if (mud->friend_list_online[friend_index] < 200) {
                    sprintf(formatted, "%s is on world %d", username,
                            mud->friend_list_online[friend_index] - 9);
                } else {
                    sprintf(formatted, "%s is on classic %d", username,
                            mud->friend_list_online[friend_index] - 219);
                }
#endif
            } else {
                sprintf(formatted, "%s is offline", username);
            }
        }

        surface_draw_string_centre(mud->surface, formatted,
                                   ui_x + (SOCIAL_WIDTH / 2), ui_y + 35, 1,
                                   WHITE);

        int text_colour = BLACK;

        if (mud->mouse_x > ui_x && mud->mouse_x < ui_x + SOCIAL_WIDTH &&
            mud->mouse_y > ui_y + SOCIAL_HEIGHT - 16 &&
            mud->mouse_y < ui_y + SOCIAL_HEIGHT) {
            text_colour = YELLOW;
        } else {
            text_colour = WHITE;
        }

        surface_draw_string_centre(mud->surface, "Click here to add a friend",
                                   ui_x + (SOCIAL_WIDTH / 2),
                                   ui_y + SOCIAL_HEIGHT - 3, 1, text_colour);
    } else if (mud->ui_tab_social_sub_tab == 1) {
        int ignore_index = panel_get_list_entry_index(mud->panel_social_list,
                                                      mud->control_list_social);

        char formatted[USERNAME_LENGTH + 17] = {0};
        strcpy(formatted, "Blocking messages from:");

        if (ignore_index >= 0 && mud->mouse_x < ui_x + 176 &&
            mud->mouse_x > ui_x + 116) {
            char username[USERNAME_LENGTH + 1] = {0};
            decode_username(mud->ignore_list[ignore_index], username);
            sprintf(formatted, "Click to remove %s", username);
        }

        surface_draw_string_centre(mud->surface, formatted,
                                   ui_x + (SOCIAL_WIDTH / 2), ui_y + 35, 1,
                                   WHITE);

        int text_colour = BLACK;

        if (mud->mouse_x > ui_x && mud->mouse_x < ui_x + SOCIAL_WIDTH &&
            mud->mouse_y > ui_y + SOCIAL_HEIGHT - 16 &&
            mud->mouse_y < ui_y + SOCIAL_HEIGHT) {
            text_colour = YELLOW;
        } else {
            text_colour = WHITE;
        }

        surface_draw_string_centre(mud->surface, "Click here to add a name",
                                   ui_x + (SOCIAL_WIDTH / 2),
                                   ui_y + SOCIAL_HEIGHT - 3, 1, text_colour);
    }

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < SOCIAL_WIDTH &&
        mouse_y < 182) {
        panel_handle_mouse(mud->panel_social_list, mouse_x + ui_x,
                           mouse_y + ui_y, mud->last_mouse_button_down,
                           mud->mouse_button_down, mud->mouse_scroll_delta);

        if (mouse_y <= SOCIAL_TAB_HEIGHT && mud->mouse_button_click == 1) {
            if (mouse_x < (SOCIAL_WIDTH / 2) &&
                mud->ui_tab_social_sub_tab == 1) {
                mud->ui_tab_social_sub_tab = 0;
                panel_reset_list_props(mud->panel_social_list,
                                       mud->control_list_social);
            } else if (mouse_x > (SOCIAL_WIDTH / 2) &&
                       mud->ui_tab_social_sub_tab == 0) {
                mud->ui_tab_social_sub_tab = 1;

                panel_reset_list_props(mud->panel_social_list,
                                       mud->control_list_social);
            }
        }

        if (mud->mouse_button_click == 1 && mud->ui_tab_social_sub_tab == 0) {
            int friend_index = panel_get_list_entry_index(
                mud->panel_social_list, mud->control_list_social);

            if (friend_index >= 0 && mouse_x < 176) {
                if (mouse_x > 116) {
                    mudclient_remove_friend(
                        mud, mud->friend_list_hashes[friend_index]);
                } else if (mud->friend_list_online[friend_index] != 0) {
                    mud->show_dialog_social_input = 2;

                    mud->private_message_target =
                        mud->friend_list_hashes[friend_index];

                    memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
                    memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
                }
            }
        }

        if (mud->mouse_button_click == 1 && mud->ui_tab_social_sub_tab == 1) {
            int ignore_index = panel_get_list_entry_index(
                mud->panel_social_list, mud->control_list_social);

            if (ignore_index >= 0 && mouse_x < 176 && mouse_x > 116) {
                mudclient_remove_ignore(mud, mud->ignore_list[ignore_index]);
            }
        }

        if (mouse_y > 166 && mud->mouse_button_click == 1) {
            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH);

            if (mud->ui_tab_social_sub_tab == 0) {
                mud->show_dialog_social_input = 1;
            } else if (mud->ui_tab_social_sub_tab == 1) {
                mud->show_dialog_social_input = 3;
            }
        }

        mud->mouse_button_click = 0;
    }
}

void mudclient_draw_social_input(mudclient *mud) {
    if (mud->mouse_button_click != 0) {
        mud->mouse_button_click = 0;

        if (mud->show_dialog_social_input == 1 &&
            (mud->mouse_x < 106 || mud->mouse_y < 145 || mud->mouse_x > 406 ||
             mud->mouse_y > 215)) {
            mud->show_dialog_social_input = 0;
            return;
        }

        if (mud->show_dialog_social_input == 2 &&
            (mud->mouse_x < 6 || mud->mouse_y < 145 || mud->mouse_x > 506 ||
             mud->mouse_y > 215)) {
            mud->show_dialog_social_input = 0;
            return;
        }

        if (mud->show_dialog_social_input == 3 &&
            (mud->mouse_x < 106 || mud->mouse_y < 145 || mud->mouse_x > 406 ||
             mud->mouse_y > 215)) {
            mud->show_dialog_social_input = 0;
            return;
        }

        if (mud->mouse_x > 236 && mud->mouse_x < 276 && mud->mouse_y > 193 &&
            mud->mouse_y < 213) {
            mud->show_dialog_social_input = 0;
            return;
        }
    }

    int y = 145;

    if (mud->show_dialog_social_input == 1) {
        surface_draw_box(mud->surface, 106, y, 300, 70, BLACK);
        surface_draw_box_edge(mud->surface, 106, y, 300, 70, WHITE);
        y += 20;

        surface_draw_string_centre(mud->surface,
                                   "Enter name to add to friends list", 256, y,
                                   4, WHITE);

        y += 20;

        char formatted_current[INPUT_TEXT_LENGTH + 2] = {0};
        sprintf(formatted_current, "%s*", mud->input_text_current);

        surface_draw_string_centre(mud->surface, formatted_current, 256, y, 4,
                                   WHITE);

        char *username = mud->input_text_final;
        int username_length = strlen(username);

        if (username_length > 0) {
            int64_t encoded_username = encode_username(username);

            if (username_length > 0 &&
                encoded_username != mud->local_player->hash) {
                mudclient_add_friend(mud, username);
            }

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

            mud->show_dialog_social_input = 0;
        }
    } else if (mud->show_dialog_social_input == 2) {
        surface_draw_box(mud->surface, 6, y, 500, 70, BLACK);
        surface_draw_box_edge(mud->surface, 6, y, 500, 70, WHITE);

        y += 20;

        char target_name[USERNAME_LENGTH + 1];
        decode_username(mud->private_message_target, target_name);

        char formatted_message[USERNAME_LENGTH + 26] = {0};
        sprintf(formatted_message, "Enter message to send to %s", target_name);

        surface_draw_string_centre(mud->surface, formatted_message, 256, y, 4,
                                   WHITE);

        y += 20;

        char formatted_current[INPUT_PM_LENGTH + 2] = {0};
        sprintf(formatted_current, "%s*", mud->input_pm_current);

        surface_draw_string_centre(mud->surface, formatted_current, 256, y, 4,
                                   WHITE);

        char *message = mud->input_pm_final;
        int message_length = strlen(message);

        if (message_length > 0) {
            int length = chat_message_encode(message);

            mudclient_send_private_message(mud, mud->private_message_target,
                                           chat_message_encoded, length);

            char *decoded_message =
                chat_message_decode(chat_message_encoded, 0, length);

            /*
            if (mud->options->word_filter) {
                message = word_filter_filter(message);
            }*/

            char formatted_message[USERNAME_LENGTH + strlen(decoded_message) +
                                   17];

            sprintf(formatted_message, "@pri@You tell %s: %s", target_name,
                    decoded_message);

            mudclient_show_server_message(mud, formatted_message);

            memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
            memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
            mud->show_dialog_social_input = 0;
        }
    } else if (mud->show_dialog_social_input == 3) {
        surface_draw_box(mud->surface, 106, y, 300, 70, BLACK);
        surface_draw_box_edge(mud->surface, 106, y, 300, 70, WHITE);

        y += 20;

        surface_draw_string_centre(
            mud->surface, "Enter name to add to ignore list", 256, y, 4, WHITE);

        y += 20;

        char formatted_current[INPUT_TEXT_LENGTH + 2] = {0};
        sprintf(formatted_current, "%s*", mud->input_text_current);

        surface_draw_string_centre(mud->surface, formatted_current, 256, y, 4,
                                   WHITE);

        char *username = mud->input_text_final;
        int username_length = strlen(username);

        if (username_length > 0) {
            int64_t encoded_username = encode_username(username);

            if (username_length > 0 &&
                encoded_username != mud->local_player->hash) {
                mudclient_add_ignore(mud, username);
            }

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

            mud->show_dialog_social_input = 0;
        }
    }

    int text_colour = WHITE;

    if (mud->mouse_x > 236 && mud->mouse_x < 276 && mud->mouse_y > 193 &&
        mud->mouse_y < 213) {
        text_colour = YELLOW;
    }

    surface_draw_string_centre(mud->surface, "Cancel", 256, 208, 1,
                               text_colour);
}
