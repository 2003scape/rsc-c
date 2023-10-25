#ifndef _H_GAME_DATA
#define _H_GAME_DATA

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "utility.h"

#define NPC_SPRITE_COUNT 12
#define MAX_REQUIRED_RUNES 4

struct TextureConfig {
    char *name;
    char *subtype_name;
};

struct ObjectConfig {
    char *name;
    char *description;
    char *command1;
    char *command2;
    uint16_t model_index;
    uint8_t width;
    uint8_t height;
    uint8_t type;
    uint8_t elevation; /* surface elev for e.g. tables */
};

struct WallConfig {
    char *name;
    char *description;
    char *command1;
    char *command2;
    int texture_front;
    int texture_back;
    uint16_t height;
    uint8_t adjacent; /* blocks */
    uint8_t invisible;
};

struct NpcConfig {
    char *name;
    char *description;
    char *command;
    uint16_t width;
    uint16_t height;
    uint8_t walk_speed; /* animation playback speed */
    uint8_t combat_speed; /* animation playback speed */
    uint8_t combat_width; /* used for e.g. long animals like dragons */
    uint8_t attack;
    uint8_t strength;
    uint8_t hits;
    uint8_t defense;
    uint8_t attackable; /* 3 == aggressive, 2 == blocks? */
    int sprites[NPC_SPRITE_COUNT];
    int hair_colour;
    int top_colour;
    int bottom_colour;
    int skin_colour;
};

struct SpellReagentConfig {
    uint16_t id;
    uint8_t count;
};

struct SpellConfig {
    char *name;
    char *description;
    uint8_t level;
    uint8_t type;
    uint8_t runes_required;
    struct SpellReagentConfig runes[MAX_REQUIRED_RUNES];
};

struct ItemConfig {
    char *name;
    char *description;
    char *command;
    uint16_t sprite;
    uint32_t mask;
    uint32_t base_price;
    uint16_t wearable; /* equpipment slot */
    uint8_t stackable;
    uint8_t special; /* untradable */
    uint8_t members;
};

struct TileConfig {
    int decoration;
    uint8_t type;
    uint8_t adjacent; /* blocks */
};

struct AnimConfig {
    char *name;
    uint32_t colour;
    uint8_t gender;
    uint8_t has_a;
    uint8_t has_f;
    uint16_t number; /* picture ID */
};

struct PrayerConfig {
    char *name;
    char *description;
    uint8_t level;
    uint8_t drain;
};

struct RoofConfig {
    uint8_t height;
    uint8_t fill;
};

struct MudConfig {
    int texture_count;
    struct TextureConfig *textures;

    int wall_object_count;
    struct WallConfig *wall_objects;

    int object_count;
    struct ObjectConfig *objects;

    int npc_count;
    struct NpcConfig *npcs;

    int spell_count;
    struct SpellConfig *spells;

    int item_count;
    int item_sprite_count;
    struct ItemConfig *items;

    int tile_count;
    struct TileConfig *tiles;

    int animation_count;
    struct AnimConfig *animations;

    int prayer_count;
    struct PrayerConfig *prayers;

    int roof_count;
    struct RoofConfig *roofs;

    int model_count;
    int projectile_sprite;

    int8_t *data_string;
    size_t data_string_len;
    int8_t *data_integer;
    size_t data_integer_len;
    int string_offset;
    int offset;

    char *model_name[5000];
};

extern struct MudConfig game_data;
extern int game_data_get_model_index(char *name);
extern void game_data_load_data(int8_t *buffer, int is_members);

#endif
