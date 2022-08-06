#include "options.h"

void options_new(Options *options) {
    memset(options, 0, sizeof(Options));

    /* connection */
    strcpy(options->server, "127.0.0.1");
    options->port = 43594;

    // strcpy(options->server, "162.198.202.160"); /* openrsc preservation */
    // mud->options->port = 43596;
    // mud->options->port = 43496; /* websockets */

    strcpy(options->rsa_exponent, "00010001");

    strcpy(options->rsa_modulus,
           "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41f"
           "efbdc5fed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25");

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

void options_set_defaults(Options *options) {}

void options_set_vanilla(Options *options) {}

void options_save(Options *options) {
    FILE *ini_file = fopen("./options.ini", "w");

    fprintf(ini_file, OPTIONS_INI_TEMPLATE,
            options->server,                //
            options->port,                  //
            options->members,               //
            options->rsa_exponent,          //
            options->rsa_modulus,           //
            options->idle_logout,           //
            options->remember_username,     //
            options->remember_password,     //
            options->username,              //
            options->password,              //
                                            //
            options->mouse_wheel,           //
            options->middle_click_camera,   //
            options->zoom_camera,           //
            options->tab_respond,           //
            options->option_numbers,        //
            options->compass_menu,          //
            options->transaction_menus,     //
            options->offer_x,               //
            options->last_offer_x,          //
                                            //
            options->interlace,             //
            options->display_fps,           //
            options->ui_scale,              //
            options->field_of_view,         //
            options->show_roofs,            //
            options->number_commas,         //
            options->remaining_experience,  //
            options->total_experience,      //
            options->inventory_count,       //
            options->condense_item_amounts, //
            options->certificate_items,     //
            options->wilderness_warning,    //
                                            //
            options->bank_search,           //
            options->bank_capacity,         //
            options->bank_value,            //
            options->bank_expand,           //
            options->bank_scroll,           //
            options->bank_menus,            //
            options->bank_inventory,        //
            options->bank_maintain_slot     //
    );
}

void options_load(Options *options) {
    ini_t *options_ini = ini_load("options.ini");

    if (options_ini == NULL) {
        return;
    }

    /* connection */
    OPTION_INI_STR("server", options->server, 15);
    OPTION_INI_INT("port", options->port, 0, 1);
    OPTION_INI_INT("members", options->members, 0, 1);
    OPTION_INI_STR("rsa_exponent", options->rsa_exponent, 512);
    OPTION_INI_STR("rsa_modulus", options->rsa_modulus, 512);
    OPTION_INI_INT("idle_logout", options->idle_logout, 0, 1);
    OPTION_INI_INT("remember_username", options->remember_username, 0, 1);
    OPTION_INI_INT("remember_password", options->remember_password, 0, 1);

    /* controls */
    OPTION_INI_INT("mouse_wheel", options->mouse_wheel, 0, 1);
    OPTION_INI_INT("middle_click_camera", options->middle_click_camera, 0, 1);
    OPTION_INI_INT("zoom_camera", options->zoom_camera, 0, 1);
    OPTION_INI_INT("tab_respond", options->tab_respond, 0, 1);
    OPTION_INI_INT("option_numbers", options->option_numbers, 0, 1);
    OPTION_INI_INT("compass_menu", options->compass_menu, 0, 1);
    OPTION_INI_INT("transaction_menus", options->transaction_menus, 0, 1);
    OPTION_INI_INT("offer_x", options->offer_x, 0, 1);
    OPTION_INI_INT("last_offer_x", options->last_offer_x, 0, 1);

    /* display */
    OPTION_INI_INT("interlace", options->interlace, 0, 1);
    OPTION_INI_INT("display_fps", options->display_fps, 0, 1);
    OPTION_INI_INT("ui_scale", options->ui_scale, 0, 1);
    OPTION_INI_INT("field_of_view", options->field_of_view, 0, 1);
    OPTION_INI_INT("show_roofs", options->show_roofs, 0, 1);
    OPTION_INI_INT("number_commas", options->number_commas, 0, 1);
    OPTION_INI_INT("remaining_experience", options->remaining_experience, 0, 1);
    OPTION_INI_INT("total_experience", options->total_experience, 0, 1);
    OPTION_INI_INT("inventory_count", options->inventory_count, 0, 1);
    OPTION_INI_INT("condense_item_amounts", options->condense_item_amounts, 0,
                   1);
    OPTION_INI_INT("certificate_items", options->certificate_items, 0, 1);
    OPTION_INI_INT("wilderness_warning", options->wilderness_warning, 0, 1);

    ini_free(options_ini);
}
