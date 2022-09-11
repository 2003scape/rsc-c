#include "./experience-drops.h"

void mudclient_drop_experience(mudclient *mud, int skill_index,
                               int experience) {
    if (!mud->options->experience_drops) {
        return;
    }

    int max_y = MUD_HEIGHT / 4;

    mud->experience_drop_y[mud->experience_drop_count] =
        max_y + (mud->experience_drop_count * 16);

    mud->experience_drop_skill[mud->experience_drop_count] = skill_index;
    mud->experience_drop_amount[mud->experience_drop_count] = experience;
    mud->experience_drop_speed[mud->experience_drop_count] = -30;
    mud->experience_drop_count++;
}

void mudclient_draw_experience_drops(mudclient *mud) {
    int x = mud->surface->width / 2;

    for (int i = 0; i < mud->experience_drop_count; i++) {
        int drop_y = (int)mud->experience_drop_y[i];
        int experience = mud->experience_drop_amount[i];

        char formatted_amount[15] = {0};
        mudclient_format_number_commas(mud, experience / 4, formatted_amount);

        char *skill_name = skill_names[mud->experience_drop_skill[i]];

        char formatted_drop[strlen(skill_name) + strlen(formatted_amount) + 5];
        sprintf(formatted_drop, "%s %s XP", formatted_amount, skill_name);

        surface_draw_string_centre(mud->surface, formatted_drop, x, drop_y, 1,
                                   WHITE);

        if (mud->experience_drop_speed[i] > 0) {
            mud->experience_drop_y[i] -= mud->experience_drop_speed[i];
        }

        mud->experience_drop_speed[i] += 0.5f;
    }

    int is_drop_visible = 0;

    for (int i = 0; i < mud->experience_drop_count; i++) {
        if ((int)mud->experience_drop_y[i] > 0) {
            is_drop_visible = 1;
        }
    }

    if (!is_drop_visible) {
        mud->experience_drop_count = 0;
    }
}
