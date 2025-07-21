#include <lame/impl/Pin_Impl.h>
#include <Pin.h>

static void enablePortClock(GPIO_TypeDef *GPIOx)
{

    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
}

LAME_Pin LAME_Pin_init(struct LAME_Pin_Impl *handle, GPIO_InitTypeDef *initStruct)
{
    enablePortClock(handle->GPIOx);

    HAL_GPIO_Init(handle->GPIOx, initStruct);

    return handle;
}

void LAME_Pin_SetActive(LAME_Pin handle, bool state)
{
    if (state) {
        HAL_GPIO_WritePin(handle->GPIOx, handle->GPIO_Pin, GPIO_PIN_SET);
    }
    else {
        HAL_GPIO_WritePin(handle->GPIOx, handle->GPIO_Pin, GPIO_PIN_RESET);
    }
}

bool LAME_Pin_GetActive(const LAME_Pin handle)
{
    return HAL_GPIO_ReadPin(handle->GPIOx, handle->GPIO_Pin) == GPIO_PIN_SET;
}

void LAME_Pin_Toggle(LAME_Pin handle)
{
    HAL_GPIO_TogglePin(handle->GPIOx, handle->GPIO_Pin);
}
