#include "menu.h"

void mudclient_menu_item_click(mudclient *mud, int i) {
    int menu_x = mud->menu_item_x[i];
    int menu_y = mud->menu_item_y[i];
    int menu_index = mud->menu_index[i];
    int menu_source_index = mud->menu_source_index[i];
    int menu_target_index = mud->menu_target_index[i];
    MenuType menu_type = mud->menu_type[i];

    switch (menu_type) {
    case MENU_CAST_GROUNDITEM:
        mudclient_walk_to_ground_item(mud, mud->local_region_x,
                                      mud->local_region_y, menu_x, menu_y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_GROUNDITEM);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_USEWITH_GROUNDITEM:
        mudclient_walk_to_ground_item(mud, mud->local_region_x,
                                      mud->local_region_y, menu_x, menu_y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_USEWITH_GROUNDITEM);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        break;
    case MENU_GROUNDITEM_TAKE:
        mudclient_walk_to_ground_item(mud, mud->local_region_x,
                                      mud->local_region_y, menu_x, menu_y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_GROUNDITEM_TAKE);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_GROUNDITEM_EXAMINE:
        mudclient_show_message(mud, game_data.items[menu_index].description,
                               MESSAGE_TYPE_GAME);
        break;
    case MENU_CAST_WALLOBJECT:
        mudclient_walk_to_wall_object(mud, menu_x, menu_y, menu_index);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_WALLOBJECT);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_byte(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_USEWITH_WALLOBJECT:
        mudclient_walk_to_wall_object(mud, menu_x, menu_y, menu_index);

        packet_stream_new_packet(mud->packet_stream, CLIENT_USEWITH_WALLOBJECT);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_byte(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        break;
    case MENU_WALL_OBJECT_COMMAND1:
        mudclient_walk_to_wall_object(mud, menu_x, menu_y, menu_index);

        packet_stream_new_packet(mud->packet_stream,
                                 CLIENT_WALL_OBJECT_COMMAND1);

        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_byte(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_WALL_OBJECT_COMMAND2:
        mudclient_walk_to_wall_object(mud, menu_x, menu_y, menu_index);

        packet_stream_new_packet(mud->packet_stream,
                                 CLIENT_WALL_OBJECT_COMMAND2);

        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_byte(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_WALL_OBJECT_EXAMINE:
        mudclient_show_message(mud,
                               game_data.wall_objects[menu_index].description,
                               MESSAGE_TYPE_GAME);
        break;
    case MENU_CAST_OBJECT:
        mudclient_walk_to_object(mud, menu_x, menu_y, menu_index,
                                 menu_source_index);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_OBJECT);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_target_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_USEWITH_OBJECT:
        mudclient_walk_to_object(mud, menu_x, menu_y, menu_index,
                                 menu_source_index);

        packet_stream_new_packet(mud->packet_stream, CLIENT_USEWITH_OBJECT);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_target_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        break;
    case MENU_OBJECT_COMMAND1:
        mudclient_walk_to_object(mud, menu_x, menu_y, menu_index,
                                 menu_source_index);

        packet_stream_new_packet(mud->packet_stream, CLIENT_OBJECT_COMMAND1);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_OBJECT_COMMAND2:
        mudclient_walk_to_object(mud, menu_x, menu_y, menu_index,
                                 menu_source_index);

        packet_stream_new_packet(mud->packet_stream, CLIENT_OBJECT_COMMAND2);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_OBJECT_EXAMINE:
        mudclient_show_message(mud, game_data.objects[menu_index].description,
                               MESSAGE_TYPE_GAME);
        break;
    case MENU_CAST_INVITEM:
        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_INVITEM);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_USEWITH_INVITEM:
        packet_stream_new_packet(mud->packet_stream, CLIENT_USEWITH_INVITEM);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        break;
    case MENU_INVENTORY_UNEQUIP:
        packet_stream_new_packet(mud->packet_stream, CLIENT_INVENTORY_UNEQUIP);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_INVENTORY_WEAR:
        packet_stream_new_packet(mud->packet_stream, CLIENT_INVENTORY_WEAR);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_INVENTORY_COMMAND:
        packet_stream_new_packet(mud->packet_stream, CLIENT_INVENTORY_COMMAND);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_INVENTORY_USE:
        mud->selected_item_inventory_index = menu_index;
        mud->show_ui_tab = 0;

        mud->selected_item_name =
            game_data
                .items
                    [mud->inventory_item_id[mud->selected_item_inventory_index]]
                .name;
        break;
    case MENU_INVENTORY_DROP: {
        packet_stream_new_packet(mud->packet_stream, CLIENT_INVENTORY_DROP);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        mud->show_ui_tab = 0;

        char *item_name =
            game_data.items[mud->inventory_item_id[menu_index]].name;

        char formatted_drop[strlen(item_name) + 10];
        sprintf(formatted_drop, "Dropping %s", item_name);

        mudclient_show_message(mud, formatted_drop, MESSAGE_TYPE_BOR);
        break;
    }
    case MENU_INVENTORY_EXAMINE:
        mudclient_show_message(mud, game_data.items[menu_index].description,
                               MESSAGE_TYPE_GAME);

        if (mud->options->condense_item_amounts) {
            char *item_name = game_data.items[menu_index].name;

            if (mud->show_dialog_bank) {
                for (int j = 0; j < mud->bank_item_count; j++) {
                    int bank_count = mud->bank_items_count[j];

                    if (mud->bank_items[j] == menu_index &&
                        bank_count >= 100000) {
                        char formatted_amount[15] = {0};

                        mudclient_format_number_commas(mud, bank_count,
                                                       formatted_amount);

                        char formatted_total[strlen(item_name) + 31];

                        sprintf(formatted_total, "Total %s in bank: %s",
                                item_name, formatted_amount);

                        mudclient_show_message(mud, formatted_total,
                                               MESSAGE_TYPE_GAME);
                        break;
                    }
                }
            }

            int inventory_amount =
                mudclient_get_inventory_count(mud, menu_index);

            if (inventory_amount >= 100000) {
                char formatted_amount[15] = {0};

                mudclient_format_number_commas(mud, inventory_amount,
                                               formatted_amount);

                char formatted_total[strlen(item_name) + 36];

                sprintf(formatted_total, "Total %s in inventory: %s", item_name,
                        formatted_amount);

                mudclient_show_message(mud, formatted_total, MESSAGE_TYPE_GAME);
            }
        }
        break;
    case MENU_CAST_NPC: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_NPC);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    }
    case MENU_USEWITH_NPC: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_USEWITH_NPC);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        break;
    }
    case MENU_NPC_TALK: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_NPC_TALK);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    }
    case MENU_NPC_COMMAND: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_NPC_COMMAND);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    }
    case MENU_NPC_ATTACK1:
    case MENU_NPC_ATTACK2: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_NPC_ATTACK);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    }
    case MENU_NPC_EXAMINE:
        mudclient_show_message(mud, game_data.npcs[menu_index].description,
                               MESSAGE_TYPE_GAME);
        break;
    case MENU_CAST_PLAYER: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_PLAYER);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    }
    case MENU_USEWITH_PLAYER: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_USEWITH_PLAYER);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_put_short(mud->packet_stream, menu_source_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        break;
    }
    case MENU_PLAYER_ATTACK1:
    case MENU_PLAYER_ATTACK2: {
        int x = (menu_x - 64) / MAGIC_LOC;
        int y = (menu_y - 64) / MAGIC_LOC;

        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, x, y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_PLAYER_ATTACK);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    }
    case MENU_PLAYER_DUEL:
        packet_stream_new_packet(mud->packet_stream, CLIENT_PLAYER_DUEL);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_PLAYER_TRADE:
        packet_stream_new_packet(mud->packet_stream, CLIENT_PLAYER_TRADE);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_PLAYER_FOLLOW:
        packet_stream_new_packet(mud->packet_stream, CLIENT_PLAYER_FOLLOW);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_CAST_GROUND:
        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, menu_x, menu_y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_GROUND);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_WALK:
#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        if (mud->scene->gl_terrain_pick_step == 0) {
            mud->scene->gl_terrain_pick_step = 1;
        }

        mud->gl_is_walking = 1;
#else
        mudclient_walk_to_action_source(mud, mud->local_region_x,
                                        mud->local_region_y, menu_x, menu_y, 0);

        if (mud->mouse_click_x_step == -24) {
            mud->mouse_click_x_step = 24;
        }
#endif
        break;
    case MENU_CAST_SELF:
        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_SELF);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_BANK_WITHDRAW:
    case MENU_BANK_DEPOSIT: {
        int is_withdraw = menu_type == MENU_BANK_WITHDRAW;

        if (menu_target_index < 0) {
            mud->bank_offer_type =
                is_withdraw ? BANK_OFFER_WITHDRAW : BANK_OFFER_DEPOSIT;

            mud->offer_id = menu_index;
            mud->offer_max = abs(menu_target_index);

            mud->input_digits_final = 0;
            mud->show_dialog_offer_x = 1;
        } else {
            mudclient_bank_transaction(mud, menu_index, menu_target_index,
                                       is_withdraw ? CLIENT_BANK_WITHDRAW
                                                   : CLIENT_BANK_DEPOSIT);
        }
        break;
    }
    case MENU_TRANSACTION_OFFER: {
        if (menu_target_index < 0) {
            mud->transaction_offer_type = TRANSACTION_OFFER_OFFER;
            mud->offer_id = menu_index;
            mud->offer_max = abs(menu_target_index);
            mud->input_digits_final = 0;
            mud->show_dialog_offer_x = 1;
        } else {
            mudclient_offer_transaction_item(mud,
                                             mud->show_dialog_trade
                                                 ? CLIENT_TRADE_ITEM_UPDATE
                                                 : CLIENT_DUEL_ITEM_UPDATE,
                                             menu_index, menu_target_index);
        }
        break;
    }
    case MENU_TRANSACTION_REMOVE: {
        if (menu_target_index < 0) {
            mud->transaction_offer_type = TRANSACTION_OFFER_REMOVE;
            mud->offer_id = menu_index;
            mud->offer_max = abs(menu_target_index);
            mud->input_digits_final = 0;
            mud->show_dialog_offer_x = 1;
        } else {
            mudclient_remove_transaction_item(mud,
                                              mud->show_dialog_trade
                                                  ? CLIENT_TRADE_ITEM_UPDATE
                                                  : CLIENT_DUEL_ITEM_UPDATE,
                                              menu_index, menu_target_index);
        }
        break;
    }
    case MENU_MAP_LOOK:
        mud->camera_rotation = menu_index;
        break;
    case MENU_WIKI_LOOKUP: {
        char *page_name = mud->menu_wiki_page[i];
        int page_name_length = strlen(page_name);

        char encoded_page_name[(page_name_length * 3) + 1];

        if (strncmp(page_name, "Special:", strlen("Special:")) == 0) {
            strcpy(encoded_page_name, page_name);
        } else {
            url_encode(page_name, encoded_page_name);
        }

        char encoded_url[strlen(mud->options->wiki_url) +
                         strlen(encoded_page_name) + 1];

        sprintf(encoded_url, mud->options->wiki_url, encoded_page_name);

#ifdef EMSCRIPTEN
        EM_ASM(
            {
                const url = UTF8ToString($0);
                window.open(url, '_blank');
            },
            encoded_url);
#else
        char formatted_command[strlen(encoded_url) +
                               strlen(mud->options->browser_command) + 1];

        sprintf(formatted_command, mud->options->browser_command, encoded_url);

        system(formatted_command);
#endif

        mud->selected_wiki = 0;
        break;
    }
    case MENU_CANCEL:
        mud->selected_item_inventory_index = -1;
        mud->selected_spell = -1;
        mud->selected_wiki = 0;
        break;
    }
}

void mudclient_create_top_mouse_menu(mudclient *mud) {
    int add_cancel = mud->selected_spell >= 0 ||
                     mud->selected_item_inventory_index >= 0 ||
                     mud->selected_wiki;

    if (!add_cancel) {
        for (int i = 0; i < mud->menu_items_count; i++) {
            if (mud->menu_type[i] == MENU_MAP_LOOK ||
                mud->menu_type[i] == MENU_BANK_WITHDRAW ||
                mud->menu_type[i] == MENU_BANK_DEPOSIT ||
                mud->menu_type[i] == MENU_TRANSACTION_OFFER ||
                mud->menu_type[i] == MENU_TRANSACTION_REMOVE) {
                add_cancel = 1;
                break;
            }
        }
    }

    if (add_cancel) {
        strcpy(mud->menu_item_text1[mud->menu_items_count], "Cancel");
        strcpy(mud->menu_item_text2[mud->menu_items_count], "");
        mud->menu_type[mud->menu_items_count] = MENU_CANCEL;
        mud->menu_items_count++;
    }

    for (int i = 0; i < mud->menu_items_count; i++) {
        mud->menu_indices[i] = i;
    }

    /* sort */
    for (int flag = 0; !flag;) {
        flag = 1;

        for (int i = 0; i < mud->menu_items_count - 1; i++) {
            int current = mud->menu_indices[i];
            int next = mud->menu_indices[i + 1];

            if (mud->menu_type[current] > mud->menu_type[next]) {
                mud->menu_indices[i] = next;
                mud->menu_indices[i + 1] = current;
                flag = 0;
            }
        }
    }

    if (mud->menu_items_count <= 0) {
        return;
    }

    if (mud->menu_items_count > MENU_ITEMS_MAX) {
        mud->menu_items_count = MENU_ITEMS_MAX;
    }

    int index = -1;

    for (int i = 0; i < mud->menu_items_count; i++) {
        if (strlen(mud->menu_item_text2[mud->menu_indices[i]]) <= 0) {
            continue;
        }

        index = i;
        break;
    }

    char menu_text[255] = {0};

#if !defined(WII) && !defined(_3DS)
    if ((index == -1 || !mud->selected_wiki) && mud->is_hand_cursor) {
        SDL_SetCursor(mud->default_cursor);
        mud->is_hand_cursor = 0;
    }
#endif

    if (index == -1 && mud->selected_wiki) {
        strcpy(menu_text, "@cya@Choose a wiki lookup target");
    } else if ((mud->selected_item_inventory_index >= 0 ||
                mud->selected_spell >= 0) &&
               mud->menu_items_count == 1) {
        strcpy(menu_text, "Choose a target");
    } else if ((mud->selected_item_inventory_index >= 0 ||
                mud->selected_spell >= 0) &&
               mud->menu_items_count > 1) {
        sprintf(menu_text, "@whi@%s %s",
                mud->menu_item_text1[mud->menu_indices[0]],
                mud->menu_item_text2[mud->menu_indices[0]]);
    } else if (index != -1) {
#if !defined(WII) && !defined(_3DS)
        if (mud->selected_wiki && !mud->is_hand_cursor) {
            SDL_SetCursor(mud->hand_cursor);
            mud->is_hand_cursor = 1;
        }
#endif

        sprintf(menu_text, "%s: @whi@%s",
                mud->menu_item_text2[mud->menu_indices[index]],
                mud->menu_item_text1[mud->menu_indices[0]]);
    }

    if (mud->menu_items_count == 2 && strlen(menu_text) > 0) {
        strcat(menu_text, "@whi@ / 1 more option");
    }

    if (mud->menu_items_count > 2 && strlen(menu_text) > 0) {
        char more_options[33] = {0};

        sprintf(more_options, "@whi@ / %d more options",
                mud->menu_items_count - 1);

        strcat(menu_text, more_options);
    }

    int is_touch = mudclient_is_touch(mud);

    if (!is_touch && strlen(menu_text) > 0) {
        surface_draw_string(mud->surface, menu_text, 6, 14, FONT_BOLD_12,
                            YELLOW);
    }

    if ((!mud->settings_mouse_button_one && mud->mouse_button_click == 1) ||
        (mud->settings_mouse_button_one && mud->mouse_button_click == 1 &&
         mud->menu_items_count == 1)) {
        mudclient_menu_item_click(mud, mud->menu_indices[0]);
        mud->mouse_button_click = 0;
        return;
    }

    if ((!mud->settings_mouse_button_one && mud->mouse_button_click == 2) ||
        (mud->settings_mouse_button_one && mud->mouse_button_click == 1)) {
        int entry_height = is_touch ? 19 : 15;

        mud->menu_height = (mud->menu_items_count + 1) * entry_height;
        mud->menu_width = surface_text_width("Choose option", 1) + 5;

        if (is_touch) {
            mud->menu_height += 3;
        }

        for (int i = 0; i < mud->menu_items_count; i++) {
            char *menu_item_text1 = mud->menu_item_text1[i];
            char *menu_item_text2 = mud->menu_item_text2[i];

            char menu_item_text[strlen(menu_item_text1) +
                                strlen(menu_item_text2) + 2];

            sprintf(menu_item_text, "%s %s", menu_item_text1,
                    menu_item_text2);

            int text_width = surface_text_width(menu_item_text, 1) + 5;

            if (text_width > mud->menu_width) {
                mud->menu_width = text_width;
            }
        }

        mud->menu_x = mud->mouse_x - (mud->menu_width / 2);
        mud->menu_y = mud->mouse_y - 7;
        mud->show_right_click_menu = 1;

        if (mud->menu_x < 0) {
            mud->menu_x = 0;
        }

        if (mud->menu_y < 0) {
            mud->menu_y = 0;
        }

        if (mud->menu_x + mud->menu_width > mud->surface->width - 2) {
            mud->menu_x = mud->surface->width - 2 - mud->menu_width;
        }

        if (mud->menu_y + mud->menu_height > mud->surface->height - 31) {
            mud->menu_y = mud->surface->height - 31 - mud->menu_height;
        }

        mud->mouse_button_click = 0;
    }
}

void mudclient_menu_add_wiki(mudclient *mud, const char *display,
                             const char *page) {
    if (!mud->options->wiki_lookup) {
        return;
    }

    strcpy(mud->menu_item_text1[mud->menu_items_count], "Wiki lookup");
    strcpy(mud->menu_item_text2[mud->menu_items_count], display);
    mud->menu_type[mud->menu_items_count] = MENU_WIKI_LOOKUP;
    strcpy(mud->menu_wiki_page[mud->menu_items_count], page);
    mud->menu_items_count++;
}

void mudclient_menu_add_id_wiki(mudclient *mud, const char *display,
                                const char *type, int id) {
    const char *name = display;

    for (int i = strlen(name); i >= 0; i--) {
        if (name[i] == '@') {
            name += i + 1;
            break;
        }
    }

    char encoded_display[(strlen(name) * 3) + 1];
    url_encode(name, encoded_display);

    char page[strlen(WIKI_TYPE_PAGE) + strlen(type) + strlen(encoded_display) +
              5];

    sprintf(page, WIKI_TYPE_PAGE, type, id, encoded_display);
    mudclient_menu_add_wiki(mud, display, page);
}

void mudclient_menu_add_ground_item(mudclient *mud, int index) {
    int item_id = mud->ground_items[index].id;
    char *item_name = game_data.items[item_id].name;

    char formatted_item_name[strlen(item_name) + 6];
    sprintf(formatted_item_name, "@lre@%s", item_name);

    strcpy(mud->menu_item_text2[mud->menu_items_count], formatted_item_name);

    mud->menu_item_x[mud->menu_items_count] = mud->ground_items[index].x;
    mud->menu_item_y[mud->menu_items_count] = mud->ground_items[index].y;
    mud->menu_index[mud->menu_items_count] = item_id;

    if (mud->selected_wiki) {
        mudclient_menu_add_id_wiki(mud, formatted_item_name, "item", item_id);
    } else if (mud->selected_spell >= 0) {
        if (game_data.spells[mud->selected_spell].type == 3) {
            sprintf(mud->menu_item_text1[mud->menu_items_count], "Cast %s on",
                    game_data.spells[mud->selected_spell].name);

            mud->menu_type[mud->menu_items_count] = MENU_CAST_GROUNDITEM;
            mud->menu_source_index[mud->menu_items_count] = mud->selected_spell;
            mud->menu_items_count++;
        }
    } else if (mud->selected_item_inventory_index >= 0) {
        sprintf(mud->menu_item_text1[mud->menu_items_count], "Use %s with",
                mud->selected_item_name);

        mud->menu_type[mud->menu_items_count] = MENU_USEWITH_GROUNDITEM;

        mud->menu_source_index[mud->menu_items_count] =
            mud->selected_item_inventory_index;

        mud->menu_items_count++;
    } else {
        strcpy(mud->menu_item_text1[mud->menu_items_count], "Take");

        mud->menu_type[mud->menu_items_count] = MENU_GROUNDITEM_TAKE;
        mud->menu_items_count++;

        strcpy(mud->menu_item_text1[mud->menu_items_count], "Examine");

        strcpy(mud->menu_item_text2[mud->menu_items_count],
               mud->menu_item_text2[mud->menu_items_count - 1]);

        mud->menu_type[mud->menu_items_count] = MENU_GROUNDITEM_EXAMINE;
        mud->menu_index[mud->menu_items_count] = mud->ground_items[index].id;
        mud->menu_items_count++;
    }
}

void mudclient_create_right_click_menu(mudclient *mud) {
    int wilderness_depth = mudclient_get_wilderness_depth(mud);

    int selected_face = -1;

    for (int i = 0; i < mud->object_count; i++) {
        mud->objects[i].already_in_menu = 0;
    }

    for (int i = 0; i < mud->wall_object_count; i++) {
        mud->wall_objects[i].already_in_menu = 0;
    }

    for (int i = 0; i < mud->ground_item_count; i++) {
        mud->ground_items[i].already_in_menu = 0;
    }

    int picked_count = mud->scene->mouse_picked_count;
    GameModel **picked_models = mud->scene->mouse_picked_models;
    int *picked_faces = mud->scene->mouse_picked_faces;

    for (int i = 0; i < picked_count; i++) {
        if (mud->menu_items_count > 200) {
            break;
        }

        GameModel *game_model = picked_models[i];
        int face = picked_faces[i];

        // TODO clean up
        if (face != -1 && !((game_model->face_tag[face] <= 65535 ||
                             game_model->face_tag[face] >= 200000) &&
                            game_model->face_tag[face] <= 300000)) {
            continue;
        }

        /* 2D face picking */
        if (game_model == mud->scene->view) {
            int index = game_model->face_tag[face] % 10000;

            /* 1 = player, 2 = ground item, 3 = npc */
            int type = game_model->face_tag[face] / 10000;

            if (!mud->selected_wiki && type == 1) {
                GameCharacter *player = mud->players[index];
                char level_text[26] = {0};
                int level_difference = 0;

                if (mud->local_player->level > 0 &&
                    mud->players[index]->level > 0) {
                    level_difference =
                        mud->local_player->level - mud->players[index]->level;
                }

                char colour[6] = {0};
                get_level_difference_colour(level_difference, colour);

                sprintf(level_text, " %s(level-%d)", colour,
                        mud->players[index]->level);

                if (mud->selected_spell >= 0) {
                    if (game_data.spells[mud->selected_spell].type == 1 ||
                        game_data.spells[mud->selected_spell].type == 2) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Cast %s on",
                                game_data.spells[mud->selected_spell].name);

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@whi@%s%s", mud->players[index]->name,
                                level_text);

                        mud->combat_target = player;

                        mud->menu_type[mud->menu_items_count] =
                            MENU_CAST_PLAYER;

                        mud->menu_item_x[mud->menu_items_count] =
                            player->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            player->current_y;

                        mud->menu_index[mud->menu_items_count] =
                            player->server_index;

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->selected_spell;

                        mud->menu_items_count++;
                    }
                } else if (mud->selected_item_inventory_index >= 0) {
                    sprintf(mud->menu_item_text1[mud->menu_items_count],
                            "Use %s with", mud->selected_item_name);

                    sprintf(mud->menu_item_text2[mud->menu_items_count],
                            "@whi@%s%s", mud->players[index]->name, level_text);

                    mud->menu_type[mud->menu_items_count] = MENU_USEWITH_PLAYER;

                    mud->menu_item_x[mud->menu_items_count] = player->current_x;
                    mud->menu_item_y[mud->menu_items_count] = player->current_y;

                    mud->menu_index[mud->menu_items_count] =
                        player->server_index;

                    mud->menu_source_index[mud->menu_items_count] =
                        mud->selected_item_inventory_index;

                    mud->menu_items_count++;
                } else {
                    if (wilderness_depth > 0 &&
                        ((mud->players[index]->current_y - 64) / MAGIC_LOC +
                         mud->plane_height + mud->region_y) < 2203) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Attack");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@whi@%s%s", mud->players[index]->name,
                                level_text);

                        mud->menu_type[mud->menu_items_count] =
                            level_difference >= 0 && level_difference < 5
                                ? MENU_PLAYER_ATTACK1
                                : MENU_PLAYER_ATTACK2;

                        mud->combat_target = player;

                        mud->menu_item_x[mud->menu_items_count] =
                            player->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            player->current_y;

                        mud->menu_index[mud->menu_items_count] =
                            player->server_index;

                        mud->menu_items_count++;
                    } else if (mud->options->members) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Duel with");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@whi@%s%s", mud->players[index]->name,
                                level_text);

                        mud->menu_item_x[mud->menu_items_count] =
                            player->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            player->current_y;

                        mud->menu_type[mud->menu_items_count] =
                            MENU_PLAYER_DUEL;

                        mud->menu_index[mud->menu_items_count] =
                            player->server_index;

                        mud->menu_items_count++;
                    }

                    strcpy(mud->menu_item_text1[mud->menu_items_count],
                           "Trade with");

                    sprintf(mud->menu_item_text2[mud->menu_items_count],
                            "@whi@%s%s", player->name, level_text);

                    mud->menu_type[mud->menu_items_count] = MENU_PLAYER_TRADE;

                    mud->menu_index[mud->menu_items_count] =
                        player->server_index;

                    mud->menu_items_count++;

                    strcpy(mud->menu_item_text1[mud->menu_items_count],
                           "Follow");

                    sprintf(mud->menu_item_text2[mud->menu_items_count],
                            "@whi@%s%s", player->name, level_text);

                    mud->menu_type[mud->menu_items_count] = MENU_PLAYER_FOLLOW;

                    mud->menu_index[mud->menu_items_count] =
                        player->server_index;

                    mud->menu_items_count++;
                }
            } else if (type == 2 && !mud->options->ground_item_models) {
                mudclient_menu_add_ground_item(mud, index);
            } else if (type == 3) {
                GameCharacter *npc = mud->npcs[index];
                char level_text[26] = {0};
                int level_difference = -1;
                int npc_id = npc->npc_id;
                char *npc_name = game_data.npcs[npc_id].name;

                char formatted_npc_name[strlen(npc_name) + 6];
                sprintf(formatted_npc_name, "@yel@%s", npc_name);

                if (game_data.npcs[npc_id].attackable > 0) {
                    int npc_level = (game_data.npcs[npc_id].attack +
                                     game_data.npcs[npc_id].defense +
                                     game_data.npcs[npc_id].strength +
                                     game_data.npcs[npc_id].hits) /
                                    4;

                    int player_level =
                        (mud->player_skill_base[SKILL_ATTACK] +
                         mud->player_skill_base[SKILL_DEFENSE] +
                         mud->player_skill_base[SKILL_STRENGTH] +
                         mud->player_skill_base[SKILL_HITS] + 27) /
                        4;

                    level_difference = player_level - npc_level;

                    char colour[6] = "@yel@";
                    get_level_difference_colour(level_difference, colour);

                    sprintf(level_text, " %s(level-%d)", colour, npc_level);
                }

                mud->menu_item_x[mud->menu_items_count] = npc->current_x;
                mud->menu_item_y[mud->menu_items_count] = npc->current_y;
                mud->menu_index[mud->menu_items_count] = npc->server_index;

                if (mud->selected_wiki) {
                    mudclient_menu_add_id_wiki(mud, formatted_npc_name, "npc",
                                               npc_id);
                } else if (mud->selected_spell >= 0) {
                    if (game_data.spells[mud->selected_spell].type == 2) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Cast %s on",
                                game_data.spells[mud->selected_spell].name);

                        strcpy(mud->menu_item_text2[mud->menu_items_count],
                               formatted_npc_name);

                        mud->combat_target = npc;

                        mud->menu_type[mud->menu_items_count] = MENU_CAST_NPC;

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->selected_spell;

                        mud->menu_items_count++;
                    }
                } else if (mud->selected_item_inventory_index >= 0) {
                    sprintf(mud->menu_item_text1[mud->menu_items_count],
                            "Use %s with", mud->selected_item_name);

                    sprintf(mud->menu_item_text2[mud->menu_items_count],
                            "@yel@%s",
                            game_data.npcs[mud->npcs[index]->npc_id].name);

                    mud->menu_type[mud->menu_items_count] = MENU_USEWITH_NPC;

                    mud->menu_source_index[mud->menu_items_count] =
                        mud->selected_item_inventory_index;

                    mud->menu_items_count++;
                } else {
                    if (game_data.npcs[npc_id].attackable > 0) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Attack");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s%s",
                                game_data.npcs[mud->npcs[index]->npc_id].name,
                                level_text);

                        mud->combat_target = npc;

                        if (level_difference >= 0) {
                            mud->menu_type[mud->menu_items_count] =
                                MENU_NPC_ATTACK1;
                        } else {
                            mud->menu_type[mud->menu_items_count] =
                                MENU_NPC_ATTACK2;
                        }

                        mud->menu_items_count++;
                    }

                    strcpy(mud->menu_item_text1[mud->menu_items_count],
                           "Talk-to");

                    strcpy(mud->menu_item_text2[mud->menu_items_count],
                           formatted_npc_name);

                    mud->menu_type[mud->menu_items_count] = MENU_NPC_TALK;

                    mud->menu_item_x[mud->menu_items_count] = npc->current_x;
                    mud->menu_item_y[mud->menu_items_count] = npc->current_y;
                    mud->menu_index[mud->menu_items_count] = npc->server_index;

                    mud->menu_items_count++;

                    if (strlen(game_data.npcs[npc_id].command) > 0) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               game_data.npcs[npc_id].command);

                        strcpy(mud->menu_item_text2[mud->menu_items_count],
                               formatted_npc_name);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_NPC_COMMAND;

                        mud->menu_item_x[mud->menu_items_count] =
                            npc->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            npc->current_y;

                        mud->menu_index[mud->menu_items_count] =
                            npc->server_index;

                        mud->menu_items_count++;
                    }

                    strcpy(mud->menu_item_text1[mud->menu_items_count],
                           "Examine");

                    strcpy(mud->menu_item_text2[mud->menu_items_count],
                           formatted_npc_name);

                    mud->menu_type[mud->menu_items_count] = MENU_NPC_EXAMINE;
                    mud->menu_index[mud->menu_items_count] = npc_id;

                    mud->menu_items_count++;
                }
            }
        } else if (game_model && game_model->key >= 20000) {
            int index = game_model->key - 20000;

            if (!mud->ground_items[index].already_in_menu) {
                mudclient_menu_add_ground_item(mud, index);
                mud->ground_items[index].already_in_menu = 1;
            }
        } else if (game_model && game_model->key >= 10000) {
            int index = game_model->key - 10000;
            int wall_object_id = mud->wall_objects[index].id;

            char *wall_object_name =
                game_data.wall_objects[wall_object_id].name;

            char formatted_wall_object_name[strlen(wall_object_name) + 6];
            sprintf(formatted_wall_object_name, "@cya@%s", wall_object_name);

            if (mud->selected_wiki) {
                mudclient_menu_add_id_wiki(mud, formatted_wall_object_name,
                                           "wallobject", wall_object_id);
            } else if (!mud->wall_objects[index].already_in_menu) {
                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_wall_object_name);

                mud->menu_item_x[mud->menu_items_count] =
                    mud->wall_objects[index].x;

                mud->menu_item_y[mud->menu_items_count] =
                    mud->wall_objects[index].y;

                mud->menu_index[mud->menu_items_count] =
                    mud->wall_objects[index].direction;

                if (mud->selected_spell >= 0) {
                    if (game_data.spells[mud->selected_spell].type == 4) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Cast %s on",
                                game_data.spells[mud->selected_spell].name);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_CAST_WALLOBJECT;

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->selected_spell;

                        mud->menu_items_count++;
                    }
                } else if (mud->selected_item_inventory_index >= 0) {
                    sprintf(mud->menu_item_text1[mud->menu_items_count],
                            "Use %s with", mud->selected_item_name);

                    mud->menu_type[mud->menu_items_count] =
                        MENU_USEWITH_WALLOBJECT;

                    mud->menu_source_index[mud->menu_items_count] =
                        mud->selected_item_inventory_index;

                    mud->menu_items_count++;
                } else {
                    if (strncasecmp(
                            game_data.wall_objects[wall_object_id].command1,
                            "WalkTo", 6) != 0) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               game_data.wall_objects[wall_object_id].command1);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_WALL_OBJECT_COMMAND1;

                        mud->menu_items_count++;
                    }

                    if (strncasecmp(
                            game_data.wall_objects[wall_object_id].command2,
                            "Examine", 7) != 0) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               game_data.wall_objects[wall_object_id].command2);

                        strcpy(mud->menu_item_text2[mud->menu_items_count],
                               formatted_wall_object_name);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_WALL_OBJECT_COMMAND2;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->wall_objects[index].x;

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->wall_objects[index].y;

                        mud->menu_index[mud->menu_items_count] =
                            mud->wall_objects[index].direction;

                        mud->menu_items_count++;
                    }

                    strcpy(mud->menu_item_text1[mud->menu_items_count],
                           "Examine");

                    sprintf(mud->menu_item_text2[mud->menu_items_count],
                            "@cya@%s",
                            game_data.wall_objects[wall_object_id].name);

                    mud->menu_type[mud->menu_items_count] =
                        MENU_WALL_OBJECT_EXAMINE;

                    mud->menu_index[mud->menu_items_count] = wall_object_id;
                    mud->menu_items_count++;
                }

                mud->wall_objects[index].already_in_menu = 1;
            }
        } else if (game_model && game_model->key >= 0) {
            int index = game_model->key;
            int id = mud->objects[index].id;

            if (!mud->objects[index].already_in_menu) {
                int object_id = mud->objects[index].id;
                char *object_name = game_data.objects[id].name;

                char formatted_object_name[strlen(object_name) + 6];
                sprintf(formatted_object_name, "@cya@%s", object_name);

                strcpy(mud->menu_item_text2[mud->menu_items_count],
                       formatted_object_name);

                mud->menu_item_x[mud->menu_items_count] = mud->objects[index].x;
                mud->menu_item_y[mud->menu_items_count] = mud->objects[index].y;

                mud->menu_index[mud->menu_items_count] =
                    mud->objects[index].direction;

                mud->menu_source_index[mud->menu_items_count] = object_id;

                if (mud->selected_wiki) {
                    mudclient_menu_add_id_wiki(mud, formatted_object_name,
                                               "object", object_id);
                } else if (mud->selected_spell >= 0) {
                    if (game_data.spells[mud->selected_spell].type == 5) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Cast %s on",
                                game_data.spells[mud->selected_spell].name);

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@cya@%s", game_data.objects[id].name);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_CAST_OBJECT;

                        mud->menu_target_index[mud->menu_items_count] =
                            mud->selected_spell;

                        mud->menu_items_count++;
                    }
                } else if (mud->selected_item_inventory_index >= 0) {
                    sprintf(mud->menu_item_text1[mud->menu_items_count],
                            "Use %s with", mud->selected_item_name);

                    mud->menu_type[mud->menu_items_count] = MENU_USEWITH_OBJECT;

                    mud->menu_target_index[mud->menu_items_count] =
                        mud->selected_item_inventory_index;

                    mud->menu_items_count++;
                } else {
                    if (strncasecmp(game_data.objects[id].command1, "WalkTo",
                                    6) != 0) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               game_data.objects[id].command1);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_OBJECT_COMMAND1;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->objects[index].x;

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->objects[index].y;

                        mud->menu_index[mud->menu_items_count] =
                            mud->objects[index].direction;

                        mud->menu_source_index[mud->menu_items_count] =
                            object_id;

                        mud->menu_items_count++;
                    }

                    if (strncasecmp(game_data.objects[id].command2, "Examine",
                                    7) != 0) {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               game_data.objects[id].command2);

                        strcpy(mud->menu_item_text2[mud->menu_items_count],
                               formatted_object_name);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_OBJECT_COMMAND2;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->objects[index].x;

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->objects[index].y;

                        mud->menu_index[mud->menu_items_count] =
                            mud->objects[index].direction;

                        mud->menu_source_index[mud->menu_items_count] =
                            object_id;

                        mud->menu_items_count++;
                    }

                    strcpy(mud->menu_item_text1[mud->menu_items_count],
                           "Examine");

                    sprintf(mud->menu_item_text2[mud->menu_items_count],
                            "@cya@%s", game_data.objects[id].name);

                    mud->menu_type[mud->menu_items_count] = MENU_OBJECT_EXAMINE;
                    mud->menu_index[mud->menu_items_count] = id;
                    mud->menu_items_count++;
                }

                mud->objects[index].already_in_menu = 1;
            }
        } else {
            if (face >= 0) {
                face = game_model->face_tag[face] - TILE_FACE_TAG;
            }

            if (face >= 0) {
                selected_face = face;
            }
        }
    }

    if (mud->selected_spell >= 0 &&
        game_data.spells[mud->selected_spell].type <= 1) {
        sprintf(mud->menu_item_text1[mud->menu_items_count], "Cast %s on self",
                game_data.spells[mud->selected_spell].name);

        strcpy(mud->menu_item_text2[mud->menu_items_count], "");

        mud->menu_type[mud->menu_items_count] = MENU_CAST_SELF;
        mud->menu_index[mud->menu_items_count] = mud->selected_spell;
        mud->menu_items_count++;
    }

    int walkable = selected_face != -1;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    walkable = mud->scene->gl_terrain_walkable;
#endif

    if (walkable) {
        if (mud->selected_spell >= 0) {
            if (game_data.spells[mud->selected_spell].type == 6) {
                sprintf(mud->menu_item_text1[mud->menu_items_count],
                        "Cast %s on ground",
                        game_data.spells[mud->selected_spell].name);

                strcpy(mud->menu_item_text2[mud->menu_items_count], "");

                mud->menu_type[mud->menu_items_count] = MENU_CAST_GROUND;

#ifdef RENDER_SW
                mud->menu_item_x[mud->menu_items_count] =
                    mud->world->local_x[selected_face];

                mud->menu_item_y[mud->menu_items_count] =
                    mud->world->local_y[selected_face];
#endif

                mud->menu_index[mud->menu_items_count] = mud->selected_spell;
                mud->menu_items_count++;
                return;
            }
        } else if (!mud->selected_wiki &&
                   mud->selected_item_inventory_index < 0) {
            strcpy(mud->menu_item_text1[mud->menu_items_count], "Walk here");
            strcpy(mud->menu_item_text2[mud->menu_items_count], "");

            mud->menu_type[mud->menu_items_count] = MENU_WALK;

#ifdef RENDER_SW
            mud->menu_item_x[mud->menu_items_count] =
                mud->world->local_x[selected_face];

            mud->menu_item_y[mud->menu_items_count] =
                mud->world->local_y[selected_face];
#endif

            mud->menu_items_count++;
        }
    }
}

void mudclient_draw_right_click_menu(mudclient *mud) {
    int is_touch = mudclient_is_touch(mud);
    int entry_height = is_touch ? 19 : 15;

    if (mud->mouse_button_click != 0) {
        for (int i = 0; i < mud->menu_items_count; i++) {
            int entry_x = mud->menu_x + 2;
            int entry_y = mud->menu_y + entry_height + 12 + i * entry_height;

            int min_y = entry_y - (is_touch ? 14 : 12);
            int max_y = entry_y + (is_touch ? 6 : 4);

            if (mud->mouse_x <= entry_x - 2 || mud->mouse_y <= min_y ||
                mud->mouse_y >= max_y ||
                mud->mouse_x >= (entry_x - 3) + mud->menu_width) {
                continue;
            }

            mudclient_menu_item_click(mud, mud->menu_indices[i]);
            break;
        }

        mud->mouse_button_click = 0;
        mud->show_right_click_menu = 0;
        return;
    }

    if (mud->mouse_x < mud->menu_x - 10 || mud->mouse_y < mud->menu_y - 10 ||
        mud->mouse_x > mud->menu_x + mud->menu_width + 10 ||
        mud->mouse_y > mud->menu_y + mud->menu_height + 10) {
        mud->show_right_click_menu = 0;
        // TODO on mobile set click/down to 0
        return;
    }

    /* make it a bit darker for the item interfaces */
    int is_dark_menu = mud->show_dialog_bank || mud->show_dialog_trade ||
                       mud->show_dialog_duel;

    surface_draw_box_alpha(mud->surface, mud->menu_x, mud->menu_y,
                           mud->menu_width, mud->menu_height, GREY_D0,
                           160 + (is_dark_menu ? 40 : 0));

    surface_draw_string(mud->surface, "Choose option", mud->menu_x + 2,
                        mud->menu_y + 12, FONT_BOLD_12, CYAN);

    for (int i = 0; i < mud->menu_items_count; i++) {
#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        /* pick and store the position as soon as the right click menu is opened
         * in case the camera moves before they press it */
        if (mud->scene->gl_terrain_pick_step == GL_PICK_STEP_NONE &&
            mud->menu_type[i] == MENU_WALK) {
            mud->scene->gl_terrain_pick_step = GL_PICK_STEP_SAMPLE;
        }
#endif

        int entry_x = mud->menu_x + 2;
        int entry_y = mud->menu_y + entry_height + 12 + i * entry_height;
        int text_colour = WHITE;

        int min_y = entry_y - (is_touch ? 14 : 12);
        int max_y = entry_y + (is_touch ? 6 : 4);

        if (mud->mouse_x > entry_x - 2 && mud->mouse_y > min_y &&
            mud->mouse_y < max_y &&
            mud->mouse_x < (entry_x - 3) + mud->menu_width) {
            text_colour = YELLOW;
        }

        char *menu_item_text1 = mud->menu_item_text1[mud->menu_indices[i]];
        char *menu_item_text2 = mud->menu_item_text2[mud->menu_indices[i]];

        char combined[strlen(menu_item_text1) + strlen(menu_item_text2) + 2];
        sprintf(combined, "%s %s", menu_item_text1, menu_item_text2);

        surface_draw_string(mud->surface, combined, entry_x, entry_y,
                            FONT_BOLD_12, text_colour);
    }
}

void mudclient_draw_hover_tooltip(mudclient *mud) {
    if (mud->options->show_hover_tooltip && mud->menu_items_count > 1 &&
        mud->mouse_button_down == 0 && mud->show_right_click_menu == 0) {
        char *menu_item_text1 = mud->menu_item_text1[mud->menu_indices[0]];

        // if the first menu item equals "Walk here" then we don't want to show
        // the tooltip
        if (strcmp(menu_item_text1, "Walk here") == 0) {
            return;
        }

        char *menu_item_text2 = mud->menu_item_text2[mud->menu_indices[0]];

        char combined[strlen(menu_item_text1) + strlen(menu_item_text2) + 2];
        sprintf(combined, "%s %s", menu_item_text1, menu_item_text2);

        int text_width = surface_text_width(combined, FONT_BOLD_12);
        int text_height = surface_text_height(FONT_BOLD_12);

        surface_draw_box_alpha(mud->surface, mud->mouse_x,
                               mud->mouse_y - text_height, text_width + 5,
                               text_height + 3, GREY_D0, 120);

        surface_draw_string(mud->surface, combined, mud->mouse_x + 2,
                            mud->mouse_y, FONT_BOLD_12, WHITE);
    }
}
