#include "mudclient.h"

int main(int argc, char **argv) {
    printf("hi\n");

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
}
