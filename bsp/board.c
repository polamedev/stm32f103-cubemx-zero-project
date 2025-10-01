#include "board.h"

#include <cubemx.h>
#include <st_hal.h>

#include <lame/Led.h>
#include <lame/impl/Pin_Impl.h>

#define pwm_tim         (&htim1)
#define pwm_tim_channel (TIM_CHANNEL_1)
extern TIM_HandleTypeDef htim1;

LAME_Led led;

static void clockInit();
static void pinInit();

void board_init()
{
    MX_Init();
    clockInit();
    pinInit();
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

void pinInit()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    {
        GPIO_InitStruct.Pin   = LED_Pin;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

        static struct LAME_Pin_Impl led_impl;
        led_impl.GPIOx    = LED_GPIO_Port;
        led_impl.GPIO_Pin = LED_Pin;
        LAME_Pin led_pin  = LAME_Pin_init(&led_impl, &GPIO_InitStruct);

        led = LAME_Led_Create(led_pin, false, 3);
    }

    // HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

void led_toggle()
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void nop()
{
    __NOP();
}

TIM_HandleTypeDef *board_get_pwm()
{
    return &htim1;
}

void Board_SetPwmActive(bool active)
{
    if (active) {
        HAL_TIM_PWM_Start(pwm_tim, pwm_tim_channel);
    }
    else {
        HAL_TIM_PWM_Stop(pwm_tim, pwm_tim_channel);
    }
}

static uint32_t mcsToTicks(uint32_t mcs)
{
    static const uint32_t HCLK_MHz      = 72;
    uint32_t              clockDivision = htim1.Init.ClockDivision + 1;
    uint32_t              prescaler     = htim1.Init.Prescaler + 1;

    uint32_t tick = mcs * (HCLK_MHz) / clockDivision / prescaler;
    return tick - 1;
}

void Board_SetPwmPeriod(int period_mcs)
{
    htim1.Init.Period = mcsToTicks(period_mcs);
    HAL_TIM_Base_Init(&htim1);
}

void Board_SetPwmFront(int front_mcs)
{
    // htim1.Init.AutoReloadPreload = mcsToTicks(front_mcs);
    // HAL_TIM_Base_Init(&htim1);
    htim1.Instance->CCR1 = mcsToTicks(front_mcs);
}

#define USER_FLASH_WRITE_ADDRESS 0x0801F800

bool Board_FlashWrite(const uint32_t *data, size_t dataSize)
{
    uint32_t               addr = USER_FLASH_WRITE_ADDRESS;
    HAL_StatusTypeDef      status;
    // uint32_t structureSize = sizeof(test_struct);
    uint32_t               structureSize = dataSize;
    FLASH_EraseInitTypeDef FlashErase;
    uint32_t               pageError = 0;

    __disable_irq();
    status = HAL_FLASH_Unlock();

    FlashErase.TypeErase   = FLASH_TYPEERASE_PAGES;
    FlashErase.PageAddress = addr;
    FlashErase.NbPages     = structureSize / 1024 + 1;

    if (HAL_FLASHEx_Erase(&FlashErase, &pageError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        __enable_irq();
        return false;
    }

    const uint32_t *dataPtr = data;

    for (uint32_t i = 0; i < structureSize / 4; i++)
    {
        status += HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, dataPtr[i]);
        addr   += 4;
    }
    __enable_irq();
    HAL_FLASH_Lock();
    return status == HAL_OK;
}

bool Board_FlashRead(uint32_t *data, size_t dataSize)
{
    uint32_t               addr = USER_FLASH_WRITE_ADDRESS;

    uint32_t structureSize = dataSize;
	uint32_t *dataPtr = data;
	for (uint32_t i = 0; i < structureSize / 4; i++)
	{
		dataPtr[i] = *(__IO uint32_t*)addr;
		addr += 4;
	}
    return true;
}

#define BOARD_GET_UNIQUE_ID 1

// Get USB Serial number string from unique ID if available. Return number of character.
// Input is string descriptor from index 1 (index 0 is type + len)
size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars)
{
    uint8_t uid[16];
    size_t  uid_len;

    // TODO work with make, but not working with esp32s3 cmake
    if (BOARD_GET_UNIQUE_ID) {
        uid_len = board_get_unique_id(uid, sizeof(uid));
    }
    else {
        // fixed serial string is 01234567889ABCDEF
        uint32_t *uid32 = (uint32_t *)(uintptr_t)uid;
        uid32[0]        = 0x67452301;
        uid32[1]        = 0xEFCDAB89;
        uid_len         = 8;
    }

    if (uid_len > max_chars / 2)
        uid_len = max_chars / 2;

    for (size_t i = 0; i < uid_len; i++) {
        for (size_t j = 0; j < 2; j++) {
            const char nibble_to_hex[16] = {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
            const uint8_t nibble       = (uid[i] >> (j * 4)) & 0xf;
            desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble]; // UTF-16-LE
        }
    }

    return 2 * uid_len;
}

size_t board_get_unique_id(uint8_t id[], size_t max_len)
{
    (void)max_len;
    volatile uint32_t *stm32_uuid = (volatile uint32_t *)UID_BASE;
    uint32_t          *id32       = (uint32_t *)(uintptr_t)id;
    const uint8_t      len        = 12;

    id32[0] = stm32_uuid[0];
    id32[1] = stm32_uuid[1];
    id32[2] = stm32_uuid[2];

    return len;
}
