#ifndef _H_MENU
#define _H_MENU

/* higher number is higher priority in the context menu */
typedef enum {
    MENU_CAST_GROUNDITEM = 200,
    MENU_USEWITH_GROUNDITEM = 210,
    MENU_GROUNDITEM_TAKE = 220,
    MENU_GROUNDITEM_EXAMINE = 3200,

    MENU_CAST_WALLOBJECT = 300,
    MENU_USEWITH_WALLOBJECT = 310,
    MENU_WALL_OBJECT_COMMAND1 = 320,
    MENU_WALL_OBJECT_COMMAND2 = 2300,
    MENU_WALL_OBJECT_EXAMINE = 3300,

    MENU_CAST_OBJECT = 400,
    MENU_USEWITH_OBJECT = 410,
    MENU_OBJECT_COMMAND1 = 420,
    MENU_OBJECT_COMMAND2 = 2400,
    MENU_OBJECT_EXAMINE = 3400,

    MENU_CAST_INVITEM = 600,
    MENU_USEWITH_INVITEM = 610,
    MENU_INVENTORY_UNEQUIP = 620,
    MENU_INVENTORY_WEAR = 630,
    MENU_INVENTORY_COMMAND = 640,
    MENU_INVENTORY_USE = 650,
    MENU_INVENTORY_DROP = 660,
    MENU_INVENTORY_EXAMINE = 3600,

    MENU_CAST_NPC = 700,
    MENU_USEWITH_NPC = 710,
    MENU_NPC_TALK = 720,
    MENU_NPC_COMMAND = 725,
    MENU_NPC_ATTACK1 = 715,
    MENU_NPC_ATTACK2 = 2715,
    MENU_NPC_EXAMINE = 3700,

    MENU_CAST_PLAYER = 800,
    MENU_USEWITH_PLAYER = 810,
    MENU_PLAYER_ATTACK1 = 805,
    MENU_PLAYER_ATTACK2 = 2805,
    MENU_PLAYER_DUEL = 2806,
    MENU_PLAYER_TRADE = 2810,
    MENU_PLAYER_FOLLOW = 2820,

    MENU_CAST_GROUND = 900,
    MENU_WALK = 920,
    MENU_CAST_SELF = 1000,

    MENU_BANK_WITHDRAW = 601,
    MENU_BANK_DEPOSIT = 602,

    /* trade and duel */
    MENU_TRANSACTION_OFFER = 603,
    MENU_TRANSACTION_REMOVE = 604,

    /* compass directions */
    MENU_MAP_LOOK = 101,

    MENU_WIKI_LOOKUP = 102,

    MENU_CANCEL = 4000
} MenuType;

#include "../mudclient.h"
#include "transaction.h"

/* for mouse picking */
#define PLAYER_FACE_TAG 10000
#define GROUND_ITEM_FACE_TAG 20000
#define NPC_FACE_TAG 30000
#define TILE_FACE_TAG 200000

#define WIKI_TYPE_PAGE "Special:Lookup?type=%s&id=%d&name=%s"

void mudclient_menu_item_click(mudclient *mud, int i);
void mudclient_create_top_mouse_menu(mudclient *mud);
void mudclient_menu_add_wiki(mudclient *mud, char *display, char *page);
void mudclient_menu_add_id_wiki(mudclient *mud, char *display, char *type,
                                int id);
void mudclient_menu_add_ground_item(mudclient *mud, int index);
void mudclient_create_right_click_menu(mudclient *mud);
void mudclient_draw_right_click_menu(mudclient *mud);
void mudclient_draw_hover_tooltip(mudclient *mud);

#endif
