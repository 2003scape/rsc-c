#include "wilderness-warning.h"

void mudclient_draw_wilderness_warning(mudclient *mud) {
    int y = 97;

    surface_draw_box(mud->surface, 86, 77, 340, 180, BLACK);
    surface_draw_box_edge(mud->surface, 86, 77, 340, 180, WHITE);

    surface_draw_string_centre(mud->surface, "Warning! Proceed with caution",
                               256, y, 4, RED);

    y += 26;

    surface_draw_string_centre(
        mud->surface, "If you go much further north you will enter the", 256, y,
        1, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface,
                               "wilderness. This a very dangerous area where",
                               256, y, 1, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface, "other players can attack you!",
                               256, y, 1, WHITE);

    y += 22;

    surface_draw_string_centre(mud->surface,
                               "The further north you go the more dangerous it",
                               256, y, 1, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface,
                               "becomes, but the more treasure you will find.",
                               256, y, 1, WHITE);

    y += 22;

    surface_draw_string_centre(
        mud->surface, "In the wilderness an indicator at the bottom-right", 256,
        y, 1, WHITE);

    y += 13;

    surface_draw_string_centre(
        mud->surface, "of the screen will show the current level of danger",
        256, y, 1, WHITE);

    y += 22;

    int text_colour = WHITE;

    if (mud->mouse_y > y - 12 && mud->mouse_y <= y && mud->mouse_x > 181 &&
        mud->mouse_x < 331) {
        text_colour = RED;
    }

    surface_draw_string_centre(mud->surface, "Click here to close window", 256,
                               y, 1, text_colour);

    if (mud->mouse_button_click != 0) {
        if (mud->mouse_y > y - 12 && mud->mouse_y <= y && mud->mouse_x > 181 &&
            mud->mouse_x < 331) {
            mud->show_ui_wild_warn = 2;
        }

        if (mud->mouse_x < 86 || mud->mouse_x > 426 || mud->mouse_y < 77 ||
            mud->mouse_y > 257) {
            mud->show_ui_wild_warn = 2;
        }

        mud->mouse_button_click = 0;
    }
}
