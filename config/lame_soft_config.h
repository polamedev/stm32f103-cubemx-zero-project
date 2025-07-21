/**
 * @file lame_soft_config.h
 * @author polame (polame.dev@gmail.com)
 * @brief  Файл конфигурации библиотеки lame::soft
 * @version 0.1
 * @date 2025-07-21
 */

#pragma once

#include <lame/mSec.h>

/**
 * @brief  Функция получения времени в миллисекундах, определена в lame::drivers
 * @return 
 */
inline LAME_mSec LAME_SoftTimer_Get_mSec() {
    return LAME_Get_mSec();
}
