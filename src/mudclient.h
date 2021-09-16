#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define K_LEFT -1
#define K_RIGHT -1
#define K_F1 -1
#define K_ENTER -1
#define K_BACKSPACE -1

#define VERSION 204

#define ZOOM_MIN 450
#define ZOOM_MAX 1250
#define ZOOM_INDOORS 550
#define ZOOM_OUTDOORS 750

#define MAGIC_LOC 128

#define FONT_COUNT 8
#define ANIMATED_MODELS_COUNT 20
#define MAX_SOCIAL_LIST_COUNT 100
#define INPUT_TEXT_LENGTH 20
#define INPUT_PM_LENGTH 80
#define GAME_OBJECTS_MAX 1000
#define WALL_OBJECTS_MAX 500
#define OBJECTS_MAX 1500
#define PLAYERS_SERVER_MAX 4000
#define PLAYERS_MAX 500
#define NPCS_SERVER_MAX 5000
#define NPCS_MAX 500
#define GROUND_ITEMS_MAX 5000
#define PRAYER_COUNT 50

#define MUD_WIDTH 512
#define MUD_HEIGHT 346

typedef struct mudclient mudclient;

#include "bzip.h"
#include "client-opcodes.h"
#include "colours.h"
#include "game-character.h"
#include "game-model.h"
#include "options.h"
#include "packet-stream.h"
#include "panel.h"
#include "scene.h"
#include "server-opcodes.h"
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
extern char login_screen_status[255];

void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code);

void init_mudclient_global();

typedef struct mudclient {
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *pixel_surface;
    Options *options;
    int8_t middle_button_down;
    int mouse_scroll_delta;
    int mouse_action_timeout;
    int mouse_x;
    int mouse_y;
    int mouse_button_down;
    int last_mouse_button_down;
    int mouse_button_click;
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
    char *login_prompt;
    char login_user_disp[22];
    int player_count;
    int npc_count;
    int login_timer;
    int camera_rotation_time;
    int message_tab_flash_all;
    int message_tab_flash_history;
    int message_tab_flash_quest;
    int message_tab_flash_private;
    int welcome_screen_already_shown;
    int system_update;
    int combat_style;
    int logout_timeout;

    int object_count;
    GameModel *object_model[OBJECTS_MAX];
    int object_x[OBJECTS_MAX];
    int object_y[OBJECTS_MAX];
    int object_id[OBJECTS_MAX];
    int object_direction[OBJECTS_MAX];
    int wall_object_count;
    GameModel *wall_object_model[WALL_OBJECTS_MAX];
    int wall_object_x[WALL_OBJECTS_MAX];
    int wall_object_y[WALL_OBJECTS_MAX];
    int wall_object_id[WALL_OBJECTS_MAX];
    int wall_object_direction[WALL_OBJECTS_MAX];
    GameCharacter *player_server[PLAYERS_SERVER_MAX];
    GameCharacter *players[PLAYERS_MAX];
    GameCharacter *npcs_server[NPCS_SERVER_MAX];
    GameCharacter *npcs[NPCS_MAX];
    int ground_item_count;
    int ground_item_x [GROUND_ITEMS_MAX];
    int ground_item_y [GROUND_ITEMS_MAX];
    int ground_item_id[GROUND_ITEMS_MAX];
    int ground_item_z [GROUND_ITEMS_MAX];
    int known_player_count;
    GameCharacter *known_players[PLAYERS_MAX];

    int8_t prayer_on[PRAYER_COUNT];
    int is_sleeping;
    int last_height_offset;
    int fog_of_war;
    GameCharacter local_player;

    int object_animation_cycle;
    int last_object_animation_cycle;
    int torch_animation_cycle;
    int last_torch_animation_cycle;
    int claw_animation_cycle;
    int last_claw_animation_cycle;

    int sprite_count;
    int items_above_head_count;
    int received_messages_count;
    int health_bar_count;

    int option_camera_mode_auto;

    /* stores absolute mouse position and initial rotation for middle click
     * camera */
    int origin_mouse_x;
    int origin_rotation;

    int camera_rotation;
    int camera_zoom;
    int camera_rotation_x;
    int camera_rotation_y;
    int camera_rotation_x_increment;
    int camera_rotation_y_increment;
    int camera_auto_rotate_player_x;
    int camera_auto_rotate_player_y;

    /* yellow/red X sprite location and sprite cycle */
    int mouse_click_x_step;
    int mouse_click_x_x;
    int mouse_click_x_y;

    int is_in_wild;
    int loading_area;
    int plane_height;
    int plane_width;
    int local_region_y;
    int region_y;
    int local_region_x;
    int region_x;
    int show_ui_wild_warn;

    int packet_last_read;

    int local_player_server_index;
    int plane_index;
    int plane_multiplier;
    int local_lower_x;
    int local_lower_y;
    int local_upper_x;
    int local_upper_y;
    int death_screen_timeout;
} mudclient;

void mudclient_new(mudclient *mud);
void mudclient_start_application(mudclient *mud, int width, int height,
                                 char *title);
void mudclient_handle_key_press(mudclient *mud, int key_code);
void mudclient_key_pressed(mudclient *mud, int code, char char_code);
void mudclient_key_released(mudclient *mud, int code);
void mudclient_mouse_moved(mudclient *mud, int x, int y);
void mudclient_mouse_released(mudclient *mud, int x, int y, int button);
void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button);
void mudclient_set_target_fps(mudclient *mud, int fps);
void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1);
void mudclient_reset_timings(mudclient *mud);
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
void mudclient_draw_login_screens(mudclient *mud);
void mudclient_reset_game(mudclient *mud);
void mudclient_login(mudclient *mud, char *username, char *password,
                     int reconnecting);
void mudclient_handle_login_screen_input(mudclient *mud);
GameModel *mudclient_create_model(mudclient *mud, int x, int y, int direction,
                                  int id, int count);
int mudclient_load_next_region(mudclient *mud, int lx, int ly);
void mudclient_check_connection(mudclient *mud);
void mudclient_handle_game_input(mudclient *mud);
void mudclient_handle_inputs(mudclient *mud);
void mudclient_update_object_animation(mudclient *mud, int object_index,
                                       char *model_name);
void mudclient_draw_game(mudclient *mud);
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
