#include "logout.h"

void mudclient_draw_logout(mudclient *mud) {
    int x = mud->surface->width / 2 - LOGOUT_WIDTH / 2;
    int y = (mud->surface->height - 12) / 2 - LOGOUT_HEIGHT / 2;

    surface_draw_box(mud->surface, x, y, LOGOUT_WIDTH, LOGOUT_HEIGHT, BLACK);
    surface_draw_border(mud->surface, x, y, LOGOUT_WIDTH, LOGOUT_HEIGHT, WHITE);

    surface_draw_string_centre(mud->surface, "Logging out...",
                               mud->surface->width / 2,
                               mud->surface->height / 2, 5, WHITE);
}
