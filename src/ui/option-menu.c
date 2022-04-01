#include "option-menu.h"

void mudclient_draw_option_menu(mudclient *mud) {
    int font_size = 1;
    int font_height = 12;
    int ui_x = 6;
    int ui_y = 0;

    if (mud->mouse_button_click != 0) {
        for (int i = 0; i < mud->option_menu_count; i++) {
            if (mud->mouse_x < ui_x - 6 ||
                mud->mouse_x >= ui_x - 6 +
                                    surface_text_width(
                                        mud->option_menu_entry[i], font_size) ||
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

        if (mud->mouse_x > ui_x - 6 &&
            mud->mouse_x <
                ui_x - 6 +
                    surface_text_width(mud->option_menu_entry[i], font_size) &&
            mud->mouse_y > ui_y + i * font_height &&
            mud->mouse_y < ui_y + (i * font_height + font_height)) {
            text_colour = RED;
        }

        surface_draw_string(mud->surface, mud->option_menu_entry[i], ui_x,
                            ui_y + (font_height + i * font_height), font_size,
                            text_colour);
    }
}
