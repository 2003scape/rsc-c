#include "offer-x.h"

void mudclient_draw_offer_x(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - OFFER_X_WIDTH / 2;
    int dialog_y = mud->surface->height / 2 - OFFER_X_HEIGHT / 2;

    int cancel_offset_x = mud->surface->width / 2 - OFFER_X_CANCEL_SIZE / 2;
    int cancel_offset_y = mud->surface->height / 2 + OFFER_X_CANCEL_SIZE / 2;

    surface_draw_box(mud->surface, dialog_x, dialog_y, OFFER_X_WIDTH,
                     OFFER_X_HEIGHT, BLACK);

    surface_draw_box_edge(mud->surface, dialog_x, dialog_y, OFFER_X_WIDTH,
                          OFFER_X_HEIGHT, BLACK);

    int x = mud->surface->width / 2;
    int y = dialog_y + 20;

    surface_draw_string_centre(mud->surface, "Enter an amount", x, y, 4, WHITE);

    y += 20;

    char formatted_current[INPUT_DIGITS_LENGTH + 2] = {0};
    sprintf(formatted_current, "%s*", mud->input_digits_current);

    surface_draw_string_centre(mud->surface, formatted_current, x, y, 4, WHITE);

    int text_colour = WHITE;

    if (mud->mouse_x > cancel_offset_x &&
        mud->mouse_x < cancel_offset_x + OFFER_X_CANCEL_SIZE &&
        mud->mouse_y > cancel_offset_y &&
        mud->mouse_y < cancel_offset_y + OFFER_X_CANCEL_SIZE) {
        text_colour = YELLOW;
    }

    surface_draw_string_centre(mud->surface, "Cancel", x, y + 23, 1,
                               text_colour);
}
