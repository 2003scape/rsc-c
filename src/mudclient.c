#include "mudclient.h"

void mudclient_draw_teleport_bubble(mudclient *mud, int x, int y, int width,
                                    int height, int id) {}

void mudclient_draw_item(mudclient *mud, int x, int y, int width, int height,
                         int id) {}

void mudclient_draw_player(mudclient *mud, int x, int y, int width, int height,
                           int id, int tx, int ty) {}

void mudclient_draw_npc(mudclient *mud, int x, int y, int width, int height,
                        int id, int tx, int ty) {}

int main(int argc, char **argv) {
    srand(0);

    init_surface_global();
    init_utility_global();

    /*
    char formatted[21];
    format_auth_string("uSeR3$m$%", 20, formatted);
    printf("%s %d\n", formatted, strlen(formatted));

    char hi[20];
    strcpy(hi, "  sdas hey hi  ");
    strtrim(hi);
    printf("|%s|\n", hi);*/

    /*
    int64_t encoded = encode_username("Farts");
    char decoded[20];
    decode_username(encoded, decoded);
    printf("%s\n", decoded);*/

    char formatted[255];
    format_confirm_amount(2147483544, formatted);
    printf("%s\n", formatted);

    printf("%s\n", CHAR_SET);
}
