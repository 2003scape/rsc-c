#include "mudclient.h"

char *font_files[] = {"h11p.jf", "h12b.jf", "h12p.jf", "h13b.jf",
                      "h14b.jf", "h16b.jf", "h20b.jf", "h24b.jf"};

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
    mud->applet_width = 512;
    mud->applet_height = 346;
    mud->target_fps = 20;
    mud->max_draw_time = 100;
    mud->loading_step = 1;
    mud->loading_progess_text = "Loading";
    mud->thread_sleep = 1;
    mud->camera_rotation = 128;

    memset(mud->input_text_current, '\0', INPUT_TEXT_LENGTH + 1);
    memset(mud->input_pm_current, '\0', INPUT_PM_LENGTH + 1);
    memset(mud->input_text_final, '\0', INPUT_TEXT_LENGTH + 1);
    memset(mud->input_pm_final, '\0', INPUT_PM_LENGTH + 1);

    mud->server = "127.0.0.1";
    mud->port = 43594;
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

    if (mud->options.middle_click_camera && button == 1) {
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

void mudclient_run(mudclient *mud) {
    if (mud->loading_step == 1) {
        mud->loading_step = 2;
        mudclient_load_jagex(mud);
        mudclient_draw_loading_screen(mud, 0, "Loading...");
        // mudclient_start_game();
        mud->loading_step = 0;
    }

    int i = 0;
    int j = 256;
    int delay = 1;
    int i1 = 0;

    for (int j1 = 0; j1 < 10; j1++) {
        mud->timings[j1] = SDL_GetTicks();
    }
}

void mudclient_load_jagex(mudclient *mud) {
    // fill black rect
    int8_t *jagex_jag =
        mudclient_read_data_file(mud, "jagex.jag", "Jagex library", 0);

    if (jagex_jag != NULL) {
        int8_t *logo_tga = load_data("logo.tga", 0, jagex_jag);
        free(jagex_jag);
        // mud->image_logo = mudclient_parse_tga(logo_tga);
    }

    char font_file[13];
    sprintf(font_file, "fonts%d.jag", FONTS);

    int8_t *fonts_jag =
        mudclient_read_data_file(mud, font_file, "Game fonts", 5);

    if (fonts_jag != NULL) {
        for (int i = 0; i < FONT_COUNT; i++) {
            create_font(load_data(font_files[i], 0, fonts_jag), i);
        }

        free(fonts_jag);
    }
}

void mudclient_draw_loading_screen(mudclient *mud, int percent, char *text) {}

void mudclient_show_loading_progress(mudclient *mud, int percent, char *text) {}

void mudclient_draw_string(mudclient *mud, char *string, int font, int x,
                           int y) {}

/* used for the jagex logo in the loading screen */
void mudclient_parse_tga(mudclient *mud, int8_t *tga_buffer) {}

int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent) {
    char loading_text[35]; /* max description is 19 */

    sprintf(loading_text, "Loading %s - 0%", description);
    mudclient_show_loading_progress(mud, percent, loading_text);

    int file_length = strlen(file);
    char *prefix = "./cache";
    char prefixed_file[file_length + strlen(prefix) + 1];
    sprintf(prefixed_file, "%s/%s", prefix, file);

    FILE *archive_stream = fopen(prefixed_file, "rb");

    if (archive_stream == NULL) {
        fprintf(stderr, "unable to read file: %s\n", prefixed_file);
        exit(1);
    }

    int8_t header[6];
    fread(header, 6, 1, archive_stream);

    int archive_size = ((header[0] & 0xff) << 16) + ((header[1] & 0xff) << 8) +
                       (header[2] & 0xff);

    int archive_size_compressed = ((header[3] & 0xff) << 16) +
                                  ((header[4] & 0xff) << 8) +
                                  (header[5] & 0xff);

    sprintf(loading_text, "Loading %s - 5%", description);
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
    init_world_global();
    init_mudclient_global();

    mudclient *mud = malloc(sizeof(mudclient));
    mudclient_new(mud);
    mudclient_start_application(mud, 512, 346, "Runescape by Andrew Gower");
    mudclient_run(mud);
}
