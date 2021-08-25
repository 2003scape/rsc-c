#ifndef _H_GAME_DATA
#define _H_GAME_DATA

#include <stdint.h>
#include <string.h>

#include "utility.h"

extern char *game_data_model_name[5000];

extern int game_data_texture_count;
extern char **game_data_texture_name;
extern char **game_data_texture_subtype_name;

extern int game_data_object_count;
extern int *game_data_object_model_index;
extern int *game_data_object_width;
extern int *game_data_object_height;
extern int *game_data_object_type;
extern int *game_data_object_elevation;
extern char **game_data_object_name;
extern char **game_data_object_description;
extern char **game_data_object_command1;
extern char **game_data_object_command2;

extern int game_data_wall_object_count;
extern int *game_data_wall_object_height;
extern int *game_data_wall_object_texture_front;
extern int *game_data_wall_object_texture_back;
extern int *game_data_wall_object_adjacent;
extern int *game_data_wall_object_invisible;
extern char **game_data_wall_object_name;
extern char **game_data_wall_object_description;
extern char **game_data_wall_object_command1;
extern char **game_data_wall_object_command2;

extern int game_data_npc_count;
extern char **game_data_npc_name;
extern char **game_data_npc_description;
extern char **game_data_npc_command;
extern int *game_data_npc_width;
extern int *game_data_npc_height;
extern int *game_data_npc_sprite[12];
extern int *game_data_npc_colour_hair;
extern int *game_data_npc_colour_top;
extern int *game_data_npc_color_bottom;
extern int *game_data_npc_colour_skin;
extern int *game_data_npc_attack;
extern int *game_data_npc_strength;
extern int *game_data_npc_hits;
extern int *game_data_npc_defense;
extern int *game_data_npc_attackable;
extern int *game_data_npc_walk_model;
extern int *game_data_npc_combat_model;
extern int *game_data_npc_combat_animation;

extern int game_data_spell_count;
extern int *game_data_spell_level;
extern int *game_data_spell_runes_required;
extern int *game_data_spell_type;
extern int **game_data_spell_runes_id;
extern int **game_data_spell_runes_count;
extern char **game_data_spell_name;
extern char **game_data_spell_description;

/* TODO some of these types can be int8_t instead */
extern int game_data_item_count;
extern int game_data_item_sprite_count;
extern char **game_data_item_name;
extern char **game_data_item_description;
extern char **game_data_item_command;
extern int *game_data_item_picture;
extern int *game_data_item_base_price;
extern int *game_data_item_stackable;
extern int *game_data_item_unused;
extern int *game_data_item_wearable;
extern int *game_data_item_mask;
extern int *game_data_item_special;
extern int *game_data_item_members;

extern int game_data_tile_count;
extern int *game_data_tile_decoration;
extern int *game_data_tile_type;
extern int *game_data_tile_adjacent;

extern int game_data_animation_count;
extern int *game_data_animation_character_colour;
extern int *game_data_animation_gender;
extern int *game_data_animation_has_a;
extern int *game_data_animation_has_f;
extern int *game_data_animation_number;
extern char **game_data_animation_name;

extern int game_data_prayer_count;
extern int *game_data_prayer_level;
extern int *game_data_prayer_drain;
extern char **game_data_prayer_name;
extern char **game_data_prayer_description;

extern int game_data_roof_count;
extern int *game_data_roof_height;
extern int *game_data_roof_num_vertices;

extern int game_data_model_count;
extern int game_data_projectile_sprite;

extern char *game_data_data_string;
extern int8_t *game_data_data_integer;
extern int game_data_string_offset;
extern int game_data_offset;

#endif
