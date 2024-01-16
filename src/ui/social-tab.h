#ifndef _H_SOCIAL_TAB
#define _H_SOCIAL_TAB

typedef enum {
    SOCIAL_ADD_FRIEND = 1,
    SOCIAL_MESSAGE_FRIEND,
    SOCIAL_ADD_IGNORE
} SocialInput;

#include "../mudclient.h"

/* width and height of tab window */
#define SOCIAL_WIDTH 196
#define SOCIAL_HEIGHT 182
#define SOCIAL_TAB_HEIGHT 24

/* width of add friend/ignore dialog box */
#define SOCIAL_DIALOG_ADD_WIDTH 300

/* width of message friend dialog box */
#define SOCIAL_DIALOG_MESSAGE_WIDTH (MUD_WIDTH - 12)

/* height of add, ignore and message dialog boxes */
#define SOCIAL_DIALOG_HEIGHT 70

/* width and height of clickable cancel button */
#define SOCIAL_CANCEL_SIZE 40

#ifdef REVISION_177
#define FRIEND_ONLINE 99
#else
#define FRIEND_ONLINE 255
#endif

void mudclient_sort_friends(mudclient *mud);
void mudclient_add_friend(mudclient *mud, char *username);
void mudclient_remove_friend(mudclient *mud, int64_t encoded_username);
void mudclient_add_ignore(mudclient *mud, char *username);
void mudclient_remove_ignore(mudclient *mud, int64_t encoded_username);
void mudclient_send_private_message(mudclient *mud, int64_t username,
                                    int8_t *message, int length);

void mudclient_draw_ui_tab_social(mudclient *mud, int no_menus);
void mudclient_draw_social_input(mudclient *mud);

#endif
