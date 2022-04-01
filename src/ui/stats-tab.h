#ifndef _H_STATS_TAB
#define _H_STATS_TAB

#include "../mudclient.h"

#define STATS_WIDTH 196
#define STATS_HEIGHT 275
#define STATS_TAB_HEIGHT 24
#define STATS_LINE_BREAK 12

#define STATS_TAB_SPRITE_OFFSET 3

extern char *short_skill_names[];
extern char *skill_names[];
extern int skills_length;
extern char *equipment_stat_names[];
extern int experience_array[100];

extern char *free_quests[];
extern char *members_quests[];
extern char **quest_names;
extern int quests_length;

extern char *stats_tabs[];

void init_stats_tab_global();

void mudclient_draw_ui_tab_stats(mudclient *mud, int no_menus);

#endif
