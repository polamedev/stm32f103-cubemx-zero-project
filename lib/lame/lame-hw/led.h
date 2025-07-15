#pragma once


typedef int lame_led;

typedef enum lame_led_active_state {
    LAME_LED_ACTIVE_STATE_HIGH,
    LAME_LED_ACTIVE_STATE_LOW,
} lame_led_active_state;

void lame_led_init(lame_led handle);
void lame_led_on(lame_led handle);
void lame_led_off(lame_led handle);
