#include "ui-tabs.h"

#if defined (_3DS) || defined (__SWITCH__)
void mudclient_3ds_toggle_ui_tab(mudclient *mud, int tab) {
    if (mud->mouse_button_click != 0) {
        if (mud->show_ui_tab == tab) {
            mud->show_ui_tab = 0;
        } else {
            mud->show_ui_tab = tab;
        }

        mud->mouse_button_click = 0;
    }
}
#endif

void mudclient_set_active_ui_tab(mudclient *mud, int no_menus) {
    if (mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 &&
        mud->mouse_y < UI_BUTTON_SIZE) {
#if defined (_3DS) || defined (__SWITCH__)
        mudclient_3ds_toggle_ui_tab(mud, INVENTORY_TAB);
#else
        if (mud->show_ui_tab == 0) {
            mud->show_ui_tab = INVENTORY_TAB;
        }
#endif

        if (mud->selected_wiki && no_menus) {
            mudclient_menu_add_wiki(mud, "Inventory", "Inventory");
        }
    }

    if (mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 33 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 36 &&
        mud->mouse_y < UI_BUTTON_SIZE) {
#if defined (_3DS) || defined (__SWITCH__)
        mudclient_3ds_toggle_ui_tab(mud, MAP_TAB);
#else
        if (mud->show_ui_tab == 0) {
            mud->show_ui_tab = MAP_TAB;

            if (mud->options->anti_macro) {
                mud->minimap_random_rotation =
                    (int)((((float)rand() / (float)RAND_MAX)) * 13) - 6;

                mud->minimap_random_scale =
                    (int)((((float)rand() / (float)RAND_MAX)) * 23) - 11;
            }
        }
#endif

        if (mud->selected_wiki && no_menus) {
            mudclient_menu_add_wiki(mud, "Map", "RuneScape_Classic_Map");
        }
    }

    if (mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 66 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 66 &&
        mud->mouse_y < UI_BUTTON_SIZE) {
#if defined (_3DS) || defined (__SWITCH__)
        mudclient_3ds_toggle_ui_tab(mud, STATS_TAB);
#else
        if (mud->show_ui_tab == 0) {
            mud->show_ui_tab = STATS_TAB;
        }
#endif

        if (mud->selected_wiki && no_menus) {
            mudclient_menu_add_wiki(mud, "Stats", "Stats");
        }
    }

    if (mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 99 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 99 &&
        mud->mouse_y < UI_BUTTON_SIZE) {
#if defined (_3DS) || defined (__SWITCH__)
        mudclient_3ds_toggle_ui_tab(mud, MAGIC_TAB);
#else
        if (mud->show_ui_tab == 0) {
            mud->show_ui_tab = MAGIC_TAB;
        }
#endif

        if (mud->selected_wiki && no_menus) {
            mudclient_menu_add_wiki(mud, "Spellbook", "Spellbook");
        }
    }

    if (mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 132 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 132 &&
        mud->mouse_y < UI_BUTTON_SIZE) {
#if defined (_3DS) || defined (__SWITCH__)
        mudclient_3ds_toggle_ui_tab(mud, SOCIAL_TAB);
#else
        if (mud->show_ui_tab == 0) {
            mud->show_ui_tab = SOCIAL_TAB;
        }
#endif

        if (mud->selected_wiki && no_menus) {
            mudclient_menu_add_wiki(mud, "Friends", "Friends");
        }
    }

    if (mud->mouse_x >= mud->surface->width - UI_BUTTON_SIZE - 165 &&
        mud->mouse_y >= 3 && mud->mouse_x < mud->surface->width - 3 - 165 &&
        mud->mouse_y < UI_BUTTON_SIZE) {
#if defined (_3DS) || defined (__SWITCH__)
        mudclient_3ds_toggle_ui_tab(mud, OPTIONS_TAB);
#else
        if (mud->show_ui_tab == 0) {
            mud->show_ui_tab = OPTIONS_TAB;
        }
#endif

        if (mud->selected_wiki && no_menus) {
            mudclient_menu_add_wiki(mud, "Options", "Options");
        }
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
