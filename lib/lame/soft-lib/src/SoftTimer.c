#include <SoftTimer.h>

#include <lame_soft_config.h>

bool LAME_SoftTimer_Occur(LAME_SoftTimer *timer)
{
    if (timer->state == LAME_SoftTimer_StateStop) {
        return false;
    }

    if (timer->state == LAME_SoftTimer_StateElapsedComplete) {
        return true;
    }

    LAME_mSec currentTime = LAME_SoftTimer_Get_mSec();

    /* Таймер прошел. Переполнение должно само устраниться */
    if (currentTime - timer->startTime > timer->period) {
        if (timer->mode == LAME_SoftTimer_ModePeriodic) {
            timer->startTime += timer->period;
        }
        else {
            timer->state = LAME_SoftTimer_StateElapsedComplete;
        }
        return true;
    }

    return false;
}

void LAME_SoftTimer_Init(LAME_SoftTimer *timer, LAME_SoftTimer_Mode mode, LAME_mSec period)
{
    timer->period = period;
    timer->mode   = mode;
    timer->state  = LAME_SoftTimer_StateStop;
}

LAME_SoftTimer_Mode LAME_SoftTimer_GetMode(const LAME_SoftTimer *timer)
{
    return timer->mode;
}

LAME_SoftTimer_State LAME_SoftTimer_GetState(const LAME_SoftTimer *timer)
{
    return timer->state;
}

void LAME_SoftTimer_Start(LAME_SoftTimer *timer)
{
    timer->state = LAME_SoftTimer_StateRun;
}

void LAME_SoftTimer_Stop(LAME_SoftTimer *timer)
{
    timer->state = LAME_SoftTimer_StateStop;
}

void LAME_SoftTimer_SetPeriod(LAME_SoftTimer *timer, LAME_mSec period)
{
    timer->period = period;
}

LAME_mSec LAME_SoftTimer_GetPeriod(const LAME_SoftTimer *timer)
{
    return timer->period;
}
