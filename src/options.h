#ifndef _H_OPTIONS
#define _H_OPTIONS

typedef struct Options {
    int middle_click_camera;
    int mouse_wheel;
    int reset_compass;
    int zoom_camera;
    int show_roofs;
    int remaining_experience;
    int total_experience;
    int word_filter;
    int account_management;
    int fps_counter;
    int retry_login_on_disconnect;
    int mobile;
} Options;

void options_new(Options *options);

#endif
