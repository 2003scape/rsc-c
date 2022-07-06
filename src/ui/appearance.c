#include "appearance.h"

struct appearance_buttons
mudclient_create_appearance_box(mudclient *mud, char *type, int x, int y) {
    panel_add_box_rounded(mud->panel_appearance, x, y, APPEARANCE_BOX_WIDTH,
                          APPEARANCE_BOX_HEIGHT);

    char type_copy[strlen(type) + 1];
    strcpy(type_copy, type);

    char *type_split = strtok(type_copy, "\n");

    char *type_1 = malloc(strlen(type_split) + 1);
    strcpy(type_1, type_split);

    type_split = strtok(NULL, "\n");

    if (type_split == NULL) {
        panel_add_text_centre(mud->panel_appearance, x, y, type_1, 1, 1);
    } else {
        panel_add_text_centre(mud->panel_appearance, x, y - 8, type_1, 1, 1);

        char *type_2 = malloc(strlen(type_split) + 1);
        strcpy(type_2, type_split);

        panel_add_text_centre(mud->panel_appearance, x, y + 8, type_2, 1, 1);
    }

    struct appearance_buttons buttons = {0};

    buttons.left =
        panel_add_button(mud->panel_appearance, x - 40, y,
                         APPEARANCE_ARROW_SIZE, APPEARANCE_ARROW_SIZE);

    panel_add_sprite(mud->panel_appearance, x - 40, y,
                     panel_base_sprite_start + 7);

    buttons.right =
        panel_add_button(mud->panel_appearance, x + 40, y,
                         APPEARANCE_ARROW_SIZE, APPEARANCE_ARROW_SIZE);

    panel_add_sprite(mud->panel_appearance, x + 40, y,
                     panel_base_sprite_start + 6);

    return buttons;
}

void mudclient_create_appearance_panel(mudclient *mud) {
    mud->panel_appearance = malloc(sizeof(Panel));
    panel_new(mud->panel_appearance, mud->surface, 100);

    int x = 256;

    panel_add_text_centre(mud->panel_appearance, x, 10,
                          "Please design Your Character", 4, 1);

    int y = 24;

    panel_add_text_centre(mud->panel_appearance, x - 55, y + 110, "Front", 3,
                          1);

    panel_add_text_centre(mud->panel_appearance, x, y + 110, "Side", 3, 1);
    panel_add_text_centre(mud->panel_appearance, x + 55, y + 110, "Back", 3, 1);

    y += 145;

    struct appearance_buttons head_buttons = mudclient_create_appearance_box(
        mud, "Head\nType", x - APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_head_left = head_buttons.left;
    mud->control_appearance_head_right = head_buttons.right;

    struct appearance_buttons hair_buttons = mudclient_create_appearance_box(
        mud, "Hair\nColor", x + APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_hair_left = hair_buttons.left;
    mud->control_appearance_hair_right = hair_buttons.right;

    y += 50;

    struct appearance_buttons gender_buttons = mudclient_create_appearance_box(
        mud, "Gender", x - APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_gender_left = gender_buttons.left;
    mud->control_appearance_gender_right = gender_buttons.right;

    struct appearance_buttons top_buttons = mudclient_create_appearance_box(
        mud, "Top\nColor", x + APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_top_left = top_buttons.left;
    mud->control_appearance_top_right = top_buttons.right;

    y += 50;

    struct appearance_buttons skin_buttons = mudclient_create_appearance_box(
        mud, "Skin\nColor", x - APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_skin_left = skin_buttons.left;
    mud->control_appearance_skin_right = skin_buttons.right;

    struct appearance_buttons bottom_buttons = mudclient_create_appearance_box(
        mud, "Bottom\nColor", x + APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_bottom_left = bottom_buttons.left;
    mud->control_appearance_bottom_right = bottom_buttons.right;

    y += 47;

    panel_add_button_background(mud->panel_appearance, x, y,
                                APPEARANCE_ACCEPT_WIDTH,
                                APPEARANCE_ACCEPT_HEIGHT);

    panel_add_text_centre(mud->panel_appearance, x, y, "Accept", 4, 0);

    mud->control_appearance_accept =
        panel_add_button(mud->panel_appearance, x, y, APPEARANCE_ACCEPT_WIDTH,
                         APPEARANCE_ACCEPT_HEIGHT);
}

void mudclient_handle_appearance_panel_input(mudclient *mud) {
    panel_handle_mouse(mud->panel_appearance, mud->mouse_x, mud->mouse_y,
                       mud->last_mouse_button_down, mud->mouse_button_down,
                       mud->mouse_scroll_delta);

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_head_left)) {
        do {
            mud->appearance_head_type =
                (mud->appearance_head_type - 1 + game_data_animation_count) %
                game_data_animation_count;
        } while ((game_data_animation_gender[mud->appearance_head_type] & 3) !=
                     1 ||
                 (game_data_animation_gender[mud->appearance_head_type] &
                  (4 * mud->appearance_head_gender)) == 0);
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_head_right)) {
        do {
            mud->appearance_head_type =
                (mud->appearance_head_type + 1) % game_data_animation_count;
        } while ((game_data_animation_gender[mud->appearance_head_type] & 3) !=
                     1 ||
                 (game_data_animation_gender[mud->appearance_head_type] &
                  (4 * mud->appearance_head_gender)) == 0);
    }

    int hair_colours_length =
        sizeof(player_hair_colours) / sizeof(player_hair_colours[0]);

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_hair_left)) {
        mud->appearance_hair_colour =
            (mud->appearance_hair_colour - 1 + hair_colours_length) %
            hair_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_hair_right)) {
        mud->appearance_hair_colour =
            (mud->appearance_hair_colour + 1) % hair_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_gender_left) ||
        panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_gender_right)) {
        for (mud->appearance_head_gender = 3 - mud->appearance_head_gender;
             (game_data_animation_gender[mud->appearance_head_type] & 3) != 1 ||
             (game_data_animation_gender[mud->appearance_head_type] &
              (4 * mud->appearance_head_gender)) == 0;
             mud->appearance_head_type =
                 (mud->appearance_head_type + 1) % game_data_animation_count)
            ;

        for (;
             (game_data_animation_gender[mud->appearance_body_type] & 3) != 2 ||
             (game_data_animation_gender[mud->appearance_body_type] &
              (4 * mud->appearance_head_gender)) == 0;
             mud->appearance_body_type =
                 (mud->appearance_body_type + 1) % game_data_animation_count)
            ;
    }

    int top_bottom_colours_length = sizeof(player_top_bottom_colours) /
                                    sizeof(player_top_bottom_colours[0]);

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_top_left)) {
        mud->appearance_top_colour =
            (mud->appearance_top_colour - 1 + top_bottom_colours_length) %
            top_bottom_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_top_right)) {
        mud->appearance_top_colour =
            (mud->appearance_top_colour + 1) % top_bottom_colours_length;
    }

    int skin_colours_length =
        sizeof(player_skin_colours) / sizeof(player_skin_colours[0]);

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_skin_left)) {
        mud->appearance_skin_colour =
            (mud->appearance_skin_colour - 1 + skin_colours_length) %
            skin_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_skin_right)) {
        mud->appearance_skin_colour =
            (mud->appearance_skin_colour + 1) % skin_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_bottom_left)) {
        mud->appearance_bottom_colour =
            (mud->appearance_bottom_colour - 1 + top_bottom_colours_length) %
            top_bottom_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_bottom_right)) {
        mud->appearance_bottom_colour =
            (mud->appearance_bottom_colour + 1) % top_bottom_colours_length;
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_accept)) {
        packet_stream_new_packet(mud->packet_stream, CLIENT_APPEARANCE);
        packet_stream_put_byte(mud->packet_stream, mud->appearance_head_gender);
        packet_stream_put_byte(mud->packet_stream, mud->appearance_head_type);
        packet_stream_put_byte(mud->packet_stream, mud->appearance_body_type);
        packet_stream_put_byte(mud->packet_stream, 2);
        packet_stream_put_byte(mud->packet_stream, mud->appearance_hair_colour);
        packet_stream_put_byte(mud->packet_stream, mud->appearance_top_colour);

        packet_stream_put_byte(mud->packet_stream,
                               mud->appearance_bottom_colour);

        packet_stream_put_byte(mud->packet_stream, mud->appearance_skin_colour);
        packet_stream_send_packet(mud->packet_stream);

        surface_black_screen(mud->surface);
        mud->show_appearance_change = 0;
    }
}

void mudclient_draw_appearance_panel(mudclient *mud) {
    mud->surface->interlace = 0;
    surface_black_screen(mud->surface);
    panel_draw_panel(mud->panel_appearance);

    int x = 256;
    int y = 25;

    surface_sprite_clipping_from6(
        mud->surface, x - 32 - 55, y, 64, 102,
        game_data_animation_number[2],
        player_top_bottom_colours[mud->appearance_bottom_colour]);

    surface_sprite_clipping_from9(
        mud->surface, x - 32 - 55, y, 64, 102,
        game_data_animation_number[mud->appearance_body_type],
        player_top_bottom_colours[mud->appearance_top_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_sprite_clipping_from9(
        mud->surface, x - 32 - 55, y, 64, 102,
        game_data_animation_number[mud->appearance_head_type],
        player_hair_colours[mud->appearance_hair_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_sprite_clipping_from6(
        mud->surface, x - 32, y, 64, 102, game_data_animation_number[2] + 6,
        player_top_bottom_colours[mud->appearance_bottom_colour]);

    surface_sprite_clipping_from9(
        mud->surface, x - 32, y, 64, 102,
        game_data_animation_number[mud->appearance_body_type] + 6,
        player_top_bottom_colours[mud->appearance_top_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_sprite_clipping_from9(
        mud->surface, x - 32, y, 64, 102,
        game_data_animation_number[mud->appearance_head_type] + 6,
        player_hair_colours[mud->appearance_hair_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_sprite_clipping_from6(
        mud->surface, x - 32 + 55, y, 64, 102,
        game_data_animation_number[2] + 12,
        player_top_bottom_colours[mud->appearance_bottom_colour]);

    surface_sprite_clipping_from9(
        mud->surface, x - 32 + 55, y, 64, 102,
        game_data_animation_number[mud->appearance_body_type] + 12,
        player_top_bottom_colours[mud->appearance_top_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_sprite_clipping_from9(
        mud->surface, x - 32 + 55, y, 64, 102,
        game_data_animation_number[mud->appearance_head_type] + 12,
        player_hair_colours[mud->appearance_hair_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    /* blue bar */
    surface_draw_sprite_from3(mud->surface, 0, mud->surface->height - 16,
                              mud->sprite_media + 22);

    surface_draw(mud->surface);
}
