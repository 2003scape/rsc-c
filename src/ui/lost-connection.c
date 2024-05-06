#include "lost-connection.h"

void mudclient_draw_lost_connection(mudclient *mud) {
    int box_x = mud->surface->width / 2 - (LOST_CONNECTION_WIDTH / 2);
    int box_y = (mud->surface->height / 2) - (LOST_CONNECTION_HEIGHT / 2) - 1;

    surface_draw_box(mud->surface, box_x, box_y, LOST_CONNECTION_WIDTH,
                     LOST_CONNECTION_HEIGHT, BLACK);

    surface_draw_border(mud->surface, box_x, box_y, LOST_CONNECTION_WIDTH,
                        LOST_CONNECTION_HEIGHT, WHITE);

    surface_draw_string_centre(mud->surface, "Connection lost! Please wait...",
                               mud->surface->width / 2,
                               mud->surface->height / 2 - 5, FONT_BOLD_14,
                               WHITE);

    surface_draw_string_centre(
        mud->surface, "Attempting to re-establish", mud->surface->width / 2,
        mud->surface->height / 2 + 13, FONT_BOLD_14, WHITE);
}
