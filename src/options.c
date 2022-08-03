#include "options.h"

void options_new(Options *options) {
    memset(options, 0, sizeof(Options));

    /* presets */
    options->account_management = 1;
    options->anti_macro = 0;
    options->idle_logout = 0;
    options->retry_login_on_disconnect = 0;
    options->show_additional_options = 1;
    options->skip_tutorial = 1;

    /* controls */
    options->off_handle_scroll_drag = 1;
    options->escape_clear = 1;
    options->mouse_wheel = 1;
    options->middle_click_camera = 1;
    options->zoom_camera = 1;
    options->tab_respond = 1;
    options->option_numbers = 1;

    /* display */
    options->display_fps = 0;
    options->number_commas = 1;
    options->show_roofs = 1;
    options->compass_menu = 1;
    options->transaction_menus = 1;
    options->remaining_experience = 1;
    options->total_experience = 1;
    options->inventory_count = 0;
    options->condense_item_amounts = 1;
    options->certificate_items = 1;

    options->offer_x = 1;
    options->last_offer_x = 1;

    options->wilderness_warning = 1;

    /* bank */
    options->bank_unstackble_withdraw = 1;
    options->bank_search = 1;
    options->bank_capacity = 1;
    options->bank_value = 1;
    options->bank_expand = 1;
    options->bank_scroll = 1;
    options->bank_menus = 1;
    options->bank_inventory = 1;
    options->bank_maintain_slot = 1;

    /* gl */
    options->ui_scale = 1;
    options->field_of_view = 360;

    options->thick_walls = 0;
}

void options_save(Options *options) {
    FILE *ini_file = fopen("./settings.ini", "w");

    fprintf(ini_file, OPTIONS_INI_TEMPLATE,
            options->server,                //
            options->port,                  //
            options->members,               //
            options->idle_logout,           //
            options->remember_username,     //
            options->remember_password,     //
                                            //
            options->mouse_wheel,           //
            options->middle_click_camera,   //
            options->zoom_camera,           //
            options->tab_respond,           //
            options->option_numbers,        //
                                            //
            options->interlace,             //
            options->display_fps,           //
            options->ui_scale,              //
            options->number_commas,         //
            options->show_roofs,            //
            options->compass_menu,          // move to controls?
            options->transaction_menus,     // move to controls?
            options->remaining_experience,  //
            options->total_experience,      //
            options->inventory_count,       //
            options->condense_item_amounts, //
            options->certificate_items,     //
            options->offer_x,               // move to controls?
            options->last_offer_x,          // remove from UI?
            options->wilderness_warning,    //
                                            //
            options->bank_search,           //
            options->bank_capacity,         //
            options->bank_value,            //
            options->bank_expand,           //
            options->bank_scroll,           //
            options->bank_menus,            //
            options->bank_inventory,        //
            options->bank_maintain_slot);   //
}

void options_load(Options *options) {}
