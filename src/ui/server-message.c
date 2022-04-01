#include "server-message.h"

void mudclient_draw_server_message(mudclient *mud) {
    int height = 100;

    if (mud->server_message_box_top) {
        height = 300;
    }

    surface_draw_box(mud->surface, 256 - (SERVER_MESSAGE_WIDTH / 2),
                     167 - (height / 2), SERVER_MESSAGE_WIDTH, height, BLACK);

    surface_draw_box_edge(mud->surface, 256 - (SERVER_MESSAGE_WIDTH / 2),
                          167 - (height / 2), SERVER_MESSAGE_WIDTH, height,
                          WHITE);

    surface_draw_paragraph(mud->surface, mud->server_message, 256,
                           167 - (height / 2) + 20, 1, WHITE,
                           SERVER_MESSAGE_WIDTH - 40);

    int offset_y = 157 + (height / 2);
    int text_colour = WHITE;

    if (mud->mouse_y > offset_y - 12 && mud->mouse_y <= offset_y &&
        mud->mouse_x > 106 && mud->mouse_x < 406) {
        text_colour = RED;
    }

    surface_draw_string_centre(mud->surface, "Click here to close window", 256,
                               offset_y, 1, text_colour);

    if (mud->mouse_button_click == 1) {
        if (text_colour == RED) {
            mud->show_dialog_server_message = 0;
        }

        if ((mud->mouse_x < 256 - (SERVER_MESSAGE_WIDTH / 2) ||
             mud->mouse_x > 256 + (SERVER_MESSAGE_WIDTH / 2)) &&
            (mud->mouse_y < 167 - (height / 2) ||
             mud->mouse_y > 167 + (height / 2))) {
            mud->show_dialog_server_message = 0;
        }
    }

    mud->mouse_button_click = 0;
}
