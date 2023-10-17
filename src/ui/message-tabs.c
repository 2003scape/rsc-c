#include "message-tabs.h"

void mudclient_create_message_tabs_panel(mudclient *mud) {
    mud->panel_message_tabs = malloc(sizeof(Panel));
    panel_new(mud->panel_message_tabs, mud->surface, 10);

    // TODO make the coordinates for these dynamic

    mud->control_text_list_all =
        panel_add_text_list_input(mud->panel_message_tabs, 7, MUD_HEIGHT - 22,
                                  MUD_WIDTH - 14, 14, 1, 80, 0, 1);

    int text_list_y = MUD_HEIGHT - 77;
    int text_list_width = MUD_WIDTH - 10;

    mud->control_text_list_chat = panel_add_text_list(
        mud->panel_message_tabs, 5, text_list_y, text_list_width, 56, 1, 20, 1);

    mud->control_text_list_quest = panel_add_text_list(
        mud->panel_message_tabs, 5, text_list_y, text_list_width, 56, 1, 20, 1);

    mud->control_text_list_private = panel_add_text_list(
        mud->panel_message_tabs, 5, text_list_y, text_list_width, 56, 1, 20, 1);

    panel_set_focus(mud->panel_message_tabs, mud->control_text_list_all);
}

void mudclient_draw_chat_message_tabs(mudclient *mud) {
    int x = 0;
    int y = mud->surface->height - 16 + (MUD_IS_COMPACT ? 1 : 0);

    int button_width = MUD_IS_COMPACT ? 78 : 100;
    int button_offset_x = MUD_IS_COMPACT ? 41 : 54;

    int bar_width =
        MUD_IS_COMPACT ? MUD_WIDTH + (MUD_WIDTH / 5) + 13 : HBAR_WIDTH;

    if (MUD_IS_COMPACT) {
        surface_draw_sprite_transform_mask(
            mud->surface, x, y, bar_width, 15,
            mud->sprite_media + HBAR_SPRITE_OFFSET, 0, 0, 0, 0);
    } else {
        surface_draw_sprite(mud->surface, x, y,
                            mud->sprite_media + HBAR_SPRITE_OFFSET);
    }

    if (!MUD_IS_COMPACT && mud->surface->width > HBAR_WIDTH) {
        for (int i = 0; i < mud->surface->width / HBAR_WIDTH; i++) {
            surface_draw_sprite(mud->surface,
                                (x + HBAR_WIDTH) + (HBAR_WIDTH * i), y + 4,
                                mud->sprite_media + 22);
        }

        surface_draw_line_horizontal(mud->surface, 503,
                                     mud->surface->height - 14,
                                     mud->surface->width - 503, BLACK);

        surface_draw_line_horizontal(mud->surface, 503,
                                     mud->surface->height - 13,
                                     mud->surface->width - 503, BLACK);
    }

    if (!MUD_IS_COMPACT && mud->options->wiki_lookup) {
        surface_draw_box(mud->surface, x + 416, y + 3, 84, 9, MESSAGE_TAB_WIKI);
        surface_draw_box(mud->surface, x + 414, y + 7, 88, 5, MESSAGE_TAB_WIKI);
        surface_draw_box(mud->surface, x + 413, y + 8, 90, 3, MESSAGE_TAB_WIKI);
    }

    y = mud->surface->height - 6 + (MUD_IS_COMPACT ? 1 : 0);

    int text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_ALL) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_all % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(mud->surface,
                               MUD_IS_COMPACT ? "All" : "All messages",
                               x + button_offset_x, y, 0, text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_CHAT) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_history % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(
        mud->surface, MUD_IS_COMPACT ? "Chat" : "Chat history",
        x + button_offset_x + button_width + 1, y, 0, text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_QUEST) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_quest % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(
        mud->surface, MUD_IS_COMPACT ? "Quest" : "Quest history",
        x + button_offset_x + (button_width * 2) + 1, y, 0, text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_PRIVATE) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_private % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(
        mud->surface, MUD_IS_COMPACT ? "Private" : "Private history",
        x + button_offset_x + (button_width * 3) + 1, y, 0, text_colour);

    if (!MUD_IS_COMPACT) {
        surface_draw_string_centre(
            mud->surface,
            mud->options->wiki_lookup ? "Wiki lookup" : "Report abuse",
            x + button_offset_x + (button_width * 4) + 3, y, 0, WHITE);
    }
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
    int mouse_x = mud->mouse_x;

    if (mud->last_mouse_button_down == 1 &&
        mud->mouse_y > mud->surface->height - 16) {
        if (MUD_IS_COMPACT ? (mouse_x > 20 && mouse_x < 86)
                           : (mouse_x > 15 && mouse_x < 96)) {
            mud->message_tab_selected = MESSAGE_TAB_ALL;
        } else if (MUD_IS_COMPACT ? (mouse_x > 96 && mouse_x < 163)
                                  : (mouse_x > 110 && mouse_x < 194)) {
            mud->message_tab_selected = MESSAGE_TAB_CHAT;

            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_chat] = 999999;
        } else if (MUD_IS_COMPACT ? (mouse_x > 174 && mouse_x < 240)
                                  : (mouse_x > 215 && mouse_x < 295)) {
            mud->message_tab_selected = MESSAGE_TAB_QUEST;

            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_quest] = 999999;
        } else if (MUD_IS_COMPACT ? (mouse_x > 253 && mouse_x < 318)
                                  : (mouse_x > 315 && mouse_x < 395)) {
            mud->message_tab_selected = MESSAGE_TAB_PRIVATE;

            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_private] =
                999999;
        } else if (!MUD_IS_COMPACT && mouse_x > 417 && mouse_x < 497 &&
                   mud->last_mouse_button_down == 1) {
            if (mud->options->wiki_lookup) {
                mud->selected_wiki = 1;
            }

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

    if (mud->message_tab_selected > 0 && mud->mouse_x >= (MUD_WIDTH - 18) &&
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
                mud->options->display_fps = !mud->options->display_fps;
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

    int max_text_width = MUD_WIDTH - 15;

    if (MUD_IS_COMPACT &&
        surface_text_width(message, FONT_BOLD_12) >= max_text_width) {
        char message1[message_length + 1];
        memset(message1, '\0', message_length + 1);

        int last_space = -1;
        char last_colour[3] = {0};

        for (int i = 0; i < message_length; i++) {
            message1[i] = message[i];

            if (message[i] == ' ') {
                last_space = i;
            }

            if (message[i] == '@' && (message_length - i) >= 4 &&
                message[i + 4] == '@') {
                memcpy(last_colour, message + i + 1, 3);
            }

            if (surface_text_width(message1, FONT_BOLD_12) >= max_text_width) {
                int position = last_space == -1 ? i : last_space;
                message1[position] = '\0';

                int message2_length = message_length - position;
                mudclient_show_message(mud, message1, type);

                if (last_colour[0] != 0) {
                    message2_length += 5;
                }

                char message2[message2_length + 6];
                memset(message2, '\0', message2_length + 6);
                int message2_offset = 0;

                if (message1[0] == '@') {
                    strncpy(message2, message1, 5);
                    message2_offset = 5;
                }

                if (last_colour[0] != 0) {
                    message2[message2_offset] = '@';
                    memcpy(message2 + message2_offset + 1, last_colour, 3);
                    message2[message2_offset + 4] = '@';
                    message2_offset += 5;
                }

                strncpy(message2 + message2_offset, message + position + 1,
                        message2_length);

                mudclient_show_message(mud, message2, type);
                return;
            }
        }

        return;
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
                ->control_list_position[mud->control_text_list_chat] ==
            mud->panel_message_tabs
                    ->control_list_entry_count[mud->control_text_list_chat] -
                4;

        panel_add_list_entry_wrapped(mud->panel_message_tabs,
                                     mud->control_text_list_chat,
                                     coloured_message, flash);
    } else if (type == MESSAGE_TYPE_QUEST) {
        int flash =
            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_quest] ==
            mud->panel_message_tabs
                    ->control_list_entry_count[mud->control_text_list_quest] -
                4;

        panel_add_list_entry_wrapped(mud->panel_message_tabs,
                                     mud->control_text_list_quest,
                                     coloured_message, flash);
    } else if (type == MESSAGE_TYPE_PRIVATE) {
        int flash =
            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_private] ==
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
