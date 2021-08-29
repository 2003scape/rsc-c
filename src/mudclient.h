#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#define K_LEFT -1
#define K_RIGHT -1
#define K_F1 -1
#define K_ENTER -1
#define K_BACKSPACE -1

#define FONT_COUNT 8
#define ANIMATED_MODELS_COUNT 20
#define MAX_SOCIAL_LIST_COUNT 100
#define INPUT_TEXT_LENGTH 20
#define INPUT_PM_LENGTH 80
#define GAME_OBJECTS_MAX 1000

typedef struct mudclient mudclient;

#include "bzip.h"
#include "colours.h"
#include "game-model.h"
#include "options.h"
#include "packet-stream.h"
#include "panel.h"
#include "scene.h"
#include "surface.h"
#include "utility.h"
#include "version.h"
#include "world.h"

extern char *font_files[];
extern char *animated_models[];
extern char *short_skill_names[];
extern char *skill_names[];
extern char *equipment_stat_names[];
extern int experience_array[100];

void init_mudclient_global();

typedef struct mudclient {
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *pixel_surface;
    Options *options;
    int middle_button_down;
    int mouse_scroll_delta;
    int mouse_action_timeout;
    int mouse_x;
    int mouse_y;
    int mouse_button_down;
    int last_mouse_button_down;
    uint32_t timings[10];
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
    char input_text_current[INPUT_TEXT_LENGTH + 1];
    char input_pm_current[INPUT_PM_LENGTH + 1];
    char input_text_final[INPUT_TEXT_LENGTH + 1];
    char input_pm_final[INPUT_PM_LENGTH + 1];
    int origin_mouse_x;
    int origin_rotation;
    int camera_rotation;
    int fps;
    int max_read_tries;
    int world_full_timeout;
    int moderator_level;
    int auto_login_timeout;
    int message_index;
    int settings_block_chat;
    int settings_block_private;
    int settings_block_trade;
    int settings_block_duel;
    int64_t session_id;
    int friend_list_count;
    int friend_list_online[MAX_SOCIAL_LIST_COUNT * 2];
    int64_t friend_list_hashes[MAX_SOCIAL_LIST_COUNT * 2];
    int ignore_list_count;
    int64_t ignore_list[MAX_SOCIAL_LIST_COUNT * 2];
    int message_tokens[MAX_SOCIAL_LIST_COUNT];
    char *server;
    int port;
    int8_t incoming_packet[5000];
    PacketStream *packet_stream;
    int error_loading_data;
    int members;
    int sprite_media;
    int sprite_util;
    int sprite_item;
    int sprite_logo;
    int sprite_projectile;
    int sprite_texture;
    int sprite_texture_world;
    int game_width;
    int game_height;
    Surface *surface;
    Panel *panel_quest_list;
    Panel *panel_magic;
    Panel *panel_social_list;
    int control_list_quest;
    int control_list_magic;
    int control_list_social;
    Scene *scene;
    World *world;
    GameModel *game_models[GAME_OBJECTS_MAX];

    Panel *panel_login_welcome;
    Panel *panel_login_new_user;
    Panel *panel_login_existing_user;
    int control_welcome_new_user;
    int control_welcome_existing_user;
    int refer_id;
    int control_login_new_ok;
    int control_register_status;
    int control_register_user;
    int control_register_password;
    int control_register_confirm_password;
    int control_register_checkbox;
    int control_register_submit;
    int control_register_cancel;
    int control_login_status;
    int control_login_user;
    int control_login_password;
    int control_login_ok;
    int control_login_cancel;
    int control_login_recover;

    int logged_in;
    int login_screen;
    char login_user[21];
    char login_pass[21];
    char *login_user_desc;
    char login_user_disp[22];
    int player_count;
    int npc_count;
    int login_timer;
    int camera_rotation_time;
    int camera_rotation_x;
    int camera_rotation_y;
    int camera_rotation_x_increment;
    int camera_rotation_y_increment;
    int message_tab_flash_all;
    int message_tab_flash_history;
    int message_tab_flash_quest;
    int message_tab_flash_private;
    int welcome_screen_already_shown;
} mudclient;

void mudclient_new(mudclient *mud);
void mudclient_start_application(mudclient *mud, int width, int height,
                                 char *title);
void mudclient_key_pressed(mudclient *mud, int code, char char_code);
void mudclient_key_released(mudclient *mud, int code);
void mudclient_mouse_moved(mudclient *mud, int x, int y);
void mudclient_mouse_released(mudclient *mud, int x, int y, int button);
void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button);
void mudclient_set_target_fps(mudclient *mud, int fps);
void mudclient_start(mudclient *mud);
void mudclient_stop(mudclient *mud);
void mudclient_draw_string(mudclient *mud, char *string, int font, int x,
                           int y);
void mudclient_draw_loading_screen(mudclient *mud, int percent, char *text);
void mudclient_show_loading_progress(mudclient *mud, int percent, char *text);
int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent);
void mudclient_parse_tga(mudclient *mud, int8_t *tga_buffer);
void mudclient_load_jagex(mudclient *mud);
void mudclient_load_game_config(mudclient *mud);
void mudclient_load_media(mudclient *mud);
void mudclient_load_entities(mudclient *mud);
void mudclient_load_textures(mudclient *mud);
void mudclient_load_models(mudclient *mud);
void mudclient_load_maps(mudclient *mud);
void mudclient_create_login_panels(mudclient *mud);
void mudclient_reset_login_screen_variables(mudclient *mud);
void mudclient_render_login_screen_viewports(mudclient *mud);
void mudclient_handle_inputs(mudclient *mud);
void mudclient_start_game(mudclient *mud);
void mudclient_draw(mudclient *mud);
void mudclient_poll_sdl_events(mudclient *mud);
void mudclient_run(mudclient *mud);
void mudclient_sort_friends(mudclient *mud);
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
