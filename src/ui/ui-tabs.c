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

    int has_changed_tab = 0;

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

            if (mud->show_ui_tab == 0 || mud->mouse_y < UI_BUTTON_SIZE - 9) {
                mud->show_ui_tab = tab;

                if (tab == MAP_TAB && mud->options->anti_macro) {
                    mud->minimap_random_rotation =
                        (int)((((float)rand() / (float)RAND_MAX)) * 13) - 6;

                    mud->minimap_random_scale =
                        (int)((((float)rand() / (float)RAND_MAX)) * 23) - 11;
                }

                has_changed_tab = 1;
            }

            if (mud->selected_wiki && no_menus) {
                mudclient_menu_add_wiki(mud, tab_name, page);
            }
        }

        offset_start_x -= UI_BUTTON_SIZE - 2;
        offset_end_x -= UI_BUTTON_SIZE - 2;
    }

    int is_dragging_scrollbar = 0;

    if (mud->options->off_handle_scroll_drag) {
        is_dragging_scrollbar =
            mud->panel_quests->control_list_scrollbar_handle_dragged
                [mud->control_list_quest] ||
            mud->panel_magic->control_list_scrollbar_handle_dragged
                [mud->control_list_magic] ||
            mud->panel_social_list->control_list_scrollbar_handle_dragged
                [mud->control_list_social];
    }

    /* ui_tab_ boundary values will change next frame after the change */
    if (!has_changed_tab && !is_dragging_scrollbar && mud->show_ui_tab != 0 &&
        (mud->mouse_x < mud->ui_tab_min_x || mud->mouse_x > mud->ui_tab_max_x ||
         mud->mouse_y < mud->ui_tab_min_y ||
         mud->mouse_y > mud->ui_tab_max_y)) {
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
