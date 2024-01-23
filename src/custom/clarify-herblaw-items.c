#include "clarify-herblaw-items.h"

void modify_unidentified_herbs(void) {
    /* citing https://oldschool.runescape.wiki/w/Gielinor's_flora_-_herbs */

    if (game_data.item_count < 454) {
        /* pre-herblaw config.jag, most likely */
        return;
    }

    free(game_data.items[165].name);
    game_data.items[165].name = mud_strdup("Unidentified guam leaf");
    free(game_data.items[165].description);
    game_data.items[165].description =
        mud_strdup("It can be extracted to improve coordination, but I need a closer look to not poison myself");

    free(game_data.items[435].name);
    game_data.items[435].name = mud_strdup("Unidentified marrentill");
    free(game_data.items[435].description);
    game_data.items[435].description =
        mud_strdup("I think it's a poison cure, but I need a closer look to not poison myself");

    free(game_data.items[436].name);
    game_data.items[436].name = mud_strdup("Unidentified tarromin");
    free(game_data.items[436].description);
    game_data.items[436].description =
        mud_strdup("It can be extracted to improve strength, but I need a closer look to not poison myself");

    free(game_data.items[437].name);
    game_data.items[437].name = mud_strdup("Unidentified harralander");
    free(game_data.items[437].description);
    game_data.items[437].description =
        mud_strdup("I think it's harralander, but I need a closer look to not poison myself");

    free(game_data.items[438].name);
    game_data.items[438].name = mud_strdup("Unidentified ranarr weed");
    free(game_data.items[438].description);
    game_data.items[438].description =
        mud_strdup("A floral weed, I need a closer look to process it into something useful");

    free(game_data.items[439].name);
    game_data.items[439].name = mud_strdup("Unidentified irit leaf");
    free(game_data.items[439].description);
    game_data.items[439].description =
        mud_strdup("A leafy herb, but I need a closer look to not poison myself");

    free(game_data.items[440].name);
    game_data.items[440].name = mud_strdup("Unidentified avantoe");
    free(game_data.items[440].description);
    game_data.items[440].description =
        mud_strdup("I think it's avantoe, but I need a closer look to not poison myself");

    free(game_data.items[441].name);
    game_data.items[441].name = mud_strdup("Unidentified kwuarm");
    free(game_data.items[441].description);
    game_data.items[441].description =
        mud_strdup("It's leaking foul smelling sap, I need to process it very carefully");

    free(game_data.items[442].name);
    game_data.items[442].name = mud_strdup("Unidentified cadantine");
    free(game_data.items[442].description);
    game_data.items[442].description =
        mud_strdup("I think it's cadantine, but I need a closer look to not poison myself");

    free(game_data.items[443].name);
    game_data.items[443].name = mud_strdup("Unidentified dwarf weed");
    free(game_data.items[443].description);
    game_data.items[443].description =
        mud_strdup("It smells like dwarven settlements, but I need a closer look to not poison myself");

    /* torstol */
    if (game_data.item_count >= 933) {
        free(game_data.items[933].name);
        game_data.items[933].name = mud_strdup("Unidentified rare herb");
        free(game_data.items[933].description);
        game_data.items[933].description =
            mud_strdup("A very unusual herb, I need a closer look to not poison myself");
    }
}

void modify_unfinished_potions(void) {
    if (game_data.item_count < 454) {
        /* pre-herblaw config.jag, most likely */
        return;
    }

    free(game_data.items[454].name);
    game_data.items[454].name = mud_strdup("Unfinished guam potion");

    free(game_data.items[455].name);
    game_data.items[455].name = mud_strdup("Unfinished marrentill potion");

    free(game_data.items[456].name);
    game_data.items[456].name = mud_strdup("Unfinished tarromin potion");

    free(game_data.items[457].name);
    game_data.items[457].name = mud_strdup("Unfinished harralander potion");

    free(game_data.items[458].name);
    game_data.items[458].name = mud_strdup("Unfinished ranarr potion");

    free(game_data.items[459].name);
    game_data.items[459].name = mud_strdup("Unfinished irit potion");

    free(game_data.items[460].name);
    game_data.items[460].name = mud_strdup("Unfinished avantoe potion");

    free(game_data.items[461].name);
    game_data.items[461].name = mud_strdup("Unfinished kwuarm potion");

    free(game_data.items[462].name);
    game_data.items[462].name = mud_strdup("Unfinished cadantine potion");

    free(game_data.items[463].name);
    game_data.items[463].name = mud_strdup("Unfinished dwarf weed potion");

    if (game_data.item_count >= 935) {
        free(game_data.items[935].name);
        game_data.items[935].name = mud_strdup("Unfinished torstol potion");
    }

    if (game_data.item_count >= 1052) {
        free(game_data.items[1052].name);
        game_data.items[1052].name = mud_strdup("Unfinished shamanic potion");
    }

    if (game_data.item_count >= 1074) {
        free(game_data.items[1074].name);
        game_data.items[1074].name = mud_strdup("Unfinished jangerberry potion");
    }
}

static void append_dosage(int id) {
    game_data.items[id].name =
        strcat_realloc(game_data.items[id].name, " (3)");
    game_data.items[id + 1].name =
        strcat_realloc(game_data.items[id + 1].name, " (2)");
    game_data.items[id + 2].name =
        strcat_realloc(game_data.items[id + 2].name, " (1)");
}

void modify_potion_dosage(void) {
    /*
     * strength potion from apothecary is the only 4-dose potion
     * i guess he makes it stronger than our player character does
     */
    game_data.items[221].name =
        strcat_realloc(game_data.items[221].name, " (4)");
    append_dosage(222);

    if (game_data.item_count > 474) {
        /* normal potions */
        for (unsigned i = 0; i < 9; ++i) {
            append_dosage(474 + (i * 3));
        }
    }

    if (game_data.item_count > 566) {
        /* antipoisons */
        for (unsigned i = 0; i < 2; ++i) {
            append_dosage(566 + (i * 3));
        }
    }

    /* potion of zamorak */
    if (game_data.item_count > 963) {
        append_dosage(963);
    }
}
