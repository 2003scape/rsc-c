#include "status-bars.h"

void mudclient_draw_status_bars(mudclient *mud) {
    int is_touch = mudclient_is_touch(mud);

    int status_bar_height = is_touch ? 14 : 16;

    int ui_x = is_touch ? 9 : 7;

    int ui_y = is_touch ? mud->surface->height - status_bar_height
                        : 15 + (COMBAT_BUTTON_HEIGHT * 5) + 6;

    int x = ui_x;
    int y = ui_y;

    int max_hits = mud->player_skill_base[SKILL_HITS];
    int current_hits = mud->player_skill_current[SKILL_HITS];

    surface_draw_status_bar(mud->surface, max_hits, current_hits, "Hits", x, y,
                            STATUS_BAR_WIDTH, status_bar_height, RED, GREEN, 0);

    x += STATUS_BAR_WIDTH + (is_touch ? 10 : 3);

    int max_prayer = mud->player_skill_base[SKILL_PRAYER];
    int current_prayer = mud->player_skill_current[SKILL_PRAYER];

    surface_draw_status_bar(mud->surface, max_prayer, current_prayer, "Prayer",
                            x, y, STATUS_BAR_WIDTH, status_bar_height, BLACK,
                            CYAN, 0);

    GameCharacter *opponent = mudclient_get_opponent(mud);

    if (opponent != NULL && opponent->max_hits != 0 &&
        (is_touch ? mud->surface->width > 508 : 1)) {
        if (is_touch) {
            x += STATUS_BAR_WIDTH + 10;
            y = ui_y;
        } else {
            x = ui_x;
            y += status_bar_height + 5;
        }

        char *opponent_name = "";

        if (opponent->npc_id != -1) {
            opponent_name = game_data.npcs[opponent->npc_id].name;
        } else {
            opponent_name = opponent->name;
        }

        char formatted_opponent[strlen(opponent_name) + 2];
        sprintf(formatted_opponent, "%s:", opponent_name);

        surface_draw_status_bar(mud->surface, opponent->max_hits,
                                opponent->current_hits, opponent_name, x, y,
                                (STATUS_BAR_WIDTH * 2) + 3, status_bar_height,
                                RED, GREEN, 0);

        if (is_touch) {
            x += STATUS_BAR_WIDTH + 3;
        }
    }

    if (!mud->is_sleeping) {
        if (is_touch) {
            x += STATUS_BAR_WIDTH + 10;
            y = ui_y;
        } else {
            x = ui_x + 2;
            y += status_bar_height + 5;
        }

        surface_draw_status_bar(
            mud->surface, 750, mud->stat_fatigue, "Fatigue", x, y,
            is_touch ? (int)(STATUS_BAR_WIDTH * 1.33f) : (STATUS_BAR_WIDTH * 2) + 3,
            status_bar_height, BLACK, 0xe2e2e2, 1);
    }

    x = is_touch ? 86 : mud->game_width - 2;
    y = is_touch ? 108 + (COMBAT_BUTTON_HEIGHT * 5) + 15 : 48;

    /* Display drained / boosted skills */
    for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
        if (i == SKILL_PRAYER || i == SKILL_HITS) {
            continue;
        }

        if (mud->player_skill_current[i] == mud->player_skill_base[i]) {
            continue;
        }

        const char *colour = "";

        float p =
            mud->player_skill_current[i] / (float)mud->player_skill_base[i];

        if (p >= 1.15f) { /* super potion */
            colour = "@gre@";
        } else if (p >= 1.1f) { /* normal potion */
            colour = "@gr3@";
        } else if (p >= 1.05f) { /* cocktails */
            colour = "@gr2@";
        } else if (p >= 1.0f) {
            colour = "@gr1@";
        } else if (p <= 0.9f) { /* enfeeble */
            colour = "@red@";
        } else if (p <= 0.95f) {
            colour = "@or3@";
        } else if (p <= 0.975f) {
            colour = "@or2@";
        } else {
            colour = "@or1@";
        }

        char formatted_effect[32] = {0};

        snprintf(formatted_effect, sizeof(formatted_effect), "%s%s: %d/%d",
                 colour, skill_names[i], mud->player_skill_current[i],
                 mud->player_skill_base[i]);

        surface_draw_string_right(mud->surface, formatted_effect,
                                  x, y, FONT_REGULAR_11, WHITE);

        y += 14;
    }
}
