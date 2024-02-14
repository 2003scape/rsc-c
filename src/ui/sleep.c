#include "sleep.h"

void mudclient_draw_sleep(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    surface_fade_to_black(mud->surface);

    int zzz_width = 80 + (mud->surface->width / 2 - MUD_VANILLA_WIDTH / 2);

    /* make proportionally more ZZZ's as the area increases */
    float zzz_frequency = 0.15f *
                          (float)(mud->surface->width * mud->surface->height) /
                          (float)(MUD_VANILLA_WIDTH * MUD_VANILLA_HEIGHT);

    zzz_frequency = fmin(zzz_frequency, 1.0);

    if (zzz_frequency > 1.0f) {
        zzz_frequency = 1.0f;
    }

    if (((float)rand() / (float)RAND_MAX) <= zzz_frequency) {
        surface_draw_string_centre(
            mud->surface, "ZZZ", ((float)rand() / (float)RAND_MAX) * zzz_width,
            ((float)rand() / (float)RAND_MAX) * mud->surface->height,
            FONT_BOLD_16, ((float)rand() / (float)RAND_MAX) * WHITE);
    }

    if (((float)rand() / (float)RAND_MAX) <= zzz_frequency) {
        surface_draw_string_centre(
            mud->surface, "ZZZ",
            mud->surface->width -
                (((float)rand() / (float)RAND_MAX) * zzz_width),
            ((float)rand() / (float)RAND_MAX) * mud->surface->height,
            FONT_BOLD_16, ((float)rand() / (float)RAND_MAX) * WHITE);
    }

    int is_touch = mudclient_is_touch(mud); // TODO

    int x = mud->surface->width / 2;

    int y = (mud->surface->height / 2) -
            ((is_compact ? MUD_MIN_HEIGHT : MUD_VANILLA_HEIGHT) / 2);

    if (is_compact) {
        y += 28;
    } else {
        y += 50;
    }

    if (is_touch) {
        y += 15;
    }

    /* draw a box so our input text doesn't ghost */
    int keyboard_x = x - 100;
    int keyboard_y = y + 74 + (is_compact ? 0 : 36);
    int keyboard_width = 200;
    int keyboard_height = (is_compact ? 36 : 40);

    surface_draw_box(mud->surface, keyboard_x, keyboard_y, keyboard_width,
                     keyboard_height, BLACK);

    surface_draw_string_centre(mud->surface, "You are sleeping", x, y,
                               FONT_BOLD_24, YELLOW);

    y += (is_compact ? 28 : 40);

    surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_24, YELLOW,
                                "Fatigue: %d%%",
                                (mud->fatigue_sleeping * 100) / 750);

    y += (is_compact ? 25 : 50);

    char *line_top = NULL;

    if (is_compact) {
        line_top = is_touch ? "To wake up just tap below"
                            : "To wake up just use your keyboard";
    } else {
        line_top = is_touch ? "When you want to wake up just tap below"
                            : "When you want to wake up just use your";
    }

    surface_draw_string_centre(mud->surface, line_top, x, y, FONT_BOLD_16,
                               WHITE);

    y += 20;

    char *line_bottom = NULL;

    if (is_compact) {
        line_bottom = is_touch ? "and type the word in the box"
                               : "to type the word in the box below";
    } else {
        line_bottom = is_touch ? "and type the word in the box"
                               : "keyboard to type the word in the box below";
    }

    surface_draw_string_centre(mud->surface, line_bottom, x, y, FONT_BOLD_16,
                               WHITE);

    y += 20;

    surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_16, CYAN, "%s*",
                                mud->input_text_current);

    y += (is_compact ? 16 : 49);

    if (mud->sleeping_status_text == NULL) {
        surface_draw_sprite(mud->surface, x - 127, y + 1,
                            mud->sprite_texture + 1);
    } else {
        surface_draw_string_centre(mud->surface, mud->sleeping_status_text, x,
                                   y + 31, FONT_BOLD_16, RED);
    }

    surface_draw_border(mud->surface, x - 128, y, 257, 42, WHITE);

    mudclient_draw_chat_message_tabs(mud);

    y += 61;

    surface_draw_string_centre(mud->surface, "If you can't read the word", x, y,
                               FONT_BOLD_12, WHITE);

    y += 15;

    surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_12, WHITE,
                                "@yel@%s here@whi@ to get a different one",
                                is_touch ? "tap" : "click");

    surface_draw(mud->surface);
}

void mudclient_handle_sleep_input(mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int x = mud->surface->width / 2;

    int y = (mud->surface->height / 2) -
            ((is_compact ? MUD_MIN_HEIGHT : MUD_VANILLA_HEIGHT) / 2) +
            (is_compact ? 28 : 50);

    if (mudclient_is_touch(mud)) {
        y += 15;
    }

    if (strlen(mud->input_text_final) > 0) {
        if (strncasecmp(mud->input_text_final, "::lostcon", 9) == 0) {
            packet_stream_close(mud->packet_stream);
        } else if (strncasecmp(mud->input_text_final, "::closecon", 10) == 0) {
            mudclient_close_connection(mud);
        } else {
            packet_stream_new_packet(mud->packet_stream, CLIENT_SLEEP_WORD);
            packet_stream_put_string(mud->packet_stream, mud->input_text_final);

#ifndef REVISION_177
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

    int keyboard_x = x - 100 - 3;
    int keyboard_y = y + 74 + (is_compact ? 0 : 36);
    int keyboard_width = 200;
    int keyboard_height = 28;

    if (mud->last_mouse_button_down == 1 && mud->mouse_x > keyboard_x &&
        mud->mouse_x < keyboard_x + keyboard_width &&
        mud->mouse_y > keyboard_y &&
        mud->mouse_y < keyboard_y + keyboard_height) {
        mudclient_trigger_keyboard(mud, mud->input_text_current, 0, keyboard_x,
                                   keyboard_y, keyboard_width, keyboard_height,
                                   FONT_BOLD_16, 1);
    }

    /* new sleep word */
    if (mud->last_mouse_button_down == 1 &&
        mud->mouse_y > y + (is_compact ? 157 : 225) &&
        mud->mouse_y < y + (is_compact ? 192 : 260) && mud->mouse_x > x - 200 &&
        mud->mouse_x < x + 200) {
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
