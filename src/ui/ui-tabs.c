#include "ui-tabs.h"

void mudclient_set_active_ui_tab(mudclient *mud) {
    if (mud->show_ui_tab == 0 && mud->mouse_x >= mud->surface->width - 35 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 &&
        mud->mouse_y < 35) {
        mud->show_ui_tab = INVENTORY_TAB;
    }

    if (mud->show_ui_tab == 0 && mud->mouse_x >= mud->surface->width - 35 - 33 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 33 &&
        mud->mouse_y < 35) {
        mud->show_ui_tab = MAP_TAB;

        mud->minimap_random_rotation =
            (int)((((float)rand() / (float)RAND_MAX)) * 13) - 6;

        mud->minimap_random_scale =
            (int)((((float)rand() / (float)RAND_MAX)) * 23) - 11;
    }

    if (mud->show_ui_tab == 0 && mud->mouse_x >= mud->surface->width - 35 - 66 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 66 &&
        mud->mouse_y < 35) {
        mud->show_ui_tab = STATS_TAB;
    }

    if (mud->show_ui_tab == 0 && mud->mouse_x >= mud->surface->width - 35 - 99 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 99 &&
        mud->mouse_y < 35) {
        mud->show_ui_tab = MAGIC_TAB;
    }

    if (mud->show_ui_tab == 0 && mud->mouse_x >= mud->surface->width - 35 - 132 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 132 &&
        mud->mouse_y < 35) {
        mud->show_ui_tab = SOCIAL_TAB;
    }

    if (mud->show_ui_tab == 0 && mud->mouse_x >= mud->surface->width - 35 - 165 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 165 &&
        mud->mouse_y < 35) {
        mud->show_ui_tab = OPTIONS_TAB;
    }

    if (mud->show_ui_tab != 0 && mud->mouse_x >= mud->surface->width - 35 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 &&
        mud->mouse_y < 26) {
        mud->show_ui_tab = INVENTORY_TAB;
    }

    if (mud->show_ui_tab != 0 && mud->show_ui_tab != MAP_TAB &&
        mud->mouse_x >= mud->surface->width - 35 - 33 && mud->mouse_y >= 3 &&
        mud->mouse_x < mud->surface->width - 3 - 33 && mud->mouse_y < 26) {
        mud->show_ui_tab = MAP_TAB;

        mud->minimap_random_rotation =
            (int)((((float)rand() / (float)RAND_MAX)) * 13) - 6;

        mud->minimap_random_scale =
            (int)((((float)rand() / (float)RAND_MAX)) * 23) - 11;
    }

    if (mud->show_ui_tab != 0 && mud->mouse_x >= mud->surface->width - 35 - 66 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 66 &&
        mud->mouse_y < 26) {
        mud->show_ui_tab = STATS_TAB;
    }

    if (mud->show_ui_tab != 0 && mud->mouse_x >= mud->surface->width - 35 - 99 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 99 &&
        mud->mouse_y < 26) {
        mud->show_ui_tab = MAGIC_TAB;
    }

    if (mud->show_ui_tab != 0 && mud->mouse_x >= mud->surface->width - 35 - 132 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 132 &&
        mud->mouse_y < 26) {
        mud->show_ui_tab = SOCIAL_TAB;
    }

    if (mud->show_ui_tab != 0 && mud->mouse_x >= mud->surface->width - 35 - 165 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 165 &&
        mud->mouse_y < 26) {
        mud->show_ui_tab = OPTIONS_TAB;
    }

    if (mud->show_ui_tab == INVENTORY_TAB &&
        (mud->mouse_x < mud->surface->width - (INVENTORY_WIDTH + 3) ||
         mud->mouse_y > 36 + INVENTORY_HEIGHT)) {
        mud->show_ui_tab = 0;
    }

    if (mud->show_ui_tab == STATS_TAB &&
        (mud->mouse_x < mud->surface->width - 199 || mud->mouse_y > 316)) {
        mud->show_ui_tab = 0;
    }

    if ((mud->show_ui_tab == MAP_TAB || mud->show_ui_tab == MAGIC_TAB ||
         mud->show_ui_tab == 5) &&
        (mud->mouse_x < mud->surface->width - 199 || mud->mouse_y > 240)) {
        mud->show_ui_tab = 0;
    }

    if (mud->show_ui_tab == OPTIONS_TAB &&
        (mud->mouse_x < mud->surface->width - 199 || mud->mouse_y > 311)) {
        mud->show_ui_tab = 0;
    }
}

void mudclient_draw_active_ui_tab(mudclient *mud, int no_menus) {
    if (mud->show_ui_tab == INVENTORY_TAB) {
        mudclient_draw_ui_tab_inventory(mud, no_menus);
    } else if (mud->show_ui_tab == MAP_TAB) {
        mudclient_draw_ui_tab_minimap(mud, no_menus);
    } else if (mud->show_ui_tab == STATS_TAB) {
        mudclient_draw_ui_tab_stats(mud, no_menus);
    } else if (mud->show_ui_tab == MAGIC_TAB) {
        mudclient_draw_ui_tab_magic(mud, no_menus);
    } else if (mud->show_ui_tab == SOCIAL_TAB) {
        mudclient_draw_ui_tab_social(mud, no_menus);
    } else if (mud->show_ui_tab == OPTIONS_TAB) {
        mudclient_draw_ui_tab_options(mud, no_menus);
    }
}
