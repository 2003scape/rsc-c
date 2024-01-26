#include "duel.h"

static void mudclient_draw_duel_option(mudclient *mud, int x, int y, int width,
                                       int option);

static void mudclient_draw_duel_option(mudclient *mud, int x, int y, int width,
                                       int option) {
    surface_draw_border(mud->surface, x + width, y, DUEL_CHECKBOX_WIDTH,
                        DUEL_CHECKBOX_HEIGHT, YELLOW);

    if (option) {
        surface_draw_box(mud->surface, x + width + 2, y + 2,
                         DUEL_CHECKBOX_WIDTH - 4, DUEL_CHECKBOX_HEIGHT - 4,
                         YELLOW);
    }
}

void mudclient_draw_duel(mudclient *mud) {
    int is_compact = TRANSACTION_IS_COMPACT(mud);

    int columns = is_compact ? 3 : 4;

    int slot_width = is_compact ? 39 : ITEM_GRID_SLOT_WIDTH;
    int slot_height = is_compact ? 27 : ITEM_GRID_SLOT_HEIGHT;

    int inventory_width = (TRANSACTION_INVENTORY_COLUMNS * slot_width);

    int transaction_width =
        inventory_width + (columns * slot_width) + (is_compact ? 8 : 27);

    int transaction_height =
        ((TRANSACTION_INVENTORY_ROWS * slot_height) + (is_compact ? 43 : 74));

    int offer_rows = (is_compact ? 3 : 2);

    int dialog_x = mud->surface->width / 2 - transaction_width / 2; // 22

    int dialog_y = (mud->surface->height / 2 - transaction_height / 2) +
                   (is_compact ? 0 : 2); // 36

    int offset_x = is_compact ? 2 : 8;

    int offset_y = is_compact ? (offer_rows * slot_height) + 40
                              : (offer_rows * slot_height * 2) + 79; // 215

    int left_option_width = (is_compact ? 49 : 85);
    int right_option_width = (is_compact ? 44 : 81);

    int padding_left =
        offset_x + left_option_width + (is_compact ? 12 : 17); // 110

    if (mud->mouse_button_click != 0) {
        int mouse_x = mud->mouse_x - dialog_x;
        int mouse_y = mud->mouse_y - dialog_y;

        int send_update = 0;

        if (mouse_x >= offset_x + left_option_width &&
            mouse_x <= offset_x + left_option_width + DUEL_CHECKBOX_WIDTH &&
            mouse_y >= offset_y + 6 &&
            mouse_y <= offset_y + 6 + DUEL_CHECKBOX_HEIGHT) {
            mud->duel_option_retreat = !mud->duel_option_retreat;
            send_update = 1;
        }

        if (mouse_x >= offset_x + left_option_width &&
            mouse_x <= offset_x + left_option_width + DUEL_CHECKBOX_WIDTH &&
            mouse_y >= offset_y + 25 &&
            mouse_y <= offset_y + 25 + DUEL_CHECKBOX_HEIGHT) {
            mud->duel_option_magic = !mud->duel_option_magic;
            send_update = 1;
        }

        if (mouse_x >= padding_left + right_option_width &&
            mouse_x <=
                padding_left + right_option_width + DUEL_CHECKBOX_WIDTH &&
            mouse_y >= offset_y + 6 &&
            mouse_y <= offset_y + 6 + DUEL_CHECKBOX_HEIGHT) {
            mud->duel_option_prayer = !mud->duel_option_prayer;
            send_update = 1;
        }

        if (mouse_x >= padding_left + right_option_width &&
            mouse_x <=
                padding_left + right_option_width + DUEL_CHECKBOX_WIDTH &&
            mouse_y >= offset_y + 25 &&
            mouse_y <= offset_y + 25 + DUEL_CHECKBOX_HEIGHT) {
            mud->duel_option_weapons = !mud->duel_option_weapons;
            send_update = 1;
        }

        if (send_update) {
            packet_stream_new_packet(mud->packet_stream, CLIENT_DUEL_OPTIONS);

            packet_stream_put_byte(mud->packet_stream,
                                   mud->duel_option_retreat);

            packet_stream_put_byte(mud->packet_stream, mud->duel_option_magic);
            packet_stream_put_byte(mud->packet_stream, mud->duel_option_prayer);

            packet_stream_put_byte(mud->packet_stream,
                                   mud->duel_option_weapons);

            packet_stream_send_packet(mud->packet_stream);

            mud->transaction_recipient_accepted = 0;
            mud->transaction_accepted = 0;
        }
    }

    mudclient_draw_transaction(mud, dialog_x, dialog_y, 0);

    if (!mud->show_dialog_duel) {
        return;
    }

    int options_width = ((columns * slot_width) + 1);
    int options_height = ((DUEL_CHECKBOX_HEIGHT * 2) + 22);

    surface_draw_box_alpha(mud->surface, dialog_x + offset_x,
                           dialog_y + offset_y, options_width, options_height,
                           GREY_D0, 160);

    surface_draw_border(mud->surface, dialog_x + offset_x, dialog_y + offset_y,
                        options_width, options_height, BLACK);

    if (!is_compact) {
        surface_draw_string(mud->surface, "Duel Options",
                            dialog_x + offset_x + 1, dialog_y + offset_y - 3,
                            FONT_BOLD_14, WHITE);
    }

    surface_draw_string(mud->surface, is_compact ? "Retreat" : "No retreating",
                        dialog_x + offset_x + 1, dialog_y + offset_y + 16,
                        FONT_BOLD_13, YELLOW);

    mudclient_draw_duel_option(mud, dialog_x + offset_x,
                               dialog_y + offset_y + 6, left_option_width,
                               mud->duel_option_retreat);

    surface_draw_string(mud->surface, is_compact ? "Magic" : "No magic",
                        dialog_x + offset_x + 1, dialog_y + offset_y + 35,
                        FONT_BOLD_13, YELLOW);

    mudclient_draw_duel_option(mud, dialog_x + offset_x,
                               dialog_y + offset_y + 25, left_option_width,
                               mud->duel_option_magic);

    surface_draw_string(mud->surface, is_compact ? "Prayer" : "No prayer",
                        dialog_x + padding_left, dialog_y + offset_y + 16,
                        FONT_BOLD_13, YELLOW);

    mudclient_draw_duel_option(mud, dialog_x + padding_left,
                               dialog_y + offset_y + 6, right_option_width,
                               mud->duel_option_prayer);

    surface_draw_string(mud->surface, is_compact ? "Equip" : "No weapons",
                        dialog_x + padding_left, dialog_y + offset_y + 35,
                        FONT_BOLD_13, YELLOW);

    mudclient_draw_duel_option(mud, dialog_x + padding_left,
                               dialog_y + offset_y + 25, right_option_width,
                               mud->duel_option_weapons);
}

void mudclient_draw_duel_confirm(mudclient *mud) {
    int is_compact = TRANSACTION_IS_COMPACT(mud);

    int columns = is_compact ? 3 : 4;

    int slot_width = is_compact ? 39 : ITEM_GRID_SLOT_WIDTH;
    int slot_height = is_compact ? 27 : ITEM_GRID_SLOT_HEIGHT;

    int inventory_width = (TRANSACTION_INVENTORY_COLUMNS * slot_width);

    int transaction_width =
        inventory_width + (columns * slot_width) + (is_compact ? 8 : 27);

    int transaction_height =
        ((TRANSACTION_INVENTORY_ROWS * slot_height) + (is_compact ? 43 : 74));

    int dialog_x = mud->surface->width / 2 - transaction_width / 2; // 22

    int dialog_y = (mud->surface->height / 2 - transaction_height / 2) +
                   (is_compact ? -8 : 2); // 36

    mudclient_draw_transaction_confirm(mud, dialog_x, dialog_y, 0);

    if (is_compact && mud->transaction_tab != 2) {
        return;
    }

    int x = dialog_x + (transaction_width / 2);
    int y = dialog_y + (is_compact ? 52 : 180);

    if (mud->duel_option_retreat == 0) {
        surface_draw_string_centre(mud->surface,
                                   "You can retreat from this duel", x, y,
                                   FONT_BOLD_12, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "No retreat is possible!", x,
                                   y, FONT_BOLD_12, RED);
    }

    y += 12;

    if (mud->duel_option_magic == 0) {
        surface_draw_string_centre(mud->surface, "Magic may be used", x, y,
                                   FONT_BOLD_12, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Magic cannot be used", x, y,
                                   FONT_BOLD_12, RED);
    }

    y += 12;

    if (mud->duel_option_prayer == 0) {
        surface_draw_string_centre(mud->surface, "Prayer may be used", x, y,
                                   FONT_BOLD_12, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Prayer cannot be used", x, y,
                                   FONT_BOLD_12, RED);
    }

    y += 12;

    if (mud->duel_option_weapons == 0) {
        surface_draw_string_centre(mud->surface, "Weapons may be used", x, y,
                                   FONT_BOLD_12, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Weapons cannot be used", x, y,
                                   FONT_BOLD_12, RED);
    }
}
