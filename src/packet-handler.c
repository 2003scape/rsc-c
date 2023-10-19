#include "packet-handler.h"

void mudclient_update_ground_item_models(mudclient *mud) {
    for (int i = 0; i < GROUND_ITEMS_MAX; i++) {
        if (mud->ground_item_model[i] == NULL) {
            continue;
        }

        scene_remove_model(mud->scene, mud->ground_item_model[i]);
#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
        game_model_destroy(mud->ground_item_model[i]);
#endif
        free(mud->ground_item_model[i]);

        mud->ground_item_model[i] = NULL;
    }

    if (!mud->options->ground_item_models) {
        return;
    }

    for (int i = 0; i < mud->ground_item_count; i++) {
        int item_id = mud->ground_item_id[i];

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        GameModel *original_model = mud->item_models[item_id];

        if (original_model == NULL) {
            continue;
        }

        GameModel *model = game_model_copy(original_model);
#else
        int sprite_id = game_data_item_sprite[item_id];
        GameModel *original_model = mud->item_models[sprite_id];

        if (original_model == NULL) {
            continue;
        }

        GameModel *model = game_model_copy(original_model);

        int mask_colour = game_data_item_mask[item_id];

        if (mask_colour != 0) {
            game_model_mask_faces(model, model->face_fill_back, mask_colour);
            game_model_mask_faces(model, model->face_fill_front, mask_colour);
        }
#endif

        model->key = i + GROUND_ITEM_FACE_TAG;

        int area_x = mud->ground_item_x[i];
        int area_y = mud->ground_item_y[i];
        int model_x = ((area_x + area_x + 1) * MAGIC_LOC) / 2;
        int model_y = ((area_y + area_y + 1) * MAGIC_LOC) / 2;

        game_model_translate(
            model, model_x,
            -(world_get_elevation(mud->world, model_x, model_y) +
              mud->ground_item_z[i]) -
                10,
            model_y);

        game_model_set_light_from6(model, 1, 48, 48, -50, -10, -50);

        scene_add_model(mud->scene, model);

        mud->ground_item_model[i] = model;
    }

#ifdef RENDER_3DS_GL
    if (mud->ground_item_count > 0) {
        game_model_gl_buffer_models(
            &mud->scene->gl_item_buffers, &mud->scene->gl_item_buffer_length,
            mud->ground_item_model, mud->ground_item_count);
    }
#endif
}

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
void mudclient_gl_update_wall_models(mudclient *mud) {
    int vbo_offset = 0;
    int ebo_offset = 0;

    for (int i = 0; i < mud->wall_object_count; i++) {
        GameModel *game_model = mud->wall_object_model[i];

        game_model->gl_buffer = mud->scene->gl_wall_buffers[0];
        game_model->gl_ebo_length = 6;

        game_model->gl_vbo_offset = vbo_offset;
        game_model->gl_ebo_offset = ebo_offset;

        game_model_gl_buffer_arrays(game_model, &vbo_offset, &ebo_offset);

        vbo_offset += 4;
        ebo_offset += 6;
    }
}
#endif

void mudclient_packet_tick(mudclient *mud) {
    uint64_t timestamp = get_ticks();

    if (packet_stream_has_packet(mud->packet_stream)) {
        mud->packet_last_read = timestamp;
    }

    if (timestamp - mud->packet_last_read > 5000) {
        mud->packet_last_read = timestamp;
        packet_stream_new_packet(mud->packet_stream, CLIENT_PING);
        packet_stream_send_packet(mud->packet_stream);
    }

    packet_stream_write_packet(mud->packet_stream, 20);

    int size =
        packet_stream_read_packet(mud->packet_stream, mud->incoming_packet);

    if (size <= 0) {
        return;
    }

    int8_t *data = mud->incoming_packet;
    SERVER_OPCODE opcode = data[0] & 0xff; /* TODO isaac */

    switch (opcode) {
    case SERVER_WORLD_INFO:
        mud->loading_area = 1;
        mud->local_player_server_index = get_unsigned_short(data, 1, size);
        mud->plane_width = get_unsigned_short(data, 3, size);
        mud->plane_height = get_unsigned_short(data, 5, size);
        mud->plane_index = get_unsigned_short(data, 7, size);
        mud->plane_multiplier = get_unsigned_short(data, 9, size);
        mud->plane_height -= mud->plane_index * mud->plane_multiplier;
        break;
    case SERVER_REGION_PLAYERS: {
        mud->known_player_count = mud->player_count;

        for (int i = 0; i < mud->known_player_count; i++) {
            mud->known_players[i] = mud->players[i];
        }

        int offset = 8;

        mud->local_region_x = get_bit_mask(data, offset, size, 11);
        offset += 11;

        mud->local_region_y = get_bit_mask(data, offset, size, 13);
        offset += 13;

        int sprite = get_bit_mask(data, offset, size, 4);
        offset += 4;

        int has_loaded_region = mudclient_load_next_region(
            mud, mud->local_region_x, mud->local_region_y);

        mud->local_region_x -= mud->region_x;
        mud->local_region_y -= mud->region_y;

        int player_x = mud->local_region_x * MAGIC_LOC + 64;
        int player_y = mud->local_region_y * MAGIC_LOC + 64;

        if (has_loaded_region) {
            mud->local_player->waypoint_current = 0;
            mud->local_player->moving_step = 0;

            mud->local_player->current_x = mud->local_player->waypoints_x[0] =
                player_x;

            mud->local_player->current_y = mud->local_player->waypoints_y[0] =
                player_y;
        }

        mud->player_count = 0;

        mud->local_player = mudclient_add_player(
            mud, mud->local_player_server_index, player_x, player_y, sprite);

        int length = get_bit_mask(data, offset, size, 8);
        offset += 8;

        for (int i = 0; i < length; i++) {
            GameCharacter *player = mud->known_players[i + 1];
            int has_updated = get_bit_mask(data, offset, size, 1);

            offset++;

            if (has_updated != 0) {
                int update_type = get_bit_mask(data, offset, size, 1);
                offset++;

                if (update_type == 0) {
                    int sprite = get_bit_mask(data, offset, size, 3);
                    offset += 3;

                    int waypoint_current = player->waypoint_current;
                    int player_x = player->waypoints_x[waypoint_current];
                    int player_y = player->waypoints_y[waypoint_current];

                    if (sprite == 2 || sprite == 1 || sprite == 3) {
                        player_x += MAGIC_LOC;
                    }

                    if (sprite == 6 || sprite == 5 || sprite == 7) {
                        player_x -= MAGIC_LOC;
                    }

                    if (sprite == 4 || sprite == 3 || sprite == 5) {
                        player_y += MAGIC_LOC;
                    }

                    if (sprite == 0 || sprite == 1 || sprite == 7) {
                        player_y -= MAGIC_LOC;
                    }

                    player->next_animation = sprite;

                    player->waypoint_current = waypoint_current =
                        (waypoint_current + 1) % 10;

                    player->waypoints_x[waypoint_current] = player_x;
                    player->waypoints_y[waypoint_current] = player_y;
                } else {
                    int sprite = get_bit_mask(data, offset, size, 4);

                    if ((sprite & 12) == 12) {
                        offset += 2;
                        continue;
                    }

                    player->next_animation =
                        get_bit_mask(data, offset, size, 4);
                    offset += 4;
                }
            }

            mud->players[mud->player_count++] = player;
        }

        int player_count = 0;

        while (offset + 24 < size * 8) {
            int server_index = get_bit_mask(data, offset, size, 11);
            offset += 11;

            int area_x = get_bit_mask(data, offset, size, 5);
            offset += 5;

            if (area_x > 15) {
                area_x -= 32;
            }

            int area_y = get_bit_mask(data, offset, size, 5);
            offset += 5;

            if (area_y > 15) {
                area_y -= 32;
            }

            int sprite = get_bit_mask(data, offset, size, 4);
            offset += 4;

            int is_player_known = get_bit_mask(data, offset, size, 1);
            offset++;

            int x = (mud->local_region_x + area_x) * MAGIC_LOC + 64;
            int y = (mud->local_region_y + area_y) * MAGIC_LOC + 64;

            mudclient_add_player(mud, server_index, x, y, sprite);

            if (is_player_known == 0) {
                mud->player_server_indexes[player_count++] = server_index;
            }
        }

        if (player_count > 0) {
            packet_stream_new_packet(mud->packet_stream, CLIENT_KNOWN_PLAYERS);
            packet_stream_put_short(mud->packet_stream, player_count);

            for (int i = 0; i < player_count; i++) {
                GameCharacter *player =
                    mud->player_server[mud->player_server_indexes[i]];

                packet_stream_put_short(mud->packet_stream,
                                        player->server_index);

                packet_stream_put_short(mud->packet_stream, player->server_id);
            }

            packet_stream_send_packet(mud->packet_stream);
        }
        break;
    }
    case SERVER_REGION_PLAYER_UPDATE: {
        int length = get_unsigned_short(data, 1, size);
        int offset = 3;

        for (int i = 0; i < length; i++) {
            int player_index = get_unsigned_short(data, offset, size);
            offset += 2;

            GameCharacter *player = mud->player_server[player_index];

            int update_type = get_unsigned_byte(data, offset++, size);
            if (update_type == 0) {
                /* action bubble with an item in it */
                int item_id = get_unsigned_short(data, offset, size);
                offset += 2;

                if (player != NULL) {
                    player->bubble_timeout = 150;
                    player->bubble_item = item_id;
                }
            } else if (update_type == 1) {
                /* chat */
                int message_length = get_unsigned_byte(data, offset++, size);

                if (player != NULL) {
                    char *message =
                        chat_message_decode(data, offset, message_length);

                    /*if (mud->options->word_filter) {
                        message = word_filter_filter(message);
                    }*/

                    int ignored = 0;

                    for (int i = 0; i < mud->ignore_list_count; i++) {
                        if (mud->ignore_list[i] == player->encoded_username) {
                            ignored = 1;
                            break;
                        }
                    }

                    if (!ignored) {
                        player->message_timeout = 150;
                        strcpy(player->message, message);

                        char formatted_message[strlen(player->name) +
                                               strlen(player->message) + 3];

                        sprintf(formatted_message, "%s: %s", player->name,
                                player->message);

                        mudclient_show_message(mud, formatted_message,
                                               MESSAGE_TYPE_CHAT);
                    }
                }

                offset += message_length;
            } else if (update_type == 2) {
                /* combat damage and hp */
                int damage = get_unsigned_byte(data, offset++, size);
                int current = get_unsigned_byte(data, offset++, size);
                int max = get_unsigned_byte(data, offset++, size);

                if (player != NULL) {
                    player->damage_taken = damage;
                    player->current_hits = current;
                    player->max_hits = max;
                    player->combat_timer = 200;

                    if (player == mud->local_player) {
                        mud->player_skill_current[SKILL_HITS] = current;
                        mud->player_skill_base[SKILL_HITS] = max;
                        mud->show_dialog_welcome = 0;
                        mud->show_dialog_server_message = 0;
                    }
                }
            } else if (update_type == 3) {
                /* new incoming projectile to npc */
                int projectile_sprite = get_unsigned_short(data, offset, size);
                offset += 2;

                int npc_index = get_unsigned_short(data, offset, size);
                offset += 2;

                if (player != NULL) {
                    player->incoming_projectile_sprite = projectile_sprite;
                    player->attacking_npc_server_index = npc_index;
                    player->attacking_player_server_index = -1;
                    player->projectile_range = PROJECTILE_RANGE_MAX;
                }
            } else if (update_type == 4) {
                /* new incoming projectile from player */
                int projectile_sprite = get_unsigned_short(data, offset, size);
                offset += 2;

                int opponent_index = get_unsigned_short(data, offset, size);
                offset += 2;

                if (player != NULL) {
                    player->incoming_projectile_sprite = projectile_sprite;
                    player->attacking_player_server_index = opponent_index;
                    player->attacking_npc_server_index = -1;
                    player->projectile_range = PROJECTILE_RANGE_MAX;
                }
            } else if (update_type == 5) {
                /* player appearance update */
                if (player != NULL) {
                    player->server_id = get_unsigned_short(data, offset, size);
                    offset += 2;

                    player->encoded_username = get_unsigned_long(data, offset, size);
                    offset += 8;

                    decode_username(player->encoded_username, player->name);

                    int equipped_count = get_unsigned_byte(data, offset, size);
                    offset++;

                    for (int j = 0; j < equipped_count; j++) {
                        player->animations[j] =
                            get_unsigned_byte(data, offset++, size);
                    }

                    for (int j = equipped_count; j < ANIMATION_COUNT; j++) {
                        player->animations[j] = 0;
                    }

                    player->hair_colour = get_unsigned_byte(data, offset++, size);
                    player->top_colour = get_unsigned_byte(data, offset++, size);
                    player->bottom_colour = get_unsigned_byte(data, offset++, size);
                    player->skin_colour = get_unsigned_byte(data, offset++, size);
                    player->level = get_unsigned_byte(data, offset++, size);
                    player->skull_visible = get_unsigned_byte(data, offset++, size);
                } else {
                    offset += 14;

                    int unused = get_unsigned_byte(data, offset, size);
                    offset += unused + 1;
                }
            } else if (update_type == 6) {
                /* public chat */
                int message_length = get_unsigned_byte(data, offset++, size);

                if (player != NULL) {
                    char *message =
                        chat_message_decode(data, offset, message_length);

                    player->message_timeout = 150;
                    strcpy(player->message, message);

                    if (player == mud->local_player) {
                        char formatted_message[strlen(player->name) +
                                               strlen(player->message) + 3];

                        sprintf(formatted_message, "%s: %s", player->name,
                                player->message);

                        mudclient_show_message(mud, formatted_message,
                                               MESSAGE_TYPE_QUEST);
                    }
                }

                offset += message_length;
            }
        }
        break;
    }
    case SERVER_REGION_OBJECTS: {
        for (int offset = 1; offset < size;) {
            if (get_unsigned_byte(data, offset, size) == 255) {
                /* remove the object */
                int index = 0;
                int l_x = (mud->local_region_x +
                    get_signed_byte(data, offset + 1, size)) / 8;
                int l_y = (mud->local_region_y +
                    get_signed_byte(data, offset + 2, size)) / 8;

                offset += 3;

                for (int i = 0; i < mud->object_count; i++) {
                    int o_x = (mud->object_x[i] / 8) - l_x;
                    int o_y = (mud->object_y[i] / 8) - l_y;

                    if (o_x != 0 || o_y != 0) {
                        if (i != index) {
                            mud->object_model[index] = mud->object_model[i];
                            mud->object_model[index]->key = index;
                            mud->object_x[index] = mud->object_x[i];
                            mud->object_y[index] = mud->object_y[i];
                            mud->object_id[index] = mud->object_id[i];

                            mud->object_direction[index] =
                                mud->object_direction[i];
                        }

                        index++;
                    } else {
                        scene_remove_model(mud->scene, mud->object_model[i]);

                        world_remove_object(mud->world, mud->object_x[i],
                                            mud->object_y[i],
                                            mud->object_id[i]);

#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
                        game_model_destroy(mud->object_model[i]);
#endif
                        free(mud->object_model[i]);
                        mud->object_model[i] = NULL;
                    }
                }

                mud->object_count = index;
            } else {
                int object_id = get_unsigned_short(data, offset, size);
                offset += 2;

                int area_x = mud->local_region_x +
                    get_signed_byte(data, offset++, size);
                int area_y = mud->local_region_y +
                    get_signed_byte(data, offset++, size);
                int object_index = 0;

                for (int i = 0; i < mud->object_count; i++) {
                    if (mud->object_x[i] != area_x ||
                        mud->object_y[i] != area_y) {
                        if (i != object_index) {
                            mud->object_model[object_index] =
                                mud->object_model[i];

                            mud->object_model[object_index]->key = object_index;
                            mud->object_x[object_index] = mud->object_x[i];
                            mud->object_y[object_index] = mud->object_y[i];
                            mud->object_id[object_index] = mud->object_id[i];

                            mud->object_direction[object_index] =
                                mud->object_direction[i];
                        }

                        object_index++;
                    } else {
                        scene_remove_model(mud->scene, mud->object_model[i]);

                        world_remove_object(mud->world, mud->object_x[i],
                                            mud->object_y[i],
                                            mud->object_id[i]);

#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
                        game_model_destroy(mud->object_model[i]);
#endif
                        free(mud->object_model[i]);
                        mud->object_model[i] = NULL;
                    }
                }

                mud->object_count = object_index;

                if (object_id != 60000) {
                    int tile_direction =
                        world_get_tile_direction(mud->world, area_x, area_y);

                    int width = 0;
                    int height = 0;

                    if (tile_direction == 0 || tile_direction == 4) {
                        width = game_data_object_width[object_id];
                        height = game_data_object_height[object_id];
                    } else {
                        height = game_data_object_width[object_id];
                        width = game_data_object_height[object_id];
                    }

                    int model_x = ((area_x + area_x + width) * MAGIC_LOC) / 2;
                    int model_y = ((area_y + area_y + height) * MAGIC_LOC) / 2;
                    int model_index = game_data_object_model_index[object_id];

                    GameModel *model =
                        game_model_copy(mud->game_models[model_index]);

                    scene_add_model(mud->scene, model);

                    model->key = mud->object_count;

                    game_model_rotate(model, 0, tile_direction * 32, 0);

                    game_model_translate(
                        model, model_x,
                        -world_get_elevation(mud->world, model_x, model_y),
                        model_y);

                    game_model_set_light_from6(model, 1, 48, 48, -50, -10, -50);

                    world_remove_object2(mud->world, area_x, area_y, object_id);

                    if (object_id == WINDMILL_SAILS_ID) {
                        game_model_translate(model, 0, -480, 0);
                    }

                    mud->object_x[mud->object_count] = area_x;
                    mud->object_y[mud->object_count] = area_y;
                    mud->object_id[mud->object_count] = object_id;
                    mud->object_direction[mud->object_count] = tile_direction;
                    mud->object_model[mud->object_count++] = model;
                }
            }
        }

#ifdef RENDER_3DS_GL
        if (mud->object_count > 0) {
            int object_count = mud->object_count + ANIMATED_MODELS_LENGTH + 8;
            GameModel *object_model[object_count];

            memcpy(object_model, mud->object_model,
                   sizeof(GameModel *) * mud->object_count);

            int first_animated_index = 0;

            for (int i = 0; i < ANIMATED_MODELS_LENGTH; i++) {
                int name_length = strlen(animated_models[i]);
                char model_name[name_length + 1];

                strcpy(model_name, animated_models[i]);

                int model_index = game_data_get_model_index(model_name);

                object_model[mud->object_count + i] =
                    mud->game_models[model_index];

                if (model_name[name_length - 1] == '2') {
                    model_name[name_length - 1] = '1';

                    object_model[mud->object_count + ANIMATED_MODELS_LENGTH +
                                 first_animated_index] =
                        mud->game_models[game_data_get_model_index(model_name)];

                    first_animated_index++;
                }
            }

            game_model_gl_buffer_models(
                &mud->scene->gl_game_model_buffers,
                &mud->scene->gl_game_model_buffer_length, object_model,
                object_count);
        }
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        world_gl_update_terrain_buffers(mud->world);
#endif
        break;
    }
    case SERVER_REGION_NPCS: {
        mud->known_npc_count = mud->npc_count;
        mud->npc_count = 0;

        for (int i = 0; i < mud->known_npc_count; i++) {
            mud->known_npcs[i] = mud->npcs[i];
        }

        int offset = 8;

        int length = get_bit_mask(data, offset, size, 8);
        offset += 8;

        for (int i = 0; i < length; i++) {
            GameCharacter *npc = mud->known_npcs[i];
            int has_updated = get_bit_mask(data, offset++, size, 1);

            if (has_updated != 0) {
                int has_moved = get_bit_mask(data, offset++, size, 1);

                if (has_moved == 0) {
                    int sprite = get_bit_mask(data, offset, size, 3);
                    offset += 3;

                    int waypoint_current = npc->waypoint_current;
                    int npc_x = npc->waypoints_x[waypoint_current];
                    int npc_y = npc->waypoints_y[waypoint_current];

                    if (sprite == 2 || sprite == 1 || sprite == 3) {
                        npc_x += MAGIC_LOC;
                    }

                    if (sprite == 6 || sprite == 5 || sprite == 7) {
                        npc_x -= MAGIC_LOC;
                    }

                    if (sprite == 4 || sprite == 3 || sprite == 5) {
                        npc_y += MAGIC_LOC;
                    }

                    if (sprite == 0 || sprite == 1 || sprite == 7) {
                        npc_y -= MAGIC_LOC;
                    }

                    npc->next_animation = sprite;

                    npc->waypoint_current = waypoint_current =
                        (waypoint_current + 1) % 10;

                    npc->waypoints_x[waypoint_current] = npc_x;
                    npc->waypoints_y[waypoint_current] = npc_y;
                } else {
                    int sprite = get_bit_mask(data, offset, size, 4);

                    if ((sprite & 12) == 12) {
                        offset += 2;
                        continue;
                    }

                    npc->next_animation = get_bit_mask(data, offset, size, 4);
                    offset += 4;
                }
            }

            mud->npcs[mud->npc_count++] = npc;
        }

        /* adding new NPCS */
        while (offset + 34 < size * 8) {
            int server_index = get_bit_mask(data, offset, size, 12);
            offset += 12;

            int area_x = get_bit_mask(data, offset, size, 5);
            offset += 5;

            if (area_x > 15) {
                area_x -= 32;
            }

            int area_y = get_bit_mask(data, offset, size, 5);
            offset += 5;

            if (area_y > 15) {
                area_y -= 32;
            }

            int sprite = get_bit_mask(data, offset, size, 4);
            offset += 4;

            int x = (mud->local_region_x + area_x) * MAGIC_LOC + 64;
            int y = (mud->local_region_y + area_y) * MAGIC_LOC + 64;

            int npc_id = get_bit_mask(data, offset, size, 10);
            offset += 10;

            if (npc_id >= game_data_npc_count) {
                npc_id = SHIFTY_MAN_ID;
            }

            mudclient_add_npc(mud, server_index, x, y, sprite, npc_id);
        }
        break;
    }
    case SERVER_REGION_NPC_UPDATE: {
        int length = get_unsigned_short(data, 1, size);

        int offset = 3;

        for (int i = 0; i < length; i++) {
            int server_index = get_unsigned_short(data, offset, size);
            offset += 2;

            GameCharacter *npc = mud->npcs_server[server_index];
            int update_type = get_unsigned_byte(data, offset++, size);

            if (update_type == 1) {
                int target_index = get_unsigned_short(data, offset, size);
                offset += 2;

                int encoded_length = get_unsigned_byte(data, offset++, size);

                if (npc != NULL) {
                    char *message =
                        chat_message_decode(data, offset, encoded_length);

                    npc->message_timeout = 150;
                    strcpy(npc->message, message);

                    if (target_index == mud->local_player->server_index) {
                        char *npc_name = game_data_npc_name[npc->npc_id];

                        char formatted_message[strlen(message) +
                                               strlen(npc_name) + 8];

                        sprintf(formatted_message, "@yel@%s: %s", npc_name,
                                message);

                        mudclient_show_message(mud, formatted_message,
                                               MESSAGE_TYPE_QUEST);
                    }
                }

                offset += encoded_length;
            } else if (update_type == 2) {
                int damage_taken = get_unsigned_byte(data, offset++, size);
                int current_health = get_unsigned_byte(data, offset++, size);
                int max_health = get_unsigned_byte(data, offset++, size);

                if (npc != NULL) {
                    npc->damage_taken = damage_taken;
                    npc->current_hits = current_health;
                    npc->max_hits = max_health;
                    npc->combat_timer = 200;
                }
            }
        }
        break;
    }
    case SERVER_REGION_ENTITY_UPDATE: {
        int length = (size - 1) / 4;

        for (int i = 0; i < length; i++) {
            int delta_x =
                (mud->local_region_x + get_signed_short(data, 1 + i * 4, size)) / 8;

            int delta_y =
                (mud->local_region_y + get_signed_short(data, 3 + i * 4, size)) / 8;

            int entity_count = 0;

            for (int j = 0; j < mud->ground_item_count; j++) {
                int x = (mud->ground_item_x[j] / 8) - delta_x;
                int y = (mud->ground_item_y[j] / 8) - delta_y;

                if (x != 0 || y != 0) {
                    if (j != entity_count) {
                        mud->ground_item_x[entity_count] =
                            mud->ground_item_x[j];

                        mud->ground_item_y[entity_count] =
                            mud->ground_item_y[j];

                        mud->ground_item_id[entity_count] =
                            mud->ground_item_id[j];

                        mud->ground_item_z[entity_count] =
                            mud->ground_item_z[j];
                    }

                    entity_count++;
                }
            }

            mud->ground_item_count = entity_count;
            entity_count = 0;

            for (int j = 0; j < mud->object_count; j++) {
                int x = (mud->object_x[j] / 8) - delta_x;
                int y = (mud->object_y[j] / 8) - delta_y;

                if (x != 0 || y != 0) {
                    if (j != entity_count) {
                        mud->object_model[entity_count] = mud->object_model[j];
                        mud->object_model[entity_count]->key = entity_count;
                        mud->object_x[entity_count] = mud->object_x[j];
                        mud->object_y[entity_count] = mud->object_y[j];
                        mud->object_id[entity_count] = mud->object_id[j];

                        mud->object_direction[entity_count] =
                            mud->object_direction[j];
                    }

                    entity_count++;
                } else {
                    scene_remove_model(mud->scene, mud->object_model[j]);

                    world_remove_object(mud->world, mud->object_x[j],
                                        mud->object_y[j], mud->object_id[j]);

#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
                    game_model_destroy(mud->object_model[j]);
#endif
                    free(mud->object_model[j]);
                    mud->object_model[j] = NULL;
                }
            }

            mud->object_count = entity_count;
            entity_count = 0;

            for (int j = 0; j < mud->wall_object_count; j++) {
                int x = (mud->wall_object_x[j] / 8) - delta_x;
                int y = (mud->wall_object_y[j] / 8) - delta_y;

                if (x != 0 || y != 0) {
                    if (j != entity_count) {
                        mud->wall_object_model[entity_count] =
                            mud->wall_object_model[j];

                        mud->wall_object_model[entity_count]->key =
                            entity_count + 10000;

                        mud->wall_object_x[entity_count] =
                            mud->wall_object_x[j];

                        mud->wall_object_y[entity_count] =
                            mud->wall_object_y[j];

                        mud->wall_object_direction[entity_count] =
                            mud->wall_object_direction[j];

                        mud->wall_object_id[entity_count] =
                            mud->wall_object_id[j];
                    }

                    entity_count++;
                } else {
                    scene_remove_model(mud->scene, mud->wall_object_model[j]);

                    world_remove_wall_object(mud->world, mud->wall_object_x[j],
                                             mud->wall_object_y[j],
                                             mud->wall_object_direction[j],
                                             mud->wall_object_id[j]);

                    game_model_destroy(mud->wall_object_model[j]);
                    free(mud->wall_object_model[j]);
                    mud->wall_object_model[j] = NULL;
                }
            }

            mud->wall_object_count = entity_count;
        }

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        mudclient_gl_update_wall_models(mud);
#endif

        mudclient_update_ground_item_models(mud);
        break;
    }
    case SERVER_REGION_WALL_OBJECTS: {
        for (int offset = 1; offset < size;) {
            if (get_unsigned_byte(data, offset, size) == 255) {
                /* remove the bound */
                int index = 0;
                int l_x = (mud->local_region_x +
                    get_signed_byte(data, offset + 1, size)) / 8;
                int l_y = (mud->local_region_y +
                    get_signed_byte(data, offset + 2, size)) / 8;

                offset += 3;

                for (int i = 0; i < mud->wall_object_count; i++) {
                    int s_x = (mud->wall_object_x[i] / 8) - l_x;
                    int s_y = (mud->wall_object_y[i] / 8) - l_y;

                    if (s_x != 0 || s_y != 0) {
                        if (i != index) {
                            mud->wall_object_model[index] =
                                mud->wall_object_model[i];

                            mud->wall_object_model[index]->key = index + 10000;
                            mud->wall_object_x[index] = mud->wall_object_x[i];
                            mud->wall_object_y[index] = mud->wall_object_y[i];

                            mud->wall_object_direction[index] =
                                mud->wall_object_direction[i];

                            mud->wall_object_id[index] = mud->wall_object_id[i];
                        }

                        index++;
                    } else {
                        scene_remove_model(mud->scene,
                                           mud->wall_object_model[i]);

                        world_remove_wall_object(mud->world,
                                                 mud->wall_object_x[i],
                                                 mud->wall_object_y[i],
                                                 mud->wall_object_direction[i],
                                                 mud->wall_object_id[i]);

                        game_model_destroy(mud->wall_object_model[i]);
                        free(mud->wall_object_model[i]);
                        mud->wall_object_model[i] = NULL;
                    }
                }

                mud->wall_object_count = index;
            } else {
                int id = get_unsigned_short(data, offset, size);
                offset += 2;

                int l_x = mud->local_region_x +
                    get_signed_byte(data, offset++, size);
                int l_y = mud->local_region_y +
                    get_signed_byte(data, offset++, size);
                int direction = get_signed_byte(data, offset++, size);
                int count = 0;

                for (int i = 0; i < mud->wall_object_count; i++) {
                    if (mud->wall_object_x[i] != l_x ||
                        mud->wall_object_y[i] != l_y ||
                        mud->wall_object_direction[i] != direction) {
                        if (i != count) {
                            mud->wall_object_model[count] =
                                mud->wall_object_model[i];

                            mud->wall_object_model[count]->key = count + 10000;
                            mud->wall_object_x[count] = mud->wall_object_x[i];
                            mud->wall_object_y[count] = mud->wall_object_y[i];

                            mud->wall_object_direction[count] =
                                mud->wall_object_direction[i];

                            mud->wall_object_id[count] = mud->wall_object_id[i];
                        }

                        count++;
                    } else {
                        scene_remove_model(mud->scene,
                                           mud->wall_object_model[i]);

                        world_remove_wall_object(mud->world,
                                                 mud->wall_object_x[i],
                                                 mud->wall_object_y[i],
                                                 mud->wall_object_direction[i],
                                                 mud->wall_object_id[i]);

                        game_model_destroy(mud->wall_object_model[i]);
                        free(mud->wall_object_model[i]);
                        mud->wall_object_model[i] = NULL;
                    }
                }

                mud->wall_object_count = count;

                if (id != 65535) {
                    world_set_object_adjacency_from4(mud->world, l_x, l_y,
                                                     direction, id);

                    GameModel *model = mudclient_create_wall_object(
                        mud, l_x, l_y, direction, id, mud->wall_object_count);

                    mud->wall_object_model[mud->wall_object_count] = model;
                    mud->wall_object_x[mud->wall_object_count] = l_x;
                    mud->wall_object_y[mud->wall_object_count] = l_y;
                    mud->wall_object_id[mud->wall_object_count] = id;

                    mud->wall_object_direction[mud->wall_object_count++] =
                        direction;
                }
            }
        }

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        mudclient_gl_update_wall_models(mud);
#endif
        break;
    }
    case SERVER_REGION_GROUND_ITEMS: {
        for (int offset = 1; offset < size;) {
            if (get_unsigned_byte(data, offset, size) == 255) {
                /* remove the item */
                int index = 0;
                int l_x = (mud->local_region_x +
                    get_signed_byte(data, offset + 1, size)) / 8;
                int l_y = (mud->local_region_y +
                    get_signed_byte(data, offset + 2, size)) / 8;

                offset += 3;

                for (int i = 0; i < mud->ground_item_count; i++) {
                    int g_x = (mud->ground_item_x[i] / 8) - l_x;
                    int g_y = (mud->ground_item_y[i] / 8) - l_y;

                    if (g_x != 0 || g_y != 0) {
                        if (i != index) {
                            mud->ground_item_x[index] = mud->ground_item_x[i];
                            mud->ground_item_y[index] = mud->ground_item_y[i];
                            mud->ground_item_id[index] = mud->ground_item_id[i];
                            mud->ground_item_z[index] = mud->ground_item_z[i];
                        }

                        index++;
                    }
                }

                mud->ground_item_count = index;
            } else {
                int item_id = get_unsigned_short(data, offset, size);
                offset += 2;

                int area_x = mud->local_region_x +
                    get_signed_byte(data, offset++, size);
                int area_y = mud->local_region_y +
                    get_signed_byte(data, offset++, size);

                if ((item_id & 32768) == 0) {
                    mud->ground_item_x[mud->ground_item_count] = area_x;
                    mud->ground_item_y[mud->ground_item_count] = area_y;
                    mud->ground_item_id[mud->ground_item_count] = item_id;
                    mud->ground_item_z[mud->ground_item_count] = 0;

                    for (int i = 0; i < mud->object_count; i++) {
                        if (mud->object_x[i] != area_x ||
                            mud->object_y[i] != area_y) {
                            continue;
                        }

                        mud->ground_item_z[mud->ground_item_count] =
                            game_data_object_elevation[mud->object_id[i]];

                        break;
                    }

                    mud->ground_item_count++;
                } else {
                    item_id &= 32767;

                    int index = 0;

                    for (int i = 0; i < mud->ground_item_count; i++) {
                        if (mud->ground_item_x[i] != area_x ||
                            mud->ground_item_y[i] != area_y ||
                            mud->ground_item_id[i] != item_id) {
                            if (i != index) {
                                mud->ground_item_x[index] =
                                    mud->ground_item_x[i];

                                mud->ground_item_y[index] =
                                    mud->ground_item_y[i];

                                mud->ground_item_id[index] =
                                    mud->ground_item_id[i];

                                mud->ground_item_z[index] =
                                    mud->ground_item_z[i];
                            }

                            index++;
                        } else {
                            item_id = -123;
                        }
                    }

                    mud->ground_item_count = index;
                }
            }
        }

        mudclient_update_ground_item_models(mud);
        break;
    }
    case SERVER_MESSAGE: {
        char message[size + 1];
        memset(message, '\0', size + 1);
        strncpy(message, (char *)data + 1, size - 1);
        mudclient_show_server_message(mud, message);
        break;
    }
    case SERVER_INVENTORY_ITEMS: {
        int offset = 1;

        mud->inventory_items_count = get_unsigned_byte(data, offset++, size);

        for (int i = 0; i < mud->inventory_items_count; i++) {
            int id_equip = get_unsigned_short(data, offset, size);
            offset += 2;

            mud->inventory_item_id[i] = id_equip & 32767;
            mud->inventory_equipped[i] = id_equip / 32768;

            if (game_data_item_stackable[id_equip & 32767] == 0) {
                mud->inventory_item_stack_count[i] =
                    get_stack_int(data, offset, size);

                if (mud->inventory_item_stack_count[i] >= 128) {
                    offset += 4;
                } else {
                    offset++;
                }
            } else {
                mud->inventory_item_stack_count[i] = 1;
            }
        }
        break;
    }
    case SERVER_INVENTORY_ITEM_UPDATE: {
        int offset = 1;
        int stack = 1;

        int index = get_unsigned_byte(data, offset++, size);

        int id = get_unsigned_short(data, offset, size);
        offset += 2;

        if (game_data_item_stackable[id & 32767] == 0) {
            stack = get_stack_int(data, offset, size);

            if (stack >= 128) {
                offset += 4;
            } else {
                offset++;
            }
        }

        mud->inventory_item_id[index] = id & 32767;
        mud->inventory_equipped[index] = id / 32768;
        mud->inventory_item_stack_count[index] = stack;

        if (index >= mud->inventory_items_count) {
            mud->inventory_items_count = index + 1;
        }
        break;
    }
    case SERVER_INVENTORY_ITEM_REMOVE: {
        int index = get_unsigned_byte(data, 1, size);

        mud->inventory_items_count--;

        for (int i = index; i < mud->inventory_items_count; i++) {
            mud->inventory_item_id[i] = mud->inventory_item_id[i + 1];

            mud->inventory_item_stack_count[i] =
                mud->inventory_item_stack_count[i + 1];

            mud->inventory_equipped[i] = mud->inventory_equipped[i + 1];
        }
        break;
    }
    case SERVER_PLAYER_STAT_LIST: {
        int offset = 1;

        for (int i = 0; i < skills_length; i++) {
            mud->player_skill_current[i] = get_unsigned_byte(data, offset++, size);
        }

        for (int i = 0; i < skills_length; i++) {
            mud->player_skill_base[i] = get_unsigned_byte(data, offset++, size);
        }

        for (int i = 0; i < skills_length; i++) {
            mud->player_experience[i] = get_unsigned_int(data, offset, size);
            offset += 4;
        }

        mud->player_quest_points = get_unsigned_byte(data, offset++, size);
        break;
    }
    case SERVER_PLAYER_STAT_EQUIPMENT_BONUS: {
        for (int i = 0; i < 5; i++) {
            mud->player_stat_equipment[i] = get_unsigned_byte(data, 1 + i, size);
        }

        break;
    }
    case SERVER_PLAYER_STAT_EXPERIENCE_UPDATE: {
        int skill_index = get_unsigned_byte(data, 1, size);

        int old_experience = mud->player_experience[skill_index];
        mud->player_experience[skill_index] = get_unsigned_int(data, 2, size);

        mudclient_drop_experience(mud, skill_index,
                                  mud->player_experience[skill_index] -
                                      old_experience);
        break;
    }
    case SERVER_PLAYER_STAT_UPDATE: {
        int offset = 1;
        int skill_index = get_unsigned_byte(data, offset++, size);

        mud->player_skill_current[skill_index] =
            get_unsigned_byte(data, offset++, size);

        mud->player_skill_base[skill_index] = get_unsigned_byte(data, offset++, size);

        mud->player_experience[skill_index] = get_unsigned_int(data, offset, size);
        break;
    }
    case SERVER_PLAYER_STAT_FATIGUE: {
        mud->stat_fatigue = get_unsigned_short(data, 1, size);
        break;
    }
    case SERVER_PLAYER_QUEST_LIST: {
        for (int i = 0; i < quests_length; i++) {
            mud->quest_complete[i] = get_unsigned_byte(data, i + 1, size);
        }
        break;
    }
    case SERVER_FRIEND_LIST: {
        mud->friend_list_count = get_unsigned_byte(data, 1, size);

        for (int i = 0; i < mud->friend_list_count; i++) {
            mud->friend_list[i] = get_unsigned_long(data, 2 + i * 9, size);
            mud->friend_list_online[i] = get_unsigned_byte(data, 10 + i * 9, size);
        }

        mudclient_sort_friends(mud);
        break;
    }
    case SERVER_FRIEND_STATUS_CHANGE: {
        int64_t encoded_username = get_unsigned_long(data, 1, size);
        int world = get_unsigned_byte(data, 9, size);

        for (int i = 0; i < mud->friend_list_count; i++) {
            if (mud->friend_list[i] == encoded_username) {
                if (mud->friend_list_online[i] == 0 && world != 0) {
                    char username[USERNAME_LENGTH + 1] = {0};
                    decode_username(mud->friend_list[i], username);

                    char formatted[USERNAME_LENGTH + 20] = {0};
                    sprintf(formatted, "@pri@%s has logged in", username);

                    mudclient_show_server_message(mud, formatted);
                }

                if (mud->friend_list_online[i] != 0 && world == 0) {
                    char username[USERNAME_LENGTH + 1] = {0};
                    decode_username(mud->friend_list[i], username);

                    char formatted[USERNAME_LENGTH + 21] = {0};
                    sprintf(formatted, "@pri@%s has logged out", username);

                    mudclient_show_server_message(mud, formatted);
                }

                mud->friend_list_online[i] = world;
                mudclient_sort_friends(mud);

                return;
            }
        }

        mud->friend_list[mud->friend_list_count] = encoded_username;
        mud->friend_list_online[mud->friend_list_count] = world;

        mud->friend_list_count++;

        mudclient_sort_friends(mud);
        break;
    }
    case SERVER_FRIEND_MESSAGE: {
#ifdef REVISION_177
        int64_t from = get_unsigned_long(data, 1, size);
        char from_username[USERNAME_LENGTH + 1];
        decode_username(from, from_username);

        char *message = chat_message_decode(data, 9, size - 9);
        char formatted_message[USERNAME_LENGTH + strlen(message) + 18];

        sprintf(formatted_message, "@pri@%s: tells you %s", from_username,
                message);

        mudclient_show_server_message(mud, formatted_message);
#endif
        break;
    }
    case SERVER_IGNORE_LIST: {
        mud->ignore_list_count = get_unsigned_byte(data, 1, size);

        for (int i = 0; i < mud->ignore_list_count; i++) {
            mud->ignore_list[i] = get_unsigned_long(data, 2 + i * 8, size);
        }
        break;
    }
    case SERVER_CLOSE_CONNECTION: {
        mudclient_close_connection(mud);
        break;
    }
    case SERVER_SOUND: {
        char sound_name[size + 1];
        memset(sound_name, '\0', size + 1);
        strncpy(sound_name, (char *)data + 1, size - 1);
        mudclient_play_sound(mud, sound_name);
        break;
    }
    case SERVER_APPEARANCE: {
        mud->show_appearance_change = 1;
        break;
    }
    case SERVER_SLEEP_OPEN: {
        if (!mud->is_sleeping) {
            mud->fatigue_sleeping = mud->stat_fatigue;
        }

        mud->is_sleeping = 1;

        memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
        memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);

        surface_read_sleep_word(mud->surface, mud->sprite_texture + 1, data);
        mud->sleeping_status_text = NULL;
        break;
    }
    case SERVER_SLEEP_CLOSE: {
        mud->is_sleeping = 0;
        break;
    }
    case SERVER_SLEEP_INCORRECT: {
        mud->sleeping_status_text = "Incorrect - Please wait...";
        break;
    }
    case SERVER_PLAYER_STAT_FATIGUE_ASLEEP: {
        mud->fatigue_sleeping = get_unsigned_short(data, 1, size);
        break;
    }
    case SERVER_OPTION_LIST: {
        int count = get_unsigned_byte(data, 1, size);

        mud->show_option_menu = 1;
        mud->option_menu_count = count;

        int offset = 2;

        for (int i = 0; i < count; i++) {
            int entry_length = get_unsigned_byte(data, offset++, size);

            strncpy(mud->option_menu_entry[i], (char *)data + offset,
                    entry_length);

            mud->option_menu_entry[i][entry_length] = '\0';

            offset += entry_length;
        }
        break;
    }
    case SERVER_OPTION_LIST_CLOSE: {
        mud->show_option_menu = 0;
        break;
    }
    case SERVER_WELCOME: {
#ifdef REVISION_177
        mud->welcome_days_ago = get_unsigned_int(data, 1, size);
        mud->welcome_recovery_set_days = get_unsigned_int(data, 5, size);
        mud->welcome_last_ip = get_unsigned_int(data, 9, size);
#else
        mud->welcome_last_ip = get_unsigned_int(data, 1, size);
        mud->welcome_days_ago = get_unsigned_int(data, 5, size);
        mud->welcome_recovery_set_days = get_unsigned_byte(data, 7, size);
        mud->welcome_unread_messages = get_unsigned_int(data, 8, size);
#endif

        mud->show_dialog_welcome = 1;
        mud->welcome_screen_already_shown = 1;

        free(mud->welcome_last_ip_string);
        mud->welcome_last_ip_string = NULL;
        break;
    }
    case SERVER_SERVER_MESSAGE:
    case SERVER_SERVER_MESSAGE_ONTOP: {
        strncpy(mud->server_message, (char *)data + 1, size);
        mud->server_message[size] = '\0';

        /*strcpy(
            mud->server_message,
            "                                                                                                                                                                                                                                                                                                           *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   *                                                                                                                                                                                                                                               *                                                                                                                                                                                                                                                                                                                                                                   *                                                                                                                                                                                                                                                                 *                                 "
        );*/

        mud->show_dialog_server_message = 1;
        mud->server_message_box_top = opcode == SERVER_SERVER_MESSAGE_ONTOP;
        strcpy(mud->server_message_next, "");
        mud->server_message_page = 0;
        break;
    }
    case SERVER_BANK_OPEN: {
        mud->show_dialog_bank = 1;

        if (mud->options->bank_menus) {
            mud->show_right_click_menu = 0;
        }

        // TODO toggle
        // mud->bank_selected_item_slot = -1;
        // mud->bank_selected_item = -2;
        // mud->bank_scroll_row = 0;

        int offset = 1;

        mud->new_bank_item_count = get_unsigned_byte(data, offset++, size);
        mud->bank_items_max = get_unsigned_byte(data, offset++, size);

        for (int i = 0; i < mud->new_bank_item_count; i++) {
            mud->new_bank_items[i] = get_unsigned_short(data, offset, size);
            offset += 2;

            mud->new_bank_items_count[i] = get_stack_int(data, offset, size);

            if (mud->new_bank_items_count[i] >= 128) {
                offset += 4;
            } else {
                offset++;
            }
        }

        mudclient_update_bank_items(mud);

        if (mud->options->bank_search) {
            memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
            memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
        }
        break;
    }
    case SERVER_BANK_CLOSE: {
        mud->show_dialog_bank = 0;
        break;
    }
    case SERVER_BANK_UPDATE: {
        int offset = 1;
        int item_index = get_unsigned_byte(data, offset++, size);
        int item = get_unsigned_short(data, offset, size);
        offset += 2;

        int item_count = get_stack_int(data, offset, size);

        if (item_count >= 128) {
            offset += 4;
        } else {
            offset++;
        }

        if (item_count == 0) {
            mud->new_bank_item_count--;

            for (int i = item_index; i < mud->new_bank_item_count; i++) {
                mud->new_bank_items[i] = mud->new_bank_items[i + 1];
                mud->new_bank_items_count[i] = mud->new_bank_items_count[i + 1];
            }
        } else {
            mud->new_bank_items[item_index] = item;
            mud->new_bank_items_count[item_index] = item_count;

            if (item_index >= mud->new_bank_item_count) {
                mud->new_bank_item_count = item_index + 1;
            }
        }

        mudclient_update_bank_items(mud);
        break;
    }
    case SERVER_SHOP_OPEN: {
        mud->show_dialog_shop = 1;

        int offset = 1;
        int new_item_count = get_unsigned_byte(data, offset++, size);
        int is_general = get_unsigned_byte(data, offset++, size);

        mud->shop_sell_price_mod = get_unsigned_byte(data, offset++, size);
        mud->shop_buy_price_mod = get_unsigned_byte(data, offset++, size);

        for (int i = 0; i < 40; i++) {
            mud->shop_items[i] = -1;
        }

        for (int i = 0; i < new_item_count; i++) {
            mud->shop_items[i] = get_unsigned_short(data, offset, size);
            offset += 2;

            mud->shop_items_count[i] = get_unsigned_short(data, offset, size);
            offset += 2;

            mud->shop_items_price[i] = get_signed_byte(data, offset++, size);
        }

        if (is_general == 1) {
            int shop_index = 39;

            for (int i = 0; i < mud->inventory_items_count; i++) {
                if (shop_index < new_item_count) {
                    break;
                }

                int unsellable = 0;

                for (int j = 0; j < 40; j++) {
                    if (mud->shop_items[j] != mud->inventory_item_id[i]) {
                        continue;
                    }

                    unsellable = 1;
                    break;
                }

                if (mud->inventory_item_id[i] == 10) {
                    unsellable = 1;
                }

                if (!unsellable) {
                    mud->shop_items[shop_index] =
                        mud->inventory_item_id[i] & 32767;

                    mud->shop_items_count[shop_index] = 0;
                    mud->shop_items_price[shop_index] = 0;
                    shop_index--;
                }
            }
        }

        if (mud->shop_selected_item_index >= 0 &&
            mud->shop_selected_item_index < 40 &&
            mud->shop_items[mud->shop_selected_item_index] !=
                mud->shop_selected_item_type) {
            mud->shop_selected_item_index = -1;
            mud->shop_selected_item_type = -2;
        }
        break;
    }
    case SERVER_SHOP_CLOSE: {
        mud->show_dialog_shop = 0;
        break;
    }
    case SERVER_TRADE_OPEN:
    case SERVER_DUEL_OPEN: {
        int player_index = get_unsigned_short(data, 1, size);

        if (mud->player_server[player_index] != NULL) {
            strcpy(mud->transaction_recipient_name,
                   mud->player_server[player_index]->name);
        }

        int is_trade = opcode == SERVER_TRADE_OPEN;

        mud->show_dialog_trade = is_trade;

        mud->transaction_recipient_accepted = 0;
        mud->transaction_accepted = 0;
        mud->transaction_item_count = 0;
        mud->transaction_recipient_item_count = 0;

        if (!is_trade) {
            mud->show_dialog_duel = 1;
            mud->duel_option_retreat = 0;
            mud->duel_option_magic = 0;
            mud->duel_option_prayer = 0;
            mud->duel_option_weapons = 0;
        }
        break;
    }
    case SERVER_TRADE_CLOSE:
    case SERVER_DUEL_CLOSE: {
        mud->show_dialog_trade = 0;
        mud->show_dialog_trade_confirm = 0;

        mud->show_dialog_duel = 0;
        mud->show_dialog_duel_confirm = 0;
        break;
    }
    case SERVER_TRADE_ITEMS:
    case SERVER_DUEL_ITEMS: {
        mud->transaction_recipient_item_count =
            get_unsigned_byte(data, 1, size);

        int offset = 2;

        for (int i = 0; i < mud->transaction_recipient_item_count; i++) {
            mud->transaction_recipient_items[i] =
                get_unsigned_short(data, offset, size);

            offset += 2;

            mud->transaction_recipient_items_count[i] =
                get_unsigned_int(data, offset, size);

            offset += 4;
        }

        mud->transaction_recipient_accepted = 0;
        mud->transaction_accepted = 0;
        break;
    }
    case SERVER_TRADE_RECIPIENT_ACCEPTED:
    case SERVER_DUEL_RECIPIENT_ACCEPTED: {
        mud->transaction_recipient_accepted = get_unsigned_byte(data, 1, size);
        break;
    }
    case SERVER_TRADE_ACCEPTED:
    case SERVER_DUEL_ACCEPTED: {
        mud->transaction_accepted = get_unsigned_byte(data, 1, size);
        break;
    }
    case SERVER_TRADE_CONFIRM_OPEN:
    case SERVER_DUEL_CONFIRM_OPEN: {
        if (opcode == SERVER_TRADE_CONFIRM_OPEN) {
            mud->show_dialog_trade_confirm = 1;
        } else {
            mud->show_dialog_duel_confirm = 1;
        }

        mud->transaction_confirm_accepted = 0;
        mud->show_dialog_trade = 0;
        mud->show_dialog_duel = 0;

        int offset = 1;

        mud->transaction_recipient_confirm_name =
            get_unsigned_long(data, offset, size);

        offset += 8;

        mud->transaction_recipient_confirm_item_count =
            get_unsigned_byte(data, offset++, size);

        for (int i = 0; i < mud->transaction_recipient_confirm_item_count;
             i++) {
            mud->transaction_recipient_confirm_items[i] =
                get_unsigned_short(data, offset, size);

            offset += 2;

            mud->transaction_recipient_confirm_items_count[i] =
                get_unsigned_int(data, offset, size);

            offset += 4;
        }

        mud->transaction_confirm_item_count =
            get_unsigned_byte(data, offset++, size);

        for (int i = 0; i < mud->transaction_confirm_item_count; i++) {
            mud->transaction_confirm_items[i] =
                get_unsigned_short(data, offset, size);

            offset += 2;

            mud->transaction_confirm_items_count[i] =
                get_unsigned_int(data, offset, size);

            offset += 4;
        }
        break;
    }
    case SERVER_DUEL_SETTINGS: {
        mud->duel_option_retreat = get_unsigned_byte(data, 1, size);
        mud->duel_option_magic = get_unsigned_byte(data, 2, size);
        mud->duel_option_prayer = get_unsigned_byte(data, 3, size);
        mud->duel_option_weapons = get_unsigned_byte(data, 4, size);

        mud->transaction_recipient_accepted = 0;
        mud->transaction_accepted = 0;
        break;
    }
    case SERVER_PRAYER_STATUS: {
        for (int i = 0; i < size - 1; i++) {
            int on = data[i + 1] == 1;

            if (!mud->prayer_on[i] && on) {
                mudclient_play_sound(mud, "prayeron");
            }

            if (mud->prayer_on[i] && !on) {
                mudclient_play_sound(mud, "prayeroff");
            }

            mud->prayer_on[i] = on;
        }
        break;
    }
    case SERVER_TELEPORT_BUBBLE: {
        if (mud->teleport_bubble_count < TELEPORT_BUBBLE_MAX) {
            int type = get_unsigned_byte(data, 1, size);
            int x = get_signed_byte(data, 2, size) + mud->local_region_x;
            int y = get_signed_byte(data, 3, size) + mud->local_region_y;

            mud->teleport_bubble_type[mud->teleport_bubble_count] = type;
            mud->teleport_bubble_time[mud->teleport_bubble_count] = 0;
            mud->teleport_bubble_x[mud->teleport_bubble_count] = x;
            mud->teleport_bubble_y[mud->teleport_bubble_count] = y;

            mud->teleport_bubble_count++;
        }
        break;
    }
    case SERVER_PLAYER_DIED: {
        mud->death_screen_timeout = 250;
        break;
    }
    case SERVER_LOGOUT_DENY: {
        mud->logout_timeout = 0;

        mudclient_show_message(mud,
                               "@cya@Sorry, you can't logout at the moment",
                               MESSAGE_TYPE_GAME);
        break;
    }
    case SERVER_GAME_SETTINGS: {
        mud->settings_camera_auto = get_unsigned_byte(data, 1, size);
        mud->settings_mouse_button_one = get_unsigned_byte(data, 2, size);
        mud->settings_sound_disabled = get_unsigned_byte(data, 3, size);
        break;
    }
    case SERVER_PRIVACY_SETTINGS: {
        mud->settings_block_chat = get_unsigned_byte(data, 1, size);
        mud->settings_block_private = get_unsigned_byte(data, 2, size);
        mud->settings_block_trade = get_unsigned_byte(data, 3, size);
        mud->settings_block_duel = get_unsigned_byte(data, 4, size);
        break;
    }
#ifndef REVISION_177
    case SERVER_SYSTEM_UPDATE: {
        mud->system_update = get_unsigned_short(data, 1, size) * 32;
        break;
    }
#endif
    }
}
