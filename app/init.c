#include "init.h"
#include "board.h"

#include <tusb.h>

static void usbInit();

void init()
{
    board_init();
    usbInit();
}

static void usbInit()
{
    tusb_rhport_init_t dev_init = {
        .role  = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO,
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
}
