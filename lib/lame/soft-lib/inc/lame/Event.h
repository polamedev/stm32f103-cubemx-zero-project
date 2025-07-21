#pragma once

#include <stdbool.h>

typedef bool LAME_Event_Impl;

typedef LAME_Event_Impl *LAME_Event;

void LAME_event_init(LAME_Event event);
bool LAME_event_take(LAME_Event event);
bool LAME_event_check(const LAME_Event event);
void LAME_event_set(LAME_Event event);
void LAME_event_clear(LAME_Event event);
