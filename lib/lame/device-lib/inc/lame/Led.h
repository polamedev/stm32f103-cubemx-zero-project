#pragma once

#include <lame/Pin.h>

#include <stdbool.h>

typedef struct LAME_Led_Impl *LAME_Led;

LAME_Led LAME_Led_create(LAME_Pin pin, bool activeLow, unsigned blinkCount);
void     LAME_Led_task();
void     LAME_Led_setActive(LAME_Led led, bool activeLow);
bool     LAME_Led_GetActive(const LAME_Led led);
void     LAME_Led_setBlinkCount(LAME_Led led, unsigned blinkCount);

