#include "mudclient.h"

char *font_files[] = {"h11p.jf", "h12b.jf", "h12p.jf", "h13b.jf",
                      "h14b.jf", "h16b.jf", "h20b.jf", "h24b.jf"};

char *animated_models[] = {
    "torcha2",      "torcha3",    "torcha4",    "skulltorcha2", "skulltorcha3",
    "skulltorcha4", "firea2",     "firea3",     "fireplacea2",  "fireplacea3",
    "firespell2",   "firespell3", "lightning2", "lightning3",   "clawspell2",
    "clawspell3",   "clawspell4", "clawspell5", "spellcharge2", "spellcharge3"};

char login_screen_status[255] = {0};

int character_animation_array[8][12] = {{11, 2, 9, 7, 1, 6, 10, 0, 5, 8, 3, 4},
                                        {11, 2, 9, 7, 1, 6, 10, 0, 5, 8, 3, 4},
                                        {11, 3, 2, 9, 7, 1, 6, 10, 0, 5, 8, 4},
                                        {3, 4, 2, 9, 7, 1, 6, 10, 8, 11, 0, 5},
                                        {3, 4, 2, 9, 7, 1, 6, 10, 8, 11, 0, 5},
                                        {4, 3, 2, 9, 7, 1, 6, 10, 8, 11, 0, 5},
                                        {11, 4, 2, 9, 7, 1, 6, 10, 0, 5, 8, 3},
                                        {11, 2, 9, 7, 1, 6, 10, 0, 5, 8, 4, 3}};

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

#if defined(_3DS) || defined(WII)
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
#endif

#ifdef _3DS
u32 *SOC_buffer = NULL;

void soc_shutdown() { socExit(); }

ndspWaveBuf wave_buf[2] = {0};
u32 *audio_buffer = NULL;
int fill_block = 0;

void draw_rectangle(uint8_t *framebuffer, int x, int y, int width, int height) {
    for (int draw_y = y; draw_y < y + height; draw_y++) {
        for (int draw_x = x; draw_x < x + width; draw_x++) {
            int fb_index = (((draw_x + 1) * 240) - (draw_y + 1)) * 3;
            framebuffer[fb_index] = 132;
            framebuffer[fb_index + 1] = 132;
            framebuffer[fb_index + 2] = 132;
        }
    }
}

void draw_blue_bar(uint8_t *framebuffer) {
    for (int x = 0; x < 256; x++) {
        for (int y = 1; y <= 7; y++) {
            int bar_index = (((x + 31) * 240) + (240 - y)) * 3;

            int screen_index = (((x + 72) * 240) + (240 - (y + 50))) * 3;

            memcpy(framebuffer + screen_index, game_background_bgr + bar_index,
                   3);
        }
    }
}
#endif

#ifdef WII
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

void draw_rectangle(uint8_t *framebuffer, int x, int y, int width, int height) {
    for (int draw_y = y; draw_y < y + height; draw_y++) {
        for (int draw_x = x; draw_x < x + width; draw_x += 2) {
            int y = RGB2Y(132, 132, 132);
            int u = RGB2U(132, 132, 132);
            int v = RGB2V(132, 132, 132);

            int fb_index = (640 * 2 * draw_y) + (draw_x * 2);

            framebuffer[fb_index] = y;
            framebuffer[fb_index + 2] = y;
            framebuffer[fb_index + 1] = u;
            framebuffer[fb_index + 3] = v;
        }
    }
}

int wii_mouse_x = 0;
int wii_mouse_y = 0;
int wii_mouse_button = 0;

int update_wii_mouse(WPADData *wiimote_data) {
    mouse_event me;

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

#if !defined(WII) && !defined(_3DS)
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code) {
    *char_code = -1;

    switch (keysym->scancode) {
    case SDL_SCANCODE_LEFT:
        *code = K_LEFT;
        break;
    case SDL_SCANCODE_RIGHT:
        *code = K_RIGHT;
        break;
    case SDL_SCANCODE_F1:
        *code = K_F1;
        break;
    /*case SDL_SCANCODE_RETURN:
        *code = K_ENTER;
        break;
    case SDL_SCANCODE_UP:
        *code = 38;
        break;
    case SDL_SCANCODE_DOWN:
        *code = 40;
        break;*/
    default:
        *char_code = keysym->sym;
        *code = *char_code;

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

/*int test_x = 520;
int test_y = -106;
int test_z = 750;*/
int test_x = 0;
int test_y = 0;
int test_z = 0;
int test_yaw = 38;
GameModel *test_model = NULL;

void mudclient_new(mudclient *mud) {
    memset(mud, 0, sizeof(mudclient));

    // mud->interlace = 1;
    // mud->members = 1;

    mud->applet_width = MUD_WIDTH;
    mud->applet_height = MUD_HEIGHT;
    mud->target_fps = 20;
    mud->max_draw_time = 1000;
    mud->loading_step = 1;
    mud->loading_progess_text = "Loading";
    mud->thread_sleep = 10;
    //mud->server = "192.168.100.103";
    mud->server = "127.0.0.1";
    mud->port = 43594;
    // mud->server = "162.198.202.160"; /* openrsc preservation */
    // mud->port = 43596;
    // mud->port = 43496; /* websockets */
    mud->game_width = mud->applet_width;
    mud->game_height = mud->applet_height - 12;
    mud->camera_angle = 1;
    mud->camera_rotation = 128;
    mud->camera_zoom = ZOOM_INDOORS;
    mud->camera_rotation_x_increment = 2;
    mud->camera_rotation_y_increment = 2;
    mud->last_height_offset = -1;

    mud->options = malloc(sizeof(Options));
    options_new(mud->options);

    for (int i = 0; i < MESSAGE_HISTORY_LENGTH; i++) {
        memset(mud->message_history[i], '\0', 255);
    }

    mud->selected_spell = -1;
    mud->selected_item_name = "";
    mud->selected_item_inventory_index = -1;
    mud->quest_complete = calloc(quests_length, sizeof(int8_t));

#ifdef _3DS
    mud->sound_position = -1;

    /* for top screen panning */
    mud->zoom_offset_x = 56;
    mud->zoom_offset_y = 53;
#endif

    mud->appearance_body_type = 1;
    mud->appearance_hair_colour = 2;
    mud->appearance_top_colour = 8;
    mud->appearance_bottom_colour = 14;
    mud->appearance_head_gender = 1;

    mud->sleep_word_delay = 1;

    /* set by the server to 192 on p2p servers */
    mud->bank_items_max = 48;
}

void mudclient_start_application(mudclient *mud, int width, int height,
                                 char *title) {
    mud->applet_width = width;
    mud->applet_height = height;
    mud->loading_step = 1;

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
#endif

#ifdef _3DS
    // gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, 0);

    atexit(soc_shutdown);

    gfxInitDefault();
    // consoleInit(GFX_TOP, NULL);

    Result romfs_res = romfsInit();

    if (romfs_res) {
        fprintf(stderr, "romfsInit: %08lX\n", romfs_res);
        exit(1);
    }

    gfxSetDoubleBuffering(GFX_BOTTOM, 0);
    gfxSetDoubleBuffering(GFX_TOP, 0);

    // mud->framebuffer_top = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 400, 240);

    mud->framebuffer_top = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

    mud->framebuffer_bottom =
        gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

    // allocate buffer for SOC service (networking)
    SOC_buffer = (u32 *)memalign(SOC_ALIGN, SOC_BUFFER_SIZE);

    if (SOC_buffer == NULL) {
        fprintf(stderr, "memalign() fail\n");
        exit(1);
    }

    int ret = -1;

    if ((ret = socInit(SOC_buffer, SOC_BUFFER_SIZE)) != 0) {
        fprintf(stderr, "socInit: 0x%08X\n", (unsigned int)ret);
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

    wave_buf[1].data_vaddr = &audio_buffer[SAMPLE_BUFFER_SIZE];
    wave_buf[1].nsamples = SAMPLE_BUFFER_SIZE;

    ndspChnWaveBufAdd(0, &wave_buf[0]);
    ndspChnWaveBufAdd(0, &wave_buf[1]);
#endif

#if !defined(WII) && !defined(_3DS)
    int init = SDL_INIT_VIDEO;

    if (mud->members) {
        init |= SDL_INIT_AUDIO;
    }

    if (SDL_Init(init) < 0) {
        fprintf(stderr, "SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

    if (mud->members) {
        SDL_AudioSpec wanted_audio;

        wanted_audio.freq = SAMPLE_RATE;
        wanted_audio.format = AUDIO_S16;
        wanted_audio.channels = 1;
        wanted_audio.silence = 0;
        wanted_audio.samples = 1024;

        wanted_audio.callback = NULL;

        if (SDL_OpenAudio(&wanted_audio, NULL) < 0) {
            fprintf(stderr, "SDL_OpenAudio(): %s\n", SDL_GetError());
            exit(1);
        }
    }

#ifdef RENDER_SW
    mud->window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height, SDL_WINDOW_SHOWN);

    mud->screen = SDL_GetWindowSurface(mud->window);
#endif

    mud->pixel_surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff0000,
                                              0x00ff00, 0x0000ff, 0);

#ifdef RENDER_GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    mud->gl_window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GLContext *context = SDL_GL_CreateContext(mud->gl_window);

    if (!context) {
        fprintf(stderr, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_MakeCurrent(mud->gl_window, context);

    glewExperimental = GL_TRUE;

    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
        fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(glew_error));
        exit(1);
    }

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* transparent textures */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
#endif

    printf("Started application\n");

#ifdef WII
    while (1) {
        memcpy(mud->framebuffer, rsc_type_yuv,
               rsc_type_yuv_width * rsc_type_yuv_height * 2);

        WPAD_ReadPending(WPAD_CHAN_ALL, NULL);
        int res = WPAD_Probe(0, NULL);

        if (res != WPAD_ERR_NONE) {
            update_wii_mouse(NULL);
        } else {
            WPADData *wiimote_data = WPAD_Data(0);
            update_wii_mouse(wiimote_data);
        }

        int button_down = wii_mouse_button != 0;
        int mouse_x = wii_mouse_x;
        int mouse_y = wii_mouse_y;

        if (button_down) {
            if (mouse_x >= 62 && mouse_x <= 250 && mouse_y >= 128 &&
                mouse_y <= 230) {
                mud->members = 0;

                mudclient_run(mud);
                break;
            } else if (mouse_x >= 404 && mouse_x <= 576 && mouse_y >= 132 &&
                       mouse_y <= 232) {
                mud->members = 1;

                mudclient_run(mud);
                break;
            }
        }

        draw_arrow(mud->framebuffer, mouse_x, mouse_y);

        VIDEO_SetNextFramebuffer(mud->framebuffer);
        mud->active_framebuffer ^= 1;
        mud->framebuffer = mud->framebuffers[mud->active_framebuffer];
        VIDEO_Flush();
        VIDEO_WaitVSync();
    }
#endif

#ifdef _3DS
    memcpy((uint8_t *)mud->framebuffer_top, game_top_bgr, game_top_bgr_size);

    gspWaitForVBlank();

    while (aptMainLoop()) {
        memcpy((uint8_t *)mud->framebuffer_bottom, game_type_bgr,
               game_type_bgr_size);

        hidScanInput();

        touchPosition touch;
        hidTouchRead(&touch);

        if (touch.px >= 30 && touch.py >= 38 && touch.px <= 126 &&
            touch.py <= 88) {
            memcpy((uint8_t *)mud->framebuffer_bottom, game_background_bgr,
                   game_background_bgr_size);

            gspWaitForVBlank();

            mud->members = 0;
            mudclient_run(mud);
            break;
        } else if (touch.px >= 200 && touch.py >= 38 && touch.px <= 290 &&
                   touch.py <= 90) {
            memcpy((uint8_t *)mud->framebuffer_bottom, game_background_bgr,
                   game_background_bgr_size);

            gspWaitForVBlank();

            mud->members = 1;
            mudclient_run(mud);
            break;
        }
    }
#endif

#if !defined(WII) && !defined(_3DS)
    mudclient_run(mud);
#endif
}

void mudclient_handle_key_press(mudclient *mud, int key_code) {
    if (mud->logged_in == 0) {
        if (mud->login_screen == 0 && mud->panel_login_welcome) {
            panel_key_press(mud->panel_login_welcome, key_code);
        }

        if (mud->login_screen == 1 && mud->panel_login_new_user) {
            panel_key_press(mud->panel_login_new_user, key_code);
        }

        if (mud->login_screen == 2 && mud->panel_login_existing_user) {
            panel_key_press(mud->panel_login_existing_user, key_code);
        }

        /*if (mud->login_screen == 3 && mud->panel_recover_user) {
            panel_key_press(mud->panel_recover_user, key_code);
        }*/
    }

    if (mud->logged_in == 1) {
        if (mud->show_appearance_change && mud->panel_appearance) {
            panel_key_press(mud->panel_appearance, key_code);
            return;
        }

        if (mud->show_change_password_step == 0 &&
            mud->show_dialog_social_input == 0 &&
            /*mud->show_dialog_report_abuse_step == 0 &&*/
            !mud->is_sleeping && mud->panel_message_tabs) {
            panel_key_press(mud->panel_message_tabs, key_code);
        }

        if (mud->show_change_password_step == 3 ||
            mud->show_change_password_step == 4) {
            mud->show_change_password_step = 0;
        }
    }
}

void mudclient_key_pressed(mudclient *mud, int code, int char_code) {
    if (char_code == -1) {
        if (code == K_LEFT) {
            mud->key_left = 1;
        } else if (code == K_RIGHT) {
            mud->key_right = 1;
        } else if (code == K_F1) {
            mud->interlace = !mud->interlace;
        }
    } else {
        mudclient_handle_key_press(mud, char_code);
    }

    int found_text = 0;

    for (int i = 0; i < CHAR_SET_LENGTH; i++) {
        if (CHAR_SET[i] == char_code) {
            found_text = 1;
            break;
        }
    }

    if (found_text) {
        int current_length = strlen(mud->input_text_current);

        if (current_length < INPUT_TEXT_LENGTH) {
            mud->input_text_current[current_length] = char_code;
            mud->input_text_current[current_length + 1] = '\0';
        }

        int pm_length = strlen(mud->input_pm_current);

        if (pm_length < INPUT_PM_LENGTH) {
            mud->input_pm_current[pm_length] = char_code;
            mud->input_pm_current[pm_length + 1] = '\0';
        }
    }

    if (code == K_ENTER) {
        strcpy(mud->input_text_final, mud->input_text_current);
        strcpy(mud->input_pm_final, mud->input_pm_current);
    } else if (code == K_BACKSPACE) {
        int current_length = strlen(mud->input_text_current);

        if (current_length > 0) {
            mud->input_text_current[current_length - 1] = '\0';
        }

        int pm_length = strlen(mud->input_pm_current);

        if (pm_length > 0) {
            mud->input_pm_current[pm_length - 1] = '\0';
        }
    }
}

void mudclient_key_released(mudclient *mud, int code) {
    if (code == K_LEFT) {
        mud->key_left = 0;
    } else if (code == K_RIGHT) {
        mud->key_right = 0;
    }
}

void mudclient_mouse_moved(mudclient *mud, int x, int y) {
    mud->mouse_x = x;
    mud->mouse_y = y;
    mud->mouse_action_timeout = 0;
}

void mudclient_mouse_released(mudclient *mud, int x, int y, int button) {
    mud->mouse_x = x;
    mud->mouse_y = y;

    mud->mouse_button_down = 0;

    if (button == 1) {
        mud->middle_button_down = 0;
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

    if (mud->options->middle_click_camera && button == 2) {
        mud->middle_button_down = 1;
        mud->origin_rotation = mud->camera_rotation;
        mud->origin_mouse_x = mud->mouse_x;
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

void mudclient_show_loading_progress(mudclient *mud, int percent, char *text) {
#ifdef _3DS
    int width = (int)((percent / (float)100) * 137);
    draw_rectangle(mud->framebuffer_bottom, 91, 101, width, 8);
#endif

#ifdef WII
    int width = (int)((percent / (float)100) * 276);
    draw_rectangle(mud->framebuffer, 181, 244, width, 19);
#endif

#if !defined(WII) && !defined(_3DS)
    SDL_Rect texture_dest = {0};
    texture_dest.w = 128;
    texture_dest.h = 128;

    SDL_FillRect(mud->pixel_surface, &texture_dest, 0x000000);

    SDL_Rect loading_dest = {0};
    loading_dest.x = ((mud->applet_width - 281) / 2) + 2;
    loading_dest.y = ((mud->applet_height - 148) / 2) + 76;
    loading_dest.w = (int)((percent / (float)100) * 277);
    loading_dest.h = 20;

    SDL_FillRect(mud->pixel_surface, &loading_dest, 0x848484);

    SDL_BlitSurface(mud->pixel_surface, NULL, mud->screen, NULL);
    SDL_UpdateWindowSurface(mud->window);
#endif
}

int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent) {
    char loading_text[35] = {0}; /* max description is 19 */

    sprintf(loading_text, "Loading %s - 0%%", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

    int8_t header[6];
#ifdef WII
    const int8_t *file_data = NULL;

    if (strcmp(file, "fonts" FONTS ".jag") == 0) {
        file_data = (int8_t *)fonts1_jag;
    } else if (strcmp(file, "config" CONFIG ".jag") == 0) {
        file_data = (int8_t *)config85_jag;
    } else if (strcmp(file, "media" MEDIA ".jag") == 0) {
        file_data = (int8_t *)media58_jag;
    } else if (strcmp(file, "entity" ENTITY ".jag") == 0) {
        file_data = (int8_t *)entity24_jag;
    } else if (strcmp(file, "entity" ENTITY ".mem") == 0) {
        file_data = (int8_t *)entity24_mem;
    } else if (strcmp(file, "textures" TEXTURES ".jag") == 0) {
        file_data = (int8_t *)textures17_jag;
    } else if (strcmp(file, "maps" MAPS ".jag") == 0) {
        file_data = (int8_t *)maps63_jag;
    } else if (strcmp(file, "maps" MAPS ".mem") == 0) {
        file_data = (int8_t *)maps63_mem;
    } else if (strcmp(file, "land" MAPS ".jag") == 0) {
        file_data = (int8_t *)land63_jag;
    } else if (strcmp(file, "land" MAPS ".mem") == 0) {
        file_data = (int8_t *)land63_mem;
    } else if (strcmp(file, "models" MODELS ".jag") == 0) {
        file_data = (int8_t *)models36_jag;
    } else if (strcmp(file, "sounds" SOUNDS ".mem") == 0) {
        file_data = (int8_t *)sounds1_mem;
    }

    if (file_data == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", file);
        exit(1);
    }

    memcpy(header, file_data, 6);
#else
    int file_length = strlen(file);

#ifdef _3DS
    char *prefix = "romfs:";
#else
    char *prefix = "./cache";
#endif

    char prefixed_file[file_length + strlen(prefix) + 2];
    sprintf(prefixed_file, "%s/%s", prefix, file);

    FILE *archive_stream = fopen(prefixed_file, "rb");

    if (archive_stream == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", prefixed_file);
        exit(1);
    }

    fread(header, 6, 1, archive_stream);
#endif

    int archive_size = ((header[0] & 0xff) << 16) + ((header[1] & 0xff) << 8) +
                       (header[2] & 0xff);

    int archive_size_compressed = ((header[3] & 0xff) << 16) +
                                  ((header[4] & 0xff) << 8) +
                                  (header[5] & 0xff);

    sprintf(loading_text, "Loading %s - 5%%", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

#ifdef WII
    int8_t *archive_data = file_data + 6;
#else
    int read = 0;
    int8_t *archive_data = malloc(archive_size_compressed);

    while (read < archive_size_compressed) {
        int length = archive_size_compressed - read;

        if (length > 1000) {
            length = 1000;
        }

        fread(archive_data + read, length, 1, archive_stream);

        read += length;

        sprintf(loading_text, "Loading %s - %d", description,
                5 + (read * 95) / archive_size_compressed);

        mudclient_show_loading_progress(mud, percent, loading_text);
    }

    fclose(archive_stream);
#endif

    sprintf(loading_text, "Unpacking %s", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

    if (archive_size_compressed != archive_size) {
        int8_t *decompressed = malloc(archive_size);

        bzip_decompress(decompressed, archive_size, archive_data,
                        archive_size_compressed, 0);

#ifndef WII
        free(archive_data);
#endif

        return decompressed;
    }

    return archive_data;
}

/* used for the jagex logo in the loading screen */
#if !defined(WII) && !defined(_3DS)
SDL_Surface *mudclient_parse_tga(mudclient *mud, int8_t *buffer) {
    int width = buffer[13] * 256 + buffer[12];
    int height = buffer[15] * 256 + buffer[14];

    uint8_t r[256] = {0};
    uint8_t g[256] = {0};
    uint8_t b[256] = {0};

    for (int i = 0; i < 256; i++) {
        r[i] = buffer[20 + i * 3];
        g[i] = buffer[19 + i * 3];
        b[i] = buffer[18 + i * 3];
    }

    uint8_t *pixels = calloc(width * height * 4, sizeof(uint8_t));
    int index = 0;

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            int palette_index = buffer[(256 * 3) + x + y * width];
            pixels[index++] = b[palette_index];
            pixels[index++] = g[palette_index];
            pixels[index++] = r[palette_index];
            pixels[index++] = 0;
        }
    }

    mud->logo_pixels = pixels;

    return SDL_CreateRGBSurfaceFrom(pixels, width, height, 32, 4 * width,
                                    0xff0000, 0x00ff00, 0x0000ff, 0);
}
#endif

void mudclient_load_jagex(mudclient *mud) {
#if !defined(WII) && !defined(_3DS)
    int8_t *jagex_jag =
        mudclient_read_data_file(mud, "jagex.jag", "Jagex library", 0);

    if (jagex_jag != NULL) {
        int8_t *logo_tga = load_data("logo.tga", 0, jagex_jag);
        mud->logo_surface = mudclient_parse_tga(mud, logo_tga);
        free(logo_tga);
        free(jagex_jag);
    }
#endif

    int8_t *fonts_jag =
        mudclient_read_data_file(mud, "fonts" FONTS ".jag", "Game fonts", 5);

    if (fonts_jag != NULL) {
        for (int i = 0; i < FONT_COUNT; i++) {
            create_font(load_data(font_files[i], 0, fonts_jag), i);
        }

        free(fonts_jag);
    }
}

void mudclient_draw_loading_screen(mudclient *mud) {
#if !defined(WII) && !defined(_3DS)
    SDL_Rect logo_dest = {0};
    logo_dest.x = ((mud->applet_width - 281) / 2) - 18;
    logo_dest.y = ((mud->applet_height - 148) / 2) - 14;

    SDL_BlitSurface(mud->logo_surface, NULL, mud->pixel_surface, &logo_dest);

    SDL_Rect loading_dest = {0};
    loading_dest.x = ((mud->applet_width - 281) / 2);
    loading_dest.y = logo_dest.y + 88;
    loading_dest.w = 281;
    loading_dest.h = 24;

    SDL_FillRect(mud->pixel_surface, &loading_dest, 0x848484);

    loading_dest.y += 1;
    loading_dest.x += 1;
    loading_dest.w -= 2;
    loading_dest.h -= 2;

    SDL_FillRect(mud->pixel_surface, &loading_dest, 0);

    SDL_BlitSurface(mud->pixel_surface, NULL, mud->screen, NULL);
    SDL_UpdateWindowSurface(mud->window);

    SDL_FreeSurface(mud->logo_surface);
    free(mud->logo_pixels);
#endif
}

void mudclient_load_game_config(mudclient *mud) {
    int8_t *config_jag = mudclient_read_data_file(mud, "config" CONFIG ".jag",
                                                  "Configuration", 10);

    if (config_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    game_data_load_data(config_jag, mud->members);
    free(config_jag);

    /*int8_t *filter_jag = mudclient_read_data_file(mud, "filter" FILTER ".jag",
                                                  "Chat system", 15);

    if (filter_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    free(filter_jag);*/
}

void mudclient_load_media(mudclient *mud) {
    int8_t *media_jag =
        mudclient_read_data_file(mud, "media" MEDIA ".jag", "2d graphics", 20);

    if (media_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    int8_t *index_dat = load_data("index.dat", 0, media_jag);

    surface_parse_sprite(mud->surface, mud->sprite_media,
                         load_data("inv1.dat", 0, media_jag), index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 1,
                         load_data("inv2.dat", 0, media_jag), index_dat, 6);

    surface_parse_sprite(mud->surface, mud->sprite_media + 9,
                         load_data("bubble.dat", 0, media_jag), index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 10,
                         load_data("runescape.dat", 0, media_jag), index_dat,
                         1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 11,
                         load_data("splat.dat", 0, media_jag), index_dat, 3);

    surface_parse_sprite(mud->surface, mud->sprite_media + 14,
                         load_data("icon.dat", 0, media_jag), index_dat, 8);

    surface_parse_sprite(mud->surface, mud->sprite_media + 22,
                         load_data("hbar.dat", 0, media_jag), index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 23,
                         load_data("hbar2.dat", 0, media_jag), index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 24,
                         load_data("compass.dat", 0, media_jag), index_dat, 1);

    surface_parse_sprite(mud->surface, mud->sprite_media + 25,
                         load_data("buttons.dat", 0, media_jag), index_dat, 2);

    surface_parse_sprite(mud->surface, mud->sprite_util,
                         load_data("scrollbar.dat", 0, media_jag), index_dat,
                         2);

    surface_parse_sprite(mud->surface, mud->sprite_util + 2,
                         load_data("corners.dat", 0, media_jag), index_dat, 4);

    surface_parse_sprite(mud->surface, mud->sprite_util + 6,
                         load_data("arrows.dat", 0, media_jag), index_dat, 2);

    surface_parse_sprite(mud->surface, mud->sprite_projectile,
                         load_data("projectile.dat", 0, media_jag), index_dat,
                         game_data_projectile_sprite);

    int sprite_count = game_data_item_sprite_count;

    for (int i = 1; sprite_count > 0; i++) {
        char file_name[20] = {0};
        sprintf(file_name, "objects%d.dat", i);

        int current_sprite_count = sprite_count;
        sprite_count -= 30;

        if (current_sprite_count > 30) {
            current_sprite_count = 30;
        }

        surface_parse_sprite(mud->surface, mud->sprite_item + (i - 1) * 30,
                             load_data(file_name, 0, media_jag), index_dat,
                             current_sprite_count);
    }

    free(index_dat);

#ifndef WII
    free(media_jag);
#endif

#ifdef RENDER_SW
    surface_load_sprite(mud->surface, mud->sprite_media);
    surface_load_sprite(mud->surface, mud->sprite_media + 9);

    for (int i = 11; i <= 26; i++) {
        surface_load_sprite(mud->surface, mud->sprite_media + i);
    }

    for (int i = 0; i < game_data_projectile_sprite; i++) {
        surface_load_sprite(mud->surface, mud->sprite_projectile + i);
    }

    for (int i = 0; i < game_data_item_sprite_count; i++) {
        surface_load_sprite(mud->surface, mud->sprite_item + i);
    }
#endif
}

void mudclient_load_entities(mudclient *mud) {
    int8_t *entity_jag = mudclient_read_data_file(mud, "entity" ENTITY ".jag",
                                                  "people and monsters", 30);

    if (entity_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    int8_t *index_dat = load_data("index.dat", 0, entity_jag);
    int8_t *entity_jag_mem = NULL;
    int8_t *index_dat_mem = NULL;

    if (mud->members) {
        entity_jag_mem = mudclient_read_data_file(mud, "entity" ENTITY ".mem",
                                                  "member graphics", 45);

        if (entity_jag_mem == NULL) {
            mud->error_loading_data = 1;
            return;
        }

        index_dat_mem = load_data("index.dat", 0, entity_jag_mem);
    }

    int frame_count = 0;
    int animation_index = 0;

    int i = 0;

    while (i < game_data_animation_count) {
    label0:;
        char *animation_name = game_data_animation_name[i];

        for (int j = 0; j < i; j++) {
            if (strcmp(game_data_animation_name[j], animation_name) != 0) {
                continue;
            }

            game_data_animation_number[i] = game_data_animation_number[j];
            i++;
            goto label0;
        }

        char file_name[255] = {0};
        sprintf(file_name, "%s.dat", animation_name);

        int8_t *animation_dat = load_data(file_name, 0, entity_jag);
        int8_t *animation_index_dat = index_dat;

        if (animation_dat == NULL && mud->members) {
            animation_dat = load_data(file_name, 0, entity_jag_mem);
            animation_index_dat = index_dat_mem;
        }

        if (animation_dat != NULL) {
            surface_parse_sprite(mud->surface, animation_index, animation_dat,
                                 animation_index_dat, 15);

            frame_count += 15;

            if (game_data_animation_has_a[i]) {
                sprintf(file_name, "%sa.dat", animation_name);

                int8_t *a_dat = load_data(file_name, 0, entity_jag);
                int8_t *a_index_dat = index_dat;

                if (a_dat == NULL && mud->members) {
                    a_dat = load_data(file_name, 0, entity_jag_mem);
                    a_index_dat = index_dat_mem;
                }

                surface_parse_sprite(mud->surface, animation_index + 15, a_dat,
                                     a_index_dat, 3);

                frame_count += 3;
            }

            if (game_data_animation_has_f[i]) {
                sprintf(file_name, "%sf.dat", animation_name);

                int8_t *f_dat = load_data(file_name, 0, entity_jag);
                int8_t *f_index_dat = index_dat;

                if (f_dat == NULL && mud->members) {
                    f_dat = load_data(file_name, 0, entity_jag_mem);
                    f_index_dat = index_dat_mem;
                }

                surface_parse_sprite(mud->surface, animation_index + 18, f_dat,
                                     f_index_dat, 9);

                frame_count += 9;
            }

            /* TODO why? */
            if (game_data_animation_gender[i] != 0) {
                for (int j = animation_index; j < animation_index + 27; j++) {
                    surface_load_sprite(mud->surface, j);
                }
            }
        }

        game_data_animation_number[i] = animation_index;
        animation_index += 27;

        i++;
    }

    printf("Loaded: %d frames of animation\n", frame_count);

#ifndef WII
    free(entity_jag);
    free(entity_jag_mem);
#endif

    free(index_dat);
    free(index_dat_mem);
}

void mudclient_load_textures(mudclient *mud) {
    int8_t *textures_jag = mudclient_read_data_file(
        mud, "textures" TEXTURES ".jag", "Textures", 50);

    if (textures_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    int8_t *index_dat = load_data("index.dat", 0, textures_jag);

    scene_allocate_textures(mud->scene, game_data_texture_count, 7, 11);

    char file_name[255] = {0};

#ifdef RENDER_GL
    glGenTextures(1, &mud->scene->game_model_textures);
    glBindTexture(GL_TEXTURE_2D_ARRAY, mud->scene->game_model_textures);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 128, 128,
                   game_data_texture_count);

    int32_t *texture_raster = calloc(128 * 128, sizeof(int32_t));
#endif

    Surface *surface = mud->surface;

    for (int i = 0; i < game_data_texture_count; i++) {
        sprintf(file_name, "%s.dat", game_data_texture_name[i]);

        int8_t *texture_dat = load_data(file_name, 0, textures_jag);

        surface_parse_sprite(surface, mud->sprite_texture, texture_dat,
                             index_dat, 1);

        surface_draw_box(surface, 0, 0, 128, 128, MAGENTA);
        surface_draw_sprite_from3(surface, 0, 0, mud->sprite_texture);

        free(surface->sprite_colour_list[mud->sprite_texture]);
        surface->sprite_colour_list[mud->sprite_texture] = NULL;

        free(surface->sprite_colours_used[mud->sprite_texture]);
        surface->sprite_colours_used[mud->sprite_texture] = NULL;

        int texture_size = surface->sprite_width_full[mud->sprite_texture];
        char *name_sub = game_data_texture_subtype_name[i];

        if (name_sub) {
            int sub_length = strlen(name_sub);

            if (sub_length) {
                sprintf(file_name, "%s.dat", name_sub);

                int8_t *texture_sub_dat = load_data(file_name, 0, textures_jag);

                surface_parse_sprite(surface, mud->sprite_texture,
                                     texture_sub_dat, index_dat, 1);

                surface_draw_sprite_from3(surface, 0, 0, mud->sprite_texture);

                free(surface->sprite_colour_list[mud->sprite_texture]);
                surface->sprite_colour_list[mud->sprite_texture] = NULL;

                free(surface->sprite_colours_used[mud->sprite_texture]);
                surface->sprite_colours_used[mud->sprite_texture] = NULL;
            }
        }

        surface_draw_sprite_from5(surface, mud->sprite_texture_world + i, 0, 0,
                                  texture_size, texture_size);

        for (int j = 0; j < texture_size * texture_size; j++) {
            if (surface->surface_pixels[mud->sprite_texture_world + i][j] ==
                GREEN) {
                surface->surface_pixels[mud->sprite_texture_world + i][j] =
                    MAGENTA;
            }

#ifdef RENDER_GL
            int colour =
                surface->surface_pixels[mud->sprite_texture_world + i][j];

            if (colour != MAGENTA) {
                texture_raster[j] = 0xff000000 + colour;
            }
#endif
        }

#ifdef RENDER_GL
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 128, 128, 1, GL_BGRA,
                        GL_UNSIGNED_BYTE, texture_raster);

        memset(texture_raster, 0, 128 * 128 * sizeof(int32_t));
#endif

        surface_screen_raster_to_sprite(surface, mud->sprite_texture_world + i);

        scene_define_texture(
            mud->scene, i,
            surface->sprite_colours_used[mud->sprite_texture_world + i],
            surface->sprite_colour_list[mud->sprite_texture_world + i],
            (texture_size / 64) - 1);

        /*free(surface->sprite_colour_list[mud->sprite_texture_world + i]);
        surface->sprite_colour_list[mud->sprite_texture_world + i] = NULL;

        free(surface->sprite_colours_used[mud->sprite_texture_world + i]);
        surface->sprite_colours_used[mud->sprite_texture_world + i] =
        NULL;*/

        free(surface->surface_pixels[mud->sprite_texture_world + i]);
        surface->surface_pixels[mud->sprite_texture_world + i] = NULL;
    }

#ifdef RENDER_GL
    free(texture_raster);
#endif

#ifndef WII
    free(textures_jag);
#endif

    free(index_dat);
}

void mudclient_load_models(mudclient *mud) {
    for (int i = 0; i < ANIMATED_MODELS_COUNT; i++) {
        char name_length = strlen(animated_models[i]);
        char *name = malloc(name_length + 1);
        strcpy(name, animated_models[i]);
        game_data_get_model_index(name);
    }

    int8_t *models_jag =
        mudclient_read_data_file(mud, "models" MODELS ".jag", "3d models", 60);

    if (models_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    for (int i = 0; i < game_data_model_count - 1; i++) {
        char *model_name = game_data_model_name[i];

        char file_name[strlen(model_name) + 5];
        sprintf(file_name, "%s.ob3", model_name);

        int offset = get_data_file_offset(file_name, models_jag);

        GameModel *game_model = malloc(sizeof(GameModel));

        if (offset != 0) {
            game_model_from_bytes(game_model, models_jag, offset);
        } else {
            game_model_from2(game_model, 1, 1);
        }

        mud->game_models[i] = game_model;

        if (strcmp(model_name, "giantcrystal") == 0) {
            mud->game_models[i]->transparent = 1;
        }
    }

    free(models_jag);

#ifdef RENDER_GL
    int total_vertices = 0;
    int total_ebo_length = 0;

    for (int i = 0; i < game_data_model_count - 1; i++) {
        GameModel *game_model = mud->game_models[i];

        game_model->ebo_offset = total_ebo_length;

        for (int j = 0; j < game_model->num_faces; j++) {
            int face_num_vertices = game_model->face_num_vertices[j];
            total_vertices += face_num_vertices;
            game_model->ebo_length += (face_num_vertices - 2) * 3;
        }

        total_ebo_length += game_model->ebo_length;
    }

    glGenVertexArrays(1, &mud->scene->game_model_vao);
    glBindVertexArray(mud->scene->game_model_vao);

    glGenBuffers(1, &mud->scene->game_model_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mud->scene->game_model_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 10 * total_vertices, NULL,
                 GL_STATIC_DRAW);

    /* vertex { x, y, z } */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat),
                          (void *)0);

    glEnableVertexAttribArray(0);

    /* colour { r, g, b, a } */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(1);

    /* texture { s, t, index } */
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat),
                          (void *)(7 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);

    glGenBuffers(1, &mud->scene->game_model_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mud->scene->game_model_ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_ebo_length * sizeof(GLuint),
                 NULL, GL_STATIC_DRAW);

    total_vertices = 0;
    total_ebo_length = 0;

    for (int i = 0; i < game_data_model_count - 1; i++) {
        GameModel *game_model = mud->game_models[i];

        game_model_gl_buffer_arrays(game_model, &total_vertices,
                                    &total_ebo_length);

        if (i > 3) {
            break;
        }
    }
#endif
}

void mudclient_load_maps(mudclient *mud) {
    mud->world->map_pack =
        mudclient_read_data_file(mud, "maps" MAPS ".jag", "map", 70);

    if (mud->members) {
        mud->world->member_map_pack = mudclient_read_data_file(
            mud, "maps" MAPS ".mem", "members map", 75);
    }

    mud->world->landscape_pack =
        mudclient_read_data_file(mud, "land" MAPS ".jag", "landscape", 80);

    if (mud->members) {
        mud->world->member_landscape_pack = mudclient_read_data_file(
            mud, "land" MAPS ".mem", "members landscape", 85);
    }
}

void mudclient_load_sounds(mudclient *mud) {
    mud->sound_data = mudclient_read_data_file(mud, "sounds" SOUNDS ".mem",
                                               "Sound effects", 90);
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
    surface_draw(mud->surface);

    for (int i = 0; i < mud->object_count; i++) {
        scene_remove_model(mud->scene, mud->object_model[i]);

        world_remove_object(mud->world, mud->object_x[i], mud->object_y[i],
                            mud->object_id[i]);

        game_model_destroy(mud->object_model[i]);
        free(mud->object_model[i]);
        mud->object_model[i] = NULL;
    }

    for (int i = 0; i < mud->wall_object_count; i++) {
        scene_remove_model(mud->scene, mud->wall_object_model[i]);

        world_remove_wall_object(
            mud->world, mud->wall_object_x[i], mud->wall_object_y[i],
            mud->wall_object_direction[i], mud->wall_object_id[i]);

        game_model_destroy(mud->wall_object_model[i]);
        free(mud->wall_object_model[i]);
        mud->wall_object_model[i] = NULL;
    }

    mud->object_count = 0;
    mud->wall_object_count = 0;
    mud->ground_item_count = 0;
    mud->player_count = 0;

    GameCharacter *freed_characters[5000] = {0};
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
            for (int j = 0; j < 5000; j++) {
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

    mud->local_player = malloc(sizeof(GameCharacter));
    game_character_new(mud->local_player);

    memset(freed_characters, 0, sizeof(GameCharacter *) * 5000);
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

    if (strlen(username) == 0) {
        mudclient_show_login_screen_status(mud,
                                           "You must enter both a username",
                                           "and a password - Please try again");
        return;
    }

    strcpy(mud->username, username);
    strcpy(mud->password, password);

    char formatted_username[USERNAME_LENGTH + 1] = {0};
    format_auth_string(username, USERNAME_LENGTH, formatted_username);

    char formatted_password[PASSWORD_LENGTH + 1] = {0};
    format_auth_string(password, PASSWORD_LENGTH, formatted_password);

    if (reconnecting) {
        /*mudclient_draw_text_box(
            mud,
            "Connection lost! Please wait...",
            "Attempting to re-establish"
        );*/
    } else {
        mudclient_show_login_screen_status(mud, "Please wait...",
                                           "Connecting to server");
    }

    free(mud->packet_stream);
    mud->packet_stream = malloc(sizeof(PacketStream));
    packet_stream_new(mud->packet_stream, mud);

    if (mud->packet_stream->closed) {
        mudclient_show_login_screen_status(
            mud, "Sorry! Unable to connect.",
            "Check internet settings or try another world");
        return;
    }

#ifdef REVISION_177
    int session_id = packet_stream_get_int(mud->packet_stream);
    mud->session_id = session_id;
#else
    packet_stream_new_packet(mud->packet_stream, CLIENT_SESSION);

    int64_t encoded_username = encode_username(formatted_username);

    packet_stream_put_byte(mud->packet_stream,
                           (int)((encoded_username >> 16) & 31));

    packet_stream_flush_packet(mud->packet_stream);

    int64_t session_id = packet_stream_get_long(mud->packet_stream);
    mud->session_id = session_id;
#endif

    if (mud->session_id == 0) {
        mudclient_show_login_screen_status(mud, "Login server offline.",
                                           "Please try again in a few mins");
        return;
    }

#ifdef REVISION_177
    printf("Session id: %d\n", session_id);

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

    packet_stream_flush_packet(mud->packet_stream);

    packet_stream_get_byte(mud->packet_stream);

    int response = packet_stream_get_byte(mud->packet_stream);
#else
    printf("Verb: Session id: %ld\n", session_id);

    int32_t keys[4];
    keys[0] = (int)(((float)rand() / (float)RAND_MAX) * (float)99999999);
    keys[1] = (int)(((float)rand() / (float)RAND_MAX) * (float)99999999);
    keys[2] = (int32_t)(session_id >> 32);
    keys[3] = (int32_t)(session_id);

    packet_stream_new_packet(mud->packet_stream, CLIENT_LOGIN);
    packet_stream_put_byte(mud->packet_stream, reconnecting);
    packet_stream_put_short(mud->packet_stream, VERSION);
    packet_stream_put_byte(mud->packet_stream, 0);
    packet_stream_put_byte(mud->packet_stream, 10);
    packet_stream_put_int(mud->packet_stream, keys[0]);
    packet_stream_put_int(mud->packet_stream, keys[1]);
    packet_stream_put_int(mud->packet_stream, keys[2]);
    packet_stream_put_int(mud->packet_stream, keys[3]);
    packet_stream_put_int(mud->packet_stream, 0); // uuid
    packet_stream_put_string(mud->packet_stream, formatted_username);
    packet_stream_put_string(mud->packet_stream, formatted_password);

    packet_stream_flush_packet(mud->packet_stream);

    int response = packet_stream_get_byte(mud->packet_stream);
#endif

    printf("Login response: %d\n", response);

    if (response == 0 || response == 25) {
        mud->moderator_level = response == 25;
        mud->auto_login_timeout = 0;
        mudclient_reset_game(mud);
        return;
    }

    if (response == 1) {
        mud->auto_login_timeout = 0;
        return;
    }

    if (reconnecting) {
        // username[0] = '\0';
        // password[0] = '\0';
        mudclient_reset_login_screen(mud);
        return;
    }

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

    /*
    if (mud->auto_login_timeout > 0) {
        delay_ticks(5000);
        mud->auto_login_timeout--;
        mudclient_login(mud, username, password, reconnecting);
    }*/
}

void mudclient_registration_login(mudclient *mud) {
    char *username =
        panel_get_text(mud->panel_login_new_user, mud->control_register_user);

    char *password = panel_get_text(mud->panel_login_new_user,
                                    mud->control_register_password);

    mud->login_screen = 2;

    panel_update_text(mud->panel_login_existing_user, mud->control_login_status,
                      "Please enter your username and password");

    panel_update_text(mud->panel_login_existing_user, mud->control_login_user,
                      username);

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

#ifdef REVISION_177
    int session_id = packet_stream_get_int(mud->packet_stream);
    mud->session_id = session_id;

    printf("Session id: %d\n", mud->session_id);
#else
#endif

    packet_stream_new_packet(mud->packet_stream, CLIENT_REGISTER);
    packet_stream_put_short(mud->packet_stream, VERSION);

#ifdef REVISION_177
    packet_stream_put_long(mud->packet_stream,
                           encode_username(formatted_username));

    /* refer id */
    packet_stream_put_short(mud->packet_stream, 0);

    packet_stream_put_password(mud->packet_stream, session_id,
                               formatted_password);

    /* uid/randomDat */
    packet_stream_put_int(mud->packet_stream, 0);

    packet_stream_flush_packet(mud->packet_stream);

    packet_stream_get_byte(mud->packet_stream);
#else
#endif

    int response = packet_stream_get_byte(mud->packet_stream);
    printf("Newplayer response: %d\n", response);

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
    }
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

void mudclient_start_game(mudclient *mud) {
    mudclient_load_game_config(mud);

    if (mud->error_loading_data) {
        return;
    }

    mud->sprite_media = 2000;
    mud->sprite_util = mud->sprite_media + 100;
    mud->sprite_item = mud->sprite_util + 50;
    mud->sprite_logo = mud->sprite_item + 1000;
    mud->sprite_projectile = mud->sprite_logo + 10;
    mud->sprite_texture = mud->sprite_projectile + 50;
    mud->sprite_texture_world = mud->sprite_texture + 10;

    mudclient_set_target_fps(mud, 50);

    mud->surface = malloc(sizeof(Surface));

    surface_new(mud->surface, mud->game_width, mud->game_height + 12, 4000,
                mud);

    surface_set_bounds(mud->surface, 0, 0, mud->game_width,
                       mud->game_height + 12);

    panel_base_sprite_start = mud->sprite_util;

    int x = mud->surface->width2 - 199;
    int y = 36;

    mud->panel_quest_list = malloc(sizeof(Panel));
    panel_new(mud->panel_quest_list, mud->surface, 5);

    mud->control_list_quest = panel_add_text_list_interactive(
        mud->panel_quest_list, x, y + 24, 196, 251, 1, 500, 1);

    mud->panel_magic = malloc(sizeof(Panel));
    panel_new(mud->panel_magic, mud->surface, 5);

    mud->control_list_magic = panel_add_text_list_interactive(
        mud->panel_magic, x, y + 24, 196, 90, 1, 500, 1);

    mud->panel_social_list = malloc(sizeof(Panel));
    panel_new(mud->panel_social_list, mud->surface, 5);

    mud->control_list_social = panel_add_text_list_interactive(
        mud->panel_social_list, x, y + 40, 196, 126, 1, 500, 1);

    mudclient_load_media(mud);

    if (mud->error_loading_data) {
        return;
    }

    mudclient_load_entities(mud);

    if (mud->error_loading_data) {
        return;
    }

    mud->scene = malloc(sizeof(Scene));
    scene_new(mud->scene, mud->surface, 15000, 15000, 1000);

    scene_set_bounds(mud->scene, mud->game_width / 2, mud->game_height / 2,
                     mud->game_width / 2, mud->game_height / 2, mud->game_width,
                     9);

    mud->scene->clip_far_3d = 2400;
    mud->scene->clip_far_2d = 2400;
    mud->scene->fog_z_falloff = 1;
    mud->scene->fog_z_distance = 2300;

    // scene_set_light_from3(mud->scene, -50, -10, -50);

    mud->world = malloc(sizeof(World));
    world_new(mud->world, mud->scene, mud->surface);

    mud->world->base_media_sprite = mud->sprite_media;

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

    if (mud->members) {
        mudclient_load_sounds(mud);
    }

    if (mud->error_loading_data) {
        return;
    }

    mudclient_show_loading_progress(mud, 100, "Starting game...");
    mudclient_create_message_tabs_panel(mud);
    mudclient_create_login_panels(mud);
    mudclient_create_appearance_panel(mud);
    mudclient_reset_login_screen(mud);
    mudclient_render_login_screen_viewports(mud);

    free(surface_texture_pixels);
    surface_texture_pixels = NULL;
}

GameModel *mudclient_create_wall_object(mudclient *mud, int x, int y,
                                        int direction, int id, int count) {
    int x1 = x;
    int y1 = y;
    int x2 = x;
    int y2 = y;

    int front_texture = game_data_wall_object_texture_front[id];
    int back_texture = game_data_wall_object_texture_back[id];
    int height = game_data_wall_object_height[id];

    GameModel *game_model = malloc(sizeof(GameModel));
    game_model_from2(game_model, 4, 1);

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

    int *vertices = malloc(4 * sizeof(int));

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

    game_model_set_light_from6(game_model, 0, 60, 24, -50, -10, -50);

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

    if (mud->last_height_offset == mud->plane_index &&
        lx > mud->local_lower_x && lx < mud->local_upper_x &&
        ly > mud->local_lower_y && ly < mud->local_upper_y) {
        mud->world->player_alive = 1;
        return 0;
    }

    surface_draw_string_centre(mud->surface, "Loading... Please wait", 256, 192,
                               1, WHITE);

    mudclient_draw_chat_message_tabs(mud);
    surface_draw(mud->surface);

    int ax = mud->region_x;
    int ay = mud->region_y;
    int section_x = (lx + (REGION_SIZE / 2)) / REGION_SIZE;
    int section_y = (ly + (REGION_SIZE / 2)) / REGION_SIZE;

    mud->last_height_offset = mud->plane_index;
    mud->region_x = section_x * REGION_SIZE - REGION_SIZE;
    mud->region_y = section_y * REGION_SIZE - REGION_SIZE;
    mud->local_lower_x = section_x * REGION_SIZE - 32;
    mud->local_lower_y = section_y * REGION_SIZE - 32;
    mud->local_upper_x = section_x * REGION_SIZE + 32;
    mud->local_upper_y = section_y * REGION_SIZE + 32;

    world_load_section_from3(mud->world, lx, ly, mud->last_height_offset);

    mud->region_x -= mud->plane_width;
    mud->region_y -= mud->plane_height;

    int offset_x = mud->region_x - ax;
    int offset_y = mud->region_y - ay;

    for (int i = 0; i < mud->object_count; i++) {
        mud->object_x[i] -= offset_x;
        mud->object_y[i] -= offset_y;

        int object_x = mud->object_x[i];
        int object_y = mud->object_y[i];
        int object_id = mud->object_id[i];

        GameModel *game_model = mud->object_model[i];

        int object_direction = mud->object_direction[i];
        int obj_w = 0;
        int obj_h = 0;

        if (object_direction == 0 || object_direction == 4) {
            obj_w = game_data_object_width[object_id];
            obj_h = game_data_object_height[object_id];
        } else {
            obj_h = game_data_object_width[object_id];
            obj_w = game_data_object_height[object_id];
        }

        int base_x = ((object_x + object_x + obj_w) * MAGIC_LOC) / 2;
        int base_y = ((object_y + object_y + obj_h) * MAGIC_LOC) / 2;

        if (object_x >= 0 && object_y >= 0 && object_x < 96 && object_y < 96) {
            scene_add_model(mud->scene, game_model);

            game_model_place(game_model, base_x,
                             -world_get_elevation(mud->world, base_x, base_y),
                             base_y);

            world_remove_object2(mud->world, object_x, object_y, object_id);

            if (object_id == WINDMILL_SAILS_ID) {
                game_model_translate(game_model, 0, -480, 0);
            }
        }
    }

    for (int i = 0; i < mud->wall_object_count; i++) {
        mud->wall_object_x[i] -= offset_x;
        mud->wall_object_y[i] -= offset_y;

        int wall_obj_x = mud->wall_object_x[i];
        int wall_obj_y = mud->wall_object_y[i];
        int wall_obj_id = mud->wall_object_id[i];
        int wall_obj_dir = mud->wall_object_direction[i];

        world_set_object_adjacency_from4(mud->world, wall_obj_x, wall_obj_y,
                                         wall_obj_dir, wall_obj_id);

        game_model_destroy(mud->wall_object_model[i]);
        free(mud->wall_object_model[i]);

        GameModel *wall_object_model = mudclient_create_wall_object(
            mud, wall_obj_x, wall_obj_y, wall_obj_dir, wall_obj_id, i);

        mud->wall_object_model[i] = wall_object_model;
    }

    for (int i = 0; i < mud->ground_item_count; i++) {
        mud->ground_item_x[i] -= offset_x;
        mud->ground_item_y[i] -= offset_y;
    }

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
            free(mud->local_player);
            mud->local_player = NULL;
        }

        GameCharacter *character = malloc(sizeof(GameCharacter));
        game_character_new(character);

        character_server[server_index] = character;
        character_server[server_index]->server_index = server_index;
        // character_server[server_index]->server_id = 0; - should already be 0
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
        character->animation_next = animation;
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
        character->animation_current = animation;
        character->animation_next = animation;
        character->step_count = 0;
    }

    return character;
}

GameCharacter *mudclient_add_player(mudclient *mud, int server_index, int x,
                                    int y, int animation) {
    GameCharacter *player = mudclient_add_character(
        mud, mud->player_server, mud->known_players, mud->known_player_count,
        server_index, x, y, animation, -1);

    mud->players[mud->player_count++] = player;

    return player;
}

GameCharacter *mudclient_add_npc(mudclient *mud, int server_index, int x, int y,
                                 int animation, int npc_id) {
    GameCharacter *npc = mudclient_add_character(
        mud, mud->npcs_server, mud->known_npcs, mud->known_npc_count,
        server_index, x, y, animation, npc_id);

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

        for (int i = 0; i < mud->bank_item_count; i++) {
            if (mud->bank_items[i] == inventory_id) {
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

void mudclient_move_character(mudclient *mud, GameCharacter *character) {
    int next_waypoint = (character->waypoint_current + 1) % 10;

    if (character->moving_step != next_waypoint) {
        int animation_current = -1;
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
                animation_current = 2;
            } else if (character->current_x > character->waypoints_x[step]) {
                character->current_x -= j5;
                character->step_count++;
                animation_current = 6;
            }

            if (character->current_x - character->waypoints_x[step] < j5 &&
                character->current_x - character->waypoints_x[step] > -j5) {
                character->current_x = character->waypoints_x[step];
            }

            if (character->current_y < character->waypoints_y[step]) {
                character->current_y += j5;
                character->step_count++;

                if (animation_current == -1) {
                    animation_current = 4;
                } else if (animation_current == 2) {
                    animation_current = 3;
                } else {
                    animation_current = 5;
                }
            } else if (character->current_y > character->waypoints_y[step]) {
                character->current_y -= j5;
                character->step_count++;

                if (animation_current == -1) {
                    animation_current = 0;
                } else if (animation_current == 2) {
                    animation_current = 1;
                } else {
                    animation_current = 7;
                }
            }

            if (character->current_y - character->waypoints_y[step] < j5 &&
                character->current_y - character->waypoints_y[step] > -j5) {
                character->current_y = character->waypoints_y[step];
            }
        }

        if (animation_current != -1) {
            character->animation_current = animation_current;
        }

        if (character->current_x == character->waypoints_x[step] &&
            character->current_y == character->waypoints_y[step]) {
            character->moving_step = (step + 1) % 10;
        }
    } else {
        character->animation_current = character->animation_next;

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

void mudclient_handle_game_input(mudclient *mud) {
#ifndef REVISION_177
    if (mud->system_update > 1) {
        mud->system_update--;
    }
#endif

    mudclient_packet_tick(mud);

    if (mud->logout_timeout > 0) {
        mud->logout_timeout--;
    }

    /* Logg told me to disable this :)
    if (mud->mouse_action_timeout > 4500 && mud->combat_timeout == 0 &&
        mud->logout_timeout == 0) {
        mud->mouse_action_timeout -= 500;
        mudclient_send_logout(mud);
        return;
    }*/

    if (mud->local_player->animation_current == 8 ||
        mud->local_player->animation_current == 9) {
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
        mudclient_move_character(mud, mud->players[i]);
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
        mudclient_move_character(mud, mud->npcs[i]);
    }

    if (mud->show_ui_tab != 2) {
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
            mud->an_int_707++;

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
                ((mud->an_int_707 * j3 + 255) / 256) * direction;

            mud->camera_rotation &= 0xff;
        } else {
            mud->an_int_707 = 0;
        }
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

    if (mud->show_dialog_trade || mud->show_dialog_duel) {
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

    scene_set_mouse_loc(mud->scene, mud->mouse_x, mud->mouse_y);

    mud->last_mouse_button_down = 0;

    if (mud->settings_camera_auto) {
        if (mud->an_int_707 == 0) {
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

    if (!mud->settings_camera_auto && mud->options->middle_click_camera &&
        mud->middle_button_down) {
        mud->camera_rotation = (mud->origin_rotation +
                                ((mud->mouse_x - mud->origin_mouse_x) / 2)) &
                               0xff;
    }

    if (mud->options->zoom_camera) {
        // mudclient_handle_camera_zoom(mud);
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

    scene_scroll_texture(mud->scene, FOUNTATION_ID);

    mud->object_animation_count++;

    if (mud->object_animation_count > 5) {
        mud->object_animation_count = 0;
        mud->object_animation_cycle = (mud->object_animation_cycle + 1) % 3;
        mud->torch_animation_cycle = (mud->torch_animation_cycle + 1) % 4;
        mud->claw_animation_cycle = (mud->claw_animation_cycle + 1) % 5;
    }

    for (int i = 0; i < mud->object_count; i++) {
        int x = mud->object_x[i];
        int y = mud->object_y[i];

        if (x >= 0 && y >= 0 && x < 96 && y < 96 &&
            mud->object_id[i] == WINDMILL_SAILS_ID) {
            game_model_rotate(mud->object_model[i], 1, 0, 0);
        }
    }

    for (int i = 0; i < mud->teleport_bubble_count; i++) {
        mud->teleport_bubble_time[i]++;

        if (mud->teleport_bubble_time[i] > 50) {
            mud->teleport_bubble_count--;

            for (int j = i; j < mud->teleport_bubble_count; j++) {
                mud->teleport_bubble_x[j] = mud->teleport_bubble_x[j + 1];
                mud->teleport_bubble_y[j] = mud->teleport_bubble_y[j + 1];
                mud->teleport_bubble_time[j] = mud->teleport_bubble_time[j + 1];
                mud->teleport_bubble_type[j] = mud->teleport_bubble_type[j + 1];
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
        mudclient_handle_game_input(mud);
    }

    mud->last_mouse_button_down = 0;
    mud->camera_rotation_time++;

    if (mud->camera_rotation_time > 500) {
        mud->camera_rotation_time = 0;

        float r = (float)rand() / (float)RAND_MAX;
        int roll = (int)(r * 4);

        if ((roll & 1) == 1) {
            mud->camera_rotation_x += mud->camera_rotation_x_increment;
        }

        if ((roll & 2) == 2) {
            mud->camera_rotation_y += mud->camera_rotation_y_increment;
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
    int object_x = mud->object_x[object_index];
    int object_y = mud->object_y[object_index];
    int distance_x = object_x - (mud->local_player->current_x / 128);
    int distance_y = object_y - (mud->local_player->current_y / 128);
    int max_distance = 7;

    if (object_x >= 0 && object_y >= 0 && object_x < 96 && object_y < 96 &&
        distance_x > -max_distance && distance_x < max_distance &&
        distance_y > -max_distance && distance_y < max_distance) {
        scene_remove_model(mud->scene, mud->object_model[object_index]);

        int model_index = game_data_get_model_index(model_name);
        GameModel *game_model = game_model_copy(mud->game_models[model_index]);

        scene_add_model(mud->scene, game_model);

        game_model_set_light_from6(game_model, 1, 48, 48, -50, -10, -50);
        game_model_copy_position(game_model, mud->object_model[object_index]);

        game_model->key = object_index;

        game_model_destroy(mud->object_model[object_index]);
        free(mud->object_model[object_index]);

        mud->object_model[object_index] = game_model;
    }
}

void mudclient_draw_character_message(mudclient *mud, GameCharacter *character,
                                      int x, int y, int width) {
    if (character->message_timeout <= 0) {
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
                                     int height, int is_npc) {
    if (character->animation_current == 8 ||
        character->animation_current == 9 || character->combat_timer != 0) {
        if (character->combat_timer > 0) {
            int offset_x = x;

            if (character->animation_current == 8) {
                offset_x -= (20 * ty) / 100;
            } else if (character->animation_current == 9) {
                offset_x += (20 * ty) / 100;
            }

            int missing =
                (character->health_current * 30) / character->health_max;

            mud->health_bar_x[mud->health_bar_count] = offset_x + (width / 2);
            mud->health_bar_y[mud->health_bar_count] = y;
            mud->health_bar_missing[mud->health_bar_count++] = missing;
        }

        if (character->combat_timer > 150) {
            int offset_x = x;

            if (character->animation_current == 8) {
                offset_x -= (10 * ty) / 100;
            } else if (character->animation_current == 9) {
                offset_x += (10 * ty) / 100;
            }

            surface_draw_sprite_from3(
                mud->surface, (offset_x + (width / 2)) - 12,
                (y + (height / 2)) - 12,
                mud->sprite_media + 11 + (is_npc ? 1 : 0));

            char damage_string[12] = {0};
            sprintf(damage_string, "%d", character->damage_taken);

            surface_draw_string_centre(mud->surface, damage_string,
                                       (offset_x + (width / 2)) - 1,
                                       y + (height / 2) + 5, 3, WHITE);
        }
    }
}

void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int tx, int ty) {
    GameCharacter *player = mud->players[id];

    if (player->colour_bottom == 255) {
        return;
    }

    int animation_order =
        (player->animation_current + (mud->camera_rotation + 16) / 32) & 7;

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

    if (player->animation_current == 8) {
        i2 = 5;
        animation_order = 2;
        flip = 0;
        x -= (5 * ty) / 100;
        j2 = i2 * 3 + character_combat_model_array1[(mud->login_timer / 5) % 8];
    } else if (player->animation_current == 9) {
        i2 = 5;
        animation_order = 2;
        flip = 1;
        x += (5 * ty) / 100;
        j2 = i2 * 3 + character_combat_model_array2[(mud->login_timer / 6) % 8];
    }

    /* TODO make 12 a constant */
    for (int i = 0; i < 12; i++) {
        int animation_index = character_animation_array[animation_order][i];
        int l3 = player->equipped_item[animation_index] - 1;

        if (l3 >= 0) {
            int offset_x = 0;
            int offset_y = 0;
            int j5 = j2;

            if (flip && i2 >= 1 && i2 <= 3) {
                if (game_data_animation_has_f[l3] == 1) {
                    j5 += 15;
                } else if (animation_index == 4 && i2 == 1) {
                    offset_x = -22;
                    offset_y = -3;

                    j5 = i2 * 3 +
                         character_walk_model[(2 + (player->step_count / 6)) %
                                              4];
                } else if (animation_index == 4 && i2 == 2) {
                    offset_x = 0;
                    offset_y = -8;

                    j5 = i2 * 3 +
                         character_walk_model[(2 + (player->step_count / 6)) %
                                              4];
                } else if (animation_index == 4 && i2 == 3) {
                    offset_x = 26;
                    offset_y = -5;

                    j5 = i2 * 3 +
                         character_walk_model[(2 + (player->step_count / 6)) %
                                              4];
                } else if (animation_index == 3 && i2 == 1) {
                    offset_x = 22;
                    offset_y = 3;

                    j5 = i2 * 3 +
                         character_walk_model[(2 + (player->step_count / 6)) %
                                              4];
                } else if (animation_index == 3 && i2 == 2) {
                    offset_x = 0;
                    offset_y = 8;

                    j5 = i2 * 3 +
                         character_walk_model[(2 + (player->step_count / 6)) %
                                              4];
                } else if (animation_index == 3 && i2 == 3) {
                    offset_x = -26;
                    offset_y = 5;

                    j5 = i2 * 3 +
                         character_walk_model[(2 + (player->step_count / 6)) %
                                              4];
                }
            }

            if (i2 != 5 || game_data_animation_has_a[l3] == 1) {
                int sprite_id = j5 + game_data_animation_number[l3];

                offset_x = (offset_x * width) /
                           mud->surface->sprite_width_full[sprite_id];

                offset_y = (offset_y * height) /
                           mud->surface->sprite_height_full[sprite_id];

                int clip_width =
                    (width * mud->surface->sprite_width_full[sprite_id]) /
                    mud->surface
                        ->sprite_width_full[game_data_animation_number[l3]];

                offset_x -= (clip_width - width) / 2;

                int animation_colour = game_data_animation_character_colour[l3];
                int skin_colour = player_skin_colours[player->colour_skin];

                if (animation_colour == 1) {
                    animation_colour = player_hair_colours[player->colour_hair];
                } else if (animation_colour == 2) {
                    animation_colour =
                        player_top_bottom_colours[player->colour_top];
                } else if (animation_colour == 3) {
                    animation_colour =
                        player_top_bottom_colours[player->colour_bottom];
                }

                surface_sprite_clipping_from9(
                    mud->surface, x + offset_x, y + offset_y, clip_width,
                    height, sprite_id, animation_colour, skin_colour, tx, flip);
            }
        }
    }

    mudclient_draw_character_message(mud, player, x, y, width);

    if (player->bubble_timeout > 0) {
        mud->action_bubble_x[mud->action_bubble_count] = x + (width / 2);
        mud->action_bubble_y[mud->action_bubble_count] = y;
        mud->action_bubble_scale[mud->action_bubble_count] = ty;

        mud->action_bubble_item[mud->action_bubble_count++] =
            player->bubble_item;
    }

    mudclient_draw_character_damage(mud, player, x, y, ty, width, height, 0);

    if (player->skull_visible == 1 && player->bubble_timeout == 0) {
        int k3 = tx + x + (width / 2);

        if (player->animation_current == 8) {
            k3 -= (20 * ty) / 100;
        } else if (player->animation_current == 9) {
            k3 += (20 * ty) / 100;
        }

        int j4 = (16 * ty) / 100;
        int l4 = (16 * ty) / 100;

        surface_sprite_clipping_from5(mud->surface, k3 - (j4 / 2),
                                      y - (l4 / 2) - ((10 * ty) / 100), j4, l4,
                                      mud->sprite_media + 13);
    }
}

void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int tx, int ty) {
    GameCharacter *npc = mud->npcs[id];
    int l1 = (npc->animation_current + (mud->camera_rotation + 16) / 32) & 7;
    int flip = 0;
    int i2 = l1;

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
                                       game_data_npc_walk_model[npc->npc_id]) %
                                      4];

    if (npc->animation_current == 8) {
        i2 = 5;
        l1 = 2;
        flip = 0;
        x -= (game_data_npc_combat_animation[npc->npc_id] * ty) / 100;
        // nope
        j2 = i2 * 3 + character_combat_model_array1
                          [((mud->login_timer /
                                 (game_data_npc_combat_model[npc->npc_id]) -
                             1)) %
                           8];
    } else if (npc->animation_current == 9) {
        i2 = 5;
        l1 = 2;
        flip = 1;
        x += (game_data_npc_combat_animation[npc->npc_id] * ty) / 100;

        j2 = i2 * 3 +
             character_combat_model_array2
                 [(mud->login_timer / game_data_npc_combat_model[npc->npc_id]) %
                  8];
    }

    for (int i = 0; i < 12; i++) {
        int l2 = character_animation_array[l1][i];
        int animation_id = game_data_npc_sprite[npc->npc_id][l2];

        if (animation_id >= 0) {
            int offset_x = 0;
            int offset_y = 0;
            int k4 = j2;

            if (flip && i2 >= 1 && i2 <= 3 &&
                game_data_animation_has_f[animation_id] == 1) {
                k4 += 15;
            }

            if (i2 != 5 || game_data_animation_has_a[animation_id] == 1) {
                int sprite_id = k4 + game_data_animation_number[animation_id];

                offset_x = (offset_x * width) /
                           mud->surface->sprite_width_full[sprite_id];

                offset_y = (offset_y * height) /
                           mud->surface->sprite_height_full[sprite_id];

                int clip_width =
                    (width * mud->surface->sprite_width_full[sprite_id]) /
                    mud->surface->sprite_width_full
                        [game_data_animation_number[animation_id]];

                offset_x -= (clip_width - width) / 2;

                int animation_colour =
                    game_data_animation_character_colour[animation_id];

                int skin_colour = 0;

                if (animation_colour == 1) {
                    animation_colour = game_data_npc_colour_hair[npc->npc_id];
                    skin_colour = game_data_npc_colour_skin[npc->npc_id];
                } else if (animation_colour == 2) {
                    animation_colour = game_data_npc_colour_top[npc->npc_id];
                    skin_colour = game_data_npc_colour_skin[npc->npc_id];
                } else if (animation_colour == 3) {
                    animation_colour = game_data_npc_color_bottom[npc->npc_id];
                    skin_colour = game_data_npc_colour_skin[npc->npc_id];
                }

                surface_sprite_clipping_from9(
                    mud->surface, x + offset_x, y + offset_y, clip_width,
                    height, sprite_id, animation_colour, skin_colour, tx, flip);
            }
        }
    }

    mudclient_draw_character_message(mud, npc, x, y, width);
    mudclient_draw_character_damage(mud, npc, x, y, ty, width, height, 1);
}

void mudclient_draw_ui(mudclient *mud) {
    if (mud->logout_timeout != 0) {
        mudclient_draw_logout(mud);
    } else if (mud->show_dialog_welcome) {
        mudclient_draw_welcome(mud);
    } else if (mud->show_dialog_server_message) {
        mudclient_draw_server_message(mud);
    } else if (mud->show_ui_wild_warn == 1) {
        mudclient_draw_wilderness_warning(mud);
    } else if (mud->show_dialog_bank && mud->combat_timeout == 0) {
        mudclient_draw_bank(mud);
    } else if (mud->show_dialog_shop && mud->combat_timeout == 0) {
        mudclient_draw_shop(mud);
    } else if (mud->show_dialog_trade_confirm) {
        mudclient_draw_trade_confirm(mud);
    } else if (mud->show_dialog_trade) {
        mudclient_draw_trade(mud);
    } else if (mud->show_dialog_duel_confirm) {
        mudclient_draw_duel_confirm(mud);
    } else if (mud->show_dialog_duel) {
        mudclient_draw_duel(mud);
        /*} else if (mud->show_dialog_report_abuse_step == 1) {
            mudclient_draw_dialog_report_abuse(mud);
        } else if (mud->show_dialog_report_abuse_step == 2) {
            mudclient_draw_dialog_report_abuse_input(mud);*/
    } else if (mud->show_change_password_step != 0) {
        mudclient_draw_change_password(mud);
    } else if (mud->show_dialog_social_input != 0) {
        mudclient_draw_social_input(mud);
    } else {
        if (mud->show_option_menu) {
            mudclient_draw_option_menu(mud);
        }

        if (mud->local_player->animation_current == 8 ||
            mud->local_player->animation_current == 9) {
            mudclient_draw_combat_style(mud);
        }

        mudclient_set_active_ui_tab(mud);

        int no_menus = !mud->show_option_menu && !mud->show_right_click_menu;

        if (no_menus) {
            mud->menu_items_count = 0;
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
            }
        }
    }

    mud->mouse_button_click = 0;
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

        surface_draw_paragraph(mud->surface, mud->received_messages[i], x, y, 1,
                               YELLOW, 300);
    }

    for (int i = 0; i < mud->action_bubble_count; i++) {
        int x = mud->action_bubble_x[i];
        int y = mud->action_bubble_y[i];
        int scale = mud->action_bubble_scale[i];
        int id = mud->action_bubble_item[i];
        int scale_x = (39 * scale) / 100;
        int scale_y = (27 * scale) / 100;

        surface_draw_action_bubble(mud->surface, x - (scale_x / 2), y - scale_y,
                                   scale_x, scale_y, mud->sprite_media + 9, 85);

        int scale_x_clip = (36 * scale) / 100;
        int scale_y_clip = (24 * scale) / 100;

        surface_sprite_clipping_from9(
            mud->surface, x - (scale_x_clip / 2),
            (y - scale_y + (scale_y / 2)) - (scale_y_clip / 2), scale_x_clip,
            scale_y_clip, game_data_item_picture[id] + mud->sprite_item,
            game_data_item_mask[id], 0, 0, 0);
    }

    for (int i = 0; i < mud->health_bar_count; i++) {
        int x = mud->health_bar_x[i];
        int y = mud->health_bar_y[i];
        int missing = mud->health_bar_missing[i];

        surface_draw_box_alpha(mud->surface, x - 15, y - 3, missing, 5, GREEN,
                               192);

        surface_draw_box_alpha(mud->surface, (x - 15) + missing, y - 3,
                               30 - missing, 5, RED, 192);
    }
}

void mudclient_draw_game(mudclient *mud) {
    if (mud->death_screen_timeout != 0) {
        surface_fade_to_black(mud->surface);

        surface_draw_string_centre(mud->surface, "Oh dear! You are dead...",
                                   mud->game_width / 2, mud->game_height / 2, 7,
                                   RED);

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

#if 1
    for (int i = 0; i < TERRAIN_COUNT; i++) {
        scene_remove_model(mud->scene,
                           mud->world->roof_models[mud->last_height_offset][i]);

        if (mud->last_height_offset == 0) {
            scene_remove_model(mud->scene, mud->world->wall_models[1][i]);
            scene_remove_model(mud->scene, mud->world->roof_models[1][i]);
            scene_remove_model(mud->scene, mud->world->wall_models[2][i]);
            scene_remove_model(mud->scene, mud->world->roof_models[2][i]);
        }

        if (mud->options->show_roofs) {
            mud->fog_of_war = 1;

            if (mud->last_height_offset == 0 &&
                (mud->world
                     ->object_adjacency[mud->local_player->current_x / 128]
                                       [mud->local_player->current_y / 128] &
                 128) == 0) {
                scene_add_model(
                    mud->scene,
                    mud->world->roof_models[mud->last_height_offset][i]);

                if (mud->last_height_offset == 0) {
                    scene_add_model(mud->scene, mud->world->wall_models[1][i]);
                    scene_add_model(mud->scene, mud->world->roof_models[1][i]);
                    scene_add_model(mud->scene, mud->world->wall_models[2][i]);
                    scene_add_model(mud->scene, mud->world->roof_models[2][i]);
                }

                mud->fog_of_war = 0;
            }
        }
    }

    /* TODO move to animate_objects */
    if (mud->object_animation_cycle != mud->last_object_animation_cycle) {
        mud->last_object_animation_cycle = mud->object_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->object_id[i] == FIRE_ID) {
                char name[17];
                sprintf(name, "firea%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == FIREPLACE_ID) {
                char name[22];
                sprintf(name, "fireplacea%d",
                        (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == LIGHTNING_ID) {
                char name[21];
                sprintf(name, "lightning%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == FIRE_SPELL_ID) {
                char name[21];
                sprintf(name, "firespell%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == SPELL_CHARGE_ID) {
                char name[23];
                sprintf(name, "spellcharge%d",
                        (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    if (mud->torch_animation_cycle != mud->last_torch_animation_cycle) {
        mud->last_torch_animation_cycle = mud->torch_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->object_id[i] == TORCH_ID) {
                char name[18];
                sprintf(name, "torcha%d", mud->torch_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == SKULL_TORCH_ID) {
                char name[23];
                sprintf(name, "skulltorcha%d", mud->torch_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    if (mud->claw_animation_cycle != mud->last_claw_animation_cycle) {
        mud->last_claw_animation_cycle = mud->claw_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->object_id[i] == CLAW_SPELL_ID) {
                char name[21];
                sprintf(name, "clawspell%d", mud->claw_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    /* TODO move to draw_entity_sprites */

    scene_reduce_sprites(mud->scene, mud->scene_sprite_count);
    mud->scene_sprite_count = 0;

    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        if (player->colour_bottom != 255) {
            int x = player->current_x;
            int y = player->current_y;
            int elevation = -world_get_elevation(mud->world, x, y);

            int id = scene_add_sprite(mud->scene, 5000 + i, x, elevation, y,
                                      145, 220, i + 10000);

            mud->scene_sprite_count++;

            if (player == mud->local_player) {
                scene_set_local_player(mud->scene, id);
            }

            if (player->animation_current == 8) {
                scene_set_sprite_translate_x(mud->scene, id, -30);
            }

            if (player->animation_current == 9) {
                scene_set_sprite_translate_x(mud->scene, id, 30);
            }
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

                int delev = -(world_get_elevation(mud->world, dx, dy) -
                              game_data_npc_height[character->npc_id] / 2);

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

        int sprite =
            scene_add_sprite(mud->scene, 20000 + i, x, elevation, y,
                             game_data_npc_width[npc->npc_id],
                             game_data_npc_height[npc->npc_id], i + 30000);

        mud->scene_sprite_count++;

        if (npc->animation_current == 8) {
            scene_set_sprite_translate_x(mud->scene, sprite, -30);
        } else if (npc->animation_current == 9) {
            scene_set_sprite_translate_x(mud->scene, sprite, 30);
        }
    }

    for (int i = 0; i < mud->ground_item_count; i++) {
        int x = mud->ground_item_x[i] * MAGIC_LOC + 64;
        int y = mud->ground_item_y[i] * MAGIC_LOC + 64;

        scene_add_sprite(mud->scene, 40000 + mud->ground_item_id[i], x,
                         -world_get_elevation(mud->world, x, y) -
                             mud->ground_item_z[i],
                         y, 96, 64, i + 20000);

        mud->scene_sprite_count++;
    }

    for (int i = 0; i < mud->teleport_bubble_count; i++) {
        int x = mud->teleport_bubble_x[i] * MAGIC_LOC + 64;
        int y = mud->teleport_bubble_y[i] * MAGIC_LOC + 64;
        int type = mud->teleport_bubble_type[i];
        int height = type == 0 ? 256 : 64;

        scene_add_sprite(mud->scene, 50000 + i, x,
                         -world_get_elevation(mud->world, x, y), y, 128, height,
                         i + 50000);

        mud->scene_sprite_count++;
    }

    mud->surface->interlace = 0;
    surface_black_screen(mud->surface);
    mud->surface->interlace = mud->interlace;

    if (mud->last_height_offset == 3) {
        int ambience = 40 + ((float)rand() / (float)RAND_MAX) * 3;
        int diffuse = 40 + ((float)rand() / (float)RAND_MAX) * 7;

        scene_set_light_from5(mud->scene, ambience, diffuse, -50, -10, -50);
    }

    mud->action_bubble_count = 0;
    mud->received_messages_count = 0;
    mud->health_bar_count = 0;

    if (mud->settings_camera_auto && !mud->fog_of_war) {
        mudclient_auto_rotate_camera(mud);
    }

    if (!mud->interlace) {
        mud->scene->clip_far_3d = 2400;
        mud->scene->clip_far_2d = 2400;
        mud->scene->fog_z_falloff = 1;
        mud->scene->fog_z_distance = 2300;
    } else {
        mud->scene->clip_far_3d = 2200;
        mud->scene->clip_far_2d = 2200;
        mud->scene->fog_z_falloff = 1;
        mud->scene->fog_z_distance = 2100;
    }

    if (mud->camera_zoom > ZOOM_OUTDOORS) {
        mud->scene->clip_far_3d += 1400;
        mud->scene->clip_far_2d += 1400;
        mud->scene->fog_z_distance += 1400;
    }

    int x = mud->camera_auto_rotate_player_x + mud->camera_rotation_x;
    int y = mud->camera_auto_rotate_player_y + mud->camera_rotation_y;

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, mud->camera_rotation * 4, 0, mud->camera_zoom * 2);
#endif
    surface_black_screen(mud->surface);

#ifdef RENDER_SW
    mud->scene->camera_x += test_x;
    mud->scene->camera_y += test_y;
    mud->scene->camera_z += test_z;

    scene_render(mud->scene);

    mud->scene->camera_x -= test_x;
    mud->scene->camera_y -= test_y;
    mud->scene->camera_z -= test_z;
#endif

    mudclient_draw_overhead(mud);

    /* draw the animated X sprite when clicking */
    if (mud->mouse_click_x_step > 0) {
        surface_draw_sprite_from3(
            mud->surface, mud->mouse_click_x_x - 8, mud->mouse_click_x_y - 8,
            mud->sprite_media + 14 + ((24 - mud->mouse_click_x_step) / 6));
    } else if (mud->mouse_click_x_step < 0) {
        surface_draw_sprite_from3(
            mud->surface, mud->mouse_click_x_x - 8, mud->mouse_click_x_y - 8,
            mud->sprite_media + 18 + ((24 + mud->mouse_click_x_step) / 6));
    }

    if (mud->options->fps_counter) {
        int offset_x = mud->is_in_wild ? 70 : 0;

        char fps[17] = {0};
        sprintf(fps, "Fps: %d", mud->fps);

        surface_draw_string(mud->surface, fps, mud->game_width - 62 - offset_x,
                            mud->game_height - 10, 1, YELLOW);
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
                                   mud->game_height - 7, 1, YELLOW);
    }
#endif

    if (!mud->loading_area) {
        int wilderness_depth =
            2203 - (mud->local_region_y + mud->plane_height + mud->region_y);

        if (mud->local_region_x + mud->plane_width + mud->region_x >= 2640) {
            wilderness_depth = -50;
        }

        mud->is_in_wild = wilderness_depth > 0;

        if (mud->is_in_wild) {
            surface_draw_sprite_from3(mud->surface, mud->game_width - 59,
                                      mud->game_height - 56,
                                      mud->sprite_media + 13);

            surface_draw_string_centre(mud->surface, "Wilderness",
                                       mud->game_width - 47,
                                       mud->game_height - 20, 1, YELLOW);

            int wilderness_level = 1 + (wilderness_depth / 6);

            char formatted_level[19];
            sprintf(formatted_level, "Level: %d", wilderness_level);

            surface_draw_string_centre(mud->surface, formatted_level,
                                       mud->game_width - 47,
                                       mud->game_height - 7, 1, YELLOW);

            if (mud->show_ui_wild_warn == 0) {
                mud->show_ui_wild_warn = 2;
            }
        }

        if (mud->show_ui_wild_warn == 0 && wilderness_depth > -10 &&
            wilderness_depth <= 0) {
            mud->show_ui_wild_warn = 1;
        }
    }

    mudclient_draw_chat_message_tabs_panel(mud);

    /* ui tabs */
    surface_draw_sprite_alpha_from4(mud->surface,
                                    mud->surface->width2 - 3 - 197, 3,
                                    mud->sprite_media, 128);

    mudclient_draw_ui(mud);

    mud->surface->logged_in = 0;

    mudclient_draw_chat_message_tabs(mud);

    surface_draw(mud->surface);
}

void mudclient_draw(mudclient *mud) {
    if (mud->error_loading_data) {
        /* TODO draw error */
        // printf("ERROR LOADING DATA\n");
        return;
    }

#ifdef WII
    draw_background(mud->framebuffer, 0);
#endif

#ifdef RENDER_GL
    // if (!mud->surface->fade_to_black) {
    glClear(GL_COLOR_BUFFER_BIT);
    // }
#endif

    if (mud->logged_in == 0) {
        mud->surface->logged_in = 0;
        mudclient_draw_login_screens(mud);
    } else if (mud->logged_in == 1) {
        mud->surface->logged_in = 1;
        mudclient_draw_game(mud);
    }

#ifdef WII
    if (mud->keyboard_open) {
        draw_keyboard(mud->framebuffer, mud->keyboard_open == 2 ? 1 : 0);
    }

    draw_arrow(mud->framebuffer, mud->last_wii_x, mud->last_wii_y);
    VIDEO_SetNextFramebuffer(mud->framebuffer);
    mud->active_framebuffer ^= 1;
    mud->framebuffer = mud->framebuffers[mud->active_framebuffer];
    VIDEO_Flush();

    if (mud->keyboard_open) {
        VIDEO_WaitVSync();
    }

    // VIDEO_WaitVSync(); /* TODO investigate */
#endif
}

void mudclient_poll_events(mudclient *mud) {
#ifdef WII
    /* handle USB keyboard */
    keyboard_event ke;

    int res = KEYBOARD_GetEvent(&ke);

    if (res) {
        if (ke.type == KEYBOARD_PRESSED) {
            int code = ke.keycode;
            int char_code = ke.symbol;

            if (char_code == 62342) {
                code = K_LEFT;
                char_code = -1;
            } else if (char_code == 62343) {
                code = K_RIGHT;
                char_code = -1;
            } else if (char_code == 62208) {
                code = K_F1;
                char_code = -1;
            } else if (char_code == 106) {
                code = 106;
            } else if (char_code == 101) {
                code = 101;
            } else if (char_code == 48) {
                code = 48;
            } else if (char_code == 13) {
                code = 13;
            } else if (char_code == 8) {
                code = 8;
            }

            mud->last_keyboard_button = code;

            mudclient_key_pressed(mud, code, char_code);
        } else if (ke.type == KEYBOARD_RELEASED) {
            int code = ke.keycode;
            int char_code = ke.symbol;

            mud->last_keyboard_button = -1;

            if (char_code == 62342) {
                code = K_LEFT;
            } else if (char_code == 62343) {
                code = K_RIGHT;
            }

            mudclient_key_released(mud, code);
        }
    }

    WPAD_ReadPending(WPAD_CHAN_ALL, NULL);
    res = WPAD_Probe(0, NULL);

    if (res != WPAD_ERR_NONE) {
        update_wii_mouse(NULL);
    } else {
        WPADData *wiimote_data = WPAD_Data(0);

        if (mud->last_keyboard_button == -1) {
            if (!mud->key_left && wiimote_data->btns_h & WPAD_BUTTON_LEFT) {
                mudclient_key_pressed(mud, K_LEFT, -1);
            } else if (mud->key_left &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_LEFT)) {
                mudclient_key_released(mud, K_LEFT);
            }

            if (!mud->key_right && wiimote_data->btns_h & WPAD_BUTTON_RIGHT) {
                mudclient_key_pressed(mud, K_RIGHT, -1);
            } else if (mud->key_right &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_RIGHT)) {
                mudclient_key_released(mud, K_RIGHT);
            }
        }

        /* minus - open keyboard */
        if (mud->last_keyboard_button == -1 &&
            wiimote_data->btns_h & WPAD_BUTTON_MINUS) {
            mud->keyboard_open = !mud->keyboard_open;
            mud->last_keyboard_button = 1;
        } else if (mud->last_keyboard_button == 1 &&
                   !(wiimote_data->btns_h & WPAD_BUTTON_MINUS)) {
            mud->last_keyboard_button = -1;
        }

        /* enter */
        if (mud->last_wii_button != K_ENTER &&
            wiimote_data->btns_h & WPAD_BUTTON_PLUS) {
            mudclient_key_pressed(mud, K_ENTER, K_ENTER);
            mudclient_key_released(mud, K_ENTER);
            mud->last_wii_button = K_ENTER;
        } else if (mud->last_wii_button == K_ENTER &&
                   !(wiimote_data->btns_h & WPAD_BUTTON_PLUS)) {
            mud->last_wii_button = 0;
        }

        if (mud->last_wii_button != K_F1 &&
            wiimote_data->btns_h & WPAD_BUTTON_1) {
            mudclient_key_pressed(mud, K_F1, -1);
            mudclient_key_released(mud, K_F1);
            mud->last_wii_button = K_F1;
        } else if (mud->last_wii_button == K_F1 &&
                   !(wiimote_data->btns_h & WPAD_BUTTON_1)) {
            mud->last_wii_button = 0;
        }

        if (mud->keyboard_open) {
            /* backspace */
            if (mud->last_wii_button != K_BACKSPACE &&
                wiimote_data->btns_h & WPAD_BUTTON_B) {
                mudclient_key_pressed(mud, K_BACKSPACE, K_BACKSPACE);
                mudclient_key_released(mud, K_BACKSPACE);
                mud->last_wii_button = K_BACKSPACE;
            } else if (mud->last_wii_button == K_BACKSPACE &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_B)) {
                mud->last_wii_button = 0;
            }
        }

        update_wii_mouse(wiimote_data);
    }

    /* menu exit */
    if (wii_mouse_y <= 45 && wii_mouse_x >= 234 && wii_mouse_x <= 396 &&
        wii_mouse_button != 0) {
        exit(0);
    }

    if (mud->keyboard_open) {
        mud->last_wii_x = wii_mouse_x;
        mud->last_wii_y = wii_mouse_y;

        int keyboard_x = 52 + 16;
        int keyboard_y = 200 + 16;

        int key_width = KEY_WIDTH * 2;
        int key_height = KEY_HEIGHT * 2;
        int keyboard_width = key_width * 10;
        int keyboard_height = key_height * 5;

        if (mud->last_wii_button != 1 && wii_mouse_button == 1) {
            mud->last_wii_button = 1;

            if (wii_mouse_x >= keyboard_x &&
                wii_mouse_x <= (keyboard_x + (key_width * 2)) &&
                wii_mouse_y >= (keyboard_y + (key_height * 4)) &&
                wii_mouse_y <= (keyboard_y + (key_height * 5))) {
                mud->keyboard_open = mud->keyboard_open == 2 ? 1 : 2;
                return;
            }

            for (int y = 0; y < keyboard_height; y += key_height) {
                int row = y / key_height;
                int offset_x = keyboard_x + (keyboard_offsets[row] * 2);

                for (int x = 0; x < keyboard_width; x += key_width) {
                    if (wii_mouse_x >= (x + offset_x) &&
                        wii_mouse_x <= (x + offset_x + key_width) &&
                        wii_mouse_y >= (y + keyboard_y) &&
                        wii_mouse_y <= (y + keyboard_y + key_height)) {

                        int code = -1;

                        if (mud->keyboard_open == 2) {
                            code = keyboard_shift_buttons[row][(wii_mouse_x -
                                                                offset_x) /
                                                               key_width];
                        } else {
                            code =
                                keyboard_buttons[row][(wii_mouse_x - offset_x) /
                                                      key_width];
                        }

                        mudclient_key_pressed(mud, code, code);
                        mudclient_key_released(mud, code);
                        return;
                    }
                }
            }
        } else if (mud->last_wii_button == 1 && wii_mouse_button != 1) {
            mud->last_wii_button = 0;
        }
    } else {
        int game_x = wii_mouse_x - GAME_OFFSET_X;
        int game_y = wii_mouse_y - GAME_OFFSET_Y;

        if (wii_mouse_x != mud->last_wii_x || wii_mouse_y != mud->last_wii_y) {
            mudclient_mouse_moved(mud, game_x, game_y);
            mud->last_wii_x = wii_mouse_x;
            mud->last_wii_y = wii_mouse_y;
        }

        if (mud->last_wii_button != 1 && wii_mouse_button == 1) {
            mudclient_mouse_pressed(mud, game_x, game_y, 1);
            mud->last_wii_button = 1;
        } else if (mud->last_wii_button == 1 && wii_mouse_button != 1) {
            mudclient_mouse_released(mud, game_x, game_y, 1);
            mud->last_wii_button = 0;
        }

        if (mud->last_wii_button != 3 && wii_mouse_button == 3) {
            mudclient_mouse_pressed(mud, game_x, game_y, 3);
            mud->last_wii_button = 3;
        } else if (mud->last_wii_button == 3 && wii_mouse_button != 3) {
            mudclient_mouse_released(mud, game_x, game_y, 3);
            mud->last_wii_button = 0;
        }
    }
#endif

#ifdef _3DS
    hidScanInput();

    u32 keys_down = hidKeysDown();

    if (keys_down & KEY_LEFT) {
        if (mud->l_down) {
            mud->zoom_offset_x -= 56;

            if (mud->zoom_offset_x < 0) {
                mud->zoom_offset_x = 0;
            }
        } else {
            mudclient_key_pressed(mud, K_LEFT, -1);
        }
    }

    if (keys_down & KEY_RIGHT) {
        if (mud->l_down) {
            mud->zoom_offset_x += 56;

            if (mud->zoom_offset_x > 112) {
                mud->zoom_offset_x = 112;
            }
        } else {
            mudclient_key_pressed(mud, K_RIGHT, -1);
        }
    }

    if (mud->l_down && keys_down & KEY_UP) {
        mud->zoom_offset_y -= 53;

        if (mud->zoom_offset_y < 0) {
            mud->zoom_offset_y = 0;
        }
    }

    if (mud->l_down && keys_down & KEY_DOWN) {
        mud->zoom_offset_y += 53;

        if (mud->zoom_offset_y > 106) {
            mud->zoom_offset_y = 106;
        }
    }

    if (keys_down & KEY_SELECT) {
        mudclient_key_pressed(mud, K_F1, -1);
    }

    if (keys_down & KEY_START) {
        if (mud->keyboard_open) {
            mud->keyboard_open = 0;

            memcpy((uint8_t *)mud->framebuffer_top, game_top_bgr,
                   game_top_bgr_size);

            gspWaitForVBlank();

            memcpy((uint8_t *)mud->framebuffer_bottom, game_background_bgr,
                   game_background_bgr_size);
        } else {
            mud->keyboard_open = 1;

            memcpy((uint8_t *)mud->framebuffer_bottom, game_keyboard_bgr,
                   game_keyboard_bgr_size);

            draw_blue_bar(mud->framebuffer_top);
        }
    }

    if (keys_down & KEY_L) {
        mud->l_down = 1;
    }

    if (keys_down & KEY_R) {
        mud->r_down = !mud->r_down;

        if (!mud->r_down) {
            memcpy((uint8_t *)mud->framebuffer_top, game_top_bgr,
                   game_top_bgr_size);

            if (mud->keyboard_open) {
                draw_blue_bar(mud->framebuffer_top);
            }
        }
    }

    u32 keys_up = hidKeysUp();

    if (keys_up & KEY_LEFT) {
        mudclient_key_released(mud, K_LEFT);
    }

    if (keys_up & KEY_RIGHT) {
        mudclient_key_released(mud, K_RIGHT);
    }

    if (keys_up & KEY_L) {
        mud->l_down = 0;
    }

    touchPosition touch;
    hidTouchRead(&touch);

    if (mud->keyboard_open) {
        if (keys_down & KEY_X || keys_down & KEY_Y) {
            mudclient_key_pressed(mud, K_BACKSPACE, K_BACKSPACE);
            mudclient_key_released(mud, K_BACKSPACE);
        }

        if (keys_down & KEY_A || keys_down & KEY_B) {
            mudclient_key_pressed(mud, K_ENTER, K_ENTER);
            mudclient_key_released(mud, K_ENTER);
        }

        if (touch.px == 0 && touch.py == 0) {
            mud->touch_down = 0;
        }

        if (mud->touch_down) {
            return;
        }

        /* position of first key */
        int keyboard_x = 34;
        int keyboard_y = 31;

        int keyboard_width = KEY_WIDTH * 10;
        int keyboard_height = KEY_HEIGHT * 5;

        /* shift */
        if (touch.px >= keyboard_x &&
            touch.px <= (keyboard_x + (KEY_WIDTH * 2)) &&
            touch.py >= (keyboard_y + (KEY_HEIGHT * 4)) &&
            touch.py <= (keyboard_y + (KEY_HEIGHT * 5))) {

            if (mud->keyboard_open == 2) {
                mud->keyboard_open = 1;

                memcpy((uint8_t *)mud->framebuffer_bottom, game_keyboard_bgr,
                       game_keyboard_bgr_size);
            } else {
                mud->keyboard_open = 2;

                memcpy((uint8_t *)mud->framebuffer_bottom,
                       game_keyboard_shift_bgr, game_keyboard_shift_bgr_size);
            }

            gspWaitForVBlank();

            mud->touch_down = 1;
            return;
        }

        /* enter */
        if (touch.px >= 37 && touch.px <= 128 && touch.py >= 151 &&
            touch.py <= 169) {
            mudclient_key_pressed(mud, K_ENTER, K_ENTER);
            mudclient_key_released(mud, K_ENTER);

            mud->touch_down = 1;
            return;
        }

        /* backspace */
        if (touch.px >= 153 && touch.px <= 283 && touch.py >= 151 &&
            touch.py <= 169) {
            mudclient_key_pressed(mud, K_BACKSPACE, K_BACKSPACE);
            mudclient_key_released(mud, K_BACKSPACE);

            mud->touch_down = 1;
            return;
        }

        for (int y = 0; y < keyboard_height; y += KEY_HEIGHT) {
            int row = y / KEY_HEIGHT;
            int offset_x = keyboard_x + keyboard_offsets[row];

            for (int x = 0; x < keyboard_width; x += KEY_WIDTH) {
                if (touch.px >= (x + offset_x) &&
                    touch.px <= (x + offset_x + KEY_WIDTH) &&
                    touch.py >= (y + keyboard_y) &&
                    touch.py <= (y + keyboard_y + KEY_HEIGHT)) {

                    int code = -1;

                    if (mud->keyboard_open == 2) {
                        code =
                            keyboard_shift_buttons[row][(touch.px - offset_x) /
                                                        KEY_WIDTH];
                    } else {
                        code = keyboard_buttons[row][(touch.px - offset_x) /
                                                     KEY_WIDTH];
                    }

                    mudclient_key_pressed(mud, code, code);
                    mudclient_key_released(mud, code);

                    mud->touch_down = 1;
                    return;
                }
            }
        }
    } else {
        // TODO make these consts
        int offset_x = 32;
        int offset_y = 6 + 1;

        if (touch.px == 0 && touch.py == 0) {
            if (mud->touch_down != 0) {
                mudclient_mouse_released(mud, mud->mouse_x, mud->mouse_y,
                                         mud->touch_down);
            }

            mud->touch_down = 0;
        } else {
            int game_x = (touch.px - offset_x) * 2;
            int game_y = (touch.py - offset_y) * 2;

            if (game_x < 0 || game_y < 0 || game_x > mud->applet_width ||
                game_y > mud->applet_height) {
                return;
            }

            mudclient_mouse_moved(mud, game_x, game_y);

            int mouse_down = mud->l_down ? 3 : 1;

            if (mud->touch_down == 0) {
                mudclient_mouse_pressed(mud, game_x, game_y, mouse_down);
            }

            mud->touch_down = mouse_down;
        }
    }
#endif

#if !defined(WII) && !defined(_3DS)
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN: {
            char char_code;
            int code;
            get_sdl_keycodes(&event.key.keysym, &char_code, &code);
            mudclient_key_pressed(mud, code, char_code);

            int mag = 10;

            if (code == 113) {
                test_x -= mag;
            } else if (code == 97) {
                test_x += mag;
            } else if (code == 119) {
                test_y -= mag;
            } else if (code == 115) {
                test_y += mag;
            } else if (code == 101) {
                test_z -= mag;
            } else if (code == 100) {
                test_z += mag;
            } else if (code == 114) {
                test_yaw += 1;

                printf("yaw: %d\n", test_yaw);
            } else if (code == 102) {
                test_yaw -= 1;

                printf("yaw: %d\n", test_yaw);
            }

            /*if (test_model) {
                game_model_orient(test_model, 0, 0, test_yaw);
            }*/

            // printf("%d\n", code);

            break;
        }
        case SDL_KEYUP: {
            char char_code;
            int code;
            get_sdl_keycodes(&event.key.keysym, &char_code, &code);
            mudclient_key_released(mud, code);
            break;
        }
        case SDL_MOUSEMOTION:
            mudclient_mouse_moved(mud, event.motion.x, event.motion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            mudclient_mouse_pressed(mud, event.button.x, event.button.y,
                                    event.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            mudclient_mouse_released(mud, event.button.x, event.button.y,
                                     event.button.button);
            break;
        }
    }
#endif
}

#ifdef _3DS
void mudclient_flush_audio(mudclient *mud) {
    if (mud->sound_position < 0) {
        return;
    }

    if (wave_buf[fill_block].status == NDSP_WBUF_DONE) {
        s16 *wave_buff_data = wave_buf[fill_block].data_pcm16;
        int done = 0;

        for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
            if (done || mud->sound_position >= mud->sound_length) {
                wave_buff_data[i] = 0;
                done = 1;
            } else {
                wave_buff_data[i] = mud->pcm_out[mud->sound_position];
                mud->sound_position++;
            }
        }

        if (done) {
            mud->sound_position = -1;
            mud->sound_length = 0;
        }

        DSP_FlushDataCache((u32 *)wave_buff_data, SAMPLE_BUFFER_SIZE);

        ndspChnWaveBufAdd(0, &wave_buf[fill_block]);

        fill_block = !fill_block;
    }
}
#endif

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
        mudclient_draw_loading_screen(mud);
        mudclient_start_game(mud);
        mud->loading_step = 0;
    }

    int i = 0;
    int j = 256;
    int delay = 1;
    int i1 = 0;

    for (int j1 = 0; j1 < 10; j1++) {
        mud->timings[j1] = get_ticks();
    }

    while (mud->stop_timeout >= 0) {
        if (mud->stop_timeout > 0) {
            mud->stop_timeout--;

            if (mud->stop_timeout == 0) {
                // mudclient_on_closing(mud);
                return;
            }
        }

        int k1 = j;
        int last_delay = delay;

        j = 300;
        delay = 1;

        int time = get_ticks();

        if (mud->timings[i] == 0) {
            j = k1;
            delay = last_delay;
        } else if (time > mud->timings[i]) {
            j = (float)(2560 * mud->target_fps) /
                (float)(time - mud->timings[i]);
        }

        if (j < 25) {
            j = 25;
        }

        if (j > 256) {
            j = 256;
            delay = mud->target_fps - (time - mud->timings[i]) / 10;

            if (delay < mud->thread_sleep) {
                delay = mud->thread_sleep;
            }
        }

        delay_ticks(delay);

        mud->timings[i] = time;
        i = (i + 1) % 10;

        if (delay > 1) {
            for (int j2 = 0; j2 < 10; j2++) {
                if (mud->timings[j2] != 0) {
                    mud->timings[j2] += delay;
                }
            }
        }

        int k2 = 0;

        while (i1 < 256) {
            mudclient_poll_events(mud);
            mudclient_handle_inputs(mud);

            i1 += j;

            if (++k2 > mud->max_draw_time) {
                i1 = 0;

                mud->interlace_timer += 6;

                if (mud->interlace_timer > 25) {
                    mud->interlace_timer = 0;
                    mud->interlace = 1;
                }

                break;
            }
        }

        mud->interlace_timer--;
        i1 &= 0xff;

        mudclient_draw(mud);

#ifdef _3DS
        mudclient_flush_audio(mud);

        if (!aptMainLoop()) {
            return;
        }
#endif

        mud->fps = (1000 * j) / (mud->target_fps * 256);

        mud->mouse_scroll_delta = 0;
    }
}

void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id) {
    int type = mud->teleport_bubble_type[id];
    int time = mud->teleport_bubble_time[id];

    if (type == 0) {
        /* blue bubble used for teleports */
        int colour = BLUE + time * 5 * 256;
        surface_draw_circle(mud->surface, x + (width / 2), y + (height / 2),
                            20 + time * 2, colour, 255 - time * 5);
    } else if (type == 1) {
        /* red bubble used for telegrab */
        int colour = RED + time * 5 * 256;
        surface_draw_circle(mud->surface, x + (width / 2), y + (height / 2),
                            10 + time, colour, 255 - time * 5);
    }
}

void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id) {
    int picture = game_data_item_picture[id] + mud->sprite_item;
    int mask = game_data_item_mask[id];

    surface_sprite_clipping_from9(mud->surface, x, y, width, height, picture,
                                  mask, 0, 0, 0);
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
            if (game_data_item_stackable[id] == 1) {
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
    if (!mud->members || mud->settings_sound_disabled) {
        return;
    }

    char file_name[strlen(name) + 5];
    sprintf(file_name, "%s.pcm", name);

    int offset = get_data_file_offset(file_name, mud->sound_data);
    int length = get_data_file_length(file_name, mud->sound_data);

    memset(mud->pcm_out, 0, 1024 * 50 * 2);

    ulaw_to_linear(length, (uint8_t *)mud->sound_data + offset, mud->pcm_out);

#ifdef _3DS
    mud->sound_position = 0;
    mud->sound_length = length * 2;
#endif

#ifdef WII
    // ASND_StopVoice(0);
    ASND_SetVoice(0, VOICE_MONO_16BIT_BE, SAMPLE_RATE, 0, mud->pcm_out,
                  length * 2, 127, 127, NULL);
#endif

#if !defined(WII) && !defined(_3DS)
    /* TODO could re-pause after sound plays */
    SDL_PauseAudio(0);
    SDL_QueueAudio(1, mud->pcm_out, length * 2);
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
        width = game_data_object_width[id];
        height = game_data_object_height[id];
    } else {
        height = game_data_object_width[id];
        width = game_data_object_height[id];
    }

    if (game_data_object_type[id] == 2 || game_data_object_type[id] == 3) {
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

int main(int argc, char **argv) {
    srand(0);

    init_utility_global();
    init_surface_global();
    init_world_global();

#ifdef REVISION_177
    init_packet_stream_global();
#endif

    init_stats_tab_global();

    mudclient *mud = malloc(sizeof(mudclient));
    mudclient_new(mud);

    if (argc > 1 && strcmp(argv[1], "members") == 0) {
        mud->members = 1;
    }

    if (argc > 2) {
        mud->server = argv[2];
    }

    if (argc > 3) {
        mud->port = atoi(argv[3]);
    }

#ifdef REVISION_177
    /* BEGIN INAUTHENTIC COMMAND LINE ARGUMENTS */
    if (argc > 4) {
        rsa_exponent = argv[4];
    }

    if (argc > 5) {
        rsa_modulus = argv[5];
    }
    /* END INAUTHENTIC COMMAND LINE ARGUMENTS */
#endif

    mudclient_start_application(mud, MUD_WIDTH, MUD_HEIGHT,
                                "Runescape by Andrew Gower");
#ifdef _3DS
    linearFree(audio_buffer);
    ndspExit();

    gfxExit();
#endif

    return 0;
}
