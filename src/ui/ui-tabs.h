#ifndef _H_UI_TABS
#define _H_UI_TABS

#define UI_TABS_WIDTH 245
#define UI_BUTTON_SIZE 35

#define INVENTORY_TAB 1
#define MAP_TAB 2
#define STATS_TAB 3
#define MAGIC_TAB 4
#define SOCIAL_TAB 5
#define OPTIONS_TAB 6

#include "../mudclient.h"
#include "inventory-tab.h"
#include "magic-tab.h"
#include "minimap-tab.h"
#include "options-tab.h"
#include "social-tab.h"
#include "stats-tab.h"

#if defined (_3DS) || defined (__SWITCH__)
void mudclient_3ds_toggle_ui_tab(mudclient *mud, int tab);
#endif
void mudclient_set_active_ui_tab(mudclient *mud, int no_menus);
void mudclient_draw_active_ui_tab(mudclient *mud, int no_menus);

#endif
