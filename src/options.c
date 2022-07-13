#include "options.h"

void options_new(Options *options) {
    memset(options, 0, sizeof(Options));

    options->show_roofs = 1;
    options->word_filter = 1;
    options->account_management = 1;
    options->fps_counter = 0;
    options->retry_login_on_disconnect = 1;
    options->wilderness_warning = 1;

    options->mouse_wheel = 1;
    options->middle_click_camera = 1;
    options->reset_compass = 1;
    options->inventory_count = 1;
    options->anti_macro = 0;
    options->off_handle_scroll_drag = 1;
    //options->offer_x = 1;
    //options->last_offer_x = 1;
}
