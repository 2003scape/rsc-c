#include "bank.h"

void mudclient_draw_bank(mudclient *mud) {
    if (mud->bank_active_page > 0 &&
        mud->bank_item_count <= BANK_ITEMS_PER_PAGE) {
        mud->bank_active_page = 0;
    }

    if (mud->bank_active_page > 1 && mud->bank_item_count <= 96) {
        mud->bank_active_page = 1;
    }

    if (mud->bank_active_page > 2 && mud->bank_item_count <= 144) {
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

        int mouse_x = mud->mouse_x - ((mud->surface->width / 2) - (BANK_WIDTH / 2));

        int mouse_y = mud->mouse_y - ((mud->surface->height / 2) - (BANK_HEIGHT / 2));

        if (mouse_x >= 0 && mouse_y >= 12 && mouse_x < BANK_WIDTH && mouse_y < 280) {
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

            mouse_x = 256 - (BANK_WIDTH / 2);
            mouse_y = 170 - (BANK_HEIGHT / 2);

            int item_id = 0;

            if (mud->bank_selected_item_slot < 0) {
                item_id = -1;
            } else {
                item_id = mud->bank_items[mud->bank_selected_item_slot];
            }

            if (item_id != -1) {
                int item_amount =
                    mud->bank_items_count[mud->bank_selected_item_slot];

                if (game_data_item_stackable[item_id] == 1 && item_amount > 1) {
                    item_amount = 1;
                }

                if (item_amount >= 1 && mud->mouse_x >= mouse_x + 220 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 250 &&
                    mud->mouse_y <= mouse_y + 249) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_WITHDRAW);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 1);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_WITHDRAW);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (item_amount >= 5 && mud->mouse_x >= mouse_x + 250 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 280 &&
                    mud->mouse_y <= mouse_y + 249) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_WITHDRAW);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 5);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_WITHDRAW);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (item_amount >= 25 && mud->mouse_x >= mouse_x + 280 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 305 &&
                    mud->mouse_y <= mouse_y + 249) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_WITHDRAW);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 25);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_WITHDRAW);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (item_amount >= 100 && mud->mouse_x >= mouse_x + 305 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 335 &&
                    mud->mouse_y <= mouse_y + 249) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_WITHDRAW);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 100);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_WITHDRAW);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (item_amount >= 500 && mud->mouse_x >= mouse_x + 335 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 368 &&
                    mud->mouse_y <= mouse_y + 249) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_WITHDRAW);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 500);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_WITHDRAW);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (item_amount >= 2500 && mud->mouse_x >= mouse_x + 370 &&
                    mud->mouse_y >= mouse_y + 238 &&
                    mud->mouse_x < mouse_x + 400 &&
                    mud->mouse_y <= mouse_y + 249) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_WITHDRAW);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 2500);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_WITHDRAW);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (mudclient_get_inventory_count(mud, item_id) >= 1 &&
                    mud->mouse_x >= mouse_x + 220 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 250 &&
                    mud->mouse_y <= mouse_y + 274) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_DEPOSIT);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 1);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_DEPOSIT);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (mudclient_get_inventory_count(mud, item_id) >= 5 &&
                    mud->mouse_x >= mouse_x + 250 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 280 &&
                    mud->mouse_y <= mouse_y + 274) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_DEPOSIT);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 5);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_DEPOSIT);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (mudclient_get_inventory_count(mud, item_id) >= 25 &&
                    mud->mouse_x >= mouse_x + 280 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 305 &&
                    mud->mouse_y <= mouse_y + 274) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_DEPOSIT);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 25);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_DEPOSIT);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (mudclient_get_inventory_count(mud, item_id) >= 100 &&
                    mud->mouse_x >= mouse_x + 305 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 335 &&
                    mud->mouse_y <= mouse_y + 274) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_DEPOSIT);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 100);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_DEPOSIT);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (mudclient_get_inventory_count(mud, item_id) >= 500 &&
                    mud->mouse_x >= mouse_x + 335 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 368 &&
                    mud->mouse_y <= mouse_y + 274) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_DEPOSIT);

                    packet_stream_put_short(mud->packet_stream, item_id);
                    packet_stream_put_short(mud->packet_stream, 500);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_DEPOSIT);
#endif

                    packet_stream_send_packet(mud->packet_stream);
                }

                if (mudclient_get_inventory_count(mud, item_id) >= 2500 &&
                    mud->mouse_x >= mouse_x + 370 &&
                    mud->mouse_y >= mouse_y + 263 &&
                    mud->mouse_x < mouse_x + 400 &&
                    mud->mouse_y <= mouse_y + 274) {
                    packet_stream_new_packet(mud->packet_stream,
                                             CLIENT_BANK_DEPOSIT);

                    packet_stream_put_short(mud->packet_stream, item_id);

                    packet_stream_put_short(mud->packet_stream, 2500);

#ifndef REVISION_177
                    packet_stream_put_int(mud->packet_stream,
                                          BANK_MAGIC_DEPOSIT);
#endif

                    packet_stream_send_packet(mud->packet_stream);
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
            return;
        }
    }

    int x = (mud->surface->width / 2) - (BANK_WIDTH / 2);
    int y = (mud->surface->height / 2) - (BANK_HEIGHT / 2);

    surface_draw_box(mud->surface, x, y, 408, 12, TITLE_BAR_COLOUR);
    surface_draw_box_alpha(mud->surface, x, y + 12, 408, 17, GREY_98, 160);
    surface_draw_box_alpha(mud->surface, x, y + 29, 8, 204, GREY_98, 160);
    surface_draw_box_alpha(mud->surface, x + 399, y + 29, 9, 204, GREY_98, 160);
    surface_draw_box_alpha(mud->surface, x, y + 233, 408, 47, GREY_98, 160);
    surface_draw_string(mud->surface, "Bank", x + 1, y + 10, 1, WHITE);

    int offset_x = 50;

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE) {
        int text_colour = WHITE;

        if (mud->bank_active_page == 0) {
            text_colour = RED;
        } else if (mud->mouse_x > x + offset_x && mud->mouse_y >= y &&
                   mud->mouse_x < x + offset_x + 65 && mud->mouse_y < y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 1>", x + offset_x, y + 10, 1,
                            text_colour);

        offset_x += 65;
        text_colour = WHITE;

        if (mud->bank_active_page == 1) {
            text_colour = RED;
        } else if (mud->mouse_x > x + offset_x && mud->mouse_y >= y &&
                   mud->mouse_x < x + offset_x + 65 && mud->mouse_y < y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 2>", x + offset_x, y + 10, 1,
                            text_colour);

        offset_x += 65;
    }

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 2) {
        int text_colour = WHITE;

        if (mud->bank_active_page == 2) {
            text_colour = RED;
        } else if (mud->mouse_x > x + offset_x && mud->mouse_y >= y &&
                   mud->mouse_x < x + offset_x + 65 && mud->mouse_y < y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 3>", x + offset_x, y + 10, 1,
                            text_colour);

        offset_x += 65;
    }

    if (mud->bank_item_count > BANK_ITEMS_PER_PAGE * 3) {
        int text_colour = WHITE;

        if (mud->bank_active_page == 3) {
            text_colour = RED;
        } else if (mud->mouse_x > x + offset_x && mud->mouse_y >= y &&
                   mud->mouse_x < x + offset_x + 65 && mud->mouse_y < y + 12) {
            text_colour = YELLOW;
        }

        surface_draw_string(mud->surface, "<page 4>", x + offset_x, y + 10, 1,
                            text_colour);
        offset_x += 65;
    }

    int text_colour = WHITE;

    if (mud->mouse_x > x + 320 && mud->mouse_y >= y && mud->mouse_x < x + 408 &&
        mud->mouse_y < y + 12) {
        text_colour = RED;
    }

    surface_draw_string_right(mud->surface, "Close window", x + 406, y + 10, 1,
                              text_colour);

    surface_draw_string(mud->surface, "Number in bank in green", x + 7, y + 24,
                        1, GREEN);

    surface_draw_string(mud->surface, "Number held in blue", x + 289, y + 24, 1,
                        CYAN);

    int selected_index = mud->bank_active_page * BANK_ITEMS_PER_PAGE;

    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 8; col++) {
            int slot_x = x + 7 + col * BANK_SLOT_WIDTH;
            int slot_y = y + 28 + row * BANK_SLOT_HEIGHT;

            int slot_colour =
                mud->bank_selected_item_slot == selected_index ? RED : GREY_D0;

            surface_draw_box_alpha(mud->surface, slot_x, slot_y,
                                   BANK_SLOT_WIDTH, BANK_SLOT_HEIGHT,
                                   slot_colour, 160);

            surface_draw_box_edge(mud->surface, slot_x, slot_y, 50, 35, BLACK);

            if (selected_index < mud->bank_item_count &&
                mud->bank_items[selected_index] != -1) {
                surface_sprite_clipping_from9(
                    mud->surface, slot_x, slot_y, BANK_SLOT_WIDTH - 1,
                    BANK_SLOT_HEIGHT - 2,
                    mud->sprite_item +
                        game_data_item_picture[mud->bank_items[selected_index]],
                    game_data_item_mask[mud->bank_items[selected_index]], 0, 0,
                    0);

                char formatted_amount[12] = {0};

                sprintf(formatted_amount, "%d",
                        mud->bank_items_count[selected_index]);

                surface_draw_string(mud->surface, formatted_amount, slot_x + 1,
                                    slot_y + 10, 1, GREEN);

                sprintf(formatted_amount, "%d",
                        mudclient_get_inventory_count(
                            mud, mud->bank_items[selected_index]));

                surface_draw_string_right(mud->surface, formatted_amount,
                                          slot_x + 47, slot_y + 29, 1, CYAN);
            }

            selected_index++;
        }
    }

    surface_draw_line_horizontal(mud->surface, x + 5, y + 256, 398, BLACK);

    if (mud->bank_selected_item_slot == -1) {
        surface_draw_string_centre(mud->surface,
                                   "Select an object to withdraw or deposit",
                                   x + 204, y + 248, 3, YELLOW);

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

            surface_draw_string(mud->surface, formatted_withdraw, x + 2,
                                y + 248, 1, WHITE);

            text_colour = WHITE;

            if (mud->mouse_x >= x + 220 && mud->mouse_y >= y + 238 &&
                mud->mouse_x < x + 250 && mud->mouse_y <= y + 249) {
                text_colour = RED;
            }

            surface_draw_string(mud->surface, "One", x + 222, y + 248, 1,
                                text_colour);

            if (bank_count >= 5) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 250 && mud->mouse_y >= y + 238 &&
                    mud->mouse_x < x + 280 && mud->mouse_y <= y + 249) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "Five", x + 252, y + 248, 1,
                                    text_colour);
            }

            if (bank_count >= 25) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 280 && mud->mouse_y >= y + 238 &&
                    mud->mouse_x < x + 305 && mud->mouse_y <= y + 249) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "25", x + 282, y + 248, 1,
                                    text_colour);
            }

            if (bank_count >= 100) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 305 && mud->mouse_y >= y + 238 &&
                    mud->mouse_x < x + 335 && mud->mouse_y <= y + 249) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "100", x + 307, y + 248, 1,
                                    text_colour);
            }

            if (bank_count >= 500) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 335 && mud->mouse_y >= y + 238 &&
                    mud->mouse_x < x + 368 && mud->mouse_y <= y + 249) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "500", x + 337, y + 248, 1,
                                    text_colour);
            }

            if (bank_count >= 2500) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 370 && mud->mouse_y >= y + 238 &&
                    mud->mouse_x < x + 400 && mud->mouse_y <= y + 249) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "2500", x + 370, y + 248, 1,
                                    text_colour);
            }
        }

        int inventory_count = mudclient_get_inventory_count(mud, item_id);

        if (inventory_count > 0) {
            char *item_name = game_data_item_name[item_id];
            char formatted_deposit[strlen(item_name) + 9];

            sprintf(formatted_deposit, "Deposit %s", item_name);

            surface_draw_string(mud->surface, formatted_deposit, x + 2, y + 273,
                                1, WHITE);

            text_colour = WHITE;

            if (mud->mouse_x >= x + 220 && mud->mouse_y >= y + 263 &&
                mud->mouse_x < x + 250 && mud->mouse_y <= y + 274) {
                text_colour = RED;
            }

            surface_draw_string(mud->surface, "One", x + 222, y + 273, 1,
                                text_colour);

            if (inventory_count >= 5) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 250 && mud->mouse_y >= y + 263 &&
                    mud->mouse_x < x + 280 && mud->mouse_y <= y + 274) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "Five", x + 252, y + 273, 1,
                                    text_colour);
            }

            if (inventory_count >= 25) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 280 && mud->mouse_y >= y + 263 &&
                    mud->mouse_x < x + 305 && mud->mouse_y <= y + 274) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "25", x + 282, y + 273, 1,
                                    text_colour);
            }

            if (inventory_count >= 100) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 305 && mud->mouse_y >= y + 263 &&
                    mud->mouse_x < x + 335 && mud->mouse_y <= y + 274) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "100", x + 307, y + 273, 1,
                                    text_colour);
            }

            if (inventory_count >= 500) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 335 && mud->mouse_y >= y + 263 &&
                    mud->mouse_x < x + 368 && mud->mouse_y <= y + 274) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "500", x + 337, y + 273, 1,
                                    text_colour);
            }

            if (inventory_count >= 2500) {
                text_colour = WHITE;

                if (mud->mouse_x >= x + 370 && mud->mouse_y >= y + 263 &&
                    mud->mouse_x < x + 400 && mud->mouse_y <= y + 274) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "2500", x + 370, y + 273, 1,
                                    text_colour);
            }
        }
    }
}
