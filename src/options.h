#ifndef _H_OPTIONS
#define _H_OPTIONS

#include <string.h>

#include "lib/ini.h"

#define OPTIONS_INI_NAME "rsc-c.ini"

#define OPTIONS_INI_TEMPLATE                                                   \
    ("; IPv4 address with revision 177 compatible protocol support\n"          \
     "server = %s\n"                                                           \
     "port = %d\n"                                                             \
     "; Disable registration and load sounds, P2P landscape and items "        \
     "(requires restart)\n"                                                    \
     "members = %d\n"                                                          \
     "; Log out when mouse is idle\n"                                          \
     "idle_logout = %d\n"                                                      \
     "; Remember username on login screen\n"                                   \
     "remember_username = %d\n"                                                \
     "; Remember password on login screen (not secure)\n"                      \
     "remember_password = %d\n\n"                                              \
                                                                               \
     "; Scroll panel lists, chatbox, and camera (if zoom enabled) with "       \
     "; wheel\n"                                                               \
     "mouse_wheel = %d\n"                                                      \
     "; Hold down middle click and move mouse to rotate camera (manual "       \
     "mode)\n"                                                                 \
     "middle_click_camera = %d\n"                                              \
     "; Use arrow, page, home keys and mouse wheel (if enabled) to zoom\n"     \
     "zoom_camera = %d\n"                                                      \
     "; Respond to the last private message with tab key\n"                    \
     "tab_respond = %d\n"                                                      \
     "; Use number keys to select options\n"                                   \
     "option_numbers = %d\n\n"                                                 \
                                                                               \
     "; F1 mode - only render every second scanline\n"                         \
     "interlace = %d\n"                                                        \
     "; Display the FPS at the bottom right of the screen\n"                   \
     "display_fps = %d\n"                                                      \
     "; Double the UI size but keep the scene size if window is over double "  \
     "original size (GL only)\n"                                               \
     "ui_scale = %d\n"                                                         \
     "; Format large numbers with commas\n"                                    \
     "number_commas = %d\n"                                                    \
     "; Show roofs unless inside buildings\n"                                  \
     "show_roofs= %d\n")

typedef struct Options {
    /* configurable options: */
    char *server;
    int port;

    /* disable registration and load sounds, P2P landscape and items (requires
     * restart) */
    int members;

    /* log out when mouse is idle */
    int idle_logout;

    /* save credentials for login screen (not secure) */
    int remember_username;
    int remember_password;

    /* scroll panel lists, chatbox, and camera (if zoom enabled) with wheel */
    int mouse_wheel;

    /* hold down middle click and move mouse to rotate camera (manual mode) */
    int middle_click_camera;

    /* use arrow, page, home keys and mouse wheel (if enabled) to zoom */
    int zoom_camera;

    /* respond to the last private message with tab key */
    int tab_respond;

    /* use number keys to select options */
    int option_numbers;

    /* F1 mode - only render every second scanline */
    int interlace;

    /* display the FPS at the bottom right of the screen */
    int display_fps;

    /* double the UI size but keep the scene size if window is over double
     * original size (GL only) */
    int ui_scale;

    /* change the field of view. scales with height by default and in software.
     * about 36 degrees on the original height of 346 */
    int field_of_view;

    /* format large numbers with commas */
    int number_commas;

    /* show roofs unless inside buildings */
    int show_roofs;

    /* adds a menu with different directions to face to the minimap compass */
    int compass_menu;

    /* adds right click menus to trades and duels */
    int transaction_menus;

    /* show the remaining experience until next level in skills tab */
    int remaining_experience;

    /* show your total experience in the skills tab */
    int total_experience;

    /* show a count of inventory items on the UI */
    int inventory_count;

    /* condenses item amounts with K and M and add their amounts to examine */
    int condense_item_amounts;

    /* also draw which item a certificate is associated with */
    int certificate_items;

    /* allow inputting item amounts */
    int offer_x;

    /* add another button to perform the last offer x amount */
    int last_offer_x;

    /* withdraw multiple unstackable items */
    int bank_unstackble_withdraw;

    /* add filtering to the bank */
    int bank_search;

    /* adds capacity to the bank */
    int bank_capacity;

    /* adds total high alchemy value to the bank */
    int bank_value;

    /* TODO: adds a deposit-all from inventory to the bank */
    int bank_deposit_all;

    /* expand bank item grid with client height */
    int bank_expand;

    /* use a scrollbar instead of bank pages */
    int bank_scroll;

    /* adds right click menus to bank items */
    int bank_menus;

    /* shows the inventory along with the bank interface, given enough width */
    int bank_inventory;

    /* maintain the selected bank slot when items change position */
    int bank_maintain_slot;

    /* display the warning dialog near the wilderness border */
    int wilderness_warning;

    /* TODO: censor chat and private messages */
    // int word_filter;

    /* TODO: adjust input and layout for mobile-friendliness */
    // int mobile;

    /* presets without UI configuration in additional options: */
    /* support account registration, password changes and recovery within the
     * client */
    int account_management;

    /* enable randomized camera movement and map rotation/scaling */
    int anti_macro;

    /* retry logins when the server disconnects */
    int retry_login_on_disconnect;

    /* show "Additional options" in the options UI tab */
    int show_additional_options;

    /* show "Skip the tutorial" when the player is on Tutorial Island */
    int skip_tutorial;

    /* escape key clears input */
    int escape_clear;

    /* allow dragging the scrollbar after you've clicked it but are no longer
     * hovering over it */
    int off_handle_scroll_drag;

    /* experimental thick walls support (requires restart) */
    int thick_walls;

    /* credentials if remember username/password is enabled */
    char username[20];
    char password[20];

    /* for login credential encryption on login */
    char rsa_exponent[512];
    char rsa_modulus[512];
} Options;

void options_new(Options *options);
void options_load(Options *options);
void options_save(Options *options);

#endif
