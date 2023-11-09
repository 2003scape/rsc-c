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

    if (MUD_IS_COMPACT) {
        int character_at = surface_paragraph_height(
            mud->surface, mud->server_message, FONT_BOLD_12,
            SERVER_MESSAGE_WIDTH - 40, 196);

        if (character_at > -1) {
            mud->server_message[character_at - 1] = '\0';

            strcpy(mud->server_message_next,
                   mud->server_message + character_at + 1);
        }
    }

    char *draw_message = mud->server_message_page == 0
                             ? mud->server_message
                             : mud->server_message_next;

    surface_draw_paragraph(mud->surface, draw_message, dialog_x,
                           dialog_y - (height / 2) + 20, 1, WHITE,
                           SERVER_MESSAGE_WIDTH - 40);

    int offset_y = (dialog_y - 10) + (height / 2);
    int text_colour = WHITE;

    if (MUD_IS_COMPACT) {
        if (mud->server_message_page > 0) {
            int previous_x = dialog_x - (SERVER_MESSAGE_WIDTH / 2) + 8;
            if (mud->mouse_x >= previous_x &&
                mud->mouse_x <=
                    previous_x + surface_text_width("Previous", FONT_BOLD_12) &&
                mud->mouse_y > offset_y - 12 && mud->mouse_y <= offset_y) {
                text_colour = RED;

                if (mud->mouse_button_click == 1) {
                    mud->server_message_page = 0;
                    mud->mouse_button_click = 0;
                }
            }

            surface_draw_string(mud->surface, "Previous", previous_x, offset_y,
                                1, text_colour);

            text_colour = WHITE;
        }

        if (mud->server_message_page == 0 &&
            strlen(mud->server_message_next) > 0) {
            int next_x = dialog_x + (SERVER_MESSAGE_WIDTH / 2) -
                         surface_text_width("Next", FONT_BOLD_12) - 8;
            if (mud->mouse_x >= next_x &&
                mud->mouse_x <=
                    next_x + surface_text_width("Next", FONT_BOLD_12) &&
                mud->mouse_y > offset_y - 12 && mud->mouse_y <= offset_y) {
                text_colour = RED;

                if (mud->mouse_button_click == 1) {
                    mud->server_message_page = 1;
                    mud->mouse_button_click = 0;
                }
            }

            surface_draw_string(mud->surface, "Next", next_x, offset_y,
                                FONT_BOLD_12, text_colour);

            text_colour = WHITE;
        }
    }

    int close_width = MUD_IS_COMPACT ? 100 : 150;

    if (mud->mouse_y > offset_y - 12 && mud->mouse_y <= offset_y &&
        mud->mouse_x > dialog_x - close_width &&
        mud->mouse_x < dialog_x + close_width) {
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
        mud->server_message_page = 0;
        mud->show_dialog_server_message = 0;
    }

    mud->mouse_button_click = 0;
}
