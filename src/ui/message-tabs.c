#include "message-tabs.h"

void mudclient_create_message_tabs_panel(mudclient *mud) {
    mud->panel_message_tabs = malloc(sizeof(Panel));
    panel_new(mud->panel_message_tabs, mud->surface, 10);

    int is_touch = mudclient_is_touch(mud);

    int x = 7 + (is_touch ? 5 : 0);
    int y = is_touch ? 100 : MUD_HEIGHT - 22;

    int width = MUD_WIDTH - 14 - (is_touch ? 14 : 0);
    int height = 14;

    mud->control_text_list_all = panel_add_text_list_input(
        mud->panel_message_tabs, x, y, width, height, FONT_BOLD_12,
        CHAT_MESSAGE_MAX_INPUT_LENGTH, 0, 1);

    int text_list_x = x - 2;
    int text_list_y = y - 55 - (is_touch ? 13 : 0);
    int text_list_width = MUD_WIDTH - 10;
    int text_list_height = 56 + (is_touch ? 14 : 0);

    // TODO make this higher - like 100 - add option to enable
    int max_text_list_entries = 20;

    mud->control_text_list_chat = panel_add_text_list(
        mud->panel_message_tabs, text_list_x, text_list_y, text_list_width,
        text_list_height, FONT_BOLD_12, max_text_list_entries, 1);

    mud->control_text_list_quest = panel_add_text_list(
        mud->panel_message_tabs, text_list_x, text_list_y, text_list_width,
        text_list_height, FONT_BOLD_12, max_text_list_entries, 1);

    mud->control_text_list_private = panel_add_text_list(
        mud->panel_message_tabs, text_list_x, text_list_y, text_list_width,
        text_list_height, FONT_BOLD_12, max_text_list_entries, 1);

    panel_set_focus(mud->panel_message_tabs, mud->control_text_list_all);
}

void mudclient_draw_chat_message_tabs(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH;
    int is_touch = mudclient_is_touch(mud);

    Panel *panel = mud->panel_message_tabs;

    int panel_width = (is_compact ? mud->surface->width : MUD_VANILLA_WIDTH) -
                      14 - (is_touch ? 14 : 0);

    panel->control_width[mud->control_text_list_all] = panel_width;
    panel->control_width[mud->control_text_list_chat] = panel_width + 4;
    panel->control_width[mud->control_text_list_quest] = panel_width + 4;
    panel->control_width[mud->control_text_list_private] = panel_width + 4;

    int x = 0;
    int y = mud->surface->height - 16 + (is_compact ? 1 : 0);

    int button_width = is_compact ? (int)(mud->surface->width * 0.245f) : 100;

    if (is_touch) {
        surface_draw_sprite(mud->surface, x, y + 4, mud->sprite_media + 22);
    } else if (is_compact) {
        int bar_width =
            is_compact ? mud->surface->width + button_width : HBAR_WIDTH;

        surface_draw_sprite_transform_mask(
            mud->surface, x, y, bar_width, 15,
            mud->sprite_media + HBAR_SPRITE_OFFSET, 0, 0, 0, 0);
    } else {
        surface_draw_sprite(mud->surface, x, y,
                            mud->sprite_media + HBAR_SPRITE_OFFSET);
    }

    if (!is_compact && mud->surface->width > HBAR_WIDTH) {
        for (int i = 0; i < mud->surface->width / HBAR_WIDTH; i++) {
            surface_draw_sprite(mud->surface,
                                (x + HBAR_WIDTH) + (HBAR_WIDTH * i), y + 4,
                                mud->sprite_media + 22);
        }

        if (!is_touch) {
            surface_draw_line_horizontal(mud->surface, 503,
                                         mud->surface->height - 14,
                                         mud->surface->width - 503, BLACK);

            surface_draw_line_horizontal(mud->surface, 503,
                                         mud->surface->height - 13,
                                         mud->surface->width - 503, BLACK);
        }
    }

    if (!is_touch && !is_compact && mud->options->wiki_lookup) {
        surface_draw_box(mud->surface, x + 416, y + 3, 84, 9, MESSAGE_TAB_WIKI);
        surface_draw_box(mud->surface, x + 414, y + 7, 88, 5, MESSAGE_TAB_WIKI);
        surface_draw_box(mud->surface, x + 413, y + 8, 90, 3, MESSAGE_TAB_WIKI);
    }

    if (is_touch) {
        x = 9;
        y = 24;

        int button_width =
            is_compact ? (int)(mud->surface->width * 0.245f) : 100;

        for (int i = 0; i < 4 + (!is_compact); i++) {
            int button_x = x + (i * button_width) + (i == 4 ? 1 : 0);
            int button_y = y - 13;

            surface_draw_sprite_scale_mask(
                mud->surface, button_x, button_y, button_width - 10, 19,
                mud->sprite_media + 39,
                i == 4 && !mud->options->wiki_lookup ? 0xff1b00 : 0x00c1ff);

            if (mud->options->wiki_lookup && i == 4) {
                surface_draw_box(mud->surface, button_x + 4, button_y + 4,
                                 button_width - 18, 12, 0x3a779d);
            }
        }
    } else {
        y = mud->surface->height - 6 + (is_compact ? 1 : 0);
    }

    x = (int)(button_width * 0.54f);

    int text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_ALL) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_all % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(mud->surface,
                               is_compact ? "All" : "All messages", x, y,
                               FONT_REGULAR_11, text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_CHAT) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_history % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(
        mud->surface, is_compact ? "Chat" : "Chat history",
        x + button_width + 1, y, FONT_REGULAR_11, text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_QUEST) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_quest % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(
        mud->surface, is_compact ? "Quest" : "Quest history",
        x + (button_width * 2) + 1, y, FONT_REGULAR_11, text_colour);

    text_colour = MESSAGE_TAB_PURPLE;

    if (mud->message_tab_selected == MESSAGE_TAB_PRIVATE) {
        text_colour = MESSAGE_TAB_ORANGE;
    }

    if (mud->message_tab_flash_private % 30 > 15) {
        text_colour = MESSAGE_TAB_RED;
    }

    surface_draw_string_centre(
        mud->surface, is_compact ? "Private" : "Private history",
        x + (button_width * 3) + 1, y, FONT_REGULAR_11, text_colour);

    if (!is_compact && VERSION_MEDIA > 42) {
        surface_draw_string_centre(
            mud->surface,
            mud->options->wiki_lookup ? "Wiki lookup" : "Report abuse",
            x + (button_width * 4) + 3, y, FONT_REGULAR_11,
            mud->selected_wiki ? MESSAGE_TAB_ORANGE : WHITE);
    }
}

void mudclient_draw_chat_message_tabs_panel(mudclient *mud) {
    int is_touch = mudclient_is_touch(mud);

    if (mud->message_tab_selected == MESSAGE_TAB_ALL) {
        int x = 7 + (is_touch ? 5 : 0);
        int y = is_touch ? 91 : mud->surface->height - 30;

        for (int i = 0; i < MESSAGE_HISTORY_LENGTH; i++) {
            if (mud->message_history_timeout[i] <= 0) {
                continue;
            }

            surface_draw_string(mud->surface, mud->message_history[i], x,
                                y - i * 12, FONT_BOLD_12, YELLOW);
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

    if (is_touch) {
        /* default to left */
        int keyboard_button_x = 9;
        int keyboard_button_y = 108;

        if (mud->options->touch_keyboard_right) {
            keyboard_button_x = mud->surface->width - 50;
            keyboard_button_y = mud->surface->height - 265;
        }

        surface_draw_sprite(mud->surface, keyboard_button_x, keyboard_button_y,
                            mud->sprite_media + 40);
    }
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
    int is_touch = mudclient_is_touch(mud);
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH;

    float button_scale =
        is_compact ? mud->surface->width / (float)MUD_MIN_WIDTH : 1;

    int bar_min_y = is_touch ? 0 : mud->surface->height - 16;
    int bar_max_y = is_touch ? 30 : mud->surface->height;
    int bar_max_x = is_touch ? HBAR_WIDTH : mud->surface->width;

    if (mud->last_mouse_button_down == 1 && mud->mouse_x <= bar_max_x &&
        mud->mouse_y > bar_min_y && mud->mouse_y <= bar_max_y) {
        int all_min_x = (is_compact ? 12 * button_scale : 15);
        int all_max_x = (is_compact ? 75 * button_scale : 96);

        int chat_min_x = (is_compact ? 89 * button_scale : 110);
        int chat_max_x = (is_compact ? 151 * button_scale : 194);

        int quest_min_x = (is_compact ? 168 * button_scale : 215);
        int quest_max_x = (is_compact ? 230 * button_scale : 295);

        int private_min_x = (is_compact ? 246 * button_scale : 318);
        int private_max_x = (is_compact ? 308 * button_scale : 395);

        if (mud->mouse_x > all_min_x && mud->mouse_x < all_max_x) {
            mud->message_tab_selected = MESSAGE_TAB_ALL;
        } else if (mud->mouse_x > chat_min_x && mud->mouse_x < chat_max_x) {
            mud->message_tab_selected = MESSAGE_TAB_CHAT;

            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_chat] = 999999;
        } else if (mud->mouse_x > quest_min_x && mud->mouse_x < quest_max_x) {
            mud->message_tab_selected = MESSAGE_TAB_QUEST;

            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_quest] = 999999;
        } else if (mud->mouse_x > private_min_x &&
                   mud->mouse_x < private_max_x) {
            mud->message_tab_selected = MESSAGE_TAB_PRIVATE;

            mud->panel_message_tabs
                ->control_list_position[mud->control_text_list_private] =
                999999;
        } else if (!is_compact && mud->mouse_x > 417 && mud->mouse_x < 497) {
            if (mud->options->wiki_lookup) {
                mud->selected_wiki = !mud->selected_wiki;
            }

            /*mud->show_dialog_report_abuse_step = 1;
            mud->report_abuse_offence = 0;

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);*/
        }

        mud->last_mouse_button_down = 0;
        mud->mouse_button_down = 0;
    }

    if (mudclient_is_touch(mud)) {
        /* default to left */
        int keyboard_button_x = 9;
        int keyboard_button_y = 108;

        if (mud->options->touch_keyboard_right) {
            keyboard_button_x = mud->surface->width - 50;
            keyboard_button_y = 100;
        }

        panel_handle_mouse(mud->panel_message_tabs, mudclient_finger_1_x,
                           mudclient_finger_1_y, mud->last_mouse_button_down,
                           mudclient_finger_1_down, mud->mouse_scroll_delta);

        char *chat_input =
            mud->panel_message_tabs->control_text[mud->control_text_list_all];

        int chat_input_x = 11;
        int chat_input_width = mud->surface->width - chat_input_x;

        if (chat_input_width > HBAR_WIDTH) {
            chat_input_width = HBAR_WIDTH;
        }

        int chat_input_trigger_width =
            surface_text_width(chat_input, FONT_BOLD_12);

        if (chat_input_trigger_width < 32) {
            chat_input_trigger_width = 32;
        }

        int chat_input_y = 93;
        int chat_input_height = 15;

        int is_within_chat_input =
            (mud->mouse_x <= chat_input_x + chat_input_trigger_width &&
             mud->mouse_y >= chat_input_y - 8 &&
             mud->mouse_y <= chat_input_y + chat_input_height + 4);

        int is_within_button_input =
            (mud->mouse_x >= keyboard_button_x &&
             mud->mouse_x <=
                 keyboard_button_x +
                     mud->surface->sprite_width[mud->sprite_media + 40] &&
             mud->mouse_y >= keyboard_button_y &&
             mud->mouse_y <=
                 keyboard_button_y +
                     mud->surface->sprite_height[mud->sprite_media + 40]);

        if (!mud->show_right_click_menu && mud->last_mouse_button_down == 1 &&
            (is_within_chat_input || is_within_button_input)) {
            mudclient_trigger_keyboard(mud, chat_input, 0, chat_input_x,
                                       chat_input_y, chat_input_width,
                                       chat_input_height, FONT_BOLD_12, 0);

            mud->last_mouse_button_down = 0;
        }
    } else {
        panel_handle_mouse(mud->panel_message_tabs, mud->mouse_x, mud->mouse_y,
                           mud->last_mouse_button_down, mud->mouse_button_down,
                           mud->mouse_scroll_delta);
    }

    int text_list_width =
        (is_compact ? mud->surface->width : MUD_VANILLA_WIDTH) -
        (is_touch ? 14 : 0);

    int min_scrollbar_y = is_touch ? 0 : mud->surface->height - 78;
    int max_scrollbar_y = is_touch ? 102 : mud->surface->height;

    if (mud->message_tab_selected > 0 && mud->mouse_x >= text_list_width - 18 &&
        mud->mouse_x < text_list_width + 1 && mud->mouse_y >= min_scrollbar_y &&
        mud->mouse_y <= max_scrollbar_y) {
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
                mudclient_lost_connection(mud);
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

    // TODO make an option to disable this
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

void mudclient_show_message(mudclient *mud, char *message, MessageType type) {
    int message_length = -1;

    /* handle ignore list */
    if (type == MESSAGE_TYPE_CHAT || type == MESSAGE_TYPE_BOR ||
        type == MESSAGE_TYPE_PRIVATE) {
        for (; strlen(message) > 5 && message[0] == '@' && message[4] == '@';
             message += 5)
            ;

        message_length = (int)strlen(message);
        int colon_index = -1;

        for (int i = 0; i < message_length; i += 1) {
            if (message[i] == ':') {
                colon_index = i;
            }
        }

        if (colon_index != -1) {
            /* ensure space for null terminator */
            char username[colon_index + 1];
            memcpy(username, message, colon_index);

            int64_t encoded_username = encode_username(username);

            for (int i = 0; i < mud->ignore_list_count; i++) {
                if (mud->ignore_list[i] == encoded_username) {
                    return;
                }
            }
        }
    }

    if (message_length == -1) {
        message_length = (int)strlen(message);
    }

    /* handle wrapping */
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH;
    int max_text_width = (is_compact ? mud->surface->width : MUD_WIDTH) - 15;

    if (is_compact &&
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

    snprintf(mud->message_history[0], sizeof(mud->message_history[0]), "%s",
             coloured_message);
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
        size_t formatted_length = strlen(message) + 6;
        char formatted_message[formatted_length];
        sprintf(formatted_message, "@whi@%s", message);
        mudclient_show_message(mud, formatted_message, MESSAGE_TYPE_QUEST);
    } else if (strncasecmp(message, "@pri@", 5) == 0) {
        mudclient_show_message(mud, message, MESSAGE_TYPE_PRIVATE);
    } else {
        mudclient_show_message(mud, message, MESSAGE_TYPE_GAME);
    }
}
