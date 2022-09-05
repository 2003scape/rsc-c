#ifndef _H_WIKI
#define _H_WIKI

#include "game-data.h"

extern char *wiki_pages_npcs[];
extern char *wiki_pages_items[];

char *wiki_get_npc_page(int npc_id);
char *wiki_get_item_page(int item_id);

#endif
