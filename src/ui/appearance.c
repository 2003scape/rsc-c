#include "appearance.h"

struct appearance_buttons {
    int left;
    int right;
};

static struct appearance_buttons
mudclient_create_appearance_box(mudclient *mud, char *type, int x, int y);

static struct appearance_buttons
mudclient_create_appearance_box(mudclient *mud, char *type, int x, int y) {
    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    /* box around type text */
    int box_padding = (is_compact ? 5 : 8);
    int box_height = (box_padding + (is_compact ? 25 : 33));

    panel_add_box_rounded(mud->panel_appearance, x, y, APPEARANCE_BOX_WIDTH,
                          box_height);

    char type_copy[strlen(type) + 1];
    strcpy(type_copy, type);

    char *type_split = strtok(type_copy, "\n");

    char type_1[strlen(type_split) + 1];
    strcpy(type_1, type_split);

    type_split = strtok(NULL, "\n");

    if (type_split == NULL) {
        panel_add_text_centre(mud->panel_appearance, x, y, type_1, FONT_BOLD_12,
                              1);
    } else {
        panel_add_text_centre(mud->panel_appearance, x, y - box_padding, type_1,
                              FONT_BOLD_12, 1);

        char type_2[strlen(type_split) + 1];
        strcpy(type_2, type_split);

        panel_add_text_centre(mud->panel_appearance, x, y + box_padding, type_2,
                              FONT_BOLD_12, 1);
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

    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int x = (is_compact ? MUD_MIN_WIDTH : MUD_VANILLA_WIDTH) / 2;
    int y = 10;

    if (!is_compact) {
        panel_add_text_centre(mud->panel_appearance, x, y,
                              "Please design Your Character", FONT_BOLD_14, 1);

        y += 14;

        panel_add_text_centre(mud->panel_appearance, x - 55, y + 110, "Front",
                              FONT_BOLD_13, 1);

        panel_add_text_centre(mud->panel_appearance, x, y + 110, "Side",
                              FONT_BOLD_13, 1);

        panel_add_text_centre(mud->panel_appearance, x + 55, y + 110, "Back",
                              FONT_BOLD_13, 1);

        y += 145;
    } else {
        y += 102;
    }

    /* box around type text */
    int box_padding = (is_compact ? 5 : 8);
    int box_height = (box_padding + (is_compact ? 25 : 33));
    int box_margin = (box_height + (is_compact ? 3 : 9));

    struct appearance_buttons head_buttons = mudclient_create_appearance_box(
        mud, "Head\nType", x - APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_head_left = head_buttons.left;
    mud->control_appearance_head_right = head_buttons.right;

    struct appearance_buttons hair_buttons = mudclient_create_appearance_box(
        mud, "Hair\nColor", x + APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_hair_left = hair_buttons.left;
    mud->control_appearance_hair_right = hair_buttons.right;

    y += box_margin;

    struct appearance_buttons gender_buttons = mudclient_create_appearance_box(
        mud, "Gender", x - APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_gender_left = gender_buttons.left;
    mud->control_appearance_gender_right = gender_buttons.right;

    struct appearance_buttons top_buttons = mudclient_create_appearance_box(
        mud, "Top\nColor", x + APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_top_left = top_buttons.left;
    mud->control_appearance_top_right = top_buttons.right;

    y += box_margin;

    struct appearance_buttons skin_buttons = mudclient_create_appearance_box(
        mud, "Skin\nColor", x - APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_skin_left = skin_buttons.left;
    mud->control_appearance_skin_right = skin_buttons.right;

    struct appearance_buttons bottom_buttons = mudclient_create_appearance_box(
        mud, "Bottom\nColor", x + APPEARANCE_COLUMN_WIDTH, y);

    mud->control_appearance_bottom_left = bottom_buttons.left;
    mud->control_appearance_bottom_right = bottom_buttons.right;

    y += box_margin - (is_compact ? -1 : 3);

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
                (mud->appearance_head_type - 1 + game_data.animation_count) %
                game_data.animation_count;
        } while ((game_data.animations[mud->appearance_head_type].gender & 3) !=
                     1 ||
                 (game_data.animations[mud->appearance_head_type].gender &
                  (4 * mud->appearance_head_gender)) == 0);
    }

    if (panel_is_clicked(mud->panel_appearance,
                         mud->control_appearance_head_right)) {
        do {
            mud->appearance_head_type =
                (mud->appearance_head_type + 1) % game_data.animation_count;
        } while ((game_data.animations[mud->appearance_head_type].gender & 3) !=
                     1 ||
                 (game_data.animations[mud->appearance_head_type].gender &
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
             (game_data.animations[mud->appearance_head_type].gender & 3) !=
                 1 ||
             (game_data.animations[mud->appearance_head_type].gender &
              (4 * mud->appearance_head_gender)) == 0;
             mud->appearance_head_type =
                 (mud->appearance_head_type + 1) % game_data.animation_count)
            ;

        for (; (game_data.animations[mud->appearance_body_type].gender & 3) !=
                   2 ||
               (game_data.animations[mud->appearance_body_type].gender &
                (4 * mud->appearance_head_gender)) == 0;
             mud->appearance_body_type =
                 (mud->appearance_body_type + 1) % game_data.animation_count)
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

    int is_compact = mud->surface->width < MUD_VANILLA_WIDTH ||
                     mud->surface->height < MUD_VANILLA_HEIGHT;

    int x = mud->surface->width / 2;

    int y = (is_compact ? -7 : 25) +
            (mud->surface->height / 2 -
             (is_compact ? MUD_MIN_HEIGHT : MUD_VANILLA_HEIGHT) / 2);

    surface_draw_sprite_scale_mask(
        mud->surface, x - 32 - 55, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[ANIMATION_INDEX_LEGS].file_id,
        player_top_bottom_colours[mud->appearance_bottom_colour]);

    surface_draw_sprite_transform_mask(
        mud->surface, x - 32 - 55, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[mud->appearance_body_type].file_id,
        player_top_bottom_colours[mud->appearance_top_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_draw_sprite_transform_mask(
        mud->surface, x - 32 - 55, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[mud->appearance_head_type].file_id,
        player_hair_colours[mud->appearance_hair_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_draw_sprite_scale_mask(
        mud->surface, x - 32, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT, game_data.animations[2].file_id + 6,
        player_top_bottom_colours[mud->appearance_bottom_colour]);

    surface_draw_sprite_transform_mask(
        mud->surface, x - 32, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[mud->appearance_body_type].file_id + 6,
        player_top_bottom_colours[mud->appearance_top_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_draw_sprite_transform_mask(
        mud->surface, x - 32, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[mud->appearance_head_type].file_id + 6,
        player_hair_colours[mud->appearance_hair_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_draw_sprite_scale_mask(
        mud->surface, x - 32 + 55, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT, game_data.animations[2].file_id + 12,
        player_top_bottom_colours[mud->appearance_bottom_colour]);

    surface_draw_sprite_transform_mask(
        mud->surface, x - 32 + 55, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[mud->appearance_body_type].file_id + 12,
        player_top_bottom_colours[mud->appearance_top_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    surface_draw_sprite_transform_mask(
        mud->surface, x - 32 + 55, y, APPEARANCE_CHARACTER_WIDTH,
        APPEARANCE_CHARACTER_HEIGHT,
        game_data.animations[mud->appearance_head_type].file_id + 12,
        player_hair_colours[mud->appearance_hair_colour],
        player_skin_colours[mud->appearance_skin_colour], 0, 0);

    if (!mud->options->lowmem) {
        mudclient_draw_blue_bar(mud);
    }

    surface_draw(mud->surface);
}
