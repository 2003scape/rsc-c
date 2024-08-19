#include "status-bars.h"

void mudclient_draw_status_bars(mudclient *mud) {
    // TODO make that 390 a constant since we're using it more than once
    // (surface_draw_status_bar, combat style somewhere)
    int is_compact = mud->surface->width < 390;

    int is_touch = mudclient_is_touch(mud);

    // TODO maybe make COMBAT_BUTTON_HEIGHT_COMPACT etc.
    int combat_button_height = is_compact ? 22 : 20;

    int status_bar_width = 175 / 2 - 1;

    if (is_touch) {
        status_bar_width = (mud->surface->width * 0.353f) / 2 - 6;
    } else if (is_compact) {
        status_bar_width = 106 / 2 - 1;
    }

    int status_bar_height = is_touch ? 13 : 16;

    int ui_x = is_touch ? 4 : 7;

    int ui_y = is_touch ? mud->surface->height - status_bar_height
                        : 15 + (combat_button_height * 5) + 6;

    int x = ui_x;
    int y = ui_y;

    int max_hits = mud->player_skill_base[SKILL_HITS];
    int current_hits = mud->player_skill_current[SKILL_HITS];

    surface_draw_status_bar(mud->surface, max_hits, current_hits, "Hits", x, y,
                            status_bar_width, status_bar_height, RED, GREEN, 0);

    x += status_bar_width + (is_touch ? 4 : 3);

    int max_prayer = mud->player_skill_base[SKILL_PRAYER];
    int current_prayer = mud->player_skill_current[SKILL_PRAYER];

    surface_draw_status_bar(mud->surface, max_prayer, current_prayer, "Prayer",
                            x, y, status_bar_width, status_bar_height, BLACK,
                            CYAN, 0);

    GameCharacter *opponent = mudclient_get_opponent(mud);

    if (is_touch) {
        x += status_bar_width + mud->surface->width * 0.293f + 9;
    }

    int show_opponent_health = opponent != NULL && opponent->max_hits != 0;

    if (show_opponent_health) {
        if (is_touch) {
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
                                is_touch ? status_bar_width
                                         : (status_bar_width * 2) + 3,
                                status_bar_height, RED, GREEN, 0);

        if (is_touch) {
            x += status_bar_width + 4;
        }
    }

    if (!mud->is_sleeping && mud->options->fatigue) {
        if (is_touch) {
            y = ui_y;
        } else {
            x = ui_x;
            y += status_bar_height + 5;
        }

        int width = status_bar_width * 2 + 3;

        if (is_touch) {
            if (show_opponent_health) {
                width = status_bar_width;
            } else {
                width = status_bar_width * 1.75f;
                x += (mud->surface->width * 0.353f / 2) - (width / 2) - 3;
            }
        }

        surface_draw_status_bar(mud->surface, 750, mud->stat_fatigue, "Fatigue",
                                x, y, width, status_bar_height, BLACK, 0xe2e2e2,
                                1);
    }

    x = is_touch ? 86 : mud->game_width - 2;
    y = is_touch ? 108 + (combat_button_height * 5) + 15 : 48;

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

        surface_draw_string_right(mud->surface, formatted_effect, x, y,
                                  FONT_REGULAR_11, WHITE);

        y += 14;
    }
}
