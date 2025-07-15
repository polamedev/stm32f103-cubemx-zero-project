#include "led_driver.h"

static lame_led_driver *driver;

void lame_led_on(lame_led handle)
{
    if (driver[handle].active_state == LAME_LED_ACTIVE_STATE_HIGH) {
        HAL_GPIO_WritePin(driver[handle].GPIOx, driver[handle].GPIO_Pin, GPIO_PIN_SET);
    }
    else {
        HAL_GPIO_WritePin(driver[handle].GPIOx, driver[handle].GPIO_Pin, GPIO_PIN_RESET);
    }
}
