#ifndef _H_OPTIONS
#define _H_OPTIONS

#include <stdio.h>
#include <string.h>

#include "lib/ini.h"

#define OPTIONS_INI_TEMPLATE                                                   \
    ("; IPv4 address with revision 177 compatible protocol support\n"          \
     "server = %s\n"                                                           \
     "port = %d\n"                                                             \
     "; Disable registration and load sounds, P2P landscape and items ( "      \
     "requires\n; restart)\n"                                                  \
     "members = %d\n"                                                          \
     "; Used together to encrypt passwords, Must be represented as "           \
     "hexadecimal string\n; 0-padded to a multiple of eight characters\n"      \
     "rsa_exponent = %s\n"                                                     \
     "rsa_modulus = %s\n"                                                      \
     "; Log out when mouse is idle\n"                                          \
     "idle_logout = %d\n"                                                      \
     "; Remember username on login screen\n"                                   \
     "remember_username = %d\n"                                                \
     "; Remember password on login screen (not secure)\n"                      \
     "remember_password = %d\n\n"                                              \
     "username = %s\n"                                                         \
     "password = %s\n\n"                                                       \
                                                                               \
     "; Scroll panel lists, chatbox, and camera (if zoom enabled) with "       \
     "wheel\n"                                                                 \
     "mouse_wheel = %d\n"                                                      \
     "; Hold down middle click and move mouse to rotate camera (manual "       \
     "mode)\n"                                                                 \
     "middle_click_camera = %d\n"                                              \
     "; Use arrow, page, home keys and mouse wheel (if enabled) to zoom\n"     \
     "zoom_camera = %d\n"                                                      \
     "; Respond to the last private message with tab key\n"                    \
     "tab_respond = %d\n"                                                      \
     "; Use number keys to select options\n"                                   \
     "option_numbers = %d\n"                                                   \
     "; Adds a menu with different directions to face to the minimap "         \
     "compass\n"                                                               \
     "compass_menu = %d\n"                                                     \
     "; Adds right click menus to trades and duels\n"                          \
     "transaction_menus = %d\n"                                                \
     "; Allow inputting item amounts\n"                                        \
     "offer_x = %d\n"                                                          \
     "; Add another button to perform the last offer x amount\n"               \
     "last_offer_x = %d\n\n"                                                   \
                                                                               \
     "; F1 mode - only render every second scanline\n"                         \
     "interlace = %d\n"                                                        \
     "; Display the FPS at the bottom right of the screen\n"                   \
     "display_fps = %d\n"                                                      \
     "; Double the UI size but keep the scene size if window is over double"   \
     "; original\n; size (GL only)\n"                                          \
     "ui_scale = %d\n"                                                         \
     "; Change the field of view. scales with height by default and in \n"     \
     "; software. About 36 degrees on the original height of 346 (GL only)\n"  \
     "field_of_view = %d\n"                                                    \
     "; Show roofs unless inside buildings\n"                                  \
     "show_roofs = %d\n"                                                       \
     "; Format large numbers with commas\n"                                    \
     "number_commas = %d\n"                                                    \
     "; Show the remaining experience until next level in skills tab\n"        \
     "remaining_experience = %d\n"                                             \
     "; Show your total experience in the skills tab\n"                        \
     "total_experience = %d\n"                                                 \
     "; Show a count of inventory items on the UI\n"                           \
     "inventory_count = %d\n"                                                  \
     "; Condenses item amounts with K and M and add their amounts to "         \
     "examine\n"                                                               \
     "condense_item_amounts = %d\n"                                            \
     "; Also draw which item a certificate is associated with\n"               \
     "certificate_items = %d\n"                                                \
     "; Display the warning dialog near the wilderness border\n"               \
     "wilderness_warning = %d\n\n"                                             \
                                                                               \
     "; Add filtering to the bank\n"                                           \
     "bank_search = %d\n"                                                      \
     "; Adds capacity to the bank\n"                                           \
     "bank_capacity = %d\n"                                                    \
     "; Adds total high alchemy value to the bank\n"                           \
     "bank_value = %d\n"                                                       \
     "; Expand bank item grid with client height\n"                            \
     "bank_expand = %d\n"                                                      \
     "; Use a scrollbar instead of bank pages\n"                               \
     "bank_scroll = %d\n"                                                      \
     "; Adds right click menus to bank items\n"                                \
     "bank_menus = %d\n"                                                       \
     "; Shows the inventory along with the bank interface, given enough "      \
     "width\n"                                                                 \
     "bank_inventory = %d\n"                                                   \
     "; Maintain the selected bank slot when items change position\n"          \
     "bank_maintain_slot = %d\n")

#define OPTION_INI_STR(name, option, length)                                   \
    {                                                                          \
        char *str_option = NULL;                                               \
                                                                               \
        if (ini_sget(options_ini, NULL, name, NULL, &str_option)) {            \
            strncpy(option, str_option, length);                               \
        }                                                                      \
    }

#define OPTION_INI_INT(name, option, min, max)                                 \
    {                                                                          \
        int int_option = 0;                                                    \
                                                                               \
        if (ini_sget(options_ini, NULL, name, "%d", &int_option)) {            \
            if (int_option >= min && int_option <= max) {                      \
                option = int_option;                                           \
            }                                                                  \
        }                                                                      \
    }

typedef struct Options {
    /* configurable options: */
    char server[16];
    int port;

    /* disable registration and load sounds, P2P landscape and items (requires
     * restart) */
    int members;

    /* used together to encrypt passwords, must be represented as hexadecimal
     * string 0-padded to a multiple of eight characters */
    char rsa_exponent[512];
    char rsa_modulus[512];

    /* log out when mouse is idle */
    int idle_logout;

    /* save credentials for login screen (not secure) */
    int remember_username;
    int remember_password;

    /* credentials if remember username/password is enabled */
    char username[20];
    char password[20];

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

    /* adds a menu with different directions to face to the minimap compass */
    int compass_menu;

    /* adds right click menus to trades and duels */
    int transaction_menus;

    /* allow inputting item amounts */
    int offer_x;

    /* add another button to perform the last offer x amount */
    int last_offer_x;

    /* F1 mode - only render every second scanline */
    int interlace;

    /* display the FPS at the bottom right of the screen */
    int display_fps;

    /* double the UI size but keep the scene size if window is over double
     * original size (GL only) */
    int ui_scale;

    /* change the field of view. scales with height by default and in software.
     * about 36 degrees on the original height of 346 (GL only) */
    int field_of_view;

    /* show roofs unless inside buildings */
    int show_roofs;

    /* format large numbers with commas */
    int number_commas;

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

    /* display the warning dialog near the wilderness border */
    int wilderness_warning;

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
} Options;

void options_new(Options *options);
void options_load(Options *options);
void options_save(Options *options);

#endif
