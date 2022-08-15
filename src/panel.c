#include "panel.h"

int panel_base_sprite_start = 0;
int panel_text_list_entry_height_mod = 0;

void panel_new(Panel *panel, Surface *surface, int max) {
    memset(panel, 0, sizeof(Panel));

    panel->surface = surface;
    panel->max_controls = max;

    panel->focus_control_index = -1;

    panel->control_shown = calloc(max, sizeof(int8_t));
    panel->control_list_scrollbar_handle_dragged = calloc(max, sizeof(int8_t));
    panel->control_mask_text = calloc(max, sizeof(int8_t));
    panel->control_clicked = calloc(max, sizeof(int8_t));
    panel->control_use_alternative_colour = calloc(max, sizeof(int8_t));
    panel->control_list_position = calloc(max, sizeof(int));
    panel->control_list_entry_count = calloc(max, sizeof(int));
    panel->control_activated = calloc(max, sizeof(int));
    panel->control_list_entry_mouse_over = calloc(max, sizeof(int));
    panel->control_x = calloc(max, sizeof(int));
    panel->control_y = calloc(max, sizeof(int));
    panel->control_type = calloc(max, sizeof(int));
    panel->control_width = calloc(max, sizeof(int));
    panel->control_height = calloc(max, sizeof(int));
    panel->control_input_max_length = calloc(max, sizeof(int));
    panel->control_text_size = calloc(max, sizeof(int));
    panel->control_text = calloc(max, sizeof(char *));
    panel->control_list_entries = calloc(max, sizeof(char **));
}

void panel_handle_mouse(Panel *panel, int x, int y, int last_button,
                        int is_down, int scroll_delta) {
    panel->mouse_x = x;
    panel->mouse_y = y;
    panel->mouse_button_down = is_down;
    panel->mouse_scroll_delta = scroll_delta;

    if (last_button != 0) {
        panel->mouse_last_button_down = last_button;
    }

    if (last_button == 1) {
        for (int i = 0; i < panel->control_count; i++) {
            int control_type = panel->control_type[i];

            if (panel->control_shown[i] &&
                (control_type == PANEL_BUTTON ||
                 control_type == PANEL_CHECKBOX) &&
                panel->mouse_x >= panel->control_x[i] + panel->offset_x &&
                panel->mouse_y >= panel->control_y[i] + panel->offset_y &&
                panel->mouse_x <= panel->control_x[i] + panel->offset_x +
                                      panel->control_width[i] &&
                panel->mouse_y <= panel->control_y[i] + panel->offset_y +
                                      panel->control_height[i]) {
                if (control_type == PANEL_BUTTON) {
                    panel->control_clicked[i] = 1;
                } else if (control_type == PANEL_CHECKBOX) {
                    panel->control_activated[i] = 1 - panel->control_activated[i];
                }
            }
        }
    }

    if (is_down == 1) {
        panel->mouse_meta_button_held++;
    } else {
        panel->mouse_meta_button_held = 0;
    }
}

int panel_is_clicked(Panel *panel, int control) {
    if (panel->control_shown[control] && panel->control_clicked[control]) {
        panel->control_clicked[control] = 0;
        return 1;
    }

    return 0;
}

void panel_key_press(Panel *panel, int key) {
    if (key == 0) {
        return;
    }

    if (panel->focus_control_index != -1 &&
        panel->control_text[panel->focus_control_index] != NULL &&
        panel->control_shown[panel->focus_control_index]) {
        int input_len = strlen(panel->control_text[panel->focus_control_index]);

        /* backspace */
        if (key == 8 && input_len > 0) {
            panel->control_text[panel->focus_control_index][input_len - 1] =
                '\0';
        }

        /* enter */
        if ((key == 10 || key == 13) && input_len > 0) {
            panel->control_clicked[panel->focus_control_index] = 1;
        }

        if (input_len <
            panel->control_input_max_length[panel->focus_control_index]) {

            for (int i = 0; i < CHAR_SET_LENGTH; i++) {
                if (key == (int)CHAR_SET[i]) {
                    panel->control_text[panel->focus_control_index]
                                       [input_len++] = (char)key;

                    panel->control_text[panel->focus_control_index][input_len] =
                        '\0';
                }
            }
        }

        /* tab */
        if (key == 9) {
            do {
                panel->focus_control_index =
                    (panel->focus_control_index + 1) % panel->control_count;
            } while (panel->control_type[panel->focus_control_index] != 5 &&
                     panel->control_type[panel->focus_control_index] != 6);
        }
    }
}

void panel_draw_panel(Panel *panel) {
    for (int i = 0; i < panel->control_count; i++) {
        if (!panel->control_shown[i]) {
            continue;
        }

        switch (panel->control_type[i]) {
        case PANEL_TEXT:
            panel_draw_text(panel, i, panel->control_x[i] + panel->offset_x,
                            panel->control_y[i] + panel->offset_y,
                            panel->control_text[i],
                            panel->control_text_size[i]);
            break;
        case PANEL_CENTRE_TEXT:
            panel_draw_text(
                panel, i,
                panel->control_x[i] -
                    (surface_text_width(panel->control_text[i],
                                        panel->control_text_size[i]) /
                     2) +
                    panel->offset_x,
                panel->control_y[i] + panel->offset_y, panel->control_text[i],
                panel->control_text_size[i]);
            break;
        case PANEL_BOX:
            panel_draw_box(panel, panel->control_x[i] + panel->offset_x,
                           panel->control_y[i] + panel->offset_y,
                           panel->control_width[i], panel->control_height[i]);
            break;
        case PANEL_TEXT_LIST:
        case PANEL_TEXT_LIST_INTERACTIVE:
            panel_draw_text_list(
                panel, i, panel->control_x[i] + panel->offset_x,
                panel->control_y[i] + panel->offset_y, panel->control_width[i],
                panel->control_height[i], panel->control_text_size[i],
                panel->control_list_entries[i],
                panel->control_list_entry_count[i],
                panel->control_list_position[i],
                panel->control_type[i] == PANEL_TEXT_LIST_INTERACTIVE);
            break;
        case PANEL_LIST_INPUT:
        case PANEL_TEXT_INPUT:
            panel_draw_text_input(
                panel, i, panel->control_x[i] + panel->offset_x,
                panel->control_y[i] + panel->offset_y, panel->control_width[i],
                panel->control_height[i], panel->control_text[i],
                panel->control_text_size[i]);
            break;
        case PANEL_ROUNDED_BOX:
            panel_draw_rounded_box(panel, panel->control_x[i] + panel->offset_x,
                                   panel->control_y[i] + panel->offset_y,
                                   panel->control_width[i],
                                   panel->control_height[i]);
            break;
        case PANEL_SPRITE:
            surface_draw_sprite_from3(panel->surface,
                                      panel->control_x[i] + panel->offset_x,
                                      panel->control_y[i] + panel->offset_y,
                                      panel->control_text_size[i]);
            break;
        case PANEL_CHECKBOX:
            panel_draw_checkbox(panel, i, panel->control_x[i] + panel->offset_x,
                                panel->control_y[i] + panel->offset_y,
                                panel->control_width[i],
                                panel->control_height[i]);
            break;
        }
    }

    panel->mouse_last_button_down = 0;
}

void panel_draw_checkbox(Panel *panel, int control, int x, int y, int width,
                         int height) {
    surface_draw_box(panel->surface, x, y, width, height, WHITE);

    surface_draw_line_horizontal(panel->surface, x, y, width,
                                 PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_vertical(panel->surface, x, y, height,
                               PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_horizontal(panel->surface, x, y + height - 1, width,
                                 PANEL_BOX_TOP_COLOUR);

    surface_draw_line_vertical(panel->surface, x + width - 1, y, height,
                               PANEL_BOX_TOP_COLOUR);

    if (panel->control_activated[control] == 1) {
        for (int i = 0; i < height; i++) {
            surface_draw_line_horizontal(panel->surface, x + i, y + i, 1,
                                         BLACK);

            surface_draw_line_horizontal(panel->surface, x + width - 1 - i,
                                         y + i, 1, BLACK);
        }
    }
}

void panel_draw_text(Panel *panel, int control, int x, int y, char *text,
                     int text_size) {
    y += (surface_text_height(text_size) / 3);

    panel_draw_string(panel, control, x, y, text, text_size);
}

void panel_draw_string(Panel *panel, int control, int x, int y, char *text,
                       int text_size) {
    int text_colour = 0;

    if (panel->control_use_alternative_colour[control]) {
        text_colour = WHITE;
    }

    surface_draw_string(panel->surface, text, x, y, text_size, text_colour);
}

void panel_draw_text_input(Panel *panel, int control, int x, int y, int width,
                           int height, char *text, int text_size) {
    int text_length = strlen(text);

    char display_text[text_length + 2]; /* potential extra * if in focus */
    memset(display_text, 0, text_length + 2);

    if (panel->control_mask_text[control]) {
        for (int i = 0; i < text_length; i++) {
            display_text[i] = 'X';
        }
    } else {
        strcpy(display_text, text);
    }

    if (panel->control_type[control] == PANEL_LIST_INPUT) {
        if (panel->mouse_last_button_down == 1 && panel->mouse_x >= x &&
            panel->mouse_y >= y - (height / 2) && panel->mouse_x <= x + width &&
            panel->mouse_y <= y + (height / 2)) {
            panel->focus_control_index = control;
        }
    } else if (panel->control_type[control] == PANEL_TEXT_INPUT) {
        /* not original, but none of the original text inputs used alternative
         * colour */
        int is_centred = !panel->control_use_alternative_colour[control];
        int min_x = is_centred ? x - (width / 2) : x;
        int max_x = is_centred ? x + (width / 2) : x + width;

        if (panel->mouse_last_button_down == 1 &&
            panel->mouse_x >= min_x &&
            panel->mouse_y >= y - (height / 2) &&
            panel->mouse_x <= max_x &&
            panel->mouse_y <= y + (height / 2)) {
            panel->focus_control_index = control;
        }

        if (is_centred) {
            x -= surface_text_width(display_text, text_size) / 2;
        }
    }

    if (panel->focus_control_index == control) {
        display_text[text_length] = '*';
        display_text[text_length + 1] = '\0';
    }

    y += (surface_text_height(text_size) / 3);

    panel_draw_string(panel, control, x, y, display_text, text_size);
}

void panel_draw_box(Panel *panel, int x, int y, int width, int height) {
    surface_set_bounds(panel->surface, x, y, x + width, y + height);

    surface_draw_gradient(panel->surface, x, y, width, height,
                          PANEL_BOX_TOP_COLOUR, PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_horizontal(panel->surface, x, y, width,
                                 PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_horizontal(panel->surface, x + 1, y + 1, width - 2,
                                 PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_horizontal(panel->surface, x + 2, y + 2, width - 4,
                                 PANEL_BOX_TOP_LINE_COLOUR);

    surface_draw_line_vertical(panel->surface, x, y, height,
                               PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_vertical(panel->surface, x + 1, y + 1, height - 2,
                               PANEL_BOX_BOTTOM_COLOUR);

    surface_draw_line_vertical(panel->surface, x + 2, y + 2, height - 4,
                               PANEL_BOX_TOP_LINE_COLOUR);

    surface_draw_line_horizontal(panel->surface, x, y + height - 1, width,
                                 PANEL_BOX_TOP_COLOUR);

    surface_draw_line_horizontal(panel->surface, x + 1, y + height - 2,
                                 width - 2, PANEL_BOX_TOP_COLOUR);

    surface_draw_line_horizontal(panel->surface, x + 2, y + height - 3,
                                 width - 4, PANEL_BOX_BOTTOM_LINE_COLOUR);

    surface_draw_line_vertical(panel->surface, x + width - 1, y, height,
                               PANEL_BOX_TOP_COLOUR);

    surface_draw_line_vertical(panel->surface, x + width - 2, y + 1, height - 2,
                               PANEL_BOX_TOP_COLOUR);

    surface_draw_line_vertical(panel->surface, x + width - 3, y + 2, height - 4,
                               PANEL_BOX_BOTTOM_LINE_COLOUR);

    surface_reset_bounds(panel->surface);
}

void panel_draw_rounded_box(Panel *panel, int x, int y, int width, int height) {
    surface_draw_box(panel->surface, x, y, width, height, 0);

    surface_draw_border(panel->surface, x, y, width, height,
                          PANEL_ROUNDED_BOX_OUT_COLOUR);

    surface_draw_border(panel->surface, x + 1, y + 1, width - 2, height - 2,
                          PANEL_ROUNDED_BOX_MIDDLE_COLOUR);

    surface_draw_border(panel->surface, x + 2, y + 2, width - 4, height - 4,
                          PANEL_ROUNDED_BOX_IN_COLOUR);

    surface_draw_sprite_from3(panel->surface, x, y,
                              2 + panel_base_sprite_start);

    surface_draw_sprite_from3(panel->surface, x + width - 7, y,
                              3 + panel_base_sprite_start);

    surface_draw_sprite_from3(panel->surface, x, y + height - 7,
                              4 + panel_base_sprite_start);

    surface_draw_sprite_from3(panel->surface, x + width - 7, y + height - 7,
                              5 + panel_base_sprite_start);
}

void panel_draw_text_list(Panel *panel, int control, int x, int y, int width,
                          int height, int text_size, char **list_entries,
                          int list_entry_count, int list_entry_position,
                          int is_interactive) {
    int displayed_entry_count = height / surface_text_height(text_size);
    int max_entries = list_entry_count - displayed_entry_count;

    if (list_entry_position > max_entries) {
        list_entry_position = max_entries;
    } else if (list_entry_position < 0) {
        list_entry_position = 0;
    }

    panel->control_list_position[control] = list_entry_position;

    if (displayed_entry_count < list_entry_count) {
        int corner_top_right = x + width - 12;

        int scrub_height =
            ((height - 27) * displayed_entry_count) / list_entry_count;

        if (scrub_height < 6) {
            scrub_height = 6;
        }

        int scrub_y =
            ((height - 27 - scrub_height) * list_entry_position) / max_entries;

        if (panel->mouse_scroll_delta != 0 && panel->mouse_x > x &&
            panel->mouse_x < x + width && panel->mouse_y > y &&
            panel->mouse_y < y + height) {
            list_entry_position += panel->mouse_scroll_delta;

            if (list_entry_position < 0) {
                list_entry_position = 0;
            } else if (list_entry_position > max_entries) {
                list_entry_position = max_entries;
            }

            panel->control_list_position[control] = list_entry_position;
        }

        /* the up and down arrow buttons on the scrollbar */
        if (panel->mouse_button_down == 1 &&
            panel->mouse_x >= corner_top_right &&
            panel->mouse_x <= corner_top_right + 12) {
            if (panel->mouse_y > y && panel->mouse_y < y + 12 &&
                list_entry_position > 0) {
                list_entry_position--;
            }

            if (panel->mouse_y > y + height - 12 &&
                panel->mouse_y < y + height &&
                list_entry_position < max_entries) {
                list_entry_position++;
            }

            panel->control_list_position[control] = list_entry_position;
        }

        /* handle the thumb/middle section dragging of the scrollbar */
        if (panel->mouse_button_down == 1) {
            /* mouse is within the handle */
            int is_within_x = (panel->mouse_x >= corner_top_right &&
                               panel->mouse_x <= corner_top_right + 12);

            /* clicked and are holding down */
            int is_dragging =
                panel->control_list_scrollbar_handle_dragged[control];

            int off_handle_scroll_drag =
                panel->surface->mud->options->off_handle_scroll_drag;

            if (!off_handle_scroll_drag) {
                is_dragging =
                    is_dragging && (panel->mouse_x >= corner_top_right - 12 &&
                                    panel->mouse_x <= corner_top_right + 24);
            }

            if ((is_within_x || is_dragging) &&
                panel->mouse_y > y + 12 && panel->mouse_y < y + height - 12) {
                panel->control_list_scrollbar_handle_dragged[control] = 1;

                int l3 = panel->mouse_y - y - 12 - (scrub_height / 2);

                list_entry_position = (l3 * list_entry_count) / (height - 24);

                if (list_entry_position < 0) {
                    list_entry_position = 0;
                } else if (list_entry_position > max_entries) {
                    list_entry_position = max_entries;
                }

                panel->control_list_position[control] = list_entry_position;
            }
        } else {
            panel->control_list_scrollbar_handle_dragged[control] = 0;
        }

        scrub_y =
            ((height - 27 - scrub_height) * list_entry_position) / max_entries;

        surface_draw_scrollbar(panel->surface, x, y, width, height, scrub_y, scrub_height);
    } else {
        list_entry_position = 0;
        panel->control_list_position[control] = 0;
    }

    if (is_interactive) {
        panel->control_list_entry_mouse_over[control] = -1;
    }

    int list_start_y =
        height - displayed_entry_count * surface_text_height(text_size);

    int list_y =
        y + (((surface_text_height(text_size) * 5) / 6) + list_start_y / 2);

    for (int i = list_entry_position; i < list_entry_count; i++) {
        if (is_interactive) {
            int text_colour =
                panel->control_use_alternative_colour[control] ? WHITE : BLACK;

            if (panel->mouse_x >= x + 2 && panel->mouse_x <= x + width - 12 &&
                panel->mouse_y - 2 <= list_y &&
                panel->mouse_y - 2 > list_y - surface_text_height(text_size)) {
                if (panel->control_use_alternative_colour[control]) {
                    text_colour = GREY_80;
                } else {
                    text_colour = WHITE;
                }

                panel->control_list_entry_mouse_over[control] = i;

                if (panel->mouse_last_button_down == 1) {
                    panel->control_activated[control] = i;
                    panel->control_clicked[control] = 1;
                }
            }

            if (panel->control_activated[control] == i) {
                text_colour = RED;
            }

            surface_draw_string(panel->surface, list_entries[i], x + 2, list_y,
                                text_size, text_colour);

            list_y += surface_text_height(text_size);
        } else {
            panel_draw_string(panel, control, x + 2, list_y, list_entries[i],
                              text_size);

            list_y += surface_text_height(text_size) -
                      panel_text_list_entry_height_mod;
        }

        if (list_y >= y + height) {
            return;
        }
    }
}

int panel_prepare_component(Panel *panel, int type, int x, int y) {
    panel->control_type[panel->control_count] = type;
    panel->control_shown[panel->control_count] = 1;
    panel->control_x[panel->control_count] = x;
    panel->control_y[panel->control_count] = y;

    return panel->control_count++;
}

int panel_add_text(Panel *panel, int x, int y, char *text, int size, int flag) {
    panel->control_text_size[panel->control_count] = size;
    panel->control_use_alternative_colour[panel->control_count] = flag;
    panel->control_text[panel->control_count] = text;

    return panel_prepare_component(panel, PANEL_TEXT, x, y);
}

int panel_add_text_centre(Panel *panel, int x, int y, char *text, int size,
                          int flag) {
    panel->control_text_size[panel->control_count] = size;
    panel->control_use_alternative_colour[panel->control_count] = flag;
    panel->control_text[panel->control_count] = text;

    return panel_prepare_component(panel, PANEL_CENTRE_TEXT, x, y);
}

int panel_add_button_background(Panel *panel, int x, int y, int width,
                                int height) {
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;

    return panel_prepare_component(panel, PANEL_BOX, x - (width / 2),
                                   y - (height / 2));
}

int panel_add_box_rounded(Panel *panel, int x, int y, int width, int height) {
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;

    return panel_prepare_component(panel, PANEL_ROUNDED_BOX, x - (width / 2),
                                   y - (height / 2));
}

int panel_add_sprite(Panel *panel, int x, int y, int sprite_id) {
    int width = panel->surface->sprite_width[sprite_id];
    int height = panel->surface->sprite_height[sprite_id];

    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;
    panel->control_text_size[panel->control_count] = sprite_id;

    return panel_prepare_component(panel, PANEL_SPRITE, x - (width / 2),
                                   y - (height / 2));
}

int panel_add_text_list(Panel *panel, int x, int y, int width, int height,
                        int size, int max_length, int flag) {
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;
    panel->control_use_alternative_colour[panel->control_count] = flag;
    panel->control_text_size[panel->control_count] = size;
    panel->control_input_max_length[panel->control_count] = max_length;

    panel->control_list_entries[panel->control_count] =
        calloc(max_length, sizeof(char *));

    for (int i = 0; i < max_length; i++) {
        panel->control_list_entries[panel->control_count][i] =
            calloc(128, sizeof(char));
    }

    return panel_prepare_component(panel, PANEL_TEXT_LIST, x, y);
}

int panel_add_text_list_input(Panel *panel, int x, int y, int width, int height,
                              int size, int max_length, int flag, int flag1) {
    panel->control_mask_text[panel->control_count] = flag;
    panel->control_text_size[panel->control_count] = size;
    panel->control_use_alternative_colour[panel->control_count] = flag1;
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;
    panel->control_input_max_length[panel->control_count] = max_length;

    panel->control_text[panel->control_count] =
        calloc(max_length + 1, sizeof(char *));

    return panel_prepare_component(panel, PANEL_LIST_INPUT, x, y);
}

int panel_add_text_input(Panel *panel, int x, int y, int width, int height,
                         int size, int max_length, int is_password, int flag1) {
    panel->control_mask_text[panel->control_count] = is_password;
    panel->control_text_size[panel->control_count] = size;
    panel->control_use_alternative_colour[panel->control_count] = flag1;
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;
    panel->control_input_max_length[panel->control_count] = max_length;

    panel->control_text[panel->control_count] =
        calloc((max_length + 1), sizeof(char *));

    return panel_prepare_component(panel, PANEL_TEXT_INPUT, x, y);
}

int panel_add_text_list_interactive(Panel *panel, int x, int y, int width,
                                    int height, int text_size, int max_length,
                                    int flag) {
    panel->control_shown[panel->control_count] = 1;
    panel->control_text_size[panel->control_count] = text_size;
    panel->control_use_alternative_colour[panel->control_count] = flag;
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;
    panel->control_input_max_length[panel->control_count] = max_length;

    panel->control_list_entries[panel->control_count] =
        calloc(max_length + 1, sizeof(char *));

    for (int i = 0; i < max_length + 1; i++) {
        panel->control_list_entries[panel->control_count][i] =
            calloc(128, sizeof(char));
    }

    panel->control_activated[panel->control_count] = -1;
    panel->control_list_entry_mouse_over[panel->control_count] = -1;

    return panel_prepare_component(panel, PANEL_TEXT_LIST_INTERACTIVE, x, y);
}

int panel_add_button(Panel *panel, int x, int y, int width, int height) {
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;

    return panel_prepare_component(panel, PANEL_BUTTON, x - (width / 2),
                                   y - (height / 2));
}

int panel_add_checkbox(Panel *panel, int x, int y, int width, int height) {
    panel->control_width[panel->control_count] = width;
    panel->control_height[panel->control_count] = height;

    return panel_prepare_component(panel, PANEL_CHECKBOX, x, y);
}

void panel_toggle_checkbox(Panel *panel, int control, int activated) {
    panel->control_activated[control] = activated;
}

int panel_is_activated(Panel *panel, int control) {
    return panel->control_activated[control] != 0;
}

void panel_clear_list(Panel *panel, int control) {
    panel->control_list_entry_count[control] = 0;
}

void panel_reset_list(Panel *panel, int control) {
    panel->control_list_position[control] = 0;
    panel->control_list_entry_mouse_over[control] = -1;
}

void panel_add_list_entry(Panel *panel, int control, int index, char *text) {
    if (panel->control_type[control] == PANEL_TEXT_LIST_INTERACTIVE) {
        strcpy(panel->control_list_entries[control][index], text);
    } else {
        panel->control_list_entries[control][index] = text;
    }

    if (index + 1 > panel->control_list_entry_count[control]) {
        panel->control_list_entry_count[control] = index + 1;
    }
}

void panel_add_list_entry_wrapped(Panel *panel, int control, char *text,
                                  int flash) {
    int index = panel->control_list_entry_count[control]++;

    if (index >= panel->control_input_max_length[control]) {
        index--;

        panel->control_list_entry_count[control]--;

        for (int i = 0; i < index; i++) {
            strcpy(panel->control_list_entries[control][i],
                   panel->control_list_entries[control][i + 1]);
        }
    }

    if (panel->control_type[control] == PANEL_TEXT_LIST) {
        strcpy(panel->control_list_entries[control][index], text);
    } else {
        panel->control_list_entries[control][index] = text;
    }

    if (flash) {
        panel->control_list_position[control] = 999999;
    }
}

// TODO set_text?
// TODO strcpy any control_text setting
void panel_update_text(Panel *panel, int control, char *text) {
    if (panel->control_type[control] == PANEL_TEXT_INPUT ||
        panel->control_type[control] == PANEL_LIST_INPUT) {
        strcpy(panel->control_text[control], text);
    } else {
        panel->control_text[control] = text;
    }
}

char *panel_get_text(Panel *panel, int control) {
    if (panel->control_text[control] == NULL) {
        return "null";
    }

    return panel->control_text[control];
}

void panel_show(Panel *panel, int control) {
    panel->control_shown[control] = 1;
}

void panel_hide(Panel *panel, int control) {
    panel->control_shown[control] = 0;
}

void panel_set_focus(Panel *panel, int control) {
    panel->focus_control_index = control;
}

int panel_get_list_entry_index(Panel *panel, int control) {
    return panel->control_list_entry_mouse_over[control];
}
