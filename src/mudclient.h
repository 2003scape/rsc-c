#ifndef _H_MUDCLIENT
#define _H_MUDCLIENT

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define GAME_OFFSET_X 64
#define GAME_OFFSET_Y 54
#elif defined(__SWITCH__)
#include <switch.h>
#elif defined(_3DS)
#include <3ds.h>

#include "game_top_bgr.h"
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>
#endif

#ifdef RENDER_3DS_GL
#include <citro3d.h>
#include <tex3ds.h>

#define DISPLAY_TRANSFER_FLAGS                                                 \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |                     \
     GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |  \
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |                            \
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))
#endif

#if !defined(WII) && !defined(_3DS)
#include <SDL.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define MUD_IS_BIG_ENDIAN
#endif

#ifdef RENDER_GL
#include <GL/glew.h>
#include <GL/glu.h>
#ifndef SDL12
#include <SDL_opengl.h>
#endif

#include "gl/shader.h"
#endif
#endif

#define SAMPLE_RATE 8000
#define SAMPLE_BUFFER_SIZE 4096
#define BYTES_PER_SAMPLE 2
#define PCM_LENGTH (50 * 1024)

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

#define K_FWD_SLASH 47
#define K_ASTERISK 42
#define K_MINUS 45
#define K_PLUS 43
#define K_PERIOD 46

#define K_0 48
#define K_1 49
#define K_2 50
#define K_3 51
#define K_4 52
#define K_5 53
#define K_6 54
#define K_7 55
#define K_8 56
#define K_9 57

#ifdef REVISION_177
#define VERSION 177
#elif !defined(NO_RSA) && !defined(NO_ISAAC)
#define VERSION 203
#else
/*
 * 2003scape server compatiblity. Actual Jagex "204" clients identify
 * as 203.
 */
#define VERSION 204
#endif

#define ZOOM_MIN 450
#define ZOOM_MAX 2250 // old 1250
#define ZOOM_INDOORS 550
#define ZOOM_OUTDOORS 750

#define MAGIC_LOC 128

#define FONT_FILES_LENGTH (sizeof(font_files) / sizeof(font_files[0]))

#define ANIMATED_MODELS_LENGTH 20

/* maximum amount of friends/ignores */
#define SOCIAL_LIST_MAX 100

#define INPUT_TEXT_LENGTH 20
#define INPUT_PM_LENGTH 80
#define INPUT_DIGITS_LENGTH 14 /* 2,147,483,647m */

#define GAME_OBJECTS_MAX 1000
#define WALL_OBJECTS_MAX 500
#define OBJECTS_MAX 1500
#define PLAYERS_SERVER_MAX 2000
#define PLAYERS_MAX 500
#define NPCS_SERVER_MAX 5000
#define NPCS_MAX 500
#define GROUND_ITEMS_MAX 5000
#define PRAYER_COUNT 50
#define PLAYER_SKILL_COUNT 18
#define PLAYER_STAT_EQUIPMENT_COUNT 5
#define PROJECTILE_RANGE_MAX 40

/* TODO overhead max? */
#define RECEIVED_MESSAGE_MAX 50
#define ACTION_BUBBLE_MAX 50
#define HEALTH_BAR_MAX 50
#define MAGIC_BUBBLE_MAX 50
#define OVERWORLD_TEXT_MAX 128

#define INVENTORY_ITEMS_MAX 30
#define MENU_MAX 250
#define PATH_STEPS_MAX 8000
#define BANK_ITEMS_MAX 256
#define SHOP_ITEMS_MAX 256
#define TRADE_ITEMS_MAX 14
#define DUEL_ITEMS_MAX 8

#define MOUSE_HISTORY_LENGTH 8192

#define MUD_VANILLA_WIDTH 512
#define MUD_VANILLA_HEIGHT 346

#define MUD_MIN_WIDTH 320
#define MUD_MIN_HEIGHT 240

#ifdef _3DS
#define MUD_WIDTH 320
#define MUD_HEIGHT 240
#else
#define MUD_WIDTH MUD_VANILLA_WIDTH
#define MUD_HEIGHT MUD_VANILLA_HEIGHT
//#define MUD_WIDTH 320
//#define MUD_HEIGHT 240
#endif

#define MUD_IS_COMPACT (MUD_WIDTH < 512 || MUD_HEIGHT < 346)

/* npc IDs */
#define SHIFTY_MAN_ID 24
#define GIANT_BAT_ID 43

/* object IDs */
#define WINDMILL_SAILS_ID 74
#define FIRE_ID 97
#define FIREPLACE_ID 274
#define ODD_WELL_ID 466
#define LIGHTNING_ID 1031
#define FIRE_SPELL_ID 1036
#define SPELL_CHARGE_ID 1147
#define TORCH_ID 51
#define SKULL_TORCH_ID 143
#define CLAW_SPELL_ID 1142

/* boundary IDs */
#define ODD_LOOKING_WALL_ID 22

/* item IDs */
#define IRON_MACE_ID 0
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
#define FOUNTAIN_ID 17

/* skill IDs */
#define SKILL_ATTACK 0
#define SKILL_DEFENSE 1
#define SKILL_STRENGTH 2
#define SKILL_HITS 3
#define SKILL_PRAYER 5
#define SKILL_MAGIC 6

/* sprite stuff */
#define SPRITE_LIMIT 4000

/* jagex loading screen on startup */
#define LOADING_WIDTH 277
#define LOADING_HEIGHT 20

/* how many tiles away before objects stop animating */
#define OBJECT_ANIMATION_DISTANCE 7

typedef struct mudclient mudclient;

#include "chat-message.h"
#include "client-opcodes.h"
#include "colours.h"
#include "game-character.h"
#include "game-data.h"
#include "game-model.h"
#include "lib/bzip.h"
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

#include "ui/additional-options.h"
#include "ui/appearance.h"
#include "ui/bank.h"
#include "ui/combat-style.h"
#include "ui/confirm.h"
#include "ui/duel.h"
#include "ui/experience-drops.h"
#include "ui/login.h"
#include "ui/logout.h"
#include "ui/lost-connection.h"
#include "ui/menu.h"
#include "ui/message-tabs.h"
#include "ui/offer-x.h"
#include "ui/option-menu.h"
#include "ui/server-message.h"
#include "ui/shop.h"
#include "ui/sleep.h"
#include "ui/social-tab.h"
#include "ui/stats-tab.h"
#include "ui/status-bars.h"
#include "ui/trade.h"
#include "ui/ui-tabs.h"
#include "ui/welcome.h"
#include "ui/wilderness-warning.h"

#include "custom/clarify-herblaw-items.h"
#include "custom/diverse-npcs.h"
#include "custom/item-highlight.h"

#ifdef WII
/* these are doubled for the wii */
#define KEY_WIDTH 23
#define KEY_HEIGHT 22

#define MUD_IS_BIG_ENDIAN

extern char keyboard_buttons[5][10];
extern char keyboard_shift_buttons[5][10];
extern int keyboard_offsets[];

void draw_background(uint8_t *framebuffer, int full);
void draw_arrow(uint8_t *framebuffer, int mouse_x, int mouse_y);
void draw_keyboard(uint8_t *framebuffer, int is_shift);

extern int wii_mouse_x;
extern int wii_mouse_y;
extern int wii_mouse_button;
#elif defined(_3DS)
#define SOC_ALIGN 0x1000
#define SOC_BUFFER_SIZE 0x100000

/* for keyboard thread */
#define STACK_SIZE (4 * 1024)

#define _3DS_KEYBOARD_NORMAL 0
#define _3DS_KEYBOARD_PASSWORD 1
#define _3DS_KEYBOARD_NUMPAD 2

extern u32 *SOC_buffer;

extern ndspWaveBuf wave_buf[2];
extern u32 *audio_buffer;
extern int fill_block;

extern Thread _3ds_keyboard_thread;
extern char _3ds_keyboard_buffer[255];
extern volatile int _3ds_keyboard_received_input;
extern SwkbdButton _3ds_keyboard_button;

extern char _3ds_option_buttons[5];

void _3ds_keyboard_thread_callback(void *arg);
void _3ds_toggle_top_screen(int is_off);

#ifdef RENDER_3DS_GL
void mudclient_3ds_gl_offscreen_frame_start(mudclient *mud);
void mudclient_3ds_gl_frame_start(mudclient *mud, int clear);
void mudclient_3ds_gl_frame_end();
#endif
#else
#ifdef SDL12
void get_sdl_keycodes(SDL_keysym *keysym, char *char_code, int *code);
#else
void get_sdl_keycodes(SDL_Keysym *keysym, char *char_code, int *code);
#endif
#endif

extern int mudclient_touch_start;

extern int mudclient_horizontal_drag;
extern int mudclient_vertical_drag;

extern int mudclient_touch_start_x;
extern int mudclient_touch_start_y;

extern double mudclient_pinch_distance;

extern int64_t mudclient_finger_1_id;
extern int mudclient_finger_1_x;
extern int mudclient_finger_1_y;
extern int mudclient_finger_1_down;

extern int64_t mudclient_finger_2_id;
extern int mudclient_finger_2_x;
extern int mudclient_finger_2_y;
extern int mudclient_finger_2_down;


// TODO this was moved
extern const char *font_files[];
extern const char *animated_models[];
extern char login_screen_status[255];

/*
 * most walls are created by world.c and are non-interactive,
 * but those that are interactive or can change need to be
 * streamed from the server and are stored here.
 */
struct ServerBoundary {
    GameModel *model;
    uint16_t x;
    uint16_t y;
    uint16_t id;
    uint8_t direction;
    uint8_t already_in_menu;
};

struct ItemSpawn {
    GameModel *model; /* only used when 3D items enabled */
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t id;
    uint8_t already_in_menu;
};

struct Scenery {
    uint16_t x;
    uint16_t y;
    uint16_t id;
    uint8_t direction;
    GameModel *model;
    uint8_t already_in_menu;
};

struct MagicBubble {
    uint16_t x;
    uint16_t y;
    uint8_t type;
    uint8_t time;
};

struct ActionBubble {
    uint16_t x;
    uint16_t y;
    uint16_t scale;
    uint16_t item;
};

struct HealthBar {
    uint16_t x;
    uint16_t y;
    uint8_t missing;
};

struct OverworldText {
    char *text;
    uint16_t x;
    uint16_t y;
    uint32_t colour;
};

struct mudclient {
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

    int8_t keyboard_open;
    int last_keyboard_button;
#elif defined(_3DS)
    uint8_t *_3ds_framebuffer_top;
    uint8_t *_3ds_framebuffer_bottom;

    int8_t _3ds_l_down;
    int8_t _3ds_r_down;
    int8_t _3ds_touch_down;
    int8_t keyboard_open;
    int8_t _3ds_gyro_down;
    int8_t _3ds_gyro_start;
    int8_t _3ds_top_screen_off;

    int _3ds_sound_position;
    int _3ds_sound_length;

#ifdef RENDER_3DS_GL
    C3D_RenderTarget *_3ds_gl_render_target;
    C3D_RenderTarget *_3ds_gl_offscreen_render_target;
#endif
#else
#ifndef SDL12
    SDL_Window *window;
#endif

    SDL_Surface *screen;
    SDL_Surface *pixel_surface;

#if defined(RENDER_GL) && !defined(SDL12)
    SDL_Window *gl_window;
#endif

    SDL_Cursor *default_cursor;
    SDL_Cursor *hand_cursor;
    int is_hand_cursor;
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
    int fps;
    int target_fps;

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
    int auto_login_attempts;

    /* ./ui/social-tab.c */
    Panel *panel_social_list;
    SocialInput show_dialog_social_input;
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
    GameModel **item_models;

    /* ./ui/login.c */
    Panel *panel_login_welcome;
    Panel *panel_login_new_user;
    Panel *panel_login_existing_user;
    int control_welcome_new_user;
    int control_welcome_existing_user;
    int control_welcome_options;
    int refer_id;
    int control_login_new_ok;
    int control_register_status;
    int control_register_status_bottom;
    int control_register_user;
    int control_register_password;
    int control_register_confirm_password;
    int control_register_checkbox;
    int control_register_submit;
    int control_register_cancel;
    int control_login_status;
    int control_login_status_bottom;
    int control_login_username;
    int control_login_password;
    int control_login_ok;
    int control_login_cancel;
    int control_login_recover;

    LOGIN_STAGE login_screen;
    char login_username[USERNAME_LENGTH + 1];
    char login_pass[PASSWORD_LENGTH + 1];
    char *login_prompt;
    char login_username_display[USERNAME_LENGTH + 3];

#ifdef REVISION_177
    int session_id;
#else
    int64_t session_id;
#endif

    int8_t logged_in;

    /* ./ui/message-tabs.c */
    Panel *panel_message_tabs;
    int control_text_list_all;
    int control_text_list_chat;
    int control_text_list_quest;
    int control_text_list_private;
    MessageTab message_tab_selected;
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
    struct Scenery objects[OBJECTS_MAX];

    int wall_object_count;
    struct ServerBoundary wall_objects[WALL_OBJECTS_MAX];

    int player_server_indexes[PLAYERS_MAX];
    GameCharacter *player_server[PLAYERS_SERVER_MAX];

    int player_count;
    GameCharacter *players[PLAYERS_MAX];

    int known_player_count;
    GameCharacter *known_players[PLAYERS_MAX];

    /* the player we're controlling */
    int local_player_server_index;
    GameCharacter *local_player;
    GameCharacter *combat_target;

    GameCharacter *npcs_server[NPCS_SERVER_MAX];

    int npc_count;
    GameCharacter *npcs[NPCS_MAX];

    int known_npc_count;
    GameCharacter *known_npcs[NPCS_MAX];

    int ground_item_count;
    struct ItemSpawn ground_items[GROUND_ITEMS_MAX];

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

    int8_t is_in_wilderness;
    int loading_area;
    int plane_width;
    int plane_height;
    int plane_index;
    int plane_multiplier;
    int region_x;
    int region_y;
    int local_region_x;
    int local_region_y;

    int last_plane_index;
    int local_lower_x;
    int local_lower_y;
    int local_upper_x;
    int local_upper_y;

    /* ./ui/wilderness-warning.c */
    int show_wilderness_warning;

    /* oh dear you are dead */
    int death_screen_timeout;

    /* bubbles with items above players' heads */
    int action_bubble_count;
    struct ActionBubble action_bubbles[ACTION_BUBBLE_MAX];

    /* green/red health bars displayed above characters' heads in combat */
    int health_bar_count;
    struct HealthBar health_bars[HEALTH_BAR_MAX];

    /* blue/red bubbles used for teleporting and telegrabbing */
    int magic_bubble_count;
    struct MagicBubble magic_bubbles[MAGIC_BUBBLE_MAX];

    int overworld_text_count;
    struct OverworldText overworld_text[OVERWORLD_TEXT_MAX];

    /*int8_t show_dialog_report_abuse_step;
    int report_abuse_offence;*/

    /* ./ui/password.c */
    int show_change_password_step;
    char change_password_old[PASSWORD_LENGTH + 1];
    char change_password_new[PASSWORD_LENGTH + 1];

    /* ./ui/ui-tabs.c */
    /* which UI tab is currently hovered over */
    int show_ui_tab;

    /* boundaries for when to close a tab UI */
    int ui_tab_min_x;
    int ui_tab_max_x;
    int ui_tab_min_y;
    int ui_tab_max_y;

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
    MenuType menu_type[MENU_MAX];
    int menu_width;
    int menu_height;
    int menu_x;
    int menu_y;
    char menu_wiki_page[MENU_MAX][192];

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
    int player_skill_current[PLAYER_SKILL_COUNT];
    int player_skill_base[PLAYER_SKILL_COUNT];
    int player_experience[PLAYER_SKILL_COUNT];
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
    int16_t pcm_out[PCM_LENGTH];

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    int gl_is_walking;
#endif

#ifdef RENDER_GL
    int gl_mouse_x;
    int gl_mouse_y;

    int gl_last_swap;
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
    char server_message[4096];

    /* extra page for compact mode */
    int server_message_page;
    char server_message_next[4096];

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

    /* ./ui/transaction.c */
    int transaction_item_count;
    int transaction_items[TRADE_ITEMS_MAX];
    int transaction_items_count[TRADE_ITEMS_MAX];
    int transaction_recipient_accepted;
    int transaction_accepted;
    char transaction_recipient_name[USERNAME_LENGTH + 1];
    int transaction_recipient_item_count;
    int transaction_recipient_items[TRADE_ITEMS_MAX];
    int transaction_recipient_items_count[TRADE_ITEMS_MAX];
    int transaction_selected_item;
    int transaction_last_offer;
    int transaction_offer_type;
    int transaction_tab; /* used for compact mode */

    int64_t transaction_recipient_confirm_name;
    int transaction_confirm_item_count;
    int transaction_confirm_items[TRADE_ITEMS_MAX];
    int transaction_confirm_items_count[TRADE_ITEMS_MAX];
    int transaction_recipient_confirm_item_count;
    int transaction_recipient_confirm_items[TRADE_ITEMS_MAX];
    int transaction_recipient_confirm_items_count[TRADE_ITEMS_MAX];
    int transaction_confirm_accepted;

    /* ./ui/trade.c */
    int8_t show_dialog_trade;
    int8_t show_dialog_trade_confirm;

    /* ./ui/duel.c */
    int8_t show_dialog_duel;
    int8_t show_dialog_duel_confirm;

    int8_t duel_option_retreat;
    int8_t duel_option_magic;
    int8_t duel_option_prayer;
    int8_t duel_option_weapons;

    /* ./ui/offer-x.c */
    int8_t show_dialog_offer_x;
    int offer_id;
    int offer_max;

    /* ./ui/confirm.c */
    int8_t show_dialog_confirm;
    char *confirm_text_top;
    char *confirm_text_bottom;
    CONFIRM_TYPE confirm_type;

    /* ./ui/additional-options.c */
    int show_additional_options;
    int options_tab;

    Panel *panel_connection_options;
    void *connection_options[50];
    int connection_option_types[50];

    Panel *panel_control_options;
    void *control_options[50];
    int control_option_types[50];

    Panel *panel_display_options;
    void *display_options[50];
    int display_option_types[50];

    Panel *panel_bank_options;
    void *bank_options[50];
    int bank_option_types[50];

    /* ./ui/experience-drops.c */
    int experience_drop_skill[50];
    int experience_drop_amount[50];
    float experience_drop_y[50];
    float experience_drop_speed[50];
    int experience_drop_count;

    /* wiki */
    int selected_wiki;
};

void mudclient_new(mudclient *mud);
void mudclient_resize(mudclient *mud);
void mudclient_start_application(mudclient *mud, char *title);

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

void mudclient_draw_loading_progress(mudclient *mud, int percent, char *text);
int8_t *mudclient_read_data_file(mudclient *mud, char *file, char *description,
                                 int percent);
void mudclient_load_jagex_tga_sprite(mudclient *mud, int8_t *buffer);
void mudclient_load_jagex(mudclient *mud);
void mudclient_load_game_config(mudclient *mud);
void mudclient_load_media(mudclient *mud);
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
void mudclient_lost_connection(mudclient *mud);
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
int mudclient_should_chop_head(mudclient *mud, GameCharacter *character,
                               ANIMATION_INDEX animation_index);
void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int skew_x, int ty, float depth_top,
                           float depth_bottom);
void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int skew_x, int ty, float depth_top,
                        float depth_bottom);
void mudclient_draw_blue_bar(mudclient *mud);
int mudclient_is_in_combat(mudclient *mud);
GameCharacter *mudclient_get_opponent(mudclient *mud);
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
#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
void mudclient_update_fov(mudclient *mud);
#endif
void mudclient_start_game(mudclient *mud);
void mudclient_draw(mudclient *mud);
#ifdef SDL12
void mudclient_sdl1_on_resize(mudclient *mud,int width, int height);
#endif
void mudclient_on_resize(mudclient *mud);
void mudclient_poll_events(mudclient *mud);
int mudclient_is_touch(mudclient *mud);
void mudclient_trigger_keyboard(mudclient *mud, char *text, int is_password,
                                int x, int y, int width, int height, int font,
                                int is_centred);
#ifdef _3DS
void mudclient_3ds_flush_audio(mudclient *mud);
void mudclient_3ds_open_keyboard(mudclient *mud);
void mudclient_3ds_handle_keyboard(mudclient *mud);
void mudclient_3ds_draw_top_background(mudclient *mud);
#endif
void mudclient_run(mudclient *mud);
void mudclient_remove_ignore(mudclient *mud, int64_t encoded_username);
void mudclient_draw_magic_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id, float depth);
void mudclient_draw_ground_item(mudclient *mud, int x, int y, int width,
                                int height, int id, float depth_top,
                                float depth_bottom);
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
void mudclient_format_number_commas(mudclient *mud, int number, char *dest);
void mudclient_format_item_amount(mudclient *mud, int item_amount, char *dest);
int mudclient_get_wilderness_depth(mudclient *mud);
void mudclient_draw_item(mudclient *mud, int x, int y, int slot_width,
                         int slot_height, int item_id);
int main(int argc, char **argv);
#endif
#ifdef EMSCRIPTEN
void browser_mouse_moved(int x, int y);
void browser_key_pressed(int code, int char_code);
#endif
