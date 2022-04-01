#include "logout.h"

void mudclient_draw_logout(mudclient *mud) {
    surface_draw_box(mud->surface, 126, 137, LOGOUT_WIDTH, LOGOUT_HEIGHT,
                     BLACK);

    surface_draw_box_edge(mud->surface, 126, 137, LOGOUT_WIDTH, LOGOUT_HEIGHT,
                          WHITE);

    surface_draw_string_centre(mud->surface, "Logging out...", 256, 173, 5,
                               WHITE);
}
