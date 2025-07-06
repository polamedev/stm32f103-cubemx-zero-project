#include <cubemx.h>
#include <stdbool.h>

extern bool speed_blink;

/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_Pin) {
        speed_blink = !speed_blink;
    }
}
