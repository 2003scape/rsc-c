#ifndef _H_GAME_CHARACTER
#define _H_GAME_CHARACTER

#include <stdint.h>
#include <string.h>

/* animation order indices */
#define ANIMATION_COUNT 12

typedef enum {
    ANIMATION_INDEX_HEAD = 0,
    ANIMATION_INDEX_BODY = 1,
    ANIMATION_INDEX_LEGS = 2,
    ANIMATION_INDEX_LEFT_HAND = 3, /* shields */
    ANIMATION_INDEX_RIGHT_HAND = 4, /* swords */
    ANIMATION_INDEX_HEAD_OVERLAY = 5, /* med helms */
    ANIMATION_INDEX_BODY_OVERLAY = 6, /* chainmail */
    ANIMATION_INDEX_LEGS_OVERLAY = 7, /* skirts */
    ANIMATION_INDEX_8 = 8,
    ANIMATION_INDEX_BOOTS = 9,
    ANIMATION_INDEX_NECK = 10, /* amulets */
    ANIMATION_INDEX_CAPE = 11
} ANIMATION_INDEX;

extern ANIMATION_INDEX character_animation_array[8][ANIMATION_COUNT];

extern int character_walk_model[4];
extern int character_combat_model_array1[8];
extern int character_combat_model_array2[8];

extern int player_hair_colours[10];
extern int player_top_bottom_colours[15];
extern int player_skin_colours[5];

typedef struct GameCharacter GameCharacter;

#include "mudclient.h"
#include "utility.h"

#define WAYPOINT_COUNT 10

struct GameCharacter {
    int64_t encoded_username;
    char name[MAX_USER_LENGTH + 1];

    int server_index;
    int server_id;
    int current_x;
    int current_y;

    /* set to -1 for players */
    int npc_id;

    int step_count;
    int moving_step;
    int waypoint_current;
    int waypoints_x[WAYPOINT_COUNT];
    int waypoints_y[WAYPOINT_COUNT];

    int current_animation;
    int next_animation;

    /* chat message above head */
    char message[255];
    int message_timeout;

    /* action bubble */
    int bubble_item;
    int bubble_timeout;

    int damage_taken;
    int current_hits;
    int max_hits;
    int combat_timer;
    int level;

    int hair_colour;
    int top_colour;
    int bottom_colour;
    int skin_colour;
    ANIMATION_INDEX animations[ANIMATION_COUNT];

    int incoming_projectile_sprite;
    int attacking_player_server_index;
    int attacking_npc_server_index;
    int projectile_range;

    int8_t skull_visible;
};

void game_character_new(GameCharacter* game_character);
void game_character_move(GameCharacter *character);

#endif
