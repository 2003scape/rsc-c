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

void mudclient_draw_bank_page(mudclient *mud, int x, int y, int page) {
    int text_colour = WHITE;

    if (mud->bank_active_page == page - 1) {
        text_colour = RED;
    } else if (mud->mouse_x > x && mud->mouse_y >= y &&
               mud->mouse_x < x + BANK_PAGE_BUTTON_WIDTH &&
               mud->mouse_y < y + 12) {
        text_colour = YELLOW;
    }

    char formatted[19] = {0};
    sprintf(formatted, "<page %d>", page);

    surface_draw_string(mud->surface, formatted, x, y + 10, 1, text_colour);
}

void mudclient_draw_bank_amounts(mudclient *mud, int amount, int last_x, int x,
                                 int y, int transaction_opcode) {
    int item_id = mud->bank_items[mud->bank_selected_item_slot];
    int text_colour = WHITE;
    int offset_x = 0;

    if (mud->mouse_x >= x && mud->mouse_y >= y && mud->mouse_x < x + 30 &&
        mud->mouse_y <= y + 11) {
        text_colour = RED;

        if (mud->mouse_button_down != 0) {
            mud->mouse_button_down = 0;
            mudclient_bank_transaction(mud, item_id, 1, transaction_opcode);
        }
    }

    surface_draw_string(mud->surface, "One", x + 2, y + 10, 1, text_colour);

    offset_x += 30;

    if (amount >= 5) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 30 && mud->mouse_y <= y + 11) {
            text_colour = RED;

            if (mud->mouse_button_down != 0) {
                mud->mouse_button_down = 0;
                mudclient_bank_transaction(mud, item_id, 5, transaction_opcode);
            }
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

            if (mud->mouse_button_down != 0) {
                mud->mouse_button_down = 0;

                mudclient_bank_transaction(mud, item_id, ui_amount,
                                           transaction_opcode);
            }
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

            if (mud->mouse_button_down != 0) {
                mud->mouse_button_down = 0;

                mudclient_bank_transaction(mud, item_id, ui_amount,
                                           transaction_opcode);
            }
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

            if (mud->mouse_button_down != 0) {
                mud->mouse_button_down = 0;

                if (mud->options->offer_x) {
                    mud->bank_offer_type =
                        transaction_opcode == CLIENT_BANK_WITHDRAW
                            ? BANK_OFFER_WITHDRAW
                            : BANK_OFFER_DEPOSIT;

                    mud->show_dialog_offer_x = 1;
                } else {
                    mudclient_bank_transaction(mud, item_id, ui_amount,
                                               transaction_opcode);
                }
            }
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

            if (mud->mouse_button_down != 0) {
                mud->mouse_button_down = 0;

                mudclient_bank_transaction(mud, item_id, last_x,
                                           transaction_opcode);
            }
        }

        char formatted[255] = {0};
        format_amount_suffix(last_x, 0, 1, formatted);

        surface_draw_string(mud->surface, formatted, x + offset_x + 2, y + 10,
                            1, text_colour);
    }

    if (show_last_offer_x) {
        offset_x += 37;
    }

    ui_amount = mud->options->offer_x ? amount : 2500;

    if (amount >= ui_amount) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x + 2 && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 32 && mud->mouse_y <= y + 11) {
            text_colour = RED;

            if (mud->mouse_button_down != 0) {
                mud->mouse_button_down = 0;

                mudclient_bank_transaction(mud, item_id, ui_amount,
                                           transaction_opcode);
            }
        }

        char *ui_text = mud->options->offer_x ? "All" : "2500";

        surface_draw_string(mud->surface, ui_text, x + offset_x + 2, y + 10, 1,
                            text_colour);
    }
}

void mudclient_draw_bank(mudclient *mud) {
    int ui_x = (mud->surface->width / 2) - (BANK_WIDTH / 2);
    int ui_y = ((mud->surface->height - 6) / 2) - (BANK_HEIGHT / 2);
    int page_offset_x = 50;

    if (mud->input_digits_final > 0) {
        if (mud->bank_offer_type == BANK_OFFER_WITHDRAW) {
            int max_amount =
                mud->bank_items_count[mud->bank_selected_item_slot];

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
            int item_id = mud->bank_items[mud->bank_selected_item_slot];
            int max_amount = mudclient_get_inventory_count(mud, item_id);

            if (mud->input_digits_final > max_amount) {
                mudclient_show_message(mud, "You don't have that many!",
                                       MESSAGE_TYPE_GAME);
            } else {
                mudclient_bank_transaction(
                    mud, item_id, mud->input_digits_final, CLIENT_BANK_DEPOSIT);

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

        int mouse_x = mud->mouse_x - ui_x;
        int mouse_y = mud->mouse_y - ui_y;

        if (mud->show_dialog_offer_x && mouse_x > 0 && mouse_x < BANK_WIDTH &&
            mouse_y > 0 && mouse_y < BANK_HEIGHT) {
            mouse_x = 0;
            mouse_y = 13;
        }

        if (mouse_x >= 0 && mouse_y >= 12 && mouse_x < BANK_WIDTH &&
            mouse_y < 280) {
            int slot_index = mud->bank_active_page * BANK_ITEMS_PER_PAGE;

            for (int row = 0; row < 6; row++) {
                for (int column = 0; column < 8; column++) {
                    int slot_x = 7 + column * ITEM_GRID_SLOT_WIDTH;
                    int slot_y = 28 + row * ITEM_GRID_SLOT_HEIGHT;

                    if (mouse_x > slot_x &&
                        mouse_x < slot_x + ITEM_GRID_SLOT_WIDTH &&
                        mouse_y > slot_y &&
                        mouse_y < slot_y + ITEM_GRID_SLOT_HEIGHT &&
                        slot_index < mud->bank_item_count &&
                        mud->bank_items[slot_index] != -1) {
                        mud->bank_selected_item = mud->bank_items[slot_index];
                        mud->bank_selected_item_slot = slot_index;
                    }

                    slot_index++;
                }
            }
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE &&
                   mouse_x >= page_offset_x &&
                   mouse_x <= page_offset_x + BANK_PAGE_BUTTON_WIDTH &&
                   mouse_y <= 12) {
            mud->bank_active_page = 0;
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE &&
                   mouse_x >= (page_offset_x + BANK_PAGE_BUTTON_WIDTH) &&
                   mouse_x <= (page_offset_x + BANK_PAGE_BUTTON_WIDTH * 2) &&
                   mouse_y <= 12) {
            mud->bank_active_page = 1;
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 2 &&
                   mouse_x >= (page_offset_x + BANK_PAGE_BUTTON_WIDTH * 2) &&
                   mouse_x <= (page_offset_x + BANK_PAGE_BUTTON_WIDTH * 3) &&
                   mouse_y <= 12) {
            mud->bank_active_page = 2;
        } else if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 3 &&
                   mouse_x >= (page_offset_x + BANK_PAGE_BUTTON_WIDTH * 3) &&
                   mouse_x <= (page_offset_x + BANK_PAGE_BUTTON_WIDTH * 4) &&
                   mouse_y <= 12) {
            mud->bank_active_page = 3;
        } else {
            packet_stream_new_packet(mud->packet_stream, CLIENT_BANK_CLOSE);
            packet_stream_send_packet(mud->packet_stream);
            mud->show_dialog_bank = 0;
            mud->show_dialog_offer_x = 0;
            return;
        }
    }

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

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE) {
        mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 1);
        page_offset_x += BANK_PAGE_BUTTON_WIDTH;

        mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 2);
        page_offset_x += BANK_PAGE_BUTTON_WIDTH;
    }

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 2) {
        mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 3);
        page_offset_x += BANK_PAGE_BUTTON_WIDTH;
    }

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 3) {
        mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 4);
        page_offset_x += BANK_PAGE_BUTTON_WIDTH;
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

    int page_offset = mud->bank_active_page * BANK_ITEMS_PER_PAGE;

    surface_draw_item_grid(
        mud->surface, ui_x + 7, ui_y + 28, 6, 8, mud->bank_items + page_offset,
        mud->bank_items_count - page_offset, mud->bank_item_count - page_offset,
        mud->bank_selected_item_slot - page_offset);

    surface_draw_line_horizontal(mud->surface, ui_x + 5, ui_y + 256,
                                 BANK_WIDTH - 10, BLACK);

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

        if (!mud->options->offer_x && game_data_item_stackable[item_id] == 1 &&
            bank_count > 1) {
            bank_count = 1;
        }

        if (bank_count > 0) {
            char *item_name = game_data_item_name[item_id];
            char formatted_withdraw[strlen(item_name) + 10];

            sprintf(formatted_withdraw, "Withdraw %s", item_name);

            surface_draw_string(mud->surface, formatted_withdraw, ui_x + 2,
                                ui_y + 248, 1, WHITE);

            mudclient_draw_bank_amounts(
                mud, bank_count, mud->bank_last_withdraw_offer, ui_x + 220,
                ui_y + 238, CLIENT_BANK_WITHDRAW);
        }

        int inventory_count = mudclient_get_inventory_count(mud, item_id);

        if (inventory_count > 0) {
            char *item_name = game_data_item_name[item_id];
            char formatted_deposit[strlen(item_name) + 9];

            sprintf(formatted_deposit, "Deposit %s", item_name);

            surface_draw_string(mud->surface, formatted_deposit, ui_x + 2,
                                ui_y + 273, 1, WHITE);

            mudclient_draw_bank_amounts(
                mud, inventory_count, mud->bank_last_deposit_offer, ui_x + 220,
                ui_y + 263, CLIENT_BANK_DEPOSIT);
        }
    }

    if (mud->show_dialog_offer_x) {
        mudclient_draw_offer_x(mud);
        mudclient_handle_offer_x_input(mud);
    }
}
