#include "magic-tab.h"

char *magic_tabs[] = {"Magic", "Prayers"};

void mudclient_draw_ui_tab_magic(mudclient *mud, int no_menus) {
    int ui_x = mud->surface->width - MAGIC_WIDTH - 3;
    int ui_y = UI_BUTTON_SIZE + 1;

    int height = MAGIC_HEIGHT;

    int is_touch = mudclient_is_touch(mud);

    if (is_touch) {
        height = 198;
        ui_x = UI_TABS_TOUCH_X - MAGIC_WIDTH - 1;
        ui_y = (UI_TABS_TOUCH_Y + UI_TABS_TOUCH_HEIGHT) - height - 2;
    }

#if (VERSION_MEDIA >= 59)
    mudclient_draw_ui_tab_label(mud, MAGIC_TAB, MAGIC_WIDTH + !is_touch,
                                ui_x - !is_touch, ui_y - UI_TABS_LABEL_HEIGHT);
#endif

    mud->ui_tab_min_x = ui_x;
    mud->ui_tab_max_x = mud->surface->width;
    mud->ui_tab_min_y = 0;
    mud->ui_tab_max_y = 240;

    if (is_touch) {
        mud->ui_tab_max_x = ui_x + MAGIC_WIDTH;
        mud->ui_tab_min_y = ui_y - UI_TABS_LABEL_HEIGHT;
        mud->ui_tab_max_y = ui_y + height;
    }

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + MAGIC_TAB_HEIGHT,
                           MAGIC_WIDTH, height - MAGIC_TAB_HEIGHT, GREY_DC,
                           128);

    int spell_list_height =
        mud->panel_magic->control_height[mud->control_list_magic];

    int description_y = ui_y + spell_list_height + MAGIC_TAB_HEIGHT - 1;

    surface_draw_line_horizontal(mud->surface, ui_x, description_y, MAGIC_WIDTH,
                                 BLACK);

    description_y += 11;

    surface_draw_tabs(mud->surface, ui_x, ui_y, MAGIC_WIDTH, MAGIC_TAB_HEIGHT,
                      magic_tabs, 2, mud->ui_tab_magic_sub_tab);

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    int handle_panel_input_early = is_touch;

#ifdef _3DS
    handle_panel_input_early = 1;

    panel_handle_mouse(mud->panel_magic, mud->mouse_x, mud->mouse_y,
                       mud->last_mouse_button_down, mud->mouse_button_down,
                       mud->mouse_scroll_delta);
#else
    if (is_touch) {
        handle_panel_input_early = 1;

        panel_handle_mouse(mud->panel_magic, mudclient_finger_1_x,
                           mudclient_finger_1_y, mud->last_mouse_button_down,
                           mudclient_finger_1_down, mud->mouse_scroll_delta);
    }
#endif

    if (mud->ui_tab_magic_sub_tab == 0) {
        panel_clear_list(mud->panel_magic, mud->control_list_magic);

        int magic_level = mud->player_skill_current[SKILL_MAGIC];

        for (int i = 0; i < game_data.spell_count; i++) {
            char colour_prefix[6] = "@yel@";

            for (int j = 0; j < game_data.spells[i].runes_required; j++) {
                int rune_id = game_data.spells[i].runes[j].id;
                int rune_amount = game_data.spells[i].runes[j].count;

                if (mudclient_has_inventory_item(mud, rune_id, rune_amount)) {
                    continue;
                }

                strcpy(colour_prefix, "@whi@");
                break;
            }

            if (game_data.spells[i].level > magic_level) {
                strcpy(colour_prefix, "@bla@");
            }

            char formatted_spell[64] = {0};

            sprintf(formatted_spell, "%sLevel %d: %s", colour_prefix,
                    game_data.spells[i].level, game_data.spells[i].name);

            panel_add_list_entry(mud->panel_magic, mud->control_list_magic, i,
                                 formatted_spell);
        }

        panel_draw_panel(mud->panel_magic);

        int spell_index = panel_get_list_entry_index(mud->panel_magic,
                                                     mud->control_list_magic);

        if (spell_index != -1) {
            char *spell_name = game_data.spells[spell_index].name;
            char formatted_spell[64] = {0};

            sprintf(formatted_spell, "Level %d: %s",
                    game_data.spells[spell_index].level, spell_name);

            surface_draw_string(mud->surface, formatted_spell, ui_x + 2,
                                description_y, FONT_BOLD_12, YELLOW);

            surface_draw_string(
                mud->surface, game_data.spells[spell_index].description,
                ui_x + 2, description_y + 12, FONT_REGULAR_11, WHITE);

            for (int i = 0; i < game_data.spells[spell_index].runes_required;
                 i++) {
                int rune_id = game_data.spells[spell_index].runes[i].id;

                int inventory_rune_count =
                    mudclient_get_inventory_count(mud, rune_id);

                int rune_count = game_data.spells[spell_index].runes[i].count;

                char colour_prefix[6] = "@red@";

                if (mudclient_has_inventory_item(mud, rune_id, rune_count)) {
                    strcpy(colour_prefix, "@gre@");
                }

                surface_draw_sprite(
                    mud->surface, ui_x + 2 + i * 44, description_y + 26,
                    mud->sprite_item + game_data.items[rune_id].sprite);

                char formatted_count[29] = {0};

                sprintf(formatted_count, "%s%d/%d", colour_prefix,
                        inventory_rune_count, rune_count);

                surface_draw_string(mud->surface, formatted_count,
                                    ui_x + 2 + i * 44, description_y + 26,
                                    FONT_BOLD_12, WHITE);
            }

            if (mud->selected_wiki && no_menus) {
                mudclient_menu_add_wiki(mud, spell_name, spell_name);
            }
        } else {
            surface_draw_string(mud->surface,
                                "Point at a spell for a description", ui_x + 2,
                                description_y, FONT_BOLD_12, BLACK);
        }
    } else if (mud->ui_tab_magic_sub_tab == 1) {
        panel_clear_list(mud->panel_magic, mud->control_list_magic);

        for (int i = 0; i < game_data.prayer_count; i++) {
            char colour_prefix[6] = "@whi@";

            if (game_data.prayers[i].level >
                mud->player_skill_base[SKILL_PRAYER]) {
                strcpy(colour_prefix, "@bla@");
            }

            if (mud->prayer_on[i]) {
                strcpy(colour_prefix, "@gre@");
            }

            char formatted_prayer[64] = {0};

            sprintf(formatted_prayer, "%sLevel %d: %s", colour_prefix,
                    game_data.prayers[i].level, game_data.prayers[i].name);

            panel_add_list_entry(mud->panel_magic, mud->control_list_magic, i,
                                 formatted_prayer);
        }

        panel_draw_panel(mud->panel_magic);

        int prayer_index = panel_get_list_entry_index(mud->panel_magic,
                                                      mud->control_list_magic);

        if (prayer_index != -1) {
            char *prayer_name = game_data.prayers[prayer_index].name;
            char formatted_prayer[64] = {0};

            sprintf(formatted_prayer, "Level %d: %s",
                    game_data.prayers[prayer_index].level, prayer_name);

            surface_draw_string_centre(mud->surface, formatted_prayer,
                                       ui_x + (MAGIC_WIDTH / 2),
                                       description_y + 6, FONT_BOLD_12, YELLOW);

            surface_draw_string_centre(
                mud->surface, game_data.prayers[prayer_index].description,
                ui_x + (MAGIC_WIDTH / 2), description_y + 21, FONT_REGULAR_11,
                WHITE);

            sprintf(formatted_prayer, "Drain rate: %d",
                    game_data.prayers[prayer_index].drain);

            surface_draw_string_centre(mud->surface, formatted_prayer,
                                       ui_x + (MAGIC_WIDTH / 2),
                                       description_y + 36, FONT_BOLD_12, BLACK);

            if (mud->selected_wiki && no_menus) {
                mudclient_menu_add_wiki(mud, prayer_name, prayer_name);
            }
        } else {
            surface_draw_string(mud->surface,
                                "Point at a prayer for a description", ui_x + 2,
                                description_y, FONT_BOLD_12, BLACK);
        }
    }

    if (!no_menus) {
        return;
    }

    if (!handle_panel_input_early) {
        int is_within_x = mud->options->off_handle_scroll_drag
                              ? 1
                              : mouse_x >= 0 && mouse_x < MAGIC_WIDTH;

        if (!is_within_x || !(mouse_y >= 0 && mouse_y < height)) {
            return;
        }

        panel_handle_mouse(mud->panel_magic, mouse_x + ui_x, mouse_y + ui_y,
                           mud->last_mouse_button_down, mud->mouse_button_down,
                           mud->mouse_scroll_delta);
    }

    if (mouse_y <= MAGIC_TAB_HEIGHT && mud->mouse_button_click == 1) {
        if (mouse_x < (MAGIC_WIDTH / 2) && mud->ui_tab_magic_sub_tab == 1) {
            mud->ui_tab_magic_sub_tab = 0;
            panel_reset_list(mud->panel_magic, mud->control_list_magic);
        } else if (mouse_x > (MAGIC_WIDTH / 2) &&
                   mud->ui_tab_magic_sub_tab == 0) {
            mud->ui_tab_magic_sub_tab = 1;
            panel_reset_list(mud->panel_magic, mud->control_list_magic);
        }
    }

    if (mud->selected_wiki || mud->mouse_button_click != 1) {
        return;
    }

    if (mud->ui_tab_magic_sub_tab == 0) {
        int spell_index = panel_get_list_entry_index(mud->panel_magic,
                                                     mud->control_list_magic);

        if (spell_index != -1) {
            int magic_level = mud->player_skill_current[SKILL_MAGIC];

            if (game_data.spells[spell_index].level > magic_level) {
                mudclient_show_message(
                    mud, "Your magic ability is not high enough for this spell",
                    MESSAGE_TYPE_GAME);
            } else {
                int i = 0;

                for (i = 0; i < game_data.spells[spell_index].runes_required;
                     i++) {
                    int reagant_id = game_data.spells[spell_index].runes[i].id;

                    if (mudclient_has_inventory_item(
                            mud, reagant_id,
                            game_data.spells[spell_index].runes[i].count)) {
                        continue;
                    }

                    mudclient_show_message(mud,
                                           "You don't have all the reagents "
                                           "you need for this spell",
                                           MESSAGE_TYPE_GAME);

                    i = -1;
                    break;
                }

                if (i == game_data.spells[spell_index].runes_required) {
                    mud->selected_spell = spell_index;
                    mud->selected_item_inventory_index = -1;

                    if (MUD_IS_COMPACT) {
                        mud->show_ui_tab = 0;
                    }
                }
            }
        }
    } else if (mud->ui_tab_magic_sub_tab == 1) {
        int prayer_index = panel_get_list_entry_index(mud->panel_magic,
                                                      mud->control_list_magic);

        if (prayer_index != -1) {
            int prayer_level = mud->player_skill_base[SKILL_PRAYER];

            if (game_data.prayers[prayer_index].level > prayer_level) {
                mudclient_show_message(mud,
                                       "Your prayer ability is not high enough "
                                       "for this prayer",
                                       MESSAGE_TYPE_GAME);
            } else if (mud->player_skill_current[SKILL_PRAYER] == 0) {
                mudclient_show_message(mud,
                                       "You have run out of prayer points. "
                                       "return to a church to recharge",
                                       MESSAGE_TYPE_GAME);
            } else if (mud->prayer_on[prayer_index]) {
                packet_stream_new_packet(mud->packet_stream, CLIENT_PRAYER_OFF);
                packet_stream_put_byte(mud->packet_stream, prayer_index);
                packet_stream_send_packet(mud->packet_stream);

                mud->prayer_on[prayer_index] = 0;

                mudclient_play_sound(mud, "prayeroff");
            } else {
                packet_stream_new_packet(mud->packet_stream, CLIENT_PRAYER_ON);
                packet_stream_put_byte(mud->packet_stream, prayer_index);
                packet_stream_send_packet(mud->packet_stream);

                mud->prayer_on[prayer_index] = 1;

                mudclient_play_sound(mud, "prayeron");
            }
        }
    }

    mud->mouse_button_click = 0;
}
