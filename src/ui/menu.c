#include "menu.h"

void mudclient_menu_item_click(mudclient *mud, int i) {
    int menu_x = mud->menu_item_x[i];
    int menu_y = mud->menu_item_y[i];
    int menu_index = mud->menu_index[i];
    int menu_source_index = mud->menu_source_index[i];
    int menu_target_index = mud->menu_target_index[i];
    int menu_type = mud->menu_type[i];

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
        mudclient_show_message(mud, game_data_item_description[menu_index],
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
                               game_data_wall_object_description[menu_index],
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
        mudclient_show_message(mud, game_data_object_description[menu_index],
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
    case MENU_INV_UNEQUIP:
        packet_stream_new_packet(mud->packet_stream, CLIENT_INV_UNEQUIP);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_INV_WEAR:
        packet_stream_new_packet(mud->packet_stream, CLIENT_INV_WEAR);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_INV_COMMAND:
        packet_stream_new_packet(mud->packet_stream, CLIENT_INV_COMMAND);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);
        break;
    case MENU_INV_USE:
        mud->selected_item_inventory_index = menu_index;
        mud->show_ui_tab = 0;

        mud->selected_item_name = game_data_item_name
            [mud->inventory_item_id[mud->selected_item_inventory_index]];
        break;
    case MENU_INV_DROP: {
        packet_stream_new_packet(mud->packet_stream, CLIENT_INV_DROP);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_item_inventory_index = -1;
        mud->show_ui_tab = 0;

        char *item_name =
            game_data_item_name[mud->inventory_item_id[menu_index]];

        char formatted_drop[strlen(item_name) + 10];
        sprintf(formatted_drop, "Dropping %s", item_name);

        mudclient_show_message(mud, formatted_drop, MESSAGE_TYPE_BOR);
        break;
    }
    case MENU_INV_EXAMINE:
        mudclient_show_message(mud, game_data_item_description[menu_index],
                               MESSAGE_TYPE_GAME);
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
        mudclient_show_message(mud, game_data_npc_description[menu_index],
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
        mudclient_walk_to_action_source(
            mud, mud->local_region_x, mud->local_region_y, menu_x, menu_y, 1);

        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_GROUND);
        packet_stream_put_short(mud->packet_stream, menu_x + mud->region_x);
        packet_stream_put_short(mud->packet_stream, menu_y + mud->region_y);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_WALK:
        mudclient_walk_to_action_source(
            mud, mud->local_region_x, mud->local_region_y, menu_x, menu_y, 0);

        if (mud->mouse_click_x_step == -24) {
            mud->mouse_click_x_step = 24;
        }
        break;
    case MENU_CAST_SELF:
        packet_stream_new_packet(mud->packet_stream, CLIENT_CAST_SELF);
        packet_stream_put_short(mud->packet_stream, menu_index);
        packet_stream_send_packet(mud->packet_stream);

        mud->selected_spell = -1;
        break;
    case MENU_CANCEL:
        mud->selected_item_inventory_index = -1;
        mud->selected_spell = -1;
        break;
    }
}

void mudclient_create_top_mouse_menu(mudclient *mud) {
    if (mud->selected_spell >= 0 || mud->selected_item_inventory_index >= 0) {
        strcpy(mud->menu_item_text1[mud->menu_items_count], "Cancel");
        strcpy(mud->menu_item_text2[mud->menu_items_count], "");
        mud->menu_type[mud->menu_items_count] = 4000;
        mud->menu_items_count++;
    }

    for (int i = 0; i < mud->menu_items_count; i++) {
        mud->menu_indices[i] = i;
    }

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

    if (mud->menu_items_count > 20) {
        mud->menu_items_count = 20;
    }

    if (mud->menu_items_count > 0) {
        int index = -1;

        for (int i = 0; i < mud->menu_items_count; i++) {
            if (strlen(mud->menu_item_text2[mud->menu_indices[i]]) <= 0) {
                continue;
            }

            index = i;
            break;
        }

        // TODO better lengths
        char menu_text[255] = {0};

        if ((mud->selected_item_inventory_index >= 0 ||
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

        if (strlen(menu_text) > 0) {
            surface_draw_string(mud->surface, menu_text, 6, 14, 1, YELLOW);
        }

        if (!mud->settings_mouse_button_one && mud->mouse_button_click == 1 ||
            mud->settings_mouse_button_one && mud->mouse_button_click == 1 &&
                mud->menu_items_count == 1) {
            mudclient_menu_item_click(mud, mud->menu_indices[0]);
            mud->mouse_button_click = 0;
            return;
        }

        if (!mud->settings_mouse_button_one && mud->mouse_button_click == 2 ||
            mud->settings_mouse_button_one && mud->mouse_button_click == 1) {
            mud->menu_height = (mud->menu_items_count + 1) * 15;
            mud->menu_width = surface_text_width("Choose option", 1) + 5;

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

            if (mud->menu_x + mud->menu_width > 510) {
                mud->menu_x = 510 - mud->menu_width;
            }

            if (mud->menu_y + mud->menu_height > 315) {
                mud->menu_y = 315 - mud->menu_height;
            }

            mud->mouse_button_click = 0;
        }
    }
}

void mudclient_create_right_click_menu(mudclient *mud) {
    int wilderness_depth =
        2203 - (mud->local_region_y + mud->plane_height + mud->region_y);

    if (mud->local_region_x + mud->plane_width + mud->region_x >= 2640) {
        wilderness_depth = -50;
    }

    int selected_face = -1;

    for (int i = 0; i < mud->object_count; i++) {
        mud->object_already_in_menu[i] = 0;
    }

    for (int i = 0; i < mud->wall_object_count; i++) {
        mud->wall_object_already_in_menu[i] = 0;
    }

    int picked_count = mud->scene->mouse_picked_count;
    GameModel **picked_models = mud->scene->mouse_picked_models;
    int *picked_faces = mud->scene->mouse_picked_faces;

    for (int i = 0; i < picked_count; i++) {
        if (mud->menu_items_count > 200) {
            break;
        }

        int face = picked_faces[i];
        GameModel *game_model = picked_models[i];

        if (game_model->face_tag[face] <= 65535 ||
            game_model->face_tag[face] >= 200000 &&
                game_model->face_tag[face] <= 300000) {
            if (game_model == mud->scene->view) {
                int index = game_model->face_tag[face] % 10000;
                int type = game_model->face_tag[face] / 10000;

                if (type == 1) {
                    int level_difference = 0;

                    if (mud->local_player->level > 0 &&
                        mud->players[index]->level > 0) {
                        level_difference = mud->local_player->level -
                                           mud->players[index]->level;
                    }

                    char colour[6] = {0};
                    get_level_difference_colour(level_difference, colour);

                    char level_text[26] = {0};

                    sprintf(level_text, " %s(level-%d)", colour,
                            mud->players[index]->level);

                    if (mud->selected_spell >= 0) {
                        if (game_data_spell_type[mud->selected_spell] == 1 ||
                            game_data_spell_type[mud->selected_spell] == 2) {
                            sprintf(mud->menu_item_text1[mud->menu_items_count],
                                    "Cast %s on",
                                    game_data_spell_name[mud->selected_spell]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@whi@%s%s", mud->players[index]->name,
                                    level_text);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_CAST_PLAYER;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->players[index]->current_x;

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->players[index]->current_y;

                            mud->menu_index[mud->menu_items_count] =
                                mud->players[index]->server_index;

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->selected_spell;

                            mud->menu_items_count++;
                        }
                    } else if (mud->selected_item_inventory_index >= 0) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Use %s with", mud->selected_item_name);

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@whi@%s%s", mud->players[index]->name,
                                level_text);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_USEWITH_PLAYER;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->players[index]->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->players[index]->current_y;

                        mud->menu_index[mud->menu_items_count] =
                            mud->players[index]->server_index;

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

                            if (level_difference >= 0 && level_difference < 5) {
                                mud->menu_type[mud->menu_items_count] =
                                    MENU_PLAYER_ATTACK1;
                            } else {
                                mud->menu_type[mud->menu_items_count] =
                                    MENU_PLAYER_ATTACK2;
                            }

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->players[index]->current_x;

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->players[index]->current_y;

                            mud->menu_index[mud->menu_items_count] =
                                mud->players[index]->server_index;

                            mud->menu_items_count++;
                        } else if (mud->members) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   "Duel with");

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@whi@%s%s", mud->players[index]->name,
                                    level_text);

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->players[index]->current_x;

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->players[index]->current_y;

                            mud->menu_type[mud->menu_items_count] =
                                MENU_PLAYER_DUEL;

                            mud->menu_index[mud->menu_items_count] =
                                mud->players[index]->server_index;

                            mud->menu_items_count++;
                        }

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Trade with");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@whi@%s%s", mud->players[index]->name,
                                level_text);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_PLAYER_TRADE;

                        mud->menu_index[mud->menu_items_count] =
                            mud->players[index]->server_index;

                        mud->menu_items_count++;

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Follow");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@whi@%s%s", mud->players[index]->name,
                                level_text);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_PLAYER_FOLLOW;

                        mud->menu_index[mud->menu_items_count] =
                            mud->players[index]->server_index;

                        mud->menu_items_count++;
                    }
                } else if (type == 2) {
                    if (mud->selected_spell >= 0) {
                        if (game_data_spell_type[mud->selected_spell] == 3) {
                            sprintf(mud->menu_item_text1[mud->menu_items_count],
                                    "Cast %s on",
                                    game_data_spell_name[mud->selected_spell]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@lre@%s",
                                    game_data_item_name
                                        [mud->ground_item_id[index]]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_CAST_GROUNDITEM;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->ground_item_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->ground_item_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->ground_item_id[index];

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->selected_spell;

                            mud->menu_items_count++;
                        }
                    } else if (mud->selected_item_inventory_index >= 0) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Use %s with", mud->selected_item_name);

                        sprintf(
                            mud->menu_item_text2[mud->menu_items_count],
                            "@lre@%s",
                            game_data_item_name[mud->ground_item_id[index]]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_USEWITH_GROUNDITEM;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->ground_item_x[index];

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->ground_item_y[index];

                        mud->menu_index[mud->menu_items_count] =
                            mud->ground_item_id[index];

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->selected_item_inventory_index;

                        mud->menu_items_count++;
                    } else {
                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Take");

                        sprintf(
                            mud->menu_item_text2[mud->menu_items_count],
                            "@lre@%s",
                            game_data_item_name[mud->ground_item_id[index]]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_GROUNDITEM_TAKE;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->ground_item_x[index];

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->ground_item_y[index];

                        mud->menu_index[mud->menu_items_count] =
                            mud->ground_item_id[index];

                        mud->menu_items_count++;

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Examine");

                        sprintf(
                            mud->menu_item_text2[mud->menu_items_count],
                            "@lre@%s",
                            game_data_item_name[mud->ground_item_id[index]]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_GROUNDITEM_EXAMINE;

                        mud->menu_index[mud->menu_items_count] =
                            mud->ground_item_id[index];

                        mud->menu_items_count++;
                    }
                } else if (type == 3) {
                    char level_text[26] = {0};
                    int level_difference = -1;
                    int id = mud->npcs[index]->npc_id;

                    if (game_data_npc_attackable[id] > 0) {
                        int npc_level = (game_data_npc_attack[id] +
                                         game_data_npc_defense[id] +
                                         game_data_npc_strength[id] +
                                         game_data_npc_hits[id]) /
                                        4;

                        int player_level = (mud->player_stat_base[0] +
                                            mud->player_stat_base[1] +
                                            mud->player_stat_base[2] +
                                            mud->player_stat_base[3] + 27) /
                                           4;

                        level_difference = player_level - npc_level;

                        char colour[6] = "@yel@";
                        get_level_difference_colour(level_difference, colour);

                        sprintf(level_text, " %s(level-%d)", colour, npc_level);
                    }

                    if (mud->selected_spell >= 0) {
                        if (game_data_spell_type[mud->selected_spell] == 2) {
                            sprintf(mud->menu_item_text1[mud->menu_items_count],
                                    "Cast %s on",
                                    game_data_spell_name[mud->selected_spell]);

                            sprintf(
                                mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s",
                                game_data_npc_name[mud->npcs[index]->npc_id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_CAST_NPC;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->npcs[index]->current_x;

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->npcs[index]->current_y;

                            mud->menu_index[mud->menu_items_count] =
                                mud->npcs[index]->server_index;

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->selected_spell;

                            mud->menu_items_count++;
                        }
                    } else if (mud->selected_item_inventory_index >= 0) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Use %s with", mud->selected_item_name);

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s",
                                game_data_npc_name[mud->npcs[index]->npc_id]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_USEWITH_NPC;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->npcs[index]->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->npcs[index]->current_y;

                        mud->menu_index[mud->menu_items_count] =
                            mud->npcs[index]->server_index;

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->selected_item_inventory_index;

                        mud->menu_items_count++;
                    } else {
                        if (game_data_npc_attackable[id] > 0) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   "Attack");

                            sprintf(
                                mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s%s",
                                game_data_npc_name[mud->npcs[index]->npc_id],
                                level_text);

                            if (level_difference >= 0) {
                                mud->menu_type[mud->menu_items_count] =
                                    MENU_NPC_ATTACK1;
                            } else {
                                mud->menu_type[mud->menu_items_count] =
                                    MENU_NPC_ATTACK2;
                            }

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->npcs[index]->current_x;

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->npcs[index]->current_y;

                            mud->menu_index[mud->menu_items_count] =
                                mud->npcs[index]->server_index;

                            mud->menu_items_count++;
                        }

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Talk-to");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s",
                                game_data_npc_name[mud->npcs[index]->npc_id]);

                        mud->menu_type[mud->menu_items_count] = MENU_NPC_TALK;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->npcs[index]->current_x;

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->npcs[index]->current_y;

                        mud->menu_index[mud->menu_items_count] =
                            mud->npcs[index]->server_index;

                        mud->menu_items_count++;

                        if (strlen(game_data_npc_command[id]) > 0) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   game_data_npc_command[id]);

                            sprintf(
                                mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s",
                                game_data_npc_name[mud->npcs[index]->npc_id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_NPC_COMMAND;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->npcs[index]->current_x;

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->npcs[index]->current_y;

                            mud->menu_index[mud->menu_items_count] =
                                mud->npcs[index]->server_index;

                            mud->menu_items_count++;
                        }

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Examine");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@yel@%s",
                                game_data_npc_name[mud->npcs[index]->npc_id]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_NPC_EXAMINE;

                        mud->menu_index[mud->menu_items_count] =
                            mud->npcs[index]->npc_id;

                        mud->menu_items_count++;
                    }
                }
            } else if (game_model && game_model->key >= 10000) {
                int index = game_model->key - 10000;
                int id = mud->wall_object_id[index];

                if (!mud->wall_object_already_in_menu[index]) {
                    if (mud->selected_spell >= 0) {
                        if (game_data_spell_type[mud->selected_spell] == 4) {
                            sprintf(mud->menu_item_text1[mud->menu_items_count],
                                    "Cast %s on",
                                    game_data_spell_name[mud->selected_spell]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@cya@%s", game_data_wall_object_name[id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_CAST_WALLOBJECT;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->wall_object_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->wall_object_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->wall_object_direction[index];

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->selected_spell;

                            mud->menu_items_count++;
                        }
                    } else if (mud->selected_item_inventory_index >= 0) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Use %s with", mud->selected_item_name);

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@cya@%s", game_data_wall_object_name[id]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_USEWITH_WALLOBJECT;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->wall_object_x[index];

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->wall_object_y[index];

                        mud->menu_index[mud->menu_items_count] =
                            mud->wall_object_direction[index];

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->selected_item_inventory_index;

                        mud->menu_items_count++;
                    } else {
                        if (strncasecmp(game_data_wall_object_command1[id],
                                        "WalkTo", 6) != 0) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   game_data_wall_object_command1[id]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@cya@%s", game_data_wall_object_name[id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_WALL_OBJECT_COMMAND1;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->wall_object_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->wall_object_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->wall_object_direction[index];

                            mud->menu_items_count++;
                        }

                        if (strncasecmp(game_data_wall_object_command1[id],
                                        "Examine", 7) != 0) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   game_data_wall_object_command2[id]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@cya@%s", game_data_wall_object_name[id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_WALL_OBJECT_COMMAND2;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->wall_object_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->wall_object_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->wall_object_direction[index];

                            mud->menu_items_count++;
                        }

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Examine");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@cya@%s", game_data_wall_object_name[id]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_WALL_OBJECT_EXAMINE;

                        mud->menu_index[mud->menu_items_count] = id;
                        mud->menu_items_count++;
                    }

                    mud->wall_object_already_in_menu[index] = 1;
                }
            } else if (game_model && game_model->key >= 0) {
                int index = game_model->key;
                int id = mud->object_id[index];

                if (!mud->object_already_in_menu[index]) {
                    if (mud->selected_spell >= 0) {
                        if (game_data_spell_type[mud->selected_spell] == 5) {
                            sprintf(mud->menu_item_text1[mud->menu_items_count],
                                    "Cast %s on",
                                    game_data_spell_name[mud->selected_spell]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@cya@%s", game_data_object_name[id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_CAST_OBJECT;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->object_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->object_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->object_direction[index];

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->object_id[index];

                            mud->menu_target_index[mud->menu_items_count] =
                                mud->selected_spell;

                            mud->menu_items_count++;
                        }
                    } else if (mud->selected_item_inventory_index >= 0) {
                        sprintf(mud->menu_item_text1[mud->menu_items_count],
                                "Use %s with", mud->selected_item_name);

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@cya@%s", game_data_object_name[id]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_USEWITH_OBJECT;

                        mud->menu_item_x[mud->menu_items_count] =
                            mud->object_x[index];

                        mud->menu_item_y[mud->menu_items_count] =
                            mud->object_y[index];

                        mud->menu_index[mud->menu_items_count] =
                            mud->object_direction[index];

                        mud->menu_source_index[mud->menu_items_count] =
                            mud->object_id[index];

                        mud->menu_target_index[mud->menu_items_count] =
                            mud->selected_item_inventory_index;

                        mud->menu_items_count++;
                    } else {
                        if (strncasecmp(game_data_object_command1[id], "WalkTo",
                                        6) != 0) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   game_data_object_command1[id]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@cya@%s", game_data_object_name[id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_OBJECT_COMMAND1;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->object_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->object_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->object_direction[index];

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->object_id[index];

                            mud->menu_items_count++;
                        }

                        if (strncasecmp(game_data_object_command2[id],
                                        "Examine", 7) != 0) {
                            strcpy(mud->menu_item_text1[mud->menu_items_count],
                                   game_data_object_command2[id]);

                            sprintf(mud->menu_item_text2[mud->menu_items_count],
                                    "@cya@%s", game_data_object_name[id]);

                            mud->menu_type[mud->menu_items_count] =
                                MENU_OBJECT_COMMAND2;

                            mud->menu_item_x[mud->menu_items_count] =
                                mud->object_x[index];

                            mud->menu_item_y[mud->menu_items_count] =
                                mud->object_y[index];

                            mud->menu_index[mud->menu_items_count] =
                                mud->object_direction[index];

                            mud->menu_source_index[mud->menu_items_count] =
                                mud->object_id[index];

                            mud->menu_items_count++;
                        }

                        strcpy(mud->menu_item_text1[mud->menu_items_count],
                               "Examine");

                        sprintf(mud->menu_item_text2[mud->menu_items_count],
                                "@cya@%s", game_data_object_name[id]);

                        mud->menu_type[mud->menu_items_count] =
                            MENU_OBJECT_EXAMINE;
                        mud->menu_index[mud->menu_items_count] = id;
                        mud->menu_items_count++;
                    }

                    mud->object_already_in_menu[index] = 1;
                }
            } else {
                if (face >= 0) {
                    face = game_model->face_tag[face] - 200000;
                }

                if (face >= 0) {
                    selected_face = face;
                }
            }
        }
    }

    if (mud->selected_spell >= 0 &&
        game_data_spell_type[mud->selected_spell] <= 1) {
        sprintf(mud->menu_item_text1[mud->menu_items_count], "Cast %s on self",
                game_data_spell_name[mud->selected_spell]);

        strcpy(mud->menu_item_text2[mud->menu_items_count], "");

        mud->menu_type[mud->menu_items_count] = MENU_CAST_SELF;
        mud->menu_index[mud->menu_items_count] = mud->selected_spell;
        mud->menu_items_count++;
    }

    if (selected_face != -1) {
        if (mud->selected_spell >= 0) {
            if (game_data_spell_type[mud->selected_spell] == 6) {
                sprintf(mud->menu_item_text1[mud->menu_items_count],
                        "Cast %s on ground",
                        game_data_spell_name[mud->selected_spell]);

                strcpy(mud->menu_item_text2[mud->menu_items_count], "");

                mud->menu_type[mud->menu_items_count] = MENU_CAST_GROUND;

                mud->menu_item_x[mud->menu_items_count] =
                    mud->world->local_x[selected_face];

                mud->menu_item_y[mud->menu_items_count] =
                    mud->world->local_y[selected_face];

                mud->menu_index[mud->menu_items_count] = mud->selected_spell;
                mud->menu_items_count++;
                return;
            }
        } else if (mud->selected_item_inventory_index < 0) {
            strcpy(mud->menu_item_text1[mud->menu_items_count], "Walk here");
            strcpy(mud->menu_item_text2[mud->menu_items_count], "");

            mud->menu_type[mud->menu_items_count] = MENU_WALK;

            mud->menu_item_x[mud->menu_items_count] =
                mud->world->local_x[selected_face];

            mud->menu_item_y[mud->menu_items_count] =
                mud->world->local_y[selected_face];

            mud->menu_items_count++;
        }
    }
}

void mudclient_draw_right_click_menu(mudclient *mud) {
    if (mud->mouse_button_click != 0) {
        for (int i = 0; i < mud->menu_items_count; i++) {
            int entry_x = mud->menu_x + 2;
            int entry_y = mud->menu_y + 27 + i * 15;

            if (mud->mouse_x <= entry_x - 2 || mud->mouse_y <= entry_y - 12 ||
                mud->mouse_y >= entry_y + 4 ||
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
        return;
    }

    surface_draw_box_alpha(mud->surface, mud->menu_x, mud->menu_y,
                           mud->menu_width, mud->menu_height, GREY_D0, 160);

    surface_draw_string(mud->surface, "Choose option", mud->menu_x + 2,
                        mud->menu_y + 12, 1, CYAN);

    for (int i = 0; i < mud->menu_items_count; i++) {
        int entry_x = mud->menu_x + 2;
        int entry_y = mud->menu_y + 27 + i * 15;
        int text_colour = WHITE;

        if (mud->mouse_x > entry_x - 2 && mud->mouse_y > entry_y - 12 &&
            mud->mouse_y < entry_y + 4 &&
            mud->mouse_x < (entry_x - 3) + mud->menu_width) {
            text_colour = YELLOW;
        }

        char *menu_item_text1 = mud->menu_item_text1[mud->menu_indices[i]];
        char *menu_item_text2 = mud->menu_item_text2[mud->menu_indices[i]];

        char combined[strlen(menu_item_text1) + strlen(menu_item_text2) + 2];
        sprintf(combined, "%s %s", menu_item_text1, menu_item_text2);

        surface_draw_string(mud->surface, combined, entry_x, entry_y, 1,
                            text_colour);
    }
}
