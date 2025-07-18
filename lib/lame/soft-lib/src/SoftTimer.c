#include <lame/SoftTimer.h>

bool LAME_SoftTimer_Occur(LAME_SoftTimer *timer)
{
    if (!timer->isRunning) {
        return false;
    }

    LAME_mSec currentTime = LAME_Get_mSec();

    /* Переполнение должно само устраниться */
    if (currentTime - timer->startTime > timer->period) {
        timer->startTime = timer->period;
        return true;
    }

    return false;
}
