#include "confirm.h"

void mudclient_draw_confirm(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - CONFIRM_DIALOG_WIDTH / 2;
    int dialog_y = mud->surface->height / 2 - CONFIRM_DIALOG_HEIGHT / 2;

    surface_draw_box(mud->surface, dialog_x, dialog_y, CONFIRM_DIALOG_WIDTH,
                     CONFIRM_DIALOG_HEIGHT, BLACK);

    surface_draw_box_edge(mud->surface, dialog_x, dialog_y,
                          CONFIRM_DIALOG_WIDTH, CONFIRM_DIALOG_HEIGHT, WHITE);

    int x = mud->surface->width / 2;
    int y = dialog_y + 20;

    surface_draw_string_centre(mud->surface, mud->confirm_text_top, x, y, 4,
                               WHITE);

    y += 20;

    surface_draw_string_centre(mud->surface, mud->confirm_text_bottom, x, y, 4,
                               WHITE);

    int cancel_x = dialog_x + (CONFIRM_DIALOG_WIDTH / 2 - CONFIRM_BUTTON_SIZE);
    int ok_x = cancel_x + (CONFIRM_BUTTON_SIZE * 2);
    int button_y = dialog_y + CONFIRM_DIALOG_HEIGHT - 15;

    int text_colour = WHITE;

    if (mud->mouse_x >= cancel_x - (CONFIRM_BUTTON_SIZE / 2) &&
        mud->mouse_x <= cancel_x + (CONFIRM_BUTTON_SIZE / 2) &&
        mud->mouse_y >= button_y - (CONFIRM_BUTTON_SIZE / 2) &&
        mud->mouse_y <= button_y + (CONFIRM_BUTTON_SIZE / 2)) {
        text_colour = YELLOW;
    }

    surface_draw_string_centre(mud->surface, "Cancel", cancel_x, button_y, 1,
                               text_colour);

    text_colour = WHITE;

    if (mud->mouse_x >= ok_x - (CONFIRM_BUTTON_SIZE / 2) &&
        mud->mouse_x <= ok_x + (CONFIRM_BUTTON_SIZE / 2) &&
        mud->mouse_y >= button_y - (CONFIRM_BUTTON_SIZE / 2) &&
        mud->mouse_y <= button_y + (CONFIRM_BUTTON_SIZE / 2)) {
        text_colour = YELLOW;
    }

    surface_draw_string_centre(mud->surface, "OK", ok_x, button_y, 1,
                               text_colour);
}
