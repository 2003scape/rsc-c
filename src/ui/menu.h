#ifndef _H_MENU

#include "../mudclient.h"
#include "transaction.h"

/* for mouse picking */
#define PLAYER_FACE_TAG 10000
#define GROUND_ITEM_FACE_TAG 20000
#define NPC_FACE_TAG 30000
#define TILE_FACE_TAG 200000

/* number determines priority in the menu */
#define MENU_CAST_GROUNDITEM 200
#define MENU_USEWITH_GROUNDITEM 210
#define MENU_GROUNDITEM_TAKE 220
#define MENU_GROUNDITEM_EXAMINE 3200

#define MENU_CAST_WALLOBJECT 300
#define MENU_USEWITH_WALLOBJECT 310
#define MENU_WALL_OBJECT_COMMAND1 320
#define MENU_WALL_OBJECT_COMMAND2 2300
#define MENU_WALL_OBJECT_EXAMINE 3300

#define MENU_CAST_OBJECT 400
#define MENU_USEWITH_OBJECT 410
#define MENU_OBJECT_COMMAND1 420
#define MENU_OBJECT_COMMAND2 2400
#define MENU_OBJECT_EXAMINE 3400

#define MENU_CAST_INVITEM 600
#define MENU_USEWITH_INVITEM 610
#define MENU_INVENTORY_UNEQUIP 620
#define MENU_INVENTORY_WEAR 630
#define MENU_INVENTORY_COMMAND 640
#define MENU_INVENTORY_USE 650
#define MENU_INVENTORY_DROP 660
#define MENU_INVENTORY_EXAMINE 3600

#define MENU_CAST_NPC 700
#define MENU_USEWITH_NPC 710
#define MENU_NPC_TALK 720
#define MENU_NPC_COMMAND 725
#define MENU_NPC_ATTACK1 715
#define MENU_NPC_ATTACK2 2715
#define MENU_NPC_EXAMINE 3700

#define MENU_CAST_PLAYER 800
#define MENU_USEWITH_PLAYER 810
#define MENU_PLAYER_ATTACK1 805
#define MENU_PLAYER_ATTACK2 2805
#define MENU_PLAYER_DUEL 2806
#define MENU_PLAYER_TRADE 2810
#define MENU_PLAYER_FOLLOW 2820

#define MENU_CAST_GROUND 900
#define MENU_WALK 920
#define MENU_CAST_SELF 1000

#define MENU_BANK_WITHDRAW 601
#define MENU_BANK_DEPOSIT 602

/* trade and duel */
#define MENU_TRANSACTION_OFFER 603
#define MENU_TRANSACTION_REMOVE 604

/* compass directions */
#define MENU_MAP_LOOK 101

/* wiki lookup */
#define MENU_WIKI_LOOKUP 102

#define MENU_CANCEL 4000

#define WIKI_TYPE_PAGE "Special:Lookup?type=%s&id=%d"

void mudclient_menu_item_click(mudclient *mud, int i);
void mudclient_create_top_mouse_menu(mudclient *mud);
void mudclient_menu_add_wiki(mudclient *mud, char *display, char *page);
void mudclient_menu_add_id_wiki(mudclient *mud, char *display, char *name,
                                int id);
void mudclient_menu_add_ground_item(mudclient *mud, int index);
void mudclient_create_right_click_menu(mudclient *mud);
void mudclient_draw_right_click_menu(mudclient *mud);

#endif
