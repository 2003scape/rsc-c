#include "ui-tabs.h"

char *mudclient_ui_tab_names[] = {"Inventory", "Map",     "Stats",
                                  "Spellbook", "Friends", "Options"};

void mudclient_toggle_ui_tab(mudclient *mud, int tab) {
    if (mud->mouse_button_click != 0) {
        if (mud->show_ui_tab == tab) {
            mud->show_ui_tab = 0;
        } else {
            mud->show_ui_tab = tab;
        }

        mud->mouse_button_click = 0;
    }
}

void mudclient_set_active_ui_tab(mudclient *mud, int no_menus) {
    int is_toggle = mudclient_is_touch(mud);

#if defined(_3DS) || defined(__SWITCH__)
    is_toggle = 1;
#endif

    int offset_start_x = mud->surface->width - UI_BUTTON_SIZE;
    int offset_end_x = mud->surface->width - 3;

    for (int i = 0; i < 6; i++) {
        int tab = i + 1;

        if (mud->mouse_x >= offset_start_x && mud->mouse_x < offset_end_x &&
            mud->mouse_y >= 3 && mud->mouse_y < UI_BUTTON_SIZE) {
            char *tab_name = mudclient_ui_tab_names[i];
            char *page = tab == MAP_TAB ? "RuneScape_Classic_Map" : tab_name;

            if (is_toggle) {
                if (mud->show_ui_tab == tab && mud->selected_wiki && no_menus) {
                    mudclient_menu_add_wiki(mud, tab_name, page);
                } else {
                    mudclient_toggle_ui_tab(mud, tab);
                }

                return;
            }

            if (mud->show_ui_tab == 0) {
                mud->show_ui_tab = tab;

                if (tab == MAP_TAB && mud->options->anti_macro) {
                    mud->minimap_random_rotation =
                        (int)((((float)rand() / (float)RAND_MAX)) * 13) - 6;

                    mud->minimap_random_scale =
                        (int)((((float)rand() / (float)RAND_MAX)) * 23) - 11;
                }

            }

            if (mud->selected_wiki && no_menus) {
                mudclient_menu_add_wiki(mud, tab_name, page);
            }
        }

        offset_start_x -= UI_BUTTON_SIZE - 2;
        offset_end_x -= UI_BUTTON_SIZE - 2;
    }

    if (mud->show_ui_tab != 0 &&
        mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 &&
        mud->mouse_y < UI_BUTTON_SIZE - 9) {
        mud->show_ui_tab = INVENTORY_TAB;
    }

    if (mud->show_ui_tab != 0 && mud->show_ui_tab != MAP_TAB &&
        mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 33 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 33 &&
        mud->mouse_y < UI_BUTTON_SIZE - 9) {
        mud->show_ui_tab = MAP_TAB;

        if (mud->options->anti_macro) {
            mud->minimap_random_rotation =
                (int)((((float)rand() / (float)RAND_MAX)) * 13) - 6;

            mud->minimap_random_scale =
                (int)((((float)rand() / (float)RAND_MAX)) * 23) - 11;
        }
    }

    if (mud->show_ui_tab != 0 &&
        mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 66 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 66 &&
        mud->mouse_y < UI_BUTTON_SIZE - 9) {
        mud->show_ui_tab = STATS_TAB;
    }

    if (mud->show_ui_tab != 0 &&
        mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 99 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 99 &&
        mud->mouse_y < UI_BUTTON_SIZE - 9) {
        mud->show_ui_tab = MAGIC_TAB;
    }

    if (mud->show_ui_tab != 0 &&
        mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 132 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 132 &&
        mud->mouse_y < UI_BUTTON_SIZE - 9) {
        mud->show_ui_tab = SOCIAL_TAB;
    }

    if (mud->show_ui_tab != 0 &&
        mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 165 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 165 &&
        mud->mouse_y < UI_BUTTON_SIZE - 9) {
        mud->show_ui_tab = OPTIONS_TAB;
    }

    if (mud->show_ui_tab == INVENTORY_TAB &&
        (mud->mouse_x < mud->surface->width - (INVENTORY_WIDTH + 3) ||
         mud->mouse_y > 36 + INVENTORY_HEIGHT)) {
        mud->show_ui_tab = 0;
    }

    int is_dragging_scrollbar = 0;

    if (mud->options->off_handle_scroll_drag) {
        is_dragging_scrollbar =
            mud->panel_quests->control_list_scrollbar_handle_dragged
                [mud->control_list_quest];
    }

    int stats_height = 36 + STATS_HEIGHT + 5;

    if (mud->options->total_experience || mud->options->remaining_experience) {
        stats_height += STATS_LINE_BREAK;
    }

    if (mud->show_ui_tab == STATS_TAB &&
        (mud->mouse_x < mud->surface->width - 199 ||
         mud->mouse_y > stats_height) &&
        !is_dragging_scrollbar) {
        mud->show_ui_tab = 0;
    }

    if (mud->options->off_handle_scroll_drag) {
        is_dragging_scrollbar =
            mud->panel_magic->control_list_scrollbar_handle_dragged
                [mud->control_list_magic] ||
            mud->panel_social_list->control_list_scrollbar_handle_dragged
                [mud->control_list_social];
    }

    if ((mud->show_ui_tab == MAP_TAB || mud->show_ui_tab == MAGIC_TAB ||
         mud->show_ui_tab == SOCIAL_TAB) &&
        (mud->mouse_x < mud->surface->width - 199 || mud->mouse_y > 240) &&
        !is_dragging_scrollbar) {
        mud->show_ui_tab = 0;
    }

    if (mud->show_ui_tab == OPTIONS_TAB &&
        (mud->mouse_x < mud->surface->width - 199 ||
         mud->mouse_y > 311 + (mud->options->show_additional_options
                                   ? OPTIONS_LINE_BREAK
                                   : 0))) {
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
