#include <cubemx.h>
#include <st_hal.h>

int main()
{
    MX_Init();

    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(300);
    }
}
