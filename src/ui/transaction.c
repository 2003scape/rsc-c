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
                           transaction_items, transaction_items_count,
                           transaction_item_count, -1, 0);

    for (int i = 0; i < transaction_item_count; i++) {
        int slot_x =
            x + 1 + (i % TRANSACTION_OFFER_COLUMNS) * ITEM_GRID_SLOT_WIDTH;
        int slot_y =
            y + 1 + (i / TRANSACTION_OFFER_COLUMNS) * ITEM_GRID_SLOT_HEIGHT;

        if (mud->mouse_x > slot_x &&
            mud->mouse_x < slot_x + (ITEM_GRID_SLOT_WIDTH - 1) &&
            mud->mouse_y > slot_y &&
            mud->mouse_y < slot_y + (ITEM_GRID_SLOT_HEIGHT - 2)) {
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

    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < TRANSACTION_WIDTH &&
        mouse_y < TRANSACTION_HEIGHT - 16) {
        /* handle inventory */
        if (mouse_x > TRANSACTION_INVENTORY_X &&
            mouse_y > TRANSACTION_INVENTORY_Y &&
            mouse_x <
                TRANSACTION_INVENTORY_X + TRANSACTION_INVENTORY_WIDTH + 1 &&
            mouse_y <
                TRANSACTION_INVENTORY_Y + TRANSACTION_INVENTORY_HEIGHT + 1) {
            int slot = ((mouse_x - (TRANSACTION_INVENTORY_X + 1)) /
                        ITEM_GRID_SLOT_WIDTH) +
                       ((mouse_y - (TRANSACTION_INVENTORY_Y + 1)) /
                        ITEM_GRID_SLOT_HEIGHT) *
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
                ((mouse_x - (TRANSACTION_OFFER_X + 1)) / ITEM_GRID_SLOT_WIDTH) +
                ((mouse_y - (TRANSACTION_OFFER_Y + 1)) /
                 ITEM_GRID_SLOT_HEIGHT) *
                    TRANSACTION_OFFER_COLUMNS;

            if (slot >= 0 && slot < mud->transaction_item_count) {
                int item_id = mud->transaction_items[slot];

                if (mud->options->transaction_menus &&
                    !mud->show_right_click_menu) {
                    char *item_name = game_data_item_name[item_id];

                    char formatted_item_name[strlen(item_name) + 6];
                    sprintf(formatted_item_name, "@lre@%s", item_name);

                    int item_amount = 0;

                    for (int i = 0; i < mud->transaction_item_count; i++) {
                        if (mud->transaction_items[i] == item_id) {
                            if (game_data_item_stackable[item_id] == 0) {
                                item_amount = mud->transaction_items_count[i];
                                break;
                            }

                            item_amount++;
                        }
                    }

                    mudclient_add_offer_menus(mud, "Remove",
                                              MENU_TRANSACTION_REMOVE, item_id,
                                              item_amount, formatted_item_name,
                                              mud->transaction_last_offer);
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
    int opponent_offer_y = TRANSACTION_OFFER_Y + offer_height + 23;

    surface_draw_box(mud->surface, dialog_x, dialog_y, TRANSACTION_WIDTH, 12,
                     is_trade ? TITLE_BAR_COLOUR : DUEL_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 12,
                           TRANSACTION_WIDTH, 18, GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 30, 8, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 205, dialog_y + 30, 11, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 462, dialog_y + 30, 6, 248,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 8,
                           dialog_y + (offer_rows * ITEM_GRID_SLOT_HEIGHT) + 31,
                           197, 22, GREY_98, 160);

    if (offer_rows < 3) {
        surface_draw_box_alpha(mud->surface, dialog_x + 8,
                               dialog_y + opponent_offer_y + offer_height, 197,
                               26, GREY_98, 160);
    }

    surface_draw_box_alpha(mud->surface, dialog_x + 8, dialog_y + 258, 197, 20,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + TRANSACTION_INVENTORY_X,
                           dialog_y + 235, 246, 43, GREY_98, 160);

    char formatted_with[USERNAME_LENGTH + 15] = {0};

    sprintf(formatted_with,
            is_trade ? "Trading with: %s" : "Preparing to duel with: %s",
            mud->transaction_recipient_name);

    surface_draw_string(mud->surface, formatted_with, dialog_x + 1,
                        dialog_y + 10, 1, WHITE);

    surface_draw_string(mud->surface, is_trade ? "Your Offer" : "Your Stake",
                        dialog_x + (TRANSACTION_OFFER_X + 1),
                        dialog_y + (TRANSACTION_OFFER_Y - 3), 4, WHITE);

    surface_draw_string(mud->surface,
                        is_trade ? "Opponent's Offer" : "Opponent's Stake",
                        dialog_x + (TRANSACTION_OFFER_X + 1),
                        dialog_y + (opponent_offer_y - 3), 4, WHITE);

    surface_draw_string(mud->surface, "Your Inventory",
                        dialog_x + TRANSACTION_INVENTORY_X,
                        dialog_y + (TRANSACTION_INVENTORY_Y - 3), 4, WHITE);

    surface_draw_sprite_from3(mud->surface, dialog_x + TRANSACTION_DECLINE_X,
                              dialog_y + TRANSACTION_BUTTON_Y,
                              mud->sprite_media + 26);

    if (mud->transaction_recipient_accepted) {
        surface_draw_string_centre(mud->surface, "Other player", dialog_x + 341,
                                   dialog_y + TRANSACTION_BUTTON_Y + 8, 1,
                                   WHITE);

        surface_draw_string_centre(mud->surface, "has accepted", dialog_x + 341,
                                   dialog_y + TRANSACTION_BUTTON_Y + 18, 1,
                                   WHITE);
    }

    if (mud->transaction_accepted) {
        surface_draw_string_centre(
            mud->surface, "Waiting for", dialog_x + TRANSACTION_ACCEPT_X + 35,
            dialog_y + TRANSACTION_BUTTON_Y + 8, 1, WHITE);

        surface_draw_string_centre(
            mud->surface, "other player", dialog_x + TRANSACTION_ACCEPT_X + 35,
            dialog_y + TRANSACTION_BUTTON_Y + 18, 1, WHITE);
    } else {
        surface_draw_sprite_from3(mud->surface, dialog_x + TRANSACTION_ACCEPT_X,
                                  dialog_y + TRANSACTION_BUTTON_Y,
                                  mud->sprite_media + 25);
    }

    /* our inventory */
    surface_draw_item_grid(
        mud->surface, dialog_x + TRANSACTION_INVENTORY_X,
        dialog_y + TRANSACTION_INVENTORY_Y, TRANSACTION_INVENTORY_ROWS,
        TRANSACTION_INVENTORY_COLUMNS, mud->inventory_item_id,
        mud->inventory_item_stack_count, mud->inventory_items_count, -1, 0);

    /* our offer */
    mudclient_draw_transaction_items(
        mud, dialog_x + TRANSACTION_OFFER_X, dialog_y + TRANSACTION_OFFER_Y,
        offer_rows, mud->transaction_items, mud->transaction_items_count,
        mud->transaction_item_count);

    /* recipient's offer */
    mudclient_draw_transaction_items(mud, dialog_x + TRANSACTION_OFFER_X,
                                     dialog_y + opponent_offer_y, offer_rows,
                                     mud->transaction_recipient_items,
                                     mud->transaction_recipient_items_count,
                                     mud->transaction_recipient_item_count);

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

        char *item_name = game_data_item_name[mud->transaction_selected_item];

        char *description =
            game_data_item_description[mud->transaction_selected_item];

        char formatted_item[strlen(item_name) + strlen(description) + 25];

        sprintf(formatted_item, "%s: @whi@%s%s", item_name, description,
                formatted_amount);

        surface_draw_string(mud->surface, formatted_item, dialog_x + 8,
                            dialog_y + TRANSACTION_HEIGHT - 5, 1, YELLOW);
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

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 16,
                           TRANSACTION_WIDTH, 246, GREY_98, 160);

    char username[USERNAME_LENGTH + 1] = {0};
    decode_username(mud->transaction_recipient_confirm_name, username);

    char formatted_confirm[USERNAME_LENGTH + 37] = {0};

    sprintf(formatted_confirm, "Please confirm your %s with @yel@%s",
            is_trade ? "trade" : "duel", username);

    surface_draw_string_centre(mud->surface, formatted_confirm, dialog_x + 234,
                               dialog_y + 12, 1, WHITE);

    surface_draw_string_centre(
        mud->surface,
        is_trade ? "You are about to give:" : "Your stake:", dialog_x + 117,
        dialog_y + 30, 1, YELLOW);

    mudclient_draw_transaction_items_confirm(
        mud, dialog_x + 117, dialog_y + 42, mud->transaction_confirm_items,
        mud->transaction_confirm_items_count,
        mud->transaction_confirm_item_count);

    surface_draw_string_centre(mud->surface,
                               is_trade ? "In return you will receive:"
                                        : "Your opponent's stake:",
                               dialog_x + 351, dialog_y + 30, 1, YELLOW);

    mudclient_draw_transaction_items_confirm(
        mud, dialog_x + 351, dialog_y + 42,
        mud->transaction_recipient_confirm_items,
        mud->transaction_recipient_confirm_items_count,
        mud->transaction_recipient_confirm_item_count);

    if (is_trade) {
        surface_draw_string_centre(mud->surface,
                                   "Are you sure you want to do this?",
                                   dialog_x + 234, dialog_y + 200, 4, CYAN);

        surface_draw_string_centre(
            mud->surface,
            "there is NO WAY to reverse a trade if you change your mind.",
            dialog_x + 234, dialog_y + 215, 1, WHITE);
    }

    surface_draw_string_centre(
        mud->surface,
        is_trade ? "Remember that not all players are trustworthy"
                 : "If you are sure click 'Accept' to begin the duel",
        dialog_x + 234, dialog_y + 230, 1, WHITE);

    if (!mud->transaction_confirm_accepted) {
        surface_draw_sprite_from3(mud->surface, dialog_x + 118 - 35,
                                  dialog_y + TRANSACTION_BUTTON_Y,
                                  mud->sprite_media + 25);

        surface_draw_sprite_from3(mud->surface, dialog_x + 352 - 35,
                                  dialog_y + TRANSACTION_BUTTON_Y,
                                  mud->sprite_media + 26);
    } else {
        surface_draw_string_centre(mud->surface, "Waiting for other player...",
                                   dialog_x + 234, dialog_y + 250, 1, YELLOW);
    }

    if (mud->mouse_button_click == 1) {
        if (mud->mouse_x < dialog_x || mud->mouse_y < dialog_y ||
            mud->mouse_x > dialog_x + TRANSACTION_WIDTH ||
            mud->mouse_y > dialog_y + 262) {
            mud->show_dialog_trade_confirm = 0;
            mud->show_dialog_duel_confirm = 0;

            /* this is accurate! */
            packet_stream_new_packet(mud->packet_stream, CLIENT_TRADE_DECLINE);
            packet_stream_send_packet(mud->packet_stream);
        }

        if (mud->mouse_x >= dialog_x + 118 - 35 &&
            mud->mouse_x <= dialog_x + 118 + 70 &&
            mud->mouse_y >= dialog_y + 238 &&
            mud->mouse_y <= dialog_y + 238 + 21) {
            mud->transaction_confirm_accepted = 0;

            packet_stream_new_packet(mud->packet_stream,
                                     is_trade ? CLIENT_TRADE_CONFIRM_ACCEPT
                                              : CLIENT_DUEL_CONFIRM_ACCEPT);

            packet_stream_send_packet(mud->packet_stream);
        }

        if (mud->mouse_x >= dialog_x + 352 - 35 &&
            mud->mouse_x <= dialog_x + 353 + 70 &&
            mud->mouse_y >= dialog_y + 238 &&
            mud->mouse_y <= dialog_y + 238 + 21) {
            mud->show_dialog_trade_confirm = 0;
            mud->show_dialog_duel_confirm = 0;

            packet_stream_new_packet(mud->packet_stream,
                                     is_trade ? CLIENT_TRADE_DECLINE
                                              : CLIENT_DUEL_DECLINE);

            packet_stream_send_packet(mud->packet_stream);
        }

        mud->mouse_button_click = 0;
    }
}
