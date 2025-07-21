#include <Led.h>

#include <lame_config.h>

#include <lame/Led.h>
#include <lame/SoftTimer.h>

#include <stddef.h>

typedef struct LAME_Led_Impl {
    LAME_Pin       pin;
    bool           activeLow;
    unsigned       blinkCount;
    unsigned       currentCount;
    LAME_SoftTimer timer;
} LAME_Led_Impl;

static LAME_Led_Impl leds[LAME_LEDS_QTY];
static size_t        freeLed = 0;

const LAME_mSec blinkTime = 100;

static void LAME_Led_UnitTask(LAME_Led led)
{
    if (!LAME_SoftTimer_Occur(&led->timer)) {
        return;
    }

    if (led->currentCount % 2 == 0) {
        LAME_Led_setActive(led, true);
    }
    else {
        LAME_Led_setActive(led, false);
    }

    led->currentCount++;

    if (led->currentCount >= led->blinkCount) {
        led->currentCount = 0;
        LAME_SoftTimer_SetPeriod(&led->timer, blinkTime * 5);
    }
    else {
        LAME_SoftTimer_SetPeriod(&led->timer, blinkTime);
    }
}

LAME_Led LAME_Led_create(LAME_Pin pin, bool activeLow, unsigned blinkCount)
{
    if (freeLed == LAME_LEDS_QTY) {
        return NULL;
    }
    LAME_Led led = &leds[freeLed];
    freeLed++;

    led->pin          = pin;
    led->activeLow    = activeLow;
    led->blinkCount   = blinkCount * 2;
    led->currentCount = 0;

    LAME_Led_setActive(led, activeLow);

    LAME_SoftTimer_Init(&led->timer, LAME_SoftTimer_ModePeriodic, blinkTime);
    LAME_SoftTimer_Start(&led->timer);

    return led;
}

void LAME_Led_task()
{
    for (size_t i = 0; i < freeLed; ++i) {
        LAME_Led_UnitTask(&leds[i]);
    }
}

void LAME_Led_setActive(LAME_Led led, bool active)
{
    if (led->activeLow) {
        active = !active;
    }

    LAME_Pin_SetActive(led->pin, active);
}

bool LAME_Led_GetActive(const LAME_Led led)
{
    bool active = LAME_Pin_GetActive(led->pin);
    if (led->activeLow) {
        active = !active;
    }
    return active;
}

void LAME_Led_setBlinkCount(LAME_Led led, unsigned blinkCount)
{
    led->blinkCount = blinkCount;
}
