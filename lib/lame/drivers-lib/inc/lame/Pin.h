#pragma once

#include <stdbool.h>

// typedef enum LAME_Pin_Mode {
//     LAME_Pin_Mode_PushPull,
//     LAME_Pin_Mode_OpenDrain,
//     LAME_Pin_Mode_Input,
//     LAME_Pin_Mode_ItInput,
// } LAME_Pin_Mode;

// typedef enum LAME_Pin_Mode {
//     LAME_Pin_Pull_None,
//     LAME_Pin_Pull_Up,
//     LAME_Pin_Pull_Down,
// } LAME_Pin_Pull;

typedef enum LAME_Pin_State {
    LAME_Pin_State_Hight,
    LAME_Pin_State_Low
} LAME_Pin_State;

typedef struct LAME_Pin_Impl *LAME_Pin;

void LAME_Pin_SetActive(LAME_Pin handle, bool state);
bool LAME_Pin_GetActive(const LAME_Pin handle);
void LAME_Pin_Toggle(LAME_Pin handle);

