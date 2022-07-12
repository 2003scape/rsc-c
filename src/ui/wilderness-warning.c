#include "wilderness-warning.h"

void mudclient_draw_wilderness_warning(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - WILDERNESS_WARNING_WIDTH / 2;
    int dialog_y = mud->surface->height / 2 - WILDERNESS_WARNING_HEIGHT / 2 - 6;

    surface_draw_box(mud->surface, dialog_x, dialog_y, WILDERNESS_WARNING_WIDTH,
                     WILDERNESS_WARNING_HEIGHT, BLACK);

    surface_draw_box_edge(mud->surface, dialog_x, dialog_y,
                          WILDERNESS_WARNING_WIDTH, WILDERNESS_WARNING_HEIGHT,
                          WHITE);

    int x = mud->surface->width / 2;
    int y = dialog_y + 20;

    surface_draw_string_centre(mud->surface, "Warning! Proceed with caution", x,
                               y, 4, RED);

    y += 26;

    surface_draw_string_centre(
        mud->surface, "If you go much further north you will enter the", x, y,
        1, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface,
                               "wilderness. This a very dangerous area where",
                               x, y, 1, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface, "other players can attack you!", x,
                               y, 1, WHITE);

    y += 22;

    surface_draw_string_centre(mud->surface,
                               "The further north you go the more dangerous it",
                               x, y, 1, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface,
                               "becomes, but the more treasure you will find.",
                               x, y, 1, WHITE);

    y += 22;

    surface_draw_string_centre(
        mud->surface, "In the wilderness an indicator at the bottom-right", x,
        y, 1, WHITE);

    y += 13;

    surface_draw_string_centre(
        mud->surface, "of the screen will show the current level of danger", x,
        y, 1, WHITE);

    y += 22;

    int text_colour = WHITE;

    if (mud->mouse_y > y - 12 && mud->mouse_y <= y && mud->mouse_x > x - 75 &&
        mud->mouse_x < x + 75) {
        text_colour = RED;
    }

    surface_draw_string_centre(mud->surface, "Click here to close window", x, y,
                               1, text_colour);

    if (mud->mouse_button_click != 0) {
        if (mud->mouse_y > y - 12 && mud->mouse_y <= y &&
            mud->mouse_x > x - 75 && mud->mouse_x < x + 75) {
            mud->show_ui_wild_warn = 2;
        }

        if (mud->mouse_x < dialog_x ||
            mud->mouse_x > dialog_x + WILDERNESS_WARNING_WIDTH ||
            mud->mouse_y < dialog_y ||
            mud->mouse_y > dialog_y + WILDERNESS_WARNING_HEIGHT) {
            mud->show_ui_wild_warn = 2;
        }

        mud->mouse_button_click = 0;
    }
}
