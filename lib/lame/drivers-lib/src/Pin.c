#include <lame/impl/Pin_Impl.h>

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

void LAME_Pin_setActive(LAME_Pin handle, bool state)
{
    if (state) {
        HAL_GPIO_WritePin(handle->GPIOx, handle->GPIO_Pin, GPIO_PIN_SET);
    }
    else {
        HAL_GPIO_WritePin(handle->GPIOx, handle->GPIO_Pin, GPIO_PIN_RESET);
    }
}
