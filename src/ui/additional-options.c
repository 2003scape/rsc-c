#include "./additional-options.h"

char *option_tabs[] = {"Connection", "Controls", "Display", "Bank"};

int mudclient_add_option_panel_label(Panel *panel, char *label, int x, int y) {
    panel_add_text(panel, x, y, label, 1, 0);

    return surface_text_width(label, 1) + 2;
}

int mudclient_add_option_panel_string(Panel *panel, char *label,
                                      char *default_text, int max_length, int x,
                                      int y) {
    int label_width = mudclient_add_option_panel_label(panel, label, x, y);

    int control = panel_add_text_input(panel, x + label_width, y,
                                       (ADDITIONAL_OPTIONS_WIDTH / 4), 15, 1,
                                       max_length, 0, 1);

    panel_update_text(panel, control, default_text);

    return control;
}

int mudclient_add_option_panel_checkbox(Panel *panel, char *label,
                                        int is_checked, int x, int y) {
    int label_width = mudclient_add_option_panel_label(panel, label, x, y);

    int control = panel_add_checkbox(panel, x + label_width, y - 7, 14, 14);
    panel_toggle_checkbox(panel, control, is_checked);

    return control;
}

void mudclient_create_options_panel(mudclient *mud) {
    int ui_x = mud->surface->width / 2 - ADDITIONAL_OPTIONS_WIDTH / 2;
    int ui_y = mud->surface->height / 2 - ADDITIONAL_OPTIONS_HEIGHT / 2;
    int x = ui_x + 4;
    int y = ui_y + 20 + 24 + 4;

    y += 20;

    /* connection */
    mud->panel_connection_options = malloc(sizeof(Panel));
    panel_new(mud->panel_connection_options, mud->surface, 10);

    int control = mudclient_add_option_panel_string(
        mud->panel_connection_options, "@whi@Server: ", mud->server, 15, x, y);

    mud->connection_options[control] = &mud->options->server;
    mud->connection_option_types[control] = 0;

    y += 20;

    char formatted_digits[12] = {0};
    sprintf(formatted_digits, "%d", mud->port);

    control = mudclient_add_option_panel_string(mud->panel_connection_options,
                                                "@whi@Port: ", formatted_digits,
                                                15, x, y);

    mud->connection_options[control] = &mud->options->port;
    mud->connection_option_types[control] = 1;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_connection_options, "@whi@Members: ", mud->members, x, y);

    mud->connection_options[control] = &mud->options->members;
    mud->connection_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_connection_options,
        "@whi@Idle logout: ", mud->options->idle_logout, x, y);

    mud->connection_options[control] = &mud->options->idle_logout;
    mud->connection_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_connection_options,
        "@whi@Remember username:", mud->options->remember_username, x, y);

    mud->connection_options[control] = &mud->options->remember_username;
    mud->connection_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_connection_options,
        "@whi@Remember password: ", mud->options->remember_password, x, y);

    mud->connection_options[control] = &mud->options->remember_password;
    mud->connection_option_types[control] = 2;

    /* controls */
    y = ui_y + 20 + 24 + 4;
    y += 30;

    mud->panel_control_options = malloc(sizeof(Panel));
    panel_new(mud->panel_control_options, mud->surface, 10);

    control = mudclient_add_option_panel_checkbox(
        mud->panel_control_options,
        "@whi@Mouse wheel: ", mud->options->mouse_wheel, x, y);

    mud->control_options[control] = &mud->options->mouse_wheel;
    mud->control_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_control_options,
        "@whi@Middle click camera: ", mud->options->middle_click_camera, x, y);

    mud->control_options[control] = &mud->options->middle_click_camera;
    mud->control_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_control_options,
        "@whi@Zoom camera: ", mud->options->zoom_camera, x, y);

    mud->control_options[control] = &mud->options->zoom_camera;
    mud->control_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_control_options,
        "@whi@Tab message respond: ", mud->options->tab_respond, x, y);

    mud->control_options[control] = &mud->options->tab_respond;
    mud->control_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_control_options,
        "@whi@Option number keys: ", mud->options->option_numbers, x, y);

    mud->control_options[control] = &mud->options->option_numbers;
    mud->control_option_types[control] = 2;

    /* display */
    y = ui_y + 20 + 24 + 4;

    mud->panel_display_options = malloc(sizeof(Panel));
    panel_new(mud->panel_display_options, mud->surface, 50);

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Interlace (F1): ", mud->options->interlace, x, y);

    mud->display_options[control] = &mud->options->interlace;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Display FPS: ", mud->options->display_fps, x, y);

    mud->display_options[control] = &mud->options->display_fps;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options, "@whi@UI Scale: ", mud->options->ui_scale,
        x, y);

    mud->display_options[control] = &mud->options->ui_scale;
    mud->display_option_types[control] = 2;

    y += 20;

    sprintf(formatted_digits, "%d", mud->options->field_of_view);

    control = mudclient_add_option_panel_string(
        mud->panel_display_options, "@whi@Field of view: ", formatted_digits, 3,
        x, y);

    mud->display_options[control] = &mud->options->field_of_view;
    mud->display_option_types[control] = 1;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Number commas: ", mud->options->number_commas, x, y);

    mud->display_options[control] = &mud->options->number_commas;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Show roofs: ", mud->options->show_roofs, x, y);

    mud->display_options[control] = &mud->options->show_roofs;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Compass menu: ", mud->options->compass_menu, x, y);

    mud->display_options[control] = &mud->options->compass_menu;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Trade menus: ", mud->options->trade_menus, x, y);

    mud->display_options[control] = &mud->options->trade_menus;
    mud->display_option_types[control] = 2;

    x += (ADDITIONAL_OPTIONS_WIDTH - 4) / 2;
    y = ui_y + 20 + 24 + 4;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Remaining XP: ", mud->options->remaining_experience, x, y);

    mud->display_options[control] = &mud->options->remaining_experience;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Total XP: ", mud->options->total_experience, x, y);

    mud->display_options[control] = &mud->options->total_experience;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Inventory count: ", mud->options->inventory_count, x, y);

    mud->display_options[control] = &mud->options->inventory_count;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@K/M amounts: ", mud->options->condense_item_amounts, x, y);

    mud->display_options[control] = &mud->options->condense_item_amounts;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Certificate items: ", mud->options->certificate_items, x, y);

    mud->display_options[control] = &mud->options->certificate_items;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options, "@whi@Offer-X: ", mud->options->offer_x, x,
        y);

    mud->display_options[control] = &mud->options->offer_x;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Last Offer-X: ", mud->options->last_offer_x, x, y);

    mud->display_options[control] = &mud->options->last_offer_x;
    mud->display_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_display_options,
        "@whi@Wilderness warning: ", mud->options->wilderness_warning, x, y);

    mud->display_options[control] = &mud->options->wilderness_warning;
    mud->display_option_types[control] = 2;

    /* bank */
    x = ui_x + 4;
    y = ui_y + 20 + 24 + 4;

    mud->panel_bank_options = malloc(sizeof(Panel));
    panel_new(mud->panel_bank_options, mud->surface, 50);

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Always multi-withdraw: ", mud->options->bank_unstackble_withdraw, x, y);

    mud->bank_options[control] = &mud->options->bank_unstackble_withdraw;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Search items: ", mud->options->bank_search, x, y);

    mud->bank_options[control] = &mud->options->bank_search;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Show capacity: ", mud->options->bank_capacity, x, y);

    mud->bank_options[control] = &mud->options->bank_capacity;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Show total value: ", mud->options->bank_value, x, y);

    mud->bank_options[control] = &mud->options->bank_value;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Deposit all: ", mud->options->bank_deposit_all, x, y);

    mud->bank_options[control] = &mud->options->bank_deposit_all;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Bank expand: ", mud->options->bank_expand, x, y);

    mud->bank_options[control] = &mud->options->bank_expand;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Bank scrollbar: ", mud->options->bank_scroll, x, y);

    mud->bank_options[control] = &mud->options->bank_scroll;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Bank menus: ", mud->options->bank_menus, x, y);

    mud->bank_options[control] = &mud->options->bank_menus;
    mud->bank_option_types[control] = 2;

    x += (ADDITIONAL_OPTIONS_WIDTH - 4) / 2;
    y = ui_y + 20 + 24 + 4;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Show inventory: ", mud->options->bank_inventory, x, y);

    mud->bank_options[control] = &mud->options->bank_inventory;
    mud->bank_option_types[control] = 2;

    y += 20;

    control = mudclient_add_option_panel_checkbox(
        mud->panel_bank_options,
        "@whi@Maintain slot: ", mud->options->bank_maintain_slot, x, y);

    mud->bank_options[control] = &mud->options->bank_maintain_slot;
    mud->bank_option_types[control] = 2;
}

Panel *mudclient_get_active_option_panel(mudclient *mud) {
    Panel *panel = NULL;

    if (mud->options_tab == 0) {
        panel = mud->panel_connection_options;
    } else if (mud->options_tab == 1) {
        panel = mud->panel_control_options;
    } else if (mud->options_tab == 2) {
        panel = mud->panel_display_options;
    } else if (mud->options_tab == 3) {
        panel = mud->panel_bank_options;
    }

    return panel;
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
                      OPTIONS_TAB_HEIGHT, option_tabs, 4, mud->options_tab);

    surface_draw_line_horizontal(mud->surface, ui_x + 8,
                                 ui_y + ADDITIONAL_OPTIONS_HEIGHT - 20,
                                 ADDITIONAL_OPTIONS_WIDTH - 8, BLACK);

    surface_draw_string(mud->surface, "Reset to: default / vanilla", ui_x + 4,
                        ui_y + ADDITIONAL_OPTIONS_HEIGHT - 5, 1, WHITE);

    surface_draw_string(mud->surface, "Save and close",
                        ui_x + ADDITIONAL_OPTIONS_WIDTH - 88,
                        ui_y + ADDITIONAL_OPTIONS_HEIGHT - 5, 1, WHITE);

    Panel *panel = mudclient_get_active_option_panel(mud);

    if (panel != NULL) {
        panel_draw_panel(panel);
    }

    mud->surface->draw_string_shadow = mud->logged_in ? 1 : 0;
}

void mudclient_handle_additional_options_input(mudclient *mud) {
    int ui_x = mud->surface->width / 2 - ADDITIONAL_OPTIONS_WIDTH / 2;
    int ui_y = mud->surface->height / 2 - ADDITIONAL_OPTIONS_HEIGHT / 2;

    if (mud->last_mouse_button_down == 1) {
        /* tabs */
        if (mud->mouse_x >= ui_x &&
            mud->mouse_x <= ui_x + ADDITIONAL_OPTIONS_WIDTH &&
            mud->mouse_y >= ui_y + 12 &&
            mud->mouse_y <= ui_y + 12 + OPTIONS_TAB_HEIGHT) {
            mud->options_tab =
                (mud->mouse_x - ui_x) / (ADDITIONAL_OPTIONS_WIDTH / 4);
        }
    }

    Panel *panel = mudclient_get_active_option_panel(mud);

    if (panel != NULL) {
        panel_handle_mouse(panel, mud->mouse_x, mud->mouse_y,
                           mud->last_mouse_button_down, mud->mouse_button_down,
                           mud->mouse_scroll_delta);
    }
}
