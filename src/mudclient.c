#include "mudclient.h"

#ifdef EMSCRIPTEN
/* clang doesn't know what triple equals is, understandably */
/* clang-format off */
EM_JS(int, can_resize, (), {
    return window._mudclientCanResize &&
               document.activeElement !== window._mudclientKeyboard &&
               document.activeElement !== window._mudclientPassword;
});

EM_JS(int, get_window_width, (), { return window.innerWidth; });
EM_JS(int, get_window_height, (), { return window.innerHeight; });

EM_JS(void, browser_trigger_keyboard,
      (char *text, int is_password, int x, int y, int width, int height,
       int font, int is_centred), {
          const keyboard = is_password ? window._mudclientPassword :
                                         window._mudclientKeyboard;

          keyboard.value = UTF8ToString(text);

          if (is_centred) {
              keyboard.style.height = `${height}px`;
              keyboard.style.textAlign = 'center';
          } else {
              keyboard.style.height = null;
              keyboard.style.textAlign = 'left';
          }

          keyboard.style.left = `${x}px`;
          keyboard.style.top = `${y}px`;

          keyboard.style.width = `${width}px`;

          const fonts = {
              1: 'mudclient-font-bold-12',
              4: 'mudclient-font-bold-14',
              5: 'mudclient-font-bold-16'
          };

          keyboard.classList.remove(...Object.values(fonts));

          const fontClass = fonts[font];

          if (fontClass) {
              keyboard.classList.add(fontClass);
          }

          keyboard.style.display = 'block';

          keyboard.focus();
      });

EM_JS(int, browser_is_touch, (), { return window._mudclientIsTouch; });
/* clang-format on */

int last_canvas_check = 0;

mudclient *global_mud = NULL;
#endif

int mudclient_finger_1_x = 0;
int mudclient_finger_1_y = 0;
int mudclient_finger_1_down = 0;

int mudclient_finger_2_x = 0;
int mudclient_finger_2_y = 0;
int mudclient_finger_2_down = 0;

int mudclient_full_width = 0;
int mudclient_full_height = 0;

const char *font_files[] = {"h11p.jf", "h12b.jf", "h12p.jf", "h13b.jf",
                            "h14b.jf", "h16b.jf", "h20b.jf", "h24b.jf"};

/* only the first of models with animations are stored in the cache */
const char *animated_models[] = {
    "torcha2",      "torcha3",    "torcha4",    "skulltorcha2", "skulltorcha3",
    "skulltorcha4", "firea2",     "firea3",     "fireplacea2",  "fireplacea3",
    "firespell2",   "firespell3", "lightning2", "lightning3",   "clawspell2",
    "clawspell3",   "clawspell4", "clawspell5", "spellcharge2", "spellcharge3"};

#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
/*
 * animations that experienced a loss of fine detail in January 2002 with the
 * "Compression" update
 *
 * camel - eyes lose distinctiveness.
 * bat - most noticable. mouth is nearly gone entirely.
 * bear - loses some shading that gives it more of a "fur" texture.
 * human heads - eyes lose detail.
 * human tops - belt buckles lose detail or become flesh (ew).
 */
static const char *anims_older_is_better[] = {
    "camel",  "bat",           "battleaxe", "bear",  "fbody1",
    "fhead1", "fplatemailtop", "head1",     "head2", "head3",
    "head4",  "platemailtop",  "staff",     "body1", NULL};
#endif

char login_screen_status[255] = {0};

#ifdef __SWITCH__
SDL_Joystick *joystick;
#define MAX_KBD_STR_SIZE 200
SwkbdConfig switch_keyboard;
char switch_keyboard_buffer[MAX_KBD_STR_SIZE] = {0};
uint8_t switch_mouse_button = 1;
#endif

#ifdef WII
char keyboard_buttons[5][10] = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';'},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'},
    {' ', ' ', ' ', ' ', ' ', ' ', '-', '=', '\'', NULL}};

char keyboard_shift_buttons[5][10] = {
    {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'},
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':'},
    {'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?'},
    {' ', ' ', ' ', ' ', ' ', ' ', '_', '+', '"', NULL}};

int keyboard_offsets[] = {0, 11, 16, 21, 46};

void draw_background(uint8_t *framebuffer, int full) {
    if (full) {
        memcpy(framebuffer, rsc_game_yuv,
               rsc_game_yuv_width * rsc_game_yuv_height * 2);

        return;
    }

    for (int y = 0; y < GAME_OFFSET_Y; y++) {
        int fb_index = (640 * 2 * y);
        int yuv_index = (y * rsc_game_yuv_width * 2);

        memcpy(framebuffer + fb_index, rsc_game_yuv + yuv_index,
               rsc_game_yuv_width * 2);
    }

    for (int y = GAME_OFFSET_Y; y < GAME_OFFSET_Y + MUD_HEIGHT; y++) {
        int fb_index = (640 * 2 * y);
        int yuv_index = (y * rsc_game_yuv_width * 2);

        memcpy(framebuffer + fb_index, rsc_game_yuv + yuv_index,
               GAME_OFFSET_X * 2);

        memcpy(framebuffer + fb_index + ((MUD_WIDTH + GAME_OFFSET_X) * 2),
               rsc_game_yuv + yuv_index + ((MUD_WIDTH + GAME_OFFSET_X) * 2),
               GAME_OFFSET_X * 2);
    }

    for (int y = GAME_OFFSET_Y + MUD_HEIGHT; y < 480; y++) {
        int fb_index = (640 * 2 * y);
        int yuv_index = (y * rsc_game_yuv_width * 2);

        memcpy(framebuffer + fb_index, rsc_game_yuv + yuv_index,
               rsc_game_yuv_width * 2);
    }
}

void draw_arrow(uint8_t *framebuffer, int mouse_x, int mouse_y) {
    if (mouse_x >= 640 || mouse_y >= 460) {
        return;
    }

    for (int y = 0; y < arrow_yuv_height; y++) {
        int fb_index = (640 * 2 * (y + mouse_y)) + (mouse_x * 2);

        int arrow_offset =
            (arrow_yuv_offsets[y] * 2) + (arrow_yuv_width * 2 * y);

        int arrow_width = arrow_yuv_lines[y] * 2;

        memcpy(framebuffer + fb_index + (arrow_yuv_offsets[y] * 2),
               arrow_yuv + arrow_offset, arrow_width);
    }
}

void draw_keyboard(uint8_t *framebuffer, int is_shift) {
    for (int y = 0; y < rsc_keyboard_yuv_height; y++) {
        int fb_index = (640 * 2 * (y + 200)) + (52 * 2);
        int keyboard_index = (y * rsc_keyboard_yuv_width * 2);

        for (int x = 0; x < rsc_keyboard_yuv_width * 2; x++) {
            if (is_shift) {
                framebuffer[fb_index + x] =
                    (framebuffer[fb_index + x] +
                     rsc_keyboard_shift_yuv[keyboard_index + x]) /
                    2;
            } else {
                framebuffer[fb_index + x] =
                    (framebuffer[fb_index + x] +
                     rsc_keyboard_yuv[keyboard_index + x]) /
                    2;
            }
        }
    }
}

int wii_mouse_x = 0;
int wii_mouse_y = 0;
int wii_mouse_button = 0;

int update_wii_mouse(WPADData *wiimote_data) {
    mouse_event me = {0};

    int updated = 0;
    int res = 0;

    wii_mouse_button = 0;

    while (res = MOUSE_GetEvent(&me)) {
        updated = 1;

        if (me.button) {
            wii_mouse_button = me.button;

            if (wii_mouse_button == 2) {
                wii_mouse_button = 3;
            }
        } else {
            wii_mouse_button = 0;
        }

        wii_mouse_x += me.rx;
        wii_mouse_y += me.ry;

        if (wii_mouse_x >= 640) {
            wii_mouse_x = 640;
        } else if (wii_mouse_x <= 0) {
            wii_mouse_x = 0;
        }

        if (wii_mouse_y >= 480) {
            wii_mouse_y = 480;
        } else if (wii_mouse_y <= 0) {
            wii_mouse_y = 0;
        }
    }

    if (wiimote_data != NULL) {
        if (wiimote_data->btns_h & WPAD_BUTTON_HOME) {
            exit(0);
        }

        if (!wiimote_data->ir.valid) {
            return updated;
        }

        if (wiimote_data->btns_h & WPAD_BUTTON_A) {
            wii_mouse_button = 1;
        } else if (wiimote_data->btns_h & WPAD_BUTTON_B) {
            wii_mouse_button = 3;
        }

        wii_mouse_x = (int)wiimote_data->ir.x;
        wii_mouse_y = (int)wiimote_data->ir.y;
    }

    return updated;
}

#endif

void mudclient_new(mudclient *mud) {
    memset(mud, 0, sizeof(mudclient));

    mud->target_fps = 20;
    mud->loading_step = 1;
    mud->loading_progess_text = "Loading";
    mud->game_width = MUD_WIDTH;
    mud->game_height = MUD_HEIGHT;
    mud->camera_angle = 1;
    mud->camera_rotation = 128;
    mud->camera_rotation_x_increment = 2;
    mud->camera_rotation_y_increment = 2;
    mud->last_plane_index = -1;

    mud->menu_items_size = 32;
    mud->menu_items = calloc(mud->menu_items_size, sizeof(struct MenuEntry));
    mud->menu_indices = calloc(mud->menu_items_size, sizeof(int));

    mud->options = malloc(sizeof(Options));

    options_new(mud->options);
    options_load(mud->options);

    mud->camera_zoom = mud->options->zoom_camera ? ZOOM_OUTDOORS : ZOOM_INDOORS;

    for (int i = 0; i < MESSAGE_HISTORY_LENGTH; i++) {
        memset(mud->message_history[i], '\0', 255);
    }

    mud->selected_spell = -1;
    mud->selected_item_name = "";
    mud->selected_item_inventory_index = -1;
    mud->quest_complete = calloc(quests_length, sizeof(int8_t));

#ifdef _3DS
    mud->_3ds_sound_position = -1;
#endif

    mud->appearance_body_type = 1;
    mud->appearance_hair_colour = 2;
    mud->appearance_top_colour = 8;
    mud->appearance_bottom_colour = 14;
    mud->appearance_head_gender = 1;

    mud->sleep_word_delay = 1;

    /* set by the server to 192 on p2p servers */
    mud->bank_items_max = 48;

    mud->bank_selected_item_slot = -1;
    mud->bank_selected_item = -2;

    mud->sprite_media = 2000;
    mud->sprite_util = mud->sprite_media + 100;
    mud->sprite_item = mud->sprite_util + 50;
    mud->sprite_logo = mud->sprite_item + 1000;
    mud->sprite_projectile = mud->sprite_logo + 10;
    // TODO this is also used for sleep word
    mud->sprite_texture = mud->sprite_projectile + 50;
    mud->sprite_texture_world = mud->sprite_texture + 10;
}

void mudclient_resize(mudclient *mud) {
#if !defined(WII) && !defined(_3DS)
    SDL_FreeSurface(mud->screen);
    SDL_FreeSurface(mud->pixel_surface);

#ifdef SDL12
    mud->screen = SDL_GetVideoSurface();
#else
    mud->screen = SDL_GetWindowSurface(mud->window);
#endif

    mud->pixel_surface =
        SDL_CreateRGBSurface(0, mud->game_width, mud->game_height, 32, 0xff0000,
                             0x00ff00, 0x0000ff, 0);

    if (mud->surface != NULL) {
#ifdef RENDER_SW
        mud->surface->pixels = mud->pixel_surface->pixels;
#endif

#ifdef RENDER_GL
        free(mud->surface->pixels);

        mud->surface->pixels =
            calloc(mud->game_width * mud->game_height, sizeof(int32_t));
#endif

        panel_destroy(mud->panel_login_welcome);
        free(mud->panel_login_welcome);

        panel_destroy(mud->panel_login_new_user);
        free(mud->panel_login_new_user);

        panel_destroy(mud->panel_login_existing_user);
        free(mud->panel_login_existing_user);

        mudclient_create_login_panels(mud);

        panel_destroy(mud->panel_appearance);
        free(mud->panel_appearance);

        mudclient_create_appearance_panel(mud);

        mud->scene->raster = mud->surface->pixels;

        int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                         mud->surface->height < MUD_VANILLA_HEIGHT;

        int is_touch = mudclient_is_touch(mud);

        int full_offset_x = mud->surface->width - MUD_WIDTH;
        int full_offset_y = mud->surface->height - MUD_HEIGHT;
        int half_offset_x = (mud->surface->width / 2) - (MUD_WIDTH / 2);
        int half_offset_y = (mud->surface->height / 2) - (MUD_HEIGHT / 2);

        int dynamic_offset_x =
            (mud->surface->width / 2) -
            (is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH) / 2;

        int dynamic_offset_y =
            (mud->surface->height / 2) -
            (is_compact ? MUD_MIN_HEIGHT : MUD_VANILLA_HEIGHT) / 2;

        if (mud->panel_login_welcome != NULL) {
            mud->panel_login_welcome->offset_x = dynamic_offset_x;
            mud->panel_login_welcome->offset_y = dynamic_offset_y;
        }

        if (mud->panel_login_new_user != NULL) {
            mud->panel_login_new_user->offset_x = dynamic_offset_x;
            mud->panel_login_new_user->offset_y = dynamic_offset_y;
        }

        if (mud->panel_login_existing_user != NULL) {
            mud->panel_login_existing_user->offset_x = dynamic_offset_x;
            mud->panel_login_existing_user->offset_y = dynamic_offset_y;
        }

        if (mud->panel_appearance != NULL) {
            mud->panel_appearance->offset_x = dynamic_offset_x;
            mud->panel_appearance->offset_y = dynamic_offset_y;
        }

        if (mud->panel_message_tabs != NULL && !is_touch) {
            mud->panel_message_tabs->offset_y = full_offset_y;
        }

        if (mud->panel_quests != NULL) {
            mud->panel_quests->offset_x = full_offset_x;

            if (is_touch) {
                mud->panel_quests->offset_y = full_offset_y;
            }
        }

        if (mud->panel_magic != NULL) {
            mud->panel_magic->offset_x = full_offset_x;

            if (is_touch) {
                mud->panel_magic->offset_y = full_offset_y;
            }
        }

        if (mud->panel_social_list != NULL) {
            mud->panel_social_list->offset_x = full_offset_x;

            if (is_touch) {
                mud->panel_social_list->offset_y = full_offset_y;
            }
        }

        if (mud->panel_game_options != NULL) {
            mud->panel_game_options->offset_x = half_offset_x;
            mud->panel_game_options->offset_y = half_offset_y;
        }

        if (mud->panel_control_options != NULL) {
            mud->panel_control_options->offset_x = half_offset_x;
            mud->panel_control_options->offset_y = half_offset_y;
        }

        if (mud->panel_ui_options != NULL) {
            mud->panel_ui_options->offset_x = half_offset_x;
            mud->panel_ui_options->offset_y = half_offset_y;
        }

        if (mud->panel_bank_options != NULL) {
            mud->panel_bank_options->offset_x = half_offset_x;
            mud->panel_bank_options->offset_y = half_offset_y;
        }
    }

#ifdef RENDER_GL
    glViewport(0, 0, mud->game_width, mud->game_height);
#endif
#endif
}

void mudclient_start_application(mudclient *mud, char *title) {
#ifdef WII
    VIDEO_Init();
    WPAD_Init();
    AUDIO_Init(NULL);
    ASND_Init();
    ASND_Pause(0); // TODO move

    GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
    mud->framebuffers = malloc(2 * sizeof(uint8_t *));
    mud->framebuffers[0] = SYS_AllocateFramebuffer(rmode);
    mud->framebuffers[1] = SYS_AllocateFramebuffer(rmode);

    mud->framebuffer = mud->framebuffers[0];
    // MEM_K0_TO_K1

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(mud->framebuffer);
    VIDEO_SetBlack(0);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    if (rmode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }

    WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(0, rmode->fbWidth, rmode->xfbHeight);

    KEYBOARD_Init(NULL);
    MOUSE_Init();

    mud->last_keyboard_button = -1;

    // console_init(mud->framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
#elif defined(_3DS)
    atexit(soc_shutdown);

    // gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, 0);
    gfxInitDefault();

    /* uncomment and disable draw_top_background to see stdout */
    // consoleInit(GFX_TOP, NULL);

    Result romfs_res = romfsInit();

    if (romfs_res) {
        mud_error("romfsInit: %08lX\n", romfs_res);
        exit(1);
    }

    gfxSetDoubleBuffering(GFX_BOTTOM, 0);
    gfxSetDoubleBuffering(GFX_TOP, 0);

    mud->_3ds_framebuffer_top =
        gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

#ifndef RENDER_3DS_GL
    mud->_3ds_framebuffer_bottom =
        gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
#endif

    /* allocate buffer for SOC service (networking) */
    SOC_buffer = (u32 *)memalign(SOC_ALIGN, SOC_BUFFER_SIZE);

    if (SOC_buffer == NULL) {
        mud_error("memalign() fail\n");
        exit(1);
    }

    int ret = -1;

    if ((ret = socInit(SOC_buffer, SOC_BUFFER_SIZE)) != 0) {
        mud_error("socInit: 0x%08X\n", (unsigned int)ret);
        exit(1);
    }

    audio_buffer =
        (u32 *)linearAlloc(SAMPLE_BUFFER_SIZE * BYTES_PER_SAMPLE * 2);

    ndspInit();

    ndspSetOutputMode(NDSP_OUTPUT_MONO);

    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, SAMPLE_RATE);
    ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);

    float mix[12] = {0};
    mix[0] = 1.0;
    mix[1] = 1.0;

    ndspChnSetMix(0, mix);

    wave_buf[0].data_vaddr = &audio_buffer[0];
    wave_buf[0].nsamples = SAMPLE_BUFFER_SIZE;

    // wave_buf[1].data_vaddr = &audio_buffer[SAMPLE_BUFFER_SIZE];
    // wave_buf[1].nsamples = SAMPLE_BUFFER_SIZE;

    ndspChnWaveBufAdd(0, &wave_buf[0]);
    // ndspChnWaveBufAdd(0, &wave_buf[1]);

    HIDUSER_EnableGyroscope();
#else

#ifdef WIN32
    WSADATA wsa_data = {0};
    int ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (ret < 0) {
        mud_error("WSAStartup() error: %d\n", WSAGetLastError());
        exit(1);
    }
#endif

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

#ifdef SDL12
    (void)SDL_EnableUNICODE(1);
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
            exit(1);
        }
    }
#endif
#endif

#ifdef SDL12
    SDL_WM_SetCaption("Runescape by Andrew Gower", NULL);
    // SDL_WM_SetIcon(IMG_Load("win/2003scape.png"),NULL);
#else
    uint32_t windowflags = SDL_WINDOW_SHOWN;
#if !defined(WII) && !defined(_3DS) && !defined(EMSCRIPTEN)
    windowflags |= SDL_WINDOW_RESIZABLE;
#endif
#endif

#ifdef RENDER_SW
#ifdef SDL12
    mud->screen = SDL_SetVideoMode(mud->game_width, mud->game_height, 32,
                                   SDL_HWSURFACE | SDL_RESIZABLE);
#else
    mud->window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         mud->game_width, mud->game_height, windowflags);

    SDL_SetWindowMinimumSize(mud->window, MUD_MIN_WIDTH, MUD_MIN_HEIGHT);
#endif

    mudclient_resize(mud);
#endif
#ifndef SDL12
    mud->default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    mud->hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
#endif

#ifdef RENDER_GL
    /*if (IMG_Init(IMG_INIT_PNG) == 0) {
        mud_error("unable to initialize sdl_image: %s\n", IMG_GetError());
    }*/
#ifdef SDL12
    mud->screen = SDL_SetVideoMode(mud->game_width, mud->game_height, 32,
                                   SDL_OPENGL | SDL_RESIZABLE);
#else
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
#endif

    windowflags |= SDL_WINDOW_OPENGL;

    mud->gl_window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         mud->game_width, mud->game_height, windowflags);

    SDL_SetWindowMinimumSize(mud->gl_window, MUD_MIN_WIDTH, MUD_MIN_HEIGHT);

    SDL_GLContext *context = SDL_GL_CreateContext(mud->gl_window);

    if (!context) {
        mud_error("SDL_GL_CreateContext(): %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_MakeCurrent(mud->gl_window, context);
#endif

#ifdef GLAD
#if defined(SDL_OPENGL) || defined(SDL_WINDOW_OPENGL)
    if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
        mud_error("Error loading GL library through GLAD/SDL\n");
        exit(1);
    }
#else
    if (gladLoadGL() == 0) {
        mud_error("Error loading GL library through GLAD\n");
        exit(1);
    }
#endif
    printf("INFO: Loaded OpenGL version %d.%d\n", GLVersion.major,
           GLVersion.minor);
#elif !defined(ANDROID)
    glewExperimental = GL_TRUE;

    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
        mud_error("GLEW error: %s\n", glewGetErrorString(glew_error));
        exit(1);
    }
#endif

    glViewport(0, 0, mud->game_width, mud->game_height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* when two vertices have the same depth, the last one gets drawn rather
     * than the first one. used for entity quads */
    glDepthFunc(GL_LEQUAL);

    /* transparent textures */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);

    glDisable(GL_MULTISAMPLE);
#endif
#endif

#ifdef RENDER_3DS_GL
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    mud->_3ds_gl_render_target =
        C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH16);
    // C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24);

    mud->_3ds_gl_offscreen_render_target =
        C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA5551, GPU_RB_DEPTH16);

    C3D_RenderTargetSetOutput(mud->_3ds_gl_render_target, GFX_BOTTOM, GFX_LEFT,
                              DISPLAY_TRANSFER_FLAGS);
#endif

    mud->surface = malloc(sizeof(Surface));

    surface_new(mud->surface, mud->game_width, mud->game_height, SPRITE_LIMIT,
                mud);

    surface_set_bounds(mud->surface, 0, 0, mud->game_width, mud->game_height);

    mud_log("Started application\n");

#ifdef _3DS
    mudclient_3ds_draw_top_background(mud);
#endif

#ifdef ANDROID
    SDL_SetWindowFullscreen(mud->window, SDL_WINDOW_FULLSCREEN);
#endif

    mudclient_run(mud);
}

void mudclient_handle_key_press(mudclient *mud, int key_code) {
    if (mud->show_additional_options) {
        Panel *panel = mudclient_get_active_option_panel(mud);
        panel_key_press(panel, key_code);
        return;
    }

    if (!mud->logged_in) {
        if (mud->login_screen == LOGIN_STAGE_WELCOME &&
            mud->panel_login_welcome) {
            panel_key_press(mud->panel_login_welcome, key_code);
        }

        if (mud->login_screen == LOGIN_STAGE_NEW && mud->panel_login_new_user) {
            panel_key_press(mud->panel_login_new_user, key_code);
        }

        if (mud->login_screen == LOGIN_STAGE_EXISTING &&
            mud->panel_login_existing_user) {
            panel_key_press(mud->panel_login_existing_user, key_code);
        }

        /*if (mud->login_screen == 3 && mud->panel_recover_user) {
            panel_key_press(mud->panel_recover_user, key_code);
        }*/
    } else {
        if (mud->show_appearance_change && mud->panel_appearance) {
            panel_key_press(mud->panel_appearance, key_code);
            return;
        }

        if (mud->show_change_password_step == PASSWORD_STEP_NONE &&
            mud->show_dialog_social_input == 0 &&
            mud->show_dialog_offer_x == 0 &&
            !(mud->bank_search_focus && mud->show_dialog_bank) &&
            /*mud->show_dialog_report_abuse_step == 0 &&*/
            !mud->is_sleeping && mud->panel_message_tabs) {
            int is_option_number = mud->options->option_numbers &&
                                   mud->show_option_menu && key_code >= '1' &&
                                   key_code <= '5';

            if (!is_option_number) {
                panel_key_press(mud->panel_message_tabs, key_code);
            }
        }

        if (mud->show_change_password_step == PASSWORD_STEP_MISMATCH ||
            mud->show_change_password_step == PASSWORD_STEP_FINISHED) {
            mud->show_change_password_step = PASSWORD_STEP_NONE;
        }
    }
}

void mudclient_key_pressed(mudclient *mud, int code, int char_code) {
    if (char_code == -1) {
        if (code == K_LEFT) {
            mud->key_left = 1;
        } else if (code == K_RIGHT) {
            mud->key_right = 1;
        } else if (code == K_UP) {
            mud->key_up = 1;
        } else if (code == K_DOWN) {
            mud->key_down = 1;
        } else if (code == K_PAGE_UP) {
            mud->key_page_up = 1;
        } else if (code == K_PAGE_DOWN) {
            mud->key_page_down = 1;
        } else if (code == K_HOME) {
            mud->key_home = 1;
        } else if (code == K_F1) {
            mud->options->interlace = !mud->options->interlace;

            /*for (int i = 0; i < mud->panel_game_options->control_count; i++) {
                if ((int *)mud->ui_options[i] == &mud->options->interlace) {
                    panel_toggle_checkbox(mud->panel_ui_options, i,
                                          mud->options->interlace);
                    break;
                }
            }*/
        } else if (mud->options->escape_clear && code == K_ESCAPE) {
            memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
            memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
            memset(mud->input_digits_current, '\0', INPUT_DIGITS_LENGTH + 1);
        }
    } else {
        if (code == K_TAB) {
            mud->key_tab = 1;
        } else if (code == K_1) {
            mud->key_1 = 1;
        } else if (code == K_2) {
            mud->key_2 = 1;
        } else if (code == K_3) {
            mud->key_3 = 1;
        } else if (code == K_4) {
            mud->key_4 = 1;
        } else if (code == K_5) {
            mud->key_5 = 1;
        }

        mudclient_handle_key_press(mud, char_code);
    }

    int found_text = 0;

    for (int i = 0; i < CHAR_SET_LENGTH; i++) {
        if (CHAR_SET[i] == char_code) {
            found_text = 1;
            break;
        }
    }

    int should_append_pm = (mud->show_dialog_bank ? mud->bank_search_focus : 1);

    if (found_text) {
        if (!mud->show_dialog_offer_x) {
            size_t current_length = strlen(mud->input_text_current);

            if (current_length < INPUT_TEXT_LENGTH) {
                mud->input_text_current[current_length] = char_code;
                mud->input_text_current[current_length + 1] = '\0';
            }

            size_t pm_length = strlen(mud->input_pm_current);

            if (pm_length < INPUT_PM_LENGTH && should_append_pm) {
                mud->input_pm_current[pm_length] = char_code;
                mud->input_pm_current[pm_length + 1] = '\0';
            }
        } else if ((IS_DIGIT_SEPARATOR(char_code) ||
                    IS_DIGIT_SUFFIX(char_code) ||
                    isdigit((unsigned char)char_code))) {
            size_t digits_length = strlen(mud->input_digits_current);

            if (digits_length < INPUT_DIGITS_LENGTH) {
                int add_digit_char = 1;

                if (digits_length > 0) {
                    int last_digit_char =
                        mud->input_digits_current[digits_length - 1];

                    /* only one suffix */
                    if (IS_DIGIT_SUFFIX(last_digit_char)) {
                        add_digit_char = 0;
                    } else {
                        /* don't allow consecutive decimals or separators */
                        add_digit_char = !(IS_DIGIT_SEPARATOR(char_code) &&
                                           IS_DIGIT_SEPARATOR(last_digit_char));
                    }
                } else {
                    /* don't allow separators or suffixes as first characters */
                    add_digit_char = !IS_DIGIT_SUFFIX(char_code) &&
                                     !IS_DIGIT_SEPARATOR(char_code);
                }

                if (add_digit_char) {
                    mud->input_digits_current[digits_length] = char_code;
                    mud->input_digits_current[digits_length + 1] = '\0';
                }
            }
        }
    }

    if (code == K_ENTER) {
        strcpy(mud->input_text_final, mud->input_text_current);

        if (should_append_pm) {
            strcpy(mud->input_pm_final, mud->input_pm_current);
        }

        if (mud->options->offer_x) {
            char filtered_digits[INPUT_DIGITS_LENGTH + 1] = {0};
            int filtered_length = 0;
            char digits_suffix = '\0';
            int has_decimal = 0;
            size_t digits_length = strlen(mud->input_digits_current);

            for (size_t i = 0; i < digits_length; i++) {
                char digit_char = mud->input_digits_current[i];

                if (isdigit((unsigned char)digit_char)) {
                    filtered_digits[filtered_length++] = digit_char;
                } else if (tolower((unsigned char)digit_char) == 'k' ||
                           tolower((unsigned char)digit_char) == 'm') {
                    digits_suffix = digit_char;
                } else if (!has_decimal && digit_char == '.') {
                    filtered_digits[filtered_length++] = digit_char;
                    has_decimal = 1;
                }
            }

            int scale = 1;

            if (digits_suffix == 'k') {
                scale = 1000;
            } else if (digits_suffix == 'm') {
                scale = 1000000;
            }

            mud->input_digits_final =
                (int)(atof(filtered_digits) * (float)scale);

            memset(mud->input_digits_current, '\0', INPUT_DIGITS_LENGTH + 1);
        }
    } else if (code == K_BACKSPACE) {
        size_t current_length = strlen(mud->input_text_current);

        if (current_length > 0) {
            mud->input_text_current[current_length - 1] = '\0';
        }

        size_t pm_length = strlen(mud->input_pm_current);

        if (pm_length > 0 && should_append_pm) {
            mud->input_pm_current[pm_length - 1] = '\0';
        }

        if (mud->options->offer_x) {
            size_t digits_length = strlen(mud->input_digits_current);

            if (digits_length > 0) {
                mud->input_digits_current[digits_length - 1] = '\0';
            }
        }
    }
}

void mudclient_key_released(mudclient *mud, int code) {
    if (code == K_LEFT) {
        mud->key_left = 0;
    } else if (code == K_RIGHT) {
        mud->key_right = 0;
    } else if (code == K_UP) {
        mud->key_up = 0;
    } else if (code == K_DOWN) {
        mud->key_down = 0;
    } else if (code == K_PAGE_UP) {
        mud->key_page_up = 0;
    } else if (code == K_PAGE_DOWN) {
        mud->key_page_down = 0;
    } else if (code == K_HOME) {
        mud->key_home = 0;
    } else if (code == K_TAB) {
        mud->key_tab = 0;
    } else if (code == K_1) {
        mud->key_1 = 0;
    } else if (code == K_2) {
        mud->key_2 = 0;
    } else if (code == K_3) {
        mud->key_3 = 0;
    } else if (code == K_4) {
        mud->key_4 = 0;
    } else if (code == K_5) {
        mud->key_5 = 0;
    }
}

void mudclient_mouse_moved(mudclient *mud, int x, int y) {
    mud->mouse_x = x;
    mud->mouse_y = y;

#ifdef RENDER_GL
    mud->gl_mouse_x = x;
    mud->gl_mouse_y = y;
#endif

    if (mudclient_is_ui_scaled(mud)) {
        mud->mouse_x /= 2;
        mud->mouse_y /= 2;
    }

    mud->mouse_action_timeout = 0;
}

void mudclient_mouse_released(mudclient *mud, int x, int y, int button) {
    mud->mouse_x = x;
    mud->mouse_y = y;

#ifdef RENDER_GL
    mud->gl_mouse_x = x;
    mud->gl_mouse_y = y;
#endif

    if (mudclient_is_ui_scaled(mud)) {
        mud->mouse_x /= 2;
        mud->mouse_y /= 2;
    }

    mud->mouse_button_down = 0;

    if (button == 2) {
        mud->middle_button_down = 0;

        int tick_delta = get_ticks() - mud->last_mouse_sample_ticks;

        if (tick_delta <= 0) {
            return;
        }

        int x_delta = x - mud->last_mouse_sample_x;

        mud->camera_momentum = 2 * ((float)x_delta / (float)tick_delta);
    }
}

void mudclient_handle_mouse_history(mudclient *mud, int x, int y) {
    mud->mouse_click_x_history[mud->mouse_click_count] = x;
    mud->mouse_click_y_history[mud->mouse_click_count] = y;

    mud->mouse_click_count =
        (mud->mouse_click_count + 1) & (MOUSE_HISTORY_LENGTH - 1);

    for (int i = 10; i < 4000; i++) {
        int i1 = (mud->mouse_click_count - i) & (MOUSE_HISTORY_LENGTH - 1);

        if (mud->mouse_click_x_history[i1] == x &&
            mud->mouse_click_y_history[i1] == y) {
            int flag = 0;

            for (int j = 1; j < i; j++) {
                int k1 =
                    (mud->mouse_click_count - j) & (MOUSE_HISTORY_LENGTH - 1);

                int l1 = (i1 - j) & (MOUSE_HISTORY_LENGTH - 1);

                if (mud->mouse_click_x_history[l1] != x ||
                    mud->mouse_click_y_history[l1] != y) {
                    flag = 1;
                }

                if (mud->mouse_click_x_history[k1] !=
                        mud->mouse_click_x_history[l1] ||
                    mud->mouse_click_y_history[k1] !=
                        mud->mouse_click_y_history[l1]) {
                    break;
                }

                if (j == i - 1 && flag && mud->combat_timeout == 0 &&
                    mud->logout_timeout == 0) {
                    mudclient_send_logout(mud);
                    return;
                }
            }
        }
    }
}

void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button) {
    mud->mouse_x = x;
    mud->mouse_y = y;

#ifdef RENDER_GL
    mud->gl_mouse_x = x;
    mud->gl_mouse_y = y;
#endif

    if (mudclient_is_ui_scaled(mud)) {
        mud->mouse_x /= 2;
        mud->mouse_y /= 2;
    }

    /*
     * in SDL12 mouse wheel scrolling is treated as digital button press,
     * while in SDL2 it is handled as a different type of event entirely.
     */
    if (button == 4 || button == 5) {
        if (mud->options->mouse_wheel) {
            if (button == 4) {
                mud->mouse_scroll_delta--;
            } else {
                mud->mouse_scroll_delta++;
            }
            return;
        } else {
            /* treat it as a right click when scrolling is disabled */
            button = 3;
        }
    }

    if (mud->options->middle_click_camera != 0 && button == 2) {
        mud->middle_button_down = 1;
        mud->origin_rotation = mud->camera_rotation;
        mud->origin_mouse_x = x;

        mud->last_mouse_sample_ticks = get_ticks();
        mud->last_mouse_sample_x = x;
        mud->camera_momentum = 0;
        return;
    }

    mud->mouse_button_down = button == 3 ? 2 : 1;
    mud->last_mouse_button_down = mud->mouse_button_down;
    mud->mouse_action_timeout = 0;

    mudclient_handle_mouse_history(mud, x, y);
}

void mudclient_set_target_fps(mudclient *mud, int fps) {
    mud->target_fps = 1000 / fps;
}

void mudclient_reset_timings(mudclient *mud) {
    for (int i = 0; i < 10; i++) {
        mud->timings[i] = 0;
    }
}

void mudclient_start(mudclient *mud) {
    if (mud->stop_timeout >= 0) {
        mud->stop_timeout = 0;
    }
}

void mudclient_stop(mudclient *mud) {
    if (mud->stop_timeout >= 0) {
        mud->stop_timeout = 4000 / mud->target_fps;
    }
}

void mudclient_draw_loading_progress(mudclient *mud, int percent, char *text) {
    surface_black_screen(mud->surface);

    /* hide the previously drawn textures */
    surface_draw_box(mud->surface, 0, 0, 128, 128, BLACK);

    if (!mud->options->lowmem) {
        /* jagex logo */
        int logo_sprite_id = SPRITE_LIMIT - 1;

        if (mud->surface->sprite_width[logo_sprite_id]) {
            int offset_x = 2;

            int logo_x = (mud->game_width / 2) -
                         (mud->surface->sprite_width[logo_sprite_id] / 2) -
                         offset_x;

            int logo_y = (mud->game_height / 2) -
                         (mud->surface->sprite_height[logo_sprite_id] / 2) - 46;

            surface_draw_sprite(mud->surface, logo_x, logo_y, logo_sprite_id);
        }
    }

    /* loading bar */
    int bar_x = (mud->game_width / 2.0f) - (LOADING_WIDTH / 2.0f);
    int bar_y = (mud->game_height / 2) + 2;
    int width = (int)((percent / (float)100) * LOADING_WIDTH);

    surface_draw_border(mud->surface, bar_x - 2, bar_y - 2, LOADING_WIDTH + 4,
                        LOADING_HEIGHT + 4, GREY_84);

    surface_draw_box(mud->surface, bar_x, bar_y, LOADING_WIDTH, LOADING_HEIGHT,
                     BLACK);

    surface_draw_box(mud->surface, bar_x, bar_y, width, LOADING_HEIGHT,
                     GREY_84);

    int copyright_x = (mud->surface->width / 2) - 1;
    int copyright_y = (mud->surface->height / 2) + 16;

    if (game_fonts[2] != NULL) {
        surface_draw_string_centre(mud->surface, text, copyright_x, copyright_y,
                                   FONT_REGULAR_12, GREY_C6);
    }

    /* footer */
    if (game_fonts[3] != NULL) {
        copyright_y += 20;

        surface_draw_string_centre(
            mud->surface, "Created by JAGeX - visit www.jagex.com", copyright_x,
            copyright_y, FONT_BOLD_13, GREY_C6);

        copyright_x += 7;
        copyright_y += 16;

        char *copyright_date = "2001-2002 Andrew Gower and Jagex Ltd";

        int copyright_icon_x =
            copyright_x - (surface_text_width(copyright_date, 3) / 2) - 8;

        surface_draw_circle(mud->surface, copyright_icon_x + 2, copyright_y - 5,
                            5, GREY_C6, 255, 0);

        surface_draw_circle(mud->surface, copyright_icon_x + 2, copyright_y - 5,
                            4, BLACK, 255, 0);

        surface_draw_string(mud->surface, "c", copyright_icon_x,
                            copyright_y - 2, FONT_REGULAR_11, GREY_C6);

        surface_draw_string_centre(mud->surface, copyright_date, copyright_x,
                                   copyright_y, FONT_BOLD_13, GREY_C6);
    }

#ifdef RENDER_GL
    if (mud->gl_last_swap == 0 || get_ticks() - mud->gl_last_swap >= 16) {
        mudclient_poll_events(mud);
        surface_draw(mud->surface);
#ifdef SDL12
        SDL_GL_SwapBuffers();
#else
        SDL_GL_SwapWindow(mud->gl_window);
#endif
        mud->gl_last_swap = get_ticks();
    } else {
        surface_gl_reset_context(mud->surface);
    }
#elif defined(RENDER_3DS_GL)
    mudclient_3ds_gl_frame_start(mud, 1);
    surface_draw(mud->surface);
    mudclient_3ds_gl_frame_end();
#else
    surface_draw(mud->surface);
#endif
}

int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent) {
    char loading_text[35] = {0}; /* max description is 19 */

    sprintf(loading_text, "Loading %s - 0%%", description);
    mudclient_draw_loading_progress(mud, percent, loading_text);

    int8_t header[6];
#ifdef WII
    const int8_t *file_data = NULL;

    if (strcmp(file, "jagex.jag") == 0) {
        file_data = (int8_t *)jagex_jag;
    } else if (strcmp(file, "fonts" VERSION_STR(VERSION_FONTS) ".jag") == 0) {
        file_data = (int8_t *)fonts1_jag;
    } else if (strcmp(file, "config" VERSION_STR(VERSION_CONFIG) ".jag") == 0) {
        file_data = (int8_t *)config85_jag;
    } else if (strcmp(file, "media" VERSION_STR(VERSION_MEDIA) ".jag") == 0) {
        file_data = (int8_t *)media58_jag;
    } else if (strcmp(file, "entity" VERSION_STR(VERSION_ENTITY) ".jag") == 0) {
        file_data = (int8_t *)entity24_jag;
    } else if (strcmp(file, "entity" VERSION_STR(VERSION_ENTITY) ".mem") == 0) {
        file_data = (int8_t *)entity24_mem;
    } else if (strcmp(file, "textures" VERSION_STR(VERSION_TEXTURES) ".jag") ==
               0) {
        file_data = (int8_t *)textures17_jag;
    } else if (strcmp(file, "maps" VERSION_STR(VERSION_MAPS) ".jag") == 0) {
        file_data = (int8_t *)maps63_jag;
    } else if (strcmp(file, "maps" VERSION_STR(VERSION_MAPS) ".mem") == 0) {
        file_data = (int8_t *)maps63_mem;
    } else if (strcmp(file, "land" VERSION_STR(VERSION_MAPS) ".jag") == 0) {
        file_data = (int8_t *)land63_jag;
    } else if (strcmp(file, "land" VERSION_STR(VERSION_MAPS) ".mem") == 0) {
        file_data = (int8_t *)land63_mem;
    } else if (strcmp(file, "models" VERSION_STR(VERSION_MODELS) ".jag") == 0) {
        file_data = (int8_t *)models36_jag;
    } else if (strcmp(file, "sounds" VERSION_STR(VERSION_SOUNDS) ".mem") == 0) {
        file_data = (int8_t *)sounds1_mem;
    }

    if (file_data == NULL) {
        mud_error("Unable to read file: %s\n", file);
        exit(1);
    }

    memcpy(header, file_data, sizeof(header));
#else

#ifdef ANDROID
    char *prefixed_file = file;
    SDL_RWops *archive_stream = SDL_RWFromFile(prefixed_file, "rb");
#elif defined(_3DS) || defined(__SWITCH__)
    char prefixed_file[PATH_MAX];
    snprintf(prefixed_file, "romfs:/%s", file);
#else
    char prefixed_file[PATH_MAX];
    snprintf(prefixed_file, sizeof(prefixed_file), "./cache/%s", file);

    /* attempt to read cache from the current working directory first */
    printf("INFO: Loading %s\n", prefixed_file);
    FILE *archive_stream = fopen(prefixed_file, "rb");
    if (archive_stream == NULL) {
        /* cwd failed, now try the xdg home directory... */
        const char *xdg_home = getenv("XDG_DATA_HOME");

        if (xdg_home == NULL) {
            const char *home = getenv("HOME");
            if (home == NULL) {
                home = "";
            }
            snprintf(prefixed_file, sizeof(prefixed_file),
                     "%s/.local/share/rsc-c/%s", home, file);
        } else {
            snprintf(prefixed_file, sizeof(prefixed_file), "%s/rsc-c/%s",
                     xdg_home, file);
        }

        printf("INFO: Loading %s\n", prefixed_file);
        archive_stream = fopen(prefixed_file, "rb");

        /* XDG failed, now try the global prefix... */
        if (archive_stream == NULL) {
            snprintf(prefixed_file, sizeof(prefixed_file), "%s/%s", MUD_DATADIR,
                     file);

            printf("INFO: Loading %s\n", prefixed_file);
            archive_stream = fopen(prefixed_file, "rb");
        }
    }
#endif

    if (archive_stream == NULL) {
        mud_error("Unable to read file: %s\n", prefixed_file);
        exit(1);
    }

#ifdef ANDROID
    SDL_RWread(archive_stream, header, sizeof(header), 1);
#else
    fread(header, sizeof(header), 1, archive_stream);
#endif
#endif

    int archive_size = ((header[0] & 0xff) << 16) + ((header[1] & 0xff) << 8) +
                       (header[2] & 0xff);

    int archive_size_compressed = ((header[3] & 0xff) << 16) +
                                  ((header[4] & 0xff) << 8) +
                                  (header[5] & 0xff);

    sprintf(loading_text, "Loading %s - 5%%", description);
    mudclient_draw_loading_progress(mud, percent, loading_text);

#ifdef WII
    int8_t *archive_data = file_data + 6;
#else
    int read = 0;
    int8_t *archive_data = malloc(archive_size_compressed);

    while (read < archive_size_compressed) {
        int length = archive_size_compressed - read;

#ifdef ANDROID
        SDL_RWread(archive_stream, archive_data + read, length, 1);
#else
        fread(archive_data + read, length, 1, archive_stream);
#endif

        read += length;

        sprintf(loading_text, "Loading %s - %d%%", description,
                5 + (read * 95) / archive_size_compressed);

        mudclient_draw_loading_progress(mud, percent, loading_text);
    }

#ifdef ANDROID
    SDL_RWclose(archive_stream);
#else
    fclose(archive_stream);
#endif
#endif

    sprintf(loading_text, "Unpacking %s", description);
    mudclient_draw_loading_progress(mud, percent, loading_text);

    if (archive_size_compressed != archive_size) {
        int8_t *decompressed = malloc(archive_size);
        bzip_decompress(decompressed, archive_data, archive_size_compressed, 0);

#ifndef WII
        free(archive_data);
#endif

        return decompressed;
    }

    return archive_data;
}

void mudclient_load_jagex(mudclient *mud) {
#if defined(RENDER_SW)
    int8_t *jagex_jag =
        mudclient_read_data_file(mud, "jagex.jag", "Jagex library", 0);

    if (jagex_jag != NULL) {
        if (!mud->options->lowmem) {
            size_t len = 0;
            int8_t *logo_tga = load_data("logo.tga", 0, jagex_jag, &len);

            surface_parse_sprite_tga(mud->surface, SPRITE_LIMIT - 1, logo_tga,
                                     len, 0, 0);

            free(logo_tga);
        }

#ifndef WII
        free(jagex_jag);
#endif
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    int logo_sprite_id = SPRITE_LIMIT - 1;

    mud->surface->sprite_width[logo_sprite_id] = 281;
    mud->surface->sprite_height[logo_sprite_id] = 85;
#endif

    int8_t *fonts_jag = mudclient_read_data_file(
        mud, "fonts" VERSION_STR(VERSION_FONTS) ".jag", "Game fonts", 5);

    if (fonts_jag != NULL) {
        for (size_t i = 0; i < FONT_FILES_LENGTH; i++) {
            create_font(load_data(font_files[i], 0, fonts_jag, NULL), i);
        }

        free(fonts_jag);
    }
}

void mudclient_load_game_config(mudclient *mud) {
    int8_t *config_jag = mudclient_read_data_file(
        mud, "config" VERSION_STR(VERSION_CONFIG) ".jag", "Configuration", 10);

    if (config_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    game_data_load_data(config_jag, mud->options->members);
    free(config_jag);

    /*int8_t *filter_jag = mudclient_read_data_file(
        mud, "filter" VERSION_STR(VERSION_FILTER) ".jag", "Chat system", 15);

    if (filter_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    free(filter_jag);*/

    if (mud->options->members && mud->options->rename_herblaw_items) {
        modify_unidentified_herbs();
        modify_unfinished_potions();
    }
    if (mud->options->rename_herblaw_items) {
        modify_potion_dosage();
    }
}

static void mudclient_load_media_dat(mudclient *mud, void *media_jag) {
    int8_t *index_dat = load_data("index.dat", 0, media_jag, NULL);

#if (VERSION_MEDIA < 59)
    surface_parse_sprite(mud->surface, mud->sprite_media,
                         load_data("inv1.dat", 0, media_jag, NULL), index_dat,
                         1);
#endif

    surface_parse_sprite(mud->surface, mud->sprite_media + 1,
                         load_data("inv2.dat", 0, media_jag, NULL), index_dat,
                         6);

    surface_parse_sprite(mud->surface, mud->sprite_media + 9,
                         load_data("bubble.dat", 0, media_jag, NULL), index_dat,
                         1);

    if (!mud->options->lowmem) {
        surface_parse_sprite(mud->surface, mud->sprite_media + 10,
                             load_data("runescape.dat", 0, media_jag, NULL),
                             index_dat, 1);
    }

    surface_parse_sprite(mud->surface, mud->sprite_media + 11,
                         load_data("splat.dat", 0, media_jag, NULL), index_dat,
                         3);

    surface_parse_sprite(mud->surface, mud->sprite_media + 14,
                         load_data("icon.dat", 0, media_jag, NULL), index_dat,
                         8);

    if (!mud->options->lowmem) {
        surface_parse_sprite(mud->surface, mud->sprite_media + 22,
                             load_data("hbar.dat", 0, media_jag, NULL),
                             index_dat, 1);
    }

    surface_parse_sprite(mud->surface, mud->sprite_media + 23,
                         load_data("hbar2.dat", 0, media_jag, NULL), index_dat,
                         1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 24,
                         load_data("compass.dat", 0, media_jag, NULL),
                         index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 25,
                         load_data("buttons.dat", 0, media_jag, NULL),
                         index_dat, 2);

#if (VERSION_MEDIA >= 59)
    surface_parse_sprite(mud->surface, mud->sprite_media + 27,
                         load_data("labels.dat", 0, media_jag, NULL), index_dat,
                         6);

    surface_parse_sprite(mud->surface, mud->sprite_media + 33,
                         load_data("inv3.dat", 0, media_jag, NULL), index_dat,
                         6);

    surface_parse_sprite(mud->surface, mud->sprite_media + 39,
                         load_data("message.dat", 0, media_jag, NULL),
                         index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 40,
                         load_data("keyboard.dat", 0, media_jag, NULL),
                         index_dat, 1);
#endif

    surface_parse_sprite(mud->surface, mud->sprite_util,
                         load_data("scrollbar.dat", 0, media_jag, NULL),
                         index_dat, 2);

    surface_parse_sprite(mud->surface, mud->sprite_util + 2,
                         load_data("corners.dat", 0, media_jag, NULL),
                         index_dat, 4);

    surface_parse_sprite(mud->surface, mud->sprite_util + 6,
                         load_data("arrows.dat", 0, media_jag, NULL), index_dat,
                         2);

    surface_parse_sprite(mud->surface, mud->sprite_projectile,
                         load_data("projectile.dat", 0, media_jag, NULL),
                         index_dat, game_data.projectile_sprite);

    int sprite_count = game_data.item_sprite_count;

    for (int i = 1; sprite_count > 0; i++) {
        char file_name[20] = {0};
        sprintf(file_name, "objects%d.dat", i);

        int current_sprite_count = sprite_count;
        sprite_count -= 30;

        if (current_sprite_count > 30) {
            current_sprite_count = 30;
        }

        surface_parse_sprite(mud->surface, mud->sprite_item + (i - 1) * 30,
                             load_data(file_name, 0, media_jag, NULL),
                             index_dat, current_sprite_count);
    }

    free(index_dat);
}

static void mudclient_load_media_tga(mudclient *mud, void *media_jag) {
    void *data;
    size_t len;

    data = load_data("inv1.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media,
                         data, len, 1, 1);

    data = load_data("inv2.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 1,
                         data, len, 1, 6);

    data = load_data("bubble.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 9,
                         data, len, 1, 1);

    if (!mud->options->lowmem) {
        data = load_data("runescape.tga", 0, media_jag, &len);
        assert(data != NULL);
        surface_parse_sprite_tga(mud->surface, mud->sprite_media + 10,
                             data, len, 1, 1);
    }

    data = load_data("splat.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 11,
                             data, len, 3, 1);

    data = load_data("icon.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 14,
                             data, len, 4, 2);

    if (!mud->options->lowmem) {
        data = load_data("hbar.tga", 0, media_jag, &len);
        assert(data != NULL);
        surface_parse_sprite_tga(mud->surface, mud->sprite_media + 22,
                                 data, len, 1, 1);
    }

    data = load_data("hbar2.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 23,
                             data, len, 1, 1);

    data = load_data("compass.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 24,
                             data, len, 1, 1);

    data = load_data("buttons.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_media + 25,
                             data, len, 1, 2);

    data = load_data("scrollbar.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_util,
                             data, len, 2, 1);

    data = load_data("corners.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_util + 2,
                             data, len, 4, 1);

    data = load_data("arrows.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_util + 6,
                         data, len, 2, 1);

    data = load_data("projectile.tga", 0, media_jag, &len);
    assert(data != NULL);
    surface_parse_sprite_tga(mud->surface, mud->sprite_projectile,
                             data, len, 3, 1);

    int sprite_count = game_data.item_sprite_count;

    for (int i = 1; sprite_count > 0; i++) {
        char file_name[32];

        snprintf(file_name, sizeof(file_name), "objects%d.tga", i);

        data = load_data(file_name, 0, media_jag, &len);
        if (data == NULL) {
            break;
        }

        int current_sprite_count = sprite_count;
        sprite_count -= 30;

        if (current_sprite_count > 30) {
            current_sprite_count = 30;
        }

        surface_parse_sprite_tga(mud->surface, mud->sprite_item + (i - 1) * 30,
                             data, len, 10, 3);
    }
}

void mudclient_load_media(mudclient *mud) {
#if defined(RENDER_GL) || defined(RENDER_SW) || defined(RENDER_3DS_GL)
    int8_t *media_jag = mudclient_read_data_file(
        mud, "media" VERSION_STR(VERSION_MEDIA) ".jag", "2d graphics", 20);

    if (media_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

#if !MEDIA_IS_TGA
    mudclient_load_media_dat(mud, media_jag);
#else
    mudclient_load_media_tga(mud, media_jag);
#endif

#ifdef RENDER_SW
    /* this is probably for an optimization, but it is necessary for the action
     * bubble scaling */
#if (VERSION_MEDIA >= 59)
    for (int i = 0; i < 6; i++) {
        surface_load_sprite(mud->surface, mud->sprite_media + 33 + i);
    }

    surface_load_sprite(mud->surface, mud->sprite_media + 39);
#else
    surface_load_sprite(mud->surface, mud->sprite_media);
#endif

    surface_load_sprite(mud->surface, mud->sprite_media + 9);

    for (int i = 11; i <= 26; i++) {
        surface_load_sprite(mud->surface, mud->sprite_media + i);
    }

    for (int i = 0; i < game_data.projectile_sprite; i++) {
        surface_load_sprite(mud->surface, mud->sprite_projectile + i);
    }

    for (int i = 0; i < game_data.item_sprite_count; i++) {
        surface_load_sprite(mud->surface, mud->sprite_item + i);
    }
#endif

#ifndef WII
    free(media_jag);
#endif
#endif
}

void mudclient_load_entities(mudclient *mud) {
#if defined(RENDER_GL) || defined(RENDER_SW) || defined(RENDER_3DS_GL)
    int8_t *entity_jag = mudclient_read_data_file(
        mud, "entity" VERSION_STR(VERSION_ENTITY) ".jag", "people and monsters",
        30);

    int8_t *entity_jag_legacy = NULL;

#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
    if (mud->options->tga_sprites) {
        entity_jag_legacy = mudclient_read_data_file(mud, "entity8.jag",
                                                     "people and monsters", 37);
    }
    if (ENTITY_IS_TGA) {
        entity_jag_legacy = entity_jag;
    }
#endif

    if (entity_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    int8_t *index_dat = load_data("index.dat", 0, entity_jag, NULL);
    int8_t *entity_jag_mem = NULL;
    int8_t *index_dat_mem = NULL;

    if (mud->options->members && !ENTITY_IS_TGA) {
        entity_jag_mem = mudclient_read_data_file(
            mud, "entity" VERSION_STR(VERSION_ENTITY) ".mem", "member graphics",
            45);

        if (entity_jag_mem == NULL) {
            mud->error_loading_data = 1;
            return;
        }

        index_dat_mem = load_data("index.dat", 0, entity_jag_mem, NULL);
    }

    int frame_count = 0;
    int animation_index = 0;

    int i = 0;

    while (i < game_data.animation_count) {
    label0:;
        char *animation_name = game_data.animations[i].name;

        for (int j = 0; j < i; j++) {
            if (strcmp(game_data.animations[j].name, animation_name) != 0) {
                continue;
            }

            game_data.animations[i].file_id = game_data.animations[j].file_id;
            i++;
            goto label0;
        }

        bool older_is_better = false;
        const char *extension = "dat";
        int8_t *archive_file = entity_jag;

#if !defined(RENDER_GL) && !defined(RENDER_3DS_GL)
        if (ENTITY_IS_TGA) {
            older_is_better = true;
            extension = "tga";
        } else if (mud->options->tga_sprites) {
            const char **older_names = anims_older_is_better;
            while (*older_names != NULL) {
                if (strcmp(animation_name, *older_names) == 0) {
                    older_is_better = true;
                    extension = "tga";
                    archive_file = entity_jag_legacy;
                    break;
                }

                older_names++;
            }
        }
#endif

        char file_name[255] = {0};
        sprintf(file_name, "%s.%s", animation_name, extension);

        size_t len = 0;

        int8_t *animation_dat = load_data(file_name, 0, archive_file, &len);
        int8_t *animation_index_dat = index_dat;

        if (animation_dat == NULL && mud->options->members) {
            animation_dat = load_data(file_name, 0, entity_jag_mem, &len);
            animation_index_dat = index_dat_mem;
        }

        if (animation_dat != NULL) {
            if (older_is_better) {
                surface_parse_sprite_tga(mud->surface, animation_index,
                                         animation_dat, len, 15, 1);
            } else {
                surface_parse_sprite(mud->surface, animation_index,
                                     animation_dat, animation_index_dat, 15);
            }

            frame_count += 15;

            if (game_data.animations[i].has_a) {
                if (older_is_better && strcmp(animation_name, "camel") == 0) {
                    /* camel attack anim was a much later addition */
                    older_is_better = false;
                    extension = "dat";
                    archive_file = entity_jag;
                }

                sprintf(file_name, "%sa.%s", animation_name, extension);

                int8_t *a_dat = load_data(file_name, 0, archive_file, &len);
                int8_t *a_index_dat = index_dat;

                if (a_dat == NULL && mud->options->members) {
                    a_dat = load_data(file_name, 0, entity_jag_mem, &len);
                    a_index_dat = index_dat_mem;
                }

                if (a_dat == NULL) {
                    goto fallthrough;
                }

                if (older_is_better) {
                    surface_parse_sprite_tga(mud->surface, animation_index + 15,
                                             a_dat, len, 3, 1);
                } else {
                    surface_parse_sprite(mud->surface, animation_index + 15,
                                         a_dat, a_index_dat, 3);
                }

                frame_count += 3;
            }

            if (game_data.animations[i].has_f) {
                sprintf(file_name, "%sf.%s", animation_name, extension);

                int8_t *f_dat = load_data(file_name, 0, archive_file, &len);
                int8_t *f_index_dat = index_dat;

                if (f_dat == NULL && mud->options->members) {
                    f_dat = load_data(file_name, 0, entity_jag_mem, &len);
                    f_index_dat = index_dat_mem;
                }

                if (older_is_better) {
                    surface_parse_sprite_tga(mud->surface, animation_index + 18,
                                             f_dat, len, 9, 1);
                } else {
                    surface_parse_sprite(mud->surface, animation_index + 18,
                                         f_dat, f_index_dat, 9);
                }

                frame_count += 9;
            }

fallthrough:
            /* TODO why? */
            if (game_data.animations[i].gender != 0) {
                for (int j = animation_index; j < animation_index + 27; j++) {
                    surface_load_sprite(mud->surface, j);
                }
            }
        }

        game_data.animations[i].file_id = animation_index;
        animation_index += 27;

        i++;
    }

    mud_log("Loaded: %d frames of animation\n", frame_count);

#ifndef WII
    free(entity_jag);
    if (entity_jag_legacy != entity_jag) {
        free(entity_jag_legacy);
    }
    free(entity_jag_mem);
#endif

    free(index_dat);
    free(index_dat_mem);
#endif
}

void mudclient_load_textures(mudclient *mud) {
#ifdef RENDER_SW
    int8_t *textures_jag = mudclient_read_data_file(
        mud, "textures" VERSION_STR(VERSION_TEXTURES) ".jag", "Textures", 50);

    if (textures_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    int8_t *index_dat = load_data("index.dat", 0, textures_jag, NULL);

    scene_allocate_textures(mud->scene, game_data.texture_count, 7, 11);

    char file_name[255] = {0};

    Surface *surface = mud->surface;

    for (int i = 0; i < game_data.texture_count; i++) {
#ifdef USE_TOONSCAPE
        if (toonscape_avoid_load(i)) {
            continue;
        }
#endif
        sprintf(file_name, "%s.dat", game_data.textures[i].name);

        int8_t *texture_dat = load_data(file_name, 0, textures_jag, NULL);

        surface_parse_sprite(surface, mud->sprite_texture, texture_dat,
                             index_dat, 1);

        surface_draw_box(surface, 0, 0, 128, 128, MAGENTA);
        surface_draw_sprite(surface, 0, 0, mud->sprite_texture);

#ifndef USE_LOCOLOUR
        free(surface->sprite_palette[mud->sprite_texture]);
        surface->sprite_palette[mud->sprite_texture] = NULL;
#endif

        free(surface->sprite_colours[mud->sprite_texture]);
        surface->sprite_colours[mud->sprite_texture] = NULL;

        int texture_size = surface->sprite_width_full[mud->sprite_texture];
        char *name_sub = game_data.textures[i].subtype_name;

        if (name_sub) {
            int sub_length = strlen(name_sub);

            if (sub_length > 0 && sub_length <= 250) {
                sprintf(file_name, "%s.dat", name_sub);

                int8_t *texture_sub_dat =
                    load_data(file_name, 0, textures_jag, NULL);

                surface_parse_sprite(surface, mud->sprite_texture,
                                     texture_sub_dat, index_dat, 1);

                surface_draw_sprite(surface, 0, 0, mud->sprite_texture);

#ifndef USE_LOCOLOUR
                free(surface->sprite_palette[mud->sprite_texture]);
                surface->sprite_palette[mud->sprite_texture] = NULL;
#endif

                free(surface->sprite_colours[mud->sprite_texture]);
                surface->sprite_colours[mud->sprite_texture] = NULL;
            }
        }

        surface_screen_raster_to_sprite(surface, mud->sprite_texture_world + i,
                                        0, 0, texture_size, texture_size);

        for (int j = 0; j < texture_size * texture_size; j++) {
            if (surface->surface_pixels[mud->sprite_texture_world + i][j] ==
                GREEN) {
                surface->surface_pixels[mud->sprite_texture_world + i][j] =
                    MAGENTA;
            }
        }

        surface_screen_raster_to_palette_sprite(surface,
                                                mud->sprite_texture_world + i);

        scene_define_texture(
            mud->scene, i,
            surface->sprite_colours[mud->sprite_texture_world + i],
            surface->sprite_palette[mud->sprite_texture_world + i],
            (texture_size / 64) - 1);

        free(surface->surface_pixels[mud->sprite_texture_world + i]);
        surface->surface_pixels[mud->sprite_texture_world + i] = NULL;
    }

    free(index_dat);

#ifndef WII
    free(textures_jag);
#endif
#else
    (void)mud;
#endif
}

void mudclient_load_models(mudclient *mud) {
    if (!mud->options->lowmem) {
        for (int i = 0; i < ANIMATED_MODELS_LENGTH; i++) {
            game_data_get_model_index(mud_strdup(animated_models[i]));
        }
    }

    char *models_filename = "models" VERSION_STR(VERSION_MODELS) ".jag";

    int8_t *models_jag =
        mudclient_read_data_file(mud, models_filename, "3d models", 60);

    if (models_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    for (int i = 0; i < game_data.model_count; i++) {
        char *model_name = game_data.model_name[i];

        char file_name[strlen(model_name) + 5];
        sprintf(file_name, "%s.ob3", model_name);

        uint32_t offset = get_data_file_offset(file_name, models_jag);
        uint32_t len = get_data_file_length(file_name, models_jag);

        GameModel *game_model = malloc(sizeof(GameModel));

        if (offset != 0) {
            game_model_new_ob3(game_model, models_jag + offset, len);
        } else {
            mud_error("missing model \"%s.ob3\" from %s\n", model_name,
                      models_filename);

            game_model_new_alloc(game_model, 1, 1);
        }

        mud->game_models[i] = game_model;

        if (strcmp(model_name, "giantcrystal") == 0) {
            mud->game_models[i]->transparent = 1;
        }
    }

    if (mud->options->ground_item_models) {
#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        mud->item_models = calloc(game_data.item_count, sizeof(GameModel *));

        for (int i = 0; i < game_data.item_count; i++) {
            int sprite_id = game_data.items[i].sprite;

            char file_name[21] = {0};
            sprintf(file_name, "item-%d.ob3", sprite_id);

            uint32_t offset = get_data_file_offset(file_name, models_jag);
            uint32_t len = get_data_file_length(file_name, models_jag);

            if (offset == 0) {
                continue;
            }

            GameModel *game_model = malloc(sizeof(GameModel));
            game_model_new_ob3(game_model, models_jag + offset, len);

            int mask_colour = game_data.items[i].mask;

            if (mask_colour != 0) {
                game_model_mask_faces(game_model, game_model->face_fill_back,
                                      mask_colour);

                game_model_mask_faces(game_model, game_model->face_fill_front,
                                      mask_colour);
            }

            mud->item_models[i] = game_model;
        }
#else
        int max_sprite_id = 0;

        for (int i = 0; i < game_data.item_count; i++) {
            int sprite_id = game_data.items[i].sprite;

            if (sprite_id > max_sprite_id) {
                max_sprite_id = sprite_id;
            }
        }

        mud->item_models = calloc(max_sprite_id, sizeof(GameModel *));

        for (int i = 0; i < max_sprite_id; i++) {
            char file_name[21] = {0};
            sprintf(file_name, "item-%d.ob3", i);

            uint32_t offset = get_data_file_offset(file_name, models_jag);
            uint32_t len = get_data_file_length(file_name, models_jag);

            if (offset == 0) {
                continue;
            }

            GameModel *game_model = malloc(sizeof(GameModel));
            game_model_new_ob3(game_model, models_jag + offset, len);

            mud->item_models[i] = game_model;
        }
#endif
    }

    free(models_jag);

#ifdef RENDER_GL
    int models_length = game_data.model_count - 1;
    int item_models_length = game_data.item_count;

    if (mud->options->ground_item_models) {
        models_length += item_models_length;
    }

    GameModel *models_buffer[models_length];

    for (int i = 0; i < game_data.model_count - 1; i++) {
        models_buffer[i] = mud->game_models[i];
    }

    if (mud->options->ground_item_models) {
        for (int i = 0; i < item_models_length; i++) {
            models_buffer[game_data.model_count - 1 + i] = mud->item_models[i];
        }
    }

    game_model_gl_buffer_models(&mud->scene->gl_game_model_buffers,
                                &mud->scene->gl_game_model_buffer_length,
                                models_buffer, models_length);
#endif
}

void mudclient_load_maps(mudclient *mud) {
    mud->world->map_pack = mudclient_read_data_file(
        mud, "maps" VERSION_STR(VERSION_MAPS) ".jag", "map", 70);

    if (mud->options->members) {
        mud->world->member_map_pack = mudclient_read_data_file(
            mud, "maps" VERSION_STR(VERSION_MAPS) ".mem", "members map", 75);
    }

#if HAS_SEPARATE_LAND
    mud->world->landscape_pack = mudclient_read_data_file(
        mud, "land" VERSION_STR(VERSION_MAPS) ".jag", "landscape", 80);

    if (mud->options->members) {
        mud->world->member_landscape_pack = mudclient_read_data_file(
            mud, "land" VERSION_STR(VERSION_MAPS) ".mem", "members landscape",
            85);
    }
#endif
}

void mudclient_load_sounds(mudclient *mud) {
    mud->sound_data = mudclient_read_data_file(
        mud, "sounds" VERSION_STR(VERSION_SOUNDS) ".mem", "Sound effects", 90);
}

void mudclient_reset_game(mudclient *mud) {
#ifndef REVISION_177
    mud->system_update = 0;
#endif

    mud->combat_style = 0;
    mud->logout_timeout = 0;
    mud->login_screen = 0;
    mud->logged_in = 1;

    memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
    memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);

    surface_black_screen(mud->surface);

#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_frame_start(mud, 1);
    surface_draw(mud->surface);
    mudclient_3ds_gl_frame_end();
#else
    surface_draw(mud->surface);
#endif

    for (int i = 0; i < mud->object_count; i++) {
        scene_remove_model(mud->scene, mud->objects[i].model);

        world_remove_object(mud->world, mud->objects[i].x, mud->objects[i].y,
                            mud->objects[i].id);

#ifdef RENDER_SW
        game_model_destroy(mud->objects[i].model);
#endif
        free(mud->objects[i].model);
        mud->objects[i].model = NULL;
    }

    for (int i = 0; i < mud->wall_object_count; i++) {
        scene_remove_model(mud->scene, mud->wall_objects[i].model);

        world_remove_wall_object(
            mud->world, mud->wall_objects[i].x, mud->wall_objects[i].y,
            mud->wall_objects[i].direction, mud->wall_objects[i].id);

        game_model_destroy(mud->wall_objects[i].model);
        free(mud->wall_objects[i].model);
        mud->wall_objects[i].model = NULL;
    }

    mud->object_count = 0;
    mud->wall_object_count = 0;
    mud->ground_item_count = 0;
    mud->player_count = 0;

    GameCharacter *freed_characters[NPCS_SERVER_MAX] = {0};
    int freed_count = 0;

    for (int i = 0; i < PLAYERS_SERVER_MAX; i++) {
        GameCharacter *player = mud->player_server[i];

        if (player != NULL) {
            freed_characters[freed_count++] = player;
            free(player);
            mud->player_server[i] = NULL;
        }
    }

    for (int i = 0; i < PLAYERS_MAX; i++) {
    label0:;
        GameCharacter *player = mud->players[i];

        if (player) {
            for (int j = 0; j < NPCS_SERVER_MAX; j++) {
                if (freed_characters[j] == player) {
                    mud->players[i] = NULL;
                    i++;
                    goto label0;
                }
            }
        }

        free(player);
        mud->players[i] = NULL;
    }

    mud->combat_target = NULL;
    mud->local_player = malloc(sizeof(GameCharacter));
    game_character_new(mud->local_player);

    memset(freed_characters, 0, sizeof(GameCharacter *) * NPCS_SERVER_MAX);
    freed_count = 0;

    mud->npc_count = 0;

    for (int i = 0; i < NPCS_SERVER_MAX; i++) {
        GameCharacter *npc = mud->npcs_server[i];

        if (npc != NULL) {
            freed_characters[freed_count++] = npc;
            free(npc);
            mud->npcs_server[i] = NULL;
        }
    }

    for (int i = 0; i < NPCS_MAX; i++) {
    label1:;
        GameCharacter *npc = mud->npcs[i];

        if (npc != NULL) {
            for (int j = 0; j < freed_count; j++) {
                if (freed_characters[j] == npc) {
                    mud->npcs[i] = NULL;
                    i++;
                    goto label1;
                }
            }
        }

        free(npc);
        mud->npcs[i] = NULL;
    }

    for (int i = 0; i < PRAYER_COUNT; i++) {
        mud->prayer_on[i] = 0;
    }

    mud->mouse_button_click = 0;
    mud->last_mouse_button_down = 0;
    mud->mouse_button_down = 0;
    mud->show_dialog_shop = 0;
    mud->show_dialog_bank = 0;
    mud->is_sleeping = 0;
    mud->friend_list_count = 0;
}

void mudclient_login(mudclient *mud, char *username, char *password,
                     int reconnecting) {
    if (mud->world_full_timeout > 0) {
        mudclient_show_login_screen_status(mud, "Please wait...",
                                           "Connecting to server");

        delay_ticks(2000);

        mudclient_show_login_screen_status(
            mud, "Sorry! the server is currently full.",
            "Please try again later");

        return;
    }

    if (strlen(username) == 0 || strlen(password) == 0) {
        mudclient_show_login_screen_status(mud,
                                           "You must enter both a username",
                                           "and a password - Please try again");
        return;
    }

    if (mud->username != username) {
        strcpy(mud->username, username);
    }

    if (mud->password != password) {
        strcpy(mud->password, password);
    }

    char formatted_username[USERNAME_LENGTH + 1] = {0};
    format_auth_string(username, USERNAME_LENGTH, formatted_username);

    char formatted_password[PASSWORD_LENGTH + 1] = {0};
    format_auth_string(password, PASSWORD_LENGTH, formatted_password);

    if (reconnecting) {
#ifdef RENDER_3DS_GL
        mudclient_3ds_gl_frame_start(mud, 0);
#endif

        mudclient_draw_lost_connection(mud);
        surface_draw(mud->surface);

#ifdef RENDER_GL
#ifdef SDL12
        SDL_GL_SwapBuffers();
#else
        SDL_GL_SwapWindow(mud->gl_window);
#endif
#elif defined(RENDER_3DS_GL)
        mudclient_3ds_gl_frame_end();
#endif
    } else {
        mudclient_show_login_screen_status(mud, "Please wait...",
                                           "Connecting to server");
    }

    free(mud->packet_stream);
    mud->packet_stream = malloc(sizeof(PacketStream));
    packet_stream_new(mud->packet_stream, mud);

    if (mud->packet_stream->closed) {
        goto login_fail;
    }

#ifdef REVISION_177
    int session_id = packet_stream_get_int(mud->packet_stream);
    mud->session_id = session_id;
#else
    packet_stream_new_packet(mud->packet_stream, CLIENT_SESSION);

    int64_t encoded_username = encode_username(formatted_username);

    packet_stream_put_byte(mud->packet_stream,
                           (int)((encoded_username >> 16) & 31));

    if (packet_stream_flush_packet(mud->packet_stream) < 0) {
        goto login_fail;
    }

    int64_t session_id = packet_stream_get_long(mud->packet_stream);
    mud->session_id = session_id;
#endif

    if (mud->session_id == 0) {
        mudclient_show_login_screen_status(mud, "Login server offline.",
                                           "Please try again in a few mins");
        return;
    }

#ifdef REVISION_177
    mud_log("Session id: %d\n", session_id);

    packet_stream_new_packet(mud->packet_stream,
                             reconnecting ? CLIENT_RECONNECT : CLIENT_LOGIN);

    packet_stream_put_short(mud->packet_stream, VERSION);

    /* limit30 */
    packet_stream_put_short(mud->packet_stream, 0);

    packet_stream_put_long(mud->packet_stream,
                           encode_username(formatted_username));

    packet_stream_put_password(mud->packet_stream, session_id,
                               formatted_password);

    /* uid/randomDat */
    packet_stream_put_int(mud->packet_stream, 0);

    if (packet_stream_flush_packet(mud->packet_stream) < 0) {
        goto login_fail;
    }

    packet_stream_get_byte(mud->packet_stream);

    int response = packet_stream_get_byte(mud->packet_stream);
#else
#ifdef _3DS
    mud_log("Verb: Session id: %lld\n", session_id); /* ? */
#else
    mud_log("Verb: Session id: %ld\n", session_id);
#endif

    uint32_t keys[4] = {0};
    keys[0] = (int)(((float)rand() / (float)RAND_MAX) * (float)99999999);
    keys[1] = (int)(((float)rand() / (float)RAND_MAX) * (float)99999999);
    keys[2] = (int32_t)(session_id >> 32);
    keys[3] = (int32_t)(session_id);

    packet_stream_new_packet(mud->packet_stream, CLIENT_LOGIN);
    packet_stream_put_byte(mud->packet_stream, reconnecting);
    packet_stream_put_short(mud->packet_stream, VERSION);
    packet_stream_put_byte(mud->packet_stream, 0); /* limit30 */

    packet_stream_put_login_block(mud->packet_stream, formatted_username,
                                  formatted_password, keys, 0);

    if (packet_stream_flush_packet(mud->packet_stream) < 0) {
        goto login_fail;
    }

    int response = packet_stream_get_byte(mud->packet_stream);
#endif

    mud_log("Login response: %d\n", response);

    if (response == 0 || response == 1 || response == 25) {
        mud->moderator_level = response == 25;
        mud->auto_login_attempts = 0;

        strcpy(mud->options->username,
               mud->options->remember_username ? username : "");

        strcpy(mud->options->password,
               mud->options->remember_password ? password : "");

        if (mud->options->remember_username ||
            mud->options->remember_password) {
            options_save(mud->options);
        }

        mudclient_reset_game(mud);
        return;
    }

    /*if (response == 1) {
        mud->auto_login_attempts = 0;
        return;
    }*/

    if (reconnecting) {
        mudclient_reset_login_screen(mud);
        return;
    }

    // TODO enums
    switch (response) {
    case -1:
        mudclient_show_login_screen_status(mud, "Error unable to login.",
                                           "Server timed out");
        return;
    case 3:
        mudclient_show_login_screen_status(
            mud, "Invalid username or password.",
            "Try again, or create a new account");
        return;
    case 4:
        mudclient_show_login_screen_status(
            mud, "That username is already logged in.",
            "Wait 60 seconds then retry");
        return;
    case 5:
        mudclient_show_login_screen_status(mud, "The client has been updated.",
                                           "Please reload this page");
        return;
    case 6:
        mudclient_show_login_screen_status(
            mud, "You may only use 1 character at once.",
            "Your ip-address is already in use");
        return;
    case 7:
        mudclient_show_login_screen_status(mud, "Login attempts exceeded!",
                                           "Please try again in 5 minutes");
        return;
    case 8:
        mudclient_show_login_screen_status(mud, "Error unable to login.",
                                           "Server rejected session");
        return;
    case 9:
        mudclient_show_login_screen_status(mud, "Error unable to login.",
                                           "Loginserver rejected session");
        return;
    case 10:
        mudclient_show_login_screen_status(mud,
                                           "That username is already in use.",
                                           "Wait 60 seconds then retry");
        return;
    case 11:
        mudclient_show_login_screen_status(
            mud, "Account temporarily disabled.",
            "Check your message inbox for details");
        return;
    case 12:
        mudclient_show_login_screen_status(
            mud, "Account permanently disabled.",
            "Check your message inbox for details");
        return;
    case 14:
        mudclient_show_login_screen_status(
            mud, "Sorry! This world is currently full.",
            "Please try a different world");

        mud->world_full_timeout = 1500;
        return;
    case 15:
        mudclient_show_login_screen_status(mud, "You need a members account",
                                           "to login to this world");
        return;
    case 16:
        mudclient_show_login_screen_status(
            mud, "Error - no reply from loginserver.", "Please try again");
        return;
    case 17:
        mudclient_show_login_screen_status(mud,
                                           "Error - failed to decode profile.",
                                           "Contact customer support");
        return;
    case 18:
        mudclient_show_login_screen_status(
            mud, "Account suspected stolen.",
            "Press \"recover a locked account\" on front page.");
        return;
    case 20:
        mudclient_show_login_screen_status(mud, "Error - loginserver mismatch",
                                           "Please try a different world");
        return;
    case 21:
        mudclient_show_login_screen_status(mud, "Unable to login.",
                                           "That is not an RS-Classic account");
        return;
    case 22:
        mudclient_show_login_screen_status(
            mud, "Password suspected stolen.",
            "Press \"change your password\" on front page.");
        return;
    default:
        mudclient_show_login_screen_status(mud, "Error unable to login.",
                                           "Unrecognised response code");
        return;
    }

login_fail:
    if (mud->auto_login_attempts > 0) {
        int delay = 0;

        while (delay < 5000) {
            mudclient_poll_events(mud);
            delay += 16;
            delay_ticks(16);
        }

        mud->auto_login_attempts--;
        mudclient_login(mud, username, password, reconnecting);
        return;
    }

    if (reconnecting) {
        mudclient_reset_login_screen(mud);
        mud->login_screen = LOGIN_STAGE_EXISTING;
    }

    mudclient_show_login_screen_status(
        mud, "Sorry! Unable to connect.",
        "Check internet settings or try another world");
}

void mudclient_registration_login(mudclient *mud) {
    char *username =
        panel_get_text(mud->panel_login_new_user, mud->control_register_user);

    char *password = panel_get_text(mud->panel_login_new_user,
                                    mud->control_register_password);

    mud->login_screen = 2;

    panel_update_text(mud->panel_login_existing_user, mud->control_login_status,
                      "Please enter your username and password");

    panel_update_text(mud->panel_login_existing_user,
                      mud->control_login_username, username);

    panel_update_text(mud->panel_login_existing_user,
                      mud->control_login_password, password);

    mudclient_draw_login_screens(mud);
    mudclient_reset_timings(mud);
    mudclient_login(mud, username, password, 0);
}

void mudclient_register(mudclient *mud, char *username, char *password) {
    if (mud->world_full_timeout > 0) {
        mudclient_show_login_screen_status(mud, "Please wait...",
                                           "Connecting to server");

        delay_ticks(2000);

        mudclient_show_login_screen_status(
            mud, "Sorry! The server is currently full.",
            "Please try again later");

        return;
    }

    char formatted_username[USERNAME_LENGTH + 1] = {0};
    format_auth_string(username, USERNAME_LENGTH, formatted_username);

    char formatted_password[PASSWORD_LENGTH + 1] = {0};
    format_auth_string(password, PASSWORD_LENGTH, formatted_password);

    mudclient_show_login_screen_status(mud, "Please wait...",
                                       "Connecting to server");

    free(mud->packet_stream);
    mud->packet_stream = malloc(sizeof(PacketStream));
    packet_stream_new(mud->packet_stream, mud);

    if (mud->packet_stream->closed) {
        goto register_fail;
    }

#ifdef REVISION_177
    int session_id = packet_stream_get_int(mud->packet_stream);
    mud->session_id = session_id;
#else
    packet_stream_new_packet(mud->packet_stream, CLIENT_SESSION);

    int64_t encoded_username = encode_username(formatted_username);

    packet_stream_put_byte(mud->packet_stream,
                           (int)((encoded_username >> 16) & 31));

    if (packet_stream_flush_packet(mud->packet_stream) < 0) {
        goto register_fail;
    }

    int64_t session_id = packet_stream_get_long(mud->packet_stream);
    mud->session_id = session_id;
#endif

    if (mud->session_id == 0) {
        mudclient_show_login_screen_status(mud, "Login server offline.",
                                           "Please try again in a few mins");
        return;
    }

#ifdef REVISION_177
    mud_log("Session id: %d\n", session_id);

    packet_stream_new_packet(mud->packet_stream, CLIENT_REGISTER);
    packet_stream_put_short(mud->packet_stream, VERSION);

    packet_stream_put_long(mud->packet_stream,
                           encode_username(formatted_username));

    /* refer id */
    packet_stream_put_short(mud->packet_stream, 0);

    packet_stream_put_password(mud->packet_stream, session_id,
                               formatted_password);

    /* uid/randomDat */
    packet_stream_put_int(mud->packet_stream, 0);

    if (packet_stream_flush_packet(mud->packet_stream) < 0) {
        goto register_fail;
    }

    packet_stream_get_byte(mud->packet_stream);
#else
    mud_log("Verb: Session id: %ld\n", session_id);

    uint32_t keys[4] = {0};
    keys[0] = (int)(((float)rand() / (float)RAND_MAX) * (float)99999999);
    keys[1] = (int)(((float)rand() / (float)RAND_MAX) * (float)99999999);
    keys[2] = (int32_t)(session_id >> 32);
    keys[3] = (int32_t)(session_id);

    packet_stream_new_packet(mud->packet_stream, CLIENT_REGISTER);
    packet_stream_put_byte(mud->packet_stream, 0);
    packet_stream_put_short(mud->packet_stream, VERSION);
    packet_stream_put_byte(mud->packet_stream, 0); /* limit30 */

    packet_stream_put_login_block(mud->packet_stream, formatted_username,
                                  formatted_password, keys, 0);

    if (packet_stream_flush_packet(mud->packet_stream) < 0) {
        goto register_fail;
    }
#endif

    int response = packet_stream_get_byte(mud->packet_stream);
    mud_log("Newplayer response: %d\n", response);

    switch (response) {
    case 2:
        mudclient_registration_login(mud);
        return;
    case 13:
    case 3:
        mudclient_show_login_screen_status(mud, "Username already taken.",
                                           "Please choose another username");
        return;
    case 4:
        mudclient_show_login_screen_status(mud,
                                           "That username is already in use.",
                                           "Wait 60 seconds then retry");
        return;
    case 5:
        mudclient_show_login_screen_status(mud, "The client has been updated.",
                                           "Please reload this page");
        return;
    case 6:
        mudclient_show_login_screen_status(
            mud, "You may only use 1 character at once.",
            "Your ip-address is already in use");
        return;
    case 7:
        mudclient_show_login_screen_status(mud, "Login attempts exceeded!",
                                           "Please try again in 5 minutes");
        return;
    case 11:
        mudclient_show_login_screen_status(
            mud, "Account has been temporarily disabled",
            "for cheating or abuse");
        return;
    case 12:
        mudclient_show_login_screen_status(
            mud, "Account has been permanently disabled",
            "for cheating or abuse");
        /* ^ this would be "Check your message inbox for details." */
        return;
    case 14:
        mudclient_show_login_screen_status(
            mud, "Sorry! The server is currently full.",
            "Please try again later");

        mud->world_full_timeout = 1500;
        return;
    case 15:
        mudclient_show_login_screen_status(mud, "You need a members account",
                                           "to login to this server");
        return;
    case 16:
        mudclient_show_login_screen_status(mud,
                                           "Please login to a members server",
                                           "to access member-only features");
        return;
    default:
        mudclient_show_login_screen_status(mud,
                                           "Error unable to create username.",
                                           "Unrecognised response code");
        return;
    }

register_fail:
    mudclient_show_login_screen_status(
        mud, "Sorry! Unable to connect.",
        "Check internet settings or try another world");
}

void mudclient_change_password(mudclient *mud, char *old_password,
                               char *new_password) {
    char formatted_old_password[PASSWORD_LENGTH + 1] = {0};
    format_auth_string(old_password, 20, formatted_old_password);

    char formatted_new_password[PASSWORD_LENGTH + 1] = {0};
    format_auth_string(new_password, 20, formatted_new_password);

    char passwords[(PASSWORD_LENGTH * 2) + 1] = {0};
    sprintf(passwords, "%s%s", formatted_old_password, formatted_new_password);

    packet_stream_new_packet(mud->packet_stream, CLIENT_CHANGE_PASSWORD);

#ifdef REVISION_177
    packet_stream_put_password(mud->packet_stream, mud->session_id, passwords);
#endif

    packet_stream_flush_packet(mud->packet_stream);
}

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
void mudclient_update_fov(mudclient *mud) {
    if (mud->options->field_of_view) {
        mud->scene->gl_fov = glm_rad(mud->options->field_of_view / 10.0f);

        int view_distance =
            round((-254.452344 * pow(mud->scene->gl_fov, 3)) +
                  (1142.234460 * pow(mud->scene->gl_fov, 2)) -
                  (1901.194134 * mud->scene->gl_fov) + 1318.230265);

        mud->scene->view_distance =
            round((float)mud->scene->gl_height *
                  ((float)view_distance / (float)(346 - 12)));
    } else {
        float scaled_scene_height =
            (float)(mud->scene->gl_height - 1) / 1000.0f;

        /* no idea, i just used cubic regression */
        mud->scene->gl_fov = (-0.1608132078 * powf(scaled_scene_height, 3)) -
                             (0.3012063997 * powf(scaled_scene_height, 2)) +
                             (2.0149949882 * scaled_scene_height) -
                             0.0030409762;

        mud->scene->view_distance = 512;
    }
}
#endif

void mudclient_start_game(mudclient *mud) {
    mudclient_load_game_config(mud);

    if (mud->error_loading_data) {
        return;
    }

    mudclient_set_target_fps(mud, 50);

    panel_base_sprite_start = mud->sprite_util;

    int x = MUD_WIDTH - 199;
    int y = UI_BUTTON_SIZE + 1;

    int is_touch = mudclient_is_touch(mud);

    mud->panel_quests = malloc(sizeof(Panel));
    panel_new(mud->panel_quests, mud->surface, 5);

    if (is_touch) {
        x = UI_TABS_TOUCH_X - STATS_WIDTH - 1;

        y = (UI_TABS_TOUCH_Y + UI_TABS_TOUCH_HEIGHT) - STATS_COMPACT_HEIGHT -
            STATS_TAB_HEIGHT - 5;
    }

    mud->control_list_quest = panel_add_text_list_interactive(
        mud->panel_quests, x, y + STATS_TAB_HEIGHT, STATS_WIDTH,
        STATS_HEIGHT - STATS_TAB_HEIGHT, FONT_BOLD_12, 500, 1);

    mud->panel_magic = malloc(sizeof(Panel));
    panel_new(mud->panel_magic, mud->surface, 5);

    if (is_touch) {
        x = UI_TABS_TOUCH_X - MAGIC_WIDTH - 1;
        y = UI_TABS_TOUCH_Y + 10;
    }

    mud->control_list_magic = panel_add_text_list_interactive(
        mud->panel_magic, x, y + MAGIC_TAB_HEIGHT - (is_touch ? 11 : 0),
        MAGIC_WIDTH, 90 + (is_touch ? 16 : 0), FONT_BOLD_12, 500, 1);

    mud->panel_social_list = malloc(sizeof(Panel));
    panel_new(mud->panel_social_list, mud->surface, 5);

    mud->control_list_social = panel_add_text_list_interactive(
        mud->panel_social_list, x,
        y + SOCIAL_TAB_HEIGHT + 16 - (is_touch ? 11 : 0), 196,
        126 + (is_touch ? 16 : 0), FONT_BOLD_12, 500, 1);

    mudclient_load_media(mud);

    if (mud->error_loading_data) {
        return;
    }

    mudclient_load_entities(mud);

    if (mud->error_loading_data) {
        return;
    }

    mud->scene = malloc(sizeof(Scene));
    if (mud->options->lowmem) {
        scene_new(mud->scene, mud->surface, 7500, 7500, 1000);
    } else {
        scene_new(mud->scene, mud->surface, 15000, 15000, 1000);
    }

#ifdef RENDER_3DS_GL
    scene_set_bounds(mud->scene, mud->game_width, mud->game_height);
#else
    scene_set_bounds(mud->scene, mud->game_width, mud->game_height - 12);
#endif

    mud->scene->clip_far_3d = 2400;
    mud->scene->clip_far_2d = 2400;
    mud->scene->fog_z_distance = 2300;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    mudclient_update_fov(mud);
#endif

    mud->world = malloc(sizeof(World));
    world_new(mud->world, mud->scene, mud->surface);

    /* used for storing minimap sprite */
    mud->world->base_media_sprite = mud->sprite_media;

    mud->world->thick_walls = mud->options->thick_walls;

    mudclient_load_textures(mud);

    if (mud->error_loading_data) {
        return;
    }

    mudclient_load_models(mud);

    if (mud->error_loading_data) {
        return;
    }

    mudclient_load_maps(mud);

    if (mud->error_loading_data) {
        return;
    }

    if (mud->options->members && !mud->options->lowmem) {
        mudclient_load_sounds(mud);
    }

    if (mud->error_loading_data) {
        return;
    }

    mudclient_draw_loading_progress(mud, 100, "Starting game...");
    mudclient_create_message_tabs_panel(mud);
    mudclient_create_login_panels(mud);
    mudclient_create_appearance_panel(mud);
    mudclient_create_options_panel(mud);
    mudclient_reset_login_screen(mud);

    if (!mud->options->lowmem) {
        mudclient_render_login_scene_sprites(mud);
    }

    free(surface_texture_pixels);
    surface_texture_pixels = NULL;
}

GameModel *mudclient_create_wall_object(mudclient *mud, int x, int y,
                                        int direction, int id, int count) {
    int x1 = x;
    int y1 = y;
    int x2 = x;
    int y2 = y;

    int front_texture = game_data.wall_objects[id].texture_front;
    int back_texture = game_data.wall_objects[id].texture_back;
    int height = game_data.wall_objects[id].height;

    GameModel *game_model = malloc(sizeof(GameModel));
    game_model_new_alloc(game_model, 4, 1);

    if (direction == 0) {
        x2 = x + 1;
    } else if (direction == 1) {
        y2 = y + 1;
    } else if (direction == 2) {
        x1 = x + 1;
        y2 = y + 1;
    } else if (direction == 3) {
        x2 = x + 1;
        y2 = y + 1;
    }

    x1 *= MAGIC_LOC;
    y1 *= MAGIC_LOC;
    x2 *= MAGIC_LOC;
    y2 *= MAGIC_LOC;

    uint16_t *vertices = malloc(4 * sizeof(uint16_t));

    vertices[0] = game_model_vertex_at(
        game_model, x1, -world_get_elevation(mud->world, x1, y1), y1);

    vertices[1] = game_model_vertex_at(
        game_model, x1, -world_get_elevation(mud->world, x1, y1) - height, y1);

    vertices[2] = game_model_vertex_at(
        game_model, x2, -world_get_elevation(mud->world, x2, y2) - height, y2);

    vertices[3] = game_model_vertex_at(
        game_model, x2, -world_get_elevation(mud->world, x2, y2), y2);

    game_model_create_face(game_model, 4, vertices, front_texture,
                           back_texture);

    game_model_set_light(game_model, 0, 60, 24, -50, -10, -50);

    if (x >= 0 && y >= 0 && x < 96 && y < 96) {
        scene_add_model(mud->scene, game_model);
    }

    game_model->key = count + 10000;

    return game_model;
}

int mudclient_load_next_region(mudclient *mud, int lx, int ly) {
    if (mud->death_screen_timeout != 0) {
        mud->world->player_alive = 0;
        return 0;
    }

    mud->loading_area = 0;

    lx += mud->plane_width;
    ly += mud->plane_height;

    if (mud->last_plane_index == mud->plane_index && lx > mud->local_lower_x &&
        lx < mud->local_upper_x && ly > mud->local_lower_y &&
        ly < mud->local_upper_y) {
        mud->world->player_alive = 1;
        return 0;
    }

    surface_draw_string_centre(
        mud->surface, "Loading... Please wait", mud->surface->width / 2,
        mud->surface->height / 2 + 19, FONT_BOLD_12, WHITE);

    mudclient_draw_chat_message_tabs(mud);

#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_frame_start(mud, 0);
#endif

    surface_draw(mud->surface);

#ifdef RENDER_GL
#ifdef SDL12
    SDL_GL_SwapBuffers();
#else
    SDL_GL_SwapWindow(mud->gl_window);
#endif
#elif defined(RENDER_3DS_GL)
    mudclient_3ds_gl_frame_end();
#endif

    int ax = mud->region_x;
    int ay = mud->region_y;
    int section_x = (lx + (REGION_SIZE / 2)) / REGION_SIZE;
    int section_y = (ly + (REGION_SIZE / 2)) / REGION_SIZE;

    mud->last_plane_index = mud->plane_index;
    mud->region_x = section_x * REGION_SIZE - REGION_SIZE;
    mud->region_y = section_y * REGION_SIZE - REGION_SIZE;
    mud->local_lower_x = section_x * REGION_SIZE - 32;
    mud->local_lower_y = section_y * REGION_SIZE - 32;
    mud->local_upper_x = section_x * REGION_SIZE + 32;
    mud->local_upper_y = section_y * REGION_SIZE + 32;

    world_load_section(mud->world, lx, ly, mud->last_plane_index);

    mud->region_x -= mud->plane_width;
    mud->region_y -= mud->plane_height;

    int offset_x = mud->region_x - ax;
    int offset_y = mud->region_y - ay;

    for (int i = 0; i < mud->object_count; i++) {
        mud->objects[i].x -= offset_x;
        mud->objects[i].y -= offset_y;

        int object_x = mud->objects[i].x;
        int object_y = mud->objects[i].y;
        int object_id = mud->objects[i].id;

        GameModel *game_model = mud->objects[i].model;

        int object_direction = mud->objects[i].direction;
        int object_width = 0;
        int object_height = 0;

        if (object_direction == 0 || object_direction == 4) {
            object_width = game_data.objects[object_id].width;
            object_height = game_data.objects[object_id].height;
        } else {
            object_height = game_data.objects[object_id].width;
            object_width = game_data.objects[object_id].height;
        }

        int base_x = ((object_x + object_x + object_width) * MAGIC_LOC) / 2;
        int base_y = ((object_y + object_y + object_height) * MAGIC_LOC) / 2;

        if (object_x >= 0 && object_y >= 0 && object_x < 96 && object_y < 96) {
            scene_add_model(mud->scene, game_model);

            game_model_place(game_model, base_x,
                             -world_get_elevation(mud->world, base_x, base_y),
                             base_y);

            world_register_object(mud->world, object_x, object_y, object_id);

            if (object_id == WINDMILL_SAILS_ID) {
                game_model_translate(game_model, 0, -480, 0);
            }
        }
    }

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    world_gl_buffer_world_models(mud->world);
#endif

    for (int i = 0; i < mud->wall_object_count; i++) {
        mud->wall_objects[i].x -= offset_x;
        mud->wall_objects[i].y -= offset_y;

        int wall_object_x = mud->wall_objects[i].x;
        int wall_object_y = mud->wall_objects[i].y;
        int wall_object_id = mud->wall_objects[i].id;
        int wall_object_dir = mud->wall_objects[i].direction;

        world_register_wall_object(mud->world, wall_object_x, wall_object_y,
                                   wall_object_dir, wall_object_id);

        game_model_destroy(mud->wall_objects[i].model);
        free(mud->wall_objects[i].model);

        GameModel *wall_object_model =
            mudclient_create_wall_object(mud, wall_object_x, wall_object_y,
                                         wall_object_dir, wall_object_id, i);

        mud->wall_objects[i].model = wall_object_model;
    }

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    mudclient_gl_update_wall_models(mud);
#endif

    for (int i = 0; i < mud->ground_item_count; i++) {
        mud->ground_items[i].x -= offset_x;
        mud->ground_items[i].y -= offset_y;
    }

    mudclient_update_ground_item_models(mud);

    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        player->current_x -= offset_x * MAGIC_LOC;
        player->current_y -= offset_y * MAGIC_LOC;

        for (int j = 0; j <= player->waypoint_current; j++) {
            player->waypoints_x[j] -= offset_x * MAGIC_LOC;
            player->waypoints_y[j] -= offset_y * MAGIC_LOC;
        }
    }

    for (int i = 0; i < mud->npc_count; i++) {
        GameCharacter *npc = mud->npcs[i];

        npc->current_x -= offset_x * MAGIC_LOC;
        npc->current_y -= offset_y * MAGIC_LOC;

        for (int j = 0; j <= npc->waypoint_current; j++) {
            npc->waypoints_x[j] -= offset_x * MAGIC_LOC;
            npc->waypoints_y[j] -= offset_y * MAGIC_LOC;
        }
    }

    mud->world->player_alive = 1;

    return 1;
}

GameCharacter *mudclient_add_character(mudclient *mud,
                                       GameCharacter **character_server,
                                       GameCharacter **known_characters,
                                       int known_character_count,
                                       int server_index, int x, int y,
                                       int animation, int npc_id) {
    if (character_server[server_index] == NULL) {
        if (npc_id == -1 && server_index == mud->local_player_server_index) {
            /* unlikely but just in case */
            for (int i = 0; i < PLAYERS_SERVER_MAX; i++) {
                if (mud->player_server[i] == mud->local_player) {
                    mud->player_server[i] = NULL;
                    break;
                }
            }

            for (int i = 0; i < PLAYERS_MAX; i++) {
                if (mud->players[i] == mud->local_player) {
                    mud->players[i] = NULL;
                    break;
                }
            }

            free(mud->local_player);
            mud->local_player = NULL;
        }

        GameCharacter *character = malloc(sizeof(GameCharacter));

        if (character == NULL) {
            return NULL;
        }

        game_character_new(character);

        character_server[server_index] = character;
        character_server[server_index]->server_index = server_index;
    }

    GameCharacter *character = character_server[server_index];
    int exists = 0;

    for (int i = 0; i < known_character_count; i++) {
        if (known_characters[i]->server_index != server_index) {
            continue;
        }

        exists = 1;
        break;
    }

    if (npc_id > -1) {
        character->npc_id = npc_id;
    }

    if (exists) {
        character->next_animation = animation;
        int waypoint_index = character->waypoint_current;

        if (x != character->waypoints_x[waypoint_index] ||
            y != character->waypoints_y[waypoint_index]) {
            waypoint_index = (waypoint_index + 1) % 10;
            character->waypoint_current = waypoint_index;
            character->waypoints_x[waypoint_index] = x;
            character->waypoints_y[waypoint_index] = y;
        }
    } else {
        character->server_index = server_index;
        character->moving_step = 0;
        character->waypoint_current = 0;
        character->current_x = x;
        character->current_y = y;
        character->waypoints_x[0] = x;
        character->waypoints_y[0] = y;
        character->current_animation = animation;
        character->next_animation = animation;
        character->step_count = 0;
    }

    return character;
}

GameCharacter *mudclient_add_player(mudclient *mud, int server_index, int x,
                                    int y, int animation) {
    if (server_index >= PLAYERS_SERVER_MAX ||
        mud->player_count >= PLAYERS_MAX) {
        return NULL;
    }

    GameCharacter *player = mudclient_add_character(
        mud, mud->player_server, mud->known_players, mud->known_player_count,
        server_index, x, y, animation, -1);

    if (player == NULL) {
        return NULL;
    }

    mud->players[mud->player_count++] = player;

    return player;
}

GameCharacter *mudclient_add_npc(mudclient *mud, int server_index, int x, int y,
                                 int animation, int npc_id) {
    if (server_index >= NPCS_SERVER_MAX || mud->npc_count >= NPCS_MAX) {
        return NULL;
    }

#ifdef RENDER_SW
    if (mud->options->diversify_npcs) {
        npc_id = diversify_npc(npc_id, server_index, x, y);
    }
#endif

    GameCharacter *npc = mudclient_add_character(
        mud, mud->npcs_server, mud->known_npcs, mud->known_npc_count,
        server_index, x, y, animation, npc_id);

    if (npc == NULL) {
        return NULL;
    }

    mud->npcs[mud->npc_count++] = npc;

    return npc;
}

void mudclient_update_bank_items(mudclient *mud) {
    mud->bank_item_count = mud->new_bank_item_count;

    for (int i = 0; i < mud->new_bank_item_count; i++) {
        mud->bank_items[i] = mud->new_bank_items[i];
        mud->bank_items_count[i] = mud->new_bank_items_count[i];
    }

    for (int i = 0; i < mud->inventory_items_count; i++) {
        if (mud->bank_item_count >= mud->bank_items_max) {
            break;
        }

        int inventory_id = mud->inventory_item_id[i];
        int has_item_in_bank = 0;

        for (int j = 0; j < mud->bank_item_count; j++) {
            if (mud->bank_items[j] == inventory_id) {
                has_item_in_bank = 1;
                break;
            }
        }

        if (!has_item_in_bank) {
            mud->bank_items[mud->bank_item_count] = inventory_id;
            mud->bank_items_count[mud->bank_item_count] = 0;
            mud->bank_item_count++;
        }
    }
}

void mudclient_close_connection(mudclient *mud) {
    if (mud->packet_stream != NULL) {
        packet_stream_new_packet(mud->packet_stream, CLIENT_CLOSE_CONNECTION);
        packet_stream_flush_packet(mud->packet_stream);
    }

    memset(mud->username, '\0', USERNAME_LENGTH + 1);
    memset(mud->password, '\0', PASSWORD_LENGTH + 1);

    mudclient_reset_login_screen(mud);
}

void mudclient_lost_connection(mudclient *mud) {
#ifndef REVISION_177
    mud->system_update = 0;
#endif

    if (mud->logout_timeout != 0) {
        mudclient_reset_login_screen(mud);
    } else {
        mud->auto_login_attempts = 10;
        mudclient_login(mud, mud->username, mud->password, 1);
    }
}

int mudclient_is_valid_camera_angle(mudclient *mud, int angle) {
    int x = mud->local_player->current_x / 128;
    int y = mud->local_player->current_y / 128;

    for (int i = 2; i >= 1; i--) {
        if (angle == 1 &&
            ((mud->world->object_adjacency[x][y - i] & 128) == 128 ||
             (mud->world->object_adjacency[x - i][y] & 128) == 128 ||
             (mud->world->object_adjacency[x - i][y - i] & 128) == 128)) {
            return 0;
        }

        if (angle == 3 &&
            ((mud->world->object_adjacency[x][y + i] & 128) == 128 ||
             (mud->world->object_adjacency[x - i][y] & 128) == 128 ||
             (mud->world->object_adjacency[x - i][y + i] & 128) == 128)) {
            return 0;
        }

        if (angle == 5 &&
            ((mud->world->object_adjacency[x][y + i] & 128) == 128 ||
             (mud->world->object_adjacency[x + i][y] & 128) == 128 ||
             (mud->world->object_adjacency[x + i][y + i] & 128) == 128)) {
            return 0;
        }

        if (angle == 7 &&
            ((mud->world->object_adjacency[x][y - i] & 128) == 128 ||
             (mud->world->object_adjacency[x + i][y] & 128) == 128 ||
             (mud->world->object_adjacency[x + i][y - i] & 128) == 128)) {
            return 0;
        }

        if (angle == 0 &&
            (mud->world->object_adjacency[x][y - i] & 128) == 128) {
            return 0;
        }

        if (angle == 2 &&
            (mud->world->object_adjacency[x - i][y] & 128) == 128) {
            return 0;
        }

        if (angle == 4 &&
            (mud->world->object_adjacency[x][y + i] & 128) == 128) {
            return 0;
        }

        if (angle == 6 &&
            (mud->world->object_adjacency[x + i][y] & 128) == 128) {
            return 0;
        }
    }

    return 1;
}

void mudclient_auto_rotate_camera(mudclient *mud) {
    if ((mud->camera_angle & 1) == 1 &&
        mudclient_is_valid_camera_angle(mud, mud->camera_angle)) {
        return;
    }

    if ((mud->camera_angle & 1) == 0 &&
        mudclient_is_valid_camera_angle(mud, mud->camera_angle)) {
        if (mudclient_is_valid_camera_angle(mud, (mud->camera_angle + 1) & 7)) {
            mud->camera_angle = (mud->camera_angle + 1) & 7;
            return;
        }

        if (mudclient_is_valid_camera_angle(mud, (mud->camera_angle + 7) & 7)) {
            mud->camera_angle = (mud->camera_angle + 7) & 7;
        }

        return;
    }

    int angles[] = {1, -1, 2, -2, 3, -3, 4};

    for (int i = 0; i < 7; i++) {
        int angle = (mud->camera_angle + angles[i] + 8) & 7;

        if (!mudclient_is_valid_camera_angle(mud, angle)) {
            continue;
        }

        mud->camera_angle = angle;
        break;
    }

    if ((mud->camera_angle & 1) == 0 &&
        mudclient_is_valid_camera_angle(mud, mud->camera_angle)) {
        if (mudclient_is_valid_camera_angle(mud, (mud->camera_angle + 1) & 7)) {
            mud->camera_angle = (mud->camera_angle + 1) & 7;
            return;
        }

        if (mudclient_is_valid_camera_angle(mud, (mud->camera_angle + 7) & 7)) {
            mud->camera_angle = (mud->camera_angle + 7) & 7;
        }
    }
}

void mudclient_handle_camera_zoom(mudclient *mud) {
    if (mud->key_up) {
        mud->camera_zoom -= 16;
    } else if (mud->key_down) {
        mud->camera_zoom += 16;
    } else if (mud->key_page_up) {
        mud->camera_zoom = ZOOM_MIN;
    } else if (mud->key_page_down) {
        mud->camera_zoom = ZOOM_MAX;
    } else if (mud->key_home) {
        mud->camera_zoom = ZOOM_OUTDOORS;
    }

    int is_touch = mudclient_is_touch(mud);

    int exclude_max_x = MUD_VANILLA_WIDTH;

    int exclude_min_y = is_touch ? 0 : mud->surface->height - 80;
    int exclude_max_y = is_touch ? 100 : mud->surface->height;

    if (mud->mouse_scroll_delta != 0 &&
        (mud->show_ui_tab == 0 || mud->show_ui_tab == MAP_TAB) &&
        !(mud->message_tab_selected != MESSAGE_TAB_ALL &&
          mud->mouse_y > exclude_min_y && mud->mouse_y <= exclude_max_y &&
          mud->mouse_x <= exclude_max_x) &&
        !mud->show_dialog_bank) {
        mud->camera_zoom += mud->mouse_scroll_delta * 24;
    }

    if (mud->camera_zoom > ZOOM_MAX) {
        mud->camera_zoom = ZOOM_MAX;
    } else if (mud->camera_zoom < ZOOM_MIN) {
        mud->camera_zoom = ZOOM_MIN;
    }
}

void mudclient_handle_game_input(mudclient *mud) {
#ifndef REVISION_177
    if (mud->system_update > 1) {
        mud->system_update--;
    }
#endif

    if (mud->show_dialog_confirm) {
        mudclient_handle_confirm_input(mud);
    } else if (mud->show_additional_options) {
        mudclient_handle_additional_options_input(mud);
    }

    if (mud->options->tab_respond && mud->key_tab &&
        mud->private_message_target != 0) {
        int is_online = 0;

        for (int i = 0; i < mud->friend_list_count; i++) {
            if (mud->friend_list[i] == mud->private_message_target &&
                mud->friend_list_online[i] > 0) {
                is_online = 1;
                break;
            }
        }

        if (is_online) {
            mud->show_dialog_social_input = SOCIAL_MESSAGE_FRIEND;

            memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
            memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
        }

        mud->key_tab = 0;
    }

    if (mud->options->middle_click_camera != 0 && mud->middle_button_down) {
        int ticks = get_ticks();

        if (ticks - mud->last_mouse_sample_ticks >= 250) {
            mud->last_mouse_sample_ticks = ticks;

            int mouse_x = mud->mouse_x;

#ifdef RENDER_GL
            mouse_x = mud->gl_mouse_x;
#endif

            mud->last_mouse_sample_x = mouse_x;
        }
    }

    mudclient_packet_tick(mud);

    if (mud->logout_timeout > 0) {
        mud->logout_timeout--;
    }

    if (mud->options->idle_logout && mud->mouse_action_timeout > 4500 &&
        mud->combat_timeout == 0 && mud->logout_timeout == 0) {
        mud->mouse_action_timeout -= 500;
        mudclient_send_logout(mud);
        return;
    }

    if (mud->local_player->current_animation == 8 ||
        mud->local_player->current_animation == 9) {
        mud->combat_timeout = 500;
    }

    if (mud->combat_timeout > 0) {
        mud->combat_timeout--;
    }

    if (mud->show_appearance_change) {
        mudclient_handle_appearance_panel_input(mud);
        return;
    }

    for (int i = 0; i < mud->player_count; i++) {
        game_character_move(mud->players[i]);
    }

    if (mud->death_screen_timeout > 0) {
        mud->death_screen_timeout--;

        if (mud->death_screen_timeout == 0) {
            mudclient_show_message(mud,
                                   "You have been granted another life. Be "
                                   "more careful this time!",
                                   MESSAGE_TYPE_GAME);

            mudclient_show_message(
                mud, "You retain your skills. Your objects land where you died",
                MESSAGE_TYPE_GAME);
        }
    }

    for (int i = 0; i < mud->npc_count; i++) {
        game_character_move(mud->npcs[i]);
    }

    if (mud->show_ui_tab != MAP_TAB) {
        if (an_int_346 > 0) {
            mud->sleep_word_delay_timer++;
        }

        if (an_int_347 > 0) {
            mud->sleep_word_delay_timer = 0;
        }

        an_int_346 = 0;
        an_int_347 = 0;
    }

    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        if (player->projectile_range > 0) {
            player->projectile_range--;
        }
    }

    if (mud->camera_auto_rotate_player_x - mud->local_player->current_x <
            -500 ||
        mud->camera_auto_rotate_player_x - mud->local_player->current_x > 500 ||
        mud->camera_auto_rotate_player_y - mud->local_player->current_y <
            -500 ||
        mud->camera_auto_rotate_player_y - mud->local_player->current_y > 500) {
        mud->camera_auto_rotate_player_x = mud->local_player->current_x;
        mud->camera_auto_rotate_player_y = mud->local_player->current_y;
    }

    if (mud->camera_auto_rotate_player_x != mud->local_player->current_x) {
        mud->camera_auto_rotate_player_x +=
            (mud->local_player->current_x - mud->camera_auto_rotate_player_x) /
            (16 + ((mud->camera_zoom - 500) / 15));
    }

    if (mud->camera_auto_rotate_player_y != mud->local_player->current_y) {
        mud->camera_auto_rotate_player_y +=
            (mud->local_player->current_y - mud->camera_auto_rotate_player_y) /
            (16 + ((mud->camera_zoom - 500) / 15));
    }

    if (mud->settings_camera_auto) {
        int k1 = mud->camera_angle * 32;
        int j3 = k1 - mud->camera_rotation;
        int direction = 1;

        if (j3 != 0) {
            mud->camera_auto_counter++;

            if (j3 > 128) {
                direction = -1;
                j3 = 256 - j3;
            } else if (j3 > 0)
                direction = 1;
            else if (j3 < -128) {
                direction = 1;
                j3 = 256 + j3;
            } else if (j3 < 0) {
                direction = -1;
                j3 = -j3;
            }

            mud->camera_rotation +=
                ((mud->camera_auto_counter * j3 + 255) / 256) * direction;

            mud->camera_rotation &= 0xff;
        } else {
            mud->camera_auto_counter = 0;
        }
    } else if (mud->camera_momentum != 0) {
        int sign = mud->camera_momentum > 0 ? 1 : -1;

        mud->camera_rotation += abs(mud->camera_momentum) * sign;
        mud->camera_momentum -= 1 * sign;
    }

    if (mud->sleep_word_delay_timer > 20) {
        mud->sleep_word_delay = 0;
        mud->sleep_word_delay_timer = 0;
    }

    if (mud->is_sleeping) {
        mudclient_handle_sleep_input(mud);
        return;
    }

    mudclient_handle_message_tabs_input(mud);

    if (mud->death_screen_timeout != 0) {
        mud->last_mouse_button_down = 0;
    }

    if (mud->show_dialog_trade || mud->show_dialog_duel ||
        (mud->show_dialog_shop && mud->options->hold_to_buy)) {

        if (mud->mouse_button_down != 0) {
            mud->mouse_button_down_time++;
        } else {
            mud->mouse_button_down_time = 0;
        }

        if (mud->mouse_button_down_time > 600) {
            mud->mouse_item_count_increment += 5000;
        } else if (mud->mouse_button_down_time > 450) {
            mud->mouse_item_count_increment += 500;
        } else if (mud->mouse_button_down_time > 300) {
            mud->mouse_item_count_increment += 50;
        } else if (mud->mouse_button_down_time > 150) {
            mud->mouse_item_count_increment += 5;
        } else if (mud->mouse_button_down_time > 50) {
            mud->mouse_item_count_increment++;
        } else if (mud->mouse_button_down_time > 20 &&
                   (mud->mouse_button_down_time & 5) == 0) {
            mud->mouse_item_count_increment++;
        }
    } else {
        mud->mouse_button_down_time = 0;
        mud->mouse_item_count_increment = 0;
    }

    if (mud->last_mouse_button_down == 1) {
        mud->mouse_button_click = 1;
    } else if (mud->last_mouse_button_down == 2) {
        mud->mouse_button_click = 2;
    }

#ifdef RENDER_GL
    scene_set_mouse_location(mud->scene, mud->gl_mouse_x, mud->gl_mouse_y);
#else
    scene_set_mouse_location(mud->scene, mud->mouse_x, mud->mouse_y);
#endif

    mud->last_mouse_button_down = 0;

    if (mud->settings_camera_auto) {
        if (mud->camera_auto_counter == 0) {
            if (mud->key_left) {
                mud->camera_angle = (mud->camera_angle + 1) & 7;
                mud->key_left = 0;

                if (!mud->fog_of_war) {
                    if ((mud->camera_angle & 1) == 0) {
                        mud->camera_angle = (mud->camera_angle + 1) & 7;
                    }

                    for (int i = 0; i < 8; i++) {
                        if (mudclient_is_valid_camera_angle(
                                mud, mud->camera_angle)) {
                            break;
                        }

                        mud->camera_angle = (mud->camera_angle + 1) & 7;
                    }
                }
            } else if (mud->key_right) {
                mud->camera_angle = (mud->camera_angle + 7) & 7;
                mud->key_right = 0;

                if (!mud->fog_of_war) {
                    if ((mud->camera_angle & 1) == 0) {
                        mud->camera_angle = (mud->camera_angle + 7) & 7;
                    }

                    for (int i = 0; i < 8; i++) {
                        if (mudclient_is_valid_camera_angle(
                                mud, mud->camera_angle)) {
                            break;
                        }

                        mud->camera_angle = (mud->camera_angle + 7) & 7;
                    }
                }
            }
        }
    } else if (mud->key_left) {
        mud->camera_rotation = (mud->camera_rotation + 2) & 0xff;
    } else if (mud->key_right) {
        mud->camera_rotation = (mud->camera_rotation - 2) & 0xff;
    }

    int mouse_x = mud->mouse_x;

#ifdef RENDER_GL
    mouse_x = mud->gl_mouse_x;
#endif

    if (!mud->settings_camera_auto && mud->options->middle_click_camera != 0 &&
        mud->middle_button_down) {
        float scale = mud->options->middle_click_camera / 100.0f;

        mud->camera_rotation =
            (mud->origin_rotation +
             (int)((mouse_x - mud->origin_mouse_x) * scale)) &
            0xff;
    }

    if (mud->options->zoom_camera) {
        mudclient_handle_camera_zoom(mud);
    } else {
        if (mud->fog_of_war && mud->camera_zoom > ZOOM_INDOORS) {
            mud->camera_zoom -= 4;
        } else if (!mud->fog_of_war && mud->camera_zoom < ZOOM_OUTDOORS) {
            mud->camera_zoom += 4;
        }
    }

    if (mud->mouse_click_x_step > 0) {
        mud->mouse_click_x_step--;
    } else if (mud->mouse_click_x_step < 0) {
        mud->mouse_click_x_step++;
    }

#ifdef RENDER_SW
    scene_scroll_texture(mud->scene, FOUNTAIN_ID);
#endif

    mud->object_animation_count++;

    if (mud->object_animation_count > 5) {
        mud->object_animation_count = 0;
        mud->object_animation_cycle = (mud->object_animation_cycle + 1) % 3;
        mud->torch_animation_cycle = (mud->torch_animation_cycle + 1) % 4;
        mud->claw_animation_cycle = (mud->claw_animation_cycle + 1) % 5;
    }

    for (int i = 0; i < mud->object_count; i++) {
        int x = mud->objects[i].x;
        int y = mud->objects[i].y;

        if (x >= 0 && y >= 0 && x < 96 && y < 96 &&
            mud->objects[i].id == WINDMILL_SAILS_ID) {
            game_model_rotate(mud->objects[i].model, 1, 0, 0);
        }
    }

    for (int i = 0; i < mud->magic_bubble_count; i++) {
        mud->magic_bubbles[i].time++;

        if (mud->magic_bubbles[i].time > 50) {
            mud->magic_bubble_count--;

            for (int j = i; j < mud->magic_bubble_count; j++) {
                mud->magic_bubbles[j] = mud->magic_bubbles[j + 1];
            }
        }
    }
}

void mudclient_handle_inputs(mudclient *mud) {
    if (mud->error_loading_data) {
        return;
    }

    mud->login_timer++;

    if (mud->logged_in == 0) {
        mud->mouse_action_timeout = 0;
        mudclient_handle_login_screen_input(mud);
    } else if (mud->logged_in == 1) {
        mud->mouse_action_timeout++;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
        if (mud->gl_is_walking &&
            mud->scene->gl_terrain_pick_step == GL_PICK_STEP_FINISHED) {
            mud->gl_is_walking = 0;
            mud->scene->gl_terrain_pick_step = GL_PICK_STEP_NONE;

#ifdef EMSCRIPTEN
            int x = mud->world->local_x[mud->scene->gl_pick_face_tag];
            int y = mud->world->local_y[mud->scene->gl_pick_face_tag];
#else
            int x = mud->scene->gl_terrain_pick_x;
            int y = mud->scene->gl_terrain_pick_y;
#endif

            mudclient_walk_to_action_source(mud, mud->local_region_x,
                                            mud->local_region_y, x, y, 0);

            if (mud->mouse_click_x_step == -24) {
                mud->mouse_click_x_step = 24;
            }
        }
#endif

        mudclient_handle_game_input(mud);
    }

    mud->last_mouse_button_down = 0;
    mud->camera_rotation_time++;

    if (mud->camera_rotation_time > 500) {
        mud->camera_rotation_time = 0;

        if (mud->options->anti_macro) {
            int roll = (int)(((float)rand() / (float)RAND_MAX) * 4.0f);

            if ((roll & 1) == 1) {
                mud->camera_rotation_x += mud->camera_rotation_x_increment;
            }

            if ((roll & 2) == 2) {
                mud->camera_rotation_y += mud->camera_rotation_y_increment;
            }
        }
    }

    if (mud->camera_rotation_x < -50) {
        mud->camera_rotation_x_increment = 2;
    } else if (mud->camera_rotation_x > 50) {
        mud->camera_rotation_x_increment = -2;
    }

    if (mud->camera_rotation_y < -50) {
        mud->camera_rotation_y_increment = 2;
    } else if (mud->camera_rotation_y > 50) {
        mud->camera_rotation_y_increment = -2;
    }

    mudclient_decrement_message_flash(mud);
}

void mudclient_update_object_animation(mudclient *mud, int object_index,
                                       char *model_name) {
    int object_x = mud->objects[object_index].x;
    int object_y = mud->objects[object_index].y;

    int within_distance = 0;

    if (mud->options->distant_animation) {
        within_distance = 1;
    } else {
        int distance_x = object_x - (mud->local_player->current_x / 128);
        int distance_y = object_y - (mud->local_player->current_y / 128);

        within_distance = distance_x > -OBJECT_ANIMATION_DISTANCE &&
                          distance_x < OBJECT_ANIMATION_DISTANCE &&
                          distance_y > -OBJECT_ANIMATION_DISTANCE &&
                          distance_y < OBJECT_ANIMATION_DISTANCE;
    }

    if (object_x >= 0 && object_y >= 0 && object_x < 96 && object_y < 96 &&
        within_distance) {
        scene_remove_model(mud->scene, mud->objects[object_index].model);

        int model_index = game_data_get_model_index(model_name);
        GameModel *game_model = game_model_copy(mud->game_models[model_index]);

        scene_add_model(mud->scene, game_model);

        game_model_set_light(game_model, 1, 48, 48, -50, -10, -50);
        game_model_copy_position(game_model, mud->objects[object_index].model);

        game_model->key = object_index;

#ifdef RENDER_SW
        game_model_destroy(mud->objects[object_index].model);
#endif
        free(mud->objects[object_index].model);

        mud->objects[object_index].model = game_model;
    }
}

void mudclient_draw_character_message(mudclient *mud, GameCharacter *character,
                                      int x, int y, int width) {
    if (character->message_timeout <= 0) {
        return;
    }
    if (mud->received_messages_count >= RECEIVED_MESSAGE_MAX) {
        return;
    }

    int text_width = surface_text_width(character->message, 1);

    mud->received_message_mid_point[mud->received_messages_count] =
        text_width / 2;

    if (mud->received_message_mid_point[mud->received_messages_count] > 150) {
        mud->received_message_mid_point[mud->received_messages_count] = 150;
    }

    mud->received_message_height[mud->received_messages_count] =
        (text_width / 300) * surface_text_height(1);

    mud->received_message_x[mud->received_messages_count] = x + (width / 2);
    mud->received_message_y[mud->received_messages_count] = y;
    mud->received_messages[mud->received_messages_count++] = character->message;
}

void mudclient_draw_character_damage(mudclient *mud, GameCharacter *character,
                                     int x, int y, int ty, int width,
                                     int height, int is_npc, float depth) {
    if (character->current_animation != 8 &&
        character->current_animation != 9 && character->combat_timer == 0) {
        return;
    }

    if (character->combat_timer > 0) {
        int offset_x = x;

        if (character->current_animation == 8) {
            offset_x -= (20 * ty) / 100;
        } else if (character->current_animation == 9) {
            offset_x += (20 * ty) / 100;
        }

        int missing = (character->current_hits * 30) / character->max_hits;

        if (mud->health_bar_count < HEALTH_BAR_MAX) {
            mud->health_bars[mud->health_bar_count].x = offset_x + (width / 2);
            mud->health_bars[mud->health_bar_count].y = y;
            mud->health_bars[mud->health_bar_count++].missing = missing;
        }
    }

    if (character->combat_timer > 150) {
        int offset_x = x;

        if (character->current_animation == 8) {
            offset_x -= (10 * ty) / 100;
        } else if (character->current_animation == 9) {
            offset_x += (10 * ty) / 100;
        }

        surface_draw_sprite_depth(mud->surface, (offset_x + (width / 2)) - 12,
                                  (y + (height / 2)) - 12,
                                  mud->sprite_media + 11 + (is_npc ? 1 : 0),
                                  depth, depth);

        char damage_string[12] = {0};
        sprintf(damage_string, "%d", character->damage_taken);

        surface_draw_string_centre_depth(
            mud->surface, damage_string, (offset_x + (width / 2)) - 1,
            y + (height / 2) + 5, FONT_BOLD_13, WHITE, depth);
    }
}

// TODO make sure it's a human
int mudclient_should_chop_head(mudclient *mud, GameCharacter *character,
                               ANIMATION_INDEX animation_index) {
#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    int roof_id = world_get_wall_roof(mud->world, character->current_x / 128,
                                      character->current_y / 128);

    return (mud->options->show_roofs && roof_id > 0 &&
            /* check if he's smol */
            (character->npc_id > -1
                 ? game_data.npcs[character->npc_id].height >= 200
                 : 1) &&
            (animation_index == ANIMATION_INDEX_HEAD ||
             animation_index == ANIMATION_INDEX_HEAD_OVERLAY) &&
            !world_is_under_roof(mud->world, mud->local_player->current_x,
                                 mud->local_player->current_y) &&
            world_is_under_roof(mud->world, character->current_x,
                                character->current_y));
#else
    (void)mud;
    (void)character;
    (void)animation_index;

    return 0;
#endif
}

void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int skew_x, int ty, float depth_top,
                           float depth_bottom) {
    GameCharacter *player = mud->players[id];

    if (player->bottom_colour == 255) {
        return;
    }

    int animation_order =
        (player->current_animation + (mud->camera_rotation + 16) / 32) & 7;

    int flip = 0;
    int i2 = animation_order;

    if (i2 == 5) {
        i2 = 3;
        flip = 1;
    } else if (i2 == 6) {
        i2 = 2;
        flip = 1;
    } else if (i2 == 7) {
        i2 = 1;
        flip = 1;
    }

    int j2 = i2 * 3 + character_walk_model[(player->step_count / 6) % 4];

    if (player->current_animation == 8) {
        i2 = 5;
        animation_order = 2;
        flip = 0;
        x -= (5 * ty) / 100;
        j2 = i2 * 3 + character_combat_model_array1[(mud->login_timer / 5) % 8];
    } else if (player->current_animation == 9) {
        i2 = 5;
        animation_order = 2;
        flip = 1;
        x += (5 * ty) / 100;
        j2 = i2 * 3 + character_combat_model_array2[(mud->login_timer / 6) % 8];
    }

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    depth_top = (depth_bottom + depth_top) / 2.0f;
    depth_bottom = depth_top;
#endif

    for (int i = 0; i < ANIMATION_COUNT; i++) {
        ANIMATION_INDEX animation_index =
            character_animation_array[animation_order][i];

        int animation_id = player->animations[animation_index] - 1;

        if (animation_id < 0) {
            continue;
        }

        if (mudclient_should_chop_head(mud, player, animation_index)) {
            continue;
        }

        int offset_x = 0;
        int offset_y = 0;
        int j5 = j2;

        if (flip && i2 >= 1 && i2 <= 3) {
            if (game_data.animations[animation_id].has_f == 1) {
                j5 += 15;
            } else if (animation_index == ANIMATION_INDEX_RIGHT_HAND &&
                       i2 == 1) {
                offset_x = -22;
                offset_y = -3;

                j5 = i2 * 3 +
                     character_walk_model[(2 + (player->step_count / 6)) % 4];
            } else if (animation_index == ANIMATION_INDEX_RIGHT_HAND &&
                       i2 == 2) {
                offset_x = 0;
                offset_y = -8;

                j5 = i2 * 3 +
                     character_walk_model[(2 + (player->step_count / 6)) % 4];
            } else if (animation_index == ANIMATION_INDEX_RIGHT_HAND &&
                       i2 == 3) {
                offset_x = 26;
                offset_y = -5;

                j5 = i2 * 3 +
                     character_walk_model[(2 + (player->step_count / 6)) % 4];
            } else if (animation_index == ANIMATION_INDEX_LEFT_HAND &&
                       i2 == 1) {
                offset_x = 22;
                offset_y = 3;

                j5 = i2 * 3 +
                     character_walk_model[(2 + (player->step_count / 6)) % 4];
            } else if (animation_index == ANIMATION_INDEX_LEFT_HAND &&
                       i2 == 2) {
                offset_x = 0;
                offset_y = 8;

                j5 = i2 * 3 +
                     character_walk_model[(2 + (player->step_count / 6)) % 4];
            } else if (animation_index == ANIMATION_INDEX_LEFT_HAND &&
                       i2 == 3) {
                offset_x = -26;
                offset_y = 5;

                j5 = i2 * 3 +
                     character_walk_model[(2 + (player->step_count / 6)) % 4];
            }
        }

        if (i2 != 5 || game_data.animations[animation_id].has_a == 1) {
            int sprite_id = j5 + game_data.animations[animation_id].file_id;

#ifdef RENDER_SW
            if (mud->surface->surface_pixels[sprite_id] == NULL &&
                mud->surface->sprite_colours[sprite_id] == NULL) {
                /* sprite file was not loaded, probably on f2p version */
                continue;
            }
#endif

            offset_x =
                (offset_x * width) / mud->surface->sprite_width_full[sprite_id];

            offset_y = (offset_y * height) /
                       mud->surface->sprite_height_full[sprite_id];

            int clip_width =
                (width * mud->surface->sprite_width_full[sprite_id]) /
                mud->surface->sprite_width_full
                    [game_data.animations[animation_id].file_id];

            offset_x -= (clip_width - width) / 2;

            int animation_colour = game_data.animations[animation_id].colour;

            if (animation_colour == 1) {
                animation_colour = player_hair_colours[player->hair_colour];
            } else if (animation_colour == 2) {
                animation_colour =
                    player_top_bottom_colours[player->top_colour];
            } else if (animation_colour == 3) {
                animation_colour =
                    player_top_bottom_colours[player->bottom_colour];
            }

            int skin_colour = player_skin_colours[player->skin_colour];

            surface_draw_sprite_transform_mask_depth(
                mud->surface, x + offset_x, y + offset_y, clip_width, height,
                sprite_id, animation_colour, skin_colour, skew_x, flip,
                depth_top, depth_bottom);
        }
    }

    mudclient_draw_character_message(mud, player, x, y, width);

    if (player->bubble_timeout > 0 &&
        mud->action_bubble_count < ACTION_BUBBLE_MAX) {
        mud->action_bubbles[mud->action_bubble_count].x = x + (width / 2);
        mud->action_bubbles[mud->action_bubble_count].y = y;
        mud->action_bubbles[mud->action_bubble_count].scale = ty;

        mud->action_bubbles[mud->action_bubble_count++].item =
            player->bubble_item;
    }

    float damage_depth = 0.0f;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    damage_depth = depth_top;
#endif

    mudclient_draw_character_damage(mud, player, x, y, ty, width, height, 0,
                                    damage_depth);

    if (player->skull_visible && player->bubble_timeout == 0) {
        int k3 = skew_x + x + (width / 2);

        if (player->current_animation == 8) {
            k3 -= (20 * ty) / 100;
        } else if (player->current_animation == 9) {
            k3 += (20 * ty) / 100;
        }

        int width = (16 * ty) / 100;
        int height = (16 * ty) / 100;

        surface_draw_sprite_scale(mud->surface, k3 - (width / 2),
                                  y - (height / 2) - ((10 * ty) / 100), width,
                                  height, mud->sprite_media + 13, damage_depth);
    }
}

void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int skew_x, int ty, float depth_top,
                        float depth_bottom) {
    GameCharacter *npc = mud->npcs[id];

    int animation_order =
        (npc->current_animation + (mud->camera_rotation + 16) / 32) & 7;

    int flip = 0;
    int i2 = animation_order;

    if (i2 == 5) {
        i2 = 3;
        flip = 1;
    } else if (i2 == 6) {
        i2 = 2;
        flip = 1;
    } else if (i2 == 7) {
        i2 = 1;
        flip = 1;
    }

    int j2 =
        i2 * 3 + character_walk_model[(npc->step_count /
                                       game_data.npcs[npc->npc_id].walk_speed) %
                                      4];

    if (npc->current_animation == 8) {
        i2 = 5;
        animation_order = 2;
        flip = 0;
        x -= (game_data.npcs[npc->npc_id].combat_width * ty) / 100;
        j2 = i2 * 3 +
             character_combat_model_array1[((mud->login_timer /
                                                 (game_data.npcs[npc->npc_id]
                                                      .combat_speed) -
                                             1)) %
                                           8];
    } else if (npc->current_animation == 9) {
        i2 = 5;
        animation_order = 2;
        flip = 1;
        x += (game_data.npcs[npc->npc_id].combat_width * ty) / 100;

        j2 =
            i2 * 3 +
            character_combat_model_array2
                [(mud->login_timer / game_data.npcs[npc->npc_id].combat_speed) %
                 8];
    }

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    depth_top = (depth_bottom + depth_top) / 2.0f;
    depth_bottom = depth_top;
#endif

    for (int i = 0; i < ANIMATION_COUNT; i++) {
        int animation_index = character_animation_array[animation_order][i];
        int animation_id = game_data.npcs[npc->npc_id].sprites[animation_index];

        if (animation_id < 0) {
            continue;
        }

        if (mudclient_should_chop_head(mud, npc, animation_index)) {
            continue;
        }

        int offset_x = 0;
        int offset_y = 0;
        int k4 = j2;

        if (flip && i2 >= 1 && i2 <= 3 &&
            game_data.animations[animation_id].has_f == 1) {
            k4 += 15;
        }

        if (i2 != 5 || game_data.animations[animation_id].has_a == 1) {
            int sprite_id = k4 + game_data.animations[animation_id].file_id;

#ifdef RENDER_SW
            if (mud->surface->surface_pixels[sprite_id] == NULL &&
                mud->surface->sprite_colours[sprite_id] == NULL) {
                /* sprite file was not loaded, probably on f2p version */
                continue;
            }
#endif

            offset_x =
                (offset_x * width) / mud->surface->sprite_width_full[sprite_id];

            offset_y = (offset_y * height) /
                       mud->surface->sprite_height_full[sprite_id];

            int clip_width =
                (width * mud->surface->sprite_width_full[sprite_id]) /
                mud->surface->sprite_width_full
                    [game_data.animations[animation_id].file_id];

            offset_x -= (clip_width - width) / 2;

            int animation_colour = game_data.animations[animation_id].colour;

            int skin_colour = 0;

            if (animation_colour == 1) {
                animation_colour = game_data.npcs[npc->npc_id].hair_colour;
                skin_colour = game_data.npcs[npc->npc_id].skin_colour;
            } else if (animation_colour == 2) {
                animation_colour = game_data.npcs[npc->npc_id].top_colour;
                skin_colour = game_data.npcs[npc->npc_id].skin_colour;
            } else if (animation_colour == 3) {
                animation_colour = game_data.npcs[npc->npc_id].bottom_colour;
                skin_colour = game_data.npcs[npc->npc_id].skin_colour;
            }

            surface_draw_sprite_transform_mask_depth(
                mud->surface, x + offset_x, y + offset_y, clip_width, height,
                sprite_id, animation_colour, skin_colour, skew_x, flip,
                depth_top, depth_bottom);
        }
    }

    mudclient_draw_character_message(mud, npc, x, y, width);

    float damage_depth = 0.0f;

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    damage_depth = depth_top;
#endif

    mudclient_draw_character_damage(mud, npc, x, y, ty, width, height, 1,
                                    damage_depth);
}

void mudclient_draw_blue_bar(mudclient *mud) {
    int bars = 1;

    if (mud->surface->width > HBAR_WIDTH) {
        bars += mud->surface->width / HBAR_WIDTH;
    }

    for (int i = 0; i < bars; i++) {
        surface_draw_sprite(mud->surface, i * HBAR_WIDTH,
                            mud->surface->height - 16 +
                                (mud->surface->height < 268 ? 4 : 0),
                            mud->sprite_media + 22);
    }
}

int mudclient_is_in_combat(mudclient *mud) {
    return mud->local_player->current_animation == 8 ||
           mud->local_player->current_animation == 9;
}

GameCharacter *mudclient_get_opponent(mudclient *mud) {
    if (!mudclient_is_in_combat(mud)) {
        if (mud->combat_target != NULL) {
            if (mud->combat_target->max_hits <= 0) {
                return NULL;
            }

            /*
             * if there is a target, check that they are still in view
             */
            if (mud->combat_target->npc_id != -1) {
                for (int i = 0; i < mud->known_npc_count; i++) {
                    if (mud->known_npcs[i] == mud->combat_target) {
                        return mud->combat_target;
                    }
                }
            } else {
                for (int i = 0; i < mud->known_player_count; i++) {
                    if (mud->known_players[i] == mud->combat_target) {
                        return mud->combat_target;
                    }
                }
            }
        }

        return NULL;
    }

    int desired_animation = mud->local_player->current_animation == 8 ? 9 : 8;

    for (int i = 0; i < mud->known_npc_count; i++) {
        GameCharacter *npc = mud->known_npcs[i];

        if (npc->current_x == mud->local_player->current_x &&
            npc->current_y == mud->local_player->current_y &&
            npc->current_animation == desired_animation) {
            return npc;
        }
    }

    for (int i = 0; i < mud->known_player_count; i++) {
        GameCharacter *player = mud->known_players[i];

        if (player->current_x == mud->local_player->current_x &&
            player->current_y == mud->local_player->current_y &&
            player->current_animation == desired_animation) {
            return player;
        }
    }

    return NULL;
}

void mudclient_draw_ui(mudclient *mud) {
    mudclient_draw_ui_tabs(mud);

    int no_menus = !mud->show_option_menu && !mud->show_right_click_menu;

    if (no_menus) {
        mud->menu_items_count = 0;
    }

    if (mud->options->experience_drops) {
        mudclient_draw_experience_drops(mud);
    }

    if (mud->options->status_bars && !mudclient_is_touch(mud)) {
        mudclient_draw_status_bars(mud);
    }

    if (mud->show_additional_options) {
        mudclient_draw_additional_options(mud);

        if (mud->show_dialog_confirm) {
            mudclient_draw_confirm(mud);
        }
    } else if (mud->show_dialog_confirm) {
        mudclient_draw_confirm(mud);
    } else if (mud->logout_timeout != 0) {
        mudclient_draw_logout(mud);
    } else if (mud->show_dialog_welcome) {
        mudclient_draw_welcome(mud);
    } else if (mud->show_dialog_server_message) {
        mudclient_draw_server_message(mud);
    } else if (mud->show_wilderness_warning == 1) {
        mudclient_draw_wilderness_warning(mud);
    } else if (mud->show_dialog_bank && mud->combat_timeout == 0) {
        mudclient_draw_bank(mud);

        if (mud->options->bank_menus) {
            if (mud->show_right_click_menu) {
                mudclient_draw_right_click_menu(mud);
            } else {
                mudclient_create_top_mouse_menu(mud);
            }
        }
    } else if (mud->show_dialog_shop && mud->combat_timeout == 0) {
        mudclient_draw_shop(mud);
    } else if (mud->show_dialog_trade_confirm) {
        mudclient_draw_trade_confirm(mud);
    } else if (mud->show_dialog_trade) {
        mudclient_draw_trade(mud);

        if (mud->options->transaction_menus) {
            if (mud->show_right_click_menu) {
                mudclient_draw_right_click_menu(mud);
            } else {
                mudclient_create_top_mouse_menu(mud);
            }
        }
    } else if (mud->show_dialog_duel_confirm) {
        mudclient_draw_duel_confirm(mud);
    } else if (mud->show_dialog_duel) {
        mudclient_draw_duel(mud);

        if (mud->options->transaction_menus) {
            if (mud->show_right_click_menu) {
                mudclient_draw_right_click_menu(mud);
            } else {
                mudclient_create_top_mouse_menu(mud);
            }
        }
    } else if (mud->show_change_password_step != 0) {
        mudclient_draw_change_password(mud);
    } else if (mud->show_dialog_social_input != 0) {
        mudclient_draw_social_input(mud);
    } else {
        if (mud->show_option_menu) {
            mudclient_draw_option_menu(mud);
        }

        mudclient_set_active_ui_tab(mud, no_menus);

        if (mudclient_is_in_combat(mud) || mud->options->combat_style_always) {
            mudclient_draw_combat_style(mud);
        }

        if (mud->show_ui_tab == 0 && no_menus) {
            mudclient_create_right_click_menu(mud);
        }

        mudclient_draw_active_ui_tab(mud, no_menus);

        if (!mud->show_option_menu) {
            if (mud->show_right_click_menu) {
                mudclient_draw_right_click_menu(mud);
            } else {
                mudclient_create_top_mouse_menu(mud);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
                if (!mud->gl_is_walking) {
                    mud->scene->gl_terrain_pick_step = GL_PICK_STEP_NONE;
                }
#endif
            }
        }

        mudclient_draw_hover_tooltip(mud);
    }

    mud->mouse_button_click = 0;
}

int mudclient_compare_text(const void *v1, const void *v2) {
    struct OverworldText *ot1 = (struct OverworldText *)v1;
    struct OverworldText *ot2 = (struct OverworldText *)v2;
    return strcmp(ot1->text, ot2->text);
}

void mudclient_draw_overhead(mudclient *mud) {
    for (int i = 0; i < mud->received_messages_count; i++) {
        int text_height = surface_text_height(1);
        int x = mud->received_message_x[i];
        int y = mud->received_message_y[i];
        int message_mid = mud->received_message_mid_point[i];
        int message_height = mud->received_message_height[i];
        int flag = 1;

        while (flag) {
            flag = 0;

            for (int j = 0; j < i; j++) {
                if (y + message_height >
                        mud->received_message_y[j] - text_height &&
                    y - text_height < mud->received_message_y[j] +
                                          mud->received_message_height[j] &&
                    x - message_mid < mud->received_message_x[j] +
                                          mud->received_message_mid_point[j] &&
                    x + message_mid > mud->received_message_x[j] -
                                          mud->received_message_mid_point[j] &&
                    mud->received_message_y[j] - text_height - message_height <
                        y) {
                    y = mud->received_message_y[j] - text_height -
                        message_height;

                    flag = 1;
                }
            }
        }

        mud->received_message_y[i] = y;

        if (mudclient_is_ui_scaled(mud)) {
            x /= 2;
            y /= 2;
        }

        surface_draw_paragraph(mud->surface, mud->received_messages[i], x, y, 1,
                               YELLOW, 300);
    }

    for (int i = 0; i < mud->action_bubble_count; i++) {
        int x = mud->action_bubbles[i].x;
        int y = mud->action_bubbles[i].y;
        int scale = mud->action_bubbles[i].scale;

        if (mudclient_is_ui_scaled(mud)) {
            x /= 2;
            y /= 2;
            scale /= 2;
        }

        int id = mud->action_bubbles[i].item;
        int scale_x = (39 * scale) / 100;
        int scale_y = (27 * scale) / 100;

        surface_draw_sprite_scale_alpha(mud->surface, x - (scale_x / 2),
                                        y - scale_y, scale_x, scale_y,
                                        mud->sprite_media + 9, 85);

        int scale_x_clip = (36 * scale) / 100;
        int scale_y_clip = (24 * scale) / 100;

        int final_x = x - (scale_x_clip / 2);
        int final_y = (y - scale_y + (scale_y / 2)) - (scale_y_clip / 2);

        surface_draw_sprite_transform_mask(
            mud->surface, final_x, final_y, scale_x_clip, scale_y_clip,
            game_data.items[id].sprite + mud->sprite_item,
            game_data.items[id].mask, 0, 0, 0);
    }

    /* prevent strobing from random sort order */
    qsort(mud->overworld_text, mud->overworld_text_count,
          sizeof(struct OverworldText), mudclient_compare_text);

    /* check and fix overlapping text */
    for (int i = 0; i < mud->overworld_text_count; i++) {
        int x = mud->overworld_text[i].x;
        int y = mud->overworld_text[i].y;
        int width =
            surface_text_width(mud->overworld_text[i].text, FONT_REGULAR_11);
        int height = surface_text_height(FONT_REGULAR_11);
        for (int j = 0; j < mud->overworld_text_count; j++) {
            int x2 = mud->overworld_text[j].x;
            int y2 = mud->overworld_text[j].y;
            if ((x + width + 2) < x2 || (x - width - 2) > x2) {
                continue;
            }
            if ((y + height + 2) < y2 || (y - height - 2) > y2) {
                continue;
            }
            mud->overworld_text[i].y += (height + 1);
        }
    }

    for (int i = 0; i < mud->overworld_text_count; i++) {
        int32_t colour = (int32_t)mud->overworld_text[i].colour;
        int x = mud->overworld_text[i].x;
        int y = mud->overworld_text[i].y;

        surface_draw_string_centre(mud->surface, mud->overworld_text[i].text, x,
                                   y, FONT_REGULAR_11, colour);
    }

    for (int i = 0; i < mud->health_bar_count; i++) {
        int x = mud->health_bars[i].x;
        int y = mud->health_bars[i].y;
        int missing = mud->health_bars[i].missing;

        if (mudclient_is_ui_scaled(mud)) {
            x /= 2;
            y /= 2;
        }

        surface_draw_box_alpha(mud->surface, x - 15, y - 3, missing, 5, GREEN,
                               192);

        surface_draw_box_alpha(mud->surface, (x - 15) + missing, y - 3,
                               30 - missing, 5, RED, 192);
    }
}

void mudclient_animate_objects(mudclient *mud) {
    char name[23] = {0};

    if (mud->object_animation_cycle != mud->last_object_animation_cycle) {
        mud->last_object_animation_cycle = mud->object_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->objects[i].id == FIRE_ID) {
                sprintf(name, "firea%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->objects[i].id == FIREPLACE_ID) {
                sprintf(name, "fireplacea%d",
                        (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->objects[i].id == LIGHTNING_ID) {
                sprintf(name, "lightning%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->objects[i].id == FIRE_SPELL_ID) {
                sprintf(name, "firespell%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->objects[i].id == SPELL_CHARGE_ID) {
                sprintf(name, "spellcharge%d",
                        (mud->object_animation_cycle + 1));

                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    if (mud->torch_animation_cycle != mud->last_torch_animation_cycle) {
        mud->last_torch_animation_cycle = mud->torch_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->objects[i].id == TORCH_ID) {
                sprintf(name, "torcha%d", mud->torch_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->objects[i].id == SKULL_TORCH_ID) {
                sprintf(name, "skulltorcha%d", mud->torch_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    if (mud->claw_animation_cycle != mud->last_claw_animation_cycle) {
        mud->last_claw_animation_cycle = mud->claw_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->objects[i].id == CLAW_SPELL_ID) {
                sprintf(name, "clawspell%d", mud->claw_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }
}

// TODO prepare entity sprites
void mudclient_draw_entity_sprites(mudclient *mud) {
    scene_reduce_sprites(mud->scene, mud->scene_sprite_count);

    mud->scene_sprite_count = 0;

    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        if (player->bottom_colour == 255) {
            continue;
        }

        int x = player->current_x;
        int y = player->current_y;
        int elevation = -world_get_elevation(mud->world, x, y);

        int sprite_id = scene_add_sprite(mud->scene, 5000 + i, x, elevation, y,
                                         145, 220, i + PLAYER_FACE_TAG);

        mud->scene_sprite_count++;

        if (player == mud->local_player) {
            scene_set_local_player(mud->scene, sprite_id);
        }

        if (player->current_animation == 8) {
            scene_set_sprite_translate_x(mud->scene, sprite_id, -30);
        } else if (player->current_animation == 9) {
            scene_set_sprite_translate_x(mud->scene, sprite_id, 30);
        }
    }

    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        if (player->projectile_range > 0) {
            GameCharacter *character = NULL;

            if (player->attacking_npc_server_index != -1) {
                character =
                    mud->npcs_server[player->attacking_npc_server_index];
            } else if (player->attacking_player_server_index != -1) {
                character =
                    mud->player_server[player->attacking_player_server_index];
            }

            if (character != NULL) {
                int sx = player->current_x;
                int sy = player->current_y;
                int selev = -world_get_elevation(mud->world, sx, sy) - 110;
                int dx = character->current_x;
                int dy = character->current_y;

                /*
                 * Original game incorrectly uses the height of unicorns
                 * for players here, match it.
                 */
                int target_height =
                    player->attacking_npc_server_index != -1
                        ? game_data.npcs[character->npc_id].height
                        : game_data.npcs[0].height;

                int delev = -world_get_elevation(mud->world, dx, dy) -
                            (target_height / 2);

                int rx =
                    (sx * player->projectile_range +
                     dx * (PROJECTILE_RANGE_MAX - player->projectile_range)) /
                    PROJECTILE_RANGE_MAX;

                int rz = (selev * player->projectile_range +
                          delev * (PROJECTILE_RANGE_MAX -
                                   player->projectile_range)) /
                         PROJECTILE_RANGE_MAX;

                int ry =
                    (sy * player->projectile_range +
                     dy * (PROJECTILE_RANGE_MAX - player->projectile_range)) /
                    PROJECTILE_RANGE_MAX;

                scene_add_sprite(mud->scene,
                                 mud->sprite_projectile +
                                     player->incoming_projectile_sprite,
                                 rx, rz, ry, 32, 32, 0);

                mud->scene_sprite_count++;
            }
        }
    }

    for (int i = 0; i < mud->npc_count; i++) {
        GameCharacter *npc = mud->npcs[i];

        int x = npc->current_x;
        int y = npc->current_y;
        int elevation = -world_get_elevation(mud->world, x, y);

        int sprite_id = scene_add_sprite(mud->scene, 20000 + i, x, elevation, y,
                                         game_data.npcs[npc->npc_id].width,
                                         game_data.npcs[npc->npc_id].height,
                                         i + NPC_FACE_TAG);

        mud->scene_sprite_count++;

        if (npc->current_animation == 8) {
            scene_set_sprite_translate_x(mud->scene, sprite_id, -30);
        } else if (npc->current_animation == 9) {
            scene_set_sprite_translate_x(mud->scene, sprite_id, 30);
        }
    }

    for (int i = 0; i < mud->ground_item_count; i++) {
        int x = mud->ground_items[i].x * MAGIC_LOC + 64;
        int y = mud->ground_items[i].y * MAGIC_LOC + 64;
        int id = mud->ground_items[i].id;
        int elevation =
            -world_get_elevation(mud->world, x, y) - mud->ground_items[i].z;

        scene_add_sprite(mud->scene, 40000 + id, x, elevation, y, 96, 64,
                         i + GROUND_ITEM_FACE_TAG);

        mud->scene_sprite_count++;
    }

    for (int i = 0; i < mud->magic_bubble_count; i++) {
        int x = mud->magic_bubbles[i].x * MAGIC_LOC + 64;
        int y = mud->magic_bubbles[i].y * MAGIC_LOC + 64;
        int type = mud->magic_bubbles[i].type;
        int height = type == 0 ? 256 : 64;

        scene_add_sprite(mud->scene, 50000 + i, x,
                         -world_get_elevation(mud->world, x, y), y, 128, height,
                         i + 50000);

        mud->scene_sprite_count++;
    }
}

void mudclient_draw_game(mudclient *mud) {
#ifdef RENDER_3DS_GL
    mudclient_3ds_gl_frame_start(mud, 1);
#endif

    if (mud->death_screen_timeout != 0) {
        surface_fade_to_black(mud->surface);

        surface_draw_string_centre(
            mud->surface, "Oh dear! You are dead...", mud->surface->width / 2,
            (mud->surface->height - 12) / 2, FONT_BOLD_24, RED);

        mudclient_draw_chat_message_tabs(mud);

        surface_draw(mud->surface);
        return;
    }

    if (mud->show_appearance_change) {
        mudclient_draw_appearance_panel(mud);
        return;
    }

    if (mud->is_sleeping) {
        mudclient_draw_sleep(mud);
        return;
    }

    if (!mud->world->player_alive) {
        return;
    }

    for (int i = 0; i < TERRAIN_COUNT; i++) {
        // TODO this is really slow!
        scene_remove_model(mud->scene,
                           mud->world->roof_models[mud->last_plane_index][i]);

        if (mud->last_plane_index == 0) {
            scene_remove_model(mud->scene, mud->world->wall_models[1][i]);
            scene_remove_model(mud->scene, mud->world->roof_models[1][i]);
            scene_remove_model(mud->scene, mud->world->wall_models[2][i]);
            scene_remove_model(mud->scene, mud->world->roof_models[2][i]);
        }

        if (mud->options->show_roofs) {
            mud->fog_of_war = 1;

            if (mud->last_plane_index == 0 &&
                !world_is_under_roof(mud->world, mud->local_player->current_x,
                                     mud->local_player->current_y)) {
                scene_add_model(
                    mud->scene,
                    mud->world->roof_models[mud->last_plane_index][i]);

                scene_add_model(mud->scene, mud->world->wall_models[1][i]);
                scene_add_model(mud->scene, mud->world->roof_models[1][i]);
                scene_add_model(mud->scene, mud->world->wall_models[2][i]);
                scene_add_model(mud->scene, mud->world->roof_models[2][i]);

                mud->fog_of_war = 0;
            }
        }
    }

    if (!mud->options->lowmem) {
        mudclient_animate_objects(mud);
    }

    mudclient_draw_entity_sprites(mud);

    mud->surface->interlace = 0;

    surface_black_screen(mud->surface);

    mud->surface->interlace = mud->options->interlace;

    /* flickering lights in dungeons */
    if (mud->last_plane_index == 3 && mud->options->flicker) {
        int ambience = 40 + ((float)rand() / (float)RAND_MAX) * 3;
        int diffuse = 40 + ((float)rand() / (float)RAND_MAX) * 7;

        scene_set_light(mud->scene, ambience, diffuse, -50, -10, -50);
    }

    mud->action_bubble_count = 0;
    mud->received_messages_count = 0;
    mud->health_bar_count = 0;
    mud->overworld_text_count = 0;

    if (mud->settings_camera_auto && !mud->fog_of_war) {
        mudclient_auto_rotate_camera(mud);
    }

    if (mud->options->zoom_camera) {
        int clip_far =
            (int)((2400.0f / ZOOM_OUTDOORS) * (float)mud->camera_zoom);

        mud->scene->clip_far_3d = clip_far;
        mud->scene->clip_far_2d = clip_far;
        mud->scene->fog_z_distance = clip_far - 100;
    } else {
        mud->scene->clip_far_3d = 2400;
        mud->scene->clip_far_2d = 2400;
        mud->scene->fog_z_distance = 2300;
    }

    if (mud->options->interlace) {
        mud->scene->clip_far_3d -= 200;
        mud->scene->clip_far_2d -= 200;
        mud->scene->fog_z_distance -= 200;
    }

    /* TODO this should probably be tied with FOV instead */
#ifdef RENDER_SW
    /*
     * Keep the fog roughly "feeling the same" as the vanilla
     * 512x346 client when resized beyond that.
     */
    if (mud->game_height > MUD_VANILLA_HEIGHT) {
        int clip_far = mud->scene->clip_far_3d /
                       (MUD_VANILLA_HEIGHT / (float)mud->game_height);

        mud->scene->clip_far_3d = clip_far;
        mud->scene->clip_far_2d = clip_far;
        mud->scene->fog_z_distance = clip_far - 100;
    }
#endif

    if (!mud->options->fog_of_war) {
        mud->scene->clip_far_3d = 20000;
        mud->scene->clip_far_2d = 20000;
        mud->scene->fog_z_distance = 20000;
    }

    int camera_x = mud->camera_auto_rotate_player_x + mud->camera_rotation_x;
    int camera_z = mud->camera_auto_rotate_player_y + mud->camera_rotation_y;

    int offset_y = 0;

    int is_touch = mudclient_is_touch(mud);

    /* centres the camera for the smaller FOV */
    /* TODO could be an option */
    if (is_touch) {
        offset_y = 100;
    } else if (MUD_IS_COMPACT) {
        offset_y = 75;
    }

    scene_set_camera(
        mud->scene, camera_x,
        -world_get_elevation(mud->world, camera_x, camera_z) - offset_y,
        camera_z, 912, (mud->camera_rotation * 4), 0, (mud->camera_zoom * 2));

    surface_black_screen(mud->surface);

#if defined(RENDER_GL) && !defined(EMSCRIPTEN)
    /*if (mud->options->anti_alias) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }*/
#endif

    scene_render(mud->scene);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_draw(mud->surface, GL_DEPTH_DISABLED);
#endif

    mudclient_draw_overhead(mud);

    /* draw the animated X sprite when clicking */
    if (mud->mouse_click_x_step > 0) {
        surface_draw_sprite(
            mud->surface, mud->mouse_click_x_x - 8, mud->mouse_click_x_y - 8,
            mud->sprite_media + 14 + ((24 - mud->mouse_click_x_step) / 6));
    } else if (mud->mouse_click_x_step < 0) {
        surface_draw_sprite(
            mud->surface, mud->mouse_click_x_x - 8, mud->mouse_click_x_y - 8,
            mud->sprite_media + 18 + ((24 + mud->mouse_click_x_step) / 6));
    }

    if (mud->options->display_fps) {
        int offset_x = mud->is_in_wilderness ? 70 : 0;

        char fps[17] = {0};
        sprintf(fps, "Fps: %d", mud->fps);

        surface_draw_string(mud->surface, fps,
                            is_touch ? 9 + offset_x
                                     : mud->surface->width - 62 - offset_x,
                            mud->surface->height - 22, FONT_BOLD_12, YELLOW);
    }

#ifndef REVISION_177
    if (mud->system_update != 0) {
        int seconds = mud->system_update / 50;
        int minutes = seconds / 60;

        seconds %= 60;

        char formatted_update[41] = {0};

        sprintf(formatted_update, "System update in: %d:%02d", minutes,
                seconds);

        surface_draw_string_centre(mud->surface, formatted_update, 256,
                                   mud->game_height - 19, FONT_BOLD_12, YELLOW);
    }
#endif

    if (!mud->loading_area) {
        int wilderness_depth = mudclient_get_wilderness_depth(mud);

        mud->is_in_wilderness = wilderness_depth > 0;

        if (mud->is_in_wilderness) {
            int x = is_touch ? 29 : mud->surface->width - 59;

            surface_draw_sprite(mud->surface, x, mud->surface->height - 68,
                                mud->sprite_media + 13);

            surface_draw_string_centre(mud->surface, "Wilderness", x + 12,
                                       mud->surface->height - 32, FONT_BOLD_12,
                                       YELLOW);

            int wilderness_level = 1 + (wilderness_depth / 6);

            char formatted_level[19] = {0};
            sprintf(formatted_level, "Level: %d", wilderness_level);

            surface_draw_string_centre(mud->surface, formatted_level, x + 12,
                                       mud->surface->height - 19, FONT_BOLD_12,
                                       YELLOW);

            if (mud->show_wilderness_warning == 0) {
                mud->show_wilderness_warning = 2;
            }
        }

        if (mud->options->wilderness_warning &&
            mud->show_wilderness_warning == 0 && wilderness_depth > -10 &&
            wilderness_depth <= 0) {
            mud->show_wilderness_warning = 1;
        }
    }

    mudclient_draw_chat_message_tabs_panel(mud);
    mudclient_draw_ui(mud);

    mud->surface->draw_string_shadow = 0;
    mudclient_draw_chat_message_tabs(mud);

    if (mud->options->status_bars && mudclient_is_touch(mud)) {
        mud->surface->draw_string_shadow = 1;
        mudclient_draw_status_bars(mud);
    }

#ifdef RENDER_GL
    scene_gl_render_transparent_models(mud->scene);
#elif defined(RENDER_3DS_GL)
    scene_3ds_gl_render_transparent_models(mud->scene);
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_draw(mud->surface, GL_DEPTH_ENABLED);
    surface_gl_reset_context(mud->surface);
#else
    surface_draw(mud->surface);
#endif

#if defined(_3DS) && defined(RENDER_SW)
    gfxFlushBuffers();
    gfxSwapBuffers();
#endif
}

void mudclient_draw(mudclient *mud) {
#ifdef EMSCRIPTEN
    if (get_ticks() - last_canvas_check > 1000) {
        if (can_resize() && (get_window_width() != mud->game_width ||
                             get_window_height() != mud->game_height)) {
            mudclient_on_resize(mud);
        }

        last_canvas_check = get_ticks();
    }
#endif

    if (mud->error_loading_data) {
        /* TODO draw error */
        // mud_log("ERROR LOADING DATA\n");
        return;
    }

#ifdef WII
    draw_background(mud->framebuffer, 0);
#endif

#ifdef RENDER_GL
    glClear(GL_DEPTH_BUFFER_BIT);
#endif

    if (mud->logged_in == 0) {
        mud->surface->draw_string_shadow = 0;
        mudclient_draw_login_screens(mud);
    } else if (mud->logged_in == 1) {
        mud->surface->draw_string_shadow = 1;
        mudclient_draw_game(mud);
#ifdef RENDER_GL
#ifdef SDL12
        SDL_GL_SwapBuffers();
#else
        SDL_GL_SwapWindow(mud->gl_window);
#endif
#elif defined(RENDER_3DS_GL)
        mudclient_3ds_gl_frame_end();
#endif
    }
}

#ifdef SDL12
void mudclient_sdl1_on_resize(mudclient *mud, int width, int height) {
    int new_width = width;
    int new_height = height;
#ifdef RENDER_SW
    if ((SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_RESIZABLE)) ==
        NULL) {
        return;
    }
#else
    if ((SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_RESIZABLE)) ==
        NULL) {
        return;
    }
#endif
    mud->game_width = new_width;
    mud->game_height = new_height;

    if (mud->surface != NULL) {
        if (mudclient_is_ui_scaled(mud)) {
            mud->surface->width = new_width / 2;
            mud->surface->height = new_height / 2;
        } else {
            mud->surface->width = new_width;
            mud->surface->height = new_height;
        }

        surface_reset_bounds(mud->surface);
    }

    if (mud->scene != NULL) {
#ifdef RENDER_SW
        free(mud->scene->scanlines);
#endif

        // TODO change 12 to bar height - 1
        scene_set_bounds(mud->scene, new_width, new_height - 12);

#ifdef RENDER_GL
        mudclient_update_fov(mud);
#endif
    }

    mudclient_resize(mud);
}
#endif

void mudclient_on_resize(mudclient *mud) {
    int new_width = MUD_WIDTH;
    int new_height = MUD_HEIGHT;

#ifndef SDL12
#ifdef RENDER_GL
    SDL_Window *window = mud->gl_window;
#else
    SDL_Window *window = mud->window;
#endif
#endif

#ifdef EMSCRIPTEN
    new_width = get_window_width();
    new_height = get_window_height();
    SDL_SetWindowSize(window, new_width, new_height);
#endif

#ifndef SDL12
    SDL_GetWindowSize(window, &new_width, &new_height);
#endif

#ifdef ANDROID
    mudclient_full_width = new_width;
    mudclient_full_height = new_height;

    if (new_width > new_height) {
        new_width =
            roundf(360 * (mudclient_full_width / (float)mudclient_full_height));

        new_height = 360;
    } else {
        new_width = 360;

        new_height =
            roundf(360 * (mudclient_full_height / (float)mudclient_full_width));
    }
#endif

    mud->game_width = new_width;
    mud->game_height = new_height;

    if (mud->surface != NULL) {
        if (mudclient_is_ui_scaled(mud)) {
            mud->surface->width = new_width / 2;
            mud->surface->height = new_height / 2;
        } else {
            mud->surface->width = new_width;
            mud->surface->height = new_height;
        }

        surface_reset_bounds(mud->surface);
    }

    if (mud->scene != NULL) {
#ifdef RENDER_SW
        free(mud->scene->scanlines);
#endif

        // TODO change 12 to bar height - 1
        scene_set_bounds(mud->scene, new_width, new_height - 12);

#ifdef RENDER_GL
        mudclient_update_fov(mud);
#endif
    }

    mudclient_resize(mud);
}


int mudclient_is_touch(mudclient *mud) {
    (void)(mud);

#ifdef ANDROID
    return 1; // TODO maybe still make this toggleable
#elif defined(EMSCRIPTEN)
    return browser_is_touch();
#else
    return 0;
#endif
}

// TODO open_keyboard
void mudclient_trigger_keyboard(mudclient *mud, char *text, int is_password,
                                int x, int y, int width, int height, int font,
                                int is_centred) {
    (void)mud;
    (void)text;
    (void)is_password;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)font;
    (void)is_centred;
#ifdef ANDROID
    SDL_StartTextInput();
#elif defined(EMSCRIPTEN)
    if (mudclient_is_ui_scaled(mud)) {
        // TODO
    }

    browser_trigger_keyboard(text, is_password, x, y, width, height, font,
                             is_centred);
#endif
}

void mudclient_run(mudclient *mud) {
#ifdef WII
    draw_background(mud->framebuffers[0], 1);
    draw_background(mud->framebuffers[1], 1);

    mud->active_framebuffer ^= 1;
    mud->framebuffer = mud->framebuffers[mud->active_framebuffer];
#endif

    if (mud->loading_step == 1) {
        mud->loading_step = 2;
        mudclient_load_jagex(mud);
        mudclient_start_game(mud);
        mud->loading_step = 0;

#ifdef EMSCRIPTEN
        mudclient_on_resize(mud);
#endif
    }

    int timing_index = 0;
    int j = 256;
    int delay = 1;
    int i1 = 0;

    for (int i = 0; i < 10; i++) {
        mud->timings[i] = get_ticks();
    }

    while (mud->stop_timeout >= 0) {
        if (mud->stop_timeout > 0) {
            mud->stop_timeout--;

            if (mud->stop_timeout == 0) {
                mudclient_close_connection(mud);
                return;
            }
        }

        int k1 = j;
        int last_delay = delay;

        j = 300;
        delay = 1;

        int time = get_ticks();

        if (mud->timings[timing_index] == 0) {
            j = k1;
            delay = last_delay;
        } else if (time > mud->timings[timing_index]) {
            j = (float)(2560 * mud->target_fps) /
                (float)(time - mud->timings[timing_index]);
        }

        if (j < 25) {
            j = 25;
        }

        if (j > 256) {
            j = 256;
            delay = mud->target_fps - (time - mud->timings[timing_index]) / 10;

            // TODO minimum delay
            if (delay < 10) {
                delay = 10;
            }
        }

        delay_ticks(delay);

        mud->timings[timing_index] = time;
        timing_index = (timing_index + 1) % 10;

        if (delay > 1) {
            for (int i = 0; i < 10; i++) {
                if (mud->timings[i] != 0) {
                    mud->timings[i] += delay;
                }
            }
        }

        int k2 = 0;

        while (i1 < 256) {
            mudclient_poll_events(mud);
            mudclient_handle_inputs(mud);

            i1 += j;

            // TODO magic #
            if (++k2 > 1000) {
                i1 = 0;
                break;
            }
        }

        i1 &= 255;

#ifdef _3DS
        if (!mud->keyboard_open) {
            mudclient_draw(mud);
        }
#else
        mudclient_draw(mud);
#endif

#ifdef _3DS
        mudclient_3ds_flush_audio(mud);

        if (!aptMainLoop()) {
            return;
        }
#endif

        mud->fps = (j * 1000) / (mud->target_fps * 256);

        mud->mouse_scroll_delta = 0;
    }
}

void mudclient_draw_magic_bubble(mudclient *mud, int x, int y, int width,
                                 int height, int id, float depth) {
    int type = mud->magic_bubbles[id].type;
    int time = mud->magic_bubbles[id].time;

    if (type == 0) {
        /* blue bubble used for teleports */
        int colour = BLUE + time * 5 * 256;

        surface_draw_circle(mud->surface, x + (width / 2), y + (height / 2),
                            20 + time * 2, colour, 255 - time * 5, depth);
    } else if (type == 1) {
        /* red bubble used for telegrab */
        int colour = RED + time * 5 * 256;

        surface_draw_circle(mud->surface, x + (width / 2), y + (height / 2),
                            10 + time, colour, 255 - time * 5, depth);
    }
}

void mudclient_draw_ground_item(mudclient *mud, int x, int y, int width,
                                int height, int id, float depth_top,
                                float depth_bottom) {
    int32_t highlight_colour = highlight_item(id);

    if (highlight_colour != 0 && mud->options->ground_item_text &&
        mud->overworld_text_count < OVERWORLD_TEXT_MAX) {
        struct OverworldText text = {0};

        text.text = game_data.items[id].name;
        text.colour = highlight_colour;
        text.x = x + (width / 2);
        text.y = y - (height / 2);

        mud->overworld_text[mud->overworld_text_count++] = text;
    }

    if (!mud->options->ground_item_models) {
        int picture = game_data.items[id].sprite + mud->sprite_item;
        int mask = game_data.items[id].mask;

        surface_draw_sprite_transform_mask_depth(mud->surface, x, y, width,
                                                 height, picture, mask, 0, 0, 0,
                                                 depth_top, depth_bottom);
    }
}

int mudclient_is_item_equipped(mudclient *mud, int id) {
    for (int i = 0; i < mud->inventory_items_count; i++) {
        if (mud->inventory_item_id[i] == id && mud->inventory_equipped[i]) {
            return 1;
        }
    }

    return 0;
}

int mudclient_get_inventory_count(mudclient *mud, int id) {
    int count = 0;

    for (int i = 0; i < mud->inventory_items_count; i++) {
        if (mud->inventory_item_id[i] == id) {
            if (game_data.items[id].stackable == 1) {
                count++;
            } else {
                count += mud->inventory_item_stack_count[i];
            }
        }
    }

    return count;
}

int mudclient_has_inventory_item(mudclient *mud, int id, int minimum) {
    if (id == FIRE_RUNE_ID &&
        (mudclient_is_item_equipped(mud, FIRE_STAFF_ID) ||
         mudclient_is_item_equipped(mud, FIRE_BATTLESTAFF_ID) ||
         mudclient_is_item_equipped(mud, ENCHANTED_FIRE_BATTLESTAFF_ID))) {
        return 1;
    }

    if (id == WATER_RUNE_ID &&
        (mudclient_is_item_equipped(mud, WATER_STAFF_ID) ||
         mudclient_is_item_equipped(mud, WATER_BATTLESTAFF_ID) ||
         mudclient_is_item_equipped(mud, ENCHANTED_WATER_BATTLESTAFF_ID))) {
        return 1;
    }

    if (id == AIR_RUNE_ID &&
        (mudclient_is_item_equipped(mud, AIR_STAFF_ID) ||
         mudclient_is_item_equipped(mud, AIR_BATTLESTAFF_ID) ||
         mudclient_is_item_equipped(mud, ENCHANTED_AIR_BATTLESTAFF_ID))) {
        return 1;
    }

    if (id == EARTH_RUNE_ID &&
        (mudclient_is_item_equipped(mud, EARTH_STAFF_ID) ||
         mudclient_is_item_equipped(mud, EARTH_BATTLESTAFF_ID) ||
         mudclient_is_item_equipped(mud, ENCHANTED_EARTH_BATTLESTAFF_ID))) {
        return 1;
    }

    return mudclient_get_inventory_count(mud, id) >= minimum;
}

void mudclient_send_logout(mudclient *mud) {
    if (mud->logged_in == 0) {
        return;
    }

    if (mud->combat_timeout > 450) {
        mudclient_show_message(mud, "@cya@You can't logout during combat!",
                               MESSAGE_TYPE_GAME);

        return;
    }

    if (mud->combat_timeout > 0) {
        mudclient_show_message(
            mud, "@cya@You can't logout for 10 seconds after combat",
            MESSAGE_TYPE_GAME);

        return;
    }

    packet_stream_new_packet(mud->packet_stream, CLIENT_LOGOUT);
    packet_stream_send_packet(mud->packet_stream);

    mud->logout_timeout = 1000;
}

void mudclient_play_sound(mudclient *mud, char *name) {
    if (!mud->options->members || mud->settings_sound_disabled ||
        mud->options->lowmem) {
        return;
    }

#ifdef _3DS
    if (mud->_3ds_sound_position != -1) {
        return;
    }
#endif

    char file_name[strlen(name) + 5];
    sprintf(file_name, "%s.pcm", name);

    uint32_t offset = get_data_file_offset(file_name, mud->sound_data);

    if (offset == 0) {
        return;
    }

    uint32_t length = get_data_file_length(file_name, mud->sound_data);

    memset(mud->pcm_out, 0, PCM_LENGTH * sizeof(uint16_t));

    ulaw_to_linear(length, (uint8_t *)mud->sound_data + offset, mud->pcm_out);

#ifdef WII
    // ASND_StopVoice(0);

    ASND_SetVoice(0, VOICE_MONO_16BIT_BE, SAMPLE_RATE, 0, mud->pcm_out,
                  length * 2, 127, 127, NULL);
#elif defined(_3DS)
    mud->_3ds_sound_position = 0;
    mud->_3ds_sound_length = length * 2;
#elif defined(SDL_VERSION_ATLEAST)
#if SDL_VERSION_ATLEAST(2, 0, 4)
    // TODO could re-pause after sound plays?
    SDL_PauseAudio(0);
    SDL_QueueAudio(1, mud->pcm_out, length * 2);
#endif
#endif
}

int mudclient_walk_to(mudclient *mud, int start_x, int start_y, int x1, int y1,
                      int x2, int y2, int check_objects, int walk_to_action,
                      int first_step) {
    int steps = world_route(mud->world, start_x, start_y, x1, y1, x2, y2,
                            mud->walk_path_x, mud->walk_path_y, check_objects);

    if (first_step) {
        if (steps == -1) {
            if (walk_to_action) {
                steps = 1;
                mud->walk_path_x[0] = x1;
                mud->walk_path_y[0] = y1;
            } else {
                return 0;
            }
        }
    } else {
        if (steps == -1) {
            return 0;
        }
    }

    steps--;
    start_x = mud->walk_path_x[steps];
    start_y = mud->walk_path_y[steps];
    steps--;

    packet_stream_new_packet(mud->packet_stream,
                             walk_to_action ? CLIENT_WALK_ACTION : CLIENT_WALK);

    packet_stream_put_short(mud->packet_stream, start_x + mud->region_x);
    packet_stream_put_short(mud->packet_stream, start_y + mud->region_y);

    if (walk_to_action && steps == -1 && (start_x + mud->region_x) % 5 == 0) {
        steps = 0;
    }

    for (int i = steps; i >= 0 && i > steps - 25; i--) {
        packet_stream_put_byte(mud->packet_stream,
                               mud->walk_path_x[i] - start_x);

        packet_stream_put_byte(mud->packet_stream,
                               mud->walk_path_y[i] - start_y);
    }

    packet_stream_send_packet(mud->packet_stream);

    mud->mouse_click_x_step = -24;
    mud->mouse_click_x_x = mud->mouse_x;
    mud->mouse_click_x_y = mud->mouse_y;

    return 1;
}

void mudclient_walk_to_action_source(mudclient *mud, int start_x, int start_y,
                                     int dest_x, int dest_y, int action) {
    mudclient_walk_to(mud, start_x, start_y, dest_x, dest_y, dest_x, dest_y, 0,
                      action, 1);
}

void mudclient_walk_to_ground_item(mudclient *mud, int start_x, int start_y,
                                   int dest_x, int dest_y, int walk_to_action) {
    if (mudclient_walk_to(mud, start_x, start_y, dest_x, dest_y, dest_x, dest_y,
                          0, walk_to_action, 0)) {
        return;
    }

    mudclient_walk_to(mud, start_x, start_y, dest_x, dest_y, dest_x, dest_y, 1,
                      walk_to_action, 1);
}

void mudclient_walk_to_wall_object(mudclient *mud, int dest_x, int dest_y,
                                   int direction) {
    if (direction == 0) {
        mudclient_walk_to(mud, mud->local_region_x, mud->local_region_y, dest_x,
                          dest_y - 1, dest_x, dest_y, 0, 1, 1);
    } else if (direction == 1) {
        mudclient_walk_to(mud, mud->local_region_x, mud->local_region_y,
                          dest_x - 1, dest_y, dest_x, dest_y, 0, 1, 1);
    } else {
        mudclient_walk_to(mud, mud->local_region_x, mud->local_region_y, dest_x,
                          dest_y, dest_x, dest_y, 1, 1, 1);
    }
}

void mudclient_walk_to_object(mudclient *mud, int x, int y, int direction,
                              int id) {
    int width = 0;
    int height = 0;

    if (direction == 0 || direction == 4) {
        width = game_data.objects[id].width;
        height = game_data.objects[id].height;
    } else {
        height = game_data.objects[id].width;
        width = game_data.objects[id].height;
    }

    if (game_data.objects[id].type == 2 || game_data.objects[id].type == 3) {
        if (direction == 0) {
            x--;
            width++;
        } else if (direction == 2) {
            height++;
        } else if (direction == 4) {
            width++;
        } else if (direction == 6) {
            y--;
            height++;
        }

        mudclient_walk_to(mud, mud->local_region_x, mud->local_region_y, x, y,
                          (x + width) - 1, (y + height) - 1, 0, 1, 1);
    } else {
        mudclient_walk_to(mud, mud->local_region_x, mud->local_region_y, x, y,
                          (x + width) - 1, (y + height) - 1, 1, 1, 1);
    }
}

int mudclient_is_ui_scaled(mudclient *mud) {
#ifdef RENDER_GL
    return mud->options->ui_scale && mud->game_width >= (MUD_WIDTH * 2) &&
           mud->game_height >= (MUD_HEIGHT * 2);
#else
    (void)mud;

    return 0;
#endif
}

void mudclient_format_number_commas(mudclient *mud, int number, char *dest) {
    if (mud->options->number_commas) {
        format_number_commas(number, dest);
    } else {
        sprintf(dest, "%d", number);
    }
}

void mudclient_format_item_amount(mudclient *mud, int item_amount, char *dest) {
    if (mud->options->condense_item_amounts) {
        format_amount_suffix(item_amount, 1, 0, mud->options->number_commas,
                             dest);
    } else {
        mudclient_format_number_commas(mud, item_amount, dest);
    }
}

int mudclient_get_wilderness_depth(mudclient *mud) {
    int wilderness_depth =
        2203 - (mud->local_region_y + mud->plane_height + mud->region_y);

    if (mud->local_region_x + mud->plane_width + mud->region_x >= 2640) {
        wilderness_depth = -50;
    }

    return wilderness_depth;
}

void mudclient_draw_item(mudclient *mud, int x, int y, int slot_width,
                         int slot_height, int item_id) {
    int certificate_item_id = -1;

    if (mud->options->certificate_items) {
        certificate_item_id = get_certificate_item_id(item_id);
    }

    int offset_x = 0;

    if (certificate_item_id != -1) {
        offset_x = -2;
    }

    surface_draw_item(mud->surface, x + offset_x, y, slot_width, slot_height,
                      item_id);

    if (certificate_item_id != -1) {
        int og_width = ITEM_GRID_SLOT_WIDTH - 1;
        int og_height = ITEM_GRID_SLOT_HEIGHT - 2;

        surface_draw_sprite_transform_mask(
            mud->surface, x + 4 + og_width * 0.125f, y + 2 + og_height * 0.125f,
            og_width * 0.75f, og_height * 0.75f,
            mud->surface->mud->sprite_item +
                game_data.items[certificate_item_id].sprite,
            game_data.items[certificate_item_id].mask, 0, 0, 0);
    }
}
#ifdef WIN9X
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   int nCmdShow) {
    int argc;
    char **argv;

    argv = (char**)CommandLineToArgvW(GetCommandLineW(), &argc);
#else
int main(int argc, char **argv) {
#endif
#ifdef _3DS
    osSetSpeedupEnable(true);
#endif
    srand(0);

    init_utility_global();
    init_surface_global();
    init_world_global();
    /*init_packet_stream_global();*/
    init_stats_tab_global();

    mudclient *mud = malloc(sizeof(mudclient));
    mudclient_new(mud);

#ifdef EMSCRIPTEN
    global_mud = mud;
#endif

    if (argc > 1 && strlen(argv[1]) > 0) {
        mud->options->members = strcmp(argv[1], "members") == 0;
    }

    if (argc > 2) {
        strcpy(mud->options->server, argv[2]);
    }

    if (argc > 3) {
        mud->options->port = atoi(argv[3]);
    }

#ifdef REVISION_177
    /* BEGIN INAUTHENTIC COMMAND LINE ARGUMENTS */
    if (argc > 4) {
        strcpy(mud->options->rsa_exponent, argv[4]);
    }

    if (argc > 5) {
        strcpy(mud->options->rsa_modulus, argv[5]);
    }
    /* END INAUTHENTIC COMMAND LINE ARGUMENTS */
#endif

    mudclient_start_application(mud, "Runescape by Andrew Gower");

#ifdef RENDER_3DS_GL
    shaderProgramFree(&mud->surface->_3ds_gl_flat_shader);
    DVLB_Free(mud->surface->_3ds_gl_flat_shader_dvlb);

    C3D_Fini();
#endif

#ifdef _3DS
    linearFree(audio_buffer);
    ndspExit();

    gfxExit();
#endif

    return 0;
}

#ifdef EMSCRIPTEN
void browser_mouse_moved(int x, int y) {
    mudclient_mouse_moved(global_mud, x, y);
}

void browser_key_pressed(int code, int char_code) {
    mudclient_key_pressed(global_mud, code, char_code);
}
#endif
