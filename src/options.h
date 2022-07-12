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

    /* click the compas to face north */
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

    /* enable randomized camera movement and map rotation/scaling */
    int anti_macro;

    /* log out when the mouse hasn't moved in a while */
    int idle_logout;

    /* display the warning dialog near the wilderness border */
    int wilderness_warning;

    /* allow inputting item amounts */
    int offer_x;
} Options;

void options_new(Options *options);

#endif
