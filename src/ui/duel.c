#include "duel.h"

void mudclient_draw_duel(mudclient *mud) {
    if (mud->mouse_button_click != 0 && mud->mouse_item_count_increment == 0) {
        mud->mouse_item_count_increment = 1;
    }

    int dialog_x = 22;
    int dialog_y = 36;

    if (mud->mouse_item_count_increment > 0) {
        int mouse_x = mud->mouse_x - dialog_x;
        int mouse_y = mud->mouse_y - dialog_y;

        if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < 468 && mouse_y < 262) {
            if (mouse_x > 216 && mouse_y > 30 && mouse_x < 462 &&
                mouse_y < 235) {
                int slot = ((mouse_x - 217) / 49) + ((mouse_y - 31) / 34) * 5;

                if (slot >= 0 && slot < mud->inventory_items_count) {
                    int send_update = 0;
                    int item_count_add = 0;

                    int item_id = mud->inventory_item_id[slot];

                    for (int i = 0; i < mud->duel_item_count; i++) {
                        if (mud->duel_items[i] == item_id) {
                            if (game_data_item_stackable[item_id] == 0) {
                                for (int j = 0;
                                     j < mud->mouse_item_count_increment; j++) {
                                    if (mud->duel_items_count[i] <
                                        mud->inventory_item_stack_count[slot]) {
                                        mud->duel_items_count[i]++;
                                    }

                                    send_update = 1;
                                }
                            } else {
                                item_count_add++;
                            }
                        }
                    }

                    if (mudclient_get_inventory_count(mud, item_id) <=
                        item_count_add) {
                        send_update = 1;
                    }

                    if (game_data_item_special[item_id] == 1) {
                        mudclient_show_message(
                            mud, "This object cannot be added to a duel offer",
                            MESSAGE_TYPE_GAME);

                        send_update = 1;
                    }

                    if (!send_update && mud->duel_item_count < 8) {
                        mud->duel_items[mud->duel_item_count] = item_id;
                        mud->duel_items_count[mud->duel_item_count] = 1;
                        mud->duel_item_count++;
                        send_update = 1;
                    }

                    if (send_update) {
                        packet_stream_new_packet(mud->packet_stream,
                                                 CLIENT_DUEL_ITEM_UPDATE);
                        packet_stream_put_byte(mud->packet_stream,
                                               mud->duel_item_count);

                        for (int i = 0; i < mud->duel_item_count; i++) {
                            packet_stream_put_short(mud->packet_stream,
                                                    mud->duel_items[i]);
                            packet_stream_put_int(mud->packet_stream,
                                                  mud->duel_items_count[i]);
                        }

                        packet_stream_send_packet(mud->packet_stream);
                        mud->duel_opponent_accepted = 0;
                        mud->duel_accepted = 0;
                    }
                }
            }

            if (mouse_x > 8 && mouse_y > 30 && mouse_x < 205 && mouse_y < 129) {
                int slot = ((mouse_x - 9) / 49) + ((mouse_y - 31) / 34) * 4;

                if (slot >= 0 && slot < mud->duel_item_count) {
                    int item_id = mud->duel_items[slot];

                    for (int i = 0; i < mud->mouse_item_count_increment; i++) {
                        if (game_data_item_stackable[item_id] == 0 &&
                            mud->duel_items_count[slot] > 1) {
                            mud->duel_items_count[slot]--;
                            continue;
                        }

                        mud->duel_item_count--;
                        mud->mouse_button_down_time = 0;

                        for (int j = slot; j < mud->duel_item_count; j++) {
                            mud->duel_items[j] = mud->duel_items[j + 1];
                            mud->duel_items_count[j] =
                                mud->duel_items_count[j + 1];
                        }

                        break;
                    }

                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_DUEL_ITEM_UPDATE);

                    packet_stream_put_byte(mud->packet_stream,
                                           mud->duel_item_count);

                    for (int i = 0; i < mud->duel_item_count; i++) {
                        packet_stream_put_short(mud->packet_stream,
                                                mud->duel_items[i]);

                        packet_stream_put_int(mud->packet_stream,
                                              mud->duel_items_count[i]);
                    }

                    packet_stream_send_packet(mud->packet_stream);
                    mud->duel_opponent_accepted = 0;
                    mud->duel_accepted = 0;
                }
            }

            int send_update = 0;

            if (mouse_x >= 93 && mouse_y >= 221 && mouse_x <= 104 &&
                mouse_y <= 232) {
                mud->duel_option_retreat = !mud->duel_option_retreat;
                send_update = 1;
            }

            if (mouse_x >= 93 && mouse_y >= 240 && mouse_x <= 104 &&
                mouse_y <= 251) {
                mud->duel_option_magic = !mud->duel_option_magic;
                send_update = 1;
            }

            if (mouse_x >= 191 && mouse_y >= 221 && mouse_x <= 202 &&
                mouse_y <= 232) {
                mud->duel_option_prayer = !mud->duel_option_prayer;
                send_update = 1;
            }

            if (mouse_x >= 191 && mouse_y >= 240 && mouse_x <= 202 &&
                mouse_y <= 251) {
                mud->duel_option_weapons = !mud->duel_option_weapons;
                send_update = 1;
            }

            if (send_update) {
                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_DUEL_OPTIONS);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_retreat ? 1 : 0);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_magic ? 1 : 0);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_prayer ? 1 : 0);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_weapons ? 1 : 0);

                packet_stream_send_packet(mud->packet_stream);

                mud->duel_opponent_accepted = 0;
                mud->duel_accepted = 0;
            }

            if (mouse_x >= 217 && mouse_y >= 238 && mouse_x <= 286 &&
                mouse_y <= 259) {
                mud->duel_accepted = 1;

                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_DUEL_ACCEPT);

                packet_stream_send_packet(mud->packet_stream);
            }

            if (mouse_x >= 394 && mouse_y >= 238 && mouse_x < 463 &&
                mouse_y < 259) {
                mud->show_dialog_duel = 0;

                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_DUEL_DECLINE);

                packet_stream_send_packet(mud->packet_stream);
            }
        } else if (mud->mouse_button_click != 0) {
            mud->show_dialog_duel = 0;

            packet_stream_new_packet(mud->packet_stream, CLIENT_DUEL_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        mud->mouse_button_click = 0;
        mud->mouse_item_count_increment = 0;
    }

    if (!mud->show_dialog_duel) {
        return;
    }

    surface_draw_box(mud->surface, dialog_x, dialog_y, 468, 12,
                     DUEL_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 12, 468, 18,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 30, 8, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 205, dialog_y + 30, 11, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 462, dialog_y + 30, 6, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 99, 197, 24,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 192, 197, 23,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 258, 197, 20,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 216, dialog_y + 235, 246,
                           43, GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 30, 197, 69,
                           GREY_D0, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 123, 197, 69,
                           GREY_D0, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 215, 197, 43,
                           GREY_D0, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 216, dialog_y + 30, 246,
                           205, GREY_D0, 160);

    for (int i = 0; i < 3; i++) {
        surface_draw_line_horizontal(mud->surface, dialog_x + 8,
                                     dialog_y + 30 + i * 34, 197, BLACK);
    }

    for (int i = 0; i < 3; i++) {
        surface_draw_line_horizontal(mud->surface, dialog_x + 8,
                                     dialog_y + 123 + i * 34, 197, BLACK);
    }

    for (int i = 0; i < 7; i++) {
        surface_draw_line_horizontal(mud->surface, dialog_x + 216,
                                     dialog_y + 30 + i * 34, 246, BLACK);
    }

    for (int i = 0; i < 6; i++) {
        if (i < 5) {
            surface_draw_line_vertical(mud->surface, dialog_x + 8 + i * 49,
                                       dialog_y + 30, 69, BLACK);

            surface_draw_line_vertical(mud->surface, dialog_x + 8 + i * 49,
                                       dialog_y + 123, 69, BLACK);
        }

        surface_draw_line_vertical(mud->surface, dialog_x + 216 + i * 49,
                                   dialog_y + 30, 205, BLACK);
    }

    surface_draw_line_horizontal(mud->surface, dialog_x + 8, dialog_y + 215,
                                 197, BLACK);

    surface_draw_line_horizontal(mud->surface, dialog_x + 8, dialog_y + 257,
                                 197, BLACK);

    surface_draw_line_vertical(mud->surface, dialog_x + 8, dialog_y + 215, 43,
                               BLACK);

    surface_draw_line_vertical(mud->surface, dialog_x + 204, dialog_y + 215, 43,
                               BLACK);

    char formatted_prepare[USERNAME_LENGTH + 25] = {0};

    sprintf(formatted_prepare, "Preparing to duel with: %s",
            mud->duel_opponent_name);

    surface_draw_string(mud->surface, formatted_prepare, dialog_x + 1,
                        dialog_y + 10, 1, WHITE);

    surface_draw_string(mud->surface, "Your Stake", dialog_x + 9, dialog_y + 27,
                        4, WHITE);

    surface_draw_string(mud->surface, "Opponent's Stake", dialog_x + 9,
                        dialog_y + 120, 4, WHITE);

    surface_draw_string(mud->surface, "Duel Options", dialog_x + 9,
                        dialog_y + 212, 4, WHITE);

    surface_draw_string(mud->surface, "Your Inventory", dialog_x + 216,
                        dialog_y + 27, 4, WHITE);

    surface_draw_string(mud->surface, "No retreating", dialog_x + 8 + 1,
                        dialog_y + 215 + 16, 3, YELLOW);

    surface_draw_string(mud->surface, "No magic", dialog_x + 8 + 1,
                        dialog_y + 215 + 35, 3, YELLOW);

    surface_draw_string(mud->surface, "No prayer", dialog_x + 8 + 102,
                        dialog_y + 215 + 16, 3, YELLOW);

    surface_draw_string(mud->surface, "No weapons", dialog_x + 8 + 102,
                        dialog_y + 215 + 35, 3, YELLOW);

    surface_draw_box_edge(mud->surface, dialog_x + 93, dialog_y + 215 + 6, 11,
                          11, YELLOW);

    if (mud->duel_option_retreat) {
        surface_draw_box(mud->surface, dialog_x + 95, dialog_y + 215 + 8, 7, 7,
                         YELLOW);
    }

    surface_draw_box_edge(mud->surface, dialog_x + 93, dialog_y + 215 + 25, 11,
                          11, YELLOW);

    if (mud->duel_option_magic) {
        surface_draw_box(mud->surface, dialog_x + 95, dialog_y + 215 + 27, 7, 7,
                         YELLOW);
    }

    surface_draw_box_edge(mud->surface, dialog_x + 191, dialog_y + 215 + 6, 11,
                          11, YELLOW);

    if (mud->duel_option_prayer) {
        surface_draw_box(mud->surface, dialog_x + 193, dialog_y + 215 + 8, 7, 7,
                         YELLOW);
    }

    surface_draw_box_edge(mud->surface, dialog_x + 191, dialog_y + 215 + 25, 11,
                          11, YELLOW);

    if (mud->duel_option_weapons) {
        surface_draw_box(mud->surface, dialog_x + 193, dialog_y + 215 + 27, 7,
                         7, YELLOW);
    }

    if (!mud->duel_accepted) {
        surface_draw_sprite_from3(mud->surface, dialog_x + 217, dialog_y + 238,
                                  mud->sprite_media + 25);
    }

    surface_draw_sprite_from3(mud->surface, dialog_x + 394, dialog_y + 238,
                              mud->sprite_media + 26);

    if (mud->duel_opponent_accepted) {
        surface_draw_string_centre(mud->surface, "Other player", dialog_x + 341,
                                   dialog_y + 246, 1, WHITE);

        surface_draw_string_centre(mud->surface, "has accepted", dialog_x + 341,
                                   dialog_y + 256, 1, WHITE);
    }

    if (mud->duel_accepted) {
        surface_draw_string_centre(mud->surface, "Waiting for",
                                   dialog_x + 217 + 35, dialog_y + 246, 1,
                                   WHITE);

        surface_draw_string_centre(mud->surface, "other player",
                                   dialog_x + 217 + 35, dialog_y + 256, 1,
                                   WHITE);
    }

    for (int i = 0; i < mud->inventory_items_count; i++) {
        int x = 217 + dialog_x + (i % 5) * 49;
        int y = 31 + dialog_y + (i / 5) * 34;

        surface_sprite_clipping_from9(
            mud->surface, x, y, 48, 32,
            mud->sprite_item +
                game_data_item_picture[mud->inventory_item_id[i]],
            game_data_item_mask[mud->inventory_item_id[i]], 0, 0, 0);

        if (game_data_item_stackable[mud->inventory_item_id[i]] == 0) {
            char formatted_amount[12] = {0};
            sprintf(formatted_amount, "%d", mud->inventory_item_stack_count[i]);

            surface_draw_string(mud->surface, formatted_amount, x + 1, y + 10,
                                1, YELLOW);
        }
    }

    for (int i = 0; i < mud->duel_item_count; i++) {
        int x = 9 + dialog_x + (i % 4) * 49;
        int y = 31 + dialog_y + (i / 4) * 34;

        int item_id = mud->duel_items[i];

        surface_sprite_clipping_from9(mud->surface, x, y, 48, 32,
                                      mud->sprite_item +
                                          game_data_item_picture[item_id],
                                      game_data_item_mask[item_id], 0, 0, 0);

        if (game_data_item_stackable[item_id] == 0) {
            char formatted_amount[12] = {0};
            sprintf(formatted_amount, "%d", mud->duel_items_count[i]);

            surface_draw_string(mud->surface, formatted_amount, x + 1, y + 10,
                                1, YELLOW);
        }

        if (mud->mouse_x > x && mud->mouse_x < x + 48 && mud->mouse_y > y &&
            mud->mouse_y < y + 32) {
            char *item_name = game_data_item_name[item_id];
            char *description = game_data_item_description[item_id];

            char formatted_item[strlen(item_name) + strlen(description) + 8];
            sprintf(formatted_item, "%s: @whi@%s", item_name, description);

            surface_draw_string(mud->surface, formatted_item, dialog_x + 8,
                                dialog_y + 273, 1, YELLOW);
        }
    }

    for (int i = 0; i < mud->duel_opponent_item_count; i++) {
        int x = 9 + dialog_x + (i % 4) * 49;
        int y = 124 + dialog_y + (i / 4) * 34;

        int item_id = mud->duel_opponent_items[i];

        surface_sprite_clipping_from9(mud->surface, x, y, 48, 32,
                                      mud->sprite_item +
                                          game_data_item_picture[item_id],
                                      game_data_item_mask[item_id], 0, 0, 0);

        if (game_data_item_stackable[item_id] == 0) {
            char formatted_amount[12] = {0};
            sprintf(formatted_amount, "%d", mud->duel_opponent_items_count[i]);

            surface_draw_string(mud->surface, formatted_amount, x + 1, y + 10,
                                1, YELLOW);
        }

        if (mud->mouse_x > x && mud->mouse_x < x + 48 && mud->mouse_y > y &&
            mud->mouse_y < y + 32) {
            char *item_name = game_data_item_name[item_id];
            char *description = game_data_item_description[item_id];

            char formatted_item[strlen(item_name) + strlen(description) + 8];
            sprintf(formatted_item, "%s: @whi@%s", item_name, description);

            surface_draw_string(mud->surface, formatted_item, dialog_x + 8,
                                dialog_y + 273, 1, YELLOW);
        }
    }
}

void mudclient_draw_duel_confirm(mudclient *mud) {
    int dialog_x = 22;
    int dialog_y = 36;

    surface_draw_box(mud->surface, dialog_x, dialog_y, 468, 16,
                     TITLE_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 16, 468, 246,
                           GREY_98, 160);

    char username[USERNAME_LENGTH + 1] = {0};
    decode_username(mud->duel_opponent_confirm_hash, username);

    char formatted_confirm[USERNAME_LENGTH + 37] = {0};

    sprintf(formatted_confirm, "Please confirm your duel with @yel@%s",
            username);

    surface_draw_string_centre(mud->surface, formatted_confirm, dialog_x + 234,
                               dialog_y + 12, 1, WHITE);

    surface_draw_string_centre(mud->surface, "Your stake:", dialog_x + 117,
                               dialog_y + 30, 1, YELLOW);

    for (int i = 0; i < mud->duel_confirm_item_count; i++) {
        int item_id = mud->duel_confirm_items[i];
        char *item_name = game_data_item_name[item_id];
        int item_length = strlen(item_name);

        int line_length = item_length + 15;
        char item_line[line_length];
        memset(item_line, '\0', line_length);

        if (game_data_item_stackable[item_id] == 0) {
            strcat(item_line, " x ");

            format_confirm_amount(mud->trade_confirm_items_count[i],
                                  item_line + item_length + 3);
        }

        strcpy(item_line, item_name);

        surface_draw_string_centre(mud->surface, item_line, dialog_x + 117,
                                   dialog_y + 42 + i * 12, 1, WHITE);
    }

    if (mud->duel_confirm_item_count == 0) {
        surface_draw_string_centre(mud->surface, "Nothing!", dialog_x + 117,
                                   dialog_y + 42, 1, WHITE);
    }

    surface_draw_string_centre(mud->surface,
                               "Your opponent's stake:", dialog_x + 351,
                               dialog_y + 30, 1, YELLOW);

    for (int i = 0; i < mud->duel_opponent_confirm_item_count; i++) {
        int item_id = mud->duel_opponent_confirm_items[i];
        char *item_name = game_data_item_name[item_id];
        int item_length = strlen(item_name);

        int line_length = item_length + 15;
        char item_line[line_length];
        memset(item_line, '\0', line_length);

        if (game_data_item_stackable[item_id] == 0) {
            strcat(item_line, " x ");

            format_confirm_amount(mud->trade_confirm_items_count[i],
                                  item_line + item_length + 3);
        }

        surface_draw_string_centre(mud->surface, item_line, dialog_x + 351,
                                   dialog_y + 42 + i * 12, 1, WHITE);
    }

    if (mud->duel_opponent_confirm_item_count == 0) {
        surface_draw_string_centre(mud->surface, "Nothing!", dialog_x + 351,
                                   dialog_y + 42, 1, WHITE);
    }

    if (mud->duel_option_retreat == 0) {
        surface_draw_string_centre(mud->surface,
                                   "You can retreat from this duel",
                                   dialog_x + 234, dialog_y + 180, 1, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "No retreat is possible!",
                                   dialog_x + 234, dialog_y + 180, 1, RED);
    }

    if (mud->duel_option_magic == 0) {
        surface_draw_string_centre(mud->surface, "Magic may be used",
                                   dialog_x + 234, dialog_y + 192, 1, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Magic cannot be used",
                                   dialog_x + 234, dialog_y + 192, 1, RED);
    }

    if (mud->duel_option_prayer == 0) {
        surface_draw_string_centre(mud->surface, "Prayer may be used",
                                   dialog_x + 234, dialog_y + 204, 1, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Prayer cannot be used",
                                   dialog_x + 234, dialog_y + 204, 1, RED);
    }

    if (mud->duel_option_weapons == 0) {
        surface_draw_string_centre(mud->surface, "Weapons may be used",
                                   dialog_x + 234, dialog_y + 216, 1, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Weapons cannot be used",
                                   dialog_x + 234, dialog_y + 216, 1, RED);
    }

    surface_draw_string_centre(
        mud->surface, "If you are sure click 'Accept' to begin the duel",
        dialog_x + 234, dialog_y + 230, 1, WHITE);

    if (!mud->duel_confirm_accepted) {
        surface_draw_sprite_from3(mud->surface, (dialog_x + 118) - 35,
                                  dialog_y + 238, mud->sprite_media + 25);
        surface_draw_sprite_from3(mud->surface, (dialog_x + 352) - 35,
                                  dialog_y + 238, mud->sprite_media + 26);
    } else {
        surface_draw_string_centre(mud->surface, "Waiting for other player...",
                                   dialog_x + 234, dialog_y + 250, 1, YELLOW);
    }

    if (mud->mouse_button_click == 1) {
        if (mud->mouse_x < dialog_x || mud->mouse_y < dialog_y ||
            mud->mouse_x > dialog_x + 468 || mud->mouse_y > dialog_y + 262) {
            mud->show_dialog_duel_confirm = 0;

            /* this is accurate! */
            packet_stream_new_packet(mud->packet_stream, CLIENT_TRADE_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        if (mud->mouse_x >= (dialog_x + 118) - 35 &&
            mud->mouse_x <= dialog_x + 118 + 70 &&
            mud->mouse_y >= dialog_y + 238 &&
            mud->mouse_y <= dialog_y + 238 + 21) {
            mud->duel_accepted = 1;

            packet_stream_new_packet(mud->packet_stream,
                                     CLIENT_DUEL_CONFIRM_ACCEPT);
            packet_stream_send_packet(mud->packet_stream);
        }

        if (mud->mouse_x >= (dialog_x + 352) - 35 &&
            mud->mouse_x <= dialog_x + 353 + 70 &&
            mud->mouse_y >= dialog_y + 238 &&
            mud->mouse_y <= dialog_y + 238 + 21) {
            mud->show_dialog_duel_confirm = 0;

            packet_stream_new_packet(mud->packet_stream, CLIENT_DUEL_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        mud->mouse_button_click = 0;
    }
}
