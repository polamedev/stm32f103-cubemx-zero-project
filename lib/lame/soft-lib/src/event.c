#include <event.h>

void LAME_event_init(LAME_event *event)
{
    LAME_event_clear(event);
}

bool LAME_event_take(LAME_event *event)
{
    if (*event) {
        *event = false;
        return true;
    }
    else {
        return false;
    }
}

bool LAME_event_check(const LAME_event *event)
{
    return *event;
}
void LAME_event_set(LAME_event *event)
{
    *event = true;
}

void LAME_event_clear(LAME_event *event)
{
    *event = false;
}

