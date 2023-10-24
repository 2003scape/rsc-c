#include "option-menu.h"

void mudclient_draw_option_menu(mudclient *mud) {
    int font_size = 1;
    int font_height = 12;
    int ui_x = 6;
    int ui_y = 0;

    if (mud->options->option_numbers) {
        int index = -1;

        if (mud->key_1) {
            index = 0;
        } else if (mud->key_2) {
            index = 1;
        } else if (mud->key_3) {
            index = 2;
        } else if (mud->key_4) {
            index = 3;
        } else if (mud->key_5) {
            index = 4;
        }

        if (index != -1) {
            packet_stream_new_packet(mud->packet_stream, CLIENT_CHOOSE_OPTION);
            packet_stream_put_byte(mud->packet_stream, index);
            packet_stream_send_packet(mud->packet_stream);

            mud->show_option_menu = 0;

            mud->key_1 = 0;
            mud->key_2 = 0;
            mud->key_3 = 0;
            mud->key_4 = 0;
            mud->key_4 = 0;

            return;
        }
    }

    if (mud->mouse_button_click != 0) {
        for (int i = 0; i < mud->option_menu_count; i++) {
#ifdef _3DS
            char entry[strlen(mud->option_menu_entry[i]) + 5];

            sprintf(entry, "(%c) %s", _3ds_option_buttons[i],
                    mud->option_menu_entry[i]);
#else
            char *entry = mud->option_menu_entry[i];
#endif

            if (mud->mouse_x < ui_x - 6 ||
                mud->mouse_x >=
                    ui_x - 6 + surface_text_width(entry, font_size) ||
                mud->mouse_y <= ui_y + i * font_height ||
                mud->mouse_y >= ui_y + (font_height + i * font_height)) {
                continue;
            }

            packet_stream_new_packet(mud->packet_stream, CLIENT_CHOOSE_OPTION);
            packet_stream_put_byte(mud->packet_stream, i);
            packet_stream_send_packet(mud->packet_stream);
            break;
        }

        mud->mouse_button_click = 0;
        mud->show_option_menu = 0;
        return;
    }

    for (int i = 0; i < mud->option_menu_count; i++) {
        int text_colour = CYAN;

#ifdef _3DS
            char entry[strlen(mud->option_menu_entry[i]) + 5];

            sprintf(entry, "(%c) %s", _3ds_option_buttons[i],
                    mud->option_menu_entry[i]);
#else
            char *entry = mud->option_menu_entry[i];
#endif

        if (mud->mouse_x > ui_x - 6 &&
            mud->mouse_x < ui_x - 6 + surface_text_width(entry, font_size) &&
            mud->mouse_y > ui_y + i * font_height &&
            mud->mouse_y < ui_y + (i * font_height + font_height)) {
            text_colour = RED;
        }

        surface_draw_string(mud->surface, entry, ui_x,
                            ui_y + (font_height + i * font_height), font_size,
                            text_colour);
    }
}
