#include <cubemx.h>
#include <st_hal.h>

#include <lame/event.h>

#include <stdbool.h>

static const uint32_t LOW_SPEED_BLINK_TIMEOUT  = 500;
static const uint32_t HIGH_SPEED_BLINK_TIMEOUT = LOW_SPEED_BLINK_TIMEOUT / 4;

LAME_event key_event;

int main()
{
    LAME_event_init(&key_event);

    MX_Init();

    uint32_t timeout = LOW_SPEED_BLINK_TIMEOUT;

    while (1) {
        if (LAME_event_take(&key_event)) {
            timeout = (timeout == LOW_SPEED_BLINK_TIMEOUT) ? HIGH_SPEED_BLINK_TIMEOUT : LOW_SPEED_BLINK_TIMEOUT;
        }

        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        HAL_Delay(timeout);
    }
}
