#include "item-highlight.h"

#define ITEM_NATURE_RUNE    (40)
#define HIGHLIGHT_NATURE    (0x00DE00)

#define ITEM_CHAOS_RUNE     (41)
#define HIGHLIGHT_CHAOS     (0xDF4B4B)

#define ITEM_LAW_RUNE       (42)
#define HIGHLIGHT_LAW       (0x0BDBED)

#define ITEM_DEATH_RUNE     (38)
#define HIGHLIGHT_DEATH     (0xFFFFFF)

#define ITEM_COSMIC_RUNE    (46)
#define HIGHLIGHT_COSMIC    (0xFFFF00)

#define ITEM_BLOOD_RUNE     (619)
#define HIGHLIGHT_BLOOD     (0x8E1400)

#define ITEM_PUMPKIN        (422)
#define HIGHLIGHT_PUMPKIN   (0xFD8D28)

#define ITEM_HALLOWEEN_MASK_GREEN       (828)
#define HIGHLIGHT_HALLOWEEN_MASK_GREEN  (0x00CE00)
#define ITEM_HALLOWEEN_MASK_RED         (831)
#define HIGHLIGHT_HALLOWEEN_MASK_RED    (0xCE0000)
#define ITEM_HALLOWEEN_MASK_BLUE        (832)
#define HIGHLIGHT_HALLOWEEN_MASK_BLUE   (0x0000CE)

#define ITEM_CHRISTMAS_CRACKER      (575)
#define HIGHLIGHT_CHRISTMAS_CRACKER (0xF6F68C)
#define ITEM_PARTY_HAT_RED          (576)
#define HIGHLIGHT_PARTY_HAT_RED     (0xFF0000)
#define ITEM_PARTY_HAT_YELLOW       (577)
#define HIGHLIGHT_PARTY_HAT_YELLOW  (0xFFFF00)
#define ITEM_PARTY_HAT_BLUE         (578)
#define HIGHLIGHT_PARTY_HAT_BLUE    (0x0000FF)
#define ITEM_PARTY_HAT_GREEN        (579)
#define HIGHLIGHT_PARTY_HAT_GREEN   (0x00FF00)
#define ITEM_PARTY_HAT_PINK         (580)
#define HIGHLIGHT_PARTY_HAT_PINK    (0xFF00FF)
#define ITEM_PARTY_HAT_WHITE        (581)
#define HIGHLIGHT_PARTY_HAT_WHITE   (0xFFFFFF)

#define ITEM_SANTAS_HAT             (971)
#define HIGHLIGHT_SANTAS_HAT        (0xD81010)

#define HIGHLIGHT_HERB              (0x31B131)

#define HIGHLIGHT_SAPPHIRE          (0x004CFF)
#define HIGHLIGHT_EMERALD           (0x33CC33)
#define HIGHLIGHT_RUBY              (0xFF3300)
#define HIGHLIGHT_DIAMOND           (0xFFFFFF)
#define HIGHLIGHT_DRAGONSTONE       (0xBB00FF)

#define HIGHLIGHT_MITHRIL           (0x99B2CC)
#define HIGHLIGHT_ADAMANTITE        (0xB2CC99)
#define HIGHLIGHT_RUNITE            (0x00FFFF)
#define HIGHLIGHT_DRAGON            (0xFF0044)

const uint16_t rare_herbs[] = {
    438, 439, 440, 441, 442, 443
};

const uint16_t dragon_items[] = {
    593, 594, 795, 1276, 1277, 1278
};

const uint16_t sapphire_items[] = {
    160, 164, 284, 289, 297, 302, 314
};

const uint16_t emerald_items[] = {
    159, 163, 285, 290, 298, 303, 315
};

const uint16_t ruby_items[] = {
    158, 162, 286, 291, 299, 304, 316
};

const uint16_t diamond_items[] = {
    157, 161, 287, 292, 300, 305, 317
};

/* also includes crystal key and such */
const uint16_t dragonstone_items[] = {
    522, 523, 524, 525, 526, 527, 542, 543, 544, 597, 610
};

const uint16_t mithril_items[] = {
    64, 68, 73, 79, 85, 91, 96, 106, 110, 115, 122, 126, 130, 153, 173,
    203, 226, 310, 562, 1090, 1260
};
const uint16_t adamantite_items[] = {
    65, 69, 74, 80, 86, 92, 97, 107, 111, 116, 120, 123, 127, 131, 154,
    174, 204, 227, 311, 564, 645, 673, 1069, 1091, 1139, 1261
};

const uint16_t runite_items[] = {
    75, 81, 93, 98, 112, 396, 397, 398, 399, 400, 401, 402, 403, 404,
    405, 406, 407, 408, 409, 647, 674, 1070, 1092, 1134, 1140, 1262
};

uint32_t highlight_item(uint16_t id) {
    unsigned i;

    switch (id) {
    case ITEM_NATURE_RUNE:
        return HIGHLIGHT_NATURE;

    case ITEM_CHAOS_RUNE:
        return HIGHLIGHT_CHAOS;

    case ITEM_LAW_RUNE:
        return HIGHLIGHT_LAW;

    case ITEM_DEATH_RUNE:
        return HIGHLIGHT_DEATH;

    case ITEM_COSMIC_RUNE:
        return HIGHLIGHT_COSMIC;

    case ITEM_BLOOD_RUNE:
        return HIGHLIGHT_BLOOD;

    case ITEM_PUMPKIN:
        return HIGHLIGHT_PUMPKIN;

    case ITEM_HALLOWEEN_MASK_GREEN:
        return HIGHLIGHT_HALLOWEEN_MASK_GREEN;
    case ITEM_HALLOWEEN_MASK_RED:
        return HIGHLIGHT_HALLOWEEN_MASK_RED;
    case ITEM_HALLOWEEN_MASK_BLUE:
        return HIGHLIGHT_HALLOWEEN_MASK_BLUE;

    case ITEM_CHRISTMAS_CRACKER:
        return HIGHLIGHT_CHRISTMAS_CRACKER;
    case ITEM_PARTY_HAT_RED:
        return HIGHLIGHT_PARTY_HAT_RED;
    case ITEM_PARTY_HAT_YELLOW:
        return HIGHLIGHT_PARTY_HAT_YELLOW;
    case ITEM_PARTY_HAT_BLUE:
        return HIGHLIGHT_PARTY_HAT_BLUE;
    case ITEM_PARTY_HAT_GREEN:
        return HIGHLIGHT_PARTY_HAT_GREEN;
    case ITEM_PARTY_HAT_PINK:
        return HIGHLIGHT_PARTY_HAT_PINK;
    case ITEM_PARTY_HAT_WHITE:
        return HIGHLIGHT_PARTY_HAT_WHITE;

    case ITEM_SANTAS_HAT:
        return HIGHLIGHT_SANTAS_HAT;
    }
    for (i = 0; i < (sizeof(rare_herbs) / sizeof(uint16_t)); ++i) {
        if (rare_herbs[i] == id) {
            return HIGHLIGHT_HERB;
        }
    }
    for (i = 0; i < (sizeof(sapphire_items) / sizeof(uint16_t)); ++i) {
        if (sapphire_items[i] == id) {
            return HIGHLIGHT_SAPPHIRE;
        }
    }
    for (i = 0; i < (sizeof(emerald_items) / sizeof(uint16_t)); ++i) {
        if (emerald_items[i] == id) {
            return HIGHLIGHT_EMERALD;
        }
    }
    for (i = 0; i < (sizeof(ruby_items) / sizeof(uint16_t)); ++i) {
        if (ruby_items[i] == id) {
            return HIGHLIGHT_RUBY;
        }
    }
    for (i = 0; i < (sizeof(diamond_items) / sizeof(uint16_t)); ++i) {
        if (diamond_items[i] == id) {
            return HIGHLIGHT_DIAMOND;
        }
    }
    for (i = 0; i < (sizeof(dragonstone_items) / sizeof(uint16_t)); ++i) {
        if (dragonstone_items[i] == id) {
            return HIGHLIGHT_DRAGONSTONE;
        }
    }
    for (i = 0; i < (sizeof(mithril_items) / sizeof(uint16_t)); ++i) {
        if (mithril_items[i] == id) {
            return HIGHLIGHT_MITHRIL;
        }
    }
    for (i = 0; i < (sizeof(adamantite_items) / sizeof(uint16_t)); ++i) {
        if (adamantite_items[i] == id) {
            return HIGHLIGHT_ADAMANTITE;
        }
    }
    for (i = 0; i < (sizeof(runite_items) / sizeof(uint16_t)); ++i) {
        if (runite_items[i] == id) {
            return HIGHLIGHT_RUNITE;
        }
    }
    for (i = 0; i < (sizeof(dragon_items) / sizeof(uint16_t)); ++i) {
        if (dragon_items[i] == id) {
            return HIGHLIGHT_DRAGON;
        }
    }
    return 0;
}
