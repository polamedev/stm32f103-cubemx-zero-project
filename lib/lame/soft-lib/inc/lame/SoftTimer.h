#pragma once

#include <mSec.h>

#include <stdbool.h>

typedef enum {
    LAME_SoftTimer_ModeStop,
    LAME_SoftTimer_ModePeriodic,
    LAME_SoftTimer_ModeElapsed,
} LAME_SoftTimer_Mode;

typedef struct {
    LAME_mSec           startTime; // Начальное время старта
    LAME_mSec           period;    //< Период истечения таймера
    LAME_SoftTimer_Mode mode;
} LAME_SoftTimer;

bool LAME_SoftTimer_Occur(LAME_SoftTimer *timer);
void LAME_SoftTimer_Start(LAME_SoftTimer *timer, LAME_mSec period, LAME_SoftTimer_Mode mode);

