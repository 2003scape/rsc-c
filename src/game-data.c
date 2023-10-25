#include "game-data.h"

struct MudConfig game_data;

int game_data_get_model_index(char *name) {
    strtolower(name);

    if (strcmp(name, "na") == 0) {
        return 0;
    }

    for (int i = 0; i < game_data.model_count; i++) {
        if (strcmp(game_data.model_name[i], name) == 0) {
            return i;
        }
    }

    game_data.model_name[game_data.model_count++] = name;

    return game_data.model_count - 1;
}

static int game_data_get_unsigned_byte() {
    return get_unsigned_byte(game_data.data_integer, game_data.offset++,
                             game_data.data_integer_len);
}

static int game_data_get_unsigned_short() {
    int i = get_unsigned_short(game_data.data_integer, game_data.offset,
                               game_data.data_integer_len);
    game_data.offset += 2;
    return i;
}

static int game_data_get_unsigned_int() {
    int i = get_unsigned_int(game_data.data_integer, game_data.offset,
                             game_data.data_integer_len);
    game_data.offset += 4;

    if (i > 99999999) {
        i = 99999999 - i;
    }

    return i;
}

static char *game_data_get_string() {
    int start = game_data.string_offset;

    while (game_data.data_string[game_data.string_offset]) {
        game_data.string_offset++;
    }

    int end = game_data.string_offset + 1;

    char *string = malloc((end - start) * sizeof(char));
    memset(string, '\0', end - start);
    memcpy(string, game_data.data_string + start, end - start - 1);

    game_data.string_offset++;

    return string;
}

void game_data_load_data(int8_t *buffer, int is_members) {
    memset(game_data.model_name, 0, 5000 * sizeof(char *));

    game_data.data_string = load_data("string.dat", 0, buffer,
                                      &game_data.data_string_len);
    game_data.string_offset = 0;
    game_data.data_integer = load_data("integer.dat", 0, buffer,
                                       &game_data.data_integer_len);
    game_data.offset = 0;

    int i = 0;

    game_data.item_count = game_data_get_unsigned_short();

    game_data.items = calloc(game_data.item_count, sizeof(struct ItemConfig));

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].description = game_data_get_string();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].command = game_data_get_string();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].sprite = game_data_get_unsigned_short();

        if (game_data.items[i].sprite + 1 > game_data.item_sprite_count) {
            game_data.item_sprite_count = game_data.items[i].sprite + 1;
        }
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].base_price = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].stackable = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data_get_unsigned_byte(); /* unused */
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].wearable = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].mask = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].special = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.item_count; i++) {
        game_data.items[i].members = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.item_count; i++) {
        if (!is_members && game_data.items[i].members == 1) {
            free(game_data.items[i].name);
            game_data.items[i].name = "Members object";

            free(game_data.items[i].description);

            game_data.items[i].description =
                "You need to be a member to use this object";

            game_data.items[i].base_price = 0;

            free(game_data.items[i].command);
            game_data.items[i].command = "";

            game_data.items[i].wearable = 0;
            game_data.items[i].special = 1;
        }
    }

    game_data.npc_count = game_data_get_unsigned_short();
    game_data.npcs = calloc(game_data.npc_count, sizeof(struct NpcConfig));

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].description = game_data_get_string();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].attack = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].strength = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].hits = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].defense = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].attackable = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        for (int j = 0; j < NPC_SPRITE_COUNT; j++) {
            game_data.npcs[i].sprites[j] = game_data_get_unsigned_byte();

            if (game_data.npcs[i].sprites[j] == 255) {
                game_data.npcs[i].sprites[j] = -1;
            }
        }
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].hair_colour = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].top_colour = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].bottom_colour = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].skin_colour = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].width = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].height = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].walk_speed = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].combat_speed = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].combat_width = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.npc_count; i++) {
        game_data.npcs[i].command = game_data_get_string();
    }

    game_data.texture_count = game_data_get_unsigned_short();
    game_data.textures = calloc(game_data.texture_count, sizeof(struct TextureConfig));

    for (i = 0; i < game_data.texture_count; i++) {
        game_data.textures[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.texture_count; i++) {
        game_data.textures[i].subtype_name = game_data_get_string();
    }

    game_data.animation_count = game_data_get_unsigned_short();

    game_data.animations =
        calloc(game_data.animation_count, sizeof(struct AnimConfig));

    for (i = 0; i < game_data.animation_count; i++) {
        char *name = game_data_get_string();
        strtolower(name);

        game_data.animations[i].name = name;
    }

    for (i = 0; i < game_data.animation_count; i++) {
        game_data.animations[i].colour = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.animation_count; i++) {
        game_data.animations[i].gender = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.animation_count; i++) {
        game_data.animations[i].has_a = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.animation_count; i++) {
        game_data.animations[i].has_f = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.animation_count; i++) {
        game_data.animations[i].number = game_data_get_unsigned_byte();
    }

    game_data.object_count = game_data_get_unsigned_short();
    game_data.objects = calloc(game_data.object_count, sizeof(struct ObjectConfig));

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].description = game_data_get_string();
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].command1 = game_data_get_string();
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].command2 = game_data_get_string();
    }

    for (i = 0; i < game_data.object_count; i++) {
        char *model_name = game_data_get_string();

        int old_count = game_data.model_count;
        game_data.objects[i].model_index = game_data_get_model_index(model_name);

        /* we re-use an existing string if it's been called before */
        if (game_data.model_count == old_count) {
            free(model_name);
        }
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].width = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].height = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].type = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.object_count; i++) {
        game_data.objects[i].elevation = game_data_get_unsigned_byte();
    }

    game_data.wall_object_count = game_data_get_unsigned_short();

    game_data.wall_objects =
        calloc(game_data.wall_object_count, sizeof(struct WallConfig));

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].description = game_data_get_string();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].command1 = game_data_get_string();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].command2 = game_data_get_string();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].height = game_data_get_unsigned_short();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].texture_front = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].texture_back = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].adjacent = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.wall_object_count; i++) {
        game_data.wall_objects[i].invisible = game_data_get_unsigned_byte();
    }

    game_data.roof_count = game_data_get_unsigned_short();
    game_data.roofs = calloc(game_data.roof_count, sizeof(struct RoofConfig));

    for (i = 0; i < game_data.roof_count; i++) {
        game_data.roofs[i].height = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.roof_count; i++) {
        game_data.roofs[i].fill = game_data_get_unsigned_byte();
    }

    game_data.tile_count = game_data_get_unsigned_short();
    game_data.tiles = calloc(game_data.tile_count, sizeof(struct TileConfig));

    for (i = 0; i < game_data.tile_count; i++) {
        game_data.tiles[i].decoration = game_data_get_unsigned_int();
    }

    for (i = 0; i < game_data.tile_count; i++) {
        game_data.tiles[i].type = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.tile_count; i++) {
        game_data.tiles[i].adjacent = game_data_get_unsigned_byte();
    }

    game_data.projectile_sprite = game_data_get_unsigned_short();
    game_data.spell_count = game_data_get_unsigned_short();
    game_data.spells = calloc(game_data.spell_count, sizeof(struct SpellConfig));

    for (i = 0; i < game_data.spell_count; i++) {
        game_data.spells[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.spell_count; i++) {
        game_data.spells[i].description = game_data_get_string();
    }

    for (i = 0; i < game_data.spell_count; i++) {
        game_data.spells[i].level = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.spell_count; i++) {
        game_data.spells[i].runes_required = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.spell_count; i++) {
        game_data.spells[i].type = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.spell_count; i++) {
        int rune_amount = game_data_get_unsigned_byte();

        for (int j = 0; j < rune_amount; j++) {
            game_data.spells[i].runes[j].id = game_data_get_unsigned_short();
        }
    }

    for (i = 0; i < game_data.spell_count; i++) {
        int rune_amount = game_data_get_unsigned_byte();

        for (int j = 0; j < rune_amount; j++) {
            game_data.spells[i].runes[j].count = game_data_get_unsigned_byte();
        }
    }

    game_data.prayer_count = game_data_get_unsigned_short();
    game_data.prayers = calloc(game_data.prayer_count, sizeof(struct PrayerConfig));

    for (i = 0; i < game_data.prayer_count; i++) {
        game_data.prayers[i].name = game_data_get_string();
    }

    for (i = 0; i < game_data.prayer_count; i++) {
        game_data.prayers[i].description = game_data_get_string();
    }

    for (i = 0; i < game_data.prayer_count; i++) {
        game_data.prayers[i].level = game_data_get_unsigned_byte();
    }

    for (i = 0; i < game_data.prayer_count; i++) {
        game_data.prayers[i].drain = game_data_get_unsigned_byte();
    }

    free(game_data.data_string);
    free(game_data.data_integer);
}
