#pragma once

#include <pin.h>

#include <stdbool.h>

typedef struct LAME_Led {
    LAME_Pin *pin;
    bool      activeLow;
    unsigned  blinkCount;
    unsigned  currentCount;
} LAME_Led;

void LAME_Led_init(LAME_Led *led, LAME_Pin *pin, bool activeLow, unsigned blinkCount);
void LAME_Led_task(LAME_Led *led);
void LAME_Led_setActive(LAME_Led *led, bool activeLow);
void LAME_Led_setBlinkCount(LAME_Led *led, unsigned blinkCount);

