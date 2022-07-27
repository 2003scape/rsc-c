#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

#ifdef WII
#include <asndlib.h>
#include <gccore.h>
#include <ogc/usbmouse.h>
#include <wiikeyboard/keyboard.h>
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

#include "wii/arrow_yuv.h"
#include "wii/rsc_game_yuv.h"
#include "wii/rsc_keyboard_shift_yuv.h"
#include "wii/rsc_keyboard_yuv.h"
#include "wii/rsc_type_yuv.h"

#define GAME_OFFSET_X 64
#define GAME_OFFSET_Y 54
#endif

#ifdef _3DS
#include <3ds.h>

#include "game_background_bgr.h"
#include "game_keyboard_bgr.h"
#include "game_keyboard_shift_bgr.h"
#include "game_top_bgr.h"
#include "game_type_bgr.h"
#endif

#if !defined(WII) && !defined(_3DS)
#include <SDL2/SDL.h>

#ifdef RENDER_GL
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

#include "shader.h"
#endif
#endif

#define SAMPLE_RATE 8000
#define SAMPLE_BUFFER_SIZE 4096
#define BYTES_PER_SAMPLE 2

#define K_LEFT 37
#define K_RIGHT 39
#define K_UP 38
#define K_DOWN 40
#define K_PAGE_UP 33
#define K_PAGE_DOWN 34
#define K_HOME 36
#define K_F1 112
#define K_ENTER 13
#define K_BACKSPACE 8
#define K_ESCAPE 27
#define K_TAB 9
#define K_1 49
#define K_2 50
#define K_3 51
#define K_4 52
#define K_5 53

#ifdef REVISION_177
#define VERSION 177
#else
#define VERSION 204
#endif

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
#define INPUT_DIGITS_LENGTH 14 /* 2,147,483,647m */

#define USERNAME_LENGTH 20
#define PASSWORD_LENGTH 20

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
#define PLAYER_STAT_EQUIPMENT_COUNT 5
#define PROJECTILE_RANGE_MAX 40

/* TODO overhead max? */
#define RECEIVED_MESSAGE_MAX 50
#define ACTION_BUBBLE_MAX 50
#define HEALTH_BAR_MAX 50
#define TELEPORT_BUBBLE_MAX 50

#define INVENTORY_ITEMS_MAX 30
#define MENU_MAX 250
#define PATH_STEPS_MAX 8000 /* TODO seems a bit large */
#define BANK_ITEMS_MAX 256
#define SHOP_ITEMS_MAX 256
#define TRADE_ITEMS_MAX 14
#define DUEL_ITEMS_MAX 8

#define MOUSE_HISTORY_LENGTH 8192

#define MUD_WIDTH (512)
#define MUD_HEIGHT (346)

/* npc IDs */
#define SHIFTY_MAN_ID 24
#define GIANT_BAT_ID 43

/* object IDs */
#define WINDMILL_SAILS_ID 74
#define FIRE_ID 97
#define FIREPLACE_ID 274
#define LIGHTNING_ID 1031
#define FIRE_SPELL_ID 1036
#define SPELL_CHARGE_ID 1147
#define TORCH_ID 51
#define SKULL_TORCH_ID 143
#define CLAW_SPELL_ID 1142

/* item IDs */
#define COINS_ID 10

#define FIRE_RUNE_ID 31
#define FIRE_STAFF_ID 197
#define FIRE_BATTLESTAFF_ID 615
#define ENCHANTED_FIRE_BATTLESTAFF_ID 682

#define WATER_RUNE_ID 32
#define WATER_STAFF_ID 102
#define WATER_BATTLESTAFF_ID 616
#define ENCHANTED_WATER_BATTLESTAFF_ID 683

#define AIR_RUNE_ID 33
#define AIR_STAFF_ID 101
#define AIR_BATTLESTAFF_ID 617
#define ENCHANTED_AIR_BATTLESTAFF_ID 684

#define EARTH_RUNE_ID 34
#define EARTH_STAFF_ID 103
#define EARTH_BATTLESTAFF_ID 618
#define ENCHANTED_EARTH_BATTLESTAFF_ID 685

/* texture IDs */
#define FOUNTATION_ID 17

/* animation order indices */
#define ANIMATION_COUNT 12
#define ANIMATION_INDEX_HEAD 0
#define ANIMATION_INDEX_BODY 1
#define ANIMATION_INDEX_LEGS 2
#define ANIMATION_INDEX_LEFT_HAND 3 /* shields */
#define ANIMATION_INDEX_RIGHT_HAND 4 /* swords */
#define ANIMATION_INDEX_HEAD_OVERLAY 5 /* med helms */
#define ANIMATION_INDEX_BODY_OVERLAY 6 /* chainmail */
#define ANIMATION_INDEX_LEGS_OVERLAY 7 /* skirts */
// TODO what is 8?
#define ANIMATION_INDEX_BOOTS 9
#define ANIMATION_INDEX_NECK 10 /* amulets */
#define ANIMATION_INDEX_CAPE 11

/* skill IDs */
#define SKILL_ATTACK 0
#define SKILL_DEFENSE 1
#define SKILL_STRENGTH 2
#define SKILL_HITS 3
#define SKILL_PRAYER 5
#define SKILL_MAGIC 6

typedef struct mudclient mudclient;

#include "bzip.h"
#include "chat-message.h"
#include "client-opcodes.h"
#include "colours.h"
#include "game-character.h"
#include "game-model.h"
#include "options.h"
#include "packet-handler.h"
#include "packet-stream.h"
#include "panel.h"
#include "scene.h"
#include "server-opcodes.h"
#include "surface.h"
#include "utility.h"
#include "version.h"
#include "world.h"

#include "ui/appearance.h"
#include "ui/bank.h"
#include "ui/combat-style.h"
#include "ui/confirm.h"
#include "ui/duel.h"
#include "ui/login.h"
#include "ui/logout.h"
#include "ui/menu.h"
#include "ui/message-tabs.h"
#include "ui/offer-x.h"
#include "ui/option-menu.h"
#include "ui/server-message.h"
#include "ui/shop.h"
#include "ui/sleep.h"
#include "ui/stats-tab.h"
#include "ui/trade.h"
#include "ui/ui-tabs.h"
#include "ui/welcome.h"
#include "ui/wilderness-warning.h"

extern int test_x;
extern int test_y;
extern float test_z;
extern int test_yaw;
extern int test_colour;
extern int test_fade;
extern float test_depth;
extern GameModel *test_model; // TODO remove

extern char *font_files[];
extern char *animated_models[];
extern char login_screen_status[255];
extern int character_animation_array[8][12];
extern int character_walk_model[4];
extern int character_combat_model_array1[8];
extern int character_combat_model_array2[8];
extern int player_hair_colours[10];
extern int player_top_bottom_colours[15];
extern int player_skin_colours[5];

#if defined(_3DS) || defined(WII)
/* these are doubled for the wii */
#define KEY_WIDTH 23
#define KEY_HEIGHT 22

extern char keyboard_buttons[5][10];
extern char keyboard_shift_buttons[5][10];
extern int keyboard_offsets[];
#endif

#ifdef WII
void draw_background(uint8_t *framebuffer, int full);
void draw_arrow(uint8_t *framebuffer, int mouse_x, int mouse_y);
void draw_rectangle(uint8_t *framebuffer, int x, int y, int width, int height);

extern int wii_mouse_x;
extern int wii_mouse_y;
extern int wii_mouse_button;
#endif

#ifdef _3DS
#define SOC_ALIGN 0x1000
#define SOC_BUFFER_SIZE 0x100000

extern u32 *SOC_buffer;

extern ndspWaveBuf wave_buf[2];
extern u32 *audio_buffer;
extern int fill_block;

void draw_rectangle(uint8_t *framebuffer, int x, int y, int width, int height);
void draw_blue_bar(uint8_t *framebuffer);
#endif

#if !defined(WII) && !defined(_3DS)
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code);
#endif

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

    int keyboard_open;
    int last_keyboard_button;
#endif

#ifdef _3DS
    uint8_t *framebuffer_top;
    uint8_t *framebuffer_bottom;

    int zoom_offset_x;
    int zoom_offset_y;

    int l_down;
    int r_down;
    int touch_down;
    int keyboard_open;

    int sound_position;
    int sound_length;
#endif

#if !defined(WII) && !defined(_3DS)
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *pixel_surface;

    SDL_Window *gl_window;

    uint8_t *logo_pixels;
    SDL_Surface *logo_surface;
#endif

    Options *options;
    int game_width;
    int game_height;

    int8_t key_left;
    int8_t key_right;
    int8_t key_up;
    int8_t key_down;
    int8_t key_page_up;
    int8_t key_page_down;
    int8_t key_home;
    int8_t key_tab;
    int8_t key_1;
    int8_t key_2;
    int8_t key_3;
    int8_t key_4;
    int8_t key_5;

    /* for middle-click camera */
    int8_t middle_button_down;

    /* stores absolute mouse position and initial rotation for middle click
     * camera */
    int origin_mouse_x;
    int origin_rotation;

    /* make it spin */
    int last_mouse_sample_ticks;
    int last_mouse_sample_x;
    int camera_momentum;

    int mouse_scroll_delta;
    int mouse_action_timeout;
    int mouse_x;
    int mouse_y;
    int mouse_button_down;
    int last_mouse_button_down;
    int mouse_button_click;

    /* used for trade screen (holding for longer increases the amount) */
    int mouse_item_count_increment;
    int mouse_button_down_time;

    int mouse_click_x_history[MOUSE_HISTORY_LENGTH];
    int mouse_click_y_history[MOUSE_HISTORY_LENGTH];
    int mouse_click_count;

    /* yellow/red X sprite location and sprite cycle */
    int mouse_click_x_step;
    int mouse_click_x_x;
    int mouse_click_x_y;

    /* loading bar with jagex logo */
    int loading_step;
    int loading_progress_percent;
    char *loading_progess_text;
    int8_t error_loading_data;

    int timings[10];
    int stop_timeout;
    int interlace_timer;
    int fps;
    int target_fps;
    int max_draw_time;
    int thread_sleep;

    /* F1 mode - only render every second scanline */
    int8_t interlace;

    /* used for username boxes */
    char input_text_current[INPUT_TEXT_LENGTH + 1];
    char input_text_final[INPUT_TEXT_LENGTH + 1];

    /* used for private messaging */
    char input_pm_current[INPUT_PM_LENGTH + 1];
    char input_pm_final[INPUT_PM_LENGTH + 1];

    /* used for item amounts */
    char input_digits_current[INPUT_DIGITS_LENGTH + 1];
    int input_digits_final;

    int max_read_tries;
    int world_full_timeout;
    int moderator_level;
    int auto_login_timeout;

    /* ./ui/social-tab.c */
    Panel *panel_social_list;
    int show_dialog_social_input;
    int control_list_social;
    int ui_tab_social_sub_tab;
    int message_index;
    int message_tokens[SOCIAL_LIST_MAX];
    int friend_list_count;
    int64_t friend_list[SOCIAL_LIST_MAX * 2];
    int friend_list_online[SOCIAL_LIST_MAX * 2];
    int ignore_list_count;
    int64_t ignore_list[SOCIAL_LIST_MAX * 2];
    int64_t private_message_target;

    /* ./ui/options-tab.c */
    int8_t settings_camera_auto;
    int8_t settings_block_chat;
    int8_t settings_block_private;
    int8_t settings_block_trade;
    int8_t settings_block_duel;
    int8_t settings_mouse_button_one;
    int8_t settings_sound_disabled;

    int8_t members;
    char *server;
    int port;

    PacketStream *packet_stream;
    int packet_last_read;
    int8_t incoming_packet[PACKET_BUFFER_LENGTH];

    char username[USERNAME_LENGTH + 1];
    char password[PASSWORD_LENGTH + 1];

    /* sprite indexes used for surface sprite drawing */
    int sprite_media;
    int sprite_util;
    int sprite_item;
    int sprite_logo;
    int sprite_projectile;
    int sprite_texture;
    int sprite_texture_world;

    Surface *surface;
    Scene *scene;
    World *world;

    /* amount of entity, action and teleport bubble sprites */
    int scene_sprite_count;

    /* created from cache and copied for each in-game instance */
    GameModel *game_models[GAME_OBJECTS_MAX];

    /* ./ui/login.c */
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

    int login_screen;
    char login_user[USERNAME_LENGTH + 1];
    char login_pass[PASSWORD_LENGTH + 1];
    char *login_prompt;
    char login_user_disp[USERNAME_LENGTH + 3];

#ifdef REVISION_177
    int session_id;
#else
    int64_t session_id;
#endif

    int logged_in;

    /* ./ui/message-tabs.c */
    Panel *panel_message_tabs;
    int control_text_list_all;
    int control_text_list_chat;
    int control_text_list_quest;
    int control_text_list_private;
    int message_tab_selected;
    int message_tab_flash_all;
    int message_tab_flash_history;
    int message_tab_flash_quest;
    int message_tab_flash_private;
    char message_history[MESSAGE_HISTORY_LENGTH][255];
    int message_history_timeout[MESSAGE_HISTORY_LENGTH];

    int login_timer;

#ifndef REVISION_177
    int system_update;
#endif

    /* ./ui/combat-style.c */
    int combat_style;

    int logout_timeout;
    int combat_timeout;

    int object_count;
    GameModel *object_model[OBJECTS_MAX];
    int object_x[OBJECTS_MAX];
    int object_y[OBJECTS_MAX];
    int object_id[OBJECTS_MAX];
    int object_direction[OBJECTS_MAX];
    int object_already_in_menu[OBJECTS_MAX];

    int wall_object_count;
    GameModel *wall_object_model[WALL_OBJECTS_MAX];
    int wall_object_x[WALL_OBJECTS_MAX];
    int wall_object_y[WALL_OBJECTS_MAX];
    int wall_object_id[WALL_OBJECTS_MAX];
    int wall_object_direction[WALL_OBJECTS_MAX];
    int wall_object_already_in_menu[WALL_OBJECTS_MAX];

    int player_server_indexes[PLAYERS_MAX];
    GameCharacter *player_server[PLAYERS_SERVER_MAX];

    int player_count;
    GameCharacter *players[PLAYERS_MAX];

    int known_player_count;
    GameCharacter *known_players[PLAYERS_MAX];

    /* the player we're controlling */
    int local_player_server_index;
    GameCharacter *local_player;

    GameCharacter *npcs_server[NPCS_SERVER_MAX];

    int npc_count;
    GameCharacter *npcs[NPCS_MAX];

    int known_npc_count;
    GameCharacter *known_npcs[NPCS_MAX];

    int ground_item_count;
    int ground_item_x[GROUND_ITEMS_MAX];
    int ground_item_y[GROUND_ITEMS_MAX];
    int ground_item_z[GROUND_ITEMS_MAX];
    int ground_item_id[GROUND_ITEMS_MAX];

    /* ./ui/sleep.c */
    int8_t is_sleeping;
    int sleep_word_delay_timer;
    int sleep_word_delay;
    int fatigue_sleeping;
    char *sleeping_status_text;

    /* fade distant landscape */
    int8_t fog_of_war;

    /* used to keep track of model indexes to swap to in order to simulate
     * movement */
    int object_animation_count;
    int object_animation_cycle;
    int last_object_animation_cycle;
    int torch_animation_cycle;
    int last_torch_animation_cycle;
    int claw_animation_cycle;
    int last_claw_animation_cycle;

    /* (usually) yellow messages above player and NPC heads */
    int received_messages_count;
    int received_message_x[RECEIVED_MESSAGE_MAX];
    int received_message_y[RECEIVED_MESSAGE_MAX];
    int received_message_mid_point[RECEIVED_MESSAGE_MAX];
    int received_message_height[RECEIVED_MESSAGE_MAX];
    char *received_messages[RECEIVED_MESSAGE_MAX];

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

    int8_t is_in_wild;
    int loading_area;
    int plane_width;
    int plane_height;
    int plane_index;
    int plane_multiplier;
    int region_x;
    int region_y;
    int local_region_x;
    int local_region_y;

    int last_height_offset;
    int local_lower_x;
    int local_lower_y;
    int local_upper_x;
    int local_upper_y;

    /* ./ui/wilderness-warning.c */
    int show_ui_wild_warn;

    /* oh dear you are dead */
    int death_screen_timeout;

    /* bubbles with items above players' heads */
    int action_bubble_count;
    int action_bubble_x[ACTION_BUBBLE_MAX];
    int action_bubble_y[ACTION_BUBBLE_MAX];
    int action_bubble_scale[ACTION_BUBBLE_MAX];
    int action_bubble_item[ACTION_BUBBLE_MAX];

    /* green/red health bars displayed above characters' heads in combat */
    int health_bar_count;
    int health_bar_x[HEALTH_BAR_MAX];
    int health_bar_y[HEALTH_BAR_MAX];
    int health_bar_missing[HEALTH_BAR_MAX];

    /* blue/red bubbles used for teleporting and telegrabbing */
    int teleport_bubble_count;
    int8_t teleport_bubble_type[TELEPORT_BUBBLE_MAX];
    int teleport_bubble_x[TELEPORT_BUBBLE_MAX];
    int teleport_bubble_y[TELEPORT_BUBBLE_MAX];
    int teleport_bubble_time[TELEPORT_BUBBLE_MAX];

    /*int show_dialog_report_abuse_step;
    int report_abuse_offence;*/

    /* ./ui/password.c */
    int show_change_password_step;
    char change_password_old[PASSWORD_LENGTH + 1];
    char change_password_new[PASSWORD_LENGTH + 1];

    /* ./ui/ui-tabs.c */
    /* which UI tab is currently hovered over */
    int show_ui_tab;

    /* used to rotate minimap randomly on open for anti-macro */
    int minimap_random_rotation;
    int minimap_random_scale;

    /* ./ui/menu.c */
    int8_t show_right_click_menu;
    int menu_items_count;
    int menu_indices[MENU_MAX];
    int menu_item_x[MENU_MAX];
    int menu_item_y[MENU_MAX];
    char menu_item_text1[MENU_MAX][64];
    char menu_item_text2[MENU_MAX][64];
    int menu_index[MENU_MAX];
    int menu_source_index[MENU_MAX];
    int menu_target_index[MENU_MAX];
    int menu_type[MENU_MAX];
    int menu_width;
    int menu_height;
    int menu_x;
    int menu_y;

    /* ./ui/inventory-tab.c */
    int inventory_items_count;
    int inventory_item_id[INVENTORY_ITEMS_MAX];
    int inventory_item_stack_count[INVENTORY_ITEMS_MAX]; // TODO rename
    int inventory_equipped[INVENTORY_ITEMS_MAX];
    int selected_item_inventory_index;
    char *selected_item_name;

    /* ./ui/stats-tab.c */
    Panel *panel_quests;
    int control_list_quest;
    int8_t *quest_complete;
    int ui_tab_stats_sub_tab;
    int player_skill_current[PLAYER_STAT_COUNT];
    int player_skill_base[PLAYER_STAT_COUNT];
    int player_experience[PLAYER_STAT_COUNT];
    int player_quest_points;
    int stat_fatigue;
    int player_stat_equipment[PLAYER_STAT_EQUIPMENT_COUNT];

    /* ./ui/magic-tab.c */
    Panel *panel_magic;
    int control_list_magic;
    int ui_tab_magic_sub_tab;
    int selected_spell;
    int8_t prayer_on[PRAYER_COUNT];

    /* decompressed archive of all 8-bit 8KHz ulaw samples */
    int8_t *sound_data;

    /* 100 kilobytes of 16-bit linear PCM */
    int16_t pcm_out[1024 * 50];

#ifdef RENDER_GL
    int gl_is_walking;
    int gl_mouse_x;
    int gl_mouse_y;
#endif

    int walk_path_x[PATH_STEPS_MAX];
    int walk_path_y[PATH_STEPS_MAX];

    /* ./ui/appearance.c */
    Panel *panel_appearance;
    int control_appearance_head_left;
    int control_appearance_head_right;
    int control_appearance_hair_left;
    int control_appearance_hair_right;
    int control_appearance_gender_left;
    int control_appearance_gender_right;
    int control_appearance_top_left;
    int control_appearance_top_right;
    int control_appearance_skin_left;
    int control_appearance_skin_right;
    int control_appearance_bottom_left;
    int control_appearance_bottom_right;
    int control_appearance_accept;

    int8_t show_appearance_change;
    int appearance_head_type;
    int appearance_head_gender;
    int appearance_body_type;
    int appearance_hair_colour;
    int appearance_top_colour;
    int appearance_skin_colour;
    int appearance_bottom_colour;

    /* ./ui/option-menu.c */
    int8_t show_option_menu;
    int option_menu_count;
    char option_menu_entry[5][255];

    /* ./ui/welcome.c */
    int8_t show_dialog_welcome;
    int welcome_screen_already_shown;
    int welcome_last_ip;
    int welcome_days_ago;
    int welcome_recovery_set_days;
    int welcome_unread_messages;
    char *welcome_last_ip_string;

    /* ./ui/server-message.c */
    int8_t show_dialog_server_message;
    int server_message_box_top;
    char server_message[1024];

    /* ./ui/bank.c */
    int8_t show_dialog_bank;
    int new_bank_item_count;
    int new_bank_items[BANK_ITEMS_MAX];
    int new_bank_items_count[BANK_ITEMS_MAX];
    int bank_item_count;
    int bank_items[BANK_ITEMS_MAX];
    int bank_items_count[BANK_ITEMS_MAX];
    int bank_items_max;
    int bank_active_page;
    int bank_selected_item_slot;
    int bank_selected_item;
    int bank_offer_type;
    int bank_last_deposit_offer;
    int bank_last_withdraw_offer;
    int bank_scroll_row;
    int bank_last_scroll;
    int8_t bank_handle_dragged;
    int bank_visible_rows;

    /* ./ui/shop.c */
    int8_t show_dialog_shop;
    int shop_items[SHOP_ITEMS_MAX];
    int shop_items_count[SHOP_ITEMS_MAX];
    int shop_items_price[SHOP_ITEMS_MAX];
    int shop_selected_item_index;
    int shop_selected_item_type;
    int shop_buy_price_mod;
    int shop_sell_price_mod;

    /* ./ui/trade.c */
    int8_t show_dialog_trade;
    int trade_item_count;
    int trade_items[TRADE_ITEMS_MAX];
    int trade_items_count[TRADE_ITEMS_MAX];
    int trade_recipient_accepted;
    int trade_accepted;
    char trade_recipient_name[USERNAME_LENGTH + 1];
    int trade_recipient_item_count;
    int trade_recipient_items[TRADE_ITEMS_MAX];
    int trade_recipient_items_count[TRADE_ITEMS_MAX];
    int trade_selected_item;
    int trade_last_offer;
    int trade_offer_type;

    int8_t show_dialog_trade_confirm;
    int64_t trade_recipient_confirm_hash;
    int trade_confirm_item_count;
    int trade_confirm_items[TRADE_ITEMS_MAX];
    int trade_confirm_items_count[TRADE_ITEMS_MAX];
    int trade_recipient_confirm_item_count;
    int trade_recipient_confirm_items[TRADE_ITEMS_MAX];
    int trade_recipient_confirm_items_count[TRADE_ITEMS_MAX];
    int trade_confirm_accepted;

    /* ./ui/duel.c */
    int8_t show_dialog_duel;
    int duel_option_retreat;
    int duel_option_magic;
    int duel_option_prayer;
    int duel_option_weapons;
    int duel_item_count;
    int duel_items[DUEL_ITEMS_MAX];
    int duel_items_count[DUEL_ITEMS_MAX];
    int duel_opponent_accepted;
    int duel_accepted;
    char duel_opponent_name[USERNAME_LENGTH + 1];
    int duel_opponent_item_count;
    int duel_opponent_items[DUEL_ITEMS_MAX];
    int duel_opponent_items_count[DUEL_ITEMS_MAX];

    int show_dialog_duel_confirm;
    int64_t duel_opponent_confirm_hash;
    int duel_confirm_item_count;
    int duel_confirm_items[DUEL_ITEMS_MAX];
    int duel_confirm_items_count[DUEL_ITEMS_MAX];
    int duel_opponent_confirm_item_count;
    int duel_opponent_confirm_items[DUEL_ITEMS_MAX];
    int duel_opponent_confirm_items_count[DUEL_ITEMS_MAX];
    int duel_confirm_accepted;

    /* TODO should we combine the trade/duel item arrays + props? they're never
     * simultaneous */

    /* ./ui/offer-x.c */
    int show_dialog_offer_x;
    int offer_id;
    int offer_max;

    /* ./ui/confirm.c */
    int show_dialog_confirm;
    char *confirm_text_top;
    char *confirm_text_bottom;
    int confirm_type;
} mudclient;

void mudclient_new(mudclient *mud);
void mudclient_resize(mudclient *mud);
void mudclient_start_application(mudclient *mud, char *title);

/* TODO events.c */
void mudclient_handle_key_press(mudclient *mud, int key_code);
void mudclient_key_pressed(mudclient *mud, int code, int char_code);
void mudclient_key_released(mudclient *mud, int code);
void mudclient_handle_mouse_history(mudclient *mud, int x, int y);
void mudclient_mouse_moved(mudclient *mud, int x, int y);
void mudclient_mouse_released(mudclient *mud, int x, int y, int button);
void mudclient_mouse_pressed(mudclient *mud, int x, int y, int button);

void mudclient_set_target_fps(mudclient *mud, int fps);
void mudclient_reset_timings(mudclient *mud);
void mudclient_start(mudclient *mud);
void mudclient_stop(mudclient *mud);

void mudclient_show_loading_progress(mudclient *mud, int percent, char *text);
int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent);
#if !defined(WII) && !defined(_3DS)
SDL_Surface *mudclient_parse_tga(mudclient *mud, int8_t *buffer);
#endif
void mudclient_load_jagex(mudclient *mud);
void mudclient_load_game_config(mudclient *mud);
void mudclient_load_media(mudclient *mud);
#ifdef RENDER_GL
int mudclient_update_entity_sprite_indices(mudclient *mud, int8_t *entity_jag,
                                     int8_t *entity_jag_mem);
#endif
void mudclient_load_entities(mudclient *mud);
void mudclient_load_textures(mudclient *mud);
void mudclient_load_models(mudclient *mud);
void mudclient_load_maps(mudclient *mud);
void mudclient_load_sounds(mudclient *mud);

GameModel *mudclient_create_wall_object(mudclient *mud, int x, int y,
                                        int direction, int id, int count);
int mudclient_load_next_region(mudclient *mud, int lx, int ly);

GameCharacter *mudclient_add_character(mudclient *mud,
                                       GameCharacter **character_server,
                                       GameCharacter **known_characters,
                                       int known_character_count,
                                       int server_index, int x, int y,
                                       int animation, int npc_id);
GameCharacter *mudclient_add_player(mudclient *mud, int server_index, int x,
                                    int y, int animation);
GameCharacter *mudclient_add_npc(mudclient *mud, int server_index, int x, int y,
                                 int animation, int npc_id);

void mudclient_update_bank_items(mudclient *mud);
void mudclient_close_connection(mudclient *mud);
void mudclient_move_character(mudclient *mud, GameCharacter *character);
int mudclient_is_valid_camera_angle(mudclient *mud, int angle);
void mudclient_auto_rotate_camera(mudclient *mud);
void mudclient_handle_camera_zoom(mudclient *mud);
void mudclient_handle_game_input(mudclient *mud);
void mudclient_handle_inputs(mudclient *mud);
void mudclient_update_object_animation(mudclient *mud, int object_index,
                                       char *model_name);
void mudclient_draw_character_message(mudclient *mud, GameCharacter *character,
                                      int x, int y, int width);
void mudclient_draw_character_damage(mudclient *mud, GameCharacter *character,
                                     int x, int y, int ty, int width,
                                     int height, int is_npc, float depth);
void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int skew_x, int ty, float depth_top, float depth_bottom);
void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int skew_x, int ty, float depth_top, float depth_bottom);
void mudclient_draw_blue_bar(mudclient *mud);
void mudclient_draw_ui(mudclient *mud);
void mudclient_draw_overhead(mudclient *mud);
void mudclient_animate_objects(mudclient *mud);
void mudclient_draw_game(mudclient *mud);
void mudclient_reset_game(mudclient *mud);
void mudclient_login(mudclient *mud, char *username, char *password,
                     int reconnecting);
void mudclient_registration_login(mudclient *mud);
void mudclient_register(mudclient *mud, char *username, char *password);
void mudclient_change_password(mudclient *mud, char *old_password,
                               char *new_password);
#ifdef RENDER_GL
void mudclient_update_fov(mudclient *mud);
#endif
void mudclient_start_game(mudclient *mud);
void mudclient_draw(mudclient *mud);
void mudclient_on_resize(mudclient *mud);
void mudclient_poll_events(mudclient *mud);
#ifdef _3DS
void mudclient_flush_audio(mudclient *mud);
#endif
void mudclient_run(mudclient *mud);
void mudclient_remove_ignore(mudclient *mud, int64_t encoded_username);
void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id, float depth);
void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id, float depth_top, float depth_bottom);
int mudclient_is_item_equipped(mudclient *mud, int id);
int mudclient_get_inventory_count(mudclient *mud, int id);
int mudclient_has_inventory_item(mudclient *mud, int id, int minimum);
void mudclient_send_logout(mudclient *mud);
void mudclient_play_sound(mudclient *mud, char *name);
int mudclient_walk_to(mudclient *mud, int start_x, int start_y, int x1, int y1,
                      int x2, int y2, int check_objects, int walk_to_action,
                      int first_step);
void mudclient_walk_to_action_source(mudclient *mud, int start_x, int start_y,
                                     int dest_x, int dest_y, int action);
void mudclient_walk_to_ground_item(mudclient *mud, int start_x, int start_y,
                                   int dest_x, int dest_y, int walk_to_action);
void mudclient_walk_to_wall_object(mudclient *mud, int dest_x, int dest_y,
                                   int direction);
void mudclient_walk_to_object(mudclient *mud, int x, int y, int direction,
                              int id);
int mudclient_is_ui_scaled(mudclient *mud);
void mudclient_format_item_amount(mudclient *mud, int item_amount, char *dest);
int main(int argc, char **argv);
#endif
