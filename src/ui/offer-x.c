#include "offer-x.h"

void mudclient_draw_offer_x(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - OFFER_X_WIDTH / 2;
    int dialog_y = mud->surface->height / 2 - OFFER_X_HEIGHT / 2;

    int cancel_offset_x = mud->surface->width / 2 - OFFER_X_CANCEL_SIZE / 2;
    int cancel_offset_y = mud->surface->height / 2 + OFFER_X_CANCEL_SIZE / 2;

    surface_draw_box(mud->surface, dialog_x, dialog_y, OFFER_X_WIDTH,
                     OFFER_X_HEIGHT, BLACK);

    surface_draw_border(mud->surface, dialog_x, dialog_y, OFFER_X_WIDTH,
                        OFFER_X_HEIGHT, WHITE);

    int x = mud->surface->width / 2;
    int y = dialog_y + 20;

    surface_draw_string_centre(mud->surface, "Enter an amount", x, y,
                               FONT_BOLD_14, WHITE);

    if (mudclient_is_touch(mud) && mud->mouse_button_click != 0 &&
        mud->mouse_x > dialog_x && mud->mouse_x < dialog_x + OFFER_X_WIDTH &&
        mud->mouse_y >= y && mud->mouse_y <= y + 30) {
        mudclient_trigger_keyboard(mud, mud->input_digits_current, 0, dialog_x,
                                   y + 1, OFFER_X_WIDTH - 3, 30, FONT_BOLD_14,
                                   1);
    }

    y += 20;

    char formatted_current[INPUT_DIGITS_LENGTH + 2] = {0};
    sprintf(formatted_current, "%s*", mud->input_digits_current);

    surface_draw_string_centre(mud->surface, formatted_current, x, y,
                               FONT_BOLD_14, WHITE);

    int text_colour = WHITE;

    if (mud->mouse_x > cancel_offset_x &&
        mud->mouse_x < cancel_offset_x + OFFER_X_CANCEL_SIZE &&
        mud->mouse_y > cancel_offset_y &&
        mud->mouse_y < cancel_offset_y + OFFER_X_CANCEL_SIZE) {
        text_colour = YELLOW;
    }

    surface_draw_string_centre(mud->surface, "Cancel", x, y + 23, 1,
                               text_colour);
}

void mudclient_handle_offer_x_input(mudclient *mud) {
    int cancel_offset_x = mud->surface->width / 2 - OFFER_X_CANCEL_SIZE / 2;
    int cancel_offset_y = mud->surface->height / 2 + OFFER_X_CANCEL_SIZE / 2;

    if (mud->mouse_button_click == 1 && mud->mouse_x > cancel_offset_x &&
        mud->mouse_x < cancel_offset_x + OFFER_X_CANCEL_SIZE &&
        mud->mouse_y > cancel_offset_y &&
        mud->mouse_y < cancel_offset_y + OFFER_X_CANCEL_SIZE) {
        memset(mud->input_digits_current, '\0', INPUT_DIGITS_LENGTH + 1);
        mud->show_dialog_offer_x = 0;
        mud->mouse_button_click = 0;
    }
}

void mudclient_add_offer_menu(mudclient *mud, int type, int item_id, int amount,
                              char *display_amount, char *item_name) {
    strcpy(mud->menu_items[mud->menu_items_count].action_text, display_amount);
    strcpy(mud->menu_items[mud->menu_items_count].target_text, item_name);
    mud->menu_items[mud->menu_items_count].type = type;
    mud->menu_items[mud->menu_items_count].index = item_id;
    mud->menu_items[mud->menu_items_count].target_index = amount;
    mud->menu_items_count++;
}

void mudclient_add_offer_menus(mudclient *mud, char *type_string, int type,
                               int item_id, int item_amount, char *item_name,
                               int last_x) {
    char formatted[strlen(type_string) + 12];
    sprintf(formatted, "%s-%d", type_string, 1);

    if (item_amount >= 1) {
        mudclient_add_offer_menu(mud, type, item_id, 1, formatted, item_name);
    }

    if (item_amount >= 5) {
        sprintf(formatted, "%s-%d", type_string, 5);
        mudclient_add_offer_menu(mud, type, item_id, 5, formatted, item_name);
    }

    if (item_amount >= 10) {
        sprintf(formatted, "%s-%d", type_string, 10);
        mudclient_add_offer_menu(mud, type, item_id, 10, formatted, item_name);
    }

    if (item_amount >= 50) {
        sprintf(formatted, "%s-%d", type_string, 50);
        mudclient_add_offer_menu(mud, type, item_id, 50, formatted, item_name);
    }

    if (mud->options->offer_x && mud->options->last_offer_x) {
        /* don't add the last-x if it's already one of the other menu options */
        if (last_x != 0 && last_x != 1 && last_x != 5 && last_x != 10 &&
            last_x != 50 && item_amount >= last_x) {
            sprintf(formatted, "%s-%d", type_string, last_x);

            mudclient_add_offer_menu(mud, type, item_id, last_x, formatted,
                                     item_name);
        }
    }

    if (mud->options->offer_x && item_amount > 1) {
        sprintf(formatted, "%s-X", type_string);

        mudclient_add_offer_menu(mud, type, item_id, -item_amount, formatted,
                                 item_name);
    }

    if (mud->options->offer_x && item_amount >= 1) {
        sprintf(formatted, "%s-All", type_string);

        mudclient_add_offer_menu(mud, type, item_id, item_amount, formatted,
                                 item_name);
    }

    strcpy(mud->menu_items[mud->menu_items_count].action_text, "Examine");
    strcpy(mud->menu_items[mud->menu_items_count].target_text, item_name);
    mud->menu_items[mud->menu_items_count].type = MENU_INVENTORY_EXAMINE;
    mud->menu_items[mud->menu_items_count].index = item_id;
    mud->menu_items_count++;
}
