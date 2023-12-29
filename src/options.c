#include "options.h"

#ifdef WII
int wii_fat_enabled = 0;
#endif

void options_new(Options *options) {
    memset(options, 0, sizeof(Options));

    /* presets */
    options->account_management = 1;
    options->anti_macro = 0;
    options->retry_login_on_disconnect = 0;
    options->show_additional_options = 1;
    options->skip_tutorial = 1;

    /* experimental */
    options->thick_walls = 0;

    options_set_defaults(options);
    // options_set_vanilla(options);

#ifdef WII
    wii_fat_enabled = fatInitDefault();
#endif
}

void options_set_server(Options *options) {
#if REVISION_177
    /* openrsc preservation */
    strcpy(options->server, "game.openrsc.com"); // 206.251.222.229
    options->port = 43596;

    strcpy(options->rsa_exponent, "00010001");

    strcpy(options->rsa_modulus,
           "87cef754966ecb19806238d9fecf0f421e816976f74f365c86a584e51049794d41f"
           "efbdc5fed3a3ed3b7495ba24262bb7d1dd5d2ff9e306b5bbf5522a2e85b25");

#ifdef EMSCRIPTEN
    options->port = 43496; /* websockets */
#else
    options->port = 43596;
#endif
#else
    strcpy(options->server, "127.0.0.1");
    //strcpy(options->server, "192.168.100.178");
    //strcpy(options->server, "192.168.100.113");
    options->port = 43594;
#endif
}

void options_set_defaults(Options *options) {
    /* server */
    options->members = 1;
    options->registration = 1;
    options->idle_logout = 0;
    options->remember_username = 0;
    options->remember_password = 0;
    options->diversify_npcs = 0;

    options_set_server(options);

#ifdef _WIN32
    strcpy(options->browser_command, "explorer \"%s\"");
#elif __APPLE__
    strcpy(options->browser_command, "open \"%s\"");
#else
    strcpy(options->browser_command, "xdg-open \"%s\"");
#endif

    strcpy(options->wiki_url, "https://classic.runescape.wiki/w/%s");

    /* controls */
    options->off_handle_scroll_drag = 1;
    options->escape_clear = 1;
    options->mouse_wheel = 1;
    options->middle_click_camera = 1;
    options->zoom_camera = 1;
    options->tab_respond = 1;
    options->option_numbers = 1;
    options->compass_menu = 1;
    options->transaction_menus = 1;
    options->offer_x = 1;
    options->last_offer_x = 1;
    options->wiki_lookup = 1;
    options->combat_style_always = 0;
    options->hold_to_buy = 1;

    /* display */
    options->interlace = 0;
    options->display_fps = 0;
    options->number_commas = 1;
    options->show_roofs = 1;
    options->remaining_experience = 1;
    options->total_experience = 1;
    options->experience_drops = 0;
    options->inventory_count = 0;
    options->condense_item_amounts = 1;
    options->certificate_items = 1;
    options->wilderness_warning = 1;
    options->status_bars = 0;
    options->ground_item_models = 1;
    options->distant_animation = 1;
    options->tga_sprites = 0;
    options->show_hover_tooltip = 0;

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
    options->anti_alias = 1;
    options->field_of_view = 360;
}

void options_set_vanilla(Options *options) {
    /* connection */
    options->members = 1;
    options->registration = 0;
    options->idle_logout = 1;
    options->remember_username = 0;
    options->remember_password = 0;
    options->diversify_npcs = 0;

    options_set_server(options);

    /* controls */
    options->off_handle_scroll_drag = 0;
    options->escape_clear = 0;
    options->mouse_wheel = 0;
    options->middle_click_camera = 0;
    options->zoom_camera = 0;
    options->tab_respond = 0;
    options->option_numbers = 0;
    options->compass_menu = 0;
    options->transaction_menus = 0;
    options->offer_x = 0;
    options->last_offer_x = 0;
    options->wiki_lookup = 0;
    options->combat_style_always = 0;
    options->hold_to_buy = 0;

    /* display */
    options->interlace = 0;
    options->display_fps = 0;
    options->number_commas = 0;
    options->show_roofs = 1;
    options->remaining_experience = 0;
    options->total_experience = 0;
    options->experience_drops = 0;
    options->inventory_count = 0;
    options->condense_item_amounts = 0;
    options->certificate_items = 0;
    options->wilderness_warning = 1;
    options->status_bars = 0;
    options->ground_item_models = 0;
    options->distant_animation = 0;
    options->tga_sprites = 0;
    options->show_hover_tooltip = 0;

    /* bank */
    options->bank_unstackble_withdraw = 0;
    options->bank_search = 0;
    options->bank_capacity = 0;
    options->bank_value = 0;
    options->bank_expand = 0;
    options->bank_scroll = 0;
    options->bank_menus = 0;
    options->bank_inventory = 0;
    options->bank_maintain_slot = 0;

    /* gl */
    options->ui_scale = 0;
    options->anti_alias = 0;
    options->field_of_view = 360;
}

void options_save(Options *options) {
#ifdef WII
    if (!wii_fat_enabled) {
        return;
    }
#endif

#ifdef EMSCRIPTEN
    FILE *ini_file = fopen("/options/options.ini", "w");
#else
    FILE *ini_file = fopen("./options.ini", "w");
#endif

    fprintf(ini_file, OPTIONS_INI_TEMPLATE,
            options->server,                //
            options->port,                  //
            options->members,               //
            options->registration,          //
            options->rsa_exponent,          //
            options->rsa_modulus,           //
            options->idle_logout,           //
            options->remember_username,     //
            options->remember_password,     //
            options->username,              //
            options->password,              //
            options->browser_command,       //
            options->diversify_npcs,        //
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
            options->wiki_lookup,           //
            options->combat_style_always,   //
            options->hold_to_buy,           //
                                            //
            options->interlace,             //
            options->display_fps,           //
            options->ui_scale,              //
            options->anti_alias,            //
            options->field_of_view,         //
            options->show_roofs,            //
            options->number_commas,         //
            options->remaining_experience,  //
            options->total_experience,      //
            options->experience_drops,      //
            options->inventory_count,       //
            options->condense_item_amounts, //
            options->certificate_items,     //
            options->wilderness_warning,    //
            options->status_bars,           //
            options->ground_item_models,    //
            options->distant_animation,     //
            options->tga_sprites,           //
            options->show_hover_tooltip,    //
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

    fclose(ini_file);
}

void options_load(Options *options) {
#ifdef EMSCRIPTEN
    ini_t *options_ini = ini_load("/options/options.ini");
#else
    ini_t *options_ini = ini_load("options.ini");
#endif

    if (options_ini == NULL) {
        return;
    }

    /* connection */
    OPTION_INI_STR("server", options->server, 255);
    OPTION_INI_INT("port", options->port, 0, 65535);
    OPTION_INI_INT("members", options->members, 0, 1);
    OPTION_INI_INT("registration", options->members, 0, 1);
    OPTION_INI_STR("rsa_exponent", options->rsa_exponent, 512);
    OPTION_INI_STR("rsa_modulus", options->rsa_modulus, 512);
    OPTION_INI_INT("idle_logout", options->idle_logout, 0, 1);
    OPTION_INI_INT("remember_username", options->remember_username, 0, 1);
    OPTION_INI_INT("remember_password", options->remember_password, 0, 1);
    OPTION_INI_STR("username", options->username, 20);
    OPTION_INI_STR("password", options->password, 20);
    OPTION_INI_STR("browser_command", options->browser_command, 20);
    OPTION_INI_INT("diversify_npcs", options->diversify_npcs, 0, 1);

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
    OPTION_INI_INT("wiki_lookup", options->wiki_lookup, 0, 1);
    OPTION_INI_INT("combat_style_always", options->combat_style_always, 0, 1);
    OPTION_INI_INT("hold_to_buy", options->hold_to_buy, 0, 1);

    /* display */
    OPTION_INI_INT("interlace", options->interlace, 0, 1);
    OPTION_INI_INT("display_fps", options->display_fps, 0, 1);
    OPTION_INI_INT("ui_scale", options->ui_scale, 0, 1);
    OPTION_INI_INT("anti_alias", options->anti_alias, 0, 1);
    OPTION_INI_INT("field_of_view", options->field_of_view, 0, 880);
    OPTION_INI_INT("show_roofs", options->show_roofs, 0, 1);
    OPTION_INI_INT("number_commas", options->number_commas, 0, 1);
    OPTION_INI_INT("remaining_experience", options->remaining_experience, 0, 1);
    OPTION_INI_INT("total_experience", options->total_experience, 0, 1);
    OPTION_INI_INT("experience_drops", options->experience_drops, 0, 1);
    OPTION_INI_INT("inventory_count", options->inventory_count, 0, 1);
    OPTION_INI_INT("condense_item_amounts", options->condense_item_amounts, 0,
                   1);
    OPTION_INI_INT("certificate_items", options->certificate_items, 0, 1);
    OPTION_INI_INT("wilderness_warning", options->wilderness_warning, 0, 1);
    OPTION_INI_INT("status_bars", options->status_bars, 0, 1);
    OPTION_INI_INT("ground_item_models", options->ground_item_models, 0, 1);
    OPTION_INI_INT("distant_animation", options->distant_animation, 0, 1);
    OPTION_INI_INT("tga_sprites", options->tga_sprites, 0, 0);
    OPTION_INI_INT("show_hover_tooltip", options->show_hover_tooltip, 0, 1);

    /* bank */
    OPTION_INI_INT("bank_search", options->bank_search, 0, 1);
    OPTION_INI_INT("bank_capacity", options->bank_capacity, 0, 1);
    OPTION_INI_INT("bank_value", options->bank_value, 0, 1);
    OPTION_INI_INT("bank_expand", options->bank_expand, 0, 1);
    OPTION_INI_INT("bank_scroll", options->bank_scroll, 0, 1);
    OPTION_INI_INT("bank_menus", options->bank_menus, 0, 1);
    OPTION_INI_INT("bank_inventory", options->bank_inventory, 0, 1);
    OPTION_INI_INT("bank_maintain_slot", options->bank_maintain_slot, 0, 1);

    ini_free(options_ini);
}
