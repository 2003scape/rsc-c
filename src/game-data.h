#ifndef _H_GAME_DATA
#define _H_GAME_DATA

#include <stdint.h>

#include "utility.h"

extern char *model_name[5000];

extern int game_data_texture_count;
extern char **texture_name;
extern char **texture_subtype_name;

extern int object_count;
extern int *object_model_index;
extern int *object_width;
extern int *object_height;
extern int *object_type;
extern int *object_elevation;
extern char **object_name;
extern char **object_description;
extern char **object_command1;
extern char **object_command2;

extern int wall_object_count;
extern int *wall_object_height;
extern int *wall_object_texture_front;
extern int *wall_object_texture_back;
extern int *wall_object_adjacent;
extern int *wall_object_invisible;
extern char **wall_object_name;
extern char **wall_object_description;
extern char **wall_object_command1;
extern char **wall_object_command2;

extern int npc_count;
extern char **npc_name;
extern char **npc_description;
extern char **npc_command;
extern int *npc_width;
extern int *npc_height;
extern int *npc_sprite[12];
extern int *npc_colour_hair;
extern int *npc_colour_top;
extern int *npc_color_bottom;
extern int *npc_colour_skin;
extern int *npc_attack;
extern int *npc_strength;
extern int *npc_hits;
extern int *npc_defense;
extern int *npc_attackable;
extern int *npc_walk_model;
extern int *npc_combat_model;
extern int *npc_combat_animation;

extern int spell_count;
extern int *spell_level;
extern int *spell_runes_required;
extern int *spell_type;
extern int **spell_runes_id;
extern int **spell_runes_count;
extern char **spell_name;
extern char **spell_description;

/* TODO some of these types can be int8_t instead */
extern int item_count;
extern int item_sprite_count;
extern char **item_name;
extern char **item_description;
extern char **item_command;
extern int *item_picture;
extern int *item_base_price;
extern int *item_stackable;
extern int *item_unused;
extern int *item_wearable;
extern int *item_mask;
extern int *item_special;
extern int *item_members;

extern int tile_count;
extern int *tile_decoration;
extern int *tile_type;
extern int *tile_adjacent;

extern int animation_count;
extern int *animation_character_colour;
extern int *animation_gender;
extern int *animation_has_a;
extern int *animation_has_f;
extern int *animation_number;
extern char **animation_name;

extern int prayer_count;
extern int *prayer_level;
extern int *prayer_drain;
extern char **prayer_name;
extern char **prayer_description;

extern int roof_count;
extern int *roof_height;
extern int *roof_num_vertices;

extern int model_count;
extern int projectile_sprite;

extern int8_t data_string;
extern int8_t data_integer;
extern int string_offset;
extern int offset;

#endif
