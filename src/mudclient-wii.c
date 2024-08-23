#include "mudclient.h"

#ifdef WII
void mudclient_poll_events(mudclient *mud) {
    /* handle USB keyboard */
    keyboard_event ke;

    int res = KEYBOARD_GetEvent(&ke);

    if (res) {
        if (ke.type == KEYBOARD_PRESSED) {
            int code = ke.keycode;
            int char_code = ke.symbol;

            if (char_code == 62342) {
                code = K_LEFT;
                char_code = -1;
            } else if (char_code == 62343) {
                code = K_RIGHT;
                char_code = -1;
            } else if (char_code == 62208) {
                code = K_F1;
                char_code = -1;
            } else if (char_code == 106) {
                code = 106;
            } else if (char_code == 101) {
                code = 101;
            } else if (char_code == 48) {
                code = 48;
            } else if (char_code == 13) {
                code = 13;
            } else if (char_code == 8) {
                code = 8;
            }

            mud->last_keyboard_button = code;

            mudclient_key_pressed(mud, code, char_code);
        } else if (ke.type == KEYBOARD_RELEASED) {
            int code = ke.keycode;
            int char_code = ke.symbol;

            mud->last_keyboard_button = -1;

            if (char_code == 62342) {
                code = K_LEFT;
            } else if (char_code == 62343) {
                code = K_RIGHT;
            }

            mudclient_key_released(mud, code);
        }
    }

    WPAD_ReadPending(WPAD_CHAN_ALL, NULL);
    res = WPAD_Probe(0, NULL);

    if (res != WPAD_ERR_NONE) {
        update_wii_mouse(NULL);
    } else {
        WPADData *wiimote_data = WPAD_Data(0);

        if (mud->last_keyboard_button == -1) {
            if (!mud->key_left && wiimote_data->btns_h & WPAD_BUTTON_LEFT) {
                mudclient_key_pressed(mud, K_LEFT, -1);
            } else if (mud->key_left &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_LEFT)) {
                mudclient_key_released(mud, K_LEFT);
            }

            if (!mud->key_right && wiimote_data->btns_h & WPAD_BUTTON_RIGHT) {
                mudclient_key_pressed(mud, K_RIGHT, -1);
            } else if (mud->key_right &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_RIGHT)) {
                mudclient_key_released(mud, K_RIGHT);
            }

            if (!mud->key_up && wiimote_data->btns_h & WPAD_BUTTON_UP) {
                mudclient_key_pressed(mud, K_UP, -1);
            } else if (mud->key_up &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_UP)) {
                mudclient_key_released(mud, K_UP);
            }

            if (!mud->key_down && wiimote_data->btns_h & WPAD_BUTTON_DOWN) {
                mudclient_key_pressed(mud, K_DOWN, -1);
            } else if (mud->key_down &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_DOWN)) {
                mudclient_key_released(mud, K_DOWN);
            }
        }

        /* minus - open keyboard */
        if (mud->last_keyboard_button == -1 &&
            wiimote_data->btns_h & WPAD_BUTTON_MINUS) {
            mud->keyboard_open = !mud->keyboard_open;
            mud->last_keyboard_button = 1;
        } else if (mud->last_keyboard_button == 1 &&
                   !(wiimote_data->btns_h & WPAD_BUTTON_MINUS)) {
            mud->last_keyboard_button = -1;
        }

        /* enter */
        if (mud->last_wii_button != K_ENTER &&
            wiimote_data->btns_h & WPAD_BUTTON_PLUS) {
            mudclient_key_pressed(mud, K_ENTER, K_ENTER);
            mudclient_key_released(mud, K_ENTER);
            mud->last_wii_button = K_ENTER;
        } else if (mud->last_wii_button == K_ENTER &&
                   !(wiimote_data->btns_h & WPAD_BUTTON_PLUS)) {
            mud->last_wii_button = 0;
        }

        if (mud->last_wii_button != K_F1 &&
            wiimote_data->btns_h & WPAD_BUTTON_1) {
            mudclient_key_pressed(mud, K_F1, -1);
            mudclient_key_released(mud, K_F1);
            mud->last_wii_button = K_F1;
        } else if (mud->last_wii_button == K_F1 &&
                   !(wiimote_data->btns_h & WPAD_BUTTON_1)) {
            mud->last_wii_button = 0;
        }

        if (mud->keyboard_open) {
            /* backspace */
            if (mud->last_wii_button != K_BACKSPACE &&
                wiimote_data->btns_h & WPAD_BUTTON_B) {
                mudclient_key_pressed(mud, K_BACKSPACE, K_BACKSPACE);
                mudclient_key_released(mud, K_BACKSPACE);
                mud->last_wii_button = K_BACKSPACE;
            } else if (mud->last_wii_button == K_BACKSPACE &&
                       !(wiimote_data->btns_h & WPAD_BUTTON_B)) {
                mud->last_wii_button = 0;
            }
        }

        update_wii_mouse(wiimote_data);
    }

    /* menu exit */
    if (wii_mouse_y <= 45 && wii_mouse_x >= 234 && wii_mouse_x <= 396 &&
        wii_mouse_button != 0) {
        exit(0);
    }

    if (mud->keyboard_open) {
        mud->last_wii_x = wii_mouse_x;
        mud->last_wii_y = wii_mouse_y;

        int keyboard_x = 52 + 16;
        int keyboard_y = 200 + 16;

        int key_width = KEY_WIDTH * 2;
        int key_height = KEY_HEIGHT * 2;
        int keyboard_width = key_width * 10;
        int keyboard_height = key_height * 5;

        if (mud->last_wii_button != 1 && wii_mouse_button == 1) {
            mud->last_wii_button = 1;

            if (wii_mouse_x >= keyboard_x &&
                wii_mouse_x <= (keyboard_x + (key_width * 2)) &&
                wii_mouse_y >= (keyboard_y + (key_height * 4)) &&
                wii_mouse_y <= (keyboard_y + (key_height * 5))) {
                mud->keyboard_open = mud->keyboard_open == 2 ? 1 : 2;
                return;
            }

            for (int y = 0; y < keyboard_height; y += key_height) {
                int row = y / key_height;
                int offset_x = keyboard_x + (keyboard_offsets[row] * 2);

                for (int x = 0; x < keyboard_width; x += key_width) {
                    if (wii_mouse_x >= (x + offset_x) &&
                        wii_mouse_x <= (x + offset_x + key_width) &&
                        wii_mouse_y >= (y + keyboard_y) &&
                        wii_mouse_y <= (y + keyboard_y + key_height)) {

                        int code = -1;

                        if (mud->keyboard_open == 2) {
                            code = keyboard_shift_buttons[row][(wii_mouse_x -
                                                                offset_x) /
                                                               key_width];
                        } else {
                            code =
                                keyboard_buttons[row][(wii_mouse_x - offset_x) /
                                                      key_width];
                        }

                        mudclient_key_pressed(mud, code, code);
                        mudclient_key_released(mud, code);
                        return;
                    }
                }
            }
        } else if (mud->last_wii_button == 1 && wii_mouse_button != 1) {
            mud->last_wii_button = 0;
        }
    } else {
        int game_x = wii_mouse_x - GAME_OFFSET_X;
        int game_y = wii_mouse_y - GAME_OFFSET_Y;

        if (wii_mouse_x != mud->last_wii_x || wii_mouse_y != mud->last_wii_y) {
            mudclient_mouse_moved(mud, game_x, game_y);
            mud->last_wii_x = wii_mouse_x;
            mud->last_wii_y = wii_mouse_y;
        }

        if (mud->last_wii_button != 1 && wii_mouse_button == 1) {
            mudclient_mouse_pressed(mud, game_x, game_y, 1);
            mud->last_wii_button = 1;
        } else if (mud->last_wii_button == 1 && wii_mouse_button != 1) {
            mudclient_mouse_released(mud, game_x, game_y, 1);
            mud->last_wii_button = 0;
        }

        if (mud->last_wii_button != 3 && wii_mouse_button == 3) {
            mudclient_mouse_pressed(mud, game_x, game_y, 3);
            mud->last_wii_button = 3;
        } else if (mud->last_wii_button == 3 && wii_mouse_button != 3) {
            mudclient_mouse_released(mud, game_x, game_y, 3);
            mud->last_wii_button = 0;
        }
    }
#endif
