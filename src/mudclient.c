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
    mud->applet_width = MUD_WIDTH;
    mud->applet_height = MUD_HEIGHT;
    mud->target_fps = 20;
    mud->max_draw_time = 100;
    mud->loading_step = 1;
    mud->loading_progess_text = "Loading";
    mud->thread_sleep = 10;
    mud->camera_rotation = 128;
    mud->server = "127.0.0.1";
    mud->port = 43594;
    mud->members = 0;
    mud->game_width = mud->applet_width;
    mud->game_height = mud->applet_height - 12;
    mud->login_timer = 0;
    mud->camera_rotation_time = 0;
    mud->camera_rotation_x = 0;
    mud->camera_rotation_y = 0;
    mud->camera_rotation_x_increment = 2;
    mud->camera_rotation_y_increment = 2;
    mud->message_tab_flash_all = 0;
    mud->message_tab_flash_history = 0;
    mud->message_tab_flash_quest = 0;
    mud->message_tab_flash_private = 0;

    mud->options = malloc(sizeof(Options));
    options_new(mud->options);

    memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
    memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
    memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);
    memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);
}

void mudclient_start_application(mudclient *mud, int width, int height,
                                 char *title) {
    printf("Started application\n");

    mud->applet_width = width;
    mud->applet_height = height;
    mud->loading_step = 1;

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

    mudclient_run(mud);
}

void mudclient_key_pressed(mudclient *mud, int code, char char_code) {
    if (code == K_LEFT) {
        mud->key_left = 1;
    } else if (code == K_RIGHT) {
        mud->key_right = 1;
    } else {
        // mud_handle_key_press(mud, code);
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

    int file_length = strlen(file);
    char *prefix = "./cache";
    char prefixed_file[file_length + strlen(prefix) + 1];
    sprintf(prefixed_file, "%s/%s", prefix, file);

    FILE *archive_stream = fopen(prefixed_file, "rb");

    if (archive_stream == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", prefixed_file);
        exit(1);
    }

    int8_t header[6];
    fread(header, 6, 1, archive_stream);

    int archive_size = ((header[0] & 0xff) << 16) + ((header[1] & 0xff) << 8) +
                       (header[2] & 0xff);

    int archive_size_compressed = ((header[3] & 0xff) << 16) +
                                  ((header[4] & 0xff) << 8) +
                                  (header[5] & 0xff);

    sprintf(loading_text, "Loading %s - 5%%", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

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

    sprintf(loading_text, "Unpacking %s", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

    if (archive_size_compressed != archive_size) {
        int8_t *decompressed = malloc(archive_size);

        bzip_decompress(decompressed, archive_size, archive_data,
                        archive_size_compressed, 0);

        free(archive_data);

        return decompressed;
    }

    return archive_data;
}

/* used for the jagex logo in the loading screen */
void mudclient_parse_tga(mudclient *mud, int8_t *tga_buffer) {}

void mudclient_load_jagex(mudclient *mud) {
    // fill black rect

    int8_t *jagex_jag =
        mudclient_read_data_file(mud, "jagex.jag", "Jagex library", 0);

    if (jagex_jag != NULL) {
        int8_t *logo_tga = load_data("logo.tga", 0, jagex_jag);
        free(jagex_jag);
        // mud->image_logo = mudclient_parse_tga(logo_tga);
    }

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

    int8_t *filter_jag = mudclient_read_data_file(mud, "filter" FILTER ".jag",
                                                  "Chat system", 15);

    if (filter_jag == NULL) {
        mud->error_loading_data = 1;
        return;
    }

    /* TODO word filter */

    free(filter_jag);
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
        char file_name[15];
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

    free(entity_jag);
    free(entity_jag_mem);
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
    }

    free(textures_jag);
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

    for (int i = 0; i < game_data_model_count; i++) {
        char file_name[255];
        sprintf(file_name, "%s.ob3", game_data_model_name[i]);

        int offset = get_data_file_offset(file_name, models_jag);

        GameModel *game_model = malloc(sizeof(GameModel));
        // memset(game_model, 0, sizeof(GameModel));

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
    mud->login_user_desc = "Please enter a username:";
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

    surface_set_bounds(mud->surface, 0, 0, mud->game_width, mud->game_height + 12);

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

    mud->scene->view = malloc(sizeof(GameModel));
    game_model_from2(mud->scene->view, 1000 * 1000, 1000);

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

    //surface_free_colours(mud->surface);

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
    // this.create_appearance_panel();
    mudclient_reset_login_screen_variables(mud);
    mudclient_render_login_screen_viewports(mud);
}

void mudclient_handle_inputs(mudclient *mud) {
    if (mud->error_loading_data) {
        return;
    }

    mud->login_timer++;

    if (mud->logged_in == 0) {
        mud->mouse_action_timeout = 0;
        // mudclient_handle_login_screen_input(mud);
    } else if (mud->logged_in == 1) {
        mud->mouse_action_timeout++;
        // mudclient_handle_game_input(mud);
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
        // mudclient_draw_game(mud);
    }
}

void mudclient_poll_sdl_events(mudclient *mud) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(0);
            break;
        }
    }
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
        mud->timings[j1] = SDL_GetTicks();
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

        uint32_t time = SDL_GetTicks();

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

        SDL_Delay(delay);

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
            mudclient_poll_sdl_events(mud);
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

/* TODO rest of game-connection */

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
    memset(mud, 0, sizeof(mudclient));
    mudclient_new(mud);
    mudclient_start_application(mud, MUD_WIDTH, MUD_HEIGHT, "Runescape by Andrew Gower");
    mudclient_run(mud);
}
