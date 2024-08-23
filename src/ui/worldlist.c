#include "worldlist.h"
#include <stdio.h>

#ifdef EMSCRIPTEN
#define USE_WEBSOCKS 1
#else
#define USE_WEBSOCKS 0
#endif

struct server_type {
    char name[32];
    char host[64];
    int port;
    char rsa_exponent[512];
    char rsa_modulus[512];
};

static struct server_type list[256] = {0};

static void worldlist_set_defaults(void);
static void worldlist_read_presets(struct mudclient *mud);

static void worldlist_set_defaults(void) {
    strcpy(list[0].name, "OpenRSC_Preservation");
    strcpy(list[0].host, "game.openrsc.com");
    list[0].port = USE_WEBSOCKS ? 43496 : 43596; /* websockets */
    strcpy(list[0].rsa_exponent, "00010001");
    strcpy(list[0].rsa_modulus, "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41fefbdc5fed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25");

    strcpy(list[1].name, "OpenRSC_Uranium");
    strcpy(list[1].host, "game.openrsc.com");
    list[1].port = USE_WEBSOCKS ? 43435 : 43235;
    strcpy(list[1].rsa_exponent, "00010001");
    strcpy(list[1].rsa_modulus, "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41fefbdc5fed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25");

    strcpy(list[2].name, "Neat_F2P");
    strcpy(list[2].host, "192.3.118.9");
    list[2].port = USE_WEBSOCKS ? 43494 : 43594;
    strcpy(list[2].rsa_exponent, "00010001");
    strcpy(list[2].rsa_modulus, "86b03ac30518bdb3e508ca9660efc7738a73ee7dbedbcebf8c56d030a2bdae70503c60829b7fb5eceb529442234c21bce6d529c8da4fce870e83ceffc379e281");
}

static void worldlist_read_presets(struct mudclient *mud) {
    char path[PATH_MAX];
    int num = 0;

    panel_clear_list(mud->panel_login_worldlist, mud->control_list_worlds);

    get_config_path("worlds.cfg", path);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        worldlist_set_defaults();
        file = fopen(path, "w");
        if (file != NULL) {
            for (int i = 0; list[i].name[0] != '\0'; ++i) {
                fprintf(file, "%s %s %d %s %s\n",
                    list[i].name, list[i].host, list[i].port,
                    list[i].rsa_exponent, list[i].rsa_modulus);
            }
            fclose(file);
        }
        return;
    }
    for (;;) {
        int res = fscanf(file, "%30s %60s %d %500s %500s\n",
            list[num].name, list[num].host, &list[num].port,
            list[num].rsa_exponent, list[num].rsa_modulus);
        if (res <= 0) {
            break;
        }
        num++;
    }
    fclose(file);
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

    for (int i = 0; list[i].name[0] != '\0'; ++i) {
        for (int j = 0; list[i].name[j] != '\0'; ++j) {
            if (list[i].name[j] == '_') {
                list[i].name[j] = ' ';
            }
        }
        panel_add_list_entry(mud->panel_login_worldlist,
                             mud->control_list_worlds, i,
                             list[i].name);
    }
}
