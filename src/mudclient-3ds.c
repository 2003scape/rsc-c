#include "mudclient.h"

#ifdef _3DS
void mudclient_poll_events(mudclient *mud) {
    if (mud->keyboard_open) {
        if (_3ds_keyboard_received_input) {
            mudclient_3ds_handle_keyboard(mud);
        }

        return;
    }

    hidScanInput();

    u32 keys_down = hidKeysDown();

    if (keys_down & KEY_LEFT) {
        mudclient_key_pressed(mud, K_LEFT, -1);
    }

    if (keys_down & KEY_RIGHT) {
        mudclient_key_pressed(mud, K_RIGHT, -1);
    }

    if (keys_down & KEY_UP) {
        mudclient_key_pressed(mud, K_UP, -1);
    }

    if (keys_down & KEY_DOWN) {
        mudclient_key_pressed(mud, K_DOWN, -1);
    }

    if (keys_down & KEY_START && !mud->keyboard_open) {
        mudclient_3ds_open_keyboard(mud);
    }

    if (keys_down & KEY_L && keys_down & KEY_R) {
        mud->_3ds_gyro_down = 1;
        mud->_3ds_r_down = 1;
        mudclient_3ds_draw_top_background(mud);
    } else {
        if (keys_down & KEY_L) {
            mud->_3ds_l_down = 1;
        }

        if (keys_down & KEY_R) {
            mud->_3ds_r_down = !mud->_3ds_r_down;

            if (!mud->_3ds_r_down) {
                mudclient_3ds_draw_top_background(mud);
            }

            if (mud->show_option_menu) {
                mudclient_key_pressed(mud, K_5, K_5);
            }
        }
    }

    if (keys_down & KEY_A) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_1, K_1);
        } else {
            mudclient_key_pressed(mud, K_HOME, -1);
        }
    }

    if (keys_down & KEY_Y) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_4, K_4);
        } else {
            mudclient_key_pressed(mud, K_HOME, -1);
        }
    }

    if (keys_down & KEY_X) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_3, K_3);
        } else {
            mudclient_key_pressed(mud, K_PAGE_UP, -1);
        }
    }

    if (keys_down & KEY_B) {
        if (mud->show_option_menu) {
            mudclient_key_pressed(mud, K_2, K_2);
        } else {
            mudclient_key_pressed(mud, K_PAGE_DOWN, -1);
        }
    }

    u32 keys_up = hidKeysUp();

    if (keys_up & KEY_LEFT) {
        mudclient_key_released(mud, K_LEFT);
    }

    if (keys_up & KEY_RIGHT) {
        mudclient_key_released(mud, K_RIGHT);
    }

    if (keys_up & KEY_UP) {
        mudclient_key_released(mud, K_UP);
    }

    if (keys_up & KEY_DOWN) {
        mudclient_key_released(mud, K_DOWN);
    }

    if (keys_up & KEY_L) {
        mud->_3ds_l_down = 0;
        mud->_3ds_gyro_down = 0;
    }

    if (keys_up & KEY_R) {
        mud->_3ds_gyro_down = 0;
        mudclient_key_released(mud, K_5);
    }

    if (keys_up & KEY_A || keys_up & KEY_Y) {
        mudclient_key_released(mud, K_HOME);
        mudclient_key_released(mud, K_1);
        mudclient_key_released(mud, K_4);
    }

    if (keys_up & KEY_X) {
        mudclient_key_released(mud, K_PAGE_UP);
        mudclient_key_released(mud, K_3);
    }

    if (keys_up & KEY_B) {
        mudclient_key_released(mud, K_PAGE_DOWN);
        mudclient_key_released(mud, K_2);
    }

    if (keys_up & KEY_SELECT) {
        _3ds_toggle_top_screen(mud->_3ds_top_screen_off);
        mud->_3ds_top_screen_off = !mud->_3ds_top_screen_off;
    }

    touchPosition touch = {0};
    hidTouchRead(&touch);

    if (touch.px == 0 && touch.py == 0) {
        if (mud->_3ds_touch_down != 0) {
            mudclient_mouse_released(mud, mud->mouse_x, mud->mouse_y,
                                     mud->_3ds_touch_down);
        }

        mud->_3ds_touch_down = 0;
    } else {
        mudclient_mouse_moved(mud, touch.px, touch.py);

        int mouse_down = mud->_3ds_l_down ? 3 : 1;

        if (mud->_3ds_touch_down == 0) {
            mudclient_mouse_pressed(mud, touch.px, touch.py, mouse_down);
        }

        mud->_3ds_touch_down = mouse_down;
    }

    if (mud->_3ds_gyro_down) {
        angularRate gyro = {0};
        hidGyroRead(&gyro);

        mud->camera_rotation = (mud->camera_rotation + (gyro.y / 150)) & 0xff;

        // accelerometer should do zooming
        /*mud->camera_zoom += gyro.x / -20;

        if (mud->camera_zoom > ZOOM_MAX) {
            mud->camera_zoom = ZOOM_MAX;
        } else if (mud->camera_zoom < ZOOM_MIN) {
            mud->camera_zoom = ZOOM_MIN;
        }*/
    }
}
#endif
