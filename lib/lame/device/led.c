#include "led.h"

void LAME_Led_init(LAME_Led *led, LAME_Pin *pin, bool activeLow, unsigned blinkCount)
{
    led->pin = pin;
    led->activeLow = activeLow;
    led->blinkCount = blinkCount;

    LAME_Led_setActive(led, activeLow);
}

void LAME_Led_task(LAME_Led *led)
{
    if (led->blinkCount == 0) {
        return;
    }

    
}

void LAME_Led_setActive(LAME_Led *led, bool active)
{
    if (led->activeLow) {
        active = !active;
    }

    LAME_Pin_setActive(led->pin, active);
}

void LAME_Led_setBlinkCount(LAME_Led *led, unsigned blinkCount)
{
    led->blinkCount = blinkCount;
}
