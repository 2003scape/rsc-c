#include "message-tabs.h"

void mudclient_create_message_tabs_panel(mudclient *mud) {
    mud->panel_message_tabs = malloc(sizeof(Panel));
    panel_new(mud->panel_message_tabs, mud->surface, 10);

    // TODO make the coordinates for these dynamic

    mud->control_text_list_all = panel_add_text_list_input(
        mud->panel_message_tabs, 7, 324, 498, 14, 1, 80, 0, 1);

    mud->control_text_list_chat =
        panel_add_text_list(mud->panel_message_tabs, 5, 269, 502, 56, 1, 20, 1);

    mud->control_text_list_quest =
        panel_add_text_list(mud->panel_message_tabs, 5, 269, 502, 56, 1, 20, 1);

    mud->control_text_list_private =
        panel_add_text_list(mud->panel_message_tabs, 5, 269, 502, 56, 1, 20, 1);

    panel_set_focus(mud->panel_message_tabs, mud->control_text_list_all);
}

void mudclient_draw_chat_message_tabs(mudclient *mud) {
    int x = mud->surface->width / 2 - HBAR_WIDTH / 2;
    int y = mud->surface->height - 16;

    surface_draw_sprite_from3(mud->surface, x, y,
                              mud->sprite_media + HBAR_SPRITE_OFFSET);

    y = mud->surface->height - 6;

    int text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_ALL) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_all % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(mud->surface, "All messages", x + 54, y, 0,
                               text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_CHAT) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_history % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(mud->surface, "Chat history", x + 155, y, 0,
                               text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_QUEST) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_quest % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(mud->surface, "Quest history", x + 255, y, 0,
                               text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_PRIVATE) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_private % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(mud->surface, "Private history", x + 355, y, 0,
                               text_colour);

    surface_draw_string_centre(mud->surface, "Report abuse", x + 457, y, 0,
                               WHITE);
}

void mudclient_draw_chat_message_tabs_panel(mudclient *mud) {
    if (mud->message_tab_selected == MESSAGE_TAB_ALL) {
        int y = mud->surface->height - 30;

        for (int i = 0; i < MESSAGE_HISTORY_LENGTH; i++) {
            if (mud->message_history_timeout[i] <= 0) {
                continue;
            }

            surface_draw_string(mud->surface, mud->message_history[i], 7,
                                y - i * 12, 1, YELLOW);
        }
    }

    panel_hide(mud->panel_message_tabs, mud->control_text_list_chat);
    panel_hide(mud->panel_message_tabs, mud->control_text_list_quest);
    panel_hide(mud->panel_message_tabs, mud->control_text_list_private);

    if (mud->message_tab_selected == MESSAGE_TAB_CHAT) {
        panel_show(mud->panel_message_tabs, mud->control_text_list_chat);
    } else if (mud->message_tab_selected == MESSAGE_TAB_QUEST) {
        panel_show(mud->panel_message_tabs, mud->control_text_list_quest);
    } else if (mud->message_tab_selected == MESSAGE_TAB_PRIVATE) {
        panel_show(mud->panel_message_tabs, mud->control_text_list_private);
    }

    panel_text_list_entry_height_mod = 2;
    panel_draw_panel(mud->panel_message_tabs);
    panel_text_list_entry_height_mod = 0;
}

void mudclient_send_command_string(mudclient *mud, char *command) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_COMMAND);
    packet_stream_put_string(mud->packet_stream, command);
    packet_stream_send_packet(mud->packet_stream);
}

void mudclient_send_chat_message(mudclient *mud, int8_t *encoded,
                                 int encoded_length) {
    packet_stream_new_packet(mud->packet_stream, CLIENT_CHAT);

    packet_stream_put_bytes(mud->packet_stream, (int8_t *)encoded, 0,
                            encoded_length);

    packet_stream_send_packet(mud->packet_stream);
}

void mudclient_handle_message_tabs_input(mudclient *mud) {
    int x = mud->surface->width / 2 - HBAR_WIDTH / 2;
    int mouse_x = mud->mouse_x - x;

    if ((mud->mouse_y > mud->surface->height - 16)) {
        if (mouse_x > 15 && mouse_x < 96 && mud->last_mouse_button_down == 1) {
            mud->message_tab_selected = MESSAGE_TAB_ALL;
        }

        if (mouse_x > 110 && mouse_x < 194 &&
            mud->last_mouse_button_down == 1) {
            mud->message_tab_selected = MESSAGE_TAB_CHAT;

            mud->panel_message_tabs
                ->control_flash_text[mud->control_text_list_chat] = 999999;
        }

        if (mouse_x > 215 && mouse_x < 295 &&
            mud->last_mouse_button_down == 1) {
            mud->message_tab_selected = MESSAGE_TAB_QUEST;

            mud->panel_message_tabs
                ->control_flash_text[mud->control_text_list_quest] = 999999;
        }

        if (mouse_x > 315 && mouse_x < 395 &&
            mud->last_mouse_button_down == 1) {
            mud->message_tab_selected = MESSAGE_TAB_PRIVATE;

            mud->panel_message_tabs
                ->control_flash_text[mud->control_text_list_private] = 999999;
        }

        if (mouse_x > 417 && mouse_x < 497 &&
            mud->last_mouse_button_down == 1) {
            /*mud->show_dialog_report_abuse_step = 1;
            mud->report_abuse_offence = 0;

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);*/
        }

        mud->last_mouse_button_down = 0;
        mud->mouse_button_down = 0;
    }

    panel_handle_mouse(mud->panel_message_tabs, mud->mouse_x, mud->mouse_y,
                       mud->last_mouse_button_down, mud->mouse_button_down,
                       mud->mouse_scroll_delta);

    if (mud->message_tab_selected > 0 && mud->mouse_x >= 494 &&
        mud->mouse_y >= mud->surface->height - 78) {
        mud->last_mouse_button_down = 0;
    }

    if (panel_is_clicked(mud->panel_message_tabs, mud->control_text_list_all)) {
        char *message =
            panel_get_text(mud->panel_message_tabs, mud->control_text_list_all);

        if (strncmp(message, "::", 2) == 0) {
            if (strncasecmp(message + 2, "closecon", 8) == 0) {
                packet_stream_close(mud->packet_stream);
            } else if (strncasecmp(message + 2, "logout", 6) == 0) {
                mudclient_close_connection(mud);
            } else if (strncasecmp(message + 2, "lostcon", 7) == 0) {
                // mudclient_lost_connection(mud);
            } else if (strncasecmp(message + 2, "displayfps", 10) == 0) {
                mud->options->fps_counter = !mud->options->fps_counter;
            } else {
                mudclient_send_command_string(mud, message + 2);
            }
        } else {
            int encoded_length = chat_message_encode(message);

            mudclient_send_chat_message(mud, chat_message_encoded,
                                        encoded_length);

            message =
                chat_message_decode(chat_message_encoded, 0, encoded_length);

            /*if (mud->options.word_filter) {
                message = word_filter_filter(message);
            }*/

            mud->local_player->message_timeout = 150;
            strcpy(mud->local_player->message, message);

            char formatted_message[strlen(message) +
                                   strlen(mud->local_player->name) + 3];

            sprintf(formatted_message, "%s: %s", mud->local_player->name,
                    message);

            mudclient_show_message(mud, formatted_message, MESSAGE_TYPE_CHAT);
        }

        panel_update_text(mud->panel_message_tabs, mud->control_text_list_all,
                          "");
    }

    if (mud->message_tab_selected == MESSAGE_TAB_ALL) {
        for (int i = 0; i < 5; i++) {
            if (mud->message_history_timeout[i] > 0) {
                mud->message_history_timeout[i]--;
            }
        }
    }
}

void mudclient_decrement_message_flash(mudclient *mud) {
    if (mud->message_tab_flash_all > 0) {
        mud->message_tab_flash_all--;
    }

    if (mud->message_tab_flash_history > 0) {
        mud->message_tab_flash_history--;
    }

    if (mud->message_tab_flash_quest > 0) {
        mud->message_tab_flash_quest--;
    }

    if (mud->message_tab_flash_private > 0) {
        mud->message_tab_flash_private--;
    }
}

void mudclient_show_message(mudclient *mud, char *message, int type) {
    int message_length = -1;

    /* handle ignore list */
    if (type == MESSAGE_TYPE_CHAT || type == MESSAGE_TYPE_BOR ||
        type == MESSAGE_TYPE_PRIVATE) {
        for (; strlen(message) > 5 && message[0] == '@' && message[4] == '@';
             message += 5)
            ;

        message_length = strlen(message);
        int colon_index = -1;

        for (int i = 0; i < message_length; i += 1) {
            if (message[i] == ':') {
                colon_index = i;
            }
        }

        if (colon_index != -1) {
            char username[colon_index];
            strncpy(username, message, colon_index);

            int64_t encoded_username = encode_username(username);

            for (int i = 0; i < mud->ignore_list_count; i++) {
                if (mud->ignore_list[i] == encoded_username) {
                    return;
                }
            }
        }
    }

    if (message_length == -1) {
        message_length = strlen(message);
    }

    char coloured_message[message_length + 6];
    memset(coloured_message, '\0', message_length + 6);

    if (type == MESSAGE_TYPE_CHAT) {
        sprintf(coloured_message, "@yel@%s", message);
    } else if (type == MESSAGE_TYPE_GAME || type == MESSAGE_TYPE_BOR) {
        sprintf(coloured_message, "@whi@%s", message);
    } else if (type == MESSAGE_TYPE_PRIVATE) {
        sprintf(coloured_message, "@cya@%s", message);
    } else {
        strcpy(coloured_message, message);
    }

    if (mud->message_tab_selected != MESSAGE_TAB_ALL) {
        if (type == MESSAGE_TYPE_BOR || type == MESSAGE_TYPE_GAME) {
            mud->message_tab_flash_all = MESSAGE_FLASH_TIME;
        }

        if (type == MESSAGE_TYPE_CHAT &&
            mud->message_tab_selected != MESSAGE_TAB_CHAT) {
            mud->message_tab_flash_history = MESSAGE_FLASH_TIME;
        }

        if (type == MESSAGE_TYPE_QUEST &&
            mud->message_tab_selected != MESSAGE_TAB_QUEST) {
            mud->message_tab_flash_quest = MESSAGE_FLASH_TIME;
        }

        if (type == MESSAGE_TYPE_PRIVATE &&
            mud->message_tab_selected != MESSAGE_TAB_PRIVATE) {
            mud->message_tab_flash_private = MESSAGE_FLASH_TIME;
        }

        if (type == MESSAGE_TYPE_GAME &&
            mud->message_tab_selected != MESSAGE_TAB_ALL) {
            mud->message_tab_selected = MESSAGE_TAB_ALL;
        }

        if (type == MESSAGE_TYPE_PRIVATE &&
            mud->message_tab_selected != MESSAGE_TAB_PRIVATE &&
            mud->message_tab_selected != MESSAGE_TAB_ALL) {
            mud->message_tab_selected = MESSAGE_TAB_ALL;
        }
    }

    for (int i = MESSAGE_HISTORY_LENGTH - 1; i > 0; i--) {
        strcpy(mud->message_history[i], mud->message_history[i - 1]);
        mud->message_history_timeout[i] = mud->message_history_timeout[i - 1];
    }

    strcpy(mud->message_history[0], coloured_message);
    mud->message_history_timeout[0] = 300;

    if (type == MESSAGE_TYPE_CHAT) {
        int flash =
            mud->panel_message_tabs
                ->control_flash_text[mud->control_text_list_chat] ==
            mud->panel_message_tabs
                    ->control_list_entry_count[mud->control_text_list_chat] -
                4;

        panel_add_list_entry_wrapped(mud->panel_message_tabs,
                                     mud->control_text_list_chat,
                                     coloured_message, flash);
    } else if (type == MESSAGE_TYPE_QUEST) {
        int flash =
            mud->panel_message_tabs
                ->control_flash_text[mud->control_text_list_quest] ==
            mud->panel_message_tabs
                    ->control_list_entry_count[mud->control_text_list_quest] -
                4;

        panel_add_list_entry_wrapped(mud->panel_message_tabs,
                                     mud->control_text_list_quest,
                                     coloured_message, flash);
    } else if (type == MESSAGE_TYPE_PRIVATE) {
        int flash =
            mud->panel_message_tabs
                ->control_flash_text[mud->control_text_list_private] ==
            mud->panel_message_tabs
                    ->control_list_entry_count[mud->control_text_list_private] -
                4;

        panel_add_list_entry_wrapped(mud->panel_message_tabs,
                                     mud->control_text_list_private,
                                     coloured_message, flash);
    }
}

void mudclient_show_server_message(mudclient *mud, char *message) {
    if (strncasecmp(message, "@bor@", 5) == 0) {
        mudclient_show_message(mud, message, MESSAGE_TYPE_BOR);
    } else if (strncasecmp(message, "@que@", 5) == 0) {
        int formatted_length = strlen(message) + 6;
        char formatted_message[formatted_length];
        sprintf(formatted_message, "@whi@%s", message);
        mudclient_show_message(mud, formatted_message, MESSAGE_TYPE_QUEST);
    } else if (strncasecmp(message, "@pri@", 5) == 0) {
        mudclient_show_message(mud, message, MESSAGE_TYPE_PRIVATE);
    } else {
        mudclient_show_message(mud, message, MESSAGE_TYPE_GAME);
    }
}
