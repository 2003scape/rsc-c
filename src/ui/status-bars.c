#include "status-bars.h"

void mudclient_draw_status_bars(mudclient *mud) {
    int ui_x = 7;
    int ui_y = 15 + (20 * 5) + 6;

    int x = ui_x;
    int y = ui_y;

    int max_hits = mud->player_skill_base[SKILL_HITS];
    int current_hits = mud->player_skill_current[SKILL_HITS];

    surface_draw_status_bar(mud->surface, 0, max_hits, current_hits, "Hits", x,
                            y, STATUS_BAR_WIDTH, 16, RED, GREEN);

    x += STATUS_BAR_WIDTH + 3;

    int max_prayer = mud->player_skill_base[SKILL_PRAYER];
    int current_prayer = mud->player_skill_current[SKILL_PRAYER];

    surface_draw_status_bar(mud->surface, 0, max_prayer, current_prayer,
                            "Prayer", x, y, STATUS_BAR_WIDTH, 16, BLACK, CYAN);

    GameCharacter *opponent = mudclient_get_opponent(mud);

    if (opponent != NULL && opponent->max_hits != 0) {
        x = ui_x;
        y += 16 + 5;

        char *opponent_name = "";

        if (opponent->npc_id != -1) {
            opponent_name = game_data_npc_name[opponent->npc_id];
        } else {
            opponent_name = opponent->name;
        }

        char formatted_opponent[strlen(opponent_name) + 2];
        sprintf(formatted_opponent, "%s:", opponent_name);

        surface_draw_status_bar(mud->surface, 0, opponent->max_hits,
                                opponent->current_hits, opponent_name, x, y,
                                (STATUS_BAR_WIDTH * 2) + 2, 16, RED, GREEN);
    }
}
