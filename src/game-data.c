#include "game-data.h"

char *game_data_model_name[5000];

int game_data_texture_count;
char **game_data_texture_name;
char **game_data_texture_subtype_name;

int game_data_object_count;
int *game_data_object_model_index;
int *game_data_object_width;
int *game_data_object_height;
int *game_data_object_type;
int *game_data_object_elevation;
char **game_data_object_name;
char **game_data_object_description;
char **game_data_object_command1;
char **game_data_object_command2;

int game_data_wall_object_count;
int *game_data_wall_object_height;
int *game_data_wall_object_texture_front;
int *game_data_wall_object_texture_back;
int *game_data_wall_object_adjacent;
int *game_data_wall_object_invisible;
char **game_data_wall_object_name;
char **game_data_wall_object_description;
char **game_data_wall_object_command1;
char **game_data_wall_object_command2;

int game_data_npc_count;
char **game_data_npc_name;
char **game_data_npc_description;
char **game_data_npc_command;
int *game_data_npc_width;
int *game_data_npc_height;
int **game_data_npc_sprite;
int *game_data_npc_colour_hair;
int *game_data_npc_colour_top;
int *game_data_npc_color_bottom;
int *game_data_npc_colour_skin;
int *game_data_npc_attack;
int *game_data_npc_strength;
int *game_data_npc_hits;
int *game_data_npc_defense;
int *game_data_npc_attackable;
int *game_data_npc_walk_model;
int *game_data_npc_combat_model;
int *game_data_npc_combat_animation;

int game_data_spell_count;
int *game_data_spell_level;
int *game_data_spell_runes_required;
int *game_data_spell_type;
int **game_data_spell_runes_id;
int **game_data_spell_runes_count;
char **game_data_spell_name;
char **game_data_spell_description;

int game_data_item_count;
int game_data_item_sprite_count;
char **game_data_item_name;
char **game_data_item_description;
char **game_data_item_command;
int *game_data_item_picture;
int *game_data_item_base_price;
int *game_data_item_stackable;
int *game_data_item_unused;
int *game_data_item_wearable;
int *game_data_item_mask;
int *game_data_item_special;
int *game_data_item_members;

int game_data_tile_count;
int *game_data_tile_decoration;
int *game_data_tile_type;
int *game_data_tile_adjacent;

int game_data_animation_count;
int *game_data_animation_character_colour;
int *game_data_animation_gender;
int *game_data_animation_has_a;
int *game_data_animation_has_f;
int *game_data_animation_number;
char **game_data_animation_name;

int game_data_prayer_count;
int *game_data_prayer_level;
int *game_data_prayer_drain;
char **game_data_prayer_name;
char **game_data_prayer_description;

int game_data_roof_count;
int *game_data_roof_height;
int *game_data_roof_fills;

int game_data_model_count;
int game_data_projectile_sprite;

int8_t *game_data_data_string;
int8_t *game_data_data_integer; /* TODO use uint8_t */
int game_data_string_offset;
int game_data_offset;

int game_data_get_model_index(char *name) {
    {
        int i = 0;

        while (name[i]) {
            name[i] = tolower(name[i]);
            i++;
        }
    }

    if (strcmp(name, "na") == 0) {
        return 0;
    }

    for (int i = 0; i < game_data_model_count; i++) {
        if (strcmp(game_data_model_name[i], name) == 0) {
            return i;
        }
    }

    game_data_model_name[game_data_model_count++] = name;

    return game_data_model_count - 1;
}

int game_data_get_unsigned_byte() {
    return game_data_data_integer[game_data_offset++] & 0xff;
}

int game_data_get_unsigned_short() {
    int i = get_unsigned_short(game_data_data_integer, game_data_offset);
    game_data_offset += 2;
    return i;
}

int game_data_get_unsigned_int() {
    int i = get_unsigned_int(game_data_data_integer, game_data_offset);
    game_data_offset += 4;

    if (i > 99999999) {
        i = 99999999 - i;
    }

    return i;
}

char *game_data_get_string() {
    int start = game_data_string_offset;

    while (game_data_data_string[game_data_string_offset]) {
        game_data_string_offset++;
    }

    int end = game_data_string_offset + 1;

    char *string = malloc((end - start) * sizeof(char));
    memset(string, '\0', end - start);
    memcpy(string, game_data_data_string + start, end - start - 1);

    game_data_string_offset++;

    return string;
}

void game_data_load_data(int8_t *buffer, int is_members) {
    memset(game_data_model_name, 0, 5000 * sizeof(char *));

    game_data_data_string = load_data("string.dat", 0, buffer);
    game_data_string_offset = 0;
    game_data_data_integer = load_data("integer.dat", 0, buffer);
    game_data_offset = 0;

    int i = 0;

    game_data_item_count = game_data_get_unsigned_short();

    game_data_item_name = malloc(game_data_item_count * sizeof(char *));
    game_data_item_description = malloc(game_data_item_count * sizeof(char *));
    game_data_item_command = malloc(game_data_item_count * sizeof(char *));
    game_data_item_picture = malloc(game_data_item_count * sizeof(int));
    game_data_item_base_price = malloc(game_data_item_count * sizeof(int));
    game_data_item_stackable = malloc(game_data_item_count * sizeof(int));
    game_data_item_unused = malloc(game_data_item_count * sizeof(int));
    game_data_item_wearable = malloc(game_data_item_count * sizeof(int));
    game_data_item_mask = malloc(game_data_item_count * sizeof(int));
    game_data_item_special = malloc(game_data_item_count * sizeof(int));
    game_data_item_members = malloc(game_data_item_count * sizeof(int));

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_description[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_command[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_picture[i] = game_data_get_unsigned_short();

        if (game_data_item_picture[i] + 1 > game_data_item_sprite_count) {
            game_data_item_sprite_count = game_data_item_picture[i] + 1;
        }
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_base_price[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_stackable[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_unused[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_wearable[i] = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_mask[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_special[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_item_count; i++) {
        game_data_item_members[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_item_count; i++) {
        if (!is_members && game_data_item_members[i] == 1) {
            game_data_item_name[i] = "Members object";

            game_data_item_description[i] =
                "You need to be a member to use this object";

            game_data_item_base_price[i] = 0;
            game_data_item_command[i] = "";
            game_data_item_unused[0] = 0;
            game_data_item_wearable[i] = 0;
            game_data_item_special[i] = 1;
        }
    }

    game_data_npc_count = game_data_get_unsigned_short();
    game_data_npc_name = malloc(game_data_npc_count * sizeof(char *));
    game_data_npc_description = malloc(game_data_npc_count * sizeof(char *));
    game_data_npc_command = malloc(game_data_npc_count * sizeof(char *));
    game_data_npc_attack = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_strength = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_hits = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_defense = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_attackable = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_sprite = malloc(game_data_npc_count * sizeof(int *));
    game_data_npc_colour_hair = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_colour_top = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_color_bottom = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_colour_skin = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_width = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_height = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_walk_model = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_combat_model = malloc(game_data_npc_count * sizeof(int));
    game_data_npc_combat_animation = malloc(game_data_npc_count * sizeof(int));

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_description[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_attack[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_strength[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_hits[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_defense[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_attackable[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_sprite[i] = malloc(NPC_SPRITE_COUNT * sizeof(int));

        for (int j = 0; j < NPC_SPRITE_COUNT; j++) {
            game_data_npc_sprite[i][j] = game_data_get_unsigned_byte();

            if (game_data_npc_sprite[i][j] == 255) {
                game_data_npc_sprite[i][j] = -1;
            }
        }
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_colour_hair[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_colour_top[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_color_bottom[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_colour_skin[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_width[i] = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_height[i] = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_walk_model[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_combat_model[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_combat_animation[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_npc_count; i++) {
        game_data_npc_command[i] = game_data_get_string();
    }

    game_data_texture_count = game_data_get_unsigned_short();
    game_data_texture_name = malloc(game_data_texture_count * sizeof(char *));

    game_data_texture_subtype_name =
        malloc(game_data_texture_count * sizeof(char *));

    for (i = 0; i < game_data_texture_count; i++) {
        game_data_texture_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_texture_count; i++) {
        game_data_texture_subtype_name[i] = game_data_get_string();
    }

    game_data_animation_count = game_data_get_unsigned_short();

    game_data_animation_name =
        malloc(game_data_animation_count * sizeof(char *));

    game_data_animation_character_colour =
        malloc(game_data_animation_count * sizeof(int));

    game_data_animation_gender =
        malloc(game_data_animation_count * sizeof(int));

    game_data_animation_has_a = malloc(game_data_animation_count * sizeof(int));
    game_data_animation_has_f = malloc(game_data_animation_count * sizeof(int));

    game_data_animation_number =
        malloc(game_data_animation_count * sizeof(int));

    for (i = 0; i < game_data_animation_count; i++) {
        char *name = game_data_get_string();
        int j = 0;

        while (name[j]) {
            name[j] = tolower(name[j]);
            j++;
        }

        game_data_animation_name[i] = name;
    }

    for (i = 0; i < game_data_animation_count; i++) {
        game_data_animation_character_colour[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_animation_count; i++) {
        game_data_animation_gender[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_animation_count; i++) {
        game_data_animation_has_a[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_animation_count; i++) {
        game_data_animation_has_f[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_animation_count; i++) {
        game_data_animation_number[i] = game_data_get_unsigned_byte();
    }

    game_data_object_count = game_data_get_unsigned_short();
    game_data_object_name = malloc(game_data_object_count * sizeof(char *));

    game_data_object_description =
        malloc(game_data_object_count * sizeof(char *));

    game_data_object_command1 = malloc(game_data_object_count * sizeof(char *));
    game_data_object_command2 = malloc(game_data_object_count * sizeof(char *));
    game_data_object_model_index = malloc(game_data_object_count * sizeof(int));
    game_data_object_width = malloc(game_data_object_count * sizeof(int));
    game_data_object_height = malloc(game_data_object_count * sizeof(int));
    game_data_object_type = malloc(game_data_object_count * sizeof(int));
    game_data_object_elevation = malloc(game_data_object_count * sizeof(int));

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_description[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_command1[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_command2[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_object_count; i++) {
        char *model_name = game_data_get_string();

        int old_count = game_data_model_count;
        game_data_object_model_index[i] = game_data_get_model_index(model_name);

        /* we re-use an existing string if it's been called before */
        if (game_data_model_count == old_count) {
            free(model_name);
        }
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_width[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_height[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_type[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_object_count; i++) {
        game_data_object_elevation[i] = game_data_get_unsigned_byte();
    }

    game_data_wall_object_count = game_data_get_unsigned_short();

    game_data_wall_object_name =
        malloc(game_data_wall_object_count * sizeof(char *));

    game_data_wall_object_description =
        malloc(game_data_wall_object_count * sizeof(char *));

    game_data_wall_object_command1 =
        malloc(game_data_wall_object_count * sizeof(char *));

    game_data_wall_object_command2 =
        malloc(game_data_wall_object_count * sizeof(char *));

    game_data_wall_object_height =
        malloc(game_data_wall_object_count * sizeof(int));

    game_data_wall_object_texture_front =
        malloc(game_data_wall_object_count * sizeof(int));

    game_data_wall_object_texture_back =
        malloc(game_data_wall_object_count * sizeof(int));

    game_data_wall_object_adjacent =
        malloc(game_data_wall_object_count * sizeof(int));

    game_data_wall_object_invisible =
        malloc(game_data_wall_object_count * sizeof(int));

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_description[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_command1[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_command2[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_height[i] = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_texture_front[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_texture_back[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_adjacent[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_wall_object_count; i++) {
        game_data_wall_object_invisible[i] = game_data_get_unsigned_byte();
    }

    game_data_roof_count = game_data_get_unsigned_short();
    game_data_roof_height = malloc(game_data_roof_count * sizeof(int));
    game_data_roof_fills = malloc(game_data_roof_count * sizeof(int));

    for (i = 0; i < game_data_roof_count; i++) {
        game_data_roof_height[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_roof_count; i++) {
        game_data_roof_fills[i] = game_data_get_unsigned_byte();
    }

    game_data_tile_count = game_data_get_unsigned_short();
    game_data_tile_decoration = calloc(game_data_tile_count, sizeof(int));
    game_data_tile_type = calloc(game_data_tile_count, sizeof(int));
    game_data_tile_adjacent = calloc(game_data_tile_count, sizeof(int));

    for (i = 0; i < game_data_tile_count; i++) {
        game_data_tile_decoration[i] = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data_tile_count; i++) {
        game_data_tile_type[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_tile_count; i++) {
        game_data_tile_adjacent[i] = game_data_get_unsigned_byte();
    }

    game_data_projectile_sprite = game_data_get_unsigned_short();
    game_data_spell_count = game_data_get_unsigned_short();
    game_data_spell_name = malloc(game_data_spell_count * sizeof(char *));

    game_data_spell_description =
        malloc(game_data_spell_count * sizeof(char *));

    game_data_spell_level = malloc(game_data_spell_count * sizeof(int));

    game_data_spell_runes_required =
        malloc(game_data_spell_count * sizeof(int));

    game_data_spell_type = malloc(game_data_spell_count * sizeof(int));
    game_data_spell_runes_id = malloc(game_data_spell_count * sizeof(int *));
    game_data_spell_runes_count = malloc(game_data_spell_count * sizeof(int *));

    for (i = 0; i < game_data_spell_count; i++) {
        game_data_spell_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_spell_count; i++) {
        game_data_spell_description[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_spell_count; i++) {
        game_data_spell_level[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_spell_count; i++) {
        game_data_spell_runes_required[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_spell_count; i++) {
        game_data_spell_type[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_spell_count; i++) {
        int rune_amount = game_data_get_unsigned_byte();

        game_data_spell_runes_id[i] = malloc(rune_amount * sizeof(int));

        for (int j = 0; j < rune_amount; j++) {
            game_data_spell_runes_id[i][j] = game_data_get_unsigned_short();
        }
    }

    for (i = 0; i < game_data_spell_count; i++) {
        int rune_amount = game_data_get_unsigned_byte();

        game_data_spell_runes_count[i] = malloc(rune_amount * sizeof(int));

        for (int j = 0; j < rune_amount; j++) {
            game_data_spell_runes_count[i][j] = game_data_get_unsigned_byte();
        }
    }

    game_data_prayer_count = game_data_get_unsigned_short();
    game_data_prayer_name = malloc(game_data_prayer_count * sizeof(char *));

    game_data_prayer_description =
        malloc(game_data_prayer_count * sizeof(char *));

    game_data_prayer_level = malloc(game_data_prayer_count * sizeof(int));
    game_data_prayer_drain = malloc(game_data_prayer_count * sizeof(int));

    for (i = 0; i < game_data_prayer_count; i++) {
        game_data_prayer_name[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_prayer_count; i++) {
        game_data_prayer_description[i] = game_data_get_string();
    }

    for (i = 0; i < game_data_prayer_count; i++) {
        game_data_prayer_level[i] = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data_prayer_count; i++) {
        game_data_prayer_drain[i] = game_data_get_unsigned_byte();
    }

    free(game_data_data_string);
    free(game_data_data_integer);
}
