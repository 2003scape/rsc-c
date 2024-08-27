#include "mudclient.h"

#ifdef SDL2
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code) {
    *char_code = -1;

    switch (keysym->scancode) {
    case SDL_SCANCODE_LEFT:
        *code = K_LEFT;
        break;
    case SDL_SCANCODE_RIGHT:
        *code = K_RIGHT;
        break;
    case SDL_SCANCODE_UP:
        *code = K_UP;
        break;
    case SDL_SCANCODE_DOWN:
        *code = K_DOWN;
        break;
    case SDL_SCANCODE_PAGEUP:
        *code = K_PAGE_UP;
        break;
    case SDL_SCANCODE_PAGEDOWN:
        *code = K_PAGE_DOWN;
        break;
    case SDL_SCANCODE_HOME:
        *code = K_HOME;
        break;
    case SDL_SCANCODE_F1:
        *code = K_F1;
        break;
    case SDL_SCANCODE_ESCAPE:
        *code = K_ESCAPE;
        break;
    /*case SDL_SCANCODE_RETURN:
        *code = K_ENTER;
        break;*/
    // TODO: Swallow "bad inputs" by default? ie. numlock, capslock
    case SDL_SCANCODE_NUMLOCKCLEAR:
        *code = -1;
        *char_code = 1;
        break;
    case SDL_SCANCODE_CAPSLOCK:
        *code = -1;
        *char_code = 1;
        break;
    case SDL_SCANCODE_KP_DIVIDE:
        *code = K_FWD_SLASH;
        *char_code = K_FWD_SLASH;
        break;
    case SDL_SCANCODE_KP_MULTIPLY:
        *code = K_ASTERISK;
        *char_code = K_ASTERISK;
        break;
    case SDL_SCANCODE_KP_MINUS:
        *code = K_MINUS;
        *char_code = K_MINUS;
        break;
    case SDL_SCANCODE_KP_PLUS:
        *code = K_PLUS;
        *char_code = K_PLUS;
        break;
    case SDL_SCANCODE_KP_PERIOD:
        *code = K_PERIOD;
        *char_code = K_PERIOD;
        break;
    case SDL_SCANCODE_KP_ENTER:
        *code = K_ENTER;
        *char_code = K_ENTER;
        break;
    case SDL_SCANCODE_KP_0:
        *code = K_0;
        *char_code = K_0;
        break;
    case SDL_SCANCODE_KP_1:
        *code = K_1;
        *char_code = K_1;
        break;
    case SDL_SCANCODE_KP_2:
        *code = K_2;
        *char_code = K_2;
        break;
    case SDL_SCANCODE_KP_3:
        *code = K_3;
        *char_code = K_3;
        break;
    case SDL_SCANCODE_KP_4:
        *code = K_4;
        *char_code = K_4;
        break;
    case SDL_SCANCODE_KP_5:
        *code = K_5;
        *char_code = K_5;
        break;
    case SDL_SCANCODE_KP_6:
        *code = K_6;
        *char_code = K_6;
        break;
    case SDL_SCANCODE_KP_7:
        *code = K_7;
        *char_code = K_7;
        break;
    case SDL_SCANCODE_KP_8:
        *code = K_8;
        *char_code = K_8;
        break;
    case SDL_SCANCODE_KP_9:
        *code = K_9;
        *char_code = K_9;
        break;
    default:
        *char_code = keysym->sym;

        switch (keysym->scancode) {
        case SDL_SCANCODE_TAB:
            *code = K_TAB;
            break;
        case SDL_SCANCODE_1:
            *code = K_1;
            break;
        case SDL_SCANCODE_2:
            *code = K_2;
            break;
        case SDL_SCANCODE_3:
            *code = K_3;
            break;
        case SDL_SCANCODE_4:
            *code = K_4;
            break;
        case SDL_SCANCODE_5:
            *code = K_5;
            break;
        default:
            *code = *char_code;
            break;
        }

        if (keysym->mod & KMOD_SHIFT) {
            if (*char_code >= 'a' && *char_code <= 'z') {
                *char_code -= 32;
            } else {
                switch (*char_code) {
                case ';':
                    *char_code = ':';
                    break;
                case '`':
                    *char_code = '~';
                    break;
                case '1':
                    *char_code = '!';
                    break;
                case '2':
                    *char_code = '@';
                    break;
                case '3':
                    *char_code = '#';
                    break;
                case '4':
                    *char_code = '$';
                    break;
                case '5':
                    *char_code = '%';
                    break;
                case '6':
                    *char_code = '^';
                    break;
                case '7':
                    *char_code = '&';
                    break;
                case '8':
                    *char_code = '*';
                    break;
                case '9':
                    *char_code = '(';
                    break;
                case '0':
                    *char_code = ')';
                    break;
                case '-':
                    *char_code = '_';
                    break;
                case '=':
                    *char_code = '+';
                    break;
                case '[':
                    *char_code = '{';
                    break;
                case ']':
                    *char_code = '}';
                    break;
                case '\\':
                    *char_code = '|';
                    break;
                case ',':
                    *char_code = '<';
                    break;
                case '.':
                    *char_code = '>';
                    break;
                case '/':
                    *char_code = '?';
                    break;
                }
            }
        }

        break;
    }
}
#endif
