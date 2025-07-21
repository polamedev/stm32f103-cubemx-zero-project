#include <Event.h>

void LAME_event_init(LAME_Event *event)
{
    LAME_event_clear(event);
}

bool LAME_event_take(LAME_Event *event)
{
    if (*event) {
        *event = false;
        return true;
    }
    else {
        return false;
    }
}

bool LAME_event_check(const LAME_Event *event)
{
    return *event;
}
void LAME_event_set(LAME_Event *event)
{
    *event = true;
}

void LAME_event_clear(LAME_Event *event)
{
    *event = false;
}

