#include "worldlist.h"
#include <stdio.h>

#ifndef EMSCRIPTEN
#define EMSCRIPTEN 0
#endif

struct server_type {
    char name[32];
    char host[64];
    int port;
    char rsa_exponent[512];
    char rsa_modulus[512];
};

static struct server_type list[256] = {0};

static void worldlist_path(char *path);
static void worldlist_set_defaults(void);
static void worldlist_read_presets(struct mudclient *mud);

static void worldlist_path(char *path) {
#ifdef ANDROID
    char *pref_path = SDL_GetPrefPath("scape2003", "mudclient");
    snprintf(path, PATH_MAX, "%sworlds.cfg", pref_path);
    SDL_free(pref_path);
#elif defined(EMSCRIPTEN)
    snprintf(path, PATH_MAX, "/options/worlds.cfg");
#elif defined(OPTIONS_UNIX)
    const char *xdg = getenv("XDG_CONFIG_HOME");

    if (xdg != NULL) {
        snprintf(path, PATH_MAX, "%s/rsc-c", xdg);
        (void)mkdir(path, S_IRUSR | S_IWUSR | S_IXUSR);
        snprintf(path, PATH_MAX, "%s/rsc-c/worlds.cfg", xdg);
    } else {
        const char *home = getenv("HOME");

        if (home == NULL) {
            home = "";
        }

        snprintf(path, PATH_MAX, "%s/.config/rsc-c", home);
        (void)mkdir(path, S_IRUSR | S_IWUSR | S_IXUSR);
        snprintf(path, PATH_MAX, "%s/.config/rsc-c/worlds.cfg", home);
    }
#else
    snprintf(path, PATH_MAX, "%s", "./worlds.cfg");
#endif

}

static void worldlist_set_defaults(void) {
    strcpy(list[0].name, "OpenRSC Preservation");
    strcpy(list[0].host, "game.openrsc.com");
    list[0].port = EMSCRIPTEN ? 43496 : 43596; /* websockets */
    strcpy(list[0].rsa_exponent, "00010001");
    strcpy(list[0].rsa_modulus, "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41fefbdc5fed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25");

    strcpy(list[1].name, "OpenRSC Uranium");
    strcpy(list[1].host, "game.openrsc.com");
    list[1].port = EMSCRIPTEN ? 43435 : 43235;
    strcpy(list[1].rsa_exponent, "00010001");
    strcpy(list[1].rsa_modulus, "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41fefbdc5fed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25");

    strcpy(list[2].name, "Neat F2P");
    strcpy(list[2].host, "192.3.118.9");
    list[2].port = EMSCRIPTEN ? 43494 : 43594;
    strcpy(list[2].rsa_exponent, "00010001");
    strcpy(list[2].rsa_modulus, "86b03ac30518bdb3e508ca9660efc7738a73ee7dbedbcebf8c56d030a2bdae70503c60829b7fb5eceb529442234c21bce6d529c8da4fce870e83ceffc379e281");
}

static void worldlist_read_presets(struct mudclient *mud) {
    char path[PATH_MAX];
    int num = 0;

    panel_clear_list(mud->panel_login_worldlist, mud->control_list_worlds);

    worldlist_path(path);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        worldlist_set_defaults();
        goto end;
    }
    for (;;) {
        int res = fscanf(file, "%30s %60s %d %500s %500s\n",
            list[num].name, list[num].host, &list[num].port,
            list[num].rsa_exponent, list[num].rsa_modulus);
        if (res <= 0) {
            break;
        }
        for (int i = 0; list[num].name[i] != '\0'; ++i) {
            if (list[num].name[i] == '_') {
                list[num].name[i] = ' ';
            }
        }
        num++;
    }
    fclose(file);

end:
    for (int i = 0; list[i].name[0] != '\0'; ++i) {
        panel_add_list_entry(mud->panel_login_worldlist,
                             mud->control_list_worlds, i,
                             list[i].name);
    }
}

void worldlist_new(struct mudclient *mud) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int login_background_height = is_compact ? 125 : 200;

    int x = (is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH) / 2;
    int y = login_background_height + 18;

    mud->panel_login_worldlist = malloc(sizeof(Panel));
    assert(mud->panel_login_worldlist != NULL);
    panel_new(mud->panel_login_worldlist, mud->surface, 10);

    panel_add_text_centre(
        mud->panel_login_worldlist, x, y, "Select a world:", FONT_BOLD_12, 1);
    y += 12;

    int button_x = (is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH) - 36;

    int button_y =
        is_compact ? MUD_MIN_HEIGHT - 36 : MUD_VANILLA_HEIGHT - 32;

    panel_add_button_background(mud->panel_login_worldlist,
        button_x, button_y, 60, 20);
    panel_add_text_centre(mud->panel_login_worldlist, button_x, button_y,
                              "Back", FONT_BOLD_12, 0);

    mud->control_worldlist_button = panel_add_button( 
        mud->panel_login_worldlist, button_x, button_y, 60, 20);

    mud->control_list_worlds = panel_add_text_list_interactive(
        mud->panel_login_worldlist, x - 150, y, 250, 170, FONT_REGULAR_11, 256, 1);
    worldlist_read_presets(mud);
}
