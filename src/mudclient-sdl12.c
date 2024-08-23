#include "mudclient.h"

#ifdef SDL12
void get_sdl_keycodes(SDL_keysym *keysym, char *char_code, int *code) {
    *code = -1;
    *char_code = -1;

    /* note: unicode is not set for key released */
    if (keysym->unicode > 0 && keysym->unicode < 128) {
        if (isprint((unsigned char)keysym->unicode)) {
            *code = keysym->unicode;
            *char_code = keysym->unicode;
            return;
        }
    }

    switch (keysym->sym) {
    case SDLK_TAB:
        *code = K_TAB;
        *char_code = '\t';
        break;
    case SDLK_BACKSPACE:
        *code = K_BACKSPACE;
        *char_code = '\b';
        break;
    case SDLK_LEFT:
        *code = K_LEFT;
        break;
    case SDLK_RIGHT:
        *code = K_RIGHT;
        break;
    case SDLK_UP:
        *code = K_UP;
        break;
    case SDLK_DOWN:
        *code = K_DOWN;
        break;
    case SDLK_PAGEUP:
        *code = K_PAGE_UP;
        break;
    case SDLK_PAGEDOWN:
        *code = K_PAGE_DOWN;
        break;
    case SDLK_HOME:
        *code = K_HOME;
        break;
    case SDLK_F1:
        *code = K_F1;
        break;
    case SDLK_ESCAPE:
        *code = K_ESCAPE;
        break;
    case SDLK_RETURN:
        *code = K_ENTER;
        *char_code = '\r';
        break;
    case SDLK_KP1:
    case SDLK_1:
        *code = K_1;
        *char_code = K_1;
        break;
    case SDLK_KP2:
    case SDLK_2:
        *code = K_2;
        *char_code = K_2;
        break;
    case SDLK_KP3:
    case SDLK_3:
        *code = K_3;
        *char_code = K_3;
        break;
    case SDLK_KP4:
    case SDLK_4:
        *code = K_4;
        *char_code = K_4;
        break;
    case SDLK_KP5:
    case SDLK_5:
        *code = K_5;
        *char_code = K_5;
        break;
    }
}
#endif
