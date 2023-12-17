#include "stats-tab.h"

char *short_skill_names[] = {
    "Attack",   "Defense",  "Strength", "Hits",      "Ranged",  "Prayer",
    "Magic",    "Cooking",  "Woodcut",  "Fletching", "Fishing", "Firemaking",
    "Crafting", "Smithing", "Mining",   "Herblaw",   "Agility", "Thieving"};

char *skill_names[] = {
    "Attack",   "Defense",  "Strength",    "Hits",      "Ranged",  "Prayer",
    "Magic",    "Cooking",  "Woodcutting", "Fletching", "Fishing", "Firemaking",
    "Crafting", "Smithing", "Mining",      "Herblaw",   "Agility", "Thieving"};

int skills_length;

char *equipment_stat_names[] = {"Armour", "WeaponAim", "WeaponPower", "Magic",
                                "Prayer"};

int experience_array[100];

char *free_quests[] = {
    "Black knight's fortress", "Cook's assistant",   "Demon slayer",
    "Doric's quest",           "The restless ghost", "Goblin diplomacy",
    "Ernest the chicken",      "Imp catcher",        "Pirate's treasure",
    "Prince Ali rescue",       "Romeo & Juliet",     "Sheep shearer",
    "Shield of Arrav",         "The knight's sword", "Vampire slayer",
    "Witch's potion",          "Dragon slayer"};

char *members_quests[] = {
    "Witch's house",    "Lost city",         "Hero's quest",
    "Druidic ritual",   "Merlin's crystal",  "Scorpion catcher",
    "Family crest",     "Tribal totem",      "Fishing contest",
    "Monk's friend",    "Temple of Ikov",    "Clock tower",
    "The Holy Grail",   "Fight Arena",       "Tree Gnome Village",
    "The Hazeel Cult",  "Sheep Herder",      "Plague City",
    "Sea Slug",         "Waterfall quest",   "Biohazard",
    "Jungle potion",    "Grand tree",        "Shilo village",
    "Underground pass", "Observatory quest", "Tourist trap",
    "Watchtower",       "Dwarf Cannon",      "Murder Mystery",
    "Digsite",          "Gertrude's Cat",    "Legend's Quest"};

char **quest_names;
int quests_length = 0;

char *stats_tabs[3] = {0};

void init_stats_tab_global() {
    skills_length = sizeof(skill_names) / sizeof(skill_names[0]);

    int total_exp = 0;

    for (int i = 0; i < 99; i++) {
        int level = i + 1;
        int exp = level + 300 * pow(2, (float)level / 7);
        total_exp += exp;
        experience_array[i] = total_exp & 0xffffffc;
    }

    int free_quests_length = sizeof(free_quests) / sizeof(free_quests[0]);

    int members_quests_length =
        sizeof(members_quests) / sizeof(members_quests[0]);

    quests_length = free_quests_length + members_quests_length;
    quest_names = malloc(sizeof(char *) * quests_length);

    for (int i = 0; i < free_quests_length; i++) {
        quest_names[i] = malloc(strlen(free_quests[i]) + 1);
        strcpy(quest_names[i], free_quests[i]);
    }

    for (int i = 0; i < members_quests_length; i++) {
        quest_names[free_quests_length + i] =
            malloc(strlen(members_quests[i]) + 11);

        sprintf(quest_names[free_quests_length + i], "%s (members)",
                members_quests[i]);
    }
}

void mudclient_draw_equipment_status(mudclient *mud, int x, int y,
                                     int line_break, int no_menus) {
    if (no_menus && mud->selected_wiki && mud->mouse_x > x + 5 &&
        mud->mouse_y > y - line_break &&
        mud->mouse_y < y + (line_break / 2) - 4) {
        mudclient_menu_add_wiki(mud, "Equipment Status", "Equipment");
    }

    surface_draw_string(mud->surface, "Equipment Status", x + 5, y, 3, YELLOW);

    y += line_break;

    for (int i = 0; i < 3; i++) {
        if (no_menus && mud->selected_wiki && mud->mouse_x > x + 5 &&
            mud->mouse_x < x + (STATS_WIDTH / 2) + 25 &&
            mud->mouse_y > y - line_break && mud->mouse_y < y + 3) {
            mudclient_menu_add_wiki(mud, equipment_stat_names[i],
                                    equipment_stat_names[i]);
        }

        /* longest stat name (11) + 5 + 11 */
        char formatted_stat[28] = {0};

        sprintf(formatted_stat, "%s:@yel@%d", equipment_stat_names[i],
                mud->player_stat_equipment[i]);

        surface_draw_string(mud->surface, formatted_stat, x + 5, y, 1, WHITE);

        if (i < 2) {
            if (no_menus && mud->selected_wiki &&
                mud->mouse_x > x + (STATS_WIDTH / 2) + 25 &&
                mud->mouse_y > y - line_break && mud->mouse_y < y + 3) {
                mudclient_menu_add_wiki(mud, equipment_stat_names[i + 3],
                                        equipment_stat_names[i + 3]);
            }

            sprintf(formatted_stat, "%s:@yel@%d", equipment_stat_names[i + 3],
                    mud->player_stat_equipment[i + 3]);

            surface_draw_string(mud->surface, formatted_stat,
                                x + (STATS_WIDTH / 2) + 25, y, 1, WHITE);
        }

        y += line_break + 1;
    }
}

void mudclient_draw_ui_tab_stats(mudclient *mud, int no_menus) {
    int ui_x = mud->surface->width - STATS_WIDTH - 3;
    int ui_y = UI_BUTTON_SIZE + 1;

    int height = STATS_HEIGHT;

    int is_touch = mudclient_is_touch(mud);

    int is_compact =
        is_touch || mud->surface->height < (height + STATS_LINE_BREAK) + ui_y;

    if (is_compact) {
        height = STATS_COMPACT_HEIGHT;

        stats_tabs[0] = "Skills";
        stats_tabs[1] = "Gear";
        stats_tabs[2] = "Quests";
    } else {
        stats_tabs[0] = "Skills";
        stats_tabs[1] = "Quests";
    }

    int line_break = (is_compact ? 11 : 12);

    if (mud->options->total_experience || mud->options->remaining_experience) {
        height += line_break;
    }

    mud->panel_quests->control_height[mud->control_list_quest] =
        height - STATS_TAB_HEIGHT;

    /* equipment screen */
    if (is_compact && mud->ui_tab_stats_sub_tab == 1) {
        height = 78;
    }

    if (is_touch) {
        ui_x = UI_TABS_TOUCH_X - STATS_WIDTH - 1;
        ui_y = (UI_TABS_TOUCH_Y + UI_TABS_TOUCH_HEIGHT) - height - 2;
    }

#if (VERSION_MEDIA >= 59)
    mudclient_draw_ui_tab_label(mud, STATS_TAB, STATS_WIDTH + !is_touch,
                                ui_x - !is_touch, ui_y - 10);
#endif

    mud->ui_tab_min_x = ui_x;
    mud->ui_tab_max_x = mud->surface->width;
    mud->ui_tab_min_y = 0;
    mud->ui_tab_max_y = ui_y + height + 5;

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + STATS_TAB_HEIGHT,
                           STATS_WIDTH, height - STATS_TAB_HEIGHT, GREY_DC,
                           128);

    surface_draw_line_horizontal(mud->surface, ui_x, ui_y + STATS_TAB_HEIGHT,
                                 STATS_WIDTH, BLACK);

    surface_draw_tabs(mud->surface, ui_x, ui_y, STATS_WIDTH, STATS_TAB_HEIGHT,
                      stats_tabs, is_compact ? 3 : 2,
                      mud->ui_tab_stats_sub_tab);

    /* the handler for the skills/equipment tab */
    if (mud->ui_tab_stats_sub_tab == 0) {
        int y = ui_y + 36;
        int selected_skill = -1;
        int total_experience = 0;

        if (!is_compact) {
            surface_draw_string(mud->surface, "Skills", ui_x + 5, y, 3, YELLOW);

            if (no_menus && mud->selected_wiki && mud->mouse_x > ui_x + 5 &&
                mud->mouse_x < ui_x + (STATS_WIDTH / 2) - 8 &&
                mud->mouse_y > y - (line_break - 1) && mud->mouse_y < y + 2) {
                mudclient_menu_add_wiki(mud, "Skills", "Skills");
            }

            y += line_break + 1;
        }

        /* draw two columns with each skill name and current/base levels */
        for (int i = 0; i < 9; i++) {
            total_experience += mud->player_experience[i];
            total_experience += mud->player_experience[i + 9];

            /* left column */
            int text_colour = WHITE;

            if (no_menus && mud->mouse_x > ui_x + 3 &&
                mud->mouse_y >= y - (line_break - 1) && mud->mouse_y < y + 2 &&
                mud->mouse_x < ui_x + (STATS_WIDTH / 2) - 8) {
                text_colour = RED;
                selected_skill = i;
            }

            /* longest skill name (10), @yel@ (4), two skills 3 each (6), colon
             * (1), slash (1) */
            char formatted_skill[23] = {0};

            sprintf(formatted_skill, "%s:@yel@%d/%d", short_skill_names[i],
                    mud->player_skill_current[i], mud->player_skill_base[i]);

            surface_draw_string(mud->surface, formatted_skill, ui_x + 5, y, 1,
                                text_colour);

            /* right column */
            text_colour = WHITE;

            if (no_menus && mud->mouse_x >= ui_x + 90 &&
                mud->mouse_y >= y - (line_break * (is_compact ? 1 : 2)) &&
                mud->mouse_y < y - (is_compact ? 0 : (line_break - 1)) &&
                mud->mouse_x < ui_x + STATS_WIDTH) {
                text_colour = RED;
                selected_skill = i + 9;
            }

            sprintf(formatted_skill, "%s:@yel@%d/%d", short_skill_names[i + 9],
                    mud->player_skill_current[i + 9],
                    mud->player_skill_base[i + 9]);

            surface_draw_string(
                mud->surface, formatted_skill, ui_x + (STATS_WIDTH / 2) - 5,
                y - (is_compact ? 0 : line_break + 1), 1, text_colour);

            y += line_break + 1;
        }

        if (is_compact) {
            y += line_break + 1;
        }

        if (no_menus && mud->selected_wiki &&
            mud->mouse_x > ui_x + (STATS_WIDTH / 2) - 5 &&
            mud->mouse_y > y - (line_break * 2) &&
            mud->mouse_y < y - (line_break - 1)) {
            mudclient_menu_add_wiki(mud, "Quest Points", "Quest Points");
        }

        char formatted_quest_points[30] = {0};

        sprintf(formatted_quest_points, "Quest Points:@yel@%d",
                mud->player_quest_points);

        surface_draw_string(mud->surface, formatted_quest_points,
                            ui_x + (STATS_WIDTH / 2) - 5, y - 13, 1, WHITE);

        if (!is_compact) {
            y += line_break;
        }

        if (no_menus && mud->selected_wiki && mud->mouse_x > ui_x + 5 &&
            mud->mouse_x < ui_x + (STATS_WIDTH / 2) - 5 &&
            mud->mouse_y > y - (line_break * 2) &&
            mud->mouse_y < y - (line_break - 1)) {
            mudclient_menu_add_wiki(mud, "Fatigue", "Fatigue");
        }

        char formatted_fatigue[27] = {0};

        sprintf(formatted_fatigue, "Fatigue: @yel@%d%%",
                (mud->stat_fatigue * 100) / 750);

        surface_draw_string(mud->surface, formatted_fatigue, ui_x + 5, y - 13,
                            1, WHITE);

        if (is_compact) {
            y += 2;
        } else {
            y += 8;

            mudclient_draw_equipment_status(mud, ui_x, y, line_break, no_menus);

            y += 51;
            y += line_break / 2;
        }

        surface_draw_line_horizontal(
            mud->surface, ui_x, y - (is_compact ? 12 : 15), STATS_WIDTH, BLACK);

        char formatted_number[15] = {0};

        int wiki_height = line_break * 2;

        if (mud->options->total_experience) {
            wiki_height += line_break;
        }

        if (selected_skill != -1) {
            if (!is_compact) {
                /* longest skill (11) + 6 + 1 */
                char formatted_skill[18] = {0};
                sprintf(formatted_skill, "%s skill",
                        skill_names[selected_skill]);

                surface_draw_string(mud->surface, formatted_skill, ui_x + 5, y,
                                    1, YELLOW);

                y += line_break;
            }

            int next_level_at = experience_array[0];

            for (int j = 0; j < 98; j++) {
                if (mud->player_experience[selected_skill] >=
                    experience_array[j]) {
                    next_level_at = experience_array[j + 1];
                }
            }

            int total_xp = mud->player_experience[selected_skill];

            mudclient_format_number_commas(mud, total_xp / 4, formatted_number);

            char formatted_xp[25] = {0};
            sprintf(formatted_xp, "Total xp: %s", formatted_number);

            surface_draw_string(mud->surface, formatted_xp, ui_x + 5, y, 1,
                                WHITE);

            y += line_break;

            mudclient_format_number_commas(mud, next_level_at / 4,
                                           formatted_number);

            char formatted_next[30] = {0};
            sprintf(formatted_next, "Next level at: %s", formatted_number);

            surface_draw_string(mud->surface, formatted_next, ui_x + 5, y, 1,
                                WHITE);

            if (mud->options->remaining_experience) {
                y += line_break;

                int remaining_xp = (next_level_at - total_xp);

                mudclient_format_number_commas(mud, remaining_xp / 4,
                                               formatted_number);

                char formatted_remainder[5] = {0};
                sprintf(formatted_remainder, "%.2f", (remaining_xp % 4) / 4.0f);

                sprintf(formatted_next, "Remaining xp: %s%s", formatted_number,
                        formatted_remainder + 1);

                surface_draw_string(mud->surface, formatted_next, ui_x + 5, y,
                                    1, WHITE);
            }

            if (no_menus && mud->selected_wiki) {
                char *skill_name = skill_names[selected_skill];
                mudclient_menu_add_wiki(mud, skill_name, skill_name);
            }
        } else {
            if (no_menus && mud->selected_wiki && mud->mouse_x > ui_x + 5 &&
                mud->mouse_y > y - line_break &&
                mud->mouse_y < y - line_break + wiki_height + 3) {
                mudclient_menu_add_wiki(mud, "Experience", "Experience");
            }

            if (!is_compact) {
                surface_draw_string(mud->surface, "Overall levels", ui_x + 5, y,
                                    1, YELLOW);

                y += line_break;
            }

            int total_level = 0;

            for (int i = 0; i < skills_length; i++) {
                total_level += mud->player_skill_base[i];
            }

            mudclient_format_number_commas(mud, total_level, formatted_number);

            char formatted_total[28] = {0};
            sprintf(formatted_total, "Skill total: %s", formatted_number);

            surface_draw_string(mud->surface, formatted_total, ui_x + 5, y, 1,
                                WHITE);

            y += line_break;

            if (mud->options->total_experience) {
                mudclient_format_number_commas(mud, total_experience,
                                               formatted_number);

                sprintf(formatted_total, "Total xp: %s", formatted_number);

                surface_draw_string(mud->surface, formatted_total, ui_x + 5, y,
                                    1, WHITE);

                y += line_break;
            }

            if (no_menus && mud->selected_wiki && mud->mouse_x > ui_x &&
                mud->mouse_y > y - line_break && mud->mouse_y < y + 5) {
                mudclient_menu_add_wiki(mud, "Combat level", "Combat level");
            }

            char formatted_combat[25] = {0};

            sprintf(formatted_combat, "Combat level: %d",
                    mud->local_player->level);

            surface_draw_string(mud->surface, formatted_combat, ui_x + 5, y,
                                FONT_BOLD_12, WHITE);

            y += line_break;
        }
    } else if ((!is_compact && mud->ui_tab_stats_sub_tab == 1) ||
               (is_compact && mud->ui_tab_stats_sub_tab == 2)) {
        int stats_height = 36 + STATS_HEIGHT + 5;

        if (no_menus && mud->selected_wiki && mud->mouse_x > ui_x + 5 &&
            mud->mouse_y > ui_y + 24 + line_break &&
            mud->mouse_y < ui_y + stats_height) {
            int quest_index =
                mud->panel_quests
                    ->control_list_entry_mouse_over[mud->control_list_quest];

            if (quest_index > 0) {
                quest_index -= 1;

                char *quest_name = quest_names[quest_index];
                int quest_name_length = strlen(quest_name);

                for (int i = 0; i < quest_name_length; i++) {
                    if (quest_name[i] == '(') {
                        quest_name_length = i - 1;
                        break;
                    }
                }

                char page_name[quest_name_length + 1];
                memset(page_name, '\0', quest_name_length + 1);
                strncpy(page_name, quest_name, quest_name_length);

                mudclient_menu_add_wiki(mud, quest_name, page_name);
            }
        }

        /* the handler for the quests tab */
        panel_clear_list(mud->panel_quests, mud->control_list_quest);

        panel_add_list_entry(mud->panel_quests, mud->control_list_quest, 0,
                             "@whi@Quest-list (green=completed)");

        for (int i = 0; i < quests_length; i++) {
            char *quest_name = quest_names[i];
            char coloured_quest[strlen(quest_name) + 6];

            sprintf(coloured_quest, "%s%s",
                    mud->quest_complete[i] ? "@gre@" : "@red@", quest_name);

            panel_add_list_entry(mud->panel_quests, mud->control_list_quest,
                                 i + 1, coloured_quest);
        }

        panel_draw_panel(mud->panel_quests);
    } else if (is_compact && mud->ui_tab_stats_sub_tab == 1) {
        mudclient_draw_equipment_status(mud, ui_x, ui_y + 38, line_break,
                                        no_menus);
    }

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    /* handle clicking of stats and quest tab, and the scroll wheel for the
     * quest list */
    int is_within_x = mud->options->off_handle_scroll_drag
                          ? 1
                          : mouse_x >= 0 && mouse_x < STATS_WIDTH;

    if (is_within_x && mouse_y >= 0 && mouse_y < height) {
        int quest_tab = is_compact ? 2 : 1;

        if (mud->ui_tab_stats_sub_tab == quest_tab) {
            panel_handle_mouse(mud->panel_quests, mouse_x + ui_x,
                               mouse_y + ui_y, mud->last_mouse_button_down,
                               mud->mouse_button_down, mud->mouse_scroll_delta);
        }

        if (mouse_y <= STATS_TAB_HEIGHT) {
            int total_tabs = is_compact ? 3 : 2;
            int selected_tab = mouse_x / (STATS_WIDTH / total_tabs);

            if (selected_tab < 0 || selected_tab >= total_tabs) {
                return;
            }

            if (mud->selected_wiki) {
                char *wiki_page = stats_tabs[selected_tab];

                if (is_compact && selected_tab == 1) {
                    wiki_page = "Equipment";
                }

                mudclient_menu_add_wiki(mud, stats_tabs[selected_tab],
                                        wiki_page);
            } else if (mud->mouse_button_click == 1) {
                mud->ui_tab_stats_sub_tab = selected_tab;
            }
        }
    }
}
