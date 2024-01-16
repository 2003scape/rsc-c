#ifndef _H_CHAT_MESSAGE
#define _H_CHAT_MESSAGE

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHAT_MESSAGE_MAX_LENGTH 100
#define CHAT_MESSAGE_MAX_INPUT_LENGTH 80

#define CHAT_MESSAGE_CHAR_MAP                                                  \
    " etaoihnsrdlumwcyfgpbvkxjqz0123456789 !?.,:;()-&*\\'@#+= $%\"[]`"
//    "` etaoihnsrdlumwcyfgpbvkxjqz0123456789 !?.,:;()-&*'@#+=£$%\"[]`"

#define CHAT_MESSAGE_CHAR_MAP_LENGTH 62

extern int8_t chat_message_encoded[CHAT_MESSAGE_MAX_LENGTH];
extern char chat_message_decoded[CHAT_MESSAGE_MAX_LENGTH];

char *chat_message_decode(int8_t *buffer, int offset, int length);
int chat_message_encode(char *message);

#endif
