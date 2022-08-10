#include "status-bars.h"

void mudclient_draw_status_bars(mudclient *mud) {
    int max_hits = mud->player_skill_base[SKILL_HITS];
    int current_hits = mud->player_skill_current[SKILL_HITS];

    surface_draw_status_bar(mud->surface, 0, max_hits, current_hits, "Hits", 7,
                            15 + (20 * 5) + 6, STATUS_BAR_WIDTH, 16, RED,
                            GREEN);

    int max_prayer = mud->player_skill_base[SKILL_PRAYER];
    int current_prayer = mud->player_skill_current[SKILL_PRAYER];

    surface_draw_status_bar(mud->surface, 0, max_prayer, current_prayer,
                            "Prayer", 7 + STATUS_BAR_WIDTH + 3,
                            15 + (20 * 5) + 6, STATUS_BAR_WIDTH, 16, BLACK,
                            CYAN);
}
