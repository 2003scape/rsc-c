#include "chat-message.h"

int8_t chat_message_encoded[CHAT_MESSAGE_MAX_LENGTH] = {0};
char chat_message_decoded[CHAT_MESSAGE_MAX_LENGTH] = {0};

char *chat_message_decode(int8_t *buffer, int offset, int length) {
    int new_length = 0;
    int left_shift = -1;

    for (int i = 0; i < length; i++) {
        int current = buffer[offset++] & 0xff;
        int map_index = current >> 4 & 0xf;

        if (left_shift == -1) {
            if (map_index < 13) {
                chat_message_decoded[new_length++] =
                    CHAT_MESSAGE_CHAR_MAP[map_index];
            } else {
                left_shift = map_index;
            }
        } else {
            chat_message_decoded[new_length++] =
                CHAT_MESSAGE_CHAR_MAP[((left_shift << 4) + map_index) - 195];

            left_shift = -1;
        }

        map_index = current & 0xf;

        if (left_shift == -1) {
            if (map_index < 13) {
                chat_message_decoded[new_length++] =
                    CHAT_MESSAGE_CHAR_MAP[map_index];
            } else {
                left_shift = map_index;
            }
        } else {
            chat_message_decoded[new_length++] =
                CHAT_MESSAGE_CHAR_MAP[((left_shift << 4) + map_index) - 195];

            left_shift = -1;
        }
    }

    int flag = 1;

    for (int i = 0; i < new_length; i++) {
        char current = chat_message_decoded[i];

        if (i > 4 && current == '@') {
            chat_message_decoded[i] = ' ';
        }

        if (current == '%') {
            chat_message_decoded[i] = ' ';
        }

        if (flag && current >= 'a' && current <= 'z') {
            chat_message_decoded[i] =
                (char)((uint16_t)(chat_message_decoded[i] + 0xffe0));

            flag = 0;
        }

        if (current == '.' || current == '!') {
            flag = 1;
        }
    }

    chat_message_decoded[new_length] = '\0';

    return chat_message_decoded;
}

int chat_message_encode(char *message) {
    int message_length = strlen(message);

    if (message_length > 80) {
        message_length = 80;
    }

    int offset = 0;
    int left_shift = -1;

    for (int i = 0; i < message_length; i++) {
        char current_char = tolower((unsigned char)message[i]);
        int char_map_index = 0;

        for (int j = 0; j < CHAT_MESSAGE_CHAR_MAP_LENGTH; j++) {
            if (current_char == CHAT_MESSAGE_CHAR_MAP[j]) {
                char_map_index = j;
                break;
            }
        }

        if (char_map_index > 12) {
            char_map_index += 195;
        }

        if (left_shift == -1) {
            if (char_map_index < 13) {
                left_shift = char_map_index;
            } else {
                chat_message_encoded[offset++] = char_map_index & 0xff;
            }
        } else if (char_map_index < 13) {
            chat_message_encoded[offset++] =
                ((left_shift << 4) + char_map_index) & 0xff;

            left_shift = -1;
        } else {
            chat_message_encoded[offset++] =
                ((left_shift << 4) + (char_map_index >> 4)) & 0xff;

            left_shift = char_map_index & 0xf;
        }
    }

    if (left_shift != -1) {
        chat_message_encoded[offset++] = (left_shift << 4) & 0xff;
    }

    return offset;
}
