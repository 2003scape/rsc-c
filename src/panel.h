#ifndef _H_PANEL
#define _H_PANEL

#include <string.h>

typedef struct Panel Panel;

#include "colours.h"
#include "surface.h"
#include "utility.h"

#define PANEL_TEXT 0
#define CENTRE_TEXT 1
#define GRADIENT_BG 2
#define TEXT_LIST 4
#define LIST_INPUT 5
#define TEXT_INPUT 6
#define I_TEXT_LIST 9
#define BUTTON 10
#define ROUND_BOX 11
#define IMAGE 12
#define CHECKBOX 14

#define RED_MOD 114
#define GREEN_MOD 114
#define BLUE_MOD 176

extern int panel_base_sprite_start;
extern int panel_text_list_entry_height_mod;

typedef struct Panel {
    int8_t *control_shown;
    int8_t *control_list_scrollbar_handle_dragged;
    int8_t *control_mask_text;
    int8_t *control_clicked;
    int *control_flash_text;
    int *control_list_entry_count;
    int *control_list_entry_mouse_button_down;
    int *control_list_entry_mouse_over;
    int8_t a_boolean219;
    Surface *surface;
    int control_count;
    int max_controls;
    int8_t *control_use_alternative_colour;
    int *control_x;
    int *control_y;
    int *control_type;
    int *control_width;
    int *control_height;
    int *control_input_max_len;
    int *control_text_size;
    char **control_text;
    char ***control_list_entries;
    int mouse_x;
    int mouse_y;
    int mouse_last_button_down;
    int mouse_button_down;
    int mouse_meta_button_held;
    int mouse_scroll_delta;
    int colour_scrollbar_top;
    int colour_scrollbar_bottom;
    int colour_scrollbar_handle_left;
    int colour_scrollbar_handle_mid;
    int colour_scrollbar_handle_right;
    int colour_rounded_box_out;
    int colour_rounded_box_mid;
    int colour_rounded_box_in;
    int colour_box_top_n_bottom;
    int colour_box_top_n_bottom2;
    int colour_box_left_n_right2;
    int colour_box_left_n_right;
    int focus_control_index;
} Panel;

void panel_new(Panel *panel, Surface *surface, int max);
int panel_rgb_to_int_mod(int r, int g, int b);
void panel_handle_mouse(Panel *panel, int x, int y, int last_button,
                        int is_down, int scroll_delta);
int panel_is_clicked(Panel *panel, int control);
void panel_key_press(Panel *panel, int key);
void panel_draw_panel(Panel *panel);
void panel_draw_checkbox(Panel *panel, int control, int x, int y, int width,
                         int height);
void panel_draw_text(Panel *panel, int control, int x, int y, char *text,
                     int text_size);
void panel_draw_string(Panel *panel, int control, int x, int y, char *text,
                       int text_size);
void panel_draw_text_input(Panel *panel, int control, int x, int y, int width,
                           int height, char *text, int text_size);
void panel_draw_box(Panel *panel, int x, int y, int width, int height);
void panel_draw_rounded_box(Panel *panel, int x, int y, int width, int height);
void panel_draw_picture(Panel *panel, int x, int y, int id);
void panel_draw_line_horiz(Panel *panel, int x, int y, int width);
void panel_draw_text_list(Panel *panel, int control, int x, int y, int width,
                          int height, int text_size, char **list_entries,
                          int list_entry_count, int list_entry_position);
void panel_draw_list_container(Panel *panel, int x, int y, int width,
                               int height, int corner1, int corner2);
void panel_draw_text_list_interactive(Panel *panel, int control, int x, int y,
                                      int width, int height, int text_size,
                                      char **list_entries, int list_entry_count,
                                      int list_entry_position);
int panel_add_text(Panel *panel, int x, int y, char *text, int size, int flag);
int panel_add_text_centre(Panel *panel, int x, int y, char *text, int size,
                          int flag);
int panel_add_button_background(Panel *panel, int x, int y, int width,
                                int height);
int panel_add_box_rounded(Panel *panel, int x, int y, int width, int height);
int panel_add_sprite(Panel *panel, int x, int y, int sprite_id);
int panel_add_text_list(Panel *panel, int x, int y, int width, int height,
                        int size, int max_length, int flag);
int panel_add_text_list_input(Panel *panel, int x, int y, int width, int height,
                              int size, int max_length, int flag, int flag1);
int panel_add_text_input(Panel *panel, int x, int y, int width, int height,
                         int size, int max_length, int is_password, int flag1);
int panel_add_text_list_interactive(Panel *panel, int x, int y, int width,
                                    int height, int text_size, int max_length,
                                    int flag);
int panel_add_button(Panel *panel, int x, int y, int width, int height);
int panel_add_checkbox(Panel *panel, int x, int y, int width, int height);
void panel_toggle_checkbox(Panel *panel, int control, int activated);
int panel_is_activated(Panel *panel, int control);
void panel_clear_list(Panel *panel, int control);
void panel_reset_list_props(Panel *panel, int control);
void panel_add_list_entry(Panel *panel, int control, int index, char *text);
void panel_add_list_entry_wrapped(Panel *panel, int control, char *text,
                                  int flash);
void panel_update_text(Panel *panel, int control, char *text);
char *panel_get_text(Panel *panel, int control);
void panel_show(Panel *panel, int control);
void panel_hide(Panel *panel, int control);
void panel_set_focus(Panel *panel, int control);
int panel_get_list_entry_index(Panel *panel, int control);

#endif
