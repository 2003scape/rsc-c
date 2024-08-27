#include "mudclient.h"

#ifdef _3DS

u32 *SOC_buffer = NULL;

ndspWaveBuf wave_buf[2] = {0};
u32 *audio_buffer = NULL;
int fill_block = 0;

Thread _3ds_keyboard_thread = {0};
char _3ds_keyboard_buffer[255] = {0};
volatile int _3ds_keyboard_received_input = 0;
SwkbdButton _3ds_keyboard_button;

char _3ds_option_buttons[] = {'A', 'B', 'X', 'Y', 'R'};

void mudclient_poll_events(mudclient *mud) {
    if (mud->keyboard_open) {
        if (_3ds_keyboard_received_input) {
            mudclient_3ds_handle_keyboard(mud);
        }

        return;
    }

    hidScanInput();

    u32 keys_down = hidKeysDown();

    if (keys_down & KEY_LEFT) {
        mudclient_key_pressed(mud, K_LEFT, -1);
    }

    if (keys_down & KEY_RIGHT) {
        mudclient_key_pressed(mud, K_RIGHT, -1);
    }

    if (keys_down & KEY_UP) {
        mudclient_key_pressed(mud, K_UP, -1);
    }

    if (keys_down & KEY_DOWN) {
        mudclient_key_pressed(mud, K_DOWN, -1);
    }

    if (keys_down & KEY_START && !mud->keyboard_open) {
        mudclient_3ds_open_keyboard(mud);
    }

    if (keys_down & KEY_L && keys_down & KEY_R) {
        mud->_3ds_gyro_down = 1;
        mud->_3ds_r_down = 1;
        mudclient_3ds_draw_top_background(mud);
    } else {
        if (keys_down & KEY_L) {
            mud->_3ds_l_down = 1;
        }

        if (keys_down & KEY_R) {
            mud->_3ds_r_down = !mud->_3ds_r_down;

            if (!mud->_3ds_r_down) {
                mudclient_3ds_draw_top_background(mud);
            }

            if (mud->show_option_menu) {
                mudclient_key_pressed(mud, K_5, K_5);
            }
        }
    }

    if (keys_down & KEY_A) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_1, K_1);
        } else {
            mudclient_key_pressed(mud, K_HOME, -1);
        }
    }

    if (keys_down & KEY_Y) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_4, K_4);
        } else {
            mudclient_key_pressed(mud, K_HOME, -1);
        }
    }

    if (keys_down & KEY_X) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_3, K_3);
        } else {
            mudclient_key_pressed(mud, K_PAGE_UP, -1);
        }
    }

    if (keys_down & KEY_B) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_2, K_2);
        } else {
            mudclient_key_pressed(mud, K_PAGE_DOWN, -1);
        }
    }

    u32 keys_up = hidKeysUp();

    if (keys_up & KEY_LEFT) {
        mudclient_key_released(mud, K_LEFT);
    }

    if (keys_up & KEY_RIGHT) {
        mudclient_key_released(mud, K_RIGHT);
    }

    if (keys_up & KEY_UP) {
        mudclient_key_released(mud, K_UP);
    }

    if (keys_up & KEY_DOWN) {
        mudclient_key_released(mud, K_DOWN);
    }

    if (keys_up & KEY_L) {
        mud->_3ds_l_down = 0;
        mud->_3ds_gyro_down = 0;
    }

    if (keys_up & KEY_R) {
        mud->_3ds_gyro_down = 0;
        mudclient_key_released(mud, K_5);
    }

    if (keys_up & KEY_A || keys_up & KEY_Y) {
        mudclient_key_released(mud, K_HOME);
        mudclient_key_released(mud, K_1);
        mudclient_key_released(mud, K_4);
    }

    if (keys_up & KEY_X) {
        mudclient_key_released(mud, K_PAGE_UP);
        mudclient_key_released(mud, K_3);
    }

    if (keys_up & KEY_B) {
        mudclient_key_released(mud, K_PAGE_DOWN);
        mudclient_key_released(mud, K_2);
    }

    if (keys_up & KEY_SELECT) {
        _3ds_toggle_top_screen(mud->_3ds_top_screen_off);
        mud->_3ds_top_screen_off = !mud->_3ds_top_screen_off;
    }

    touchPosition touch = {0};
    hidTouchRead(&touch);

    if (touch.px == 0 && touch.py == 0) {
        if (mud->_3ds_touch_down != 0) {
            mudclient_mouse_released(mud, mud->mouse_x, mud->mouse_y,
                                     mud->_3ds_touch_down);
        }

        mud->_3ds_touch_down = 0;
    } else {
        mudclient_mouse_moved(mud, touch.px, touch.py);

        int mouse_down = mud->_3ds_l_down ? 3 : 1;

        if (mud->_3ds_touch_down == 0) {
            mudclient_mouse_pressed(mud, touch.px, touch.py, mouse_down);
        }

        mud->_3ds_touch_down = mouse_down;
    }

    if (mud->_3ds_gyro_down) {
        angularRate gyro = {0};
        hidGyroRead(&gyro);

        mud->camera_rotation = (mud->camera_rotation + (gyro.y / 150)) & 0xff;

        // accelerometer should do zooming
        /*mud->camera_zoom += gyro.x / -20;

        if (mud->camera_zoom > ZOOM_MAX) {
            mud->camera_zoom = ZOOM_MAX;
        } else if (mud->camera_zoom < ZOOM_MIN) {
            mud->camera_zoom = ZOOM_MIN;
        }*/
    }
}

void mudclient_3ds_flush_audio(mudclient *mud) {
    if (mud->_3ds_sound_position < 0) {
        return;
    }

    if (wave_buf[fill_block].status == NDSP_WBUF_DONE) {
        s16 *wave_buff_data = wave_buf[fill_block].data_pcm16;
        int done = 0;

        for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
            if (done || mud->_3ds_sound_position >= mud->_3ds_sound_length) {
                wave_buff_data[i] = 0;
                done = 1;
            } else {
                wave_buff_data[i] = mud->pcm_out[mud->_3ds_sound_position];
                mud->_3ds_sound_position++;
            }
        }

        if (done) {
            mud->_3ds_sound_position = -1;
            mud->_3ds_sound_length = 0;
        }

        DSP_FlushDataCache((u32 *)wave_buff_data, SAMPLE_BUFFER_SIZE);

        ndspChnWaveBufAdd(0, &wave_buf[fill_block]);

        // fill_block = !fill_block;
    }
}

void mudclient_3ds_open_keyboard(mudclient *mud) {
    if (mud->keyboard_open) {
        return;
    }

    int32_t priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

    memset(_3ds_keyboard_buffer, 0, 255);
    memset(mud->_3ds_framebuffer_top, 0, 400 * 240 * 3);

#ifdef RENDER_3DS_GL
    gspWaitForPPF();

    uint32_t *framebuffer_bottom =
        (uint32_t *)mud->_3ds_gl_render_target->frameBuf.colorBuf;

    for (int x = 0; x < 319; x++) {
        for (int y = 0; y < 240; y++) {
            int top_index = (((x + 40) * 240) + (240 - y)) * 3;

            int bottom_index =
                _3ds_gl_translate_framebuffer_index((y * 320) + x);

            int32_t colour = (int32_t)framebuffer_bottom[bottom_index];

            mud->_3ds_framebuffer_top[top_index + 2] = (colour >> 24) & 0xff;
            mud->_3ds_framebuffer_top[top_index + 1] = (colour >> 16) & 0xff;
            mud->_3ds_framebuffer_top[top_index] = (colour >> 8) & 0xff;
        }
    }
#else
    for (int x = 0; x < 319; x++) {
        int top_index = ((x + 40) * 240) * 3;
        int bottom_index = (x * 240) * 3;

        memcpy(mud->_3ds_framebuffer_top + top_index,
               mud->_3ds_framebuffer_bottom + bottom_index, 240 * 3);
    }
#endif

    int keyboard_type = _3DS_KEYBOARD_NORMAL;

    // TODO also for change password
    if (!mud->logged_in &&
        ((mud->login_screen == LOGIN_STAGE_EXISTING &&
          mud->panel_login_existing_user->focus_control_index ==
              mud->control_login_password) ||
         (mud->login_screen == LOGIN_STAGE_NEW &&
          (mud->panel_login_new_user->focus_control_index ==
               mud->control_register_password ||
           mud->panel_login_new_user->focus_control_index ==
               mud->control_register_confirm_password)))) {
        keyboard_type = _3DS_KEYBOARD_PASSWORD;
    } else if (mud->show_dialog_offer_x) {
        keyboard_type = _3DS_KEYBOARD_NUMPAD;
    }

    _3ds_keyboard_thread =
        threadCreate(_3ds_keyboard_thread_callback, (void *)keyboard_type,
                     STACK_SIZE, priority + 1, -2, 1);

    mud->keyboard_open = 1;
}

void mudclient_3ds_handle_keyboard(mudclient *mud) {
    mud->keyboard_open = 0;
    _3ds_keyboard_received_input = 0;

    for (int i = 0; i < 10; i++) {
        mud->timings[i] = get_ticks();
    }

    if (_3ds_keyboard_button == SWKBD_BUTTON_RIGHT) {
        for (int i = 0; i < 255; i++) {
            mudclient_key_pressed(mud, K_BACKSPACE, K_BACKSPACE);
        }

        int input_length = strlen(_3ds_keyboard_buffer);

        for (int i = 0; i < input_length; i++) {
            mudclient_key_pressed(mud, -1, _3ds_keyboard_buffer[i]);
        }

        if (input_length > 0) {
            mudclient_key_pressed(mud, K_ENTER, K_ENTER);
        }
    }

    mudclient_3ds_draw_top_background(mud);
}

void mudclient_3ds_draw_top_background(mudclient *mud) {
    memcpy((uint8_t *)mud->_3ds_framebuffer_top, game_top_bgr,
           game_top_bgr_size);
}

void soc_shutdown() {
    socExit();

    _3ds_toggle_top_screen(1);
}

void _3ds_keyboard_thread_callback(void *arg) {
    static SwkbdState swkbd;
    swkbdInit(&swkbd, (int)arg, 2, -1);
    swkbdSetInitialText(&swkbd, _3ds_keyboard_buffer);
    swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT);

    int reload = 1;
    static SwkbdStatusData swkbdStatus;
    swkbdSetStatusData(&swkbd, &swkbdStatus, reload, 1);

    static SwkbdLearningData swkbdLearning;
    swkbdSetLearningData(&swkbd, &swkbdLearning, reload, 1);

    _3ds_keyboard_button = swkbdInputText(&swkbd, _3ds_keyboard_buffer,
                                          sizeof(_3ds_keyboard_buffer));

    if (_3ds_keyboard_button != SWKBD_BUTTON_NONE) {
        _3ds_keyboard_received_input = 1;
    }

    threadExit(0);
}

void _3ds_toggle_top_screen(int is_off) {
    gspLcdInit();

    if (is_off) {
        GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_TOP);
    } else {
        GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_TOP);
    }

    gspLcdExit();
}

#ifdef RENDER_3DS_GL
void mudclient_3ds_gl_offscreen_frame_start(mudclient *mud) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_RenderTargetClear(mud->_3ds_gl_offscreen_render_target, C3D_CLEAR_ALL,
                          BLACK, 0);

    C3D_FrameDrawOn(mud->_3ds_gl_offscreen_render_target);
}

void mudclient_3ds_gl_frame_start(mudclient *mud, int clear) {
    /* crashes on console, faster on citra */
    // C3D_FrameBegin(C3D_FRAME_NONBLOCK);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(mud->_3ds_gl_render_target);
}

void mudclient_3ds_gl_frame_end() { C3D_FrameEnd(0); }
#endif

void mudclient_start_application(mudclient *mud, char *title) {
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
}
#endif
