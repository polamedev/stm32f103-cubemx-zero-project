#include <lame/mSec.h>

#include <st_hal.h>

LAME_mSec LAME_Get_mSec() {
    return HAL_GetTick();
}