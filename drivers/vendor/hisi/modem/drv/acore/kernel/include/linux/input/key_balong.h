
#ifndef __KEY_BALONG_H__
#define __KEY_BALONG_H__

typedef enum
{
    INVALID_KEY,
    POWER_KEY,
    MENU_KEY,
    RESET_KEY,
    WIFI_KEY,
    BUTT_KEY
}KEY_ENUM;


void key_int_disable(KEY_ENUM key);


void key_int_enable(KEY_ENUM key);

#endif