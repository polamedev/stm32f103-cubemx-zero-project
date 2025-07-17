#pragma once

#include <stdbool.h>

typedef bool LAME_event;

void LAME_event_init(LAME_event *event);
bool LAME_event_take(LAME_event *event);
bool LAME_event_check(const LAME_event *event);
void LAME_event_set(LAME_event *event);
void LAME_event_clear(LAME_event *event);
