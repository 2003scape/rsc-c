#include "shop.h"

void mudclient_draw_shop(mudclient *mud) {
    int dialog_x = 52;
    int dialog_y = 44;

    if (mud->mouse_button_click != 0) {
        mud->mouse_button_click = 0;

        int mouse_x = mud->mouse_x - dialog_x;
        int mouse_y = mud->mouse_y - dialog_y;

        if (mouse_x >= 0 && mouse_y >= 12 && mouse_x < 408 && mouse_y < 246) {
            int item_index = 0;

            for (int row = 0; row < 5; row++) {
                for (int col = 0; col < 8; col++) {
                    int slot_x = 7 + col * SHOP_SLOT_WIDTH;
                    int slot_y = 28 + row * SHOP_SLOT_HEIGHT;

                    if (mouse_x > slot_x &&
                        mouse_x < slot_x + SHOP_SLOT_WIDTH &&
                        mouse_y > slot_y &&
                        mouse_y < slot_y + SHOP_SLOT_HEIGHT &&
                        mud->shop_items[item_index] != -1) {
                        mud->shop_selected_item_index = item_index;
                        mud->shop_selected_item_type =
                            mud->shop_items[item_index];
                    }

                    item_index++;
                }
            }

            if (mud->shop_selected_item_index >= 0) {
                int item_id = mud->shop_items[mud->shop_selected_item_index];

                if (item_id != -1) {
                    if (mud->shop_items_count[mud->shop_selected_item_index] >
                            0 &&
                        mouse_x > 298 && mouse_y >= 204 && mouse_x < 408 &&
                        mouse_y <= 215) {
                        int price_mod = mud->shop_buy_price_mod +
                                        mud->shop_items_price
                                            [mud->shop_selected_item_index];

                        if (price_mod < 10) {
                            price_mod = 10;
                        }

                        int item_price =
                            (price_mod * game_data_item_base_price[item_id]) /
                            100;

                        packet_stream_new_packet(mud->packet_stream,
                                                 CLIENT_SHOP_BUY);

                        packet_stream_put_short(
                            mud->packet_stream,
                            mud->shop_items[mud->shop_selected_item_index]);

                        packet_stream_put_int(mud->packet_stream, item_price);
                        packet_stream_send_packet(mud->packet_stream);
                    }

                    if (mudclient_get_inventory_count(mud, item_id) > 0 &&
                        mouse_x > 2 && mouse_y >= 229 && mouse_x < 112 &&
                        mouse_y <= 240) {
                        int price_mod = mud->shop_sell_price_mod +
                                        mud->shop_items_price
                                            [mud->shop_selected_item_index];

                        if (price_mod < 10) {
                            price_mod = 10;
                        }

                        int item_price =
                            (price_mod * game_data_item_base_price[item_id]) /
                            100;

                        packet_stream_new_packet(mud->packet_stream,
                                                 CLIENT_SHOP_SELL);

                        packet_stream_put_short(
                            mud->packet_stream,
                            mud->shop_items[mud->shop_selected_item_index]);

                        packet_stream_put_int(mud->packet_stream, item_price);
                        packet_stream_send_packet(mud->packet_stream);
                    }
                }
            }
        } else {
            packet_stream_new_packet(mud->packet_stream, CLIENT_SHOP_CLOSE);
            packet_stream_send_packet(mud->packet_stream);
            mud->show_dialog_shop = 0;
            return;
        }
    }

    surface_draw_box(mud->surface, dialog_x, dialog_y, 408, 12,
                     TITLE_BAR_COLOUR);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 12, 408, 17,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 29, 8, 170,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x + 399, dialog_y + 29, 9, 170,
                           GREY_98, 160);

    surface_draw_box_alpha(mud->surface, dialog_x, dialog_y + 199, 408, 47,
                           GREY_98, 160);

    surface_draw_string(mud->surface, "Buying and selling items", dialog_x + 1,
                        dialog_y + 10, 1, WHITE);

    int text_colour = WHITE;

    if (mud->mouse_x > dialog_x + 320 && mud->mouse_y >= dialog_y &&
        mud->mouse_x < dialog_x + 408 && mud->mouse_y < dialog_y + 12) {
        text_colour = RED;
    }

    surface_draw_string_right(mud->surface, "Close window", dialog_x + 406,
                              dialog_y + 10, 1, text_colour);

    surface_draw_string(mud->surface, "Shops stock in green", dialog_x + 2,
                        dialog_y + 24, 1, GREEN);

    surface_draw_string(mud->surface, "Number you own in blue", dialog_x + 135,
                        dialog_y + 24, 1, CYAN);

    char formatted_money[26] = {0};

    sprintf(formatted_money, "Your money: %dgp",
            mudclient_get_inventory_count(mud, COINS_ID));

    surface_draw_string(mud->surface, formatted_money, dialog_x + 280,
                        dialog_y + 24, 1, YELLOW);

    int item_index = 0;

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 8; col++) {
            int slot_x = dialog_x + 7 + col * SHOP_SLOT_WIDTH;
            int slot_y = dialog_y + 28 + row * SHOP_SLOT_HEIGHT;

            int slot_colour =
                mud->shop_selected_item_index == item_index ? RED : GREY_D0;

            surface_draw_box_alpha(mud->surface, slot_x, slot_y,
                                   SHOP_SLOT_WIDTH, SHOP_SLOT_HEIGHT,
                                   slot_colour, 160);

            surface_draw_box_edge(mud->surface, slot_x, slot_y, 50, 35, BLACK);

            int item_id = mud->shop_items[item_index];

            if (mud->shop_items[item_index] != -1) {
                surface_sprite_clipping_from9(
                    mud->surface, slot_x, slot_y, SHOP_SLOT_WIDTH - 1,
                    SHOP_SLOT_HEIGHT - 2,
                    mud->sprite_item + game_data_item_picture[item_id],
                    game_data_item_mask[item_id], 0, 0, 0);

                char formatted_amount[12] = {0};

                sprintf(formatted_amount, "%d",
                        mud->shop_items_count[item_index]);

                surface_draw_string(mud->surface, formatted_amount, slot_x + 1,
                                    slot_y + 10, 1, GREEN);

                sprintf(formatted_amount, "%d",
                        mudclient_get_inventory_count(mud, item_id));

                surface_draw_string_right(mud->surface, formatted_amount,
                                          slot_x + 47, slot_y + 10, 1, CYAN);
            }

            item_index++;
        }
    }

    surface_draw_line_horizontal(mud->surface, dialog_x + 5, dialog_y + 222,
                                 398, 0);

    if (mud->shop_selected_item_index == -1) {
        surface_draw_string_centre(mud->surface,
                                   "Select an object to buy or sell",
                                   dialog_x + 204, dialog_y + 214, 3, YELLOW);

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
                (price_mod * game_data_item_base_price[selected_item_id]) / 100;

            char *item_name = game_data_item_name[selected_item_id];

            char formatted_buy[strlen(item_name) + 30];

            sprintf(formatted_buy, "Buy a new %s for %dgp", item_name,
                    item_price);

            surface_draw_string(mud->surface, formatted_buy, dialog_x + 2,
                                dialog_y + 214, 1, YELLOW);

            text_colour = WHITE;

            if (mud->mouse_x > dialog_x + 298 &&
                mud->mouse_y >= dialog_y + 204 &&
                mud->mouse_x < dialog_x + 408 &&
                mud->mouse_y <= dialog_y + 215) {
                text_colour = RED;
            }

            surface_draw_string_right(mud->surface, "Click here to buy",
                                      dialog_x + 405, dialog_y + 214, 3,
                                      text_colour);
        } else {
            surface_draw_string_centre(
                mud->surface, "This item is not currently available to buy",
                dialog_x + 204, dialog_y + 214, 3, YELLOW);
        }

        if (mudclient_get_inventory_count(mud, selected_item_id) > 0) {
            int price_mod =
                mud->shop_sell_price_mod +
                mud->shop_items_price[mud->shop_selected_item_index];

            if (price_mod < 10) {
                price_mod = 10;
            }

            int item_price =
                (price_mod * game_data_item_base_price[selected_item_id]) / 100;

            char *item_name = game_data_item_name[selected_item_id];

            char formatted_sell[strlen(item_name) + 29];

            sprintf(formatted_sell, "Sell your %s for %dgp", item_name,
                    item_price);

            surface_draw_string_right(mud->surface, formatted_sell,
                                      dialog_x + 405, dialog_y + 239, 1,
                                      YELLOW);

            text_colour = WHITE;

            if (mud->mouse_x > dialog_x + 2 && mud->mouse_y >= dialog_y + 229 &&
                mud->mouse_x < dialog_x + 112 &&
                mud->mouse_y <= dialog_y + 240) {
                text_colour = RED;
            }

            surface_draw_string(mud->surface, "Click here to sell",
                                dialog_x + 2, dialog_y + 239, 3, text_colour);

            return;
        }

        surface_draw_string_centre(mud->surface,
                                   "You do not have any of this item to sell",
                                   dialog_x + 204, dialog_y + 239, 3, YELLOW);
    }
}
