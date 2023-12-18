#include "bank.h"

/* handle withdrawing or depositing */
void mudclient_bank_transaction(mudclient *mud, int item_id, int amount,
                                int opcode) {
    int is_withdraw = opcode == CLIENT_BANK_WITHDRAW;

    if (is_withdraw && game_data.items[item_id].stackable != 0 &&
        amount + mud->inventory_items_count >= INVENTORY_ITEMS_MAX) {
        amount = INVENTORY_ITEMS_MAX - mud->inventory_items_count;

        if (amount <= 0) {
            return;
        }
    }

    /* TODO should probably queue if over the packet buffer length */
    int total_packets = (int)ceil((float)amount / 32767.0f);

    for (int i = 0; i < total_packets; i++) {
        packet_stream_new_packet(mud->packet_stream, opcode);
        packet_stream_put_short(mud->packet_stream, item_id);

        int send_amount = amount;

        if (send_amount > 32767) {
            send_amount = 32767;
            amount -= 32767;
        }

        packet_stream_put_short(mud->packet_stream, send_amount);

#ifndef REVISION_177
        packet_stream_put_int(mud->packet_stream, is_withdraw
                                                      ? BANK_MAGIC_WITHDRAW
                                                      : BANK_MAGIC_DEPOSIT);
#endif

        packet_stream_send_packet(mud->packet_stream);
    }

    /* select the item if it isn't already */
    if (mud->bank_selected_item != item_id) {
        // memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
        memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);

        for (int i = 0; i < mud->bank_item_count; i++) {
            int bank_item_id = mud->bank_items[i];

            if (bank_item_id == item_id) {
                mud->bank_selected_item_slot = i;
                mud->bank_selected_item = item_id;
                break;
            }
        }
    }

    /* scroll or switch to selected item page */
    if (MUD_IS_COMPACT || mud->options->bank_scroll) {
        int item_row = mud->bank_selected_item_slot / BANK_COLUMNS;

        if (item_row < mud->bank_scroll_row ||
            item_row > mud->bank_scroll_row + mud->bank_visible_rows) {
            mud->bank_scroll_row = item_row;
        }
    } else {
        int items_per_page = mud->bank_visible_rows * BANK_COLUMNS;
        mud->bank_active_page = mud->bank_selected_item_slot / items_per_page;
    }
}

/* draw the page numbers in the title bar */
void mudclient_draw_bank_page(mudclient *mud, int x, int y, int page,
                              int width) {
    int text_colour = WHITE;

    if (mud->bank_active_page == page - 1) {
        text_colour = RED;
    } else if (mud->mouse_x > x && mud->mouse_y >= y &&
               mud->mouse_x < x + width && mud->mouse_y < y + 12) {
        text_colour = YELLOW;
    }

    char formatted[19] = {0};
    sprintf(formatted, "<page %d>", page);

    surface_draw_string(mud->surface, formatted, x, y + 10, 1, text_colour);
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
            mud->mouse_x < x + offset_x + (show_last_offer_x ? 23 : 30) &&
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
            mud->mouse_x < x + offset_x + (show_last_offer_x ? 16 : 33) &&
            mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        char *ui_text = mud->options->offer_x ? "X" : "500";

        surface_draw_string(mud->surface, ui_text, x + offset_x + 2, y + 10, 1,
                            text_colour);
    }

    offset_x += 33 - (show_last_offer_x ? 17 : 0);

    if (show_last_offer_x && last_x != 0 && amount >= last_x) {
        text_colour = WHITE;

        if (mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
            mud->mouse_x < x + offset_x + 30 && mud->mouse_y <= y + 11) {
            text_colour = RED;
        }

        char formatted[255] = {0};
        format_amount_suffix(last_x, 0, 1, 0, formatted);

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
        }

        char *ui_text = mud->options->offer_x ? "All" : "2500";

        surface_draw_string(mud->surface, ui_text, x + offset_x + 2, y + 10, 1,
                            text_colour);
    }
}

void mudclient_handle_bank_amounts_input(mudclient *mud, int item_id,
                                         int amount, int last_x, int x, int y,
                                         int transaction_opcode) {
    // int item_id = mud->bank_items[mud->bank_selected_item_slot];
    int offset_x = 0;

    if (mud->mouse_x >= x && mud->mouse_y >= y && mud->mouse_x < x + 30 &&
        mud->mouse_y <= y + 11) {
        mudclient_bank_transaction(mud, item_id, 1, transaction_opcode);
    }

    offset_x += 30;

    if (amount >= 5 && mud->mouse_x >= x + offset_x && mud->mouse_y >= y &&
        mud->mouse_x < x + offset_x + 30 && mud->mouse_y <= y + 11) {
        mudclient_bank_transaction(mud, item_id, 5, transaction_opcode);
    }

    offset_x += 30; // 60

    int ui_amount = mud->options->offer_x ? 10 : 25;

    if (amount >= ui_amount && mud->mouse_x >= x + offset_x &&
        mud->mouse_y >= y && mud->mouse_x < x + offset_x + 25 &&
        mud->mouse_y <= y + 11) {
        mudclient_bank_transaction(mud, item_id, ui_amount, transaction_opcode);
    }

    offset_x += 25; // 85

    int show_last_offer_x = mud->options->offer_x && mud->options->last_offer_x;

    ui_amount = mud->options->offer_x ? 50 : 100;

    if (amount >= ui_amount && mud->mouse_x >= x + offset_x &&
        mud->mouse_y >= y &&
        mud->mouse_x < x + offset_x + 30 - (show_last_offer_x ? 7 : 0) &&
        mud->mouse_y <= y + 11) {
        mudclient_bank_transaction(mud, item_id, ui_amount, transaction_opcode);
    }

    offset_x += 30 - (show_last_offer_x ? 7 : 0); // 115 or 108

    ui_amount = mud->options->offer_x ? 2 : 500;

    if (amount >= ui_amount && mud->mouse_x >= x + offset_x &&
        mud->mouse_y >= y &&
        mud->mouse_x < x + offset_x + 33 - (show_last_offer_x ? 17 : 0) &&
        mud->mouse_y <= y + 11) {

        if (mud->options->offer_x) {
            mud->bank_offer_type = transaction_opcode == CLIENT_BANK_WITHDRAW
                                       ? BANK_OFFER_WITHDRAW
                                       : BANK_OFFER_DEPOSIT;

            mud->offer_id = item_id;
            mud->offer_max = amount;

            mud->input_digits_final = 0;
            mud->show_dialog_offer_x = 1;
        } else {
            mudclient_bank_transaction(mud, item_id, ui_amount,
                                       transaction_opcode);
        }
    }

    offset_x += 33 - (show_last_offer_x ? 17 : 0);

    if (show_last_offer_x && last_x != 0 && mud->mouse_x >= x + offset_x &&
        mud->mouse_y >= y && mud->mouse_x < x + offset_x + 30 &&
        mud->mouse_y <= y + 11) {
        mudclient_bank_transaction(mud, item_id, last_x, transaction_opcode);
    }

    if (show_last_offer_x) {
        offset_x += 37;
    }

    ui_amount = mud->options->offer_x ? amount : 2500;

    if (amount >= ui_amount && mud->mouse_x >= x + offset_x + 2 &&
        mud->mouse_y >= y && mud->mouse_x < x + offset_x + 32 &&
        mud->mouse_y <= y + 11) {
        mudclient_bank_transaction(mud, item_id, ui_amount, transaction_opcode);
    }
}

void mudclient_draw_bank(mudclient *mud) {
    int visible_rows =
        (!MUD_IS_COMPACT && mud->options->bank_expand)
            ? (mud->surface->height - 142) / ITEM_GRID_SLOT_HEIGHT
            : BANK_ROWS;

    if (visible_rows > 12) {
        visible_rows = 12;
    } else if (visible_rows < BANK_ROWS) {
        visible_rows = BANK_ROWS;
    }

    mud->bank_visible_rows = visible_rows;

    int offset_y = MUD_IS_COMPACT ? 12 : 0;

    int item_grid_height = visible_rows * ITEM_GRID_SLOT_HEIGHT;
    int bank_height = item_grid_height + 76 - offset_y;

    /*if (mud->options->bank_search) {
        bank_height += 24;
    }*/

    int page_offset_x = 50;
    int page_width = BANK_PAGE_BUTTON_WIDTH;
    int items_per_page = visible_rows * BANK_COLUMNS;

    char bank_title[15] = "Bank";

    if (mud->options->bank_capacity) {
        sprintf(bank_title, "Bank (%d/%d)", mud->bank_item_count,
                mud->bank_items_max);

        page_offset_x = surface_text_width(bank_title, 1) + 5;
        page_width = 60;
    }

    int active_page = mud->bank_active_page;

    int bank_item_offset = (MUD_IS_COMPACT || mud->options->bank_scroll)
                               ? mud->bank_scroll_row * BANK_COLUMNS
                               : active_page * items_per_page;

    int bank_item_count = 0;
    int bank_items[BANK_ITEMS_MAX];
    int bank_items_count[BANK_ITEMS_MAX];

    // TODO strtrim
    if (mud->options->bank_search && strlen(mud->input_pm_current) > 0) {
        /* clear on enter */
        /*if (strlen(mud->input_pm_final)) {
            memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
            memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
        }*/

        int prefix_match_items[BANK_ITEMS_MAX] = {0};
        int prefix_match_items_count[BANK_ITEMS_MAX] = {0};
        int prefix_match_length = 0;

        int contains_match_items[BANK_ITEMS_MAX] = {0};
        int contains_match_items_count[BANK_ITEMS_MAX] = {0};
        int contains_match_length = 0;

        for (int i = 0; i < mud->bank_item_count; i++) {
            char *item_name = game_data.items[mud->bank_items[i]].name;
            char lower_item_name[strlen(item_name) + 1];
            strcpy(lower_item_name, item_name);
            strtolower(lower_item_name);

            int search_length = strlen(mud->input_pm_current);
            char lower_search[search_length + 1];
            strcpy(lower_search, mud->input_pm_current);
            strtolower(lower_search);

            int bank_item_id = mud->bank_items[i];
            int item_amount = mud->bank_items_count[i];

            if (prefix_match_length < BANK_ITEMS_MAX &&
                strncmp(lower_search, lower_item_name, search_length) == 0) {
                prefix_match_items[prefix_match_length] = bank_item_id;
                prefix_match_items_count[prefix_match_length] = item_amount;
                prefix_match_length++;
            } else if (contains_match_length < BANK_ITEMS_MAX &&
                       strstr(lower_item_name, lower_search) != NULL) {
                contains_match_items[contains_match_length] = bank_item_id;
                contains_match_items_count[contains_match_length] = item_amount;
                contains_match_length++;
            }
        }

        /* add items that begin with search first */
        for (int i = 0; i < prefix_match_length; i++) {
            bank_items[i] = prefix_match_items[i];
            bank_items_count[i] = prefix_match_items_count[i];
        }

        for (int i = prefix_match_length; i < BANK_ITEMS_MAX; i++) {
            int contains_index = i - prefix_match_length;

            if (contains_index < contains_match_length) {
                bank_items[i] = contains_match_items[contains_index];

                bank_items_count[i] =
                    contains_match_items_count[contains_index];
            } else {
                bank_items[i] = -1;
                bank_items_count[i] = 0;
            }
        }

        bank_item_count = prefix_match_length + contains_match_length;
    } else {
        bank_item_count = mud->bank_item_count;

        for (int i = 0; i < bank_item_count; i++) {
            bank_items[i] = mud->bank_items[i];
            bank_items_count[i] = mud->bank_items_count[i];
        }
    }

    int show_bank_scroll = (MUD_IS_COMPACT || mud->options->bank_scroll) &&
                           bank_item_count > visible_rows * BANK_COLUMNS;

    int max_scroll_height = item_grid_height - 28;
    int total_rows = ceil(bank_item_count / (float)BANK_COLUMNS);

    int scrub_height =
        max_scroll_height / ((float)total_rows / (float)visible_rows);

    int bank_width = BANK_WIDTH;

    if (show_bank_scroll) {
        bank_width += 13;
    } else {
        mud->bank_scroll_row = 0;
    }

    // TODO DRY
    int inventory_width = ITEM_GRID_SLOT_WIDTH * (MUD_IS_COMPACT ? 6 : 5);

    int show_inventory =
        mud->options->bank_inventory &&
        mud->surface->width > inventory_width + bank_width + 15;

    if (show_inventory) {
        mudclient_draw_ui_tab_inventory(mud, !mud->show_right_click_menu &&
                                                 !mud->show_dialog_offer_x);
    }

    int contain_width = show_inventory && mud->surface->width <
                                              inventory_width + bank_width + 265
                            ? mud->surface->width - inventory_width - 4
                            : mud->surface->width;

    int ui_x = (contain_width / 2) - (bank_width / 2);
    int ui_y = (mud->surface->height / 2) - (bank_height / 2) - 30;

    /* the title bar makes it hard to see */
    if (ui_y < 18 && mud->options->bank_menus) {
        ui_y = 18;
    }

    int item_grid_y = ui_y + 28 - offset_y;

    if (mud->input_digits_final > 0) {
        if (mud->input_digits_final > mud->offer_max) {
            mudclient_show_message(mud, "You don't have that many!",
                                   MESSAGE_TYPE_GAME);
        } else {
            int is_withdraw = mud->bank_offer_type == BANK_OFFER_WITHDRAW;

            mudclient_bank_transaction(
                mud, mud->offer_id, mud->input_digits_final,
                is_withdraw ? CLIENT_BANK_WITHDRAW : CLIENT_BANK_DEPOSIT);

            if (is_withdraw) {
                mud->bank_last_withdraw_offer = mud->input_digits_final;
            } else {
                mud->bank_last_deposit_offer = mud->input_digits_final;
            }
        }

        mud->show_dialog_offer_x = 0;
        mud->input_digits_final = 0;
    }

    if (active_page > 0 && bank_item_count <= items_per_page) {
        mud->bank_active_page = 0;
    }

    if (active_page > 1 && bank_item_count <= (items_per_page * 2)) {
        mud->bank_active_page = 1;
    }

    if (active_page > 2 && bank_item_count <= (items_per_page * 3)) {
        mud->bank_active_page = 2;
    }

    if (mud->bank_selected_item_slot >= bank_item_count ||
        mud->bank_selected_item_slot < 0) {
        mud->bank_selected_item_slot = -1;
    }

    if (mud->bank_selected_item_slot != -1 &&
        bank_items[mud->bank_selected_item_slot] != mud->bank_selected_item) {

        if (mud->options->bank_maintain_slot) {
            if (mud->bank_selected_item != -2) {
                for (int i = 0; i < bank_item_count; i++) {
                    if (bank_items[i] == mud->bank_selected_item) {
                        mud->bank_selected_item_slot = i;
                        break;
                    }
                }
            } else {
                mud->bank_selected_item_slot = -1;
            }
        } else {
            mud->bank_selected_item_slot = -1;
            mud->bank_selected_item = -2;
        }
    }

    int item_id = 0;

    if (mud->bank_selected_item_slot < 0) {
        item_id = -1;
    } else {
        item_id = bank_items[mud->bank_selected_item_slot];
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y + offset_y;
    int mouse_handled = 0;

    if (mud->show_dialog_offer_x) {
        mouse_handled = 1;
    } else if (mouse_x >= 0 && mouse_y >= 12 + offset_y &&
               mouse_x < bank_width && mouse_y < item_grid_height + 76) {
        mouse_handled = 1;

        int slot_index = bank_item_offset;

        for (int row = 0; row < visible_rows; row++) {
            for (int column = 0; column < BANK_COLUMNS; column++) {
                int slot_x = 7 + column * ITEM_GRID_SLOT_WIDTH;
                int slot_y = 28 + row * ITEM_GRID_SLOT_HEIGHT;

                if (mouse_x > slot_x &&
                    mouse_x < slot_x + ITEM_GRID_SLOT_WIDTH &&
                    mouse_y > slot_y &&
                    mouse_y < slot_y + ITEM_GRID_SLOT_HEIGHT &&
                    slot_index < bank_item_count &&
                    bank_items[slot_index] != -1) {
                    int selected_item_id = bank_items[slot_index];

                    /* only left click selects if bank menus are enabled */
                    int clicked_slot = mud->options->bank_menus
                                           ? !mud->show_right_click_menu &&
                                                 mud->mouse_button_click == 1
                                           : mud->mouse_button_click != 0;

                    if (!mud->selected_wiki && !mud->show_dialog_offer_x &&
                        clicked_slot) {
                        /* only withdraw if they click an item that's already
                         * selected */
                        if (mud->bank_selected_item != selected_item_id) {
                            mud->mouse_button_click = 0;
                        }

                        mud->bank_selected_item = selected_item_id;
                        mud->bank_selected_item_slot = slot_index;
                    }

                    char *item_name = game_data.items[selected_item_id].name;

                    char formatted_item_name[strlen(item_name) + 6];
                    sprintf(formatted_item_name, "@lre@%s", item_name);

                    if (mud->selected_wiki) {
                        mudclient_menu_add_id_wiki(mud, formatted_item_name,
                                                   "item", selected_item_id);
                    } else if (mud->options->bank_menus &&
                               !mud->show_right_click_menu) {
                        int item_amount = bank_items_count[slot_index];

                        mudclient_add_offer_menus(
                            mud, "Withdraw", MENU_BANK_WITHDRAW,
                            selected_item_id, item_amount, formatted_item_name,
                            mud->bank_last_withdraw_offer);
                    }
                }

                slot_index++;
            }
        }
    }

    int mouse_in_inventory =
        show_inventory && mud->mouse_x > mud->surface->width - inventory_width;

    if (!mud->show_right_click_menu && mud->mouse_button_click != 0 &&
        !mouse_in_inventory) {
        int bank_count = bank_items_count[mud->bank_selected_item_slot];

        if (!mud->options->bank_unstackble_withdraw &&
            game_data.items[item_id].stackable == 1 && bank_count > 1) {
            bank_count = 1;
        }

        int offset_x = MUD_IS_COMPACT ? 100 : 0;

        if (bank_count > 0) {
            mudclient_handle_bank_amounts_input(
                mud, item_id, bank_count, mud->bank_last_withdraw_offer,
                ui_x + 220 - offset_x,
                ui_y + item_grid_height + (MUD_IS_COMPACT ? 20 : 34),
                CLIENT_BANK_WITHDRAW);
        }

        int inventory_count = mudclient_get_inventory_count(mud, item_id);

        if (inventory_count > 0) {
            mudclient_handle_bank_amounts_input(
                mud, item_id, inventory_count, mud->bank_last_deposit_offer,
                ui_x + 220 - offset_x,
                ui_y + item_grid_height + (MUD_IS_COMPACT ? 39 : 59),
                CLIENT_BANK_DEPOSIT);
        }

        if (mud->show_dialog_offer_x && mouse_x > 0 && mouse_x < bank_width &&
            mouse_y > 0 && mouse_y < bank_height) {
            mouse_x = 0;
            mouse_y = 13;
        }

        if (!mouse_handled) {
            if (!show_bank_scroll && bank_item_count > items_per_page &&
                mouse_x >= page_offset_x &&
                mouse_x <= page_offset_x + page_width && mouse_y <= 12) {
                mud->bank_active_page = 0;
            } else if (!show_bank_scroll && bank_item_count > items_per_page &&
                       mouse_x >= (page_offset_x + page_width) &&
                       mouse_x <= (page_offset_x + page_width * 2) &&
                       mouse_y <= 12) {
                mud->bank_active_page = 1;
            } else if (!show_bank_scroll &&
                       bank_item_count > items_per_page * 2 &&
                       mouse_x >= (page_offset_x + page_width * 2) &&
                       mouse_x <= (page_offset_x + page_width * 3) &&
                       mouse_y <= 12) {
                mud->bank_active_page = 2;
            } else if (!show_bank_scroll &&
                       bank_item_count > items_per_page * 3 &&
                       mouse_x >= (page_offset_x + page_width * 3) &&
                       mouse_x <= (page_offset_x + page_width * 4) &&
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
    }

    if (show_bank_scroll && !mud->show_right_click_menu &&
        !mud->show_dialog_offer_x) {
        if (mud->mouse_scroll_delta != 0) {
            mud->bank_scroll_row -= mud->mouse_scroll_delta * -1;
        }

        if (mud->mouse_button_down != 0 &&
            get_ticks() - mud->bank_last_scroll > BANK_SCROLL_SPEED) {
            int scrollbar_x = (ITEM_GRID_SLOT_WIDTH * BANK_COLUMNS) +
                              (MUD_IS_COMPACT ? 6 : 8);

            /* up arrow */
            if (mud->bank_scroll_row > 0 && mud->mouse_x > ui_x + scrollbar_x &&
                mud->mouse_x < ui_x + scrollbar_x + 21 &&
                mud->mouse_y > item_grid_y && mud->mouse_y < item_grid_y + 15) {
                mud->bank_scroll_row -= 1;
                mud->bank_last_scroll = get_ticks();
            }

            /* scrub area */
            int is_dragging = mud->bank_handle_dragged;

            if (!mud->options->off_handle_scroll_drag) {
                is_dragging = is_dragging &&
                              mud->mouse_x >= ui_x + scrollbar_x - 12 &&
                              mud->mouse_x <= ui_x + scrollbar_x + 33;
            }

            if ((is_dragging || (mud->mouse_x > ui_x + scrollbar_x &&
                                 mud->mouse_x < ui_x + scrollbar_x + 21)) &&
                mud->mouse_y > item_grid_y + 15 &&
                mud->mouse_y < ui_y + item_grid_height + 15 - offset_y) {
                int scrub_y =
                    mud->mouse_y - (ui_y + 43 - offset_y) - (scrub_height / 2);

                int scroll_row =
                    (scrub_y / (float)max_scroll_height) * total_rows;

                mud->bank_scroll_row = scroll_row;
                mud->bank_handle_dragged = 1;
            }

            /* down arrow */
            if (mud->bank_scroll_row < (total_rows - visible_rows) &&
                mud->mouse_x > ui_x + scrollbar_x &&
                mud->mouse_x < ui_x + scrollbar_x + 21 &&
                mud->mouse_y > ui_y + item_grid_height + 15 - offset_y &&
                mud->mouse_y < ui_y + item_grid_height + 30 - offset_y) {
                mud->bank_scroll_row += 1;
                mud->bank_last_scroll = get_ticks();
            }
        } else {
            mud->bank_handle_dragged = 0;
        }

        if (mud->bank_scroll_row < 0) {
            mud->bank_scroll_row = 0;
        } else if (mud->bank_scroll_row > total_rows - visible_rows) {
            mud->bank_scroll_row = total_rows - visible_rows;
        }
    }

    surface_draw_box(mud->surface, ui_x, ui_y, bank_width, 12,
                     TITLE_BAR_COLOUR);

    /* alpha box under title bar */
    surface_draw_box_alpha(mud->surface, ui_x, ui_y + 12, bank_width,
                           17 - offset_y, GREY_98, 160);

    /* alpha box to left of item grid */
    surface_draw_box_alpha(mud->surface, ui_x, ui_y + 29 - offset_y,
                           MUD_IS_COMPACT ? 5 : 8, item_grid_height, GREY_98,
                           160);

    /* alpha box to right of item grid */
    surface_draw_box_alpha(
        mud->surface,
        ui_x + (ITEM_GRID_SLOT_WIDTH * BANK_COLUMNS) + (MUD_IS_COMPACT ? 5 : 7),
        ui_y + 29 - offset_y,
        9 + (bank_width - BANK_WIDTH) - (MUD_IS_COMPACT ? 3 : 0),
        item_grid_height, GREY_98, 160);

    surface_draw_string(mud->surface, bank_title, ui_x + 1, ui_y + 10, 1,
                        WHITE);

    if (!show_bank_scroll) {
        if (bank_item_count > items_per_page) {
            mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 1,
                                     page_width);

            page_offset_x += page_width;

            mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 2,
                                     page_width);

            page_offset_x += page_width;
        }

        if (bank_item_count > items_per_page * 2) {
            mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 3,
                                     page_width);

            page_offset_x += page_width;
        }

        if (bank_item_count > items_per_page * 3) {
            mudclient_draw_bank_page(mud, ui_x + page_offset_x, ui_y, 4,
                                     page_width);

            page_offset_x += page_width;
        }
    }

    int text_colour = WHITE;

    if (mud->mouse_x > ui_x + bank_width - 88 && mud->mouse_y >= ui_y &&
        mud->mouse_x < ui_x + bank_width && mud->mouse_y < ui_y + 12) {
        text_colour = RED;
    }

    surface_draw_string_right(mud->surface, "Close window",
                              ui_x + bank_width - 2, ui_y + 10, 1, text_colour);

    if (!MUD_IS_COMPACT) {
        surface_draw_string(mud->surface, "Number in bank in green", ui_x + 7,
                            ui_y + 24, 1, GREEN);

        surface_draw_string(mud->surface, "Number held in blue",
                            ui_x + bank_width - 119, ui_y + 24, 1, CYAN);

        if (mud->options->bank_value) {
            int total_value = 0;

            for (int i = 0; i < mud->bank_item_count; i++) {
                for (int j = 0; j < mud->bank_items_count[i]; j++) {
                    total_value +=
                        game_data.items[mud->bank_items[i]].base_price;
                }

                /* overflow */
                if (total_value < 0) {
                    break;
                }
            }

            char formatted_money[30] = {0};

            if (total_value < 0) {
                sprintf(formatted_money, "Total value: a lot!");
            } else {
                char formatted_amount[15] = {0};

                mudclient_format_number_commas(mud, total_value,
                                               formatted_amount);

                sprintf(formatted_money, "Total value: %sgp", formatted_amount);
            }

            surface_draw_string_centre(mud->surface, formatted_money,
                                       ui_x + (bank_width / 2) + 12, ui_y + 24,
                                       total_value >= 10000000 ? 0 : 1, YELLOW);
        }
    }

    surface_draw_item_grid(
        mud->surface, ui_x + (MUD_IS_COMPACT ? 5 : 7), item_grid_y,
        visible_rows, BANK_COLUMNS, ITEM_GRID_SLOT_WIDTH, ITEM_GRID_SLOT_HEIGHT,
        bank_items + bank_item_offset, bank_items_count + bank_item_offset,
        bank_item_count - bank_item_offset,
        mud->bank_selected_item_slot - bank_item_offset, 1);

    if (show_bank_scroll) {
        int scrub_y = ((float)mud->bank_scroll_row / (float)total_rows) *
                      (float)max_scroll_height;

        int scrollbar_x =
            (ITEM_GRID_SLOT_WIDTH * BANK_COLUMNS) - (MUD_IS_COMPACT ? 4 : 0);

        surface_draw_scrollbar(mud->surface, ui_x + 24, item_grid_y,
                               scrollbar_x, item_grid_height, 1 + scrub_y,
                               scrub_height);

        surface_draw_line_vertical(mud->surface, ui_x + scrollbar_x + 24,
                                   item_grid_y, item_grid_height + 1, BLACK);

        surface_draw_line_horizontal(mud->surface, ui_x + scrollbar_x + 12,
                                     item_grid_y + item_grid_height, 12, BLACK);
    }

    surface_draw_box_alpha(mud->surface, ui_x,
                           ui_y + (item_grid_height - 204) + 233 - offset_y,
                           bank_width, 47 - offset_y, GREY_98, 160);

    surface_draw_line_horizontal(mud->surface, ui_x + 5,
                                 ui_y + item_grid_height +
                                     (MUD_IS_COMPACT ? 34 : 52),
                                 bank_width - 10, BLACK);

    if (mud->options->bank_search) {
        surface_draw_box_alpha(
            mud->surface, ui_x,
            ui_y + item_grid_height + (MUD_IS_COMPACT ? 52 : 76), bank_width,
            (MUD_IS_COMPACT ? 18 : 24), GREY_98, 160);

        surface_draw_line_horizontal(mud->surface, ui_x + 5,
                                     ui_y + item_grid_height +
                                         (MUD_IS_COMPACT ? 52 : 76),
                                     bank_width - 10, BLACK);

        char formatted_search[INPUT_PM_LENGTH + 10] = {0};

        sprintf(formatted_search, "Search: %s*", mud->input_pm_current);

        surface_draw_string(
            mud->surface, formatted_search, ui_x + 2,
            ui_y + item_grid_height + (MUD_IS_COMPACT ? 66 : 92), 1, WHITE);
    }

    if (item_id != -1) {
        int bank_count = bank_items_count[mud->bank_selected_item_slot];

        if (!mud->options->bank_unstackble_withdraw &&
            game_data.items[item_id].stackable == 1 && bank_count > 1) {
            bank_count = 1;
        }

        int offset_x = MUD_IS_COMPACT ? 100 : 0;

        if (bank_count > 0) {
            char *item_name = game_data.items[item_id].name;
            char formatted_withdraw[strlen(item_name) + 10];

            sprintf(formatted_withdraw, "Withdraw %s",
                    MUD_IS_COMPACT ? "" : item_name);

            surface_draw_string(
                mud->surface, formatted_withdraw, ui_x + 2,
                ui_y + item_grid_height + (MUD_IS_COMPACT ? 30 : 44), 1, WHITE);

            mudclient_draw_bank_amounts(
                mud, bank_count, mud->bank_last_withdraw_offer,
                ui_x + 220 - offset_x,
                ui_y + item_grid_height + (MUD_IS_COMPACT ? 20 : 34));
        }

        int inventory_count = mudclient_get_inventory_count(mud, item_id);

        if (inventory_count > 0) {
            char *item_name = game_data.items[item_id].name;
            char formatted_deposit[strlen(item_name) + 9];

            sprintf(formatted_deposit, "Deposit %s",
                    MUD_IS_COMPACT ? "" : item_name);

            surface_draw_string(
                mud->surface, formatted_deposit, ui_x + 2,
                ui_y + item_grid_height + (MUD_IS_COMPACT ? 48 : 69), 1, WHITE);

            mudclient_draw_bank_amounts(
                mud, inventory_count, mud->bank_last_deposit_offer,
                ui_x + 220 - offset_x,
                ui_y + item_grid_height + (MUD_IS_COMPACT ? 39 : 59));
        }
    } else {
        surface_draw_string_centre(
            mud->surface, "Select an object to withdraw or deposit",
            ui_x + BANK_WIDTH / 2,
            ui_y + item_grid_height + (MUD_IS_COMPACT ? 30 : 44), 3, YELLOW);
    }

    if (mud->show_dialog_offer_x) {
        mudclient_draw_offer_x(mud);
        mudclient_handle_offer_x_input(mud);
    }
}
