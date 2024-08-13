#include "hydra.h"

void mudclient_handle_stdin(mudclient *mud) {
    char buffer[128] = {0};

    int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("read");
            exit(1);
        }
    }

    if (bytes_read <= 0) {
        return;
    }

    /* remove \n */
    buffer[strlen(buffer) - 1] = '\0';

    char *command_split = strtok(buffer, " ");

    if (strcmp(command_split, "login") == 0) {
        char *username = strtok(NULL, " ");
        char *password = strtok(NULL, " ");

        mudclient_login(mud, username, password, 0);
    } else if (strcmp(command_split, "logout") == 0) {
        mudclient_send_logout(mud);
    } else if (strcmp(command_split, "walk") == 0) {
        char *end;

        int x = strtol(strtok(NULL, " "), &end, 10);

        if (*end != '\0') {
            mud_error("error parsing walk. bad x int\n");
            return;
        }

        int y = strtol(strtok(NULL, " "), &end, 10);

        if (*end != '\0') {
            mud_error("error parsing walk. bad y int\n");
            return;
        }

        mudclient_walk_to_action_source(
            mud, mud->local_region_x, mud->local_region_y,
            mud->local_region_x + x, mud->local_region_y + y, 0);
    }
}
