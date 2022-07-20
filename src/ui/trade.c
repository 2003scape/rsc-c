#include "trade.h"

void mudclient_draw_trade_items(mudclient *mud, int x, int y, int *trade_items,
                                int *trade_items_count, int trade_item_count) {
    surface_draw_item_grid(mud->surface, x, y, TRADE_OFFER_ROWS,
                           TRADE_OFFER_COLUMNS, trade_items, trade_items_count,
                           trade_item_count, -1, 0);

    for (int i = 0; i < trade_item_count; i++) {
        int slot_x = x + 1 + (i % 4) * ITEM_GRID_SLOT_WIDTH;
        int slot_y = y + 1 + (i / 4) * ITEM_GRID_SLOT_HEIGHT;

        if (mud->mouse_x > slot_x &&
            mud->mouse_x < slot_x + (ITEM_GRID_SLOT_WIDTH - 1) &&
            mud->mouse_y > slot_y &&
            mud->mouse_y < slot_y + (ITEM_GRID_SLOT_HEIGHT - 2)) {
            mud->trade_selected_item = trade_items[i];
        }
    }
}

void mudclient_draw_trade_items_confirm(mudclient *mud, int x, int y,
                                        int *trade_confirm_items,
                                        int *trade_confirm_items_count,
                                        int trade_confirm_item_count) {
    if (trade_confirm_item_count == 0) {
        surface_draw_string_centre(mud->surface, "Nothing!", x, y, 1, WHITE);
        return;
    }

    for (int i = 0; i < trade_confirm_item_count; i++) {
        int item_id = trade_confirm_items[i];
        char *item_name = game_data_item_name[item_id];
        int item_length = strlen(item_name);

        int line_length = item_length + 15;
        char item_line[line_length];
        memset(item_line, '\0', line_length);

        strcpy(item_line, item_name);

        if (game_data_item_stackable[item_id] == 0) {
            strcat(item_line, " x ");

            format_confirm_amount(trade_confirm_items_count[i],
                                  item_line + item_length + 3);
        }

        surface_draw_string_centre(mud->surface, item_line, x, y + i * 12, 1,
                                   WHITE);
    }
}

void mudclient_draw_trade(mudclient *mud) {
    mud->trade_selected_item = -1;

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
                int slot = ((mouse_x - 217) / ITEM_GRID_SLOT_WIDTH) +
                           ((mouse_y - 31) / ITEM_GRID_SLOT_HEIGHT) * 5;

                if (slot >= 0 && slot < mud->inventory_items_count) {
                    int send_update = 0;
                    int item_count_add = 0;

                    int item_id = mud->inventory_item_id[slot];

                    for (int i = 0; i < mud->trade_item_count; i++) {
                        if (mud->trade_items[i] == item_id) {
                            if (game_data_item_stackable[item_id] == 0) {
                                for (int j = 0;
                                     j < mud->mouse_item_count_increment; j++) {
                                    if (mud->trade_items_count[i] <
                                        mud->inventory_item_stack_count[slot]) {
                                        mud->trade_items_count[i]++;
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
                            mud,
                            "This object cannot be traded with other players",
                            MESSAGE_TYPE_GAME);

                        send_update = 1;
                    }

                    if (!send_update &&
                        mud->trade_item_count < TRADE_OFFER_MAX) {
                        mud->trade_items[mud->trade_item_count] = item_id;
                        mud->trade_items_count[mud->trade_item_count] = 1;
                        mud->trade_item_count++;
                        send_update = 1;
                    }

                    if (send_update) {
                        packet_stream_new_packet(mud->packet_stream,
                                                 CLIENT_TRADE_ITEM_UPDATE);

                        packet_stream_put_byte(mud->packet_stream,
                                               mud->trade_item_count);

                        for (int j = 0; j < mud->trade_item_count; j++) {
                            packet_stream_put_short(mud->packet_stream,
                                                    mud->trade_items[j]);

                            packet_stream_put_int(mud->packet_stream,
                                                  mud->trade_items_count[j]);
                        }

                        packet_stream_send_packet(mud->packet_stream);
                        mud->trade_recipient_accepted = 0;
                        mud->trade_accepted = 0;
                    }
                }
            }

            if (mouse_x > 8 && mouse_y > 30 && mouse_x < 205 && mouse_y < 133) {
                int item_index = ((mouse_x - 9) / ITEM_GRID_SLOT_WIDTH) +
                                 ((mouse_y - 31) / ITEM_GRID_SLOT_HEIGHT) * 4;

                if (item_index >= 0 && item_index < mud->trade_item_count) {
                    int item_type = mud->trade_items[item_index];

                    for (int i = 0; i < mud->mouse_item_count_increment; i++) {
                        if (game_data_item_stackable[item_type] == 0 &&
                            mud->trade_items_count[item_index] > 1) {
                            mud->trade_items_count[item_index]--;
                            continue;
                        }

                        mud->trade_item_count--;
                        mud->mouse_button_down_time = 0;

                        for (int j = item_index; j < mud->trade_item_count;
                             j++) {
                            mud->trade_items[j] = mud->trade_items[j + 1];
                            mud->trade_items_count[j] =
                                mud->trade_items_count[j + 1];
                        }

                        break;
                    }

                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_TRADE_ITEM_UPDATE);

                    packet_stream_put_byte(mud->packet_stream,
                                           mud->trade_item_count);

                    for (int i = 0; i < mud->trade_item_count; i++) {
                        packet_stream_put_short(mud->packet_stream,
                                                mud->trade_items[i]);
                        packet_stream_put_int(mud->packet_stream,
                                              mud->trade_items_count[i]);
                    }

                    packet_stream_send_packet(mud->packet_stream);
                    mud->trade_recipient_accepted = 0;
                    mud->trade_accepted = 0;
                }
            }

            if (mouse_x >= 217 && mouse_y >= 238 && mouse_x <= 286 &&
                mouse_y <= 259) {
                mud->trade_accepted = 1;

                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_TRADE_ACCEPT);

                packet_stream_send_packet(mud->packet_stream);
            }

            if (mouse_x >= 394 && mouse_y >= 238 && mouse_x < 463 &&
                mouse_y < 259) {
                mud->show_dialog_trade = 0;

                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_TRADE_DECLINE);

                packet_stream_send_packet(mud->packet_stream);
            }
        } else if (mud->mouse_button_click != 0) {
            mud->show_dialog_trade = 0;

            packet_stream_new_packet(mud->packet_stream, CLIENT_TRADE_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        mud->mouse_button_click = 0;
        mud->mouse_item_count_increment = 0;
    }

    if (!mud->show_dialog_trade) {
        return;
    }

    surface_draw_box(mud->surface, dialog_x, dialog_y, 468, 12,
                     TITLE_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 12, 468, 18,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 30, 8, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 205, dialog_y + 30, 11, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 462, dialog_y + 30, 6, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 133, 197, 22,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 258, 197, 20,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 216, dialog_y + 235, 246,
                           43, GREY_98, 160);

    char formatted_with[USERNAME_LENGTH + 15] = {0};
    sprintf(formatted_with, "Trading with: %s", mud->trade_recipient_name);

    surface_draw_string(mud->surface, formatted_with, dialog_x + 1,
                        dialog_y + 10, 1, WHITE);

    surface_draw_string(mud->surface, "Your Offer", dialog_x + 9, dialog_y + 27,
                        4, WHITE);

    surface_draw_string(mud->surface, "Opponent's Offer", dialog_x + 9,
                        dialog_y + 152, 4, WHITE);

    surface_draw_string(mud->surface, "Your Inventory", dialog_x + 216,
                        dialog_y + 27, 4, WHITE);

    surface_draw_sprite_from3(mud->surface, dialog_x + 394, dialog_y + 238,
                              mud->sprite_media + 26);

    if (mud->trade_recipient_accepted) {
        surface_draw_string_centre(mud->surface, "Other player", dialog_x + 341,
                                   dialog_y + 246, 1, WHITE);

        surface_draw_string_centre(mud->surface, "has accepted", dialog_x + 341,
                                   dialog_y + 256, 1, WHITE);
    }

    if (mud->trade_accepted) {
        surface_draw_string_centre(mud->surface, "Waiting for",
                                   dialog_x + 217 + 35, dialog_y + 246, 1,
                                   WHITE);

        surface_draw_string_centre(mud->surface, "other player",
                                   dialog_x + 217 + 35, dialog_y + 256, 1,
                                   WHITE);
    } else {
        surface_draw_sprite_from3(mud->surface, dialog_x + 217, dialog_y + 238,
                                  mud->sprite_media + 25);
    }

    /* our inventory */
    surface_draw_item_grid(mud->surface, dialog_x + 216, dialog_y + 30, 6, 5,
                           mud->inventory_item_id,
                           mud->inventory_item_stack_count,
                           mud->inventory_items_count, -1, 0);

    /* our offer */
    mudclient_draw_trade_items(mud, dialog_x + 8, dialog_y + 30,
                               mud->trade_items, mud->trade_items_count,
                               mud->trade_item_count);

    /* recipient's offer */
    mudclient_draw_trade_items(
        mud, dialog_x + 8, dialog_y + 155, mud->trade_recipient_items,
        mud->trade_recipient_items_count, mud->trade_recipient_item_count);

    if (mud->trade_selected_item != -1) {
        char *item_name = game_data_item_name[mud->trade_selected_item];

        char *description =
            game_data_item_description[mud->trade_selected_item];

        char formatted_item[strlen(item_name) + strlen(description) + 8];
        sprintf(formatted_item, "%s: @whi@%s", item_name, description);

        surface_draw_string(mud->surface, formatted_item, dialog_x + 8,
                            dialog_y + 273, 1, YELLOW);
    }
}

void mudclient_draw_trade_confirm(mudclient *mud) {
    int dialog_x = 22;
    int dialog_y = 36;

    surface_draw_box(mud->surface, dialog_x, dialog_y, 468, 16,
                     TITLE_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 16, 468, 246,
                           GREY_98, 160);

    char username[USERNAME_LENGTH + 1] = {0};
    decode_username(mud->trade_recipient_confirm_hash, username);

    char formatted_confirm[USERNAME_LENGTH + 37] = {0};

    sprintf(formatted_confirm, "Please confirm your trade with @yel@%s",
            username);

    surface_draw_string_centre(mud->surface, formatted_confirm, dialog_x + 234,
                               dialog_y + 12, 1, WHITE);

    surface_draw_string_centre(mud->surface,
                               "You are about to give:", dialog_x + 117,
                               dialog_y + 30, 1, YELLOW);

    mudclient_draw_trade_items_confirm(
        mud, dialog_x + 117, dialog_y + 42, mud->trade_confirm_items,
        mud->trade_confirm_items_count, mud->trade_confirm_item_count);

    surface_draw_string_centre(mud->surface,
                               "In return you will receive:", dialog_x + 351,
                               dialog_y + 30, 1, YELLOW);

    mudclient_draw_trade_items_confirm(
        mud, dialog_x + 351, dialog_y + 42, mud->trade_confirm_items,
        mud->trade_confirm_items_count, mud->trade_confirm_item_count);

    surface_draw_string_centre(mud->surface,
                               "Are you sure you want to do this?",
                               dialog_x + 234, dialog_y + 200, 4, CYAN);

    surface_draw_string_centre(
        mud->surface,
        "there is NO WAY to reverse a trade if you change your mind.",
        dialog_x + 234, dialog_y + 215, 1, WHITE);

    surface_draw_string_centre(mud->surface,
                               "Remember that not all players are trustworthy",
                               dialog_x + 234, dialog_y + 230, 1, WHITE);

    if (!mud->trade_confirm_accepted) {
        surface_draw_sprite_from3(mud->surface, dialog_x + 118 - 35,
                                  dialog_y + 238, mud->sprite_media + 25);

        surface_draw_sprite_from3(mud->surface, dialog_x + 352 - 35,
                                  dialog_y + 238, mud->sprite_media + 26);
    } else {
        surface_draw_string_centre(mud->surface, "Waiting for other player...",
                                   dialog_x + 234, dialog_y + 250, 1, YELLOW);
    }

    if (mud->mouse_button_click == 1) {
        if (mud->mouse_x < dialog_x || mud->mouse_y < dialog_y ||
            mud->mouse_x > dialog_x + 468 || mud->mouse_y > dialog_y + 262) {
            mud->show_dialog_trade_confirm = 0;
            packet_stream_new_packet(mud->packet_stream, CLIENT_TRADE_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        if (mud->mouse_x >= dialog_x + 118 - 35 &&
            mud->mouse_x <= dialog_x + 118 + 70 &&
            mud->mouse_y >= dialog_y + 238 &&
            mud->mouse_y <= dialog_y + 238 + 21) {
            mud->trade_confirm_accepted = 0;

            packet_stream_new_packet(mud->packet_stream,
                                     CLIENT_TRADE_CONFIRM_ACCEPT);

            packet_stream_send_packet(mud->packet_stream);
        }

        if (mud->mouse_x >= dialog_x + 352 - 35 &&
            mud->mouse_x <= dialog_x + 353 + 70 &&
            mud->mouse_y >= dialog_y + 238 &&
            mud->mouse_y <= dialog_y + 238 + 21) {
            mud->show_dialog_trade_confirm = 0;

            packet_stream_new_packet(mud->packet_stream, CLIENT_TRADE_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        mud->mouse_button_click = 0;
    }
}
