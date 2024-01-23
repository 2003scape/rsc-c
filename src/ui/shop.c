#include "shop.h"

void mudclient_draw_shop(mudclient *mud) {
    int is_compact =
        mud->surface->width < (SHOP_COLUMNS * ITEM_GRID_SLOT_WIDTH) + 16 ||
        mud->surface->height < (SHOP_ROWS * ITEM_GRID_SLOT_HEIGHT) + 96;

    int slot_width = (is_compact ? 39 : ITEM_GRID_SLOT_WIDTH);
    int slot_height = (is_compact ? 27 : ITEM_GRID_SLOT_HEIGHT);

    int shop_width = ((slot_width * SHOP_COLUMNS) + (is_compact ? 11 : 16));
    int shop_height = ((slot_height * SHOP_ROWS) + 76);

    int x = (mud->surface->width / 2) - (shop_width / 2);
    int y = (mud->surface->height / 2) - (shop_height / 2) - 6;
    int item_grid_height = slot_height * SHOP_ROWS;
    int mouse_x = mud->mouse_x - x;
    int mouse_y = mud->mouse_y - y;

    if (mud->shop_selected_item_index >= 0 &&
        (mud->mouse_button_click || mud->mouse_item_count_increment)) {

        int item_id = mud->shop_items[mud->shop_selected_item_index];
        if (item_id != -1) {
            if (mud->shop_items_count[mud->shop_selected_item_index] > 0 &&
                (is_compact
                     ? (mouse_x > x && mouse_x < shop_width)
                     : (mouse_x > shop_width - 110 && mouse_x < shop_width)) &&
                mouse_y >= item_grid_height + 34 &&
                mouse_y <= item_grid_height + 45) {

                int price_mod =
                    mud->shop_buy_price_mod +
                    mud->shop_items_price[mud->shop_selected_item_index];

                if (price_mod < 10) {
                    price_mod = 10;
                }

                int item_price =
                    (price_mod * game_data.items[item_id].base_price) / 100;

                packet_stream_new_packet(mud->packet_stream, CLIENT_SHOP_BUY);

                packet_stream_put_short(
                    mud->packet_stream,
                    mud->shop_items[mud->shop_selected_item_index]);

                packet_stream_put_int(mud->packet_stream, item_price);
                packet_stream_send_packet(mud->packet_stream);
            } else if (mudclient_get_inventory_count(mud, item_id) > 0 &&
                       (is_compact ? (mouse_x > x && mouse_x < shop_width)
                                   : (mouse_x > 2 && mouse_x < 112)) &&
                       mouse_y >= item_grid_height + 59 &&
                       mouse_y <= item_grid_height + 70) {

                int price_mod =
                    mud->shop_sell_price_mod +
                    mud->shop_items_price[mud->shop_selected_item_index];

                if (price_mod < 10) {
                    price_mod = 10;
                }

                int item_price =
                    (price_mod * game_data.items[item_id].base_price) / 100;

                packet_stream_new_packet(mud->packet_stream, CLIENT_SHOP_SELL);

                packet_stream_put_short(
                    mud->packet_stream,
                    mud->shop_items[mud->shop_selected_item_index]);

                packet_stream_put_int(mud->packet_stream, item_price);
                packet_stream_send_packet(mud->packet_stream);
            } else {
                mud->mouse_item_count_increment = 0;
            }
        }
    }

    if (mud->mouse_button_click != 0) {
        mud->mouse_button_click = 0;

        if (mouse_x >= 0 && mouse_y >= 12 && mouse_x < shop_width &&
            mouse_y < shop_height) {
            int item_index = 0;

            for (int row = 0; row < SHOP_ROWS; row++) {
                for (int column = 0; column < SHOP_COLUMNS; column++) {
                    int slot_x = 7 + column * slot_width;
                    int slot_y = 28 + row * slot_height;

                    if (mouse_x > slot_x && mouse_x < slot_x + slot_width &&
                        mouse_y > slot_y && mouse_y < slot_y + slot_height &&
                        mud->shop_items[item_index] != -1) {
                        mud->shop_selected_item_index = item_index;

                        mud->shop_selected_item_type =
                            mud->shop_items[item_index];
                    }

                    item_index++;
                }
            }
        } else {
            packet_stream_new_packet(mud->packet_stream, CLIENT_SHOP_CLOSE);
            packet_stream_send_packet(mud->packet_stream);
            mud->show_dialog_shop = 0;
            return;
        }
    }

    surface_draw_box(mud->surface, x, y, shop_width, 12, TITLE_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, x, y + 12, shop_width, 17, GREY_98,
                           160);

    surface_draw_box_alpha(mud->surface, x, y + 29, (is_compact ? 5 : 8),
                           item_grid_height, GREY_98, 160);

    surface_draw_box_alpha(
        mud->surface, x + (slot_width * SHOP_COLUMNS) + (is_compact ? 5 : 7),
        y + 29, (is_compact ? 6 : 9), item_grid_height, GREY_98, 160);

    surface_draw_box_alpha(mud->surface, x, y + item_grid_height + 29,
                           shop_width, 47, GREY_98, 160);

    surface_draw_string(mud->surface, "Buying and selling items", x + 1, y + 10,
                        FONT_BOLD_12, WHITE);

    int text_colour = WHITE;

    if (mud->mouse_x > x + shop_width - 88 && mud->mouse_y >= y &&
        mud->mouse_x < x + shop_width && mud->mouse_y < y + 12) {
        text_colour = RED;
    }

    surface_draw_string_right(mud->surface, "Close window", x + shop_width - 2,
                              y + 10, FONT_BOLD_12, text_colour);

    if (!is_compact) {
        surface_draw_string(mud->surface, "Shops stock in green", x + 2, y + 24,
                            FONT_BOLD_12, GREEN);

        surface_draw_string(mud->surface, "Number you own in blue", x + 135,
                            y + 24, FONT_BOLD_12, CYAN);
    }

    char formatted_amount[15] = {0};

    mudclient_format_number_commas(
        mud, mudclient_get_inventory_count(mud, COINS_ID), formatted_amount);

    char formatted_money[29] = {0};

    sprintf(formatted_money, "Your money: %sgp", formatted_amount);

    if (is_compact) {
        surface_draw_string_centre(mud->surface, formatted_money,
                                   x + shop_width / 2, y + 24, FONT_BOLD_12,
                                   YELLOW);
    } else {
        surface_draw_string(mud->surface, formatted_money, x + 280, y + 24,
                            FONT_BOLD_12, YELLOW);
    }

    surface_draw_item_grid(
        mud->surface, x + (is_compact ? 4 : 7), y + 28, SHOP_ROWS, SHOP_COLUMNS,
        slot_width, slot_height, mud->shop_items, mud->shop_items_count,
        SHOP_ROWS * SHOP_COLUMNS, mud->shop_selected_item_index, 1);

    surface_draw_line_horizontal(mud->surface, x + 5, y + item_grid_height + 52,
                                 shop_width - 10, BLACK);

    if (mud->shop_selected_item_index == -1) {
        surface_draw_string_centre(
            mud->surface, "Select an object to buy or sell",
            x + (shop_width / 2), y + item_grid_height + 44, 3, YELLOW);

        return;
    }

    int selected_item_id = mud->shop_items[mud->shop_selected_item_index];

    if (selected_item_id != -1) {
        if (mud->shop_items_count[mud->shop_selected_item_index] > 0) {
            int price_mod =
                mud->shop_buy_price_mod +
                mud->shop_items_price[mud->shop_selected_item_index];

            if (price_mod < 10) {
                price_mod = 10;
            }

            int item_price =
                (price_mod * game_data.items[selected_item_id].base_price) /
                100;

            char *item_name = game_data.items[selected_item_id].name;

            char formatted_buy[strlen(item_name) + 30];

            sprintf(formatted_buy, "Buy a new %s for %dgp", item_name,
                    item_price);

            if (is_compact) {
                text_colour = WHITE;

                if (mud->mouse_x > x &&
                    mud->mouse_y >= y + item_grid_height + +34 &&
                    mud->mouse_x < x + shop_width &&
                    mud->mouse_y <= y + item_grid_height + 45) {
                    text_colour = RED;
                }

                surface_draw_string_centre(
                    mud->surface, formatted_buy, x + (shop_width / 2),
                    y + item_grid_height + 44, FONT_BOLD_12, text_colour);

            } else {
                surface_draw_string(mud->surface, formatted_buy, x + 2,
                                    y + item_grid_height + 44, FONT_BOLD_12,
                                    YELLOW);

                text_colour = WHITE;

                if (mud->mouse_x > x + shop_width - 110 &&
                    mud->mouse_y >= y + item_grid_height + +34 &&
                    mud->mouse_x < x + shop_width &&
                    mud->mouse_y <= y + item_grid_height + 45) {
                    text_colour = RED;
                }

                surface_draw_string_right(
                    mud->surface, "Click here to buy", x + shop_width - 3,
                    y + item_grid_height + 44, FONT_BOLD_13, text_colour);
            }
        } else {
            surface_draw_string_centre(
                mud->surface, "This item is not currently available to buy",
                x + (shop_width / 2), y + item_grid_height + 44, FONT_BOLD_13,
                YELLOW);
        }

        if (mudclient_get_inventory_count(mud, selected_item_id) > 0) {
            int price_mod =
                mud->shop_sell_price_mod +
                mud->shop_items_price[mud->shop_selected_item_index];

            if (price_mod < 10) {
                price_mod = 10;
            }

            int item_price =
                (price_mod * game_data.items[selected_item_id].base_price) /
                100;

            char *item_name = game_data.items[selected_item_id].name;

            char formatted_sell[strlen(item_name) + 29];

            sprintf(formatted_sell, "Sell your %s for %dgp", item_name,
                    item_price);

            if (is_compact) {
                text_colour = WHITE;

                if (mud->mouse_x > x &&
                    mud->mouse_y >= y + item_grid_height + 59 &&
                    mud->mouse_x < x + shop_width &&
                    mud->mouse_y <= y + item_grid_height + 70) {
                    text_colour = RED;
                }

                surface_draw_string_centre(
                    mud->surface, formatted_sell, x + (shop_width / 2),
                    y + item_grid_height + 69, FONT_BOLD_12, text_colour);
            } else {
                surface_draw_string_right(
                    mud->surface, formatted_sell, x + shop_width - 3,
                    y + item_grid_height + 69, FONT_BOLD_12, YELLOW);

                text_colour = WHITE;

                if (mud->mouse_x > x + 2 &&
                    mud->mouse_y >= y + item_grid_height + 59 &&
                    mud->mouse_x < x + 112 &&
                    mud->mouse_y <= y + item_grid_height + 70) {
                    text_colour = RED;
                }

                surface_draw_string(mud->surface, "Click here to sell", x + 2,
                                    y + item_grid_height + 69, FONT_BOLD_13,
                                    text_colour);
            }

            return;
        }

        surface_draw_string_centre(
            mud->surface, "You do not have any of this item to sell",
            x + (shop_width / 2), y + item_grid_height + 69, FONT_BOLD_13,
            YELLOW);
    }
}
