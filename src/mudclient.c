#include "mudclient.h"

char *font_files[] = {"h11p.jf", "h12b.jf", "h12p.jf", "h13b.jf",
                      "h14b.jf", "h16b.jf", "h20b.jf", "h24b.jf"};

char *animated_models[] = {
    "torcha2",      "torcha3",    "torcha4",    "skulltorcha2", "skulltorcha3",
    "skulltorcha4", "firea2",     "firea3",     "fireplacea2",  "fireplacea3",
    "firespell2",   "firespell3", "lightning2", "lightning3",   "clawspell2",
    "clawspell3",   "clawspell4", "clawspell5", "spellcharge2", "spellcharge3"};

char *short_skill_names[] = {
    "Attack",   "Defense",  "Strength", "Hits",      "Ranged",  "Prayer",
    "Magic",    "Cooking",  "Woodcut",  "Fletching", "Fishing", "Firemaking",
    "Crafting", "Smithing", "Mining",   "Herblaw",   "Agility", "Thieving"};

char *skill_names[] = {
    "Attack",   "Defense",  "Strength",    "Hits",      "Ranged",  "Prayer",
    "Magic",    "Cooking",  "Woodcutting", "Fletching", "Fishing", "Firemaking",
    "Crafting", "Smithing", "Mining",      "Herblaw",   "Agility", "Thieving"};

char *equipment_stat_names[] = {"Armour", "WeaponAim", "WeaponPower", "Magic",
                                "Prayer"};

int experience_array[100];

char login_screen_status[255];

#ifndef WII
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code) {
    *char_code = -1;

    switch (keysym->scancode) {
    case SDL_SCANCODE_LEFT:
        *code = 37;
        break;
    case SDL_SCANCODE_RIGHT:
        *code = 39;
        break;
    /*
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
                }
            }
        }

        break;
    }
}
#endif

void init_mudclient_global() {
    int total_exp = 0;

    for (int i = 0; i < 99; i++) {
        int level = i + 1;
        int exp = level + 300 * pow(2, level / 7);
        total_exp += exp;
        experience_array[i] = total_exp & 0xffffffc;
    }
}

void mudclient_new(mudclient *mud) {
    memset(mud, 0, sizeof(mudclient));

    mud->applet_width = MUD_WIDTH;
    mud->applet_height = MUD_HEIGHT;
    mud->target_fps = 20;
    mud->max_draw_time = 100;
    mud->loading_step = 1;
    mud->loading_progess_text = "Loading";
    mud->thread_sleep = 10;
    mud->server = "127.0.0.1";
    mud->port = 43594;
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

    memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
    memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
    memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);
    memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);

    mud->local_player = malloc(sizeof(GameCharacter));
    game_character_new(mud->local_player);
}

void mudclient_start_application(mudclient *mud, int width, int height,
                                 char *title) {
    printf("Started application\n");

    mud->applet_width = width;
    mud->applet_height = height;
    mud->loading_step = 1;

#ifdef WII
	VIDEO_Init();
	WPAD_Init();

	GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
    mud->framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(mud->framebuffer);
	VIDEO_SetBlack(0);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if(rmode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }

	console_init(mud->framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

    /*
	while (1) {
		WPAD_ScanPads();

		u32 pressed = WPAD_ButtonsDown(0);

        printf("%d\n", get_ticks());

		if (pressed & WPAD_BUTTON_HOME) {
            exit(0);
        }

		VIDEO_WaitVSync();
	}*/
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

    SDL_AudioSpec wanted_audio;

    wanted_audio.freq = 8000;
    wanted_audio.format = AUDIO_S16;
    wanted_audio.channels = 1;
    wanted_audio.silence = 0;
    wanted_audio.samples = 1024;
    wanted_audio.callback = NULL;

    if (SDL_OpenAudio(&wanted_audio, NULL) < 0) {
        fprintf(stderr, "SDL_OpenAudio(): %s\n", SDL_GetError());
        exit(1);
    }

    mud->window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height, SDL_WINDOW_SHOWN);

    mud->screen = SDL_GetWindowSurface(mud->window);

    mud->pixel_surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff0000,
                                              0x00ff00, 0x0000ff, 0);
#endif

    mudclient_run(mud);
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

        /*
        if (mud->login_screen == 3 && mud->panel_recover_user) {
            panel_key_press(mud->panel_recover_user, key_code);
        }*/
    }

    /*
    if (mud->logged_in == 1) {
        if (mud->show_appearance_change && mud->panel_appearance) {
            panel_key_press(mud->panel_appearance, key_code);
            return;
        }

        if (
            mud->show_change_password_step == 0 &&
            mud->show_dialog_social_input == 0 &&
            mud->show_dialog_report_abuse_step == 0 &&
            !mud->is_sleeping &&
            mud->panel_message_tabs
        ) {
            panel_key_press(mud->panel_message_tabs, key_code);
        }

        if (
            mud->show_change_password_step == 3 ||
            mud->show_change_password_step == 4
        ) {
            mud->show_change_password_step = 0;
        }
    }*/
}
void mudclient_key_pressed(mudclient *mud, int code, char char_code) {
    if (code == K_LEFT) {
        mud->key_left = 1;
    } else if (code == K_RIGHT) {
        mud->key_right = 1;
    } else {
        mudclient_handle_key_press(mud, code);
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

        if (current_length < 20) {
            mud->input_text_current[current_length] = char_code;
            mud->input_text_current[current_length + 1] = '\0';
        }

        int pm_length = strlen(mud->input_pm_current);

        if (pm_length < 80) {
            mud->input_pm_current[pm_length] = char_code;
            mud->input_pm_current[current_length + 1] = '\0';
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
            mud->input_text_current[pm_length - 1] = '\0';
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

void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button) {
    mud->mouse_x = x;
    mud->mouse_y = y;

    if (mud->options->middle_click_camera && button == 1) {
        mud->middle_button_down = 1;
        mud->origin_rotation = mud->camera_rotation;
        mud->origin_mouse_x = mud->mouse_x;
        return;
    } else if (button == 2) {
        mud->mouse_button_down = 2;
    } else {
        mud->mouse_button_down = 1;
    }

    mud->last_mouse_button_down = mud->mouse_button_down;
    mud->mouse_action_timeout = 0;

    /*mud_handle_mouse_down(mud->mouse_button_down, x, y);*/
}

void mudclient_set_target_fps(mudclient *mud, int fps) {
    mud->target_fps = 1000 / fps;
}

void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1) {

    if (mud->login_screen == 1) {
        sprintf(login_screen_status, "%s %s", s, s1);

        panel_update_text(mud->panel_login_new_user,
                          mud->control_register_status, login_screen_status);
    } else if (mud->login_screen == 2) {
        sprintf(login_screen_status, "%s %s", s, s1);

        panel_update_text(mud->panel_login_existing_user,
                          mud->control_login_status, login_screen_status);
    }

    // mud->login_user_disp = s1;

    mudclient_draw_login_screens(mud);
    mudclient_reset_timings(mud);
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

void mudclient_draw_string(mudclient *mud, char *string, int font, int x,
                           int y) {}

void mudclient_draw_loading_screen(mudclient *mud, int percent, char *text) {}

void mudclient_show_loading_progress(mudclient *mud, int percent, char *text) {}

int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent) {
    char loading_text[35]; /* max description is 19 */

    sprintf(loading_text, "Loading %s - 0%%", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

    int8_t header[6];
#ifdef WII
    int8_t *file_data = NULL;

    if (strcmp(file, "fonts" FONTS ".jag") == 0) {
        file_data = fonts1_jag;
    } else if (strcmp(file, "config" CONFIG ".jag") == 0) {
        file_data = config85_jag;
    } else if (strcmp(file, "media" MEDIA ".jag") == 0) {
        file_data = media58_jag;
    } else if (strcmp(file, "entity" ENTITY ".jag") == 0) {
        file_data = entity24_jag;
    } else if (strcmp(file, "entity" ENTITY ".mem") == 0) {
        file_data = entity24_mem;
    } else if (strcmp(file, "textures" TEXTURES ".jag") == 0) {
        file_data = textures17_jag;
    } else if (strcmp(file, "maps" MAPS ".jag") == 0) {
        file_data = maps63_jag;
    } else if (strcmp(file, "maps" MAPS ".mem") == 0) {
        file_data = maps63_mem;
    } else if (strcmp(file, "land" MAPS ".jag") == 0) {
        file_data = land63_jag;
    } else if (strcmp(file, "land" MAPS ".mem") == 0) {
        file_data = land63_mem;
    } else if (strcmp(file, "models" MODELS ".jag") == 0) {
        file_data = models36_jag;
    }

    if (file_data == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", file);
        exit(1);
    }

    memcpy(header, file_data, 6);
#else
    int file_length = strlen(file);
    char *prefix = "./cache";
    char prefixed_file[file_length + strlen(prefix) + 1];
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
void mudclient_parse_tga(mudclient *mud, int8_t *tga_buffer) {}

void mudclient_load_jagex(mudclient *mud) {
    // fill black rect

    /*
    int8_t *jagex_jag =
        mudclient_read_data_file(mud, "jagex.jag", "Jagex library", 0);

    if (jagex_jag != NULL) {
        int8_t *logo_tga = load_data("logo.tga", 0, jagex_jag);
        mud->image_logo = mudclient_parse_tga(logo_tga);
        free(jagex_jag);
    }*/

    int8_t *fonts_jag =
        mudclient_read_data_file(mud, "fonts" FONTS ".jag", "Game fonts", 5);

    if (fonts_jag != NULL) {
        for (int i = 0; i < FONT_COUNT; i++) {
            create_font(load_data(font_files[i], 0, fonts_jag), i);
        }

        free(fonts_jag);
    }
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

    /*
    int8_t *filter_jag = mudclient_read_data_file(mud, "filter" FILTER ".jag",
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
        char file_name[20];
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

        char file_name[255];
        memset(file_name, 0, 255);
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

    char file_name[255];

    for (int i = 0; i < game_data_texture_count; i++) {
        sprintf(file_name, "%s.dat", game_data_texture_name[i]);

        int8_t *texture_dat = load_data(file_name, 0, textures_jag);

        surface_parse_sprite(mud->surface, mud->sprite_texture, texture_dat,
                             index_dat, 1);

        surface_draw_box(mud->surface, 0, 0, 128, 128, 0xff00ff);
        surface_draw_sprite_from3(mud->surface, 0, 0, mud->sprite_texture);

        free(mud->surface->sprite_colour_list[mud->sprite_texture]);
        mud->surface->sprite_colour_list[mud->sprite_texture] = NULL;

        free(mud->surface->sprite_colours_used[mud->sprite_texture]);
        mud->surface->sprite_colours_used[mud->sprite_texture] = NULL;

        int wh = mud->surface->sprite_width_full[mud->sprite_texture];
        char *name_sub = game_data_texture_subtype_name[i];

        if (name_sub) {
            int sub_length = strlen(name_sub);

            if (sub_length) {
                sprintf(file_name, "%s.dat", name_sub);

                int8_t *texture_sub_dat = load_data(file_name, 0, textures_jag);

                surface_parse_sprite(mud->surface, mud->sprite_texture,
                                     texture_sub_dat, index_dat, 1);

                surface_draw_sprite_from3(mud->surface, 0, 0,
                                          mud->sprite_texture);

                free(mud->surface->sprite_colour_list[mud->sprite_texture]);
                mud->surface->sprite_colour_list[mud->sprite_texture] = NULL;

                free(mud->surface->sprite_colours_used[mud->sprite_texture]);
                mud->surface->sprite_colours_used[mud->sprite_texture] = NULL;
            }
        }

        surface_draw_sprite_from5(mud->surface, mud->sprite_texture_world + i,
                                  0, 0, wh, wh);

        for (int j = 0; j < wh * wh; j++) {
            if (mud->surface->surface_pixels[mud->sprite_texture_world + i]
                                            [j] == 0xff00) {
                mud->surface->surface_pixels[mud->sprite_texture_world + i][j] =
                    0xff00ff;
            }
        }

        surface_draw_world(mud->surface, mud->sprite_texture_world + i);

        scene_define_texture(
            mud->scene, i,
            mud->surface->sprite_colours_used[mud->sprite_texture_world + i],
            mud->surface->sprite_colour_list[mud->sprite_texture_world + i],
            (wh / 64) - 1);

        /*
        free(mud->surface->sprite_colour_list[mud->sprite_texture_world + i]);
        mud->surface->sprite_colour_list[mud->sprite_texture_world + i] = NULL;

        free(mud->surface->sprite_colours_used[mud->sprite_texture_world + i]);
        mud->surface->sprite_colours_used[mud->sprite_texture_world + i] =
        NULL;*/

        free(mud->surface->surface_pixels[mud->sprite_texture_world + i]);
        mud->surface->surface_pixels[mud->sprite_texture_world + i] = NULL;
    }

#ifndef WII
    free(textures_jag);
#endif
    free(index_dat);
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

void mudclient_load_models(mudclient *mud) {
    for (int i = 0; i < ANIMATED_MODELS_COUNT; i++) {
        char name_length = strlen(animated_models[i]);
        char name[name_length + 1];
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
        char file_name[255];
        sprintf(file_name, "%s.ob3", game_data_model_name[i]);

        int offset = get_data_file_offset(file_name, models_jag);

        GameModel *game_model = malloc(sizeof(GameModel));

        if (offset != 0) {
            game_model_from_bytes(game_model, models_jag, offset);
        } else {
            game_model_from2(game_model, 1, 1);
        }

        mud->game_models[i] = game_model;

        if (strcmp(game_data_model_name[i], "giantcrystal") == 0) {
            mud->game_models[i]->transparent = 1;
        }
    }

    free(models_jag);
}

void mudclient_create_login_panels(mudclient *mud) {
    mud->panel_login_welcome = malloc(sizeof(Panel));
    panel_new(mud->panel_login_welcome, mud->surface, 50);

    int x = mud->game_width / 2;
    int y = 40;

    if (!mud->members) {
        panel_add_text_centre(mud->panel_login_welcome, x, 200 + y,
                              "Click on an option", 5, 1);

        panel_add_button_background(mud->panel_login_welcome, x - 100, 240 + y,
                                    120, 35);

        panel_add_text_centre(mud->panel_login_welcome, x - 100, 240 + y,
                              "New User", 5, 0);

        mud->control_welcome_new_user = panel_add_button(
            mud->panel_login_welcome, x - 100, 240 + y, 120, 35);

        panel_add_button_background(mud->panel_login_welcome, x + 100, 240 + y,
                                    120, 35);

        panel_add_text_centre(mud->panel_login_welcome, x + 100, 240 + y,
                              "Existing User", 5, 0);

        mud->control_welcome_existing_user = panel_add_button(
            mud->panel_login_welcome, x + 100, 240 + y, 120, 35);
    } else {
        panel_add_text_centre(mud->panel_login_welcome, x, 200 + y,
                              "Welcome to RuneScape", 4, 1);

        panel_add_text_centre(mud->panel_login_welcome, x, 215 + y,
                              "You need a members account to use this server",
                              4, 1);

        panel_add_button_background(mud->panel_login_welcome, x, 250 + y, 200,
                                    35);

        panel_add_text_centre(mud->panel_login_welcome, x, 250 + y,
                              "Click here to login", 5, 0);

        mud->control_welcome_existing_user =
            panel_add_button(mud->panel_login_welcome, x, 250 + y, 200, 35);
    }

    mud->panel_login_new_user = malloc(sizeof(Panel));
    panel_new(mud->panel_login_new_user, mud->surface, 50);

    if (!mud->options->account_management) {
        y = 230;

        if (mud->refer_id == 0) {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "to create an account please go back to the",
                                  4, 1);

            y += 20;

            panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "www.runescape.com front page, and choose 'create account'", 4,
                1);
        } else if (mud->refer_id == 1) {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "to create an account please click on the", 4,
                                  1);

            y += 20;

            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "'create account' link below the game window",
                                  4, 1);
        } else {
            panel_add_text_centre(mud->panel_login_new_user, x, y + 8,
                                  "To create an account please go back to the",
                                  4, 1);

            y += 20;

            panel_add_text_centre(
                mud->panel_login_new_user, x, y + 8,
                "runescape front webpage and choose 'create account'", 4, 1);
        }

        y += 30;

        panel_add_button_background(mud->panel_login_new_user, x, y + 17, 150,
                                    34);
        panel_add_text_centre(mud->panel_login_new_user, x, y + 17, "Ok", 5, 0);

        mud->control_login_new_ok =
            panel_add_button(mud->panel_login_new_user, x, y + 17, 150, 34);
    } else {
        y = 70;

        mud->control_register_status = panel_add_text_centre(
            mud->panel_login_new_user, x, y + 8,
            "to create an account please enter all the requested details", 4,
            1);

        int offset_y = y + 25;

        panel_add_button_background(mud->panel_login_new_user, x, offset_y + 17,
                                    250, 34);

        panel_add_text_centre(mud->panel_login_new_user, x, offset_y + 8,
                              "Choose a username", 4, 0);

        mud->control_register_user = panel_add_text_input(
            mud->panel_login_new_user, x, offset_y + 25, 200, 40, 4, 12, 0, 0);

        offset_y += 40;

        panel_add_button_background(mud->panel_login_new_user, x - 115,
                                    offset_y + 17, 220, 34);

        panel_add_text_centre(mud->panel_login_new_user, x - 115, offset_y + 8,
                              "Choose a Password", 4, 0);

        mud->control_register_password =
            panel_add_text_input(mud->panel_login_new_user, x - 115,
                                 offset_y + 25, 220, 40, 4, 20, 1, 0);

        panel_add_button_background(mud->panel_login_new_user, x + 115,
                                    offset_y + 17, 220, 34);

        panel_add_text_centre(mud->panel_login_new_user, x + 115, offset_y + 8,
                              "Confirm Password", 4, 0);

        mud->control_register_confirm_password =
            panel_add_text_input(mud->panel_login_new_user, x + 115,
                                 offset_y + 25, 220, 40, 4, 20, 1, 0);

        offset_y += 60;

        mud->control_register_checkbox = panel_add_checkbox(
            mud->panel_login_new_user, x - 196 - 7, offset_y - 7, 14, 14);

        panel_add_text(mud->panel_login_new_user, x - 181, offset_y,
                       "I have read and agreed to the terms and conditions", 4,
                       1);

        offset_y += 15;

        panel_add_text_centre(
            mud->panel_login_new_user, x, offset_y,
            "(to view these click the relevant link below this game window)", 4,
            1);

        offset_y += 20;

        panel_add_button_background(mud->panel_login_new_user, x - 100,
                                    offset_y + 17, 150, 34);

        panel_add_text_centre(mud->panel_login_new_user, x - 100, offset_y + 17,
                              "Submit", 5, 0);

        mud->control_register_submit = panel_add_button(
            mud->panel_login_new_user, x - 100, offset_y + 17, 150, 34);

        panel_add_button_background(mud->panel_login_new_user, x + 100,
                                    offset_y + 17, 150, 34);

        panel_add_text_centre(mud->panel_login_new_user, x + 100, offset_y + 17,
                              "Cancel", 5, 0);

        mud->control_register_cancel = panel_add_button(
            mud->panel_login_new_user, x + 100, offset_y + 17, 150, 34);
    }

    mud->panel_login_existing_user = malloc(sizeof(Panel));
    panel_new(mud->panel_login_existing_user, mud->surface, 50);

    y = 230;

    mud->control_login_status =
        panel_add_text_centre(mud->panel_login_existing_user, x, y - 10,
                              "Please enter your username and password", 4, 1);

    y += 28;

    panel_add_button_background(mud->panel_login_existing_user, x - 116, y, 200,
                                40);

    panel_add_text_centre(mud->panel_login_existing_user, x - 116, y - 10,
                          "Username:", 4, 0);

    mud->control_login_user = panel_add_text_input(
        mud->panel_login_existing_user, x - 116, y + 10, 200, 40, 4, 12, 0, 0);

    y += 47;

    panel_add_button_background(mud->panel_login_existing_user, x - 66, y, 200,
                                40);

    panel_add_text_centre(mud->panel_login_existing_user, x - 66, y - 10,
                          "Password:", 4, 0);

    mud->control_login_password = panel_add_text_input(
        mud->panel_login_existing_user, x - 66, y + 10, 200, 40, 4, 20, 1, 0);

    y -= 55;

    panel_add_button_background(mud->panel_login_existing_user, x + 154, y, 120,
                                25);
    panel_add_text_centre(mud->panel_login_existing_user, x + 154, y, "Ok", 4,
                          0);

    mud->control_login_ok =
        panel_add_button(mud->panel_login_existing_user, x + 154, y, 120, 25);

    y += 30;

    panel_add_button_background(mud->panel_login_existing_user, x + 154, y, 120,
                                25);
    panel_add_text_centre(mud->panel_login_existing_user, x + 154, y, "Cancel",
                          4, 0);

    mud->control_login_cancel =
        panel_add_button(mud->panel_login_existing_user, x + 154, y, 120, 25);

    if (mud->options->account_management) {
        y += 30;

        panel_add_button_background(mud->panel_login_existing_user, x + 154, y,
                                    160, 25);

        panel_add_text_centre(mud->panel_login_existing_user, x + 154, y,
                              "I've lost my password", 4, 0);

        mud->control_login_recover = panel_add_button(
            mud->panel_login_existing_user, x + 154, y, 160, 25);
    }
}

void mudclient_reset_login_screen_variables(mudclient *mud) {
    mud->logged_in = 0;
    mud->login_screen = 0;
    mud->login_user[0] = '\0';
    mud->login_pass[0] = '\0';
    mud->login_prompt = "Please enter a username:";
    sprintf(mud->login_user_disp, "*%s*", mud->login_user);
    mud->player_count = 0;
    mud->npc_count = 0;
}

void mudclient_render_login_screen_viewports(mudclient *mud) {
    int plane = 0;
    int region_x = 50; // 49;
    int region_y = 50; // 47;

    world_load_section_from3(mud->world, region_x * 48 + 23, region_y * 48 + 23,
                             plane);

    world_add_models(mud->world, mud->game_models);

    int x = 9728;
    int y = 6400;
    int zoom = 1100;
    int rotation = 888;

    mud->scene->clip_far_3d = 4100;
    mud->scene->clip_far_2d = 4100;
    mud->scene->fog_z_falloff = 1;
    mud->scene->fog_z_distance = 4000;

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

    scene_render(mud->scene);

    surface_fade_to_black(mud->surface);
    surface_fade_to_black(mud->surface);
    surface_draw_box(mud->surface, 0, 0, mud->game_width, 6, 0);

    for (int i = 6; i >= 1; i--) {
        surface_draw_line_alpha(mud->surface, 0, i, 0, i, mud->game_width, 8);
    }

    surface_draw_box(mud->surface, 0, 194, 512, 20, 0);

    for (int i = 6; i >= 1; i--) {
        surface_draw_line_alpha(mud->surface, 0, i, 0, 194 - i, mud->game_width,
                                8);
    }

    surface_draw_sprite_from3(
        mud->surface,
        (mud->game_width / 2) -
            (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
        15, mud->sprite_media + 10);

    surface_draw_sprite_from5(mud->surface, mud->sprite_logo, 0, 0,
                              mud->game_width, 200);

    surface_draw_world(mud->surface, mud->sprite_logo);

    x = 9216;
    y = 9216;
    zoom = 1100;
    rotation = 888;

    /* TODO see if we need these - they're the same */
    /*mud->scene->clip_far_3d = 4100;
    mud->scene->clip_far_2d = 4100;
    mud->scene->fog_z_falloff = 1;
    mud->scene->fog_z_distance = 4000;*/

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

    scene_render(mud->scene);

    surface_fade_to_black(mud->surface);
    surface_fade_to_black(mud->surface);
    surface_draw_box(mud->surface, 0, 0, mud->game_width, 6, 0);

    for (int i = 6; i >= 1; i--) {
        surface_draw_line_alpha(mud->surface, 0, i, 0, i, mud->game_width, 8);
    }

    surface_draw_box(mud->surface, 0, 194, mud->game_width, 20, 0);

    for (int i = 6; i >= 1; i--) {
        surface_draw_line_alpha(mud->surface, 0, i, 0, 194 - i, mud->game_width,
                                8);
    }

    surface_draw_sprite_from3(
        mud->surface,
        (mud->game_width / 2) -
            (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
        15, mud->sprite_media + 10);

    surface_draw_sprite_from5(mud->surface, mud->sprite_logo + 1, 0, 0,
                              mud->game_width, 200);

    surface_draw_world(mud->surface, mud->sprite_logo + 1);

    for (int i = 0; i < TERRAIN_COUNT; i++) {
        scene_remove_model(mud->scene, mud->world->roof_models[0][i]);
        scene_remove_model(mud->scene, mud->world->wall_models[1][i]);
        scene_remove_model(mud->scene, mud->world->roof_models[1][i]);
        scene_remove_model(mud->scene, mud->world->wall_models[2][i]);
        scene_remove_model(mud->scene, mud->world->roof_models[2][i]);
    }

    x = 11136;
    y = 10368;
    zoom = 500;
    rotation = 376;

    /*
    mud->scene->clip_far_3d = 4100;
    mud->scene->clip_far_2d = 4100;
    mud->scene->fog_z_falloff = 1;
    mud->scene->fog_z_distance = 4000;*/

    surface_black_screen(mud->surface);

    scene_set_camera(mud->scene, x, -world_get_elevation(mud->world, x, y), y,
                     912, rotation, 0, zoom * 2);

    scene_render(mud->scene);

    surface_fade_to_black(mud->surface);
    surface_fade_to_black(mud->surface);
    surface_draw_box(mud->surface, 0, 0, mud->game_width, 6, 0);

    for (int i = 6; i >= 1; i--) {
        surface_draw_line_alpha(mud->surface, 0, i, 0, i, mud->game_width, 8);
    }

    surface_draw_box(mud->surface, 0, 194, mud->game_width, 20, 0);

    for (int i = 6; i >= 1; i--) {
        surface_draw_line_alpha(mud->surface, 0, i, 0, 194, mud->game_width, 8);
    }

    surface_draw_sprite_from3(
        mud->surface,
        (mud->game_width / 2) -
            (mud->surface->sprite_width[mud->sprite_media + 10] / 2),
        15, mud->sprite_media + 10);

    surface_draw_sprite_from5(mud->surface, mud->sprite_media + 10, 0, 0,
                              mud->game_width, 200);

    surface_draw_world(mud->surface, mud->sprite_media + 10);
}

void mudclient_draw_login_screens(mudclient *mud) {
    mud->welcome_screen_already_shown = 0;
    mud->surface->interlace = 0;

    surface_black_screen(mud->surface);

    int show_background = 0;

    if (mud->options->account_management) {
        show_background = mud->login_screen == 0 || mud->login_screen == 2;
    } else {
        show_background = mud->login_screen >= 0 && mud->login_screen <= 3;
    }

    if (show_background) {
        int cycle = (mud->login_timer * 2) % 3072;

        if (cycle < 1024) {
            surface_draw_sprite_from3(mud->surface, 0, 10, mud->sprite_logo);

            if (cycle > 768) {
                surface_draw_sprite_alpha_from4(
                    mud->surface, 0, 10, mud->sprite_logo + 1, cycle - 768);
            }
        } else if (cycle < 2048) {
            surface_draw_sprite_from3(mud->surface, 0, 10,
                                      mud->sprite_logo + 1);

            if (cycle > 1792) {
                surface_draw_sprite_alpha_from4(
                    mud->surface, 0, 10, mud->sprite_media + 10, cycle - 1792);
            }
        } else {
            surface_draw_sprite_from3(mud->surface, 0, 10,
                                      mud->sprite_media + 10);

            if (cycle > 2816) {
                surface_draw_sprite_alpha_from4(mud->surface, 0, 10,
                                                mud->sprite_logo, cycle - 2816);
            }
        }
    }

    if (mud->login_screen == 0) {
        panel_draw_panel(mud->panel_login_welcome);
    } else if (mud->login_screen == 1) {
        panel_draw_panel(mud->panel_login_new_user);
    } else if (mud->login_screen == 2) {
        panel_draw_panel(mud->panel_login_existing_user);
    }

    // blue bar
    surface_draw_sprite_from3(mud->surface, 0, mud->game_height - 4,
                              mud->sprite_media + 22);

    surface_draw(mud->surface);
}

void mudclient_reset_game(mudclient *mud) {
    mud->system_update = 0;
    mud->combat_style = 0;
    mud->logout_timeout = 0;
    mud->login_screen = 0;
    mud->logged_in = 1;
    mud->input_pm_current[0] = '\0';
    mud->input_pm_final[0] = '\0';

    surface_black_screen(mud->surface);
    surface_draw(mud->surface);

    for (int i = 0; i < mud->object_count; i++) {
        scene_remove_model(mud->scene, mud->object_model[i]);

        world_remove_object(mud->world, mud->object_x[i], mud->object_y[i],
                            mud->object_id[i]);
    }

    for (int i = 0; i < mud->wall_object_count; i++) {
        scene_remove_model(mud->scene, mud->wall_object_model[i]);

        world_remove_wall_object(
            mud->world, mud->wall_object_x[i], mud->wall_object_y[i],
            mud->wall_object_direction[i], mud->wall_object_id[i]);
    }

    mud->object_count = 0;
    mud->wall_object_count = 0;
    mud->ground_item_count = 0;
    mud->player_count = 0;

    for (int i = 0; i < PLAYERS_SERVER_MAX; i++) {
        free(mud->player_server[i]);
        mud->player_server[i] = NULL;
    }

    for (int i = 0; i < PLAYERS_MAX; i++) {
        free(mud->players[i]);
        mud->players[i] = NULL;
    }

    mud->npc_count = 0;

    for (int i = 0; i < NPCS_SERVER_MAX; i++) {
        free(mud->npcs_server[i]);
        mud->npcs_server[i] = NULL;
    }

    for (int i = 0; i < NPCS_MAX; i++) {
        free(mud->npcs[i]);
        mud->npcs[i] = NULL;
    }

    for (int i = 0; i < PRAYER_COUNT; i++) {
        mud->prayer_on[i] = 0;
    }

    mud->mouse_button_click = 0;
    mud->last_mouse_button_down = 0;
    mud->mouse_button_down = 0;
    // mud->show_dialog_shop = 0;
    // mud->show_dialog_bank = 0;
    mud->is_sleeping = 0;
    mud->friend_list_count = 0;
}

void mudclient_login(mudclient *mud, char *username, char *password,
                     int reconnecting) {
    if (mud->world_full_timeout > 0) {
        mudclient_show_login_screen_status(mud, "Please wait...",
                                           "Connecting to server");

        sleep(2);

        mudclient_show_login_screen_status(
            mud, "Sorry! the server is currently full.",
            "Please try again later");

        return;
    }

    char formatted_username[21];
    format_auth_string(username, 20, formatted_username);

    char formatted_password[21];
    format_auth_string(password, 20, formatted_password);

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

    mud->packet_stream = malloc(sizeof(PacketStream));
    packet_stream_new(mud->packet_stream, mud);

    if (mud->packet_stream->closed) {
        mudclient_show_login_screen_status(
            mud, "Sorry! Unable to connect.",
            "Check internet settings or try another world");
        return;
    }

    packet_stream_new_packet(mud->packet_stream, CLIENT_SESSION);

    int64_t encoded_username = encode_username(formatted_username);

    packet_stream_put_byte(mud->packet_stream,
                           (int)((encoded_username >> 16) & 31));

    packet_stream_flush_packet(mud->packet_stream);

    int64_t session_id = packet_stream_get_long(mud->packet_stream);
    mud->session_id = session_id;

    if (mud->session_id == 0) {
        mudclient_show_login_screen_status(mud, "Login server offline.",
                                           "Please try again in a few mins");
        return;
    }

    printf("Verb: Session id: %lld\n", session_id);

    int32_t keys[4];
    keys[0] = (((float)rand() / (float)RAND_MAX) * 99999999);
    keys[1] = (((float)rand() / (float)RAND_MAX) * 99999999);
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
    printf("login response:%d\n", response);

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
        // TODO make sure this is ok
        username[0] = '\0';
        password[0] = '\0';
        mudclient_reset_login_screen_variables(mud);
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
        sleep(5);
        mud->auto_login_timeout--;
        mudclient_login(mud, username, password, reconnecting);
    }*/
}

void mudclient_handle_login_screen_input(mudclient *mud) {
    if (mud->world_full_timeout > 0) {
        mud->world_full_timeout--;
    }

    if (mud->login_screen == 0) {
        panel_handle_mouse(mud->panel_login_welcome, mud->mouse_x, mud->mouse_y,
                           mud->last_mouse_button_down, mud->mouse_button_down);

        if (panel_is_clicked(mud->panel_login_welcome,
                             mud->control_welcome_new_user)) {
            mud->login_screen = 1;

            if (mud->options->account_management) {
                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_user, "");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_password, "");

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_confirm_password, "");

                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_user);

                panel_toggle_checkbox(mud->panel_login_new_user,
                                      mud->control_register_checkbox, 0);

                panel_update_text(
                    mud->panel_login_new_user, mud->control_register_status,
                    "To create an account please enter all the requested "
                    "details");
            }
        }

        if (panel_is_clicked(mud->panel_login_welcome,
                             mud->control_welcome_existing_user)) {
            mud->login_screen = 2;

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_status,
                              "Please enter your username and password");

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_user, "");

            panel_update_text(mud->panel_login_existing_user,
                              mud->control_login_password, "");

            panel_set_focus(mud->panel_login_existing_user,
                            mud->control_login_user);

            return;
        }
    } else if (mud->login_screen == 1) {
        panel_handle_mouse(mud->panel_login_new_user, mud->mouse_x,
                           mud->mouse_y, mud->last_mouse_button_down,
                           mud->mouse_button_down);

        if (mud->options->account_management) {
            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_cancel)) {
                mud->login_screen = 0;
                return;
            }

            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_user)) {
                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_password);
                return;
            }

            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_password)) {
                panel_set_focus(mud->panel_login_new_user,
                                mud->control_register_confirm_password);

                return;
            }

            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_confirm_password) ||
                panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_register_submit)) {
                char *username = panel_get_text(mud->panel_login_new_user,
                                                mud->control_register_user);

                char *password = panel_get_text(mud->panel_login_new_user,
                                                mud->control_register_password);

                char *confirm_password =
                    panel_get_text(mud->panel_login_new_user,
                                   mud->control_register_confirm_password);

                int password_length = strlen(password);

                if (strlen(username) == 0 || password_length == 0 ||
                    strlen(confirm_password) == 0) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@Please fill in ALL requested information to "
                        "continue!");

                    return;
                }

                if (strcmp(password, confirm_password) != 0) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@The two passwords entered are not the same as "
                        "each other!");

                    return;
                }

                if (password_length < 5) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@Your password must be at least 5 letters long");

                    return;
                }

                if (!panel_is_activated(mud->panel_login_new_user,
                                        mud->control_register_checkbox)) {
                    panel_update_text(
                        mud->panel_login_new_user, mud->control_register_status,
                        "@yel@You must agree to the terms+conditions to "
                        "continue");

                    return;
                }

                panel_update_text(mud->panel_login_new_user,
                                  mud->control_register_status,
                                  "Please wait... creating new account");

                mudclient_draw_login_screens(mud);
                mudclient_reset_timings(mud);

                // mudclient_register(mud, username, password);
            }
        } else {
            if (panel_is_clicked(mud->panel_login_new_user,
                                 mud->control_login_new_ok)) {
                mud->login_screen = 0;
            }
        }
    } else if (mud->login_screen == 2) {
        panel_handle_mouse(mud->panel_login_existing_user, mud->mouse_x,
                           mud->mouse_y, mud->last_mouse_button_down,
                           mud->mouse_button_down);

        if (panel_is_clicked(mud->panel_login_existing_user,
                             mud->control_login_cancel)) {
            mud->login_screen = 0;
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_user)) {
            panel_set_focus(mud->panel_login_existing_user,
                            mud->control_login_password);
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_password) ||
                   panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_ok)) {
            strcpy(mud->login_user,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_user));
            strcpy(mud->login_pass,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_password));

            mudclient_login(mud, mud->login_user, mud->login_pass, 0);
        } else if (panel_is_clicked(mud->panel_login_existing_user,
                                    mud->control_login_recover)) {
            strcpy(mud->login_user,
                   panel_get_text(mud->panel_login_existing_user,
                                  mud->control_login_user));

            if (strlen(mud->login_user) == 0) {
                mudclient_show_login_screen_status(
                    mud,
                    "You must enter your username to recover your password",
                    "");

                return;
            }

            // mudclient_recover_attempt(mud, mud->login_user);
        }
    }
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
    memset(mud->world, 0, sizeof(World));
    world_new(mud->world, mud->scene, mud->surface);
    mud->world->base_media_sprite = mud->sprite_media;

    mudclient_load_textures(mud);

    // surface_free_colours(mud->surface);

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
        // mudclient_load_sounds(mud);
    }

    if (mud->error_loading_data) {
        return;
    }

    mudclient_show_loading_progress(mud, 100, "Starting game...");
    // this.create_message_tab_panel();
    mudclient_create_login_panels(mud);
    // mudclient_create_appearance_panel(mud);
    mudclient_reset_login_screen_variables(mud);
    mudclient_render_login_screen_viewports(mud);

    free(surface_texture_pixels);
    surface_texture_pixels = NULL;
}

GameModel *mudclient_create_model(mudclient *mud, int x, int y, int direction,
                                  int id, int count) {
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
                               1, 0xffffff);
    // mudclient_draw_chat_message_tabs(mud);
    surface_draw(mud->surface);

    int ax = mud->region_x;
    int ay = mud->region_y;
    int section_x = (lx + 24) / 48;
    int section_y = (ly + 24) / 48;

    mud->last_height_offset = mud->plane_index;
    mud->region_x = section_x * 48 - 48;
    mud->region_y = section_y * 48 - 48;
    mud->local_lower_x = section_x * 48 - 32;
    mud->local_lower_y = section_y * 48 - 32;
    mud->local_upper_x = section_x * 48 + 32;
    mud->local_upper_y = section_y * 48 + 32;

    world_load_section_from3(mud->world, lx, ly, mud->last_height_offset);

    mud->region_x -= mud->plane_width;
    mud->region_y -= mud->plane_height;

    int offset_x = mud->region_x - ax;
    int offset_y = mud->region_y - ay;

    for (int i = 0; i < mud->object_count; i++) {
        mud->object_x[i] -= offset_x;
        mud->object_y[i] -= offset_y;

        int obj_x = mud->object_x[i];
        int obj_y = mud->object_y[i];
        int obj_id = mud->object_id[i];

        GameModel *game_model = mud->object_model[i];

        int obj_type = mud->object_direction[i];
        int obj_w = 0;
        int obj_h = 0;

        if (obj_type == 0 || obj_type == 4) {
            obj_w = game_data_object_width[obj_id];
            obj_h = game_data_object_height[obj_id];
        } else {
            obj_h = game_data_object_width[obj_id];
            obj_w = game_data_object_height[obj_id];
        }

        int j6 = ((obj_x + obj_x + obj_w) * MAGIC_LOC) / 2;
        int k6 = ((obj_y + obj_y + obj_h) * MAGIC_LOC) / 2;

        if (obj_x >= 0 && obj_y >= 0 && obj_x < 96 && obj_y < 96) {
            scene_add_model(mud->scene, game_model);

            game_model_place(game_model, j6,
                             -world_get_elevation(mud->world, j6, k6), k6);

            world_remove_object2(mud->world, obj_x, obj_y, obj_id);

            if (obj_id == 74) {
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

        GameModel *wall_object_model = mudclient_create_model(
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

GameCharacter *mudclient_create_player(mudclient *mud, int server_index, int x,
                                       int y, int animation) {
    if (mud->player_server[server_index] == NULL) {
        GameCharacter *game_character = malloc(sizeof(GameCharacter));
        game_character_new(game_character);

        mud->player_server[server_index] = game_character;
        mud->player_server[server_index]->server_index = server_index;
        mud->player_server[server_index]->server_id = 0;
    }

    GameCharacter *player = mud->player_server[server_index];
    int flag = 0;

    for (int i = 0; i < mud->known_player_count; i++) {
        if (mud->known_players[i]->server_index != server_index) {
            continue;
        }

        flag = 1;
        break;
    }

    if (flag) {
        player->animation_next = animation;
        int j1 = player->waypoint_current;

        if (x != player->waypoints_x[j1] || y != player->waypoints_y[j1]) {
            player->waypoint_current = j1 = (j1 + 1) % 10;
            player->waypoints_x[j1] = x;
            player->waypoints_y[j1] = y;
        }
    } else {
        player->server_index = server_index;
        player->moving_step = 0;
        player->waypoint_current = 0;
        player->waypoints_x[0] = player->current_x = x;
        player->waypoints_y[0] = player->current_y = y;
        player->animation_next = player->animation_current = animation;
        player->step_count = 0;
    }

    mud->players[mud->player_count++] = player;

    return player;
}

void mudclient_check_connection(mudclient *mud) {
    // packet_tick
    int timestamp = get_ticks();

    if (packet_stream_has_packet(mud->packet_stream)) {
        mud->packet_last_read = timestamp;
    }

    if (timestamp - mud->packet_last_read > 5000) {
        mud->packet_last_read = timestamp;
        packet_stream_new_packet(mud->packet_stream, CLIENT_PING);
        packet_stream_send_packet(mud->packet_stream);
    }

    packet_stream_write_packet(mud->packet_stream, 20);

    int size =
        packet_stream_read_packet(mud->packet_stream, mud->incoming_packet);

    if (size <= 0) {
        return;
    }

    int8_t *data = mud->incoming_packet;
    int opcode = data[0] & 0xff; // TODO isaac

    printf("got opcode %d\n", opcode);

    switch (opcode) {
    case SERVER_WORLD_INFO:
        mud->loading_area = 1;
        mud->local_player_server_index = get_unsigned_short(data, 1);
        mud->plane_width = get_unsigned_short(data, 3);
        mud->plane_height = get_unsigned_short(data, 5);
        mud->plane_index = get_unsigned_short(data, 7);
        mud->plane_multiplier = get_unsigned_short(data, 9);
        mud->plane_height -= mud->plane_index * mud->plane_multiplier;
        break;
    case SERVER_REGION_PLAYERS: {
        mud->known_player_count = mud->player_count;

        for (int i = 0; i < mud->known_player_count; i++) {
            mud->known_players[i] = mud->players[i];
        }

        int offset = 8;

        mud->local_region_x = get_bit_mask(data, offset, 11);
        offset += 11;

        mud->local_region_y = get_bit_mask(data, offset, 13);
        offset += 13;

        int sprite = get_bit_mask(data, offset, 4);
        offset += 4;

        int has_loaded_region = mudclient_load_next_region(
            mud, mud->local_region_x, mud->local_region_y);

        mud->local_region_x -= mud->region_x;
        mud->local_region_y -= mud->region_y;

        int player_x = mud->local_region_x * MAGIC_LOC + 64;
        int player_y = mud->local_region_y * MAGIC_LOC + 64;

        if (has_loaded_region) {
            mud->local_player->waypoint_current = 0;
            mud->local_player->moving_step = 0;

            mud->local_player->current_x = mud->local_player->waypoints_x[0] =
                player_x;

            mud->local_player->current_y = mud->local_player->waypoints_y[0] =
                player_y;
        }

        mud->player_count = 0;

        mud->local_player = mudclient_create_player(
            mud, mud->local_player_server_index, player_x, player_y, sprite);

        int length = get_bit_mask(data, offset, 8);
        offset += 8;

        for (int i = 0; i < length; i++) {
            GameCharacter *player = mud->known_players[i + 1];
            int has_updated = get_bit_mask(data, offset, 1);

            offset++;

            if (has_updated != 0) {
                int update_type = get_bit_mask(data, offset, 1);
                offset++;

                if (update_type == 0) {
                    int sprite = get_bit_mask(data, offset, 3);
                    offset += 3;

                    int waypoint_current = player->waypoint_current;
                    int player_x = player->waypoints_x[waypoint_current];
                    int player_y = player->waypoints_y[waypoint_current];

                    if (sprite == 2 || sprite == 1 || sprite == 3) {
                        player_x += MAGIC_LOC;
                    }

                    if (sprite == 6 || sprite == 5 || sprite == 7) {
                        player_x -= MAGIC_LOC;
                    }

                    if (sprite == 4 || sprite == 3 || sprite == 5) {
                        player_y += MAGIC_LOC;
                    }

                    if (sprite == 0 || sprite == 1 || sprite == 7) {
                        player_y -= MAGIC_LOC;
                    }

                    player->animation_next = sprite;

                    player->waypoint_current = waypoint_current =
                        (waypoint_current + 1) % 10;

                    player->waypoints_x[waypoint_current] = player_x;
                    player->waypoints_y[waypoint_current] = player_y;
                } else {
                    int sprite = get_bit_mask(data, offset, 4);

                    if ((sprite & 12) == 12) {
                        offset += 2;
                        continue;
                    }

                    player->animation_next = get_bit_mask(data, offset, 4);
                    offset += 4;
                }
            }

            mud->players[mud->player_count++] = player;
        }

        int player_count = 0;

        while (offset + 24 < size * 8) {
            int server_index = get_bit_mask(data, offset, 11);
            offset += 11;

            int area_x = get_bit_mask(data, offset, 5);
            offset += 5;

            if (area_x > 15) {
                area_x -= 32;
            }

            int area_y = get_bit_mask(data, offset, 5);
            offset += 5;

            if (area_y > 15) {
                area_y -= 32;
            }

            int sprite = get_bit_mask(data, offset, 4);
            offset += 4;

            int is_player_known = get_bit_mask(data, offset, 1);
            offset++;

            int x = (mud->local_region_x + area_x) * MAGIC_LOC + 64;
            int y = (mud->local_region_y + area_y) * MAGIC_LOC + 64;

            mudclient_create_player(mud, server_index, x, y, sprite);

            if (is_player_known == 0) {
                mud->player_server_indexes[player_count++] = server_index;
            }
        }

        if (player_count > 0) {
            packet_stream_new_packet(mud->packet_stream, CLIENT_KNOWN_PLAYERS);
            packet_stream_put_short(mud->packet_stream, player_count);

            for (int i = 0; i < player_count; i++) {
                GameCharacter *player =
                    mud->player_server[mud->player_server_indexes[i]];

                packet_stream_put_short(mud->packet_stream,
                                        player->server_index);
                packet_stream_put_short(mud->packet_stream, player->server_id);
            }

            packet_stream_send_packet(mud->packet_stream);
            // player_count = 0;
        }

        break;
    }
    case SERVER_REGION_PLAYER_UPDATE: {
        int length = get_unsigned_short(data, 1);
        int offset = 3;

        for (int i = 0; i < length; i++) {
            int player_index = get_unsigned_short(data, offset);
            offset += 2;

            GameCharacter *player = mud->player_server[player_index];
            int update_type = data[offset++];

            if (update_type == 0) {
                // speech bubble with an item in it
                int item_id = get_unsigned_short(data, offset);
                offset += 2;

                if (player != NULL) {
                    player->bubble_timeout = 150;
                    player->bubble_item = item_id;
                }
            } else if (update_type == 1) {
                // chat
                int message_length = data[offset++];

                /*if (player != NULL) {
                    int message = chat_message->descramble(
                        data,
                        offset,
                        message_length
                    );

                    if (mud->options->word_filter) {
                        message = word_filter->filter(message);
                    }

                    int ignored = 0;

                    for (int i = 0; i < mud->ignore_list_count; i++) {
                        if (mud->ignore_list[i] == player->hash) {
                            ignored = 1;
                            break;
                        }
                    }

                    if (!ignored) {
                        player->message_timeout = 150;
                        player->message = message;
                        mud->show_message(
                            `${player->name}: ${player->message}`,
                            2
                        );
                    }
                }*/

                offset += message_length;
            } else if (update_type == 2) {
                // combat damage and hp
                int damage = get_unsigned_byte(data[offset++]);
                int current = get_unsigned_byte(data[offset++]);
                int max = get_unsigned_byte(data[offset++]);

                if (player != NULL) {
                    player->damage_taken = damage;
                    player->health_current = current;
                    player->health_max = max;
                    player->combat_timer = 200;

                    if (player == mud->local_player) {
                        mud->player_stat_current[3] = current;
                        mud->player_stat_base[3] = max;
                        // mud->show_dialog_welcome = 0;
                        // mud->show_dialog_server_message = 0;
                    }
                }
            } else if (update_type == 3) {
                // new incoming projectile to npc
                int projectile_sprite = get_unsigned_short(data, offset);
                offset += 2;

                int npc_index = get_unsigned_short(data, offset);
                offset += 2;

                if (player != NULL) {
                    player->incoming_projectile_sprite = projectile_sprite;
                    player->attacking_npc_server_index = npc_index;
                    player->attacking_player_server_index = -1;
                    player->projectile_range = PROJECTILE_RANGE_MAX;
                }
            } else if (update_type == 4) {
                // new incoming projectile from player
                int projectile_sprite = get_unsigned_short(data, offset);
                offset += 2;

                int opponent_index = get_unsigned_short(data, offset);
                offset += 2;

                if (player != NULL) {
                    player->incoming_projectile_sprite = projectile_sprite;
                    player->attacking_player_server_index = opponent_index;
                    player->attacking_npc_server_index = -1;
                    player->projectile_range = PROJECTILE_RANGE_MAX;
                }
            } else if (update_type == 5) {
                // player appearance update
                if (player != NULL) {
                    player->server_id = get_unsigned_short(data, offset);
                    offset += 2;

                    player->hash = get_unsigned_long(data, offset);
                    offset += 8;

                    decode_username(player->hash, player->name);

                    int equipped_count = get_unsigned_byte(data[offset]);
                    offset++;

                    for (int j = 0; j < equipped_count; j++) {
                        player->equipped_item[j] =
                            get_unsigned_byte(data[offset++]);
                    }

                    for (int j = equipped_count; j < 12; j++) {
                        player->equipped_item[j] = 0;
                    }

                    player->colour_hair = data[offset++] & 0xff;
                    player->colour_top = data[offset++] & 0xff;
                    player->colour_bottom = data[offset++] & 0xff;
                    player->colour_skin = data[offset++] & 0xff;
                    player->level = data[offset++] & 0xff;
                    player->skull_visible = data[offset++] & 0xff;
                } else {
                    offset += 14;

                    int unused = get_unsigned_byte(data[offset]);
                    offset += unused + 1;
                }
            } else if (update_type == 6) {
                // public chat
                int message_length = data[offset++];

                /*if (player != NULL) {
                    int message = chat_message_decode(
                        data,
                        offset,
                        message_length
                    );

                    player->message_timeout = 150;
                    player->message = message;

                    if (player == mud->local_player) {
                        mudclient_show_message(
                            `${player->name}: ${player->message}`,
                            5
                        );
                    }
                }*/

                offset += message_length;
            }
        }
        break;
    }
    }
}

void mudclient_handle_game_input(mudclient *mud) {
    if (mud->system_update > 1) {
        mud->system_update--;
    }

    mudclient_check_connection(mud);

    if (mud->logout_timeout > 0) {
        mud->logout_timeout--;
    }

    if (mud->mouse_action_timeout > 4500 && mud->combat_timeout == 0 &&
        mud->logout_timeout == 0) {
        mud->mouse_action_timeout -= 500;
        // mudclient_send_logout(mud);
        return;
    }

    if (mud->local_player->animation_current == 8 ||
        mud->local_player->animation_current == 9) {
        mud->combat_timeout = 500;
    }

    if (mud->combat_timeout > 0) {
        mud->combat_timeout--;
    }

    /*
    if (mud->show_appearance_change) {
        mudclient_handle_appearance_panel_input(mud);
        return;
    }*/

    /* TODO put this in a function */
    for (int i = 0; i < mud->player_count; i++) {
        GameCharacter *player = mud->players[i];

        int k = (player->waypoint_current + 1) % 10;

        if (player->moving_step != k) {
            int i1 = -1;
            int l2 = player->moving_step;
            int j4;

            if (l2 < k) {
                j4 = k - l2;
            } else {
                j4 = (10 + k) - l2;
            }

            int j5 = 4;

            if (j4 > 2) {
                j5 = (j4 - 1) * 4;
            }

            if (player->waypoints_x[l2] - player->current_x > MAGIC_LOC * 3 ||
                player->waypoints_y[l2] - player->current_y > MAGIC_LOC * 3 ||
                player->waypoints_x[l2] - player->current_x < -MAGIC_LOC * 3 ||
                player->waypoints_y[l2] - player->current_y < -MAGIC_LOC * 3 ||
                j4 > 8) {
                player->current_x = player->waypoints_x[l2];
                player->current_y = player->waypoints_y[l2];
            } else {
                if (player->current_x < player->waypoints_x[l2]) {
                    player->current_x += j5;
                    player->step_count++;
                    i1 = 2;
                } else if (player->current_x > player->waypoints_x[l2]) {
                    player->current_x -= j5;
                    player->step_count++;
                    i1 = 6;
                }

                if (player->current_x - player->waypoints_x[l2] < j5 &&
                    player->current_x - player->waypoints_x[l2] > -j5) {
                    player->current_x = player->waypoints_x[l2];
                }

                if (player->current_y < player->waypoints_y[l2]) {
                    player->current_y += j5;
                    player->step_count++;

                    if (i1 == -1) {
                        i1 = 4;
                    } else if (i1 == 2) {
                        i1 = 3;
                    } else {
                        i1 = 5;
                    }
                } else if (player->current_y > player->waypoints_y[l2]) {
                    player->current_y -= j5;
                    player->step_count++;

                    if (i1 == -1) {
                        i1 = 0;
                    } else if (i1 == 2) {
                        i1 = 1;
                    } else {
                        i1 = 7;
                    }
                }

                if (player->current_y - player->waypoints_y[l2] < j5 &&
                    player->current_y - player->waypoints_y[l2] > -j5) {
                    player->current_y = player->waypoints_y[l2];
                }
            }

            if (i1 != -1) {
                player->animation_current = i1;
            }

            if (player->current_x == player->waypoints_x[l2] &&
                player->current_y == player->waypoints_y[l2]) {
                player->moving_step = (l2 + 1) % 10;
            }
        } else {
            player->animation_current = player->animation_next;
        }

        if (player->message_timeout > 0) {
            player->message_timeout--;
        }

        if (player->bubble_timeout > 0) {
            player->bubble_timeout--;
        }

        if (player->combat_timer > 0) {
            player->combat_timer--;
        }

        /*
        if (mud->death_screen_timeout > 0) {
            mud->death_screen_timeout--;

            if (mud->death_screen_timeout == 0) {
                mudclient_show_message(mud,
                                       "You have been granted another life. Be "
                                       "more careful this time!",
                                       3);

                mudclient_show_message(
                    mud,
                    "You retain your skills. Your objects land where you died",
                    3);
            }
        }*/
    }

    for (int i = 0; i < mud->npc_count; i++) {
        GameCharacter *npc = mud->npcs[i];
        int j1 = (npc->waypoint_current + 1) % 10;

        if (npc->moving_step != j1) {
            int i3 = -1;
            int k4 = npc->moving_step;
            int k5 = (k4 < j1 ? j1 - k4 : 10 + j1) - k4;
            int l5 = 4;

            if (k5 > 2) {
                l5 = (k5 - 1) * 4;
            }

            if (npc->waypoints_x[k4] - npc->current_x > MAGIC_LOC * 3 ||
                npc->waypoints_y[k4] - npc->current_y > MAGIC_LOC * 3 ||
                npc->waypoints_x[k4] - npc->current_x < -MAGIC_LOC * 3 ||
                npc->waypoints_y[k4] - npc->current_y < -MAGIC_LOC * 3 ||
                k5 > 8) {
                npc->current_x = npc->waypoints_x[k4];
                npc->current_y = npc->waypoints_y[k4];
            } else {
                if (npc->current_x < npc->waypoints_x[k4]) {
                    npc->current_x += l5;
                    npc->step_count++;
                    i3 = 2;
                } else if (npc->current_x > npc->waypoints_x[k4]) {
                    npc->current_x -= l5;
                    npc->step_count++;
                    i3 = 6;
                }

                if (npc->current_x - npc->waypoints_x[k4] < l5 &&
                    npc->current_x - npc->waypoints_x[k4] > -l5) {
                    npc->current_x = npc->waypoints_x[k4];
                }

                if (npc->current_y < npc->waypoints_y[k4]) {
                    npc->current_y += l5;
                    npc->step_count++;

                    if (i3 == -1) {
                        i3 = 4;
                    } else if (i3 == 2) {
                        i3 = 3;
                    } else {
                        i3 = 5;
                    }
                } else if (npc->current_y > npc->waypoints_y[k4]) {
                    npc->current_y -= l5;
                    npc->step_count++;

                    if (i3 == -1) {
                        i3 = 0;
                    } else if (i3 == 2) {
                        i3 = 1;
                    } else {
                        i3 = 7;
                    }
                }

                if (npc->current_y - npc->waypoints_y[k4] < l5 &&
                    npc->current_y - npc->waypoints_y[k4] > -l5) {
                    npc->current_y = npc->waypoints_y[k4];
                }
            }

            if (i3 != -1) {
                npc->animation_current = i3;
            }

            if (npc->current_x == npc->waypoints_x[k4] &&
                npc->current_y == npc->waypoints_y[k4]) {
                npc->moving_step = (k4 + 1) % 10;
            }
        } else {
            npc->animation_current = npc->animation_next;

            if (npc->npc_id == 43) {
                npc->step_count++;
            }
        }

        if (npc->message_timeout > 0) {
            npc->message_timeout--;
        }

        if (npc->bubble_timeout > 0) {
            npc->bubble_timeout--;
        }

        if (npc->combat_timer > 0) {
            npc->combat_timer--;
        }
    }

    /*
    if (mud->show_ui_tab != 2) {
        if (surface->an_int346 > 0) {
            mud->sleep_word_delay_timer++;
        }

        if (surface->an_int347 > 0) {
            mud->sleep_word_delay_timer = 0;
        }

        surface->an_int346 = 0;
        surface->an_int347 = 0;
    }*/

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

    if (mud->option_camera_mode_auto) {
        int k1 = mud->camera_angle * 32;
        int j3 = k1 - mud->camera_rotation;
        int byte0 = 1;

        if (j3 != 0) {
            mud->an_int_707++;

            if (j3 > 128) {
                byte0 = -1;
                j3 = 256 - j3;
            } else if (j3 > 0)
                byte0 = 1;
            else if (j3 < -128) {
                byte0 = 1;
                j3 = 256 + j3;
            } else if (j3 < 0) {
                byte0 = -1;
                j3 = -j3;
            }

            mud->camera_rotation +=
                ((mud->an_int_707 * j3 + 255) / 256) * byte0;

            mud->camera_rotation &= 0xff;
        } else {
            mud->an_int_707 = 0;
        }
    }

    /*
    if (mud->sleep_word_delay_timer > 20) {
        mud->sleep_word_delay = 0;
        mud->sleep_word_delay_timer = 0;
    }*/
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

    if (mud->message_tab_flash_all > 0) {
        mud->message_tab_flash_all--;
    }

    if (mud->message_tab_flash_history > 0) {
        mud->message_tab_flash_history--;
    }

    if (mud->message_tab_flash_quest > 0) {
        mud->message_tab_flash_quest--;
    }

    if (mud->message_tab_flash_private > 0) {
        mud->message_tab_flash_private--;
    }
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
        mud->object_model[object_index] = game_model;
    }
}

void mudclient_draw_game(mudclient *mud) {
    /*
    if (mud->death_screen_timeout != 0) {
        surface_fade_to_black(mud->surface);
        surface_draw_string_center(mud->surface, "Oh dear! You are dead...",
                                   mud->game_width / 2, mud->game_height / 2, 7,
                                   0xff0000);
        mudclient_draw_chat_message_tabs(mud);
        surface_draw(mud->surface);

        return;
    }

    if (mud->show_appearance_change) {
        mudclient_draw_appearance_panel_character_sprites(mud);
        return;
    }

    if (mud->is_sleeping) {
        mudclient_draw_sleep(mud);
        return;
    }*/

    if (!mud->world->player_alive) {
        return;
    }

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

    if (mud->object_animation_cycle != mud->last_object_animation_cycle) {
        mud->last_object_animation_cycle = mud->object_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->object_id[i] == 97) {
                char name[17];
                sprintf(name, "firea%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == 274) {
                char name[22];
                sprintf(name, "fireplacea%d",
                        (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == 1031) {
                char name[21];
                sprintf(name, "lightning%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == 1036) {
                char name[21];
                sprintf(name, "firespell%d", (mud->object_animation_cycle + 1));
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == 1147) {
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
            if (mud->object_id[i] == 51) {
                char name[18];
                sprintf(name, "torcha%d", mud->torch_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            } else if (mud->object_id[i] == 143) {
                char name[23];
                sprintf(name, "skulltorcha%d", mud->torch_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    if (mud->claw_animation_cycle != mud->last_claw_animation_cycle) {
        mud->last_claw_animation_cycle = mud->claw_animation_cycle;

        for (int i = 0; i < mud->object_count; i++) {
            if (mud->object_id[i] == 1142) {
                char name[21];
                sprintf(name, "clawspell%d", mud->claw_animation_cycle + 1);
                mudclient_update_object_animation(mud, i, name);
            }
        }
    }

    scene_reduce_sprites(mud->scene, mud->sprite_count);
    mud->sprite_count = 0;

    /* TODO entity + bubble drawing */

    mud->surface->interlace = 0;
    surface_black_screen(mud->surface);
    mud->surface->interlace = mud->interlace;

    if (mud->last_height_offset == 3) {
        int ambience = 40 + ((float)rand() / (float)RAND_MAX) * 3;
        int diffuse = 40 + ((float)rand() / (float)RAND_MAX) * 7;

        scene_set_light_from5(mud->scene, ambience, diffuse, -50, -10, -50);
    }

    mud->items_above_head_count = 0;
    mud->received_messages_count = 0;
    mud->health_bar_count = 0;

    if (mud->option_camera_mode_auto && !mud->fog_of_war) {
        // mudclient_auto_rotate_camera(mud);
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

    scene_render(mud->scene);

    // mudclient_draw_overhead_sprites(mud);

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

        char fps[10];
        sprintf(fps, "Fps: %d", mud->fps);

        surface_draw_string(mud->surface, fps, mud->game_width - 62 - offset_x,
                            mud->game_height - 10, 1, 0xffff00);
    }

    /*
    if (this.systemUpdate != 0) {
        let seconds = ((this.systemUpdate / 50) | 0);
        const minutes = (seconds / 60) | 0;

        seconds %= 60;

        if (seconds < 10) {
            this.surface.drawStringCenter('System update in: ' + minutes + ':0'
    + seconds, 256, this.gameHeight - 7, 1, 0xffff00); } else {
            this.surface.drawStringCenter('System update in: ' + minutes + ':' +
    seconds, 256, this.gameHeight - 7, 1, 0xffff00);
        }
    }*/

    if (!mud->loading_area) {
        int j6 =
            2203 - (mud->local_region_y + mud->plane_height + mud->region_y);

        if (mud->local_region_x + mud->plane_width + mud->region_x >= 2640) {
            j6 = -50;
        }

        mud->is_in_wild = j6 > 0;

        if (mud->is_in_wild) {
            surface_draw_sprite_from3(mud->surface, mud->game_width - 59,
                                      mud->game_height - 56,
                                      mud->sprite_media + 13);

            surface_draw_string_centre(mud->surface, "Wilderness",
                                       mud->game_width - 47,
                                       mud->game_height - 20, 1, 0xffff00);

            int wilderness_level = 1 + (j6 / 6);
            char wilderness_label[255];
            sprintf(wilderness_label, "Level: %d", wilderness_level);

            surface_draw_string_centre(mud->surface, wilderness_label,
                                       mud->game_width - 47,
                                       mud->game_height - 7, 1, 0xffff00);

            if (mud->show_ui_wild_warn == 0) {
                mud->show_ui_wild_warn = 2;
            }
        }

        if (mud->show_ui_wild_warn == 0 && j6 > -10 && j6 <= 0) {
            mud->show_ui_wild_warn = 1;
        }
    }

    // mudclient_draw_chat_message_tabs_panel(mud);

    surface_draw_sprite_alpha_from4(mud->surface,
                                    mud->surface->width2 - 3 - 197, 3,
                                    mud->sprite_media, 128);

    // mudclient_draw_ui(mud);
    mud->surface->logged_in = 0;
    surface_draw(mud->surface);
}

void mudclient_draw(mudclient *mud) {
    if (mud->error_loading_data) {
        /* TODO draw error */
        printf("ERROR LOADING DATA\n");
        return;
    }

    if (mud->logged_in == 0) {
        mud->surface->logged_in = 0;
        mudclient_draw_login_screens(mud);
    } else if (mud->logged_in == 1) {
        mud->surface->logged_in = 1;
        mudclient_draw_game(mud);
    }
}

void mudclient_poll_events(mudclient *mud) {
#ifdef WII
#else
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
            mudclient_key_pressed(mud, char_code, code);
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

void mudclient_run(mudclient *mud) {
    if (mud->loading_step == 1) {
        mud->loading_step = 2;
        mudclient_load_jagex(mud);
        mudclient_draw_loading_screen(mud, 0, "Loading...");
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
            j = (2560 * mud->target_fps) / (time - mud->timings[i]);
        }

        if (j < 25) {
            j = 25;
        }

        if (j > 256) {
            j = 256;
            delay = mud->target_fps - (time - mud->timings[i] / 10);

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

        // calculate fps
        mud->fps = (1000 * j) / (mud->target_fps * 256);

        mud->mouse_scroll_delta = 0;
    }
}

void mudclient_sort_friends(mudclient *mud) {
    int flag = 1;

    while (flag) {
        flag = 0;

        for (int i = 0; i < mud->friend_list_count - 1; i++) {
            if ((mud->friend_list_online[i] != 255 &&
                 mud->friend_list_online[i + 1] == 255) ||
                (mud->friend_list_online[i] == 0 &&
                 mud->friend_list_online[i + 1] != 0)) {
                int online_status = mud->friend_list_online[i];
                mud->friend_list_online[i] = mud->friend_list_online[i + 1];
                mud->friend_list_online[i + 1] = online_status;

                int64_t encoded_username = mud->friend_list_hashes[i];
                mud->friend_list_hashes[i] = mud->friend_list_hashes[i + 1];
                mud->friend_list_hashes[i + 1] = encoded_username;

                flag = 1;
            }
        }
    }
}

void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id) {}

void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id) {}

void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int tx, int ty) {}

void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int tx, int ty) {}

int main(int argc, char **argv) {
    srand(0);

    init_utility_global();
    init_surface_global();
    init_world_global();
    init_mudclient_global();

    mudclient *mud = malloc(sizeof(mudclient));
    mudclient_new(mud);
    mudclient_start_application(mud, MUD_WIDTH, MUD_HEIGHT,
                                "Runescape by Andrew Gower");
    mudclient_run(mud);

    exit(0);
}
