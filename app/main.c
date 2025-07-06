#include <cubemx.h>
#include <st_hal.h>

#include <stdbool.h>

static const uint32_t LOW_SPEED_BLINK_TIMEOUT  = 500;
static const uint32_t HIGH_SPEED_BLINK_TIMEOUT = LOW_SPEED_BLINK_TIMEOUT / 4;

bool speed_blink = false;

int main()
{
    MX_Init();

    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        if (speed_blink) {
            HAL_Delay(HIGH_SPEED_BLINK_TIMEOUT);
        }
        else {
            HAL_Delay(LOW_SPEED_BLINK_TIMEOUT);
        }
    }
}
