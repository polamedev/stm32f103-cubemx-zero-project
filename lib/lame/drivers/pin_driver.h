#pragma once

#include "pin.h"

#include <st_hal.h>

typedef struct LAME_Pin {
    GPIO_TypeDef         *GPIOx;
    uint16_t              GPIO_Pin;
} LAME_Pin;

void LAME_Pin_init(LAME_Pin *handle, GPIO_InitTypeDef *initStruct);
