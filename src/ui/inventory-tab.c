#include "inventory-tab.h"

void mudclient_draw_ui_tab_inventory(mudclient *mud, int no_menus) {
    int ui_x = mud->surface->width - INVENTORY_WIDTH - 3;
    int ui_y = 36;

    surface_draw_sprite_from3(mud->surface,
                              mud->surface->width - UI_TABS_WIDTH - 3, 3,
                              mud->sprite_media + INVENTORY_TAB_SPRITE_OFFSET);

    /* item slots */
    for (int i = 0; i < INVENTORY_ITEMS_MAX; i++) {
        int slot_x = ui_x + (i % INVENTORY_COLUMNS) * INVENTORY_SLOT_WIDTH;
        int slot_y = ui_y + (i / INVENTORY_COLUMNS) * INVENTORY_SLOT_HEIGHT;
        int slot_colour = GREY_B5;

        if (i < mud->inventory_items_count && mud->inventory_equipped[i]) {
            slot_colour = RED;
        }

        surface_draw_box_alpha(mud->surface, slot_x, slot_y,
                               INVENTORY_SLOT_WIDTH, INVENTORY_SLOT_HEIGHT,
                               slot_colour, 128);

        if (i < mud->inventory_items_count) {
            int item_id = mud->inventory_item_id[i];

            mudclient_draw_item(mud, slot_x, slot_y, item_id);

            char formatted_amount[12] = {0};

            mudclient_format_item_amount(
                mud, mud->inventory_item_stack_count[i], formatted_amount);

            if (game_data_item_stackable[mud->inventory_item_id[i]] == 0) {
                surface_draw_string(mud->surface, formatted_amount, slot_x + 1,
                                    slot_y + 10, 1, YELLOW);
            }
        }
    }

    /* row and column lines */
    for (int i = 1; i <= INVENTORY_COLUMNS - 1; i++) {
        surface_draw_line_vertical(mud->surface,
                                   ui_x + i * INVENTORY_SLOT_WIDTH, ui_y,
                                   INVENTORY_HEIGHT, BLACK);
    }

    for (int i = 1; i <= INVENTORY_ROWS - 1; i++) {
        surface_draw_line_horizontal(mud->surface, ui_x,
                                     ui_y + i * INVENTORY_SLOT_HEIGHT,
                                     INVENTORY_WIDTH, BLACK);
    }

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    if (mouse_x < 0 || mouse_y < 0 || mouse_x > INVENTORY_WIDTH ||
        mouse_y > INVENTORY_HEIGHT) {
        return;
    }

    int item_index = (mouse_x / INVENTORY_SLOT_WIDTH) +
                     (mouse_y / INVENTORY_SLOT_HEIGHT) * INVENTORY_COLUMNS;

    if (item_index >= mud->inventory_items_count) {
        return;
    }

    int item_id = mud->inventory_item_id[item_index];
    char *item_name = game_data_item_name[item_id];

    char formatted_item_name[strlen(item_name) + 6];
    sprintf(formatted_item_name, "@lre@%s", item_name);

    if (mud->show_dialog_bank) {
        int item_amount = mudclient_get_inventory_count(mud, item_id);

        mudclient_add_offer_menus(mud, "Deposit", MENU_BANK_DEPOSIT, item_id,
                                  item_amount, formatted_item_name,
                                  mud->bank_last_deposit_offer);
    } else {
        if (mud->selected_spell >= 0) {
            if (game_data_spell_type[mud->selected_spell] == 3) {
                sprintf(mud->menu_item_text1[mud->menu_items_count],
                        "Cast %s on",
                        game_data_spell_name[mud->selected_spell]);

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

                mud->menu_type[mud->menu_items_count] = MENU_INV_UNEQUIP;
                mud->menu_index[mud->menu_items_count] = item_index;
                mud->menu_items_count++;
            } else if (game_data_item_wearable[item_id] != 0) {
                int is_wield = (game_data_item_wearable[item_id] & 24);

                strcpy(mud->menu_item_text1[mud->menu_items_count],
                       is_wield ? "Wield" : "Wear");

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_INV_WEAR;
                mud->menu_index[mud->menu_items_count] = item_index;
                mud->menu_items_count++;
            }

            if (strlen(game_data_item_command[item_id]) > 0) {
                strcpy(mud->menu_item_text1[mud->menu_items_count],
                       game_data_item_command[item_id]);

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_item_name);

                mud->menu_type[mud->menu_items_count] = MENU_INV_COMMAND;
                mud->menu_index[mud->menu_items_count] = item_index;
                mud->menu_items_count++;
            }

            strcpy(mud->menu_item_text1[mud->menu_items_count], "Use");

            strcpy(mud->menu_item_text2[mud->menu_items_count],
                   formatted_item_name);

            mud->menu_type[mud->menu_items_count] = MENU_INV_USE;
            mud->menu_index[mud->menu_items_count] = item_index;
            mud->menu_items_count++;

            strcpy(mud->menu_item_text1[mud->menu_items_count], "Drop");

            strcpy(mud->menu_item_text2[mud->menu_items_count],
                   formatted_item_name);

            mud->menu_type[mud->menu_items_count] = MENU_INV_DROP;
            mud->menu_index[mud->menu_items_count] = item_index;
            mud->menu_items_count++;

            strcpy(mud->menu_item_text1[mud->menu_items_count], "Examine");

            strcpy(mud->menu_item_text2[mud->menu_items_count],
                   formatted_item_name);

            mud->menu_type[mud->menu_items_count] = MENU_INV_EXAMINE;
            mud->menu_index[mud->menu_items_count] = item_id;
            mud->menu_items_count++;
        }
    }
}
