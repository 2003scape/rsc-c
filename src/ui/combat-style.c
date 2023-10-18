#include "combat-style.h"

const char *combat_styles[] = {
    "Controlled (+1 of each)", "Aggressive (+3 strength)",
    "Accurate (+3 attack)", "Defensive (+3 defense)"
};

const char *combat_styles_compact[] = {
    "Controlled (+1 each)", "Aggressive (+3 str.)",
    "Accurate (+3 att.)", "Defensive (+3 def.)"
};

void mudclient_draw_combat_style(mudclient *mud) {
    int ui_x = 7;
    int ui_y = 15;

    int combat_styles_length = sizeof(combat_styles) / sizeof(combat_styles[0]);

    if (mud->mouse_button_click != 0 &&
        (MUD_IS_COMPACT ? mud->show_ui_tab != INVENTORY_TAB : 1)) {
        for (int i = 0; i < combat_styles_length + 1; i++) {
            if (i <= 0 || mud->mouse_x <= ui_x ||
                mud->mouse_x >= ui_x + COMBAT_STYLE_WIDTH ||
                mud->mouse_y < ui_y + i * COMBAT_BUTTON_HEIGHT ||
                mud->mouse_y >
                    ui_y + i * COMBAT_BUTTON_HEIGHT + COMBAT_BUTTON_HEIGHT) {
                continue;
            }

            mud->combat_style = i - 1;
            mud->mouse_button_click = 0;

            packet_stream_new_packet(mud->packet_stream, CLIENT_COMBAT_STYLE);
            packet_stream_put_byte(mud->packet_stream, mud->combat_style);
            packet_stream_send_packet(mud->packet_stream);
            break;
        }
    }

    for (int i = 0; i < combat_styles_length + 1; i++) {
        int box_colour = i == mud->combat_style + 1 ? RED : GREY_BE;

        surface_draw_box_alpha(
            mud->surface, ui_x, ui_y + i * COMBAT_BUTTON_HEIGHT,
            COMBAT_STYLE_WIDTH, COMBAT_BUTTON_HEIGHT, box_colour, 128);

        surface_draw_line_horizontal(mud->surface, ui_x,
                                     ui_y + i * COMBAT_BUTTON_HEIGHT,
                                     COMBAT_STYLE_WIDTH, BLACK);

        surface_draw_line_horizontal(mud->surface, ui_x,
                                     ui_y + i * COMBAT_BUTTON_HEIGHT +
                                         COMBAT_BUTTON_HEIGHT,
                                     COMBAT_STYLE_WIDTH, BLACK);
    }

    int y = 16;

    if (MUD_IS_COMPACT) {
        surface_draw_string_centre(mud->surface, "Combat style",
           ui_x + (COMBAT_STYLE_WIDTH / 2), ui_y + y, FONT_REGULAR_11, WHITE);
    } else {
        surface_draw_string_centre(mud->surface, "Select combat style",
           ui_x + (COMBAT_STYLE_WIDTH / 2), ui_y + y, FONT_BOLD_13, WHITE);
    }

    y += COMBAT_BUTTON_HEIGHT;

    for (int i = 0; i < combat_styles_length; i++) {
        if (MUD_IS_COMPACT) {
            surface_draw_string_centre(mud->surface, combat_styles_compact[i],
                                       ui_x + (COMBAT_STYLE_WIDTH / 2),
                                       ui_y + y - 1, FONT_REGULAR_11, BLACK);
        } else {
            surface_draw_string_centre(mud->surface, combat_styles[i],
                                       ui_x + (COMBAT_STYLE_WIDTH / 2),
                                       ui_y + y, FONT_BOLD_13, BLACK);
        }

        y += COMBAT_BUTTON_HEIGHT;
    }
}
