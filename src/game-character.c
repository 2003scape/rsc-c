#include "game-character.h"

void game_character_new(GameCharacter* game_character) {
    memset(game_character, 0, sizeof(GameCharacter));
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
