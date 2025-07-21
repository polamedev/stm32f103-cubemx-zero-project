#pragma once

#include <mSec.h>

#include <stdbool.h>

typedef enum {
    LAME_SoftTimer_ModePeriodic,
    LAME_SoftTimer_ModeElapsed,
} LAME_SoftTimer_Mode;

typedef enum {
    LAME_SoftTimer_StateStop,
    LAME_SoftTimer_StateRun,
    LAME_SoftTimer_StateElapsedComplete,
} LAME_SoftTimer_State;

typedef struct {
    LAME_mSec            startTime; // Начальное время старта
    LAME_mSec            period;    // Период истечения таймера
    LAME_SoftTimer_Mode  mode;
    LAME_SoftTimer_State state;
} LAME_SoftTimer;

bool                LAME_SoftTimer_Occur(LAME_SoftTimer *timer);
void                LAME_SoftTimer_Init(LAME_SoftTimer *timer, LAME_mSec period, LAME_SoftTimer_Mode mode);
LAME_SoftTimer_Mode LAME_SoftTimer_GetMode(const LAME_SoftTimer *timer);

LAME_SoftTimer_State LAME_SoftTimer_GetState(const LAME_SoftTimer *timer);
void                 LAME_SoftTimer_Start(LAME_SoftTimer *timer);
void                 LAME_SoftTimer_Stop(LAME_SoftTimer *timer);

void      LAME_SoftTimer_SetPeriod(LAME_SoftTimer *timer, LAME_mSec period);
LAME_mSec LAME_SoftTimer_GetPeriod(const LAME_SoftTimer *timer);

