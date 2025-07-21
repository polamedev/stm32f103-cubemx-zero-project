#pragma once

#include <lame/Pin.h>

#include <stdbool.h>

typedef struct LAME_Led_Impl *LAME_Led;

/**
 * @brief  Создать диод
 * @param  pin: Дескриптор пина, должен быть всегда действителен
 * @param  activeLow: Активное положение - низкое
 * @param  blinkCount: Число миганий
 * @return LAME_Led - Дескриптор светодиода
 */
LAME_Led LAME_Led_Create(LAME_Pin pin, bool activeLow, unsigned blinkCount);

/**
 * @brief  Задача управления светодиодами
 */
void LAME_Led_Task();

/**
 * @brief  Установить количество миганий
 * @param  led: Дескриптор светодиода
 * @param  blinkCount: Количество миганий
 */
void LAME_Led_SetBlinkCount(LAME_Led led, unsigned blinkCount);

// TODO реализовать прямое управление диодом
void LAME_Led_SetActive(LAME_Led led, bool activeLow);
bool LAME_Led_GetActive(const LAME_Led led);