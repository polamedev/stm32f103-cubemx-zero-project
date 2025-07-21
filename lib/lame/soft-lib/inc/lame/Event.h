#pragma once

#include <stdbool.h>

typedef bool LAME_Event;

/**
 * @brief  Инициализация события
 * @param  event: дескриптор события
 */
void LAME_Event_Init(LAME_Event *event);

/**
 * @brief  Получить событие, событие сбросится, если оно произошло
 * @param  event: дескриптор события
 * @return true - событие произошло
 * @return false - событие не произошло
 */
bool LAME_Event_Take(LAME_Event *event);

/**
 * @brief  Проверить событие
 * @param  event: дескриптор события
 * @return true - событие произошло
 * @return false - событие не произошло
 */
bool LAME_Event_Check(const LAME_Event *event);

/**
 * @brief  Установить событие
 * @param  event: дескриптор события
 */
void LAME_Event_Set(LAME_Event *event);

/**
 * @brief  Сбросить событие
 * @param  event: дескриптор события
 */
void LAME_Event_Clear(LAME_Event *event);
