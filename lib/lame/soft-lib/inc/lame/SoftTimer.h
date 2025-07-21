#pragma once

#include <lame/types.h>

#include <stdbool.h>

typedef enum {
    LAME_SoftTimer_ModePeriodic, // Режим периодического срабатывания
    LAME_SoftTimer_ModeElapsed,  // Режим срабатывания один раз
} LAME_SoftTimer_Mode;

typedef enum {
    LAME_SoftTimer_StateStop,            // Таймер остановлен
    LAME_SoftTimer_StateRun,             // Таймер запущен
    LAME_SoftTimer_StateElapsedComplete, // Таймер завершен в режим elapsed
} LAME_SoftTimer_State;

typedef struct LAME_SoftTimer_Impl {
    LAME_mSec            startTime; // Начальное время старта
    LAME_mSec            period;    // Период истечения таймера
    LAME_SoftTimer_Mode  mode;
    LAME_SoftTimer_State state;
} LAME_SoftTimer_Impl;

typedef LAME_SoftTimer_Impl *LAME_SoftTimer;

/**
 * @brief  Проверить срабатывание таймера
 * @param  timer: Дескриптор таймера
 * @return true - таймер сработал, false таймер не сработал или выключен
 */
bool LAME_SoftTimer_Occur(LAME_SoftTimer timer);

/**
 * @brief  Инициализация таймера
 * @param  timer:  Дескриптор таймера
 * @param  mode:   Режим работы таймера - LAME_SoftTimer_Mode
 * @param  period: Период срабатывания таймера, или таймаут для режима elapsed
 */
void LAME_SoftTimer_Init(LAME_SoftTimer timer, LAME_SoftTimer_Mode mode, LAME_mSec period);

/**
 * @brief  Получить режим таймера
 * @param  timer: Дескриптор таймера
 * @return Режим таймера
 */
LAME_SoftTimer_Mode LAME_SoftTimer_GetMode(const LAME_SoftTimer timer);

/**
 * @brief  Получить состояние таймера
 * @param  timer: Дескриптор таймера
 * @return Состояние таймера:
 *           - LAME_SoftTimer_StateStop - Таймер остановлен
 *           - LAME_SoftTimer_StateRun - Таймер запущен
 *           - LAME_SoftTimer_StateElapsedComplete - Таймер завершен в режим elapsed
 */
LAME_SoftTimer_State LAME_SoftTimer_GetState(const LAME_SoftTimer timer);

/**
 * @brief  Запустить таймер
 * @param  timer: Дескриптор таймера
 */
void LAME_SoftTimer_Start(LAME_SoftTimer timer);

/**
 * @brief  Остановить таймер
 * @param  timer: Дескриптор таймера
 */
void LAME_SoftTimer_Stop(LAME_SoftTimer timer);

/**
 * @brief  Установить период или таймаут таймера
 * @param  timer:  Дескриптор таймера
 * @param  period: Период или таймаут таймера
 */
void LAME_SoftTimer_SetPeriod(LAME_SoftTimer timer, LAME_mSec period);

/**
 * @brief  Получить период или таймаут таймера
 * @param  timer: Дескриптор таймера
 * @return Период или таймаут таймера
 */
LAME_mSec LAME_SoftTimer_GetPeriod(const LAME_SoftTimer timer);

