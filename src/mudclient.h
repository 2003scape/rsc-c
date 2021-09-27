#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WII
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "config85_jag.h"
#include "entity24_jag.h"
#include "entity24_mem.h"
#include "filter2_jag.h"
#include "fonts1_jag.h"
#include "jagex_jag.h"
#include "land63_jag.h"
#include "land63_mem.h"
#include "maps63_jag.h"
#include "maps63_mem.h"
#include "media58_jag.h"
#include "models36_jag.h"
#include "sounds1_mem.h"
#include "textures17_jag.h"

#include "arrow_yuv.h"
#include "rsc_background_yuv.h"
#else
#include <SDL2/SDL.h>
#endif

#define K_LEFT 37
#define K_RIGHT 39
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
#define SOCIAL_LIST_MAX 100
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
#define PLAYER_STAT_COUNT 18
#define PROJECTILE_RANGE_MAX 40
#define RECEIVED_MESSAGE_MAX 50
#define ACTION_BUBBLE_MAX 50
#define HEALTH_BAR_MAX 50
#define TELEPORT_BUBBLE_MAX 50

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
extern int npc_animation_array[8][12];
extern int character_walk_model[4];
extern int character_combat_model_array1[8];
extern int character_combat_model_array2[8];
extern int player_hair_colours[10];
extern int player_top_bottom_colours[15];
extern int player_skin_colours[5];

#ifndef WII
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code);
#else
void draw_arrow(uint8_t *framebuffer, int mouse_x, int mouse_y);
#endif

void init_mudclient_global();

typedef struct mudclient {
#ifdef WII
    /* store two for double-buffering */
    uint8_t **framebuffers;

    /* points to one of the two frame buffers in framebuffers */
    uint8_t *framebuffer;

    /* index of active framebuffer */
    int active_framebuffer;

    int last_wii_x;
    int last_wii_y;
    int last_wii_button;
#else
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *pixel_surface;
#endif
    Options *options;
    int applet_width;
    int applet_height;

    int8_t middle_button_down;
    int mouse_scroll_delta;
    int mouse_action_timeout;
    int mouse_x;
    int mouse_y;
    int mouse_button_down;
    int last_mouse_button_down;
    int mouse_button_click;

    int8_t key_left;
    int8_t key_right;

    int loading_step;
    int loading_progress_percent;
    char *loading_progess_text;
    int error_loading_data;

    uint32_t timings[10];
    int stop_timeout;
    int interlace_timer;

    int fps;
    int target_fps;
    int max_draw_time;
    int thread_sleep;

    int8_t interlace;
    char input_text_current[INPUT_TEXT_LENGTH + 1];
    char input_pm_current[INPUT_PM_LENGTH + 1];
    char input_text_final[INPUT_TEXT_LENGTH + 1];
    char input_pm_final[INPUT_PM_LENGTH + 1];
    int max_read_tries;
    int world_full_timeout;
    int moderator_level;
    int auto_login_timeout;

    /* social lists (friends and ignore) */
    int message_index;
    int message_tokens[SOCIAL_LIST_MAX];
    int friend_list_count;
    int64_t friend_list_hashes[SOCIAL_LIST_MAX * 2];
    int friend_list_online[SOCIAL_LIST_MAX * 2];
    int ignore_list_count;
    int64_t ignore_list[SOCIAL_LIST_MAX * 2];

    /* options settings */
    int8_t settings_camera_auto;
    int8_t settings_block_chat;
    int8_t settings_block_private;
    int8_t settings_block_trade;
    int8_t settings_block_duel;

    char *server;
    int port;
    int8_t incoming_packet[5000];
    PacketStream *packet_stream;

    int members;

    /* sprite indexes used for surface sprite drawing */
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

    int64_t session_id;
    int logged_in;
    int login_screen;
    char login_user[21];
    char login_pass[21];
    char *login_prompt;
    char login_user_disp[22];

    int login_timer;
    int message_tab_flash_all;
    int message_tab_flash_history;
    int message_tab_flash_quest;
    int message_tab_flash_private;
    int welcome_screen_already_shown;
    int system_update;
    int combat_style;
    int logout_timeout;
    int combat_timeout;

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

    int player_server_indexes[PLAYERS_MAX];
    GameCharacter *player_server[PLAYERS_SERVER_MAX];

    int player_count;
    GameCharacter *players[PLAYERS_MAX];

    int known_player_count;
    GameCharacter *known_players[PLAYERS_MAX];

    GameCharacter *local_player;

    GameCharacter *npcs_server[NPCS_SERVER_MAX];

    int npc_count;
    GameCharacter *npcs[NPCS_MAX];

    int known_npc_count;
    GameCharacter *known_npcs[NPCS_MAX];

    int ground_item_count;
    int ground_item_x [GROUND_ITEMS_MAX];
    int ground_item_y [GROUND_ITEMS_MAX];
    int ground_item_id[GROUND_ITEMS_MAX];
    int ground_item_z [GROUND_ITEMS_MAX];

    int8_t is_sleeping;

    /* fade distant landscape */
    int8_t fog_of_war;

    /* used to keep track of model indexes to swap to in order to simulate ;
     * movement */
    int object_animation_count;
    int object_animation_cycle;
    int last_object_animation_cycle;
    int torch_animation_cycle;
    int last_torch_animation_cycle;
    int claw_animation_cycle;
    int last_claw_animation_cycle;

    /* amount of entity, action and teleport bubble sprites */
    int scene_sprite_count;

    /* (usually) yellow messages above player and NPC heads */
    int received_messages_count;
    int received_message_x[RECEIVED_MESSAGE_MAX];
    int received_message_y[RECEIVED_MESSAGE_MAX];
    int received_message_mid_point[RECEIVED_MESSAGE_MAX];
    int received_message_height[RECEIVED_MESSAGE_MAX];
    char *received_messages[RECEIVED_MESSAGE_MAX];

    /* stores absolute mouse position and initial rotation for middle click
     * camera */
    int origin_mouse_x;
    int origin_rotation;

    int camera_angle;
    int camera_rotation;
    int camera_zoom;
    int camera_rotation_time;
    int camera_rotation_x;
    int camera_rotation_y;
    int camera_rotation_x_increment;
    int camera_rotation_y_increment;
    int camera_auto_rotate_player_x;
    int camera_auto_rotate_player_y;
    int an_int_707;

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
    int last_height_offset;
    int plane_index;
    int plane_multiplier;
    int local_lower_x;
    int local_lower_y;
    int local_upper_x;
    int local_upper_y;

    int death_screen_timeout;

    int8_t prayer_on[PRAYER_COUNT];

    int player_stat_current[PLAYER_STAT_COUNT];
    int player_stat_base[PLAYER_STAT_COUNT];

    int action_bubble_count;
	int action_bubble_x[ACTION_BUBBLE_MAX];
	int action_bubble_y[ACTION_BUBBLE_MAX];
	int action_bubble_scale[ACTION_BUBBLE_MAX];
	int action_bubble_item[ACTION_BUBBLE_MAX];

    int health_bar_count;
    int health_bar_x[HEALTH_BAR_MAX];
    int health_bar_y[HEALTH_BAR_MAX];
    int health_bar_missing[HEALTH_BAR_MAX];

    int teleport_bubble_count;
    int8_t teleport_bubble_type[TELEPORT_BUBBLE_MAX];
    int teleport_bubble_x[TELEPORT_BUBBLE_MAX];
    int teleport_bubble_y[TELEPORT_BUBBLE_MAX];
    int teleport_bubble_time[TELEPORT_BUBBLE_MAX];
} mudclient;

void mudclient_new(mudclient *mud);
void mudclient_start_application(mudclient *mud, int width, int height,
                                 char *title);

/* TODO events.c */
void mudclient_handle_key_press(mudclient *mud, int key_code);
void mudclient_key_pressed(mudclient *mud, int code, char char_code);
void mudclient_key_released(mudclient *mud, int code);
void mudclient_mouse_moved(mudclient *mud, int x, int y);
void mudclient_mouse_released(mudclient *mud, int x, int y, int button);
void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button);

void mudclient_set_target_fps(mudclient *mud, int fps);
void mudclient_reset_timings(mudclient *mud);
void mudclient_start(mudclient *mud);
void mudclient_stop(mudclient *mud);
void mudclient_draw_string(mudclient *mud, char *string, int font, int x,
                           int y);

/* TODO loader.c */
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
void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1);
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
GameCharacter *mudclient_add_character(mudclient *mud,
                                       GameCharacter **character_server,
                                       GameCharacter **known_characters,
                                       int known_character_count,
                                       int server_index, int x, int y,
                                       int animation, int npc_id);
GameCharacter *mudclient_add_player(mudclient *mud, int server_index, int x,
                                    int y, int animation);
GameCharacter *mudclient_add_npc(mudclient *mud, int server_index, int x,
                                    int y, int animation, int npc_id);
void mudclient_check_connection(mudclient *mud);
int mudclient_is_valid_camera_angle(mudclient *mud, int angle);
void mudclient_handle_game_input(mudclient *mud);
void mudclient_handle_inputs(mudclient *mud);
void mudclient_update_object_animation(mudclient *mud, int object_index,
                                       char *model_name);
void mudclient_draw_character_message(mudclient *mud, GameCharacter *character,
                                      int x, int y, int width);
void mudclient_draw_character_damage(mudclient *mud, GameCharacter *character,
                                     int x, int y, int ty, int width,
                                     int height, int is_npc);
void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int tx, int ty);
void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int tx, int ty);
void mudclient_draw_game(mudclient *mud);
void mudclient_start_game(mudclient *mud);
void mudclient_draw(mudclient *mud);
void mudclient_poll_events(mudclient *mud);
void mudclient_run(mudclient *mud);
void mudclient_sort_friends(mudclient *mud);
void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id);
void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id);
int main(int argc, char **argv);

#endif
