#include <Event.h>

void LAME_Event_Init(LAME_Event *event)
{
    LAME_Event_Clear(event);
}

bool LAME_Event_Take(LAME_Event *event)
{
    if (*event) {
        *event = false;
        return true;
    }
    else {
        return false;
    }
}

bool LAME_Event_Check(const LAME_Event *event)
{
    return *event;
}
void LAME_Event_Set(LAME_Event *event)
{
    *event = true;
}

void LAME_Event_Clear(LAME_Event *event)
{
    *event = false;
}

