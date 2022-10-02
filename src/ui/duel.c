#include "duel.h"

void mudclient_draw_duel(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - TRANSACTION_WIDTH / 2; // 22

    int dialog_y = (mud->surface->height / 2 - TRANSACTION_HEIGHT / 2) +
                   (MUD_IS_COMPACT ? 0 : 2); // 36

    int offset_x = MUD_IS_COMPACT ? 2 : 8;

    int offset_y = MUD_IS_COMPACT
                       ? (DUEL_OFFER_ROWS * TRADE_SLOT_HEIGHT) + 40
                       : (DUEL_OFFER_ROWS * TRADE_SLOT_HEIGHT * 2) + 79; // 215

    if (mud->mouse_button_click != 0) {
        int mouse_x = mud->mouse_x - dialog_x;
        int mouse_y = mud->mouse_y - dialog_y;

        if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < TRANSACTION_WIDTH &&
            mouse_y < TRANSACTION_HEIGHT - 16) {
            int send_update = 0;

            if (mouse_x >= 93 && mouse_y >= 221 && mouse_x <= 104 &&
                mouse_y <= 232) {
                mud->duel_option_retreat = !mud->duel_option_retreat;
                send_update = 1;
            }

            if (mouse_x >= 93 && mouse_y >= 240 && mouse_x <= 104 &&
                mouse_y <= 251) {
                mud->duel_option_magic = !mud->duel_option_magic;
                send_update = 1;
            }

            if (mouse_x >= 191 && mouse_y >= 221 && mouse_x <= 202 &&
                mouse_y <= 232) {
                mud->duel_option_prayer = !mud->duel_option_prayer;
                send_update = 1;
            }

            if (mouse_x >= 191 && mouse_y >= 240 && mouse_x <= 202 &&
                mouse_y <= 251) {
                mud->duel_option_weapons = !mud->duel_option_weapons;
                send_update = 1;
            }

            if (send_update) {
                packet_stream_new_packet(mud->packet_stream,
                                         CLIENT_DUEL_OPTIONS);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_retreat);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_magic);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_prayer);

                packet_stream_put_byte(mud->packet_stream,
                                       mud->duel_option_weapons);

                packet_stream_send_packet(mud->packet_stream);

                mud->transaction_recipient_accepted = 0;
                mud->transaction_accepted = 0;
            }
        }
    }

    mudclient_draw_transaction(mud, dialog_x, dialog_y, 0);

    if (!mud->show_dialog_duel) {
        return;
    }

    int options_width = (TRANSACTION_OFFER_COLUMNS * TRADE_SLOT_WIDTH) + 1;

    surface_draw_box_alpha(mud->surface, dialog_x + offset_x,
                           dialog_y + offset_y, options_width, 43, GREY_D0,
                           160);

    surface_draw_border(mud->surface, dialog_x + offset_x, dialog_y + offset_y,
                        options_width, 43, BLACK);

    if (!MUD_IS_COMPACT) {
        surface_draw_string(mud->surface, "Duel Options",
                            dialog_x + offset_x + 1, dialog_y + offset_y - 3, 4,
                            WHITE);
    }

    surface_draw_string(
        mud->surface, MUD_IS_COMPACT ? "Retreat" : "No retreating",
        dialog_x + offset_x + 1, dialog_y + offset_y + 16, 3, YELLOW);

    int left_option_width = MUD_IS_COMPACT ? 48 : 84;

    surface_draw_border(mud->surface,
                        dialog_x + offset_x + left_option_width + 1,
                        dialog_y + offset_y + 6, 11, 11, YELLOW);

    if (mud->duel_option_retreat) {
        surface_draw_box(mud->surface,
                         dialog_x + offset_x + left_option_width + 3,
                         dialog_y + offset_y + 8, 7, 7, YELLOW);
    }

    surface_draw_string(mud->surface, MUD_IS_COMPACT ? "Magic" : "No magic",
                        dialog_x + offset_x + 1, dialog_y + offset_y + 35, 3,
                        YELLOW);

    surface_draw_border(mud->surface,
                        dialog_x + offset_x + left_option_width + 1,
                        dialog_y + offset_y + 25, 11, 11, YELLOW);

    if (mud->duel_option_magic) {
        surface_draw_box(mud->surface,
                         dialog_x + offset_x + left_option_width + 3,
                         dialog_y + offset_y + 27, 7, 7, YELLOW);
    }

    int right_option_width = MUD_IS_COMPACT ? 44 : 81;

    int padding_left =
        offset_x + left_option_width + (MUD_IS_COMPACT ? 13 : 18); // 110

    surface_draw_string(mud->surface, MUD_IS_COMPACT ? "Prayer" : "No prayer",
                        dialog_x + padding_left, dialog_y + offset_y + 16, 3,
                        YELLOW);

    surface_draw_border(mud->surface,
                        dialog_x + padding_left + right_option_width,
                        dialog_y + offset_y + 6, 11, 11, YELLOW);

    if (mud->duel_option_prayer) {
        surface_draw_box(mud->surface,
                         dialog_x + padding_left + right_option_width + 2,
                         dialog_y + offset_y + 8, 7, 7, YELLOW);
    }

    surface_draw_string(mud->surface, MUD_IS_COMPACT ? "Equip" : "No weapons",
                        dialog_x + padding_left, dialog_y + offset_y + 35, 3,
                        YELLOW);

    surface_draw_border(mud->surface,
                        dialog_x + padding_left + right_option_width,
                        dialog_y + offset_y + 25, 11, 11, YELLOW);

    if (mud->duel_option_weapons) {
        surface_draw_box(mud->surface,
                         dialog_x + padding_left + right_option_width + 2,
                         dialog_y + offset_y + 27, 7, 7, YELLOW);
    }
}

void mudclient_draw_duel_confirm(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - TRANSACTION_WIDTH / 2; // 22

    int dialog_y =
        (mud->surface->height / 2 - TRANSACTION_HEIGHT / 2) + 2; // 36

    mudclient_draw_transaction_confirm(mud, dialog_x, dialog_y, 0);

    int x = dialog_x + 234;
    int y = dialog_y + 180;

    if (mud->duel_option_retreat == 0) {
        surface_draw_string_centre(
            mud->surface, "You can retreat from this duel", x, y, 1, GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "No retreat is possible!", x,
                                   y, 1, RED);
    }

    y += 12;

    if (mud->duel_option_magic == 0) {
        surface_draw_string_centre(mud->surface, "Magic may be used", x, y, 1,
                                   GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Magic cannot be used", x, y,
                                   1, RED);
    }

    y += 12;

    if (mud->duel_option_prayer == 0) {
        surface_draw_string_centre(mud->surface, "Prayer may be used", x, y, 1,
                                   GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Prayer cannot be used", x, y,
                                   1, RED);
    }

    y += 12;

    if (mud->duel_option_weapons == 0) {
        surface_draw_string_centre(mud->surface, "Weapons may be used", x, y, 1,
                                   GREEN);
    } else {
        surface_draw_string_centre(mud->surface, "Weapons cannot be used", x, y,
                                   1, RED);
    }
}
