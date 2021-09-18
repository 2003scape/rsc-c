#ifndef _H_GAME_CHARACTER
#define _H_GAME_CHARACTER

#include <stdint.h>
#include <string.h>

#define WAYPOINT_COUNT 10
#define EQUIP_COUNT 12

typedef struct GameCharacter {
    int64_t hash;
    char name[13];
    int server_index;
    int server_id;
    int current_x;
    int current_y;
    int npc_id;
    int step_count;
    int animation_current;
    int animation_next;
    int moving_step;
    int waypoint_current;
    int waypoints_x[WAYPOINT_COUNT];
    int waypoints_y[WAYPOINT_COUNT];
    int equipped_item[EQUIP_COUNT];
    char *message;
    int message_timeout;
    int bubble_item;
    int bubble_timeout;
    int damage_taken;
    int health_current;
    int health_max;
    int combat_timer;
    int level;
    int colour_hair;
    int colour_top;
    int colour_bottom;
    int colour_skin;
    int incoming_projectile_sprite;
    int attacking_player_server_index;
    int attacking_npc_server_index;
    int projectile_range;
    int skull_visible;
} GameCharacter;

void game_character_new(GameCharacter* game_character);

#endif
