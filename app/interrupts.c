#include <cubemx.h>

#include <lame/event.h>

#include <stdbool.h>

extern LAME_event key_event;

/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_Pin) {
        LAME_event_set(&key_event);
    }
}
