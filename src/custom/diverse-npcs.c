#include <assert.h>
#include <stdbool.h>

#include "../game-character.h"
#include "../game-data.h"

#include "diverse-npcs.h"

/*
 * Due to either awkward tooling or time constraints, many of the
 * citizens of RuneScape Classic are quite bland and similar.
 *
 * The purpose of this mod is to make them more interesting but
 * still maintain the aesthetic of the game and its regions.
 *
 * Only NPCs that exist in mass numbers are modified.
 *
 * Another aim is to keep all NPCs immediately recognizable to
 * experienced players.
 *
 * When adding new weapons to NPCs, use their drop tables as a
 * hint for what they should wield.
 *
 * Regional variation is also encouraged. For example, Guards
 * from Ardougne use slightly more expensive weapons than their
 * counterparts in Varrock due to better funding. They are also
 * more dark-skinned due to the connection with Brimhaven.
 */

static bool has_diverse_npcs = false;
static int man_start = -1;
static int man_ardy_start = -1;
static int man_al_kharid_start = -1;
static int guard_start = -1;
static int barbarian_start = -1;
static int banker_start = -1;
static int pirate_start = -1;
static int pirate_brimhaven_start = -1;
static int paladin_start = -1;
static int dark_warrior_start = -1;
static int guard_ardy_start = -1;
static int gnome_banker_start = -1;
static int giant_start = -1;
static int moss_giant_start = -1;
static int ice_giant_start = -1;
static int fire_giant_start = -1;
static int rogue_start = -1;
static int black_knight_start = -1;
static int black_knight_aggr_start = -1;
static int gnome_troop_start = -1;
static int khazard_troop_start = -1;
static int chaos_druid_start = -1;
static int druid_start = -1;
static int darkwizard_start = -1;
static int darkwizard2_start = -1;
static int al_kharid_banker_start = -1;
static int fairy_banker_start = -1;
static int tribesman_start = -1;
static int jungle_savage_start = -1;
static int earth_warrior_start = -1;
static int thug_start = -1;

#define NPC_MAN                         (11)
#define NPC_MAN_AL_KHARID               (72)
#define NPC_MAN_ARDOUGNE                (318)
#define NPC_DARKWIZARD                  (57)
#define NPC_DARKWIZARD2                 (60)
#define NPC_GIANT                       (61)
#define NPC_GUARD                       (65)
#define NPC_BLACK_KNIGHT                (66)
#define NPC_BLACK_KNIGHT_AGGR           (189)
#define NPC_BARBARIAN                   (76)
#define NPC_BANKER                      (95)
#define NPC_MOSS_GIANT                  (104)
#define NPC_ICE_GIANT                   (135)
#define NPC_PIRATE                      (137)
#define NPC_DARK_WARRIOR                (199)
#define NPC_DRUID                       (200)
#define NPC_FAIRY_BANKER                (224)
#define NPC_THUG                        (251)
#define NPC_PIRATE_BRIMHAVEN            (264)
#define NPC_BANKER_AL_KHARID            (268)
#define NPC_CHAOS_DRUID                 (270)
#define NPC_GUARD_ARDOUGNE              (321)
#define NPC_PALADIN                     (323)
#define NPC_ROGUE                       (342)
#define NPC_FIRE_GIANT                  (344)
#define NPC_KHAZARD_TROOP               (407)
#define NPC_GNOME_TROOP                 (409)
#define NPC_TRIBESMAN                   (421)
#define NPC_GNOME_BANKER                (540)
#define NPC_EARTH_WARRIOR               (584)
#define NPC_JUNGLE_SAVAGE               (776)

#define ANIM_HEAD1                      (0)
#define ANIM_LEGS1                      (2)
#define ANIM_FHEAD1                     (3)
#define ANIM_FBODY1                     (4)
#define ANIM_HEAD2                      (5) /* ruddy face */
#define ANIM_HEAD3                      (6) /* beard */
#define ANIM_SWORD_BRONZE               (47)
#define ANIM_SWORD_ADAMANTITE           (51)
#define ANIM_SWORD_BLACK                (53)
#define ANIM_PLATEMAIL_TOP_IRON         (55)
#define ANIM_SKIRT_BROWN                (87)
#define ANIM_SHIELD_IRON                (98)
#define ANIM_CROSSBOW                   (106)
#define ANIM_LONGBOW                    (107)
#define ANIM_AXE_BRONZE                 (108)
#define ANIM_AXE_IRON                   (109)
#define ANIM_AXE_STEEL                  (110)
#define ANIM_MACE_BRONZE                (115)
#define ANIM_MACE_IRON                  (116)
#define ANIM_MACE_MITHRIL               (118)
#define ANIM_MACE_BLACK                 (121)
#define ANIM_STAFF                      (122)
#define ANIM_SKIRT_BLACK                (158)
#define ANIM_SPEAR                      (180)

#define SKIN_COLOUR_TAN1                (0xCCB366)
#define SKIN_COLOUR_TAN2                (0xB38C40)
#define SKIN_COLOUR_DARK1               (0x906020)
#define SKIN_COLOUR_DARK2               (0x6F5737)

#define HAIR_COLOUR_YELLOW              (0xFFC030)
#define HAIR_COLOUR_BROWN               (0x805030)
#define HAIR_COLOUR_RED                 (0xFF4000)
#define HAIR_COLOUR_ORANGE              (0xFFA040)

static void
add_diverse_npcs(void) {
    struct NpcConfig npc;

    game_data.npcs = realloc(game_data.npcs,
        (game_data.npc_count + 100) * sizeof(struct NpcConfig));
    assert(game_data.npcs != NULL);

    /* Men: can't make them women due to chat dialogue... */

    man_start = game_data.npc_count;

    npc = game_data.npcs[NPC_MAN];
    npc.hair_colour = HAIR_COLOUR_ORANGE;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD3;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_MAN];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.hair_colour = HAIR_COLOUR_BROWN;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD3;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_MAN];
    npc.hair_colour = HAIR_COLOUR_RED;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD2;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Men from Al Kharid: also can't make them women */

    man_al_kharid_start = game_data.npc_count;

    npc = game_data.npcs[NPC_MAN_AL_KHARID];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD3;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Men from Ardougne: also can't make them women */

    man_ardy_start = game_data.npc_count;

    npc = game_data.npcs[NPC_MAN_ARDOUGNE];
    npc.hair_colour = HAIR_COLOUR_YELLOW;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_MAN_ARDOUGNE];
    npc.hair_colour = HAIR_COLOUR_ORANGE;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD2;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_MAN_ARDOUGNE];
    npc.skin_colour = SKIN_COLOUR_DARK2;
    npc.hair_colour = HAIR_COLOUR_BROWN;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD3;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Guards: melee */

    guard_start = game_data.npc_count;

    npc = game_data.npcs[NPC_GUARD];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SWORD_BRONZE;
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_SHIELD_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_LONGBOW;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD];
    npc.skin_colour= SKIN_COLOUR_TAN1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SWORD_BRONZE;
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_SHIELD_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_LONGBOW;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Barbarians */

    barbarian_start = game_data.npc_count;

    npc = game_data.npcs[NPC_BARBARIAN];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BARBARIAN];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BARBARIAN];
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Bankers */

    banker_start = game_data.npc_count;

    npc = game_data.npcs[NPC_BANKER];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BANKER];
    npc.description = "She can look after my money";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BANKER];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.description = "She can look after my money";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Pirates! You would expect them to be well travelled... */

    pirate_start = game_data.npc_count;

    npc = game_data.npcs[NPC_PIRATE];
    npc.skin_colour = SKIN_COLOUR_TAN2;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE];
    npc.skin_colour = SKIN_COLOUR_DARK2;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE];
    npc.skin_colour = SKIN_COLOUR_DARK1;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Dark warrior */

    dark_warrior_start = game_data.npc_count;

    npc = game_data.npcs[NPC_DARK_WARRIOR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_DARK_WARRIOR];
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_DARK_WARRIOR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_BLACK;
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Paladin */

    paladin_start = game_data.npc_count;

    npc = game_data.npcs[NPC_PALADIN];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_PLATEMAIL_TOP_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Brimhaven pirates */

    pirate_brimhaven_start = game_data.npc_count;

    npc = game_data.npcs[NPC_PIRATE_BRIMHAVEN];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE_BRIMHAVEN];
    npc.skin_colour = SKIN_COLOUR_DARK1;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE_BRIMHAVEN];
    npc.skin_colour = SKIN_COLOUR_DARK2;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_HEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE_BRIMHAVEN];
    npc.skin_colour = SKIN_COLOUR_DARK1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_PIRATE_BRIMHAVEN];
    npc.skin_colour = SKIN_COLOUR_DARK2;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Ardougne guards. Contact with Karamja and higher budget */

    guard_ardy_start = game_data.npc_count;

    npc = game_data.npcs[NPC_GUARD_ARDOUGNE];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD_ARDOUGNE];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD_ARDOUGNE];
    npc.skin_colour = SKIN_COLOUR_DARK1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_CROSSBOW;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GUARD_ARDOUGNE];
    npc.skin_colour = SKIN_COLOUR_DARK2;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_CROSSBOW;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Gnome banker */

    gnome_banker_start = game_data.npc_count;

    npc = game_data.npcs[NPC_GNOME_BANKER];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Giant */

    giant_start = game_data.npc_count;

    npc = game_data.npcs[NPC_GIANT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_STAFF;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GIANT];
    npc.name = "Giantess";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_STAFF;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GIANT];
    npc.name = "Giantess";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Moss Giant */

    moss_giant_start = game_data.npc_count;

    npc = game_data.npcs[NPC_MOSS_GIANT];
    npc.name = "Moss Giantess";
    npc.description = "Her hair seems to have a life of its own";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Ice Giant */

    ice_giant_start = game_data.npc_count;

    npc = game_data.npcs[NPC_ICE_GIANT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_MITHRIL;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_ICE_GIANT];
    npc.name = "Ice Giantess";
    npc.description = "She's got icicles in her hair";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_MITHRIL;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_ICE_GIANT];
    npc.name = "Ice Giantess";
    npc.description = "She's got icicles in her hair";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Fire Giant */

    fire_giant_start = game_data.npc_count;

    npc = game_data.npcs[NPC_FIRE_GIANT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_STEEL;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_FIRE_GIANT];
    npc.name = "Fire Giantess";
    npc.description = "A big woman with red glowing skin";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_FIRE_GIANT];
    npc.name = "Fire Giantess";
    npc.description = "A big woman with red glowing skin";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_STEEL;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Rogue (also can't be changed to female due to dialogue) */

    rogue_start = game_data.npc_count;

    npc = game_data.npcs[NPC_ROGUE];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_ROGUE];
    npc.skin_colour = SKIN_COLOUR_TAN2;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_ROGUE];
    npc.skin_colour = SKIN_COLOUR_DARK1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Black Knight */

    black_knight_start = game_data.npc_count;

    npc = game_data.npcs[NPC_BLACK_KNIGHT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SWORD_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT];
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_BLACK;
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SWORD_BLACK;
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Black Knight (aggressive) */

    black_knight_aggr_start = game_data.npc_count;

    npc = game_data.npcs[NPC_BLACK_KNIGHT_AGGR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT_AGGR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SWORD_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT_AGGR];
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT_AGGR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_MACE_BLACK;
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_BLACK_KNIGHT_AGGR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SWORD_BLACK;
    npc.sprites[ANIMATION_INDEX_LEGS] = ANIM_LEGS1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Gnome troop */

    gnome_troop_start = game_data.npc_count;

    npc = game_data.npcs[NPC_GNOME_TROOP];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_CROSSBOW;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GNOME_TROOP];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_GNOME_TROOP];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_CROSSBOW;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Khazard troop */

    khazard_troop_start = game_data.npc_count;

    npc = game_data.npcs[NPC_KHAZARD_TROOP];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_STEEL;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_KHAZARD_TROOP];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_KHAZARD_TROOP];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_STEEL;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Druid */

    druid_start = game_data.npc_count;

    npc = game_data.npcs[NPC_DRUID];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_STAFF;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_DRUID];
    npc.name = "Druidess";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_DRUID];
    npc.name = "Druidess";
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_STAFF;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Chaos druid */

    chaos_druid_start = game_data.npc_count;

    npc = game_data.npcs[NPC_CHAOS_DRUID];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_STAFF;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_CHAOS_DRUID];
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_CHAOS_DRUID];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_STAFF;
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Darkwizard */

    darkwizard_start = game_data.npc_count;

    npc = game_data.npcs[NPC_DARKWIZARD];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_DARKWIZARD];
    npc.description = "They work evil magic";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    game_data.npcs[game_data.npc_count++] = npc;

    darkwizard2_start = game_data.npc_count;

    npc = game_data.npcs[NPC_DARKWIZARD2];
    npc.skin_colour = SKIN_COLOUR_TAN2;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_DARKWIZARD];
    npc.skin_colour = SKIN_COLOUR_DARK2;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Banker (Al Kharid) */

    al_kharid_banker_start = game_data.npc_count;

    npc = game_data.npcs[NPC_BANKER_AL_KHARID];
    npc.description = "She can look after my money";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BLACK;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Fairy banker */

    fairy_banker_start = game_data.npc_count;

    npc = game_data.npcs[NPC_FAIRY_BANKER];
    npc.description = "She can look after my money";
    npc.sprites[ANIMATION_INDEX_HEAD] = ANIM_FHEAD1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Tribesman */

    tribesman_start = game_data.npc_count;

    /* MODIFIED to not be racist */
    game_data.npcs[NPC_TRIBESMAN].description = "A warrior from Karamja";

    npc = game_data.npcs[NPC_TRIBESMAN];
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_LONGBOW;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = -1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_TRIBESMAN];
    npc.name = "Tribeswoman";
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BROWN;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_TRIBESMAN];
    npc.name = "Tribeswoman";
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_LONGBOW;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = -1;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BROWN;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Jungle Savage */

    jungle_savage_start = game_data.npc_count;

    /* MODIFIED to not be racist */
    game_data.npcs[NPC_JUNGLE_SAVAGE].name = "Tormented Warrior";
    game_data.npcs[NPC_JUNGLE_SAVAGE].description = "A Kharazi warrior, tormented by evil spirits.";

    /* By the way, Karamja has Adamantite deposits. */

    npc = game_data.npcs[NPC_JUNGLE_SAVAGE];
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_SWORD_ADAMANTITE;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_JUNGLE_SAVAGE];
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_SPEAR;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BROWN;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_JUNGLE_SAVAGE];
    npc.sprites[ANIMATION_INDEX_LEFT_HAND] = ANIM_SWORD_ADAMANTITE;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    npc.sprites[ANIMATION_INDEX_LEGS_OVERLAY] = ANIM_SKIRT_BROWN;
    game_data.npcs[game_data.npc_count++] = npc;

    earth_warrior_start = game_data.npc_count;

    npc = game_data.npcs[NPC_EARTH_WARRIOR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SPEAR;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_EARTH_WARRIOR];
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_EARTH_WARRIOR];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_SPEAR;
    npc.sprites[ANIMATION_INDEX_BODY] = ANIM_FBODY1;
    npc.sprites[ANIMATION_INDEX_BODY_OVERLAY] = -1;
    game_data.npcs[game_data.npc_count++] = npc;

    /* Thug */

    thug_start = game_data.npc_count;

    npc = game_data.npcs[NPC_THUG];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_THUG];
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;

    npc = game_data.npcs[NPC_THUG];
    npc.skin_colour = SKIN_COLOUR_TAN1;
    npc.sprites[ANIMATION_INDEX_RIGHT_HAND] = ANIM_AXE_IRON;
    game_data.npcs[game_data.npc_count++] = npc;
}

static int
roll_isaac(uint32_t seed, uint32_t max)
{
    struct isaac rng = {0};

    rng.randrsl[0] = seed;
    isaac_init(&rng, 1);
    return isaac_next(&rng) % max;
}

int
diversify_npc(int id, int server_index, int x, int y) {
    uint32_t r;

    (void)x;
    (void)y;
    if (!has_diverse_npcs) {
        add_diverse_npcs();
        has_diverse_npcs = true;
    }
    switch (id) {
        case NPC_MAN:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_MAN : man_start + (r - 1);
        case NPC_MAN_ARDOUGNE:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_MAN_ARDOUGNE : man_ardy_start + (r - 1);
        case NPC_MAN_AL_KHARID:
            r = roll_isaac(server_index, 2);
            return r == 0 ? NPC_MAN_AL_KHARID : man_al_kharid_start + (r - 1);
        case NPC_GUARD:
            r = roll_isaac(server_index, 6);
            return r == 0 ? NPC_GUARD : guard_start + (r - 1);
        case NPC_BARBARIAN:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_BARBARIAN : barbarian_start + (r - 1);
        case NPC_BANKER:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_BANKER : banker_start + (r - 1);
        case NPC_PIRATE:
            r = roll_isaac(server_index, 6);
            return r == 0 ? NPC_PIRATE : pirate_start + (r - 1);
        case NPC_DARK_WARRIOR:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_DARK_WARRIOR : dark_warrior_start + (r - 1);
        case NPC_PIRATE_BRIMHAVEN:
            r = roll_isaac(server_index, 6);
            return r == 0 ? NPC_PIRATE_BRIMHAVEN: pirate_brimhaven_start + (r - 1);
        case NPC_GUARD_ARDOUGNE:
            r = roll_isaac(server_index, 5);
            return r == 0 ? NPC_GUARD_ARDOUGNE : guard_ardy_start + (r - 1);
        case NPC_PALADIN:
            r = roll_isaac(server_index, 2);
            return r == 0 ? NPC_PALADIN : paladin_start + (r - 1);
        case NPC_GNOME_BANKER:
            r = roll_isaac(server_index, 2);
            return r == 0 ? NPC_GNOME_BANKER : gnome_banker_start + (r - 1);
        case NPC_GIANT:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_GIANT : giant_start + (r - 1);
        case NPC_MOSS_GIANT:
            r = roll_isaac(server_index, 2);
            return r == 0 ? NPC_MOSS_GIANT : moss_giant_start + (r - 1);
        case NPC_ICE_GIANT:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_ICE_GIANT : ice_giant_start + (r - 1);
        case NPC_FIRE_GIANT:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_FIRE_GIANT : fire_giant_start + (r - 1);
        case NPC_ROGUE:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_ROGUE : rogue_start + (r - 1);
        case NPC_BLACK_KNIGHT:
            r = roll_isaac(server_index, 6);
            return r == 0 ? NPC_BLACK_KNIGHT : black_knight_start + (r - 1);
        case NPC_BLACK_KNIGHT_AGGR:
            r = roll_isaac(server_index, 6);
            return r == 0 ? NPC_BLACK_KNIGHT_AGGR : black_knight_aggr_start + (r - 1);
        case NPC_KHAZARD_TROOP:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_KHAZARD_TROOP : khazard_troop_start + (r - 1);
        case NPC_GNOME_TROOP:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_GNOME_TROOP: gnome_troop_start + (r - 1);
        case NPC_DRUID:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_DRUID : druid_start + (r - 1);
        case NPC_CHAOS_DRUID:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_CHAOS_DRUID : chaos_druid_start + (r - 1);
        case NPC_DARKWIZARD:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_DARKWIZARD : darkwizard_start + (r - 1);
        case NPC_DARKWIZARD2:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_DARKWIZARD : darkwizard_start + (r - 1);
        case NPC_BANKER_AL_KHARID:
            r = roll_isaac(server_index, 2);
            return r == 0 ? NPC_BANKER_AL_KHARID : al_kharid_banker_start + (r - 1);
        case NPC_FAIRY_BANKER:
            r = roll_isaac(server_index, 2);
            return r == 0 ? NPC_FAIRY_BANKER : fairy_banker_start + (r - 1);
        case NPC_TRIBESMAN:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_TRIBESMAN : tribesman_start + (r - 1);
        case NPC_JUNGLE_SAVAGE:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_JUNGLE_SAVAGE : jungle_savage_start + (r - 1);
        case NPC_EARTH_WARRIOR:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_EARTH_WARRIOR : earth_warrior_start + (r - 1);
        case NPC_THUG:
            r = roll_isaac(server_index, 4);
            return r == 0 ? NPC_THUG : thug_start + (r - 1);
    }
    return id;
}
