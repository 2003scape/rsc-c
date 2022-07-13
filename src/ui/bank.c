#include "bank.h"

void mudclient_bank_transaction(mudclient *mud, int item_id, int amount,
                                int opcode) {
    packet_stream_new_packet(mud->packet_stream, opcode);
    packet_stream_put_short(mud->packet_stream, item_id);
    packet_stream_put_short(mud->packet_stream, amount);

#ifndef REVISION_177
    packet_stream_put_int(mud->packet_stream, opcode == CLIENT_BANK_WITHDRAW
                                                  ? BANK_MAGIC_WITHDRAW
                                                  : BANK_MAGIC_DEPOSIT);
#endif

    packet_stream_send_packet(mud->packet_stream);
}

void mudclient_draw_bank_amounts(mudclient *mud, int amount, int last_x, int x,
                                 int y) {
    int text_colour = WHITE;
    int offset_x = 0;

    if (mud->mouse_x >= x && mud->mouse_y >= y && mud->mouse_x < x + 30 &&
        mud->mouse_y <= y + 11) {
        text_colour = RED;
    }

    surface_draw_string(mud->surface, "One", x + 2, y + 10, 1, text_colour);

    offset_x += 30;

    if (amount >= 5) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 30 && mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        surface_draw_string(mud->surface, "Five", x + offset_x + 2, y + 10, 1,
                            text_colour);
    }

    offset_x += 30; // 60

    int ui_amount = mud->options->offer_x ? 10 : 25;
    char formatted_ui_amount[12] = {0};

    if (amount >= ui_amount) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 25 && mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        sprintf(formatted_ui_amount, "%d", ui_amount);

        surface_draw_string(mud->surface, formatted_ui_amount, x + offset_x + 2,
                            y + 10, 1, text_colour);
    }

    offset_x += 25; // 85

    int show_last_offer_x = mud->options->offer_x && mud->options->last_offer_x;

    ui_amount = mud->options->offer_x ? 50 : 100;

    if (amount >= ui_amount) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 30 - (show_last_offer_x ? 7 : 0) &&
            mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        sprintf(formatted_ui_amount, "%d", ui_amount);

        surface_draw_string(mud->surface, formatted_ui_amount, x + offset_x + 2,
                            y + 10, 1, text_colour);
    }

    offset_x += 30 - (show_last_offer_x ? 7 : 0); // 115 or 108

    ui_amount = mud->options->offer_x ? 2 : 500;

    if (amount >= ui_amount) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 33 - (show_last_offer_x ? 17 : 0) &&
            mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        char *ui_text = mud->options->offer_x ? "X" : "500";

        surface_draw_string(mud->surface, ui_text, x + offset_x + 2, y + 10, 1,
                            text_colour);
    }

    offset_x += 33 - (show_last_offer_x ? 17 : 0);

    if (show_last_offer_x && last_x != 0) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 30 && mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        char formatted[255] = {0};
        format_amount_suffix(last_x, 0, 1, formatted);

        surface_draw_string(mud->surface, formatted, x + offset_x + 2, y + 10,
                            1, text_colour);
    }

    if (show_last_offer_x) {
        offset_x += 30;
    }

    ui_amount = mud->options->offer_x ? 0 : 2500;

    if (amount >= ui_amount) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 30 && mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        char *ui_text = mud->options->offer_x ? "All" : "2500";

        surface_draw_string(mud->surface, ui_text, x + offset_x + 2, y + 10, 1,
                            text_colour);
    }
}

void mudclient_draw_bank(mudclient *mud) {
    if (mud->input_digits_final > 0) {
        if (mud->bank_offer_type == BANK_OFFER_WITHDRAW) {
            int max_amount = mud->bank_items_count[mud->bank_offer_item];

            if (mud->input_digits_final > max_amount) {
                mudclient_show_message(mud, "You don't have that many!",
                                       MESSAGE_TYPE_GAME);
            } else {
                mudclient_bank_transaction(
                    mud, mud->bank_items[mud->bank_selected_item_slot],
                    mud->input_digits_final, CLIENT_BANK_WITHDRAW);

                mud->bank_last_withdraw_offer = mud->input_digits_final;
            }
        } else if (mud->bank_offer_type == BANK_OFFER_DEPOSIT) {
            int max_amount =
                mudclient_get_inventory_count(mud, mud->bank_offer_item);

            if (mud->input_digits_final > max_amount) {
                mudclient_show_message(mud, "You don't have that many!",
                                       MESSAGE_TYPE_GAME);
            } else {
                mudclient_bank_transaction(mud, mud->bank_offer_item,
                                           mud->input_digits_final,
                                           CLIENT_BANK_DEPOSIT);

                mud->bank_last_deposit_offer = mud->input_digits_final;
            }
        }

        mud->show_dialog_offer_x = 0;
        mud->input_digits_final = 0;
    }

    if (mud->bank_active_page > 0 &&
        mud->bank_item_count <= BANK_ITEMS_PER_PAGE) {
        mud->bank_active_page = 0;
    }

    if (mud->bank_active_page > 1 &&
        mud->bank_item_count <= (BANK_ITEMS_PER_PAGE * 2)) {
        mud->bank_active_page = 1;
    }

    if (mud->bank_active_page > 2 &&
        mud->bank_item_count <= (BANK_ITEMS_PER_PAGE * 3)) {
        mud->bank_active_page = 2;
    }

    if (mud->bank_selected_item_slot >= mud->bank_item_count ||
        mud->bank_selected_item_slot < 0) {
        mud->bank_selected_item_slot = -1;
    }

    if (mud->bank_selected_item_slot != -1 &&
        mud->bank_items[mud->bank_selected_item_slot] !=
            mud->bank_selected_item) {
        mud->bank_selected_item_slot = -1;
        mud->bank_selected_item = -2;
    }

    if (mud->mouse_button_click != 0) {
        mud->mouse_button_click = 0;

        int mouse_x =
            mud->mouse_x - ((mud->surface->width / 2) - (BANK_WIDTH / 2));

        int mouse_y = mud->mouse_y -
                      (((mud->surface->height - 6) / 2) - (BANK_HEIGHT / 2));

        if (mud->show_dialog_offer_x) {
            mouse_x = 0;
            mouse_y = 13;
        }

        if (mouse_x >= 0 && mouse_y >= 12 && mouse_x < BANK_WIDTH &&
            mouse_y < 280) {
            int slot_index = mud->bank_active_page * BANK_ITEMS_PER_PAGE;

            for (int row = 0; row < 6; row++) {
                for (int col = 0; col < 8; col++) {
                    int slot_x = 7 + col * BANK_SLOT_WIDTH;
                    int slot_y = 28 + row * BANK_SLOT_HEIGHT;

                    if (mouse_x > slot_x &&
                        mouse_x < slot_x + BANK_SLOT_WIDTH &&
                        mouse_y > slot_y &&
                        mouse_y < slot_y + BANK_SLOT_HEIGHT &&
                        slot_index < mud->bank_item_count &&
                        mud->bank_items[slot_index] != -1) {
                        mud->bank_selected_item = mud->bank_items[slot_index];
                        mud->bank_selected_item_slot = slot_index;
                    }

                    slot_index++;
                }
            }

            mouse_x = (mud->surface->width / 2) - (BANK_WIDTH / 2);
            mouse_y = ((mud->surface->height - 6) / 2) - (BANK_HEIGHT / 2);

            int item_id = 0;

            if (mud->bank_selected_item_slot < 0) {
                item_id = -1;
            } else {
                item_id = mud->bank_items[mud->bank_selected_item_slot];
            }

            if (item_id != -1) {
                int bank_count =
                    mud->bank_items_count[mud->bank_selected_item_slot];

                if (game_data_item_stackable[item_id] == 1 && bank_count > 1) {
                    bank_count = 1;
                }

                if (bank_count >= 1 && mud->mouse_x >= mouse_x + 220 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 250 &&
                    mud->mouse_y <= mouse_y + 249) {
                    mudclient_bank_transaction(mud, item_id, 1,
                                               CLIENT_BANK_WITHDRAW);
                }

                if (bank_count >= 5 && mud->mouse_x >= mouse_x + 250 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 280 &&
                    mud->mouse_y <= mouse_y + 249) {
                    mudclient_bank_transaction(mud, item_id, 5,
                                               CLIENT_BANK_WITHDRAW);
                }

                int ui_amount = mud->options->offer_x ? 10 : 25;

                if (bank_count >= ui_amount && mud->mouse_x >= mouse_x + 280 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 305 &&
                    mud->mouse_y <= mouse_y + 249) {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               CLIENT_BANK_WITHDRAW);
                }

                ui_amount = mud->options->offer_x ? 50 : 100;

                if (bank_count >= ui_amount && mud->mouse_x >= mouse_x + 305 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 335 &&
                    mud->mouse_y <= mouse_y + 249) {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               CLIENT_BANK_WITHDRAW);
                }

                ui_amount = mud->options->offer_x ? 2 : 500;

                if (bank_count >= ui_amount && mud->mouse_x >= mouse_x + 335 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 368 &&
                    mud->mouse_y <= mouse_y + 249) {
                    if (mud->options->offer_x) {
                        mud->bank_offer_type = BANK_OFFER_WITHDRAW;
                        mud->bank_offer_item = mud->bank_selected_item_slot;
                        mud->show_dialog_offer_x = 1;
                    } else {
                        mudclient_bank_transaction(mud, item_id, ui_amount,
                                                   CLIENT_BANK_WITHDRAW);
                    }
                }

                ui_amount = mud->options->offer_x ? bank_count : 2500;

                if (bank_count >= ui_amount && mud->mouse_x >= mouse_x + 370 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 400 &&
                    mud->mouse_y <= mouse_y + 249) {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               CLIENT_BANK_WITHDRAW);
                }

                int inventory_count =
                    mudclient_get_inventory_count(mud, item_id);

                if (inventory_count >= 1 && mud->mouse_x >= mouse_x + 220 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 250 &&
                    mud->mouse_y <= mouse_y + 274) {
                    mudclient_bank_transaction(mud, item_id, 1,
                                               CLIENT_BANK_DEPOSIT);
                }

                if (inventory_count >= 5 && mud->mouse_x >= mouse_x + 250 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 280 &&
                    mud->mouse_y <= mouse_y + 274) {
                    mudclient_bank_transaction(mud, item_id, 5,
                                               CLIENT_BANK_DEPOSIT);
                }

                ui_amount = mud->options->offer_x ? 10 : 25;

                if (inventory_count >= ui_amount &&
                    mud->mouse_x >= mouse_x + 280 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 305 &&
                    mud->mouse_y <= mouse_y + 274) {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               CLIENT_BANK_DEPOSIT);
                }

                ui_amount = mud->options->offer_x ? 50 : 100;

                if (inventory_count >= ui_amount &&
                    mud->mouse_x >= mouse_x + 305 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 335 &&
                    mud->mouse_y <= mouse_y + 274) {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               CLIENT_BANK_DEPOSIT);
                }

                ui_amount = mud->options->offer_x ? 2 : 500;

                if (inventory_count >= ui_amount &&
                    mud->mouse_x >= mouse_x + 335 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 368 &&
                    mud->mouse_y <= mouse_y + 274) {
                    if (mud->options->offer_x) {
                        mud->bank_offer_type = BANK_OFFER_DEPOSIT;
                        mud->bank_offer_item = item_id;
                        mud->show_dialog_offer_x = 1;
                    } else {
                        mudclient_bank_transaction(mud, item_id, ui_amount,
                                                   CLIENT_BANK_DEPOSIT);
                    }
                }

                ui_amount = mud->options->offer_x ? bank_count : 2500;

                if (inventory_count >= ui_amount &&
                    mud->mouse_x >= mouse_x + 370 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 400 &&
                    mud->mouse_y <= mouse_y + 274) {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               CLIENT_BANK_DEPOSIT);
                }
            }
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE &&
                   mouse_x >= 50 && mouse_x <= 115 && mouse_y <= 12) {
            mud->bank_active_page = 0;
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE &&
                   mouse_x >= 115 && mouse_x <= 180 && mouse_y <= 12) {
            mud->bank_active_page = 1;
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 2 &&
                   mouse_x >= 180 && mouse_x <= 245 && mouse_y <= 12) {
            mud->bank_active_page = 2;
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 3 &&
                   mouse_x >= 245 && mouse_x <= 310 && mouse_y <= 12) {
            mud->bank_active_page = 3;
        } else {
            packet_stream_new_packet(mud->packet_stream, CLIENT_BANK_CLOSE);
            packet_stream_send_packet(mud->packet_stream);
            mud->show_dialog_bank = 0;
            mud->show_dialog_offer_x = 0;
            return;
        }
    }

    int ui_x = (mud->surface->width / 2) - (BANK_WIDTH / 2);
    int ui_y = ((mud->surface->height - 6) / 2) - (BANK_HEIGHT / 2);

    surface_draw_box(mud->surface, ui_x, ui_y, BANK_WIDTH, 12,
                     TITLE_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + 12, BANK_WIDTH, 17,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + 29, 8, 204, GREY_98, 160);

    surface_draw_box_alpha(mud->surface, ui_x + 399, ui_y + 29, 9, 204, GREY_98,
                           160);

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + 233, BANK_WIDTH, 47,
                           GREY_98, 160);

    surface_draw_string(mud->surface, "Bank", ui_x + 1, ui_y + 10, 1, WHITE);

    int offset_x = 50;

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE) {
        int text_colour = WHITE;

        if (mud->bank_active_page == 0) {
            text_colour = RED;
        } else if (mud->mouse_x > ui_x + offset_x && mud->mouse_y >= ui_y &&
                   mud->mouse_x < ui_x + offset_x + 65 &&
                   mud->mouse_y < ui_y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 1>", ui_x + offset_x,
                            ui_y + 10, 1, text_colour);

        offset_x += 65;
        text_colour = WHITE;

        if (mud->bank_active_page == 1) {
            text_colour = RED;
        } else if (mud->mouse_x > ui_x + offset_x && mud->mouse_y >= ui_y &&
                   mud->mouse_x < ui_x + offset_x + 65 &&
                   mud->mouse_y < ui_y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 2>", ui_x + offset_x,
                            ui_y + 10, 1, text_colour);

        offset_x += 65;
    }

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 2) {
        int text_colour = WHITE;

        if (mud->bank_active_page == 2) {
            text_colour = RED;
        } else if (mud->mouse_x > ui_x + offset_x && mud->mouse_y >= ui_y &&
                   mud->mouse_x < ui_x + offset_x + 65 &&
                   mud->mouse_y < ui_y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 3>", ui_x + offset_x,
                            ui_y + 10, 1, text_colour);

        offset_x += 65;
    }

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 3) {
        int text_colour = WHITE;

        if (mud->bank_active_page == 3) {
            text_colour = RED;
        } else if (mud->mouse_x > ui_x + offset_x && mud->mouse_y >= ui_y &&
                   mud->mouse_x < ui_x + offset_x + 65 &&
                   mud->mouse_y < ui_y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 4>", ui_x + offset_x,
                            ui_y + 10, 1, text_colour);
        offset_x += 65;
    }

    int text_colour = WHITE;

    if (mud->mouse_x > ui_x + BANK_WIDTH - 88 && mud->mouse_y >= ui_y &&
        mud->mouse_x < ui_x + BANK_WIDTH && mud->mouse_y < ui_y + 12) {
        text_colour = RED;
    }

    surface_draw_string_right(mud->surface, "Close window", ui_x + 406,
                              ui_y + 10, 1, text_colour);

    surface_draw_string(mud->surface, "Number in bank in green", ui_x + 7,
                        ui_y + 24, 1, GREEN);

    surface_draw_string(mud->surface, "Number held in blue", ui_x + 289,
                        ui_y + 24, 1, CYAN);

    int item_index = mud->bank_active_page * BANK_ITEMS_PER_PAGE;

    for (int row = 0; row < 6; row++) {
        for (int column = 0; column < 8; column++) {
            int slot_x = ui_x + 7 + column * BANK_SLOT_WIDTH;
            int slot_y = ui_y + 28 + row * BANK_SLOT_HEIGHT;

            int slot_colour =
                mud->bank_selected_item_slot == item_index ? RED : GREY_D0;

            surface_draw_box_alpha(mud->surface, slot_x, slot_y,
                                   BANK_SLOT_WIDTH, BANK_SLOT_HEIGHT,
                                   slot_colour, 160);

            surface_draw_box_edge(mud->surface, slot_x, slot_y,
                                  BANK_SLOT_WIDTH + 1, BANK_SLOT_HEIGHT + 1,
                                  BLACK);
            int item_id = mud->bank_items[item_index];

            if (item_index < mud->bank_item_count && item_id != -1) {
                surface_sprite_clipping_from9(
                    mud->surface, slot_x, slot_y, BANK_SLOT_WIDTH - 1,
                    BANK_SLOT_HEIGHT - 2,
                    mud->sprite_item + game_data_item_picture[item_id],
                    game_data_item_mask[item_id], 0, 0, 0);

                char formatted_amount[12] = {0};

                sprintf(formatted_amount, "%d",
                        mud->bank_items_count[item_index]);

                surface_draw_string(mud->surface, formatted_amount, slot_x + 1,
                                    slot_y + 10, 1, GREEN);

                sprintf(formatted_amount, "%d",
                        mudclient_get_inventory_count(
                            mud, mud->bank_items[item_index]));

                surface_draw_string_right(mud->surface, formatted_amount,
                                          slot_x + 47, slot_y + 29, 1, CYAN);
            }

            item_index++;
        }
    }

    surface_draw_line_horizontal(mud->surface, ui_x + 5, ui_y + 256, 398,
                                 BLACK);

    if (mud->bank_selected_item_slot == -1) {
        surface_draw_string_centre(mud->surface,
                                   "Select an object to withdraw or deposit",
                                   ui_x + 204, ui_y + 248, 3, YELLOW);

        return;
    }

    int item_id = 0;

    if (mud->bank_selected_item_slot < 0) {
        item_id = -1;
    } else {
        item_id = mud->bank_items[mud->bank_selected_item_slot];
    }

    if (item_id != -1) {
        int bank_count = mud->bank_items_count[mud->bank_selected_item_slot];

        if (game_data_item_stackable[item_id] == 1 && bank_count > 1) {
            bank_count = 1;
        }

        if (bank_count > 0) {
            char *item_name = game_data_item_name[item_id];
            char formatted_withdraw[strlen(item_name) + 10];

            sprintf(formatted_withdraw, "Withdraw %s", item_name);

            surface_draw_string(mud->surface, formatted_withdraw, ui_x + 2,
                                ui_y + 248, 1, WHITE);

            mudclient_draw_bank_amounts(mud, bank_count,
                                        mud->bank_last_withdraw_offer,
                                        ui_x + 220, ui_y + 238);
        }

        int inventory_count = mudclient_get_inventory_count(mud, item_id);

        if (inventory_count > 0) {
            char *item_name = game_data_item_name[item_id];
            char formatted_deposit[strlen(item_name) + 9];

            sprintf(formatted_deposit, "Deposit %s", item_name);

            surface_draw_string(mud->surface, formatted_deposit, ui_x + 2,
                                ui_y + 273, 1, WHITE);

            mudclient_draw_bank_amounts(mud, inventory_count,
                                        mud->bank_last_deposit_offer,
                                        ui_x + 220, ui_y + 263);
        }
    }

    if (mud->show_dialog_offer_x) {
        mudclient_draw_offer_x(mud);
        mudclient_handle_offer_x_input(mud);
    }
}
