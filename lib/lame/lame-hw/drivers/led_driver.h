#pragma once

#include "../led.h"

#include <st_hal.h>

typedef struct lame_led_driver {
    GPIO_TypeDef         *GPIOx;
    uint16_t              GPIO_Pin;
    lame_led_active_state active_state;
} lame_led_driver;

void lame_led_driver_init(lame_led_driver *unit);
