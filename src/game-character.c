#include "game-character.h"

void game_character_new(GameCharacter* game_character) {
    memset(game_character, 0, sizeof(game_character));
    game_character->level = -1;
}
