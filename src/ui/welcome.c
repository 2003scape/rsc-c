#include "welcome.h"

/* TODO recovery */

void mudclient_draw_welcome(mudclient *mud) {
    int is_compact = mud->surface->width < 400;
    int width = (is_compact ? MUD_MIN_WIDTH : 400);
    int height = WELCOME_HEIGHT;

    if (mud->welcome_last_ip != 0) {
        height += 15 * 3;
    }

    int dialog_x = (mud->surface->width / 2) - (width / 2);
    int dialog_y = (mud->surface->height / 2) - (height / 2);

    surface_draw_box(mud->surface, dialog_x, dialog_y, width, height, BLACK);
    surface_draw_border(mud->surface, dialog_x, dialog_y, width, height, WHITE);

    int y = dialog_y + 20;
    int x = mud->surface->width / 2;

    surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_14, YELLOW,
                                "Welcome to RuneScape %s", mud->login_username);

    y += 30;

    char days_ago[21] = {0};

    if (mud->welcome_days_ago <= 0) {
        strcpy(days_ago, "earlier today");
    } else if (mud->welcome_days_ago == 1) {
        strcpy(days_ago, "yesterday");
    } else {
        sprintf(days_ago, "%d days ago", mud->welcome_days_ago);
    }

    if (mud->welcome_last_ip != 0) {
        surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_12, WHITE,
                                    "You last logged in %s", days_ago);

        y += 15;

        if (mud->welcome_last_ip_string == NULL) {
            mud->welcome_last_ip_string = calloc(45, sizeof(char));
            ip_to_string(mud->welcome_last_ip, mud->welcome_last_ip_string);
        }

        surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_12, WHITE,
                                    "from: %s", mud->welcome_last_ip_string);

        y += 15 * 2;
    }

    int text_colour = WHITE;

    if (mud->mouse_y > y - 12 && mud->mouse_y <= y &&
        mud->mouse_x > dialog_x + 50 && mud->mouse_x < dialog_x + width - 50) {
        text_colour = RED;
    }

    surface_draw_stringf_centre(mud->surface, x, y, FONT_BOLD_12, text_colour,
                                "%s here to close window",
                                mudclient_is_touch(mud) ? "Tap" : "Click");

    if (mud->mouse_button_click == 1) {
        if (text_colour == RED) {
            mud->show_dialog_welcome = 0;
        }

        if ((mud->mouse_x < dialog_x + 30 ||
             mud->mouse_x > dialog_x + width - 30) &&
            (mud->mouse_y < dialog_y - (height / 2) ||
             mud->mouse_y > dialog_y + (height / 2))) {
            mud->show_dialog_welcome = 0;
        }
    }

    mud->mouse_button_click = 0;
}
