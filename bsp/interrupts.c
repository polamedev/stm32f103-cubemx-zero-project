#include <cubemx.h>

#include <lame/Event.h>
#include <tusb.h>

#include <stdbool.h>

extern LAME_Event_Impl key_event;

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

//--------------------------------------------------------------------+
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------+
void USB_HP_IRQHandler(void)
{
    tud_int_handler(0);
}

void USB_LP_IRQHandler(void)
{
    tud_int_handler(0);
}

void USBWakeUp_IRQHandler(void)
{
    tud_int_handler(0);
}
