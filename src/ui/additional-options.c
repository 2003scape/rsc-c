#include "./additional-options.h"

char *option_tabs[] = {"Connection", "Controls", "Display"};

void mudclient_create_options_panel(mudclient *mud) {
    int x = mud->surface->width / 2 - ADDITIONAL_OPTIONS_WIDTH / 2;
    int y = mud->surface->height / 2 - ADDITIONAL_OPTIONS_HEIGHT / 2;

    x += 4;
    y += 20 + 24 + 4;

    mud->panel_connection_options = malloc(sizeof(Panel));
    panel_new(mud->panel_connection_options, mud->surface, 50);

    panel_add_text(mud->panel_connection_options, x, y, "@whi@Server:", 1, 0);

    mud->control_options_server = panel_add_text_input(
        mud->panel_connection_options, x + 46, y, 114, 15, 1, 15, 0, 1);

    panel_update_text(mud->panel_connection_options,
                      mud->control_options_server, mud->server);

    y += 20;

    panel_add_text(mud->panel_connection_options, x, y, "@whi@Port:", 1, 0);
    mud->control_options_port = panel_add_text_input(
        mud->panel_connection_options, x + 31, y, 36, 15, 1, 15, 0, 1);

    char formatted_port[12] = {0};
    sprintf(formatted_port, "%d", mud->port);

    panel_update_text(mud->panel_connection_options, mud->control_options_port,
                      formatted_port);

    y += 20;

    panel_add_text(mud->panel_connection_options, x, y, "@whi@Members:", 1, 0);
    panel_add_checkbox(mud->panel_connection_options, x + 62, y - 7, 14, 14);

    y += 20;

    //printf("%d\n", surface_text_width("Idle log out: ", 1));

    panel_add_text(mud->panel_connection_options, x, y, "@whi@Idle log out:", 1,
                   0);

    panel_add_checkbox(mud->panel_connection_options, x + 69, y - 7, 14, 14);

    y += 20;

    panel_add_text(mud->panel_connection_options, x, y, "@whi@Remember username:", 1,
                   0);

    panel_add_checkbox(mud->panel_connection_options, x + 132, y - 7, 14, 14);

    y += 20;

    panel_add_text(mud->panel_connection_options, x, y, "@whi@Remember password:", 1,
                   0);

    panel_add_checkbox(mud->panel_connection_options, x + 131, y - 7, 14, 14);
}

void mudclient_draw_additional_options(mudclient *mud) {
    mud->surface->draw_string_shadow = 1;

    int ui_x = mud->surface->width / 2 - ADDITIONAL_OPTIONS_WIDTH / 2;
    int ui_y = mud->surface->height / 2 - ADDITIONAL_OPTIONS_HEIGHT / 2;

    surface_draw_box(mud->surface, ui_x, ui_y, ADDITIONAL_OPTIONS_WIDTH, 12,
                     TITLE_BAR_COLOUR);

    surface_draw_string(mud->surface, "Additional options", ui_x + 1, ui_y + 10,
                        1, WHITE);

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + 12,
                           ADDITIONAL_OPTIONS_WIDTH,
                           ADDITIONAL_OPTIONS_HEIGHT - 12, GREY_98, 224);

    int text_colour = WHITE;

    if (mud->mouse_x > ui_x + ADDITIONAL_OPTIONS_WIDTH - 88 &&
        mud->mouse_y >= ui_y &&
        mud->mouse_x < ui_x + ADDITIONAL_OPTIONS_WIDTH &&
        mud->mouse_y < ui_y + 12) {
        text_colour = RED;
    }

    surface_draw_string_right(mud->surface, "Close window",
                              ui_x + ADDITIONAL_OPTIONS_WIDTH - 2, ui_y + 10, 1,
                              text_colour);

    surface_draw_tabs(mud->surface, ui_x, ui_y + 12, ADDITIONAL_OPTIONS_WIDTH,
                      OPTIONS_TAB_HEIGHT, option_tabs, 3, 0);

    surface_draw_line_horizontal(mud->surface, ui_x + 8,
                                 ui_y + ADDITIONAL_OPTIONS_HEIGHT - 20,
                                 ADDITIONAL_OPTIONS_WIDTH - 8, BLACK);

    surface_draw_string(mud->surface, "Reset to: default / vanilla", ui_x + 4,
                        ui_y + ADDITIONAL_OPTIONS_HEIGHT - 5, 1, WHITE);

    surface_draw_string(mud->surface, "Save and close",
                        ui_x + ADDITIONAL_OPTIONS_WIDTH - 88,
                        ui_y + ADDITIONAL_OPTIONS_HEIGHT - 5, 1, WHITE);

    panel_draw_panel(mud->panel_connection_options);

    mud->surface->draw_string_shadow = mud->logged_in ? 1 : 0;
}

void mudclient_handle_additional_options_input(mudclient *mud) {
    panel_handle_mouse(mud->panel_connection_options, mud->mouse_x,
                       mud->mouse_y, mud->last_mouse_button_down,
                       mud->mouse_button_down, mud->mouse_scroll_delta);
}
