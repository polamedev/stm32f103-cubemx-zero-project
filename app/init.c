#include "init.h"

#include <cubemx.h>
#include <tusb.h>

static void clockInit();
static void usbInit();

void init()
{
    MX_Init();
    clockInit();
    usbInit();
}

static void clockInit()
{
    /* Настройка тактирования usb */
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection     = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection        = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /* Включение тактированяи usb */
    __HAL_RCC_USB_CLK_ENABLE();

    /**
     *  Вывод usb для f103 не нужно ставить а альтернативную функцию 
     * это делается само при включении тактирования usb
     */
}

static void usbInit()
{
    tusb_rhport_init_t dev_init = {
        .role  = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO,
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
}
