#include <lame-config.h>

#include <lame/led.h>

#include <stddef.h>

typedef struct LAME_Led_Impl {
    LAME_Pin *pin;
    bool      activeLow;
    unsigned  blinkCount;
    unsigned  currentCount;
} LAME_Led_Impl;

static LAME_Led_Impl leds[LAME_LEDS_QTY];
static size_t        freeLed = 0;

LAME_Led LAME_Led_create(LAME_Pin *pin, bool activeLow, unsigned blinkCount)
{
    if (freeLed == LAME_LEDS_QTY) {
        return NULL;
    }
    LAME_Led led = &leds[freeLed];
    freeLed++;

    led->pin        = pin;
    led->activeLow  = activeLow;
    led->blinkCount = blinkCount;

    LAME_Led_setActive(led, activeLow);

    return led;
}

void LAME_Led_task()
{
    for (size_t i = 0; i < freeLed; ++i) {
    }
    return;
}

void LAME_Led_setActive(LAME_Led led, bool active)
{
    if (led->activeLow) {
        active = !active;
    }

    LAME_Pin_setActive(led->pin, active);
}

void LAME_Led_setBlinkCount(LAME_Led led, unsigned blinkCount)
{
    led->blinkCount = blinkCount;
}
