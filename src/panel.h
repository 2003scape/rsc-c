#ifndef _H_PANEL
#define _H_PANEL

#include <string.h>

typedef struct Panel Panel;

#include "colours.h"
#include "mudclient.h"
#include "surface.h"
#include "utility.h"

typedef enum {
    PANEL_TEXT = 0,
    PANEL_CENTRE_TEXT = 1,
    PANEL_BOX = 2,
    PANEL_TEXT_LIST = 4,
    PANEL_LIST_INPUT = 5,
    PANEL_TEXT_INPUT = 6,
    PANEL_TEXT_LIST_INTERACTIVE = 9,
    PANEL_BUTTON = 10,
    PANEL_ROUNDED_BOX = 11,
    PANEL_SPRITE = 12,
    PANEL_CHECKBOX = 14
} PanelControlType;

#define PANEL_MAX_TEXT_LEN 255

#define PANEL_ROUNDED_BOX_OUT_COLOUR 0x758eab
#define PANEL_ROUNDED_BOX_MIDDLE_COLOUR 0x627a9e
#define PANEL_ROUNDED_BOX_IN_COLOUR 0x566488

/* colour of the top of the gradient and the thick south/east lines */
#define PANEL_BOX_TOP_COLOUR 0x545d78

/* colour of the bottom of the gradient and the thick north/west lines */
#define PANEL_BOX_BOTTOM_COLOUR 0x8792b3

/* colour of the thin north/west lines */
#define PANEL_BOX_TOP_LINE_COLOUR 0x617097

/* colour of the thin south/east lines */
#define PANEL_BOX_BOTTOM_LINE_COLOUR 0x586688

extern int panel_base_sprite_start;
extern int panel_text_list_entry_height_mod;

struct Panel {
    int8_t *control_shown;
    int8_t *control_list_scrollbar_handle_dragged;
    int8_t *control_mask_text;
    int8_t *control_clicked;
    int *control_list_position;
    int *control_list_entry_count;
    int *control_activated;
    int *control_list_entry_mouse_over;
    Surface *surface;
    int control_count;
    int max_controls;
    int8_t *control_use_alternative_colour;
    int *control_x;
    int *control_y;
    PanelControlType *control_type;
    int *control_width;
    int *control_height;
    int *control_input_max_length;
    int *control_font_style; /* also used for sprite IDs */
    char **control_text;
    char ***control_list_entries;
    int mouse_x;
    int mouse_y;
    int mouse_last_button_down;
    int mouse_button_down;
    int mouse_meta_button_held;
    int mouse_scroll_delta;
    int focus_control_index;

    /* used for resizable mode */
    int offset_x;
    int offset_y;
};

void panel_new(Panel *panel, Surface *surface, int max);
void panel_handle_mouse(Panel *panel, int x, int y, int last_button,
                        int is_down, int scroll_delta);
int panel_is_clicked(Panel *panel, int control);
void panel_key_press(Panel *panel, int key);
void panel_draw_panel(Panel *panel);
void panel_draw_checkbox(Panel *panel, int control, int x, int y, int width,
                         int height);
void panel_draw_text(Panel *panel, int control, int x, int y, char *text,
                     FontStyle font_style);
void panel_draw_string(Panel *panel, int control, int x, int y, char *text,
                       FontStyle font_style);
void panel_draw_text_input(Panel *panel, int control, int x, int y, int width,
                           int height, char *text, FontStyle font_style);
void panel_draw_box(Panel *panel, int x, int y, int width, int height);
void panel_draw_rounded_box(Panel *panel, int x, int y, int width, int height);
void panel_draw_text_list(Panel *panel, int control, int x, int y, int width,
                          int height, FontStyle font_style, char **list_entries,
                          int list_entry_count, int list_entry_position,
                          int is_interactive);
int panel_add_text(Panel *panel, int x, int y, char *text, FontStyle font_style,
                   int flag);
int panel_add_text_centre(Panel *panel, int x, int y, char *text,
                          FontStyle font_style, int flag);
int panel_add_box_rounded(Panel *panel, int x, int y, int width, int height);
int panel_add_sprite(Panel *panel, int x, int y, int sprite_id);
int panel_add_text_list(Panel *panel, int x, int y, int width, int height,
                        FontStyle font_style, int max_list_entries, int flag);
int panel_add_text_list_input(Panel *panel, int x, int y, int width, int height,
                              FontStyle font_style, int max_length,
                              int is_password, int flag1);
int panel_add_text_input(Panel *panel, int x, int y, int width, int height,
                         FontStyle font_style, int max_length, int is_password,
                         int flag1);
int panel_add_text_list_interactive(Panel *panel, int x, int y, int width,
                                    int height, FontStyle font_style,
                                    int max_length, int flag);
int panel_add_button_background(Panel *panel, int x, int y, int width,
                                int height);
int panel_add_button(Panel *panel, int x, int y, int width, int height);
int panel_add_checkbox(Panel *panel, int x, int y, int width, int height);
void panel_toggle_checkbox(Panel *panel, int control, int activated);
int panel_is_activated(Panel *panel, int control);
void panel_clear_list(Panel *panel, int control);
void panel_reset_list(Panel *panel, int control);
void panel_add_list_entry(Panel *panel, int control, int index, char *text);
void panel_add_list_entry_wrapped(Panel *panel, int control, char *text,
                                  int flash);
void panel_update_text(Panel *panel, int control, char *text);
char *panel_get_text(Panel *panel, int control);
void panel_show(Panel *panel, int control);
void panel_hide(Panel *panel, int control);
void panel_set_focus(Panel *panel, int control);
int panel_get_list_entry_index(Panel *panel, int control);
void panel_destroy(Panel *panel);

#endif
