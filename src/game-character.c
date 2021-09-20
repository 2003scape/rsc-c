#include "game-character.h"

void game_character_new(GameCharacter* game_character) {
    memset(game_character, 0, sizeof(GameCharacter));
    game_character->level = -1;
}
