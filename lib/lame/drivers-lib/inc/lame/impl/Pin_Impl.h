#pragma once

#include <lame/Pin.h>

#include <st_hal.h>

struct LAME_Pin_Impl {
    GPIO_TypeDef *GPIOx;
    uint16_t      GPIO_Pin;
};

LAME_Pin LAME_Pin_init(struct LAME_Pin_Impl *handle, GPIO_InitTypeDef *initStruct);
