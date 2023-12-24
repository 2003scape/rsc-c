#include "ui-tabs.h"

char *mudclient_ui_tab_names[] = {"Inventory", "Map",     "Stats",
                                  "Spellbook", "Friends", "Options"};

void mudclient_draw_ui_tabs(mudclient *mud) {
    int button_y = 3;

#if (VERSION_MEDIA >= 59)
    int button_x = mud->surface->width - UI_BUTTON_SIZE;

    if (mudclient_is_touch(mud)) {
        button_x = UI_TABS_TOUCH_X;
        button_y = UI_TABS_TOUCH_Y;
    }

    for (int i = 0; i < 6; i++) {
        surface_draw_sprite_alpha(mud->surface, button_x, button_y,
                                  mud->sprite_media + 33 + i, 128);

        if (mudclient_is_touch(mud)) {
            button_y += UI_BUTTON_SIZE - 2;
        } else {
            button_x -= UI_BUTTON_SIZE - 2;
        }
    }
#else
    int button_x =
        mud->surface->width - mud->surface->sprite_width[mud->sprite_media] - 3;

    surface_draw_sprite_alpha(mud->surface, button_x, 3, mud->sprite_media,
                              128);
#endif
}

void mudclient_toggle_ui_tab(mudclient *mud, int tab) {
    if (mud->mouse_button_click != 0) {
        mud->show_ui_tab = mud->show_ui_tab == tab ? 0 : tab;
        mud->mouse_button_click = 0;
    }
}

void mudclient_set_active_ui_tab(mudclient *mud, int no_menus) {
    int is_toggle = mudclient_is_touch(mud);

#if defined(_3DS) || defined(__SWITCH__)
    is_toggle = 1;
#endif

    int offset_min_x = mud->surface->width - UI_BUTTON_SIZE;
    int offset_max_x = mud->surface->width - 3;

    int offset_min_y = 3;
    int offset_max_y = UI_BUTTON_SIZE;

    if (mudclient_is_touch(mud)) {
        offset_min_x = UI_TABS_TOUCH_X;
        offset_max_x = mud->surface->width;

        offset_min_y = UI_TABS_TOUCH_Y;
        offset_max_y = offset_min_y + (UI_BUTTON_SIZE - 2);
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

        if (is_dragging_scrollbar) {
            return;
        }
    }

    int has_changed_tab = 0;

    for (int i = 0; i < 6; i++) {
        int tab = i + 1;

        if (mud->mouse_x >= offset_min_x && mud->mouse_x < offset_max_x &&
            mud->mouse_y >= offset_min_y && mud->mouse_y < offset_max_y) {
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

        if (mudclient_is_touch(mud)) {
            offset_min_y += UI_BUTTON_SIZE - 2;
            offset_max_y += UI_BUTTON_SIZE - 2;
        } else {
            offset_min_x -= UI_BUTTON_SIZE - 2;
            offset_max_x -= UI_BUTTON_SIZE - 2;
        }
    }

    /* ui_tab_ boundary values will change next frame after the change */
    if (!has_changed_tab && !is_dragging_scrollbar && mud->show_ui_tab != 0 &&
        (mud->mouse_x < mud->ui_tab_min_x || mud->mouse_x > mud->ui_tab_max_x ||
         mud->mouse_y < mud->ui_tab_min_y ||
         mud->mouse_y > mud->ui_tab_max_y)) {
        mud->show_ui_tab = 0;
    }
}

void mudclient_draw_ui_tab_label(mudclient *mud, int selected_tab, int width,
                                 int x, int y) {
    surface_draw_box(mud->surface, x, y, width, 9, UI_LABEL_COLOUR);

    int sprite_id = mud->sprite_media + 26 + selected_tab;

    int label_offset = !mudclient_is_touch(mud) && mud->show_ui_tab > 4
                           ? width - mud->surface->sprite_width[sprite_id] - 3
                           : 1;

    surface_draw_sprite(mud->surface, x + label_offset, y + 1, sprite_id);
    surface_draw_line_horizontal(mud->surface, x, y + 9, width, BLACK);
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

    if (mud->show_ui_tab != 0) {
        int selected_y = 3;

#if (VERSION_MEDIA >= 59)
        int selected_x = mud->surface->width -
                         UI_BUTTON_SIZE * mud->show_ui_tab +
                         (mud->show_ui_tab - 1) * 2;

        if (mudclient_is_touch(mud)) {
            selected_x = UI_TABS_TOUCH_X;

            selected_y =
                UI_TABS_TOUCH_Y + (mud->show_ui_tab - 1) * (UI_BUTTON_SIZE - 2);
        }
#else
        int selected_x = mud->surface->width - UI_TABS_WIDTH - 3;
#endif

        surface_draw_sprite(mud->surface, selected_x, selected_y,
                            mud->sprite_media + mud->show_ui_tab);
    }
}
