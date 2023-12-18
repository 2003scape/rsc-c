#include "inventory-tab.h"

void mudclient_draw_ui_tab_inventory(mudclient *mud, int no_menus) {
    int is_touch = mudclient_is_touch(mud);
    int slot_height = ITEM_GRID_SLOT_HEIGHT - is_touch;

    int height = slot_height * INVENTORY_ROWS;

    int ui_x = mud->surface->width - INVENTORY_WIDTH - 3;
    int ui_y = UI_BUTTON_SIZE + 1;

    if (is_touch) {
        ui_x = UI_TABS_TOUCH_X - INVENTORY_WIDTH - 1;
        ui_y = (UI_TABS_TOUCH_Y + UI_TABS_TOUCH_HEIGHT) - height - 2;
    }

#if (VERSION_MEDIA >= 59)
    mudclient_draw_ui_tab_label(mud, INVENTORY_TAB, INVENTORY_WIDTH, ui_x,
                                ui_y - UI_TABS_LABEL_HEIGHT);
#endif

    mud->ui_tab_min_x = ui_x;
    mud->ui_tab_max_x = mud->surface->width;
    mud->ui_tab_min_y = 0;
    mud->ui_tab_max_y = ui_y + height;

    if (is_touch) {
        mud->ui_tab_max_x = ui_x + INVENTORY_WIDTH;
        mud->ui_tab_min_y = ui_y - UI_TABS_LABEL_HEIGHT;
        mud->ui_tab_max_y = ui_y + height;
    }

    /* item slots */
    for (int i = 0; i < INVENTORY_ITEMS_MAX; i++) {
        int slot_x = ui_x + (i % INVENTORY_COLUMNS) * ITEM_GRID_SLOT_WIDTH;
        int slot_y = ui_y + (i / INVENTORY_COLUMNS) * slot_height;
        int slot_colour = GREY_B5;

        if (i < mud->inventory_items_count && mud->inventory_equipped[i]) {
            slot_colour = RED;
        }

        surface_draw_box_alpha(mud->surface, slot_x, slot_y,
                               ITEM_GRID_SLOT_WIDTH, slot_height,
                               slot_colour, 128);

        if (i < mud->inventory_items_count) {
            int item_id = mud->inventory_item_id[i];

            mudclient_draw_item(mud, slot_x, slot_y, ITEM_GRID_SLOT_WIDTH,
                                ITEM_GRID_SLOT_HEIGHT, item_id);

            char formatted_amount[12] = {0};

            mudclient_format_item_amount(
                mud, mud->inventory_item_stack_count[i], formatted_amount);

            if (game_data.items[mud->inventory_item_id[i]].stackable == 0) {
                surface_draw_string(mud->surface, formatted_amount, slot_x + 1,
                                    slot_y + 10, FONT_BOLD_12, YELLOW);
            }
        }
    }

    /* row and column lines */
    for (int i = 1; i <= INVENTORY_COLUMNS - 1; i++) {
        surface_draw_line_vertical(mud->surface,
                                   ui_x + i * ITEM_GRID_SLOT_WIDTH, ui_y,
                                   height, BLACK);
    }

    for (int i = 1; i <= INVENTORY_ROWS - 1; i++) {
        surface_draw_line_horizontal(mud->surface, ui_x,
                                     ui_y + i * slot_height,
                                     INVENTORY_WIDTH, BLACK);
    }

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    if (mouse_x < 0 || mouse_y < 0 || mouse_x > INVENTORY_WIDTH ||
        mouse_y > height) {
        return;
    }

    int item_index = (mouse_x / ITEM_GRID_SLOT_WIDTH) +
                     (mouse_y / slot_height) * INVENTORY_COLUMNS;

    if (item_index >= mud->inventory_items_count) {
        return;
    }

    int item_id = mud->inventory_item_id[item_index];
    char *item_name = game_data.items[item_id].name;

    char formatted_item_name[strlen(item_name) + 6];
    sprintf(formatted_item_name, "@lre@%s", item_name);

    if (mud->selected_wiki) {
        mudclient_menu_add_id_wiki(mud, formatted_item_name, "item", item_id);
    } else if (mud->show_dialog_bank) {
        int item_amount = mudclient_get_inventory_count(mud, item_id);

        mudclient_add_offer_menus(mud, "Deposit", MENU_BANK_DEPOSIT, item_id,
                                  item_amount, formatted_item_name,
                                  mud->bank_last_deposit_offer);
    } else {
        if (mud->selected_spell >= 0) {
            if (game_data.spells[mud->selected_spell].type == 3) {
                sprintf(mud->menu_item_text1[mud->menu_items_count],
                        "Cast %s on",
                        game_data.spells[mud->selected_spell].name);

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_CAST_INVITEM;
                mud->menu_index[mud->menu_items_count] = item_index;

                mud->menu_source_index[mud->menu_items_count] =
                    mud->selected_spell;

                mud->menu_items_count++;

                return;
            }
        } else {
            if (mud->selected_item_inventory_index >= 0) {
                sprintf(mud->menu_item_text1[mud->menu_items_count],
                        "Use %s with:", mud->selected_item_name);

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_USEWITH_INVITEM;
                mud->menu_index[mud->menu_items_count] = item_index;

                mud->menu_source_index[mud->menu_items_count] =
                    mud->selected_item_inventory_index;

                mud->menu_items_count++;

                return;
            }

            if (mud->inventory_equipped[item_index] == 1) {
                strcpy(mud->menu_item_text1[mud->menu_items_count], "Remove");

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_INVENTORY_UNEQUIP;
                mud->menu_index[mud->menu_items_count] = item_index;
                mud->menu_items_count++;
            } else if (game_data.items[item_id].wearable != 0) {
                int is_wield = (game_data.items[item_id].wearable & 24);

                strcpy(mud->menu_item_text1[mud->menu_items_count],
                       is_wield ? "Wield" : "Wear");

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_INVENTORY_WEAR;
                mud->menu_index[mud->menu_items_count] = item_index;
                mud->menu_items_count++;
            }

            if (strlen(game_data.items[item_id].command) > 0) {
                strcpy(mud->menu_item_text1[mud->menu_items_count],
                       game_data.items[item_id].command);

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_INVENTORY_COMMAND;
                mud->menu_index[mud->menu_items_count] = item_index;
                mud->menu_items_count++;
            }

            strcpy(mud->menu_item_text1[mud->menu_items_count], "Use");

            strcpy(mud->menu_item_text2[mud->menu_items_count],
                   formatted_item_name);

            mud->menu_type[mud->menu_items_count] = MENU_INVENTORY_USE;
            mud->menu_index[mud->menu_items_count] = item_index;
            mud->menu_items_count++;

            strcpy(mud->menu_item_text1[mud->menu_items_count], "Drop");

            strcpy(mud->menu_item_text2[mud->menu_items_count],
                   formatted_item_name);

            mud->menu_type[mud->menu_items_count] = MENU_INVENTORY_DROP;
            mud->menu_index[mud->menu_items_count] = item_index;
            mud->menu_items_count++;

            strcpy(mud->menu_item_text1[mud->menu_items_count], "Examine");

            strcpy(mud->menu_item_text2[mud->menu_items_count],
                   formatted_item_name);

            mud->menu_type[mud->menu_items_count] = MENU_INVENTORY_EXAMINE;
            mud->menu_index[mud->menu_items_count] = item_id;
            mud->menu_items_count++;
        }
    }
}
