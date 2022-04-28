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

char *stats_tabs[] = {"Stats", "Quests"};

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

void mudclient_draw_ui_tab_stats(mudclient *mud, int no_menus) {
    int height = STATS_HEIGHT;

    if (mud->ui_tab_stats_sub_tab == 0 &&
        (mud->options->total_experience ||
         mud->options->remaining_experience)) {
        height += STATS_LINE_BREAK;
    }

    int ui_x = mud->game_width - STATS_WIDTH - 3;
    int ui_y = 36;

    surface_draw_sprite_from3(mud->surface, mud->game_width - UI_TABS_WIDTH - 3,
                              3, mud->sprite_media + STATS_TAB_SPRITE_OFFSET);

    surface_draw_box_alpha(mud->surface, ui_x, ui_y + STATS_TAB_HEIGHT,
                           STATS_WIDTH, height - STATS_TAB_HEIGHT, GREY_DC,
                           128);

    surface_draw_line_horizontal(mud->surface, ui_x, ui_y + STATS_TAB_HEIGHT,
                                 STATS_WIDTH, BLACK);

    surface_draw_tabs(mud->surface, ui_x, ui_y, STATS_WIDTH, STATS_TAB_HEIGHT,
                      stats_tabs, 2, mud->ui_tab_stats_sub_tab);

    // the handler for the stats tab
    if (mud->ui_tab_stats_sub_tab == 0) {
        int y = ui_y + 36;
        int selected_skill = -1;
        int total_experience = 0;

        surface_draw_string(mud->surface, "Skills", ui_x + 5, y, 3, YELLOW);

        y += STATS_LINE_BREAK + 1;

        // draw two columns with each skill name and current/base levels
        for (int i = 0; i < 9; i++) {
            total_experience += mud->player_experience[i];
            total_experience += mud->player_experience[i + 9];

            // left column
            int text_colour = WHITE;

            if (mud->mouse_x > ui_x + 3 && mud->mouse_y >= y - 11 &&
                mud->mouse_y < y + 2 && mud->mouse_x < ui_x + 90) {
                text_colour = RED;
                selected_skill = i;
            }

            // longest skill name (10), @yel@ (4), two skills 3 each (6),
            // colon (1), slash (1)
            char formatted_skill[23] = {0};

            sprintf(formatted_skill, "%s:@yel@%d/%d", short_skill_names[i],
                    mud->player_stat_current[i], mud->player_stat_base[i]);

            surface_draw_string(mud->surface, formatted_skill, ui_x + 5, y, 1,
                                text_colour);

            // right column
            text_colour = WHITE;

            if (mud->mouse_x >= ui_x + 90 && mud->mouse_y >= y - 13 - 11 &&
                mud->mouse_y < y - 13 + 2 && mud->mouse_x < ui_x + 196) {
                text_colour = RED;
                selected_skill = i + 9;
            }

            sprintf(formatted_skill, "%s:@yel@%d/%d", short_skill_names[i + 9],
                    mud->player_stat_current[i + 9],
                    mud->player_stat_base[i + 9]);

            surface_draw_string(mud->surface, formatted_skill,
                                ui_x + (STATS_WIDTH / 2) - 5,
                                y - STATS_LINE_BREAK + 1, 1, text_colour);

            y += STATS_LINE_BREAK + 1;
        }

        char formatted_quest_points[30] = {0};
        sprintf(formatted_quest_points, "Quest Points:@yel@%d",
                mud->player_quest_points);

        surface_draw_string(mud->surface, formatted_quest_points,
                            ui_x + (STATS_WIDTH / 2) - 5, y - 13, 1, WHITE);

        y += STATS_LINE_BREAK;

        char formatted_fatigue[27] = {0};

        sprintf(formatted_fatigue, "Fatigue: @yel@%d%%",
                (mud->stat_fatigue * 100) / 750);

        surface_draw_string(mud->surface, formatted_fatigue, ui_x + 5, y - 13,
                            1, WHITE);

        y += 8;

        surface_draw_string(mud->surface, "Equipment Status", ui_x + 5, y, 3,
                            YELLOW);

        y += STATS_LINE_BREAK;

        for (int i = 0; i < 3; i++) {
            // longest stat name (11) + 5 + 11
            char formatted_stat[28] = {0};

            sprintf(formatted_stat, "%s:@yel@%d", equipment_stat_names[i],
                    mud->player_stat_equipment[i]);

            surface_draw_string(mud->surface, formatted_stat, ui_x + 5, y, 1,
                                WHITE);

            if (i < 2) {
                sprintf(formatted_stat, "%s:@yel@%d",
                        equipment_stat_names[i + 3],
                        mud->player_stat_equipment[i + 3]);

                surface_draw_string(mud->surface, formatted_stat,
                                    ui_x + (STATS_WIDTH / 2) + 25, y, 1, WHITE);
            }

            y += STATS_LINE_BREAK + 1;
        }

        y += STATS_LINE_BREAK / 2;

        surface_draw_line_horizontal(mud->surface, ui_x, y - 15, STATS_WIDTH,
                                     BLACK);

        if (selected_skill != -1) {
            // longest skill (11) + 6 + 1
            char formatted_skill[18] = {0};
            sprintf(formatted_skill, "%s skill", skill_names[selected_skill]);

            surface_draw_string(mud->surface, formatted_skill, ui_x + 5, y, 1,
                                YELLOW);

            y += STATS_LINE_BREAK;

            int next_level_at = experience_array[0];

            for (int j = 0; j < 98; j++) {
                if (mud->player_experience[selected_skill] >=
                    experience_array[j]) {
                    next_level_at = experience_array[j + 1];
                }
            }

            int total_xp = mud->player_experience[selected_skill];

            char formatted_xp[22] = {0};
            sprintf(formatted_xp, "Total xp: %d", total_xp / 4);

            surface_draw_string(mud->surface, formatted_xp, ui_x + 5, y, 1,
                                WHITE);

            y += STATS_LINE_BREAK;

            char formatted_next[27] = {0};
            sprintf(formatted_next, "Next level at: %d", next_level_at / 4);

            surface_draw_string(mud->surface, formatted_next, ui_x + 5, y, 1,
                                WHITE);

            if (mud->options->remaining_experience) {
                y += STATS_LINE_BREAK;

                sprintf(formatted_next, "Remaining xp: %d",
                        (next_level_at - total_xp) / 4);

                surface_draw_string(mud->surface, formatted_next, ui_x + 5, y,
                                    1, WHITE);
            }
        } else {
            surface_draw_string(mud->surface, "Overall levels", ui_x + 5, y, 1,
                                YELLOW);

            y += STATS_LINE_BREAK;

            int total_level = 0;

            for (int i = 0; i < skills_length; i++) {
                total_level += mud->player_stat_base[i];
            }

            char formatted_total[24] = {0};
            sprintf(formatted_total, "Skill total: %d", total_level);

            surface_draw_string(mud->surface, formatted_total, ui_x + 5, y, 1,
                                WHITE);

            y += STATS_LINE_BREAK;

            if (mud->options->total_experience) {
                sprintf(formatted_total, "Total xp: %d", total_experience);

                surface_draw_string(mud->surface, formatted_total, ui_x + 5, y,
                                    1, WHITE);

                y += STATS_LINE_BREAK;
            }

            char formatted_combat[25] = {0};

            sprintf(formatted_combat, "Combat level: %d", mud->local_player->level);

            surface_draw_string(mud->surface, formatted_combat, ui_x + 5, y, 1,
                                WHITE);

            y += STATS_LINE_BREAK;
        }
    } else if (mud->ui_tab_stats_sub_tab == 1) {
        // the handler for the quests tab
        panel_clear_list(mud->panel_quest_list, mud->control_list_quest);

        panel_add_list_entry(mud->panel_quest_list, mud->control_list_quest, 0,
                             "@whi@Quest-list (green=completed)");

        for (int i = 0; i < quests_length; i++) {
            char *quest_name = quest_names[i];
            char coloured_quest[strlen(quest_name) + 6];

            sprintf(coloured_quest, "%s%s",
                    mud->quest_complete[i] ? "@gre@" : "@red@", quest_name);

            panel_add_list_entry(mud->panel_quest_list, mud->control_list_quest,
                                 i + 1, coloured_quest);
        }

        panel_draw_panel(mud->panel_quest_list);
    }

    if (!no_menus) {
        return;
    }

    int mouse_x = mud->mouse_x - ui_x;
    int mouse_y = mud->mouse_y - ui_y;

    // handle clicking of stats and quest tab, and the scroll wheel for the
    // quest list
    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < STATS_WIDTH &&
        mouse_y < height) {
        if (mud->ui_tab_stats_sub_tab == 1) {
            panel_handle_mouse(mud->panel_quest_list, mouse_x + ui_x,
                               mouse_y + ui_y, mud->last_mouse_button_down,
                               mud->mouse_button_down, mud->mouse_scroll_delta);
        }

        if (mouse_y <= STATS_TAB_HEIGHT && mud->mouse_button_click == 1) {
            if (mouse_x < (STATS_WIDTH / 2)) {
                mud->ui_tab_stats_sub_tab = 0;
            } else if (mouse_x > (STATS_WIDTH / 2)) {
                mud->ui_tab_stats_sub_tab = 1;
            }
        }
    }
}
