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

                int64_t encoded_username = mud->friend_list[i];
                mud->friend_list[i] = mud->friend_list[i + 1];
                mud->friend_list[i + 1] = encoded_username;

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
        if (mud->friend_list[i] == encoded_username) {
            return;
        }
    }

    if (mud->friend_list_count >= SOCIAL_LIST_MAX) {
        return;
    }

    mud->friend_list[mud->friend_list_count] = encoded_username;
    mud->friend_list_online[mud->friend_list_count] = 0;
    mud->friend_list_count++;
}

void mudclient_remove_friend(mudclient *mud, int64_t encoded_username) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_FRIEND_REMOVE);
    packet_stream_put_long(mud->packet_stream, encoded_username);
    packet_stream_send_packet(mud->packet_stream);

    for (int i = 0; i < mud->friend_list_count; i++) {
        if (mud->friend_list[i] != encoded_username) {
            continue;
        }

        mud->friend_list_count--;

        for (int j = i; j < mud->friend_list_count; j++) {
            mud->friend_list[j] = mud->friend_list[j + 1];
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
    int ui_x = mud->surface->width - SOCIAL_WIDTH - 3;
    int ui_y = 36;

    int height = SOCIAL_HEIGHT;

    int is_touch = mudclient_is_touch(mud);

    if (is_touch) {
        height = 198;
        ui_x = UI_TABS_TOUCH_X - SOCIAL_WIDTH - 1;
        ui_y = (UI_TABS_TOUCH_Y + UI_TABS_TOUCH_HEIGHT) - height - 2;
    }

#if (VERSION_MEDIA >= 59)
    mudclient_draw_ui_tab_label(mud, SOCIAL_TAB, SOCIAL_WIDTH + !is_touch,
                                ui_x - !is_touch, ui_y - 10);
#endif

    mud->ui_tab_min_x = ui_x;
    mud->ui_tab_max_x = mud->surface->width;
    mud->ui_tab_min_y = 0;
    mud->ui_tab_max_y = 240;

    if (is_touch) {
        mud->ui_tab_max_x = ui_x + SOCIAL_WIDTH;
        mud->ui_tab_min_y = ui_y;
        mud->ui_tab_max_y = ui_y + height;
    }

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + SOCIAL_TAB_HEIGHT,
                           SOCIAL_WIDTH, height - SOCIAL_TAB_HEIGHT, GREY_DC,
                           128);

    surface_draw_line_horizontal(mud->surface, ui_x, ui_y + height - 16,
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
            decode_username(mud->friend_list[i], username);

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

            sprintf(formatted_username, "@yel@%s~%04d~@whi@Remove", username,
                    ui_x + 126);

            panel_add_list_entry(mud->panel_social_list,
                                 mud->control_list_social, i,
                                 formatted_username);
        }
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

#ifdef _3DS
    panel_handle_mouse(mud->panel_social_list, mouse_x + ui_x, mouse_y + ui_y,
                       mud->last_mouse_button_down, mud->mouse_button_down,
                       mud->mouse_scroll_delta);
#endif

    panel_draw_panel(mud->panel_social_list);

    if (mud->ui_tab_social_sub_tab == 0) {
        int friend_index = panel_get_list_entry_index(mud->panel_social_list,
                                                      mud->control_list_social);

        char formatted[48] = {0};
        strcpy(formatted, "Click a name to send a message");

        if (friend_index >= 0 && mud->mouse_x < ui_x + 176) {
            char username[USERNAME_LENGTH + 1] = {0};
            decode_username(mud->friend_list[friend_index], username);

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
                                   ui_x + (SOCIAL_WIDTH / 2), ui_y + 35,
                                   FONT_BOLD_12, WHITE);

        int text_colour = BLACK;

        if (mud->mouse_x > ui_x && mud->mouse_x < ui_x + SOCIAL_WIDTH &&
            mud->mouse_y > ui_y + height - 16 && mud->mouse_y < ui_y + height) {
            text_colour = YELLOW;
        } else {
            text_colour = WHITE;
        }

        surface_draw_string_centre(mud->surface, "Click here to add a friend",
                                   ui_x + (SOCIAL_WIDTH / 2), ui_y + height - 3,
                                   FONT_BOLD_12, text_colour);
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
                                   ui_x + (SOCIAL_WIDTH / 2), ui_y + 35,
                                   FONT_BOLD_12, WHITE);

        int text_colour = BLACK;

        if (mud->mouse_x > ui_x && mud->mouse_x < ui_x + SOCIAL_WIDTH &&
            mud->mouse_y > ui_y + height - 16 && mud->mouse_y < ui_y + height) {
            text_colour = YELLOW;
        } else {
            text_colour = WHITE;
        }

        surface_draw_string_centre(mud->surface, "Click here to add a name",
                                   ui_x + (SOCIAL_WIDTH / 2), ui_y + height - 3,
                                   FONT_BOLD_12, text_colour);
    }

    if (!no_menus) {
        return;
    }

#ifndef _3DS
    int is_within_x = mud->options->off_handle_scroll_drag
                          ? 1
                          : mouse_x >= 0 && mouse_x < SOCIAL_WIDTH;

    if (!is_within_x || !(mouse_y >= 0 && mouse_y < height)) {
        return;
    }

    panel_handle_mouse(mud->panel_social_list, mouse_x + ui_x, mouse_y + ui_y,
                       mud->last_mouse_button_down, mud->mouse_button_down,
                       mud->mouse_scroll_delta);
#endif

    if (mouse_y <= SOCIAL_TAB_HEIGHT && mud->mouse_button_click == 1) {
        if (mouse_x < (SOCIAL_WIDTH / 2) && mud->ui_tab_social_sub_tab == 1) {
            mud->ui_tab_social_sub_tab = 0;
            panel_reset_list(mud->panel_social_list, mud->control_list_social);
        } else if (mouse_x > (SOCIAL_WIDTH / 2) &&
                   mud->ui_tab_social_sub_tab == 0) {
            mud->ui_tab_social_sub_tab = 1;

            panel_reset_list(mud->panel_social_list, mud->control_list_social);
        }
    }

    if (mud->mouse_button_click == 1 && mud->ui_tab_social_sub_tab == 0) {
        int friend_index = panel_get_list_entry_index(mud->panel_social_list,
                                                      mud->control_list_social);

        if (friend_index >= 0 && mouse_x < 176) {
            if (mouse_x > 116) {
                mudclient_remove_friend(mud, mud->friend_list[friend_index]);
            } else if (mud->friend_list_online[friend_index] != 0) {
                mud->show_dialog_social_input = SOCIAL_MESSAGE_FRIEND;

                mud->private_message_target = mud->friend_list[friend_index];

                memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
                memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
            }
        }
    }

    if (mud->mouse_button_click == 1 && mud->ui_tab_social_sub_tab == 1) {
        int ignore_index = panel_get_list_entry_index(mud->panel_social_list,
                                                      mud->control_list_social);

        if (ignore_index >= 0 && mouse_x < 176 && mouse_x > 116) {
            mudclient_remove_ignore(mud, mud->ignore_list[ignore_index]);
        }
    }

    if (mouse_y > 166 && mud->mouse_button_click == 1) {
        memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH);
        memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH);

        if (mud->ui_tab_social_sub_tab == 0) {
            mud->show_dialog_social_input = SOCIAL_ADD_FRIEND;
        } else if (mud->ui_tab_social_sub_tab == 1) {
            mud->show_dialog_social_input = SOCIAL_ADD_IGNORE;
        }
    }

    mud->mouse_button_click = 0;
}

void mudclient_draw_social_input(mudclient *mud) {
    int box_width = mud->show_dialog_social_input == SOCIAL_MESSAGE_FRIEND
                        ? SOCIAL_DIALOG_MESSAGE_WIDTH
                        : SOCIAL_DIALOG_ADD_WIDTH;

    int dialog_x = mud->surface->width / 2 - box_width / 2;
    int dialog_y = (mud->surface->height / 2) - (SOCIAL_DIALOG_HEIGHT / 2) + 7;

    int cancel_offset_x = mud->surface->width / 2 - SOCIAL_CANCEL_SIZE / 2;
    int cancel_offset_y = mud->surface->height / 2 + SOCIAL_CANCEL_SIZE / 2;

    char *input_current = mud->show_dialog_social_input == SOCIAL_MESSAGE_FRIEND
                              ? mud->input_pm_current
                              : mud->input_text_current;

    if (mud->mouse_button_click != 0) {
        mud->mouse_button_click = 0;

        if (mud->mouse_x < dialog_x || mud->mouse_y < dialog_y ||
            mud->mouse_x > dialog_x + box_width ||
            mud->mouse_y > dialog_y + SOCIAL_DIALOG_HEIGHT) {
            mud->show_dialog_social_input = 0;
            return;
        }

        if (mud->mouse_x > cancel_offset_x &&
            mud->mouse_x < cancel_offset_x + SOCIAL_CANCEL_SIZE &&
            mud->mouse_y > cancel_offset_y &&
            mud->mouse_y < cancel_offset_y + SOCIAL_CANCEL_SIZE) {
            mud->show_dialog_social_input = 0;
            return;
        }

        if (mudclient_is_touch(mud)) {
            int keyboard_x = mud->surface->width / 2 - box_width / 2;

            int keyboard_y = dialog_y + 20;

            if (mud->mouse_x >= keyboard_x &&
                mud->mouse_x <= keyboard_x + box_width &&
                mud->mouse_y >= keyboard_y && mud->mouse_y <= keyboard_y + 20) {

                mudclient_trigger_keyboard(mud, input_current, 0, keyboard_x,
                                           keyboard_y, box_width - 5, 30,
                                           FONT_BOLD_14, 1);
            }
        }
    }

    surface_draw_box(mud->surface, dialog_x, dialog_y, box_width,
                     SOCIAL_DIALOG_HEIGHT, BLACK);

    surface_draw_border(mud->surface, dialog_x, dialog_y, box_width,
                        SOCIAL_DIALOG_HEIGHT, WHITE);

    dialog_y += 20;

    char formatted_current[INPUT_PM_LENGTH + 2] = {0};
    sprintf(formatted_current, "%s*", input_current);

    switch (mud->show_dialog_social_input) {
    case SOCIAL_ADD_FRIEND: {
        surface_draw_string_centre(
            mud->surface, "Enter name to add to friends list",
            mud->surface->width / 2, dialog_y, FONT_BOLD_14, WHITE);

        dialog_y += 20;

        surface_draw_string_centre(mud->surface, formatted_current,
                                   mud->surface->width / 2, dialog_y,
                                   FONT_BOLD_14, WHITE);

        char *username = mud->input_text_final;
        int username_length = strlen(username);

        if (username_length > 0) {
            int64_t encoded_username = encode_username(username);

            if (username_length > 0 &&
                encoded_username != mud->local_player->encoded_username) {
                mudclient_add_friend(mud, username);
            }

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

            mud->show_dialog_social_input = 0;
        }
        break;
    }
    case SOCIAL_MESSAGE_FRIEND: {
        char target_name[USERNAME_LENGTH + 1];
        decode_username(mud->private_message_target, target_name);

        char formatted_message[USERNAME_LENGTH + 26] = {0};
        sprintf(formatted_message, "Enter message to send to %s", target_name);

        surface_draw_string_centre(mud->surface, formatted_message,
                                   mud->surface->width / 2, dialog_y,
                                   FONT_BOLD_14, WHITE);

        dialog_y += 20;

        surface_draw_string_centre(mud->surface, formatted_current,
                                   mud->surface->width / 2, dialog_y,
                                   FONT_BOLD_14, WHITE);

        char *message = mud->input_pm_final;
        int message_length = strlen(message);

        if (message_length > 0) {
            int length = chat_message_encode(message);

            mudclient_send_private_message(mud, mud->private_message_target,
                                           chat_message_encoded, length);

            char *decoded_message =
                chat_message_decode(chat_message_encoded, 0, length);

            /*if (mud->options->word_filter) {
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
        break;
    }
    case SOCIAL_ADD_IGNORE: {
        surface_draw_string_centre(
            mud->surface, "Enter name to add to ignore list",
            mud->surface->width / 2, dialog_y, FONT_BOLD_14, WHITE);

        dialog_y += 20;

        surface_draw_string_centre(mud->surface, formatted_current,
                                   mud->surface->width / 2, dialog_y,
                                   FONT_BOLD_14, WHITE);

        char *username = mud->input_text_final;
        int username_length = strlen(username);

        if (username_length > 0) {
            int64_t encoded_username = encode_username(username);

            if (username_length > 0 &&
                encoded_username != mud->local_player->encoded_username) {
                mudclient_add_ignore(mud, username);
            }

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

            mud->show_dialog_social_input = 0;
        }
        break;
    }
    }

    int text_colour = WHITE;

    if (mud->mouse_x > cancel_offset_x &&
        mud->mouse_x < cancel_offset_x + SOCIAL_CANCEL_SIZE &&
        mud->mouse_y > cancel_offset_y &&
        mud->mouse_y < cancel_offset_y + SOCIAL_CANCEL_SIZE) {
        text_colour = YELLOW;
    }

    surface_draw_string_centre(mud->surface, "Cancel", mud->surface->width / 2,
                               dialog_y + 23, FONT_BOLD_12, text_colour);
}
