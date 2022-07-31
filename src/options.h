#ifndef _H_OPTIONS
#define _H_OPTIONS

#include <string.h>

typedef struct Options {
    char *server;
    int port;
    int members;
    int interlace;
    int display_fps;

    /* for login screen */
    int remember_username;
    int remember_password;

    /* support account registration, password changes and recovery within the
     * client */
    int account_management;

    /* enable randomized camera movement and map rotation/scaling */
    int anti_macro;

    /* log out when the mouse hasn't moved in a while */
    int idle_logout;

    /* retry logins when the server disconnects */
    int retry_login_on_disconnect;

    /* show "Additional options" in the options UI tab */
    int show_additional_options;

    /* show "Skip the tutorial" when the player is on Tutorial Island */
    int skip_tutorial;

    /* display an "Fps: X" counter at the bottom right of the screen */
    int fps_counter;

    /* format large numbers with commas */
    int number_commas;

    /* show roofs unless inside buildings */
    int show_roofs;

    /* adds a menu with different directions to face to the minimap compass */
    int compass_menu;

    /* adds right click menus to trades */
    int trade_menus;

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

    /* adds a deposit-all from inventory to the bank */
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

    /* allow dragging the scrollbar after you've clicked it but are no longer
     * hovering over it */
    int off_handle_scroll_drag;

    /* escape key clears input */
    int escape_clear;

    /* scroll panel lists and chatbox with the mouse wheel (and camera if zoom
     * enabled) */
    int mouse_wheel;

    /* hold down middle click and move the mouse to rotate the camera (only when
     * camera is type manual) */
    int middle_click_camera;

    /* use arrow keys (and mouse wheel if enabled) to zoom in and out */
    int zoom_camera;

    /* respond to the last PM with tab */
    int tab_respond;

    /* use numbers on keyboard to select game options */
    int option_numbers;

    /* double the UI size (gl only) */
    int ui_scale;

    /* change the field of view. scales with height by default and in software.
     * about 36 degrees on the original height of 346 */
    int field_of_view;

    /* experimental thick walls support (requires restart) */
    int thick_walls;

    /* TODO: censor chat and private messages */
    //int word_filter;

    /* TODO: adjust input and layout for mobile-friendliness */
    //int mobile;

    /* if remember username/password is enabled */
    char username[20];
    char password[20];
} Options;

void options_new(Options *options);

#endif
