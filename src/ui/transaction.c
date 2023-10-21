#include "transaction.h"

void mudclient_offer_transaction_item(mudclient *mud, int update_opcode,
                                      int item_id, int item_amount) {
    if (game_data_item_special[item_id] == 1) {
        char *message = update_opcode == CLIENT_TRADE_ITEM_UPDATE
                            ? "This object cannot be traded with other players"
                            : "This object cannot be added to a duel offer";

        mudclient_show_message(mud, message, MESSAGE_TYPE_GAME);
        return;
    }

    int inventory_count = mudclient_get_inventory_count(mud, item_id);
    int send_update = 0;
    int item_count_add = 0;

    for (int i = 0; i < mud->transaction_item_count; i++) {
        if (mud->transaction_items[i] != item_id) {
            continue;
        }

        if (game_data_item_stackable[item_id] == 0) {
            for (int j = 0; j < item_amount; j++) {
                if (mud->transaction_items_count[i] < inventory_count) {
                    mud->transaction_items_count[i]++;
                }

                send_update = 1;
            }
        } else {
            item_count_add++;
        }
    }

    if (inventory_count <= item_count_add) {
        send_update = 1;
    }

    int max = update_opcode == CLIENT_TRADE_ITEM_UPDATE ? TRADE_OFFER_MAX
                                                        : DUEL_OFFER_MAX;

    if (!send_update && mud->transaction_item_count < TRADE_OFFER_MAX) {
        if (mud->options->transaction_menus) {
            // TODO broken for stackable?
            int end = mud->transaction_item_count + item_amount;

            if (end > max) {
                end = max;
            }

            if (game_data_item_stackable[item_id] == 1) {
                for (int i = mud->transaction_item_count; i < end; i++) {
                    mud->transaction_items[i] = item_id;
                    mud->transaction_items_count[i] = 1;
                    mud->transaction_item_count++;
                }
            } else {
                mud->transaction_items[mud->transaction_item_count] = item_id;

                mud->transaction_items_count[mud->transaction_item_count] =
                    item_amount;

                mud->transaction_item_count++;
            }
        } else {
            mud->transaction_items[mud->transaction_item_count] = item_id;
            mud->transaction_items_count[mud->transaction_item_count] = 1;
            mud->transaction_item_count++;
        }

        send_update = 1;
    }

    if (!send_update) {
        return;
    }

    packet_stream_new_packet(mud->packet_stream, update_opcode);
    packet_stream_put_byte(mud->packet_stream, mud->transaction_item_count);

    for (int j = 0; j < mud->transaction_item_count; j++) {
        packet_stream_put_short(mud->packet_stream, mud->transaction_items[j]);

        packet_stream_put_int(mud->packet_stream,
                              mud->transaction_items_count[j]);
    }

    packet_stream_send_packet(mud->packet_stream);

    mud->transaction_recipient_accepted = 0;
    mud->transaction_accepted = 0;
}

void mudclient_remove_transaction_item(mudclient *mud, int update_opcode,
                                       int item_id, int item_amount) {
    if (mud->options->transaction_menus) {
        int new_count = mud->transaction_item_count;

        for (int i = 0; i < mud->transaction_item_count; i++) {
            if (mud->transaction_items[i] != item_id) {
                continue;
            }

            if (game_data_item_stackable[item_id] == 0) {
                mud->transaction_items_count[i] -= item_amount;

                if (mud->transaction_items_count[i] <= 0) {
                    mud->transaction_items[i] = -1;
                    mud->transaction_items_count[i] = 0;
                    new_count--;
                }

                break;
            } else {
                for (int j = 0; j < mud->transaction_item_count; j++) {
                    if (item_amount <= 0) {
                        break;
                    }

                    if (mud->transaction_items[j] == item_id) {
                        mud->transaction_items[j] = -1;
                        mud->transaction_items_count[j] = 0;
                        item_amount--;
                        new_count--;
                    }
                }

                break;
            }
        }

        int new_items[new_count];
        int new_items_count[new_count];
        int new_index = 0;

        for (int i = 0; i < mud->transaction_item_count; i++) {
            if (mud->transaction_items[i] == -1) {
                continue;
            }

            new_items[new_index] = mud->transaction_items[i];
            new_items_count[new_index] = mud->transaction_items_count[i];
            new_index++;
        }

        memcpy(mud->transaction_items, new_items, sizeof(int) * new_count);

        memcpy(mud->transaction_items_count, new_items_count,
               sizeof(int) * new_count);

        mud->transaction_item_count = new_count;
    } else {
        int slot = -1;

        for (int i = 0; i < mud->transaction_item_count; i++) {
            if (mud->transaction_items[i] == item_id) {
                slot = i;
                break;
            }
        }

        if (slot < 0) {
            return;
        }

        for (int i = 0; i < item_amount; i++) {
            if (game_data_item_stackable[item_id] == 0 &&
                mud->transaction_items_count[slot] > 1) {
                mud->transaction_items_count[slot]--;
                continue;
            }

            mud->transaction_item_count--;
            mud->mouse_button_down_time = 0;

            for (int j = slot; j < mud->transaction_item_count; j++) {
                mud->transaction_items[j] = mud->transaction_items[j + 1];
                mud->transaction_items_count[j] =
                    mud->transaction_items_count[j + 1];
            }

            break;
        }
    }

    packet_stream_new_packet(mud->packet_stream, update_opcode);
    packet_stream_put_byte(mud->packet_stream, mud->transaction_item_count);

    for (int i = 0; i < mud->transaction_item_count; i++) {
        packet_stream_put_short(mud->packet_stream, mud->transaction_items[i]);

        packet_stream_put_int(mud->packet_stream,
                              mud->transaction_items_count[i]);
    }

    packet_stream_send_packet(mud->packet_stream);

    mud->transaction_recipient_accepted = 0;
    mud->transaction_accepted = 0;
}

void mudclient_draw_transaction_items(mudclient *mud, int x, int y, int rows,
                                      int *transaction_items,
                                      int *transaction_items_count,
                                      int transaction_item_count) {
    surface_draw_item_grid(mud->surface, x, y, rows, TRANSACTION_OFFER_COLUMNS,
                           TRADE_SLOT_WIDTH, TRADE_SLOT_HEIGHT,
                           transaction_items, transaction_items_count,
                           transaction_item_count, -1, 0);

    for (int i = 0; i < transaction_item_count; i++) {
        int slot_x = x + 1 + (i % TRANSACTION_OFFER_COLUMNS) * TRADE_SLOT_WIDTH;

        int slot_y =
            y + 1 + (i / TRANSACTION_OFFER_COLUMNS) * TRADE_SLOT_HEIGHT;

        if (mud->mouse_x > slot_x &&
            mud->mouse_x < slot_x + (TRADE_SLOT_WIDTH - 1) &&
            mud->mouse_y > slot_y &&
            mud->mouse_y < slot_y + (TRADE_SLOT_HEIGHT - 2)) {
            mud->transaction_selected_item = transaction_items[i];
        }
    }
}

void mudclient_draw_transaction(mudclient *mud, int dialog_x, int dialog_y,
                                int is_trade) {
    int update_opcode =
        is_trade ? CLIENT_TRADE_ITEM_UPDATE : CLIENT_DUEL_ITEM_UPDATE;

    int decline_opcode = is_trade ? CLIENT_TRADE_DECLINE : CLIENT_DUEL_DECLINE;

    if (mud->show_dialog_offer_x && mud->input_digits_final > 0) {
        if (mud->input_digits_final > mud->offer_max) {
            mudclient_show_message(mud, "You don't have that many!",
                                   MESSAGE_TYPE_GAME);
        } else {
            if (mud->transaction_offer_type == TRANSACTION_OFFER_OFFER) {
                mudclient_offer_transaction_item(
                    mud, update_opcode, mud->offer_id, mud->input_digits_final);
            } else {
                mudclient_remove_transaction_item(
                    mud, update_opcode, mud->offer_id, mud->input_digits_final);
            }

            mud->transaction_last_offer = mud->input_digits_final;
        }

        mud->input_digits_final = 0;
        mud->show_dialog_offer_x = 0;
    }

    mud->transaction_selected_item = -1;

    if (!mud->options->transaction_menus && mud->mouse_button_click != 0 &&
        mud->mouse_item_count_increment == 0) {
        mud->mouse_item_count_increment = 1;
    }

    int mouse_x = mud->mouse_x - dialog_x;
    int mouse_y = mud->mouse_y - dialog_y;

    int offer_height = is_trade ? TRADE_OFFER_HEIGHT : DUEL_OFFER_HEIGHT;

    int tabs_width = TRADE_SLOT_WIDTH * TRANSACTION_OFFER_COLUMNS;
    int tabs_height = 24;

    if (MUD_IS_COMPACT && mud->mouse_button_click != 0 &&
        mouse_x >= TRANSACTION_OFFER_X &&
        mouse_x <= TRANSACTION_OFFER_X + tabs_width &&
        mouse_y >= TRANSACTION_INVENTORY_Y &&
        mouse_y <= TRANSACTION_INVENTORY_Y + tabs_height) {
        mud->transaction_tab =
            (mouse_x - TRANSACTION_OFFER_X) / (tabs_width / 2);
    }

    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < TRANSACTION_WIDTH &&
        mouse_y < TRANSACTION_HEIGHT - 16) {
        /* handle inventory */
        if (mouse_x > TRANSACTION_INVENTORY_X &&
            mouse_y > TRANSACTION_INVENTORY_Y &&
            mouse_x <
                TRANSACTION_INVENTORY_X + TRANSACTION_INVENTORY_WIDTH + 1 &&
            mouse_y <
                TRANSACTION_INVENTORY_Y + TRANSACTION_INVENTORY_HEIGHT + 1) {
            int slot =
                ((mouse_x - (TRANSACTION_INVENTORY_X + 1)) / TRADE_SLOT_WIDTH) +
                ((mouse_y - (TRANSACTION_INVENTORY_Y + 1)) /
                 TRADE_SLOT_HEIGHT) *
                    TRANSACTION_INVENTORY_COLUMNS;

            if (slot >= 0 && slot < mud->inventory_items_count) {
                int item_id = mud->inventory_item_id[slot];

                if (mud->options->transaction_menus &&
                    !mud->show_right_click_menu) {
                    char *item_name = game_data_item_name[item_id];

                    char formatted_item_name[strlen(item_name) + 6];
                    sprintf(formatted_item_name, "@lre@%s", item_name);

                    int item_amount =
                        mudclient_get_inventory_count(mud, item_id);

                    mudclient_add_offer_menus(mud, "Offer",
                                              MENU_TRANSACTION_OFFER, item_id,
                                              item_amount, formatted_item_name,
                                              mud->transaction_last_offer);
                } else if (mud->mouse_item_count_increment > 0) {
                    mudclient_offer_transaction_item(
                        mud, item_id, update_opcode,
                        mud->mouse_item_count_increment);

                    mud->mouse_button_click = 0;
                }
            }
        }

        /* handle offer */
        if (mouse_x > TRANSACTION_OFFER_X && mouse_y > TRANSACTION_OFFER_Y &&
            mouse_x < TRANSACTION_OFFER_X + TRANSACTION_OFFER_WIDTH + 1 &&
            mouse_y < TRANSACTION_OFFER_Y + offer_height + 1) {
            int slot =
                ((mouse_x - (TRANSACTION_OFFER_X + 1)) / TRADE_SLOT_WIDTH) +
                ((mouse_y - (TRANSACTION_OFFER_Y + 1)) / TRADE_SLOT_HEIGHT) *
                    TRANSACTION_OFFER_COLUMNS;

            if (slot >= 0 &&
                slot < (mud->transaction_tab == 0
                            ? mud->transaction_item_count
                            : mud->transaction_recipient_item_count)) {
                int *transaction_items = mud->transaction_tab == 0
                                             ? mud->transaction_items
                                             : mud->transaction_recipient_items;

                int *transaction_items_count =
                    mud->transaction_tab == 0
                        ? mud->transaction_items_count
                        : mud->transaction_recipient_items_count;

                int item_id = transaction_items[slot];

                if (mud->options->transaction_menus &&
                    !mud->show_right_click_menu) {
                    char *item_name = game_data_item_name[item_id];

                    char formatted_item_name[strlen(item_name) + 6];
                    sprintf(formatted_item_name, "@lre@%s", item_name);

                    int item_amount = 0;

                    for (int i = 0; i < mud->transaction_item_count; i++) {
                        if (mud->transaction_items[i] == item_id) {
                            if (game_data_item_stackable[item_id] == 0) {
                                item_amount = transaction_items_count[i];
                                break;
                            }

                            item_amount++;
                        }
                    }

                    if (mud->transaction_tab == 0) {
                        mudclient_add_offer_menus(
                            mud, "Remove", MENU_TRANSACTION_REMOVE, item_id,
                            item_amount, formatted_item_name,
                            mud->transaction_last_offer);
                    } else {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Examine");

                        strcpy(mud->menu_item_text2[mud->menu_items_count],
                               formatted_item_name);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_INVENTORY_EXAMINE;

                        mud->menu_index[mud->menu_items_count] = item_id;
                        mud->menu_items_count++;

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Cancel");

                        strcpy(mud->menu_item_text2[mud->menu_items_count], "");
                        mud->menu_type[mud->menu_items_count] = MENU_CANCEL;
                        mud->menu_items_count++;
                    }
                } else if (mud->mouse_item_count_increment > 0) {
                    mudclient_remove_transaction_item(
                        mud, update_opcode, item_id,
                        mud->mouse_item_count_increment);

                    mud->mouse_button_click = 0;
                }
            }
        }

        /* handle accept buttons */
        if (mud->mouse_button_click != 0 && mouse_y >= TRANSACTION_BUTTON_Y &&
            mouse_y <= TRANSACTION_BUTTON_Y + TRANSACTION_BUTTON_HEIGHT) {
            if (mouse_x >= TRANSACTION_ACCEPT_X &&
                mouse_x <= TRANSACTION_ACCEPT_X + TRANSACTION_BUTTON_WIDTH) {
                mud->transaction_accepted = 1;

                packet_stream_new_packet(mud->packet_stream,
                                         is_trade ? CLIENT_TRADE_ACCEPT
                                                  : CLIENT_DUEL_ACCEPT);

                packet_stream_send_packet(mud->packet_stream);
            } else if (mouse_x >= TRANSACTION_DECLINE_X &&
                       mouse_x <
                           TRANSACTION_DECLINE_X + TRANSACTION_BUTTON_WIDTH) {
                mud->show_dialog_trade = 0;
                mud->show_dialog_duel = 0;

                packet_stream_new_packet(mud->packet_stream, decline_opcode);
                packet_stream_send_packet(mud->packet_stream);
            }

            mud->mouse_button_click = 0;
        }
    } else if (mud->mouse_button_click != 0) {
        mud->show_dialog_trade = 0;
        mud->show_dialog_duel = 0;

        packet_stream_new_packet(mud->packet_stream, decline_opcode);
        packet_stream_send_packet(mud->packet_stream);

        mud->mouse_button_click = 0;
    }

    mud->mouse_item_count_increment = 0;

    int show_dialog = is_trade ? mud->show_dialog_trade : mud->show_dialog_duel;

    if (!show_dialog) {
        return;
    }

    int offer_rows = is_trade ? TRADE_OFFER_ROWS : DUEL_OFFER_ROWS;

    int opponent_padding_bottom = is_trade ? 22 : 24;

    int opponent_offer_y =
        TRANSACTION_OFFER_Y + offer_height + (opponent_padding_bottom + 1);

    surface_draw_box(mud->surface, dialog_x, dialog_y, TRANSACTION_WIDTH, 12,
                     is_trade ? TITLE_BAR_COLOUR : DUEL_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 12,
                           TRANSACTION_WIDTH, MUD_IS_COMPACT ? 2 : 18, GREY_98,
                           160);

    int box_width = TRANSACTION_OFFER_COLUMNS * TRADE_SLOT_WIDTH; // 196
    int box_height = TRANSACTION_INVENTORY_HEIGHT + (MUD_IS_COMPACT ? 26 : 44);
    int padding_left = MUD_IS_COMPACT ? 2 : 8;

    surface_draw_box_alpha(mud->surface, dialog_x,
                           dialog_y + TRANSACTION_INVENTORY_Y, padding_left,
                           box_height, GREY_98, 160);

    surface_draw_box_alpha(mud->surface,
                           dialog_x + box_width + padding_left + 1,
                           dialog_y + TRANSACTION_INVENTORY_Y,
                           MUD_IS_COMPACT ? 2 : 11, box_height, GREY_98, 160);

    surface_draw_box_alpha(mud->surface,
                           dialog_x + box_width + padding_left +
                               TRANSACTION_INVENTORY_WIDTH +
                               (MUD_IS_COMPACT ? 4 : 13),
                           dialog_y + TRANSACTION_INVENTORY_Y,
                           MUD_IS_COMPACT ? 2 : 6, box_height, GREY_98, 160);

    if (MUD_IS_COMPACT) {
        int box_y =
            dialog_y + offer_height + TRANSACTION_INVENTORY_Y + tabs_height;

        /* below offers */
        if (is_trade) {
            surface_draw_box_alpha(mud->surface, dialog_x + padding_left, box_y,
                                   box_width + 1, 56, GREY_98, 160);
        } else {
            surface_draw_box_alpha(mud->surface, dialog_x + padding_left, box_y,
                                   box_width + 1, 2, GREY_98, 160);

            surface_draw_box_alpha(mud->surface, dialog_x + padding_left,
                                   box_y + 44, box_width + 1, 39, GREY_98, 160);
        }

        /* under inventory */
        surface_draw_box_alpha(
            mud->surface,
            dialog_x + TRANSACTION_WIDTH - TRANSACTION_INVENTORY_WIDTH - 3,
            dialog_y + TRANSACTION_INVENTORY_Y + TRANSACTION_INVENTORY_HEIGHT +
                1,
            TRANSACTION_INVENTORY_WIDTH + 1, 25, GREY_98, 160);
    } else {
        surface_draw_box_alpha(
            mud->surface, dialog_x + padding_left,
            dialog_y + offer_height + TRANSACTION_OFFER_Y + 1, box_width + 1,
            opponent_padding_bottom, GREY_98, 160);

        surface_draw_box_alpha(mud->surface, dialog_x + padding_left,
                               dialog_y + opponent_offer_y + offer_height,
                               box_width + 1, is_trade ? 21 : 26, GREY_98, 160);

        if (!is_trade) {
            surface_draw_box_alpha(mud->surface, dialog_x + padding_left,
                                   dialog_y + 258, box_width + 1, 20, GREY_98,
                                   160);
        }

        surface_draw_box_alpha(mud->surface, dialog_x + TRANSACTION_INVENTORY_X,
                               dialog_y + 235, 246, 43, GREY_98, 160);
    }

    char formatted_with[USERNAME_LENGTH + 25] = {0};

    sprintf(formatted_with,
            is_trade ? "Trading with: %s" : "Preparing to duel with: %s",
            mud->transaction_recipient_name);

    surface_draw_string(mud->surface, formatted_with, dialog_x + 1,
                        dialog_y + 10, 1, WHITE);

    if (MUD_IS_COMPACT) {
        char *tabs[] = {"Yours", "Theirs"};
        int tabs_x = dialog_x + TRANSACTION_OFFER_X;
        int tabs_y = dialog_y + TRANSACTION_INVENTORY_Y;

        surface_draw_tabs(mud->surface, tabs_x + 1, tabs_y, tabs_width,
                          tabs_height - 1, tabs, 2, mud->transaction_tab);

        surface_draw_line_vertical(mud->surface, tabs_x, tabs_y, tabs_height,
                                   BLACK);

        surface_draw_line_vertical(mud->surface, tabs_x + tabs_width, tabs_y,
                                   tabs_height, BLACK);

        surface_draw_line_horizontal(
            mud->surface, dialog_x + (TRANSACTION_OFFER_X + 1),
            dialog_y + (TRANSACTION_INVENTORY_Y), tabs_width, BLACK);
    } else {
        surface_draw_string(mud->surface,
                            is_trade ? "Your Offer" : "Your Stake",
                            dialog_x + (TRANSACTION_OFFER_X + 1),
                            dialog_y + (TRANSACTION_OFFER_Y - 3), 4, WHITE);

        surface_draw_string(mud->surface,
                            is_trade ? "Opponent's Offer" : "Opponent's Stake",
                            dialog_x + (TRANSACTION_OFFER_X + 1),
                            dialog_y + (opponent_offer_y - 3), 4, WHITE);
    }

    if (!MUD_IS_COMPACT) {
        surface_draw_string(mud->surface, "Your Inventory",
                            dialog_x + TRANSACTION_INVENTORY_X,
                            dialog_y + (TRANSACTION_INVENTORY_Y - 3), 4, WHITE);
    }

    surface_draw_sprite(mud->surface, dialog_x + TRANSACTION_DECLINE_X,
                        dialog_y + TRANSACTION_BUTTON_Y,
                        mud->sprite_media + 26);

    if (mud->transaction_recipient_accepted) {
        int text_x =
            dialog_x + TRANSACTION_ACCEPT_X + (MUD_IS_COMPACT ? 98 : 124);

        surface_draw_string_centre(
            mud->surface, (MUD_IS_COMPACT ? "Player" : "Other player"), text_x,
            dialog_y + TRANSACTION_BUTTON_Y + 8, 1, WHITE);

        surface_draw_string_centre(
            mud->surface, (MUD_IS_COMPACT ? "accepted" : "has accepted"),
            text_x, dialog_y + TRANSACTION_BUTTON_Y + 18, 1, WHITE);
    }

    if (mud->transaction_accepted) {
        surface_draw_string_centre(
            mud->surface, "Waiting for", dialog_x + TRANSACTION_ACCEPT_X + 35,
            dialog_y + TRANSACTION_BUTTON_Y + 8, 1, WHITE);

        surface_draw_string_centre(
            mud->surface, "other player", dialog_x + TRANSACTION_ACCEPT_X + 35,
            dialog_y + TRANSACTION_BUTTON_Y + 18, 1, WHITE);
    } else {
        surface_draw_sprite(mud->surface, dialog_x + TRANSACTION_ACCEPT_X,
                            dialog_y + TRANSACTION_BUTTON_Y,
                            mud->sprite_media + 25);
    }

    /* our inventory */
    surface_draw_item_grid(
        mud->surface, dialog_x + TRANSACTION_INVENTORY_X,
        dialog_y + TRANSACTION_INVENTORY_Y, TRANSACTION_INVENTORY_ROWS,
        TRANSACTION_INVENTORY_COLUMNS, TRADE_SLOT_WIDTH, TRADE_SLOT_HEIGHT,
        mud->inventory_item_id, mud->inventory_item_stack_count,
        mud->inventory_items_count, -1, 0);

    /* our offer */
    mudclient_draw_transaction_items(
        mud, dialog_x + TRANSACTION_OFFER_X, dialog_y + TRANSACTION_OFFER_Y,
        offer_rows,
        mud->transaction_tab == 0 ? mud->transaction_items
                                  : mud->transaction_recipient_items,
        mud->transaction_tab == 0 ? mud->transaction_items_count
                                  : mud->transaction_recipient_items_count,
        mud->transaction_tab == 0 ? mud->transaction_item_count
                                  : mud->transaction_recipient_item_count);

    if (!MUD_IS_COMPACT) {
        /* recipient's offer */
        mudclient_draw_transaction_items(
            mud, dialog_x + TRANSACTION_OFFER_X, dialog_y + opponent_offer_y,
            offer_rows, mud->transaction_recipient_items,
            mud->transaction_recipient_items_count,
            mud->transaction_recipient_item_count);
    }

    /* highlighted item */
    if (mud->transaction_selected_item != -1) {
        char formatted_amount[18] = {0};

        if (mud->options->condense_item_amounts) {
            int is_opponent = mouse_y >= opponent_offer_y;

            int transaction_item_count =
                is_opponent ? mud->transaction_recipient_item_count
                            : mud->transaction_item_count;

            int *transaction_items = is_opponent
                                         ? mud->transaction_recipient_items
                                         : mud->transaction_items;

            int *transaction_items_count =
                is_opponent ? mud->transaction_recipient_items_count
                            : mud->transaction_items_count;

            int item_amount = 0;

            for (int i = 0; i < transaction_item_count; i++) {
                if (transaction_items[i] == mud->transaction_selected_item) {
                    item_amount = transaction_items_count[i];
                    break;
                }
            }

            if (item_amount >= 100000) {
                char formatted_commas[15] = {0};

                mudclient_format_number_commas(mud, item_amount,
                                               formatted_commas);

                sprintf(formatted_amount, " (%s)", formatted_commas);
            }
        }

        if (!MUD_IS_COMPACT) {
            char *item_name =
                game_data_item_name[mud->transaction_selected_item];

            char *description =
                game_data_item_description[mud->transaction_selected_item];

            char formatted_item[strlen(item_name) + strlen(description) + 25];

            sprintf(formatted_item, "%s: @whi@%s%s", item_name, description,
                    formatted_amount);

            surface_draw_string(mud->surface, formatted_item, dialog_x + 8,
                                dialog_y + TRANSACTION_HEIGHT - 5, 1, YELLOW);
        }
    }

    if (mud->show_dialog_offer_x) {
        mudclient_draw_offer_x(mud);
        mudclient_handle_offer_x_input(mud);
    }
}

void mudclient_draw_transaction_items_confirm(
    mudclient *mud, int x, int y, int *transaction_confirm_items,
    int *transaction_confirm_items_count, int transaction_confirm_item_count) {
    if (transaction_confirm_item_count == 0) {
        surface_draw_string_centre(mud->surface, "Nothing!", x, y, 1, WHITE);
        return;
    }

    for (int i = 0; i < transaction_confirm_item_count; i++) {
        int item_id = transaction_confirm_items[i];
        char *item_name = game_data_item_name[item_id];
        int item_length = strlen(item_name);

        int line_length = item_length + 15;
        char item_line[line_length];
        memset(item_line, '\0', line_length);

        strcpy(item_line, item_name);

        if (game_data_item_stackable[item_id] == 0) {
            strcat(item_line, " x ");

            format_confirm_amount(transaction_confirm_items_count[i],
                                  item_line + item_length + 3);
        }

        surface_draw_string_centre(mud->surface, item_line, x, y + i * 12, 1,
                                   WHITE);
    }
}

void mudclient_draw_transaction_confirm(mudclient *mud, int dialog_x,
                                        int dialog_y, int is_trade) {
    surface_draw_box(mud->surface, dialog_x, dialog_y, TRANSACTION_WIDTH, 16,
                     TITLE_BAR_COLOUR);

    surface_draw_box_alpha(
        mud->surface, dialog_x, dialog_y + 16, TRANSACTION_WIDTH,
        TRANSACTION_HEIGHT - (MUD_IS_COMPACT ? 11 : 32), GREY_98, 160);

    char username[USERNAME_LENGTH + 1] = {0};
    decode_username(mud->transaction_recipient_confirm_name, username);

    char formatted_confirm[USERNAME_LENGTH + 37] = {0};

    sprintf(formatted_confirm, "Please confirm your %s with @yel@%s",
            is_trade ? "trade" : "duel", username);

    surface_draw_string_centre(mud->surface, formatted_confirm,
                               dialog_x + (TRANSACTION_WIDTH / 2),
                               dialog_y + 12, 1, WHITE);

    int y = 30;

    if (MUD_IS_COMPACT) {
        char *tabs[3] = {0};

        if (is_trade) {
            tabs[0] = "Your Offer";
            tabs[1] = "Their Offer";
        } else {
            tabs[0] = "Your Stake";
            tabs[1] = "Their Stake";
            tabs[2] = "Rules";
        }

        surface_draw_tabs(mud->surface, dialog_x, dialog_y + 16,
                          TRANSACTION_WIDTH, 22, tabs, is_trade ? 2 : 3,
                          mud->transaction_tab);

        y += 22;
    } else {
        surface_draw_string_centre(
            mud->surface,
            is_trade ? "You are about to give:" : "Your stake:", dialog_x + 117,
            dialog_y + y, 1, YELLOW);

        surface_draw_string_centre(mud->surface,
                                   is_trade ? "In return you will receive:"
                                            : "Your opponent's stake:",
                                   dialog_x + 351, dialog_y + y, 1, YELLOW);
        y += 12;
    }

    if (MUD_IS_COMPACT) {
        if (mud->transaction_tab != 2) {
            int *confirm_items = mud->transaction_tab == 0
                                     ? mud->transaction_confirm_items
                                     : mud->transaction_recipient_confirm_items;

            int *confirm_items_count =
                mud->transaction_tab == 0
                    ? mud->transaction_confirm_items_count
                    : mud->transaction_recipient_confirm_items_count;

            int confirm_item_count =
                mud->transaction_tab == 0
                    ? mud->transaction_confirm_item_count
                    : mud->transaction_recipient_confirm_item_count;

            mudclient_draw_transaction_items_confirm(
                mud, dialog_x + (TRANSACTION_WIDTH / 2), dialog_y + y,
                confirm_items, confirm_items_count, confirm_item_count);
        }
    } else {
        mudclient_draw_transaction_items_confirm(
            mud, dialog_x + 351, dialog_y + y,
            mud->transaction_recipient_confirm_items,
            mud->transaction_recipient_confirm_items_count,
            mud->transaction_recipient_confirm_item_count);

        mudclient_draw_transaction_items_confirm(
            mud, dialog_x + 117, dialog_y + y, mud->transaction_confirm_items,
            mud->transaction_confirm_items_count,
            mud->transaction_confirm_item_count);
    }

    y = 200;

    if (!MUD_IS_COMPACT) {
        if (is_trade) {
            surface_draw_string_centre(
                mud->surface, "Are you sure you want to do this?",
                dialog_x + (TRANSACTION_WIDTH / 2), dialog_y + y, 4, CYAN);

            y += 15;

            surface_draw_string_centre(
                mud->surface,
                "There is NO WAY to reverse a trade if you change your mind.",
                dialog_x + (TRANSACTION_WIDTH / 2), dialog_y + y, 1, WHITE);
        } else {
            y += 15;
        }

        y += 15;

        surface_draw_string_centre(
            mud->surface,
            is_trade ? "Remember that not all players are trustworthy"
                     : "If you are sure click 'Accept' to begin the duel",
            dialog_x + (TRANSACTION_WIDTH / 2), dialog_y + y, 1, WHITE);

        y += 20;
    }

    if (!mud->transaction_confirm_accepted) {
        int offset_y = MUD_IS_COMPACT ? 8 : 0;

        surface_draw_sprite(mud->surface, dialog_x + (MUD_IS_COMPACT ? 4 : 83),
                            dialog_y + TRANSACTION_BUTTON_Y + offset_y,
                            mud->sprite_media + 25);

        surface_draw_sprite(
            mud->surface,
            dialog_x + (MUD_IS_COMPACT ? TRANSACTION_WIDTH - 73 : 317),
            dialog_y + TRANSACTION_BUTTON_Y + offset_y, mud->sprite_media + 26);
    } else {
        surface_draw_string_centre(mud->surface, "Waiting for other player...",
                                   dialog_x + (TRANSACTION_WIDTH / 2),
                                   dialog_y + y, 1, YELLOW);
    }

    if (mud->mouse_button_click == 1) {
        int confirm_accept_opcode =
            is_trade ? CLIENT_TRADE_CONFIRM_ACCEPT : CLIENT_DUEL_CONFIRM_ACCEPT;

        int decline_opcode =
            is_trade ? CLIENT_TRADE_DECLINE : CLIENT_DUEL_DECLINE;

        if (MUD_IS_COMPACT) {
            if (mud->mouse_y < dialog_y + 12 || mud->mouse_x < dialog_x ||
                mud->mouse_x > dialog_x + TRANSACTION_WIDTH) {
                mud->show_dialog_trade_confirm = 0;
                mud->show_dialog_duel_confirm = 0;

                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_TRADE_DECLINE);

                packet_stream_send_packet(mud->packet_stream);
            } else if (mud->mouse_y > dialog_y + 12 &&
                       mud->mouse_y < dialog_y + 12 + 22 &&
                       mud->mouse_x < dialog_x + TRANSACTION_WIDTH - 4) {
                mud->transaction_tab = (mud->mouse_x - dialog_x) /
                                       (TRANSACTION_WIDTH / (is_trade ? 2 : 3));
            } else if (mud->mouse_y > dialog_y + TRANSACTION_HEIGHT -
                                          TRANSACTION_BUTTON_HEIGHT &&
                       mud->mouse_y < dialog_y + TRANSACTION_HEIGHT) {
                if (mud->mouse_x > dialog_x + 4 &&
                    mud->mouse_x < dialog_x + TRANSACTION_BUTTON_WIDTH + 4) {
                    mud->transaction_confirm_accepted = 1;

                    packet_stream_new_packet(mud->packet_stream,
                                             confirm_accept_opcode);

                    packet_stream_send_packet(mud->packet_stream);
                } else if (mud->mouse_x > dialog_x + TRANSACTION_WIDTH -
                                              TRANSACTION_BUTTON_WIDTH - 4 &&
                           mud->mouse_x < dialog_x + TRANSACTION_WIDTH - 4) {
                    mud->show_dialog_trade_confirm = 0;
                    mud->show_dialog_duel_confirm = 0;

                    packet_stream_new_packet(mud->packet_stream,
                                             decline_opcode);

                    packet_stream_send_packet(mud->packet_stream);
                }
            }
        } else {
            if (mud->mouse_x < dialog_x || mud->mouse_y < dialog_y ||
                mud->mouse_x > dialog_x + TRANSACTION_WIDTH ||
                mud->mouse_y > dialog_y + 262) {
                mud->show_dialog_trade_confirm = 0;
                mud->show_dialog_duel_confirm = 0;

                /* this is accurate! */
                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_TRADE_DECLINE);

                packet_stream_send_packet(mud->packet_stream);
            } else if (mud->mouse_y >= dialog_y + 238 &&
                       mud->mouse_y <=
                           dialog_y + 238 + TRANSACTION_BUTTON_HEIGHT) {
                if (mud->mouse_x >= dialog_x + 118 - 35 &&
                    mud->mouse_x <= dialog_x + 119 + TRANSACTION_BUTTON_WIDTH) {
                    mud->transaction_confirm_accepted = 1;

                    packet_stream_new_packet(mud->packet_stream,
                                             confirm_accept_opcode);

                    packet_stream_send_packet(mud->packet_stream);
                } else if (mud->mouse_x >= dialog_x + 352 - 35 &&
                           mud->mouse_x <=
                               dialog_x + 354 + TRANSACTION_BUTTON_WIDTH) {
                    mud->show_dialog_trade_confirm = 0;
                    mud->show_dialog_duel_confirm = 0;

                    packet_stream_new_packet(mud->packet_stream,
                                             decline_opcode);

                    packet_stream_send_packet(mud->packet_stream);
                }
            }
        }

        mud->mouse_button_click = 0;
    }
}
