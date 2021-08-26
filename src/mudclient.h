#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define K_LEFT -1
#define K_RIGHT -1
#define K_F1 -1
#define K_ENTER -1
#define K_BACKSPACE -1

typedef struct mudclient mudclient;

#include "colours.h"
#include "game-model.h"
#include "options.h"
#include "scene.h"
#include "surface.h"
#include "utility.h"
#include "world.h"

extern char *short_skill_names[];
extern char *skill_names[];
extern char *equipment_stat_names[];
extern int experience_array[100];

void init_mudclient_global();

typedef struct mudclient {
    Options options;
    int middle_button_down;
    int mouse_scroll_delta;
    int mouse_action_timeout;
    int mouse_x;
    int mouse_y;
    int mouse_button_down;
    int last_mouse_button_down;
    int stop_timeout;
    int interlace_timer;
    int loading_progress_percent;
    int applet_width;
    int applet_height;
    int target_fps;
    int max_draw_time;
    int loading_step;
    int has_referer_logo_not_used;
    char *loading_progess_text;
    int key_left;
    int key_right;
    int thread_sleep;
    int interlace;
    char input_text_current[21];
    char input_pm_current[81];
    char input_text_final[21];
    char input_pm_final[81];
    int origin_mouse_x;
    int origin_rotation;
    int camera_rotation;
    int mobile_input_caret;
    int fps;
} mudclient;

void mudclient_new(mudclient *mud);
void mudclient_key_pressed(mudclient *mud, int code, char char_code);
void mudclient_key_released(mudclient *mud, int code);
void mudclient_mouse_moved(mudclient *mud, int x, int y);
void mudclient_mouse_released(mudclient *mud, int x, int y, int button);
void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button);
void mudclient_set_target_fps(mudclient *mud, int fps);
void mudclient_start(mudclient *mud);
void mudclient_stop(mudclient *mud);
void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id);
void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id);
void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int tx, int ty);
void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int tx, int ty);

int main(int argc, char **argv);

#endif
