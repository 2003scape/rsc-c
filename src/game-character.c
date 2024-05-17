#include "game-character.h"

/* the order in which to draw character (player and NPC) sprites at different
 * angles */
ANIMATION_INDEX character_animation_array[8][ANIMATION_COUNT] = {
    {ANIMATION_INDEX_CAPE, ANIMATION_INDEX_LEGS, ANIMATION_INDEX_BOOTS,
     ANIMATION_INDEX_LEGS_OVERLAY, ANIMATION_INDEX_BODY,
     ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY, 8, ANIMATION_INDEX_LEFT_HAND,
     ANIMATION_INDEX_RIGHT_HAND},

    {ANIMATION_INDEX_CAPE, ANIMATION_INDEX_LEGS, ANIMATION_INDEX_BOOTS,
     ANIMATION_INDEX_LEGS_OVERLAY, ANIMATION_INDEX_BODY,
     ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY, 8, ANIMATION_INDEX_LEFT_HAND,
     ANIMATION_INDEX_RIGHT_HAND},

    {ANIMATION_INDEX_CAPE, ANIMATION_INDEX_LEFT_HAND, ANIMATION_INDEX_LEGS,
     ANIMATION_INDEX_BOOTS, ANIMATION_INDEX_LEGS_OVERLAY, ANIMATION_INDEX_BODY,
     ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY, 8, ANIMATION_INDEX_RIGHT_HAND},

    {ANIMATION_INDEX_LEFT_HAND, ANIMATION_INDEX_RIGHT_HAND,
     ANIMATION_INDEX_LEGS, ANIMATION_INDEX_BOOTS, ANIMATION_INDEX_LEGS_OVERLAY,
     ANIMATION_INDEX_BODY, ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK,
     8, ANIMATION_INDEX_CAPE, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY},

    {ANIMATION_INDEX_LEFT_HAND, ANIMATION_INDEX_RIGHT_HAND,
     ANIMATION_INDEX_LEGS, ANIMATION_INDEX_BOOTS, ANIMATION_INDEX_LEGS_OVERLAY,
     ANIMATION_INDEX_BODY, ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK,
     8, ANIMATION_INDEX_CAPE, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY},

    {ANIMATION_INDEX_RIGHT_HAND, ANIMATION_INDEX_LEFT_HAND,
     ANIMATION_INDEX_LEGS, ANIMATION_INDEX_BOOTS, ANIMATION_INDEX_LEGS_OVERLAY,
     ANIMATION_INDEX_BODY, ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK,
     8, ANIMATION_INDEX_CAPE, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY},

    {ANIMATION_INDEX_CAPE, ANIMATION_INDEX_RIGHT_HAND, ANIMATION_INDEX_LEGS,
     ANIMATION_INDEX_BOOTS, ANIMATION_INDEX_LEGS_OVERLAY, ANIMATION_INDEX_BODY,
     ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY, 8, ANIMATION_INDEX_LEFT_HAND},

    {ANIMATION_INDEX_CAPE, ANIMATION_INDEX_LEGS, ANIMATION_INDEX_BOOTS,
     ANIMATION_INDEX_LEGS_OVERLAY, ANIMATION_INDEX_BODY,
     ANIMATION_INDEX_BODY_OVERLAY, ANIMATION_INDEX_NECK, ANIMATION_INDEX_HEAD,
     ANIMATION_INDEX_HEAD_OVERLAY, 8, ANIMATION_INDEX_RIGHT_HAND,
     ANIMATION_INDEX_LEFT_HAND}};

int character_walk_model[] = {0, 1, 2, 1};
int character_combat_model_array1[] = {0, 1, 2, 1, 0, 0, 0, 0};
int character_combat_model_array2[] = {0, 0, 0, 0, 0, 1, 2, 1};

int player_hair_colours[] = {0xffc030, 0xffa040, 0x805030, 0x604020, 0x303030,
                             0xff6020, 0xff4000, 0xffffff, 0x00ff00, 0x00ffff};

int player_top_bottom_colours[] = {0xff0000, 0xff8000, 0xffe000, 0xa0e000,
                                   0x00e000, 0x008000, 0x00a080, 0x00b0ff,
                                   0x0080ff, 0x0030f0, 0xe000e0, 0x303030,
                                   0x604000, 0x805000, 0xffffff};

int player_skin_colours[] = {0xecded0, 0xccb366, 0xb38c40, 0x997326, 0x906020};

void game_character_new(GameCharacter *game_character) {
    memset(game_character, 0, sizeof(GameCharacter));
    game_character->npc_id = -1;
    game_character->level = -1;
}

void game_character_move(GameCharacter *character) {
    int next_waypoint = (character->waypoint_current + 1) % 10;

    if (character->moving_step != next_waypoint) {
        int current_animation = -1;
        int step = character->moving_step;

        int delta_step = step < next_waypoint ? next_waypoint - step
                                              : (10 + next_waypoint) - step;

        int j5 = 4;

        if (delta_step > 2) {
            j5 = (delta_step - 1) * 4;
        }

        if (character->waypoints_x[step] - character->current_x >
                MAGIC_LOC * 3 ||
            character->waypoints_y[step] - character->current_y >
                MAGIC_LOC * 3 ||
            character->waypoints_x[step] - character->current_x <
                -MAGIC_LOC * 3 ||
            character->waypoints_y[step] - character->current_y <
                -MAGIC_LOC * 3 ||
            delta_step > 8) {
            character->current_x = character->waypoints_x[step];
            character->current_y = character->waypoints_y[step];
        } else {
            if (character->current_x < character->waypoints_x[step]) {
                character->current_x += j5;
                character->step_count++;
                current_animation = 2;
            } else if (character->current_x > character->waypoints_x[step]) {
                character->current_x -= j5;
                character->step_count++;
                current_animation = 6;
            }

            if (character->current_x - character->waypoints_x[step] < j5 &&
                character->current_x - character->waypoints_x[step] > -j5) {
                character->current_x = character->waypoints_x[step];
            }

            if (character->current_y < character->waypoints_y[step]) {
                character->current_y += j5;
                character->step_count++;

                if (current_animation == -1) {
                    current_animation = 4;
                } else if (current_animation == 2) {
                    current_animation = 3;
                } else {
                    current_animation = 5;
                }
            } else if (character->current_y > character->waypoints_y[step]) {
                character->current_y -= j5;
                character->step_count++;

                if (current_animation == -1) {
                    current_animation = 0;
                } else if (current_animation == 2) {
                    current_animation = 1;
                } else {
                    current_animation = 7;
                }
            }

            if (character->current_y - character->waypoints_y[step] < j5 &&
                character->current_y - character->waypoints_y[step] > -j5) {
                character->current_y = character->waypoints_y[step];
            }
        }

        if (current_animation != -1) {
            character->current_animation = current_animation;
        }

        if (character->current_x == character->waypoints_x[step] &&
            character->current_y == character->waypoints_y[step]) {
            character->moving_step = (step + 1) % 10;
        }
    } else {
        character->current_animation = character->next_animation;

        if (character->npc_id == GIANT_BAT_ID) {
            character->step_count++;
        }
    }

    if (character->message_timeout > 0) {
        character->message_timeout--;
    }

    if (character->bubble_timeout > 0) {
        character->bubble_timeout--;
    }

    if (character->combat_timer > 0) {
        character->combat_timer--;
    }
}
