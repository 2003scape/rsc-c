#include "options.h"

void options_new(Options *options) {
    memset(options, 0, sizeof(Options));

    options->show_roofs = 1;
    options->word_filter = 1;
    options->account_management = 1;
    options->fps_counter = 1;
    options->retry_login_on_disconnect = 1;
}
