#ifndef _H_OPTIONS
#define _H_OPTIONS

#include <string.h>

typedef struct Options {
    /* hold down middle click and move the mouse to rotate the camera (only when
     * camera is type manual) */
    int middle_click_camera;

    /* scroll panel lists and chatbox with the mouse wheel (and camera if zoom
     * enabled) */
    int mouse_wheel;

    /* click the compass to face north */
    int reset_compass;

    /* show roofs unless inside buildings */
    int show_roofs;

    /* use arrow keys (and mouse wheel if enabled) to zoom in and out */
    int zoom_camera;

    /* show the remaining experience until next level in skills tab */
    int remaining_experience;

    /* show your total experience in the skills tab */
    int total_experience;

    /* show a count of inventory items on the UI */
    int inventory_count;

    /* censor chat and private messages */
    int word_filter;

    /* support account registration, password changes and recovery within the
     * client */
    int account_management;

    /* display an "Fps: X" counter at the bottom right of the screen */
    int fps_counter;

    /* retry logins when the server disconnects */
    int retry_login_on_disconnect;

    // TODO
    int mobile;

    /* allow dragging the scrollbar after you've clicked it but are no longer
     * hovering over it */
    int off_handle_scroll_drag;

    /* escape key clears input */
    int escape_clear;

    /* enable randomized camera movement and map rotation/scaling */
    int anti_macro;

    /* log out when the mouse hasn't moved in a while */
    int idle_logout;

    /* display the warning dialog near the wilderness border */
    int wilderness_warning;

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

    /* adds right click menus to trades */
    int trade_menus;

    /* condenses item amounts with K and M and add their amounts to examine */
    int condense_item_amounts;

    /* respond to the last PM with tab */
    int tab_respond;

    /* double the UI size (gl only) */
    int ui_scale;

    /* change the field of view. scales with height by default and in software.
     * about 36 degrees on the original height of 346 */
    int field_of_view;

    /* experimental thick walls support (requires restart) */
    int thick_walls;

    /* show "Additional options" in the options UI tab */
    int show_additional_options;

    /* show "Skip the tutorial" when the player is on Tutorial Island */
    int skip_tutorial;
} Options;

void options_new(Options *options);

#endif
