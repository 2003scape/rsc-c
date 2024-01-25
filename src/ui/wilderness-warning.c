#include "wilderness-warning.h"

void mudclient_draw_wilderness_warning(mudclient *mud) {
    int wilderness_warning_width =
        (mud->surface->width < 340 ? MUD_WIDTH : 340);

    int dialog_x = mud->surface->width / 2 - wilderness_warning_width / 2;
    int dialog_y = mud->surface->height / 2 - WILDERNESS_WARNING_HEIGHT / 2 - 6;

    surface_draw_box(mud->surface, dialog_x, dialog_y, wilderness_warning_width,
                     WILDERNESS_WARNING_HEIGHT, BLACK);

    surface_draw_border(mud->surface, dialog_x, dialog_y,
                        wilderness_warning_width, WILDERNESS_WARNING_HEIGHT,
                        WHITE);

    int x = mud->surface->width / 2;
    int y = dialog_y + 20;

    surface_draw_string_centre(mud->surface, "Warning! Proceed with caution", x,
                               y, FONT_BOLD_14, RED);

    y += 26;

    surface_draw_string_centre(
        mud->surface, "If you go much further north you will enter the", x, y,
        FONT_BOLD_12, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface,
                               "wilderness. This a very dangerous area where",
                               x, y, FONT_BOLD_12, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface, "other players can attack you!", x,
                               y, FONT_BOLD_12, WHITE);

    y += 22;

    surface_draw_string_centre(mud->surface,
                               "The further north you go the more dangerous it",
                               x, y, FONT_BOLD_12, WHITE);

    y += 13;

    surface_draw_string_centre(mud->surface,
                               "becomes, but the more treasure you will find.",
                               x, y, FONT_BOLD_12, WHITE);

    y += 22;

    surface_draw_string_centre(
        mud->surface, "In the wilderness an indicator at the bottom-right", x,
        y, FONT_BOLD_12, WHITE);

    y += 13;

    surface_draw_string_centre(
        mud->surface, "of the screen will show the current level of danger", x,
        y, FONT_BOLD_12, WHITE);

    y += 22;

    int text_colour = WHITE;

    if (mud->mouse_y > y - 12 && mud->mouse_y <= y && mud->mouse_x > x - 75 &&
        mud->mouse_x < x + 75) {
        text_colour = RED;
    }

    surface_draw_string_centre(mud->surface, "Click here to close window", x, y,
                               FONT_BOLD_12, text_colour);

    if (mud->mouse_button_click != 0) {
        if (mud->mouse_y > y - 12 && mud->mouse_y <= y &&
            mud->mouse_x > x - 75 && mud->mouse_x < x + 75) {
            mud->show_wilderness_warning = 2;
        }

        if (mud->mouse_x < dialog_x ||
            mud->mouse_x > dialog_x + wilderness_warning_width ||
            mud->mouse_y < dialog_y ||
            mud->mouse_y > dialog_y + WILDERNESS_WARNING_HEIGHT) {
            mud->show_wilderness_warning = 2;
        }

        mud->mouse_button_click = 0;
    }
}
