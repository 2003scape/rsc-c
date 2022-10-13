#include "server-message.h"

void mudclient_draw_server_message(mudclient *mud) {
    int dialog_x = mud->surface->width / 2;
    int dialog_y = (mud->surface->height / 2) - 6;
    int height = mud->server_message_box_top ? 300 : 100;

    if (MUD_IS_COMPACT && mud->server_message_box_top) {
        dialog_y -= 2;
        height = MUD_HEIGHT - 16;
    }

    surface_draw_box(mud->surface, dialog_x - (SERVER_MESSAGE_WIDTH / 2),
                     dialog_y - (height / 2), SERVER_MESSAGE_WIDTH, height,
                     BLACK);

    surface_draw_border(mud->surface, dialog_x - (SERVER_MESSAGE_WIDTH / 2),
                          dialog_y - (height / 2), SERVER_MESSAGE_WIDTH, height,
                          WHITE);

    surface_draw_paragraph(mud->surface, mud->server_message, dialog_x,
                           dialog_y - (height / 2) + 20, 1, WHITE,
                           SERVER_MESSAGE_WIDTH - 40);

    int offset_y = (dialog_y - 10) + (height / 2);
    int text_colour = WHITE;

    if (mud->mouse_y > offset_y - 12 && mud->mouse_y <= offset_y &&
        mud->mouse_x > dialog_x - 150 && mud->mouse_x < dialog_x + 150) {
        text_colour = RED;
    }

    surface_draw_string_centre(mud->surface, "Click here to close window",
                               dialog_x, offset_y, 1, text_colour);

    if (mud->mouse_button_click == 1 &&
        (text_colour == RED ||
         ((mud->mouse_x < dialog_x - (SERVER_MESSAGE_WIDTH / 2) ||
           mud->mouse_x > dialog_x + (SERVER_MESSAGE_WIDTH / 2)) &&
          (mud->mouse_y < dialog_y - (height / 2) ||
           mud->mouse_y > dialog_y + (height / 2))))) {
        mud->show_dialog_server_message = 0;
    }

    mud->mouse_button_click = 0;
}
