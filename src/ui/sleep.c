#include "sleep.h"

void mudclient_draw_sleep(mudclient *mud) {
    surface_fade_to_black(mud->surface);

    if (((float)rand() / (float)RAND_MAX) <= 0.15) {
        surface_draw_string_centre(
            mud->surface, "ZZZ", ((float)rand() / (float)RAND_MAX) * 80,
            ((float)rand() / (float)RAND_MAX) * mud->surface->height, 5,
            ((float)rand() / (float)RAND_MAX) * WHITE);
    }

    if (((float)rand() / (float)RAND_MAX) <= 0.15) {
        surface_draw_string_centre(
            mud->surface, "ZZZ",
            mud->surface->width - (((float)rand() / (float)RAND_MAX) * 80),
            ((float)rand() / (float)RAND_MAX) * mud->surface->height, 5,
            ((float)rand() / (float)RAND_MAX) * WHITE);
    }

    surface_draw_box(mud->surface, (mud->surface->width / 2) - 100, 160, 200, 40,
                     BLACK);

    int x = mud->surface->width / 2;
    int y = 50;

    surface_draw_string_centre(mud->surface, "You are sleeping", x, y, 7,
                               YELLOW);

    char formatted_fatigue[22] = {0};

    sprintf(formatted_fatigue, "Fatigue: %d%%",
            (mud->fatigue_sleeping * 100) / 750);

    y += 40;

    surface_draw_string_centre(mud->surface, formatted_fatigue, x, y, 7,
                               YELLOW);

    y += 50;

    surface_draw_string_centre(
        mud->surface, "When you want to wake up just use your", x, y, 5, WHITE);

    y += 20;

    surface_draw_string_centre(mud->surface,
                               "keyboard to type the word in the box below", x,
                               y, 5, WHITE);

    char formatted_input[strlen(mud->input_text_current) + 2];
    sprintf(formatted_input, "%s*", mud->input_text_current);

    y += 20;

    surface_draw_string_centre(mud->surface, formatted_input, x, y, 5, CYAN);

    if (mud->sleeping_status_text == NULL) {
        surface_draw_sprite_from3(mud->surface, x - 127, 230,
                                  mud->sprite_texture + 1);
    } else {
        surface_draw_string_centre(mud->surface, mud->sleeping_status_text, x,
                                   260, 5, RED);
    }

    y += 49;

    surface_draw_box_edge(mud->surface, x - 128, y, 257, 42, WHITE);

    mudclient_draw_chat_message_tabs(mud);

    y += 61;

    surface_draw_string_centre(mud->surface, "If you can't read the word", x, y,
                               1, WHITE);

    y += 15;

    surface_draw_string_centre(mud->surface,
                               "@yel@click here@whi@ to get a different one", x,
                               y, 1, WHITE);

    surface_draw(mud->surface);
}

void mudclient_handle_sleep_input(mudclient *mud) {
    if (strlen(mud->input_text_final) > 0) {
        if (strncasecmp(mud->input_text_final, "::lostcon", 9) == 0) {
            packet_stream_close(mud->packet_stream);
        } else if (strncasecmp(mud->input_text_final, "::closecon", 10) == 0) {
            mudclient_close_connection(mud);
        } else {
            packet_stream_new_packet(mud->packet_stream, CLIENT_SLEEP_WORD);
            packet_stream_put_string(mud->packet_stream, mud->input_text_final);

#ifndef REVISION_177
            /* TODO confirm this isn't in 177 */
            if (!mud->sleep_word_delay) {
                packet_stream_put_byte(mud->packet_stream, 0);
                mud->sleep_word_delay = 1;
            }
#endif

            packet_stream_send_packet(mud->packet_stream);

            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

            mud->sleeping_status_text = "Please wait...";
        }
    }

    if (mud->last_mouse_button_down == 1 && mud->mouse_y > 275 &&
        mud->mouse_y < 310 && mud->mouse_x > 56 && mud->mouse_x < 456) {
        packet_stream_new_packet(mud->packet_stream, CLIENT_SLEEP_WORD);
        packet_stream_put_string(mud->packet_stream, "-null-");

#ifndef REVISION_177
        if (!mud->sleep_word_delay) {
            packet_stream_put_byte(mud->packet_stream, 0);
            mud->sleep_word_delay = 1;
        }
#endif

        packet_stream_send_packet(mud->packet_stream);
        mud->input_text_current[0] = '\0';
        mud->input_text_final[0] = '\0';
        mud->sleeping_status_text = "Please wait...";
    }

    mud->last_mouse_button_down = 0;
}
