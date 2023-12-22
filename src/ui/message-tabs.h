#ifndef _H_MESSAGE_TABS
#define _H_MESSAGE_TABS

#include <string.h>
#include <strings.h>

#define MESSAGE_HISTORY_LENGTH 5
#define MESSAGE_FLASH_TIME 200

typedef enum MessageTab {
    MESSAGE_TAB_ALL = 0,
    MESSAGE_TAB_CHAT = 1,
    MESSAGE_TAB_QUEST = 2,
    MESSAGE_TAB_PRIVATE = 3
} MessageTab;

typedef enum MessageType {
    MESSAGE_TYPE_CHAT = 2,
    MESSAGE_TYPE_GAME = 3,
    MESSAGE_TYPE_BOR = 4,
    MESSAGE_TYPE_QUEST = 5,
    MESSAGE_TYPE_PRIVATE = 6
} MessageType;

#define HBAR_SPRITE_OFFSET 23
#define HBAR_WIDTH 512

#include "../chat-message.h"
#include "../mudclient.h"
#include "../panel.h"

void mudclient_create_message_tabs_panel(mudclient *mud);
void mudclient_draw_chat_message_tabs(mudclient *mud);
void mudclient_draw_chat_message_tabs_panel(mudclient *mud);
void mudclient_send_command_string(mudclient *mud, char *command);
void mudclient_send_chat_message(mudclient *mud, int8_t *encoded,
                                 int encoded_length);
void mudclient_handle_message_tabs_input(mudclient *mud);
void mudclient_decrement_message_flash(mudclient *mud);
void mudclient_show_message(mudclient *mud, char *message, MessageType type);
void mudclient_show_server_message(mudclient *mud, char *message);

#endif
