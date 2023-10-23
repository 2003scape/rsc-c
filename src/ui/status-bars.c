#include "status-bars.h"

void mudclient_draw_status_bars(mudclient *mud) {
    int ui_x = 7;
    int ui_y = 15 + (COMBAT_BUTTON_HEIGHT * 5) + 6;

    int x = ui_x;
    int y = ui_y;

    int max_hits = mud->player_skill_base[SKILL_HITS];
    int current_hits = mud->player_skill_current[SKILL_HITS];

    surface_draw_status_bar(mud->surface, max_hits, current_hits, "Hits", x, y,
                            STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT, RED, GREEN);

    x += STATUS_BAR_WIDTH + 3;

    int max_prayer = mud->player_skill_base[SKILL_PRAYER];
    int current_prayer = mud->player_skill_current[SKILL_PRAYER];

    surface_draw_status_bar(mud->surface, max_prayer, current_prayer, "Prayer",
                            x, y, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT, BLACK,
                            CYAN);

    GameCharacter *opponent = mudclient_get_opponent(mud);

    if (opponent != NULL && opponent->max_hits != 0) {
        x = ui_x;
        y += STATUS_BAR_HEIGHT + 5;

        char *opponent_name = "";

        if (opponent->npc_id != -1) {
            opponent_name = game_data_npc_name[opponent->npc_id];
        } else {
            opponent_name = opponent->name;
        }

        char formatted_opponent[strlen(opponent_name) + 2];
        sprintf(formatted_opponent, "%s:", opponent_name);

        surface_draw_status_bar(mud->surface, opponent->max_hits,
                                opponent->current_hits, opponent_name, x, y,
                                (STATUS_BAR_WIDTH * 2) + 3, STATUS_BAR_HEIGHT,
                                RED, GREEN);
    }

    if (!mud->is_sleeping) {
        x = ui_x + 2;
        y += STATUS_BAR_HEIGHT + 14;

        char formatted_fatigue[24];
        snprintf(formatted_fatigue, sizeof(formatted_fatigue),
                 "Fatigue: %.2f%%", (mud->stat_fatigue * 100.0f) / 750.0f);
        surface_draw_string(mud->surface, formatted_fatigue,
                            x, y, FONT_REGULAR_11, WHITE);
    }

    y = 48;

    /* Display drained / boosted skills */
    for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
        if (i == SKILL_PRAYER || i == SKILL_HITS) {
            continue;
        }
        if (mud->player_skill_current[i] == mud->player_skill_base[i]) {
            continue;
        }
        const char *colour = "";
        float p = mud->player_skill_current[i] /
                (float)mud->player_skill_base[i];
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
        char formatted_effect[32];
        snprintf(formatted_effect, sizeof(formatted_effect),
                "%s%s: %d/%d", colour, skill_names[i],
                mud->player_skill_current[i], mud->player_skill_base[i]);
        surface_draw_string_right(mud->surface, formatted_effect,
                mud->game_width - 2, y, FONT_REGULAR_11, WHITE);
        y += 14;
    }
}
