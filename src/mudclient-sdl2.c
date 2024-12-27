#include "mudclient.h"

#ifdef SDL2
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code) {
    *code = -1;
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
    case SDL_SCANCODE_KP_ENTER:
    case SDL_SCANCODE_RETURN:
        *code = K_ENTER;
        *char_code = K_ENTER;
        break;
    // TODO: Swallow "bad inputs" by default? ie. numlock, capslock
    case SDL_SCANCODE_NUMLOCKCLEAR:
        *code = -1;
        *char_code = 1;
        break;
    case SDL_SCANCODE_CAPSLOCK:
        *code = -1;
        *char_code = 1;
        break;
    case SDL_SCANCODE_TAB:
        *code = K_TAB;
        *char_code = K_TAB;
        break;
    case SDL_SCANCODE_BACKSPACE:
        *code = K_BACKSPACE;
        *char_code = K_BACKSPACE;
        break;
    default:
        break;
    }
}

void mudclient_start_application(mudclient *mud, char *title) {
#ifdef __SWITCH__
    Result romfs_res = romfsInit();

    if (romfs_res) {
        mud_error("romfsInit: %08lX\n", romfs_res);
        exit(1);
    }
#endif

    int init = SDL_INIT_VIDEO;

    if (mud->options->members && !mud->options->lowmem) {
        init |= SDL_INIT_AUDIO;
    }

#ifdef __SWITCH__
    init |= SDL_INIT_JOYSTICK;
#endif

    if (SDL_Init(init) < 0) {
        mud_error("SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

#ifdef SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

#ifdef __SWITCH__
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);
#endif

/* XXX: currently require non-callback-based audio from SDL >= 2.0.4 */
#ifdef SDL_VERSION_ATLEAST
#if SDL_VERSION_ATLEAST(2, 0, 4)
    if (mud->options->members && !mud->options->lowmem) {
        SDL_AudioSpec wanted_audio;

        wanted_audio.freq = SAMPLE_RATE;
        wanted_audio.format = AUDIO_S16;
        wanted_audio.channels = 1;
        wanted_audio.silence = 0;
        wanted_audio.samples = 1024;
        wanted_audio.callback = NULL;

        if (SDL_OpenAudio(&wanted_audio, NULL) < 0) {
            mud_error("SDL_OpenAudio(): %s\n", SDL_GetError());
        }
    }
#endif
#endif

    uint32_t windowflags = SDL_WINDOW_SHOWN;

#if !defined(WII) && !defined(_3DS) && !defined(EMSCRIPTEN)
    windowflags |= SDL_WINDOW_RESIZABLE;
#endif

#ifdef RENDER_GL
    windowflags |= SDL_WINDOW_OPENGL;

#ifdef EMSCRIPTEN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#elif defined(OPENGL15)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
#elif defined(OPENGL20)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif /* EMSCRIPTEN */
#endif /* RENDER_GL */

    mud->window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         mud->game_width, mud->game_height, windowflags);

    SDL_SetWindowMinimumSize(mud->window, MUD_MIN_WIDTH, MUD_MIN_HEIGHT);

    mud->default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    mud->hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

#ifdef RENDER_GL
    mud->gl_window = mud->window;
    mud->window = NULL;

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        mud_error("unable to initialize sdl_image: %s\n", IMG_GetError());
    }

    SDL_GLContext *context = SDL_GL_CreateContext(mud->gl_window);

    if (!context) {
        mud_error("SDL_GL_CreateContext(): %s\n", SDL_GetError());
        exit(1);
    }

    if (SDL_GL_MakeCurrent(mud->gl_window, context) != 0) {
        mud_error("SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
        exit(1);
    }
#endif
}
#endif
