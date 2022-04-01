#include "welcome.h"

/* TODO recovery */

void mudclient_draw_welcome(mudclient *mud) {
    int height = 65;

    if (mud->welcome_last_ip != 0) {
        height += 15 * 3;
    }

    int y = 167 - (height / 2);

    surface_draw_box(mud->surface, 56, 167 - (height / 2), WELCOME_WIDTH,
                     height, 0);

    surface_draw_box_edge(mud->surface, 56, 167 - (height / 2), WELCOME_WIDTH,
                          height, WHITE);

    y += 20;

    char formatted_welcome[21 + USERNAME_LENGTH + 1] = {0};
    sprintf(formatted_welcome, "Welcome to RuneScape %s", mud->login_user);

    surface_draw_string_centre(mud->surface, formatted_welcome, 256, y, 4,
                               YELLOW);

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
        char formatted_last_login[51] = {0};

        sprintf(formatted_last_login, "You last logged in %s", days_ago);

        surface_draw_string_centre(mud->surface, formatted_last_login, 256, y,
                                   1, WHITE);

        y += 15;

        if (mud->welcome_last_ip_string == NULL) {
            mud->welcome_last_ip_string = calloc(45, sizeof(char));
            ip_to_string(mud->welcome_last_ip, mud->welcome_last_ip_string);
        }

        sprintf(formatted_last_login, "from: %s", mud->welcome_last_ip_string);

        surface_draw_string_centre(mud->surface, formatted_last_login, 256, y,
                                   1, WHITE);

        y += 15 * 2;
    }

    int text_colour = WHITE;

    if (mud->mouse_y > y - 12 && mud->mouse_y <= y && mud->mouse_x > 106 &&
        mud->mouse_x < 406) {
        text_colour = RED;
    }

    surface_draw_string_centre(mud->surface, "Click here to close window", 256,
                               y, 1, text_colour);

    if (mud->mouse_button_click == 1) {
        if (text_colour == RED) {
            mud->show_dialog_welcome = 0;
        }

        if ((mud->mouse_x < 86 || mud->mouse_x > 426) &&
            (mud->mouse_y < 167 - ((height / 2) | 0) ||
             mud->mouse_y > 167 + ((height / 2) | 0))) {
            mud->show_dialog_welcome = 0;
        }
    }

    mud->mouse_button_click = 0;
}
