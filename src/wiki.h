#ifndef _H_WIKI
#define _H_WIKI

#include "game-data.h"

extern char *wiki_pages_npcs[];
extern char *wiki_pages_items[];
extern char *wiki_pages_objects[];

char *wiki_get_npc_page(int npc_id);
char *wiki_get_item_page(int item_id);
char *wiki_get_object_page(int object_id);

#endif
