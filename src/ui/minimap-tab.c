#include "./minimap-tab.h"

void mudclient_draw_minimap_entity(mudclient *mud, int x, int y, int colour) {
    surface_set_pixel(mud->surface, x, y, colour);
    surface_set_pixel(mud->surface, x - 1, y, colour);
    surface_set_pixel(mud->surface, x + 1, y, colour);
    surface_set_pixel(mud->surface, x, y - 1, colour);
    surface_set_pixel(mud->surface, x, y + 1, colour);
}

void mudclient_draw_ui_tab_minimap(mudclient *mud, int no_menus) {
    int ui_x = mud->game_width - MINIMAP_WIDTH - 3;
    int ui_y = 36;

    surface_draw_sprite_from3(mud->surface, mud->game_width - UI_TABS_WIDTH - 3,
                              3, mud->sprite_media + MINIMAP_TAB_SPRITE_OFFSET);

    surface_draw_box(mud->surface, ui_x, ui_y, MINIMAP_WIDTH, MINIMAP_HEIGHT,
                     0);

    surface_set_bounds(mud->surface, ui_x, ui_y, ui_x + MINIMAP_WIDTH,
                       ui_y + MINIMAP_HEIGHT);

    int scale = 192 + mud->minimap_random2;
    int rotation = (mud->camera_rotation + mud->minimap_random1) & 0xff;

    int player_x = ((mud->local_player->current_x - 6040) * 3 * scale) / 2048;
    int player_y = ((mud->local_player->current_y - 6040) * 3 * scale) / 2048;

    int sin = sin_cos_2048[(1024 - rotation * 4) & 1023];
    int cos = sin_cos_2048[((1024 - rotation * 4) & 1023) + 1024];
    int temp_x = (player_y * sin + player_x * cos) >> 18;

    player_y = (player_y * cos - player_x * sin) >> 18;
    player_x = temp_x;

    //scale = 128;

    surface_draw_minimap_sprite(
        mud->surface, ui_x + (MINIMAP_WIDTH / 2) - player_x,
        ui_y + (MINIMAP_HEIGHT / 2) + player_y, mud->sprite_media - 1,
        (rotation + 64) & 255, scale);

    for (int i = 0; i < mud->object_count; i++) {
        int object_x = ((mud->object_x[i] * MAGIC_LOC + 64 -
                         mud->local_player->current_x) *
                        3 * scale) /
                       2048;

        int object_y = ((mud->object_y[i] * MAGIC_LOC + 64 -
                         mud->local_player->current_y) *
                        3 * scale) /
                       2048;

        int temp_x = (object_y * sin + object_x * cos) >> 18;

        object_y = (object_y * cos - object_x * sin) >> 18;
        object_x = temp_x;

        mudclient_draw_minimap_entity(
            mud, ui_x + (MINIMAP_WIDTH / 2) + object_x,
            ui_y + (MINIMAP_HEIGHT / 2) - object_y, CYAN);
    }

    for (int i = 0; i < mud->ground_item_count; i++) {
        int item_x = ((mud->ground_item_x[i] * MAGIC_LOC + 64 -
                       mud->local_player->current_x) *
                      3 * scale) /
                     2048;

        int item_y = ((mud->ground_item_y[i] * MAGIC_LOC + 64 -
                       mud->local_player->current_y) *
                      3 * scale) /
                     2048;

        int temp_x = (item_y * sin + item_x * cos) >> 18;

        item_y = (item_y * cos - item_x * sin) >> 18;
        item_x = temp_x;

        mudclient_draw_minimap_entity(mud, ui_x + (MINIMAP_WIDTH / 2) + item_x,
                                      ui_y + (MINIMAP_HEIGHT / 2) - item_y,
                                      RED);
    }

    for (int i = 0; i < mud->npc_count; i++) {
        GameCharacter *npc = mud->npcs[i];

        int npc_x =
            ((npc->current_x - mud->local_player->current_x) * 3 * scale) /
            2048;

        int npc_y =
            ((npc->current_y - mud->local_player->current_y) * 3 * scale) /
            2048;

        int temp_x = (npc_y * sin + npc_x * cos) >> 18;

        npc_y = (npc_y * cos - npc_x * sin) >> 18;
        npc_x = temp_x;

        mudclient_draw_minimap_entity(mud, ui_x + (MINIMAP_WIDTH / 2) + npc_x,
                                      ui_y + (MINIMAP_HEIGHT / 2) - npc_y,
                                      YELLOW);
    }

    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        int other_player_x =
            ((player->current_x - mud->local_player->current_x) * 3 * scale) /
            2048;

        int other_player_y =
            ((player->current_y - mud->local_player->current_y) * 3 * scale) /
            2048;

        int temp_x = (other_player_y * sin + other_player_x * cos) >> 18;

        other_player_y = (other_player_y * cos - other_player_x * sin) >> 18;
        other_player_x = temp_x;

        int player_colour = WHITE;

        for (int j = 0; j < mud->friend_list_count; j++) {
            if (player->hash != mud->friend_list_hashes[j] ||
                mud->friend_list_online[j] != 255) {
                continue;
            }

            player_colour = GREEN;
            break;
        }

        mudclient_draw_minimap_entity(
            mud, ui_x + (MINIMAP_WIDTH / 2) + other_player_x,
            ui_y + (MINIMAP_HEIGHT / 2) - other_player_y, player_colour);
    }

    // player
    surface_draw_circle(mud->surface, ui_x + (MINIMAP_WIDTH / 2),
                        ui_y + (MINIMAP_HEIGHT / 2), 2, WHITE, 255);

    // compass
    surface_draw_minimap_sprite(mud->surface, ui_x + 19, ui_y + 19,
                                mud->sprite_media + 24,
                                (mud->camera_rotation + 128) & 255, 128);

    surface_set_bounds(mud->surface, 0, 0, mud->game_width,
                       mud->game_height + 12);

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    if (mud->options->reset_compass && mud->mouse_button_click == 1 &&
        mouse_x > 0 && mouse_x <= 32 && mouse_y > 0 && mouse_y <= 32) {
        mud->camera_rotation = 128;
        mud->mouse_button_click = 0;
        return;
    }

    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < MINIMAP_WIDTH + 40 &&
        mouse_y < MINIMAP_HEIGHT) {
        int delta_y = ((mud->mouse_x - (ui_x + (MINIMAP_WIDTH / 2))) * 16384) /
                      (3 * scale);

        int delta_x = ((mud->mouse_y - (ui_y + (MINIMAP_HEIGHT / 2))) * 16384) /
                      (3 * scale);

        int temp_x = (delta_x * sin + delta_y * cos) >> 15;

        delta_x = (delta_x * cos - delta_y * sin) >> 15;
        delta_y = temp_x;
        delta_y += mud->local_player->current_x;
        delta_x = mud->local_player->current_y - delta_x;

        if (mud->mouse_button_click == 1) {
            mudclient_walk_to_action_source(
                mud, mud->local_region_x, mud->local_region_y, delta_y / 128,
                delta_x / 128, 0);

            mud->mouse_button_click = 0;
        }
    }
}
